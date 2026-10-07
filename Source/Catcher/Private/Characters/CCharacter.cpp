// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CCharacter.h"
#include "AbilitySystem/CAbilitySystemComponent.h"
#include "AbilitySystem/CAbilitySystemNativeTags.h"
#include "AbilitySystem/CAttributeSet.h"
#include "Catcher/Catcher.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/OverheadStatusGauge.h"
#include "WorldPartition/HLOD/DestructibleHLODComponent.h"

// Sets default values
ACCharacter::ACCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>("AbilitySystemComponent");
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>("CAttributeSet");

	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Overhead Widget Component");
	OverheadWidgetComponent->SetupAttachment(GetRootComponent());
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_CAMERA_BOOM, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_CAMERA_BOOM, ECR_Ignore);
}

void ACCharacter::ServerSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->ApplyInitialEffects();
	AbilitySystemComponent->GiveInitialAbilities();
}

void ACCharacter::ClientSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

bool ACCharacter::bIsLocallyControllerByPlayer() const
{
	return IsLocallyControlled() && GetController()->IsPlayerController();
}

void ACCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController())
	{
		ServerSideInit();
	}
}

void ACCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCharacter, TeamId);
}

// Called when the game starts or when spawned
void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverheadWidgetComponent();
	BindGASDelegates();
}

// Called every frame
void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACCharacter::BindGASDelegates()
{
	if (bGASDelegateBound || !AbilitySystemComponent){return;}
	
	bGASDelegateBound = true;
	AbilitySystemComponent->RegisterGameplayTagEvent(TAG_STAT_DEAD).AddUObject(this, &ACCharacter::DeathTagUpdated);
}

void ACCharacter::DeathTagUpdated(const FGameplayTag Tag, int32 Count)
{
	if (Count != 0)
	{
		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
}

void ACCharacter::StartDeathSequence()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Death Sequence"));
	PlayDeathMontage();
	
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (APlayerController* PlayerController = GetController<APlayerController>())
	{
		DisableInput(PlayerController);
	}
	//SetActorHiddenInGame(true);
}

void ACCharacter::Respawn()
{
	UE_LOG(LogTemp, Warning, TEXT("Respawn"));
	if (DeathAnimMontage)
	{
		StopAnimMontage(DeathAnimMontage);
	}
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	if (APlayerController* PlayerController = GetController<APlayerController>())
	{
		EnableInput(PlayerController);
	}
	
	if (CAttributeSet)
	{
		CAttributeSet->SetHealth(CAttributeSet->GetMaxHealth());
	}
	
	RespawnFinished();
	//this->SetActorHiddenInGame(false);
}

bool ACCharacter::IsDead() const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(TAG_STAT_DEAD);
}

void ACCharacter::DeathAnimationFinished()
{
	if (IsDead())
	{
		SetRagdollEnabled(true);
	}
}

void ACCharacter::RespawnFinished()
{
	if (IsValid(GetController()))
	{
		if (HasAuthority() && GetController()->StartSpot.IsValid())
		{
			SetActorTransform(GetController()->StartSpot->GetActorTransform());
		}
	}
	
	SetRagdollEnabled(false);
}

void ACCharacter::SetRagdollEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		SkeletalMeshRelativeTransform = GetMesh()->GetRelativeTransform();
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(SkeletalMeshRelativeTransform);
	}
}

void ACCharacter::PlayDeathMontage()
{
	if (DeathAnimMontage)
	{
		float DeathAnimDuration = PlayAnimMontage(DeathAnimMontage);
		GetWorldTimerManager().SetTimer(DeathAnimationTimerHandle, this, &ACCharacter::DeathAnimationFinished, DeathAnimDuration + DeathAnimationTimeOffset);
	}
}

void ACCharacter::ConfigureOverheadWidgetComponent()
{
	if (!OverheadWidgetComponent)
	{
		return;
	}

	if (bIsLocallyControllerByPlayer())
	{
		OverheadWidgetComponent->SetHiddenInGame(true);
		return;
	}

	UOverheadStatusGauge* OverheadStatusGauge = Cast<UOverheadStatusGauge>(OverheadWidgetComponent->GetUserWidgetObject());
	if (OverheadStatusGauge)
	{
		OverheadStatusGauge->ConfigureWithAbilitySystemComponent(GetAbilitySystemComponent());
	}
	OverheadWidgetComponent->SetHiddenInGame(false);
}

void ACCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

FGenericTeamId ACCharacter::GetGenericTeamId() const
{
	return TeamId;
}


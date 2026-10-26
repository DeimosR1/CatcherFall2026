// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"

#include "AbilitySystem/CAbilitySystemNativeTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/StateTreeAIComponent.h"
#include "AbilitySystem/CAbilitySystemNativeTags.h"

ACAIController::ACAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");
	
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	
	SightConfig->SightRadius = 1000.0f;
	SightConfig->LoseSightRadius = 1200.0f;
	
	//How long before the AI forget you, not unsee you
	SightConfig->SetMaxAge(5.0f);
	
	SightConfig->PeripheralVisionAngleDegrees = 180.0f;
	
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	StateTreeComponent = CreateDefaultSubobject<UStateTreeAIComponent>("State Tree Component");
}

void ACAIController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	SetGenericTeamId(0);
	
	IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(NewPawn);
	if (PawnTeamInterface)
	{
		PawnTeamInterface->SetGenericTeamId(GetGenericTeamId());
	}
	
	UAbilitySystemComponent* PawnASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(NewPawn);
	if (PawnASC)
	{
		PawnASC->RegisterGameplayTagEvent(TAG_STAT_DEAD).AddUObject(this, &ACAIController::PawnDeadTagUpdated);
		PawnASC->RegisterGameplayTagEvent(TAG_STAT_STUN).AddUObject(this, &ACAIController::PawnStunTagUpdated);
	}
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		StateTreeComponent->Deactivate();
	}
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController::TargetPerceptionUpdated);
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ACAIController::TargetForgotten);
	
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent && GetPawn())
	{
		FVector PatrolCenterLocation = GetPawn()->GetActorLocation();
		BlackboardComponent->SetValueAsVector(BlackboardPatrolCenterKeyName, PatrolCenterLocation);
	}
}

void ACAIController::TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (!GetCurrentTarget())
		{
			SetCurrentTarget(TargetActor);
		}
	}
	else
	{
		ForgetActorIfDead(TargetActor);
	}
}

void ACAIController::TargetForgotten(AActor* ForgottenActor)
{
	if (!ForgottenActor){return;}
	if (GetCurrentTarget() == ForgottenActor)
	{
		SetCurrentTarget(GetNextPerceivedActor());
	}
	if (!GetCurrentTarget() && GetBlackboardComponent())
	{
		GetBlackboardComponent()->SetValueAsVector(BlackboardLastSeenLocationKeyName, ForgottenActor->GetActorLocation());
	}
}

const UObject* ACAIController::GetCurrentTarget() const
{
	if (const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		return BlackboardComponent->GetValueAsObject(BlackboardTargetKeyName);
	}
	return CurrentTargetActor;
}

void ACAIController::SetCurrentTarget(AActor* NewTarget)
{
	if (NewTarget != CurrentTargetActor)
	{
		CurrentTargetActor = NewTarget;
		StateTreeComponent->SendStateTreeEvent(TAG_PERCEPTION_TARGET_CHANGE);
	}
	
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (NewTarget)
		{
			BlackboardComponent->SetValueAsObject(BlackboardTargetKeyName, NewTarget);
		}
		else
		{
			BlackboardComponent->ClearValue(BlackboardTargetKeyName);
		}
	}
}

AActor* ACAIController::GetNextPerceivedActor() const
{
	if (PerceptionComponent)
	{
		TArray<AActor*> PerceivedActors;
		AIPerceptionComponent->GetPerceivedHostileActors(PerceivedActors);
		if (PerceivedActors.Num() != 0)
		{
			return PerceivedActors[0];
		}
	}
	return nullptr;
}

void ACAIController::ForgetActorIfDead(AActor* ActorToForget)
{
	UAbilitySystemComponent* ActorAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToForget);
	
	if (!ActorAbilitySystemComponent){return;}
	
	if (ActorAbilitySystemComponent->HasMatchingGameplayTag(TAG_STAT_DEAD))
	{
		for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator Iter = AIPerceptionComponent->GetPerceptualDataIterator(); Iter; ++Iter)
		{
			if (Iter->Key != ActorToForget) {continue;}
			
			for (FAIStimulus& Stimulus : Iter->Value.LastSensedStimuli)
			{
				Stimulus.SetStimulusAge(TNumericLimits<float>::Max());
			}
		}
	}
}

void ACAIController::ClearAndDisableAllSenses()
{
	AIPerceptionComponent->AgeStimuli(TNumericLimits<float>::Max());
	for (auto SenseConfigIt = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), false);
	}
	SetCurrentTarget(nullptr);
}

void ACAIController::EnableAllSenses()
{
	for (auto SenseConfigIt = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), true);
	}
}

void ACAIController::PawnDeadTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0)
	{
		GetBrainComponent()->StopLogic("Dead");
		ClearAndDisableAllSenses();
		bCachedIsPawnDead = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Revived"));
		GetBrainComponent()->StartLogic();
		EnableAllSenses();
		bCachedIsPawnDead = false;
	}
}

void ACAIController::PawnStunTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (bCachedIsPawnDead)
	{
		return;
	}
	if (NewCount != 0)
	{
		GetBrainComponent()->StopLogic("Stun");
	}
	else
	{
		GetBrainComponent()->StartLogic();
	}
}

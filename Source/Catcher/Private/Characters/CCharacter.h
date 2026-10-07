// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "CCharacter.generated.h"

UCLASS()
class ACCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACCharacter();

	void ServerSideInit();
	void ClientSideInit();

	bool bIsLocallyControllerByPlayer() const;

	virtual void PossessedBy(AController* NewController) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//-----------------------------------------------------------//
	//                    Gameplay Ability                       //
	//-----------------------------------------------------------//

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
private:
	void BindGASDelegates();
	
	void DeathTagUpdated(const FGameplayTag Tag, int32 Count);
	
	bool bGASDelegateBound;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Ability System")
	class UCAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UCAttributeSet* CAttributeSet;
	//-----------------------------------------------------------//
	//                   Death and Respawn                       //
	//-----------------------------------------------------------//

private:
	UFUNCTION()
	void StartDeathSequence();
	UFUNCTION()
	void Respawn();
	UFUNCTION()
	bool IsDead() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathAnimMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DeathAnimationTimeOffset = -0.5f;
	
	UFUNCTION()
	void PlayDeathMontage();
	FTimerHandle DeathAnimationTimerHandle;
	void DeathAnimationFinished();
	void RespawnFinished();
	
	void SetRagdollEnabled(bool bIsEnabled);
	
	UPROPERTY(EditDefaultsOnly, Category = "Ragdoll")
	FTransform SkeletalMeshRelativeTransform;
//-----------------------------------------------------------//
//                          Widget                           //
//-----------------------------------------------------------//
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "UI")
	class UWidgetComponent* OverheadWidgetComponent;

	void ConfigureOverheadWidgetComponent();
	
	//--------------------------------------------------------//
	//                          Team                         //
	//-------------------------------------------------------//
public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;
};

//By adding =0 at the end of a virtusl function, you are saying it is completely virtual. If you have at least of these, your class becomes an abstract class, which is the class that is incomplete.
//If you don't have an implementation of the function, the child class is also an abstract class
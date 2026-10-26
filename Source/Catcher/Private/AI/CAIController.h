// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "CAIController.generated.h"

struct FAIStimulus;
class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class ACAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ACAIController();
	virtual void OnPossess(APawn* NewPawn) override;
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	class UAIPerceptionComponent* AIPerceptionComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	UAISenseConfig_Sight* SightConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	class UBehaviorTree* BehaviorTree;
	
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	class UStateTreeAIComponent* StateTreeComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	FName BlackboardTargetKeyName = "Target";
	
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	FName BlackboardPatrolCenterKeyName = "PatrolCenter";
	
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	FName BlackboardLastSeenLocationKeyName = "LastSeenLocation";
	
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus);
	UFUNCTION()
	void TargetForgotten(AActor* ForgottenActor);
	
	const UObject* GetCurrentTarget() const;
	
	void SetCurrentTarget(AActor* NewTarget);
	
	AActor* GetNextPerceivedActor() const;
	
	void ForgetActorIfDead(AActor* ActorToForget);
	
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	AActor* CurrentTargetActor;
	
	void ClearAndDisableAllSenses();
	void EnableAllSenses();
	void PawnDeadTagUpdated(const FGameplayTag Tag, int32 NewCount);
	void PawnStunTagUpdated(const FGameplayTag Tag, int32 NewCount);
	
	bool bCachedIsPawnDead = false;
};

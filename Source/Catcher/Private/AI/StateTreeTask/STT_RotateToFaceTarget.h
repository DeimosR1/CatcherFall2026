// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AISystem.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Tasks/StateTreeAITask.h"
#include "Templates/SubclassOf.h"

#include "STT_RotateToFaceTarget.generated.h"

class AActor;
class AAIController;
class IGameplayTaskOwnerInterface;
class UAITask_MoveTo;

struct FAIMoveRequest;

USTRUCT()
struct FStateTreeRotateToTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	AAIController* AIController;
	
	UPROPERTY(EditAnywhere, Category = "Context")
	AActor* Target;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	float PrecisionDegrees = 5.0f;
	
	UPROPERTY(EditAnywhere)
	bool bShouldSuccessWhenFinished = true;
};


/**
* Task moving the given AIController's pawn using the AITask_MoveTo framework. Succeed when move ends at destinations, fails if move is impossible.
*/
USTRUCT(meta = (DisplayName = "Rotate To Face Target", Category = "AI|Action"))
struct FSTT_RotateToFaceTarget : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeRotateToTaskInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
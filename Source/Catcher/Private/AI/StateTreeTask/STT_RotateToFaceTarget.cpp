// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/StateTreeTask/STT_RotateToFaceTarget.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FSTT_RotateToFaceTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if (!InstanceData.AIController)
	{
		UE_LOG(LogTemp,Warning, TEXT("Can't find the AI Controller!"));
		return EStateTreeRunStatus::Failed;
	}
	
	if (!InstanceData.Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find the target for the target actor!"));
		return EStateTreeRunStatus::Failed;
	}
	
	InstanceData.AIController->SetFocus(InstanceData.Target);
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_RotateToFaceTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if (!InstanceData.AIController)
	{
		UE_LOG(LogTemp,Warning, TEXT("Can't find the AI Controller!"));
		return EStateTreeRunStatus::Failed;
	}
	const FVector PawnDir = InstanceData.AIController->GetPawn()->GetActorForwardVector();
	const FVector FocalPoint = InstanceData.AIController->GetFocalPointForPriority(EAIFocusPriority::Gameplay);
	
	if (FocalPoint != FAISystem::InvalidLocation)
	{
		const float AngleCos = PawnDir.CosineAngle2D(FocalPoint - InstanceData.AIController->GetPawn()->GetActorLocation());
		const float PrecisionCos = FMath::Cos(FMath::DegreesToRadians(InstanceData.PrecisionDegrees));
		
		if (AngleCos >= PrecisionCos && InstanceData.bShouldSuccessWhenFinished)
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}
	
	return EStateTreeRunStatus::Running;
}

void FSTT_RotateToFaceTarget::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if (!InstanceData.AIController)
	{
		UE_LOG(LogTemp,Warning, TEXT("Can't find the AI Controller!"));
		return;
	}
	
	InstanceData.AIController->ClearFocus(EAIFocusPriority::Gameplay);
}

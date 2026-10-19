// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/StateTreeTask/STT_PressAbilityInput.h"

#include "AbilitySystemComponent.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FSTT_PressAbilityInput::EnterState(FStateTreeExecutionContext& Context,
                                                       const FStateTreeTransitionResult& TransitionResult) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find the ability system component to press the input"))
		return EStateTreeRunStatus::Failed;
	}
	InstanceData.AbilitySystemComponent->PressInputID((int32)InstanceData.InputToPress);
	return EStateTreeRunStatus::Succeeded;
}

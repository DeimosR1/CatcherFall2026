// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Clear.h"

#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_Clear::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent && !BlackboardKey.IsNone())
	{
		BlackboardComponent->ClearValue(BlackboardKey.SelectedKeyName);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

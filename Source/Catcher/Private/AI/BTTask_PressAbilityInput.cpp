// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_PressAbilityInput.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_PressAbilityInput::UBTTask_PressAbilityInput(const FObjectInitializer&)
{
	BlackBoardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_PressAbilityInput, BlackBoardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_PressAbilityInput::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	
	if (AIOwner)
	{
		UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIOwner->GetPawn());
		if (OwnerASC)
		{
			OwnerASC->PressInputID((int32)InputToPress);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
	
	/*
	UE_LOG(LogTemp, Warning, TEXT("Task Executed"));
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	if (!MyBlackboard)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get Blackboard"));
		return EBTNodeResult::Failed;
	}
	
	
	UE_LOG(LogTemp, Warning, TEXT("Reached Action"));
		
	UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackBoardKey.GetSelectedKeyID());
	AActor* TargetActor = Cast<AActor>(KeyValue);
	UAbilitySystemComponent* ActorAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		
	if (!ActorAbilitySystemComponent){return EBTNodeResult::Failed;}
		
	UE_LOG(LogTemp, Warning, TEXT("Ability Component Exists"));
		
	ActorAbilitySystemComponent->PressInputID((int32)InputToPress);
		
	return Super::ExecuteTask(OwnerComp, NodeMemory);
	*/
	

}

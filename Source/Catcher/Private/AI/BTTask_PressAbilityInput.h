// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CAbilityInputID.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BTTask_PressAbilityInput.generated.h"

/**
 * 
 */
UCLASS()
class UBTTask_PressAbilityInput : public UBTTaskNode
{
	GENERATED_BODY()

	protected:
	UBTTask_PressAbilityInput(const FObjectInitializer&);
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere, Category= "Ability Input")
	ECAbilityInputID InputToPress;

private:
	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector BlackBoardKey;
	
};

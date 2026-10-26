// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "CGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class UCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	UCGameplayAbility();
	TArray<FHitResult> GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius=30.f, ETeamAttitude::Type TargetTeamAttitute = ETeamAttitude::Hostile, bool bDrawDebug=false, bool bIgnoreSelf=true) const;
	
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bShouldDrawDebug = false;
};

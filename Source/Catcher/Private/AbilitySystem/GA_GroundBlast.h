// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CGameplayAbility.h"
#include "GA_GroundBlast.generated.h"

/**
 * 
 */
UCLASS()
class UGA_GroundBlast : public UCGameplayAbility
{
	GENERATED_BODY()
	
protected:
	UGA_GroundBlast();
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="GameplayCue", meta=(Category="GameplayCue"))
	FGameplayTagContainer BlastingGameplayCueTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* CastMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ATA_GroundPick> TargetActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> DamageEffect;
	//UPROPERTY(EditDefaultsOnly, Category = "Effect")
	//TSubclassOf<UGameplayEffect> StunEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetTraceRange = 2000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetAreaRadius = 400.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float StunDuration = 3.0f;
	
	UFUNCTION()
	void TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	UFUNCTION()
	void TargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};


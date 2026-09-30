// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CAbilitySystemComponent.h"

#include "CAttributeSet.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &UCAbilitySystemComponent::HealthChanged);
}

void UCAbilitySystemComponent::HealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (!GetOwner()){return;}
	
	if (OnAttributeChangeData.NewValue <= 0 && GetOwner()->HasAuthority() && DeathEffect)
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingSpec(DeathEffect, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);
	}
}

void UCAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) { return; }

	for (const TSubclassOf<UGameplayEffect>& InitialEffectClass : InitialEffects)
	{
		FGameplayEffectSpecHandle EffecSpec = MakeOutgoingSpec(InitialEffectClass, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffecSpec.Data);
	}
}

void UCAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) { return; }

	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& InitialAbilityPair : InitialAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(InitialAbilityPair.Value, 1, (int32)InitialAbilityPair.Key));
	}
}

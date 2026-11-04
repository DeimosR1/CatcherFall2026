// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityListView.h"

#include "Abilities/GameplayAbility.h"

void UAbilityListView::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities)
{
	for (const TPair<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& AbilityPair : Abilities)
	{
		//We use the default object of the ability class to be the item added in.
		AddItem(AbilityPair.Value->GetDefaultObject());
	}
}

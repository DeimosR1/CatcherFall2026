// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityListView.h"

#include "AbilityGauge.h"
#include "Abilities/GameplayAbility.h"

void UAbilityListView::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities)
{
	for (const TPair<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& AbilityPair : Abilities)
	{
		//We use the default object of the ability class to be the item added in.
		AddItem(AbilityPair.Value->GetDefaultObject());
	}
}

void UAbilityListView::NativeOnEntryGenerated(UUserWidget* EntryWidget)
{
	UAbilityGauge* AbilityGauge = Cast<UAbilityGauge>(EntryWidget);
	if (AbilityGauge)
	{
		AbilityGauge->ConfigureWithWidgetData(FindWidgetDataForAbility(AbilityGauge->GetListItem()->GetClass()));
	}
}

const struct FAbilityWidgetData* UAbilityListView::FindWidgetDataForAbility(
	const TSubclassOf<class UGameplayAbility>& AbilityClass) const
{
	if (!AbilityDataTable){return nullptr;}
	
	for (auto& AbilityWidgetDataPair : AbilityDataTable->GetRowMap())
	{
		const FAbilityWidgetData* WidgetData = AbilityDataTable->FindRow<FAbilityWidgetData>(AbilityWidgetDataPair.Key, "");
		if (WidgetData->AbilityClass == AbilityClass){return WidgetData;}
	}
	return nullptr;
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CAbilityInputID.h"
#include "Components/ListView.h"
#include "AbilityListView.generated.h"

/**
 * 
 */
UCLASS()
class UAbilityListView : public UListView
{
	GENERATED_BODY()
	
public:
	void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities);
	virtual void NativeOnEntryGenerated(UUserWidget* EntryWidget) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* AbilityDataTable;
	
	const struct FAbilityWidgetData* FindWidgetDataForAbility(const TSubclassOf<class UGameplayAbility>& AbilityClass) const;
};

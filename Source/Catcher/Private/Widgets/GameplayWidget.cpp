// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayWidget.h"

#include "AbilityListView.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/CAbilitySystemComponent.h"
#include "Widgets/ValueGauge.h"
#include "AbilitySystem/CAttributeSet.h"

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UCAbilitySystemComponent* AbilitySystemComponent = Cast<UCAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn()));

	HealthBar->SetAndBindWithAbilitySystemComponent(AbilitySystemComponent, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
	Manabar->SetAndBindWithAbilitySystemComponent(AbilitySystemComponent, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());

	OwnerAbilitySystemComponent = AbilitySystemComponent;
}

void UGameplayWidget::InitializedWithAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities)
{
	AbilityListView->ConfigureAbilities(Abilities);
}

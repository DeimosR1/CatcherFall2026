// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityGauge.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	
	UAbilitySystemComponent* OwnerASC =  UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &UAbilityGauge::OnAbilityCommitted);
		OwnerASC->AbilitySpecDirtiedCallbacks.AddUObject(this, &UAbilityGauge::OnAbilitySpecUpdated);
	}
}

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	UE_LOG(LogTemp, Warning, TEXT("Ability Gauge Created for ability: %s"), *(ListItemObject->GetName()));
	CachedAbilityCD0 = Cast<UGameplayAbility>(ListItemObject);
	
	float CooldownDuration = GetStaticCooldownDuration();
	float Cost = GetStaticCost();
	
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
	CostText->SetText(FText::AsNumber(FMath::Abs(Cost)));
}

void UAbilityGauge::ConfigureWithWidgetData(const FAbilityWidgetData* AbilityWidgetData)
{
	if (Icon && AbilityWidgetData)
	{
		Icon->GetDynamicMaterial()->SetTextureParameterValue(IconTextureParamName, AbilityWidgetData->Icon.LoadSynchronous());
	}
}

float UAbilityGauge::GetStaticCooldownDuration() const
{
	if (!CachedAbilityCD0)
	{
		return 0.0f;
	}
	
	const UGameplayEffect* CooldownEffect = CachedAbilityCD0->GetCooldownGameplayEffect();
	if (!CooldownEffect)
	{
		return 0.0f;
	}
	
	float CooldownDuration = 0.0f;
	if (CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CooldownDuration))
	{
		return CooldownDuration;
	}
	return 0.0f;
}

float UAbilityGauge::GetStaticCost() const
{
	if (!CachedAbilityCD0){return 0.0f;}
	
	const UGameplayEffect* CostEffect = CachedAbilityCD0->GetCostGameplayEffect();
	if (!CostEffect || CostEffect->Modifiers.Num() == 0)
	{
		return 0.0f;
	}
	
	float Cost = 0.0f;
	if (CostEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost))
	{
		return Cost;
	}
	return 0.0f;
}

void UAbilityGauge::OnAbilityCommitted(UGameplayAbility* GameplayAbility)
{
	if (GameplayAbility->GetClass()->GetDefaultObject() == CachedAbilityCD0)
	{
		float CooldownTimeRemaining = 0.0f;
		float CooldownDuration = 0.0f;
		
		GameplayAbility->GetCooldownTimeRemainingAndDuration(GameplayAbility->GetCurrentAbilitySpecHandle(), 
			GameplayAbility->GetCurrentActorInfo(), CooldownTimeRemaining, CooldownDuration);
		
		StartCooldown(CooldownTimeRemaining, CooldownDuration);
	}
}

void UAbilityGauge::StartCooldown(float CooldownTimeRemaining, float CooldownDuration)
{
	CachedCooldownTimeRemaining = CooldownTimeRemaining;
	CachedCooldownDuration = CooldownDuration;
	CooldownCounterText->SetText(FText::AsNumber(CooldownDuration));
	
	CooldownCounterText->SetVisibility(ESlateVisibility::Visible);
	
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &UAbilityGauge::CooldownFinished, CooldownTimeRemaining);
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerUpdateHandle, this, &UAbilityGauge::UpdateCooldown, CooldownUpdateInterval, true, 0.0f);
}

void UAbilityGauge::UpdateCooldown()
{
	CachedCooldownTimeRemaining -= CooldownUpdateInterval;
	
	int NumberDigits = CachedCooldownTimeRemaining >= 1.0 ? 0 : 2;
	FNumberFormattingOptions FormattingOptions = FNumberFormattingOptions().SetMaximumFractionalDigits(NumberDigits);
	CooldownCounterText->SetText(FText::AsNumber(CachedCooldownTimeRemaining, &FormattingOptions));
	if (Icon)
	{
		Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName, 1- (CachedCooldownTimeRemaining/CachedCooldownDuration));
	}
}

void UAbilityGauge::CooldownFinished()
{
	CachedCooldownDuration = CachedCooldownTimeRemaining = 0.0f;
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerUpdateHandle);
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
}

void UAbilityGauge::OnAbilitySpecUpdated(const FGameplayAbilitySpec& GameplayAbilitySpec)
{
	//react to spec changes like level.
}

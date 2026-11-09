// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "AbilityGauge.generated.h"

enum class ECAbilityInputID : uint8;
/**
 * 
 */

USTRUCT(BlueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<class UGameplayAbility> AbilityClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName AbilityName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon;
};

UCLASS()
class UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	void ConfigureWithWidgetData(const FAbilityWidgetData* AbilityWidgetData);
	
	float GetStaticCooldownDuration() const;
	float GetStaticCost() const;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	FName IconTextureParamName = "Icon";
	UPROPERTY(meta=(BindWidget))
	class UImage* Icon;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownDurationText;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CostText;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownCounterText;
	
	UPROPERTY()
	const class UGameplayAbility* CachedAbilityCD0;
	
	void OnAbilityCommitted(UGameplayAbility* GameplayAbility);
	
	void StartCooldown(float CooldownTimeRemaining, float CooldownDuration);
	void UpdateCooldown();
	void CooldownFinished();
	
	FTimerHandle CooldownTimerHandle;
	FTimerHandle CooldownTimerUpdateHandle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownUpdateInterval = 0.1f;
	
	float CachedCooldownDuration;
	float CachedCooldownTimeRemaining;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName CooldownPercentParamName="Percent";
	
	void OnAbilitySpecUpdated(const FGameplayAbilitySpec& GameplayAbilitySpec);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CAbilityInputID.generated.h"

UENUM(BlueprintType)
enum class ECAbilityInputID : uint8
{
	None							UMETA(DisplayName = "None"),
	BasicAttack						UMETA(DisplayName = "BasicAttack"),
	AbilityOne						UMETA(DisplayName = "AbilityOne"),
	AbilityTwo						UMETA(DisplayName = "AbilityTwo"),
	AbilityThree					UMETA(DisplayName = "AbilityThree"),
	AbilityFour						UMETA(DisplayName = "AbilityFour"),
	
	Confirm							UMETA(DisplayName = "Confirm"),
	Cancel							UMETA(DisplayName = "Cancel"),
};
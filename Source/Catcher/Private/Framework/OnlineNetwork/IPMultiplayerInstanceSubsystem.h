// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "IPMultiplayerInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UIPMultiplayerInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	FName LevelName;
	UPROPERTY(EditDefaultsOnly)
	FString IPAddress;
	
	UFUNCTION()
	void CreateServerAsListening();
	UFUNCTION()
	void JoinServerAsClient();
};

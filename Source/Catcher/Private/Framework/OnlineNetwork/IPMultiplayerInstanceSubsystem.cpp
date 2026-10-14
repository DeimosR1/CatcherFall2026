// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/OnlineNetwork/IPMultiplayerInstanceSubsystem.h"

#include "Kismet/GameplayStatics.h"

void UIPMultiplayerInstanceSubsystem::CreateServerAsListening()
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelName, true, "listen");
}

void UIPMultiplayerInstanceSubsystem::JoinServerAsClient()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->ClientTravel(IPAddress, ETravelType::TRAVEL_Absolute);
}

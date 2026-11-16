// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameSession.h"
#include "Framework/CGameInstance.h"

bool ACGameSession::ProcessAutoLogin()
{
	return true;
}

void ACGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
	if (UCGameInstance* CGameInstance = GetGameInstance<UCGameInstance>())
	{
		CGameInstance->PlayerJoined(UniqueId);
	}
}

void ACGameSession::UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId)
{
	Super::UnregisterPlayer(InSessionName, UniqueId);
	if (UCGameInstance* CGameInstance = GetGameInstance<UCGameInstance>())
	{
		CGameInstance->PlayerLeft(UniqueId);
	}
}

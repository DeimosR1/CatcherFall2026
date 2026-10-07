// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameMode.h"

#include "EngineUtils.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerStart.h"

APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewController = Super::SpawnPlayerController(InRemoteRole, Options);
	IGenericTeamAgentInterface* ControllerTeamInterface = Cast<IGenericTeamAgentInterface>(NewController);
	FGenericTeamId NewTeamId = GetTeamIdForPlayer(NewController);
	if (ControllerTeamInterface)
	{
		ControllerTeamInterface->SetGenericTeamId(NewTeamId);
	}
	NewController->StartSpot = FindNextStartSpotForTeam(NewTeamId);
	return NewController;
}

FGenericTeamId ACGameMode::GetTeamIdForPlayer(const APlayerController* PlayerController)
{
	static int PlayerCount = 0;
	PlayerCount++;
	return FGenericTeamId(PlayerCount%2);
}

AActor* ACGameMode::FindNextStartSpotForTeam(const FGenericTeamId& TeamId)
{
	const FName* StartSpotTag = TeamPlayerStartTagMap.Find(TeamId);
	if (!StartSpotTag)
	{
		return nullptr;
	}
	
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			if (It->PlayerStartTag ==  *StartSpotTag)
			{
				It->PlayerStartTag = FName("Taken");
				return *It;
			}
		}
	}
	return nullptr;
}

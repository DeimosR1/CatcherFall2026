// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerController.h"

#include "Net/UnrealNetwork.h"
#include "Player/CPlayerCharacter.h"
#include "Widgets/GameplayWidget.h"

void ACPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPlayerController, TeamId);
}

void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	
	if (IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(NewPawn))
	{
		PawnTeamInterface->SetGenericTeamId(TeamId);
	}
	CPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (CPlayerCharacter) 
	{
		CPlayerCharacter->ServerSideInit();
		//CPlayerCharacter->SetGenericTeamId(GetGenericTeamId());
	}
}

void ACPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	CPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ClientSideInit();
		SpawnGameplayWidget();
	}
}

void ACPlayerController::SpawnGameplayWidget()
{
	if (IsLocalPlayerController()) 
	{ 
		if (GameplayWidgetClass)
		{
			GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
			GameplayWidget->AddToViewport();
		}
	}
}

void ACPlayerController::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	TeamId = TeamID;
}

FGenericTeamId ACPlayerController::GetGenericTeamId() const
{
	return TeamId;
}

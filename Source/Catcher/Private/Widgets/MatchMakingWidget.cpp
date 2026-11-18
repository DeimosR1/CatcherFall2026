// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MatchMakingWidget.h"

#include "Components/Button.h"
#include "Framework/CGameInstance.h"
#include "MainMenu/MainMenuWidget.h"

void UMatchMakingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GameInstance = GetGameInstance<UCGameInstance>();
	CreateNewSessionButton->OnClicked.AddDynamic(this, &UMatchMakingWidget::CreateNewSessionButtonClicked);
	FindAndJoinSessionButton->OnClicked.AddDynamic(this, &UMatchMakingWidget::FindAndJoinSessionButtonClicked);
}

FOnButtonClickedEvent& UMatchMakingWidget::GetFindAndJoinSessionButtonClickedEvent()
{
	return FindAndJoinSessionButton->OnClicked;
}

FOnButtonClickedEvent& UMatchMakingWidget::GetCreateNewSessionButtonClickedEvent()
{
	return CreateNewSessionButton->OnClicked;
}

void UMatchMakingWidget::CreateNewSessionButtonClicked()
{
	if (GameInstance)
	{
		GameInstance->RequestCreateNewSession();
	}
}

void UMatchMakingWidget::FindAndJoinSessionButtonClicked()
{
	if (GameInstance)
	{
		GameInstance->TryFindAndJoinSession();
	}
}

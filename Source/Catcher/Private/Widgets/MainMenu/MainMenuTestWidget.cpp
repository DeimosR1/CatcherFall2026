// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenu/MainMenuTestWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Framework/OnlineNetwork/IPMultiplayerInstanceSubsystem.h"

void UMainMenuTestWidget::NativeConstruct()
{
	Super::NativeConstruct();
	JoinServerButton->OnClicked.AddDynamic(this, &UMainMenuTestWidget::OnJoinServerButtonClicked);
	CreateServerButton->OnClicked.AddDynamic(this, &UMainMenuTestWidget::OnCreateServerButtonClicked);
}

void UMainMenuTestWidget::OnJoinServerButtonClicked()
{
	if (!GetIPMultiplayerInstanceSubsystem()){return;}
	GetIPMultiplayerInstanceSubsystem()->IPAddress = IPTextBox->GetText().ToString();
	GetIPMultiplayerInstanceSubsystem()->JoinServerAsClient();
}

void UMainMenuTestWidget::OnCreateServerButtonClicked()
{
	if (!GetIPMultiplayerInstanceSubsystem()){return;}
	GetIPMultiplayerInstanceSubsystem()->LevelName = GetLevelName();
}

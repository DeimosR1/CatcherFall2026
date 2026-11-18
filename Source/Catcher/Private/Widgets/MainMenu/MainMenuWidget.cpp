// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenu/MainMenuWidget.h"
#include "Framework/CGameInstance.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Widgets/MatchMakingWidget.h"
#include "Widgets/WaitingWidget.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CGameInstance = GetGameInstance<UCGameInstance>();
	if (CGameInstance)
	{
		//...
		CGameInstance->OnAuthLoginCompletedDelegate.AddUObject(this, &UMainMenuWidget::LoginCompleted);
	}
	
	LoginButton->OnClicked.AddDynamic(this, &UMainMenuWidget::LoginButtonClicked);
	MatchMakingWidget->GetFindAndJoinSessionButtonClickedEvent().AddDynamic(this, &UMainMenuWidget::WaitForFindAndJoinSession);
	MatchMakingWidget->GetCreateNewSessionButtonClickedEvent().AddDynamic(this, &UMainMenuWidget::WaitForNewSessionCreate);
}

FReply UMainMenuWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Focus Added to Path"));
	UWidget* ActiveWidget = MainSwitcher->GetActiveWidget();
	if (ActiveWidget)
	{
		ActiveWidget->SetKeyboardFocus();
		return FReply::Handled();
	}
	return Super::NativeOnFocusReceived(InGeometry,InFocusEvent);
}

void UMainMenuWidget::SwitchToMatchMakingWidget()
{
	MainSwitcher->SetActiveWidget(MatchMakingWidget);
	MatchMakingWidget->SetKeyboardFocus();
}

void UMainMenuWidget::LoginButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Login Button Clicked"));
	if (CGameInstance)
	{
		if (CGameInstance->IsLoggedIn() || CGameInstance->IsLoggingIn())
		{
			UE_LOG(LogTemp, Warning, TEXT("Log in not allowed at this time, done or trying."));
			SwitchToMatchMakingWidget();
			return;
		}
		CGameInstance->ClientAuthPortalLogin();
		MainSwitcher->SetActiveWidget(WaitingWidget);
		WaitingWidget->ConfigureWaiting("Logging In", false);
	}
	
}

void UMainMenuWidget::LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMsg)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login succeeded"));
		SwitchToMatchMakingWidget();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Login failed. Error:"));
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
	}
}

void UMainMenuWidget::WaitForNewSessionCreate()
{
	MainSwitcher->SetActiveWidget(WaitingWidget);
	WaitingWidget->ConfigureWaiting("Creating Game", true);
	WaitingWidget->ClearAndGetButtonClickedEvent().AddDynamic(this, &UMainMenuWidget::SwitchToMatchMakingWidget);
	WaitingWidget->SetKeyboardFocus();
}

void UMainMenuWidget::WaitForFindAndJoinSession()
{
	MainSwitcher->SetActiveWidget(WaitingWidget);
	WaitingWidget->ConfigureWaiting("Finding Game", true);
	WaitingWidget->ClearAndGetButtonClickedEvent().AddDynamic(this, &UMainMenuWidget::SwitchToMatchMakingWidget);
	WaitingWidget->SetKeyboardFocus();
}

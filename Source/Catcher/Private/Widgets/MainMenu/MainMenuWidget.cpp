// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenu/MainMenuWidget.h"
#include "Framework/CGameInstance.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

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

void UMainMenuWidget::LoginButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Login Button Clicked"));
	if (CGameInstance)
	{
		CGameInstance->ClientAuthPortalLogin();
	}
}

void UMainMenuWidget::LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMsg)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login succeeded"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Login failed. Error:"));
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
	}
}

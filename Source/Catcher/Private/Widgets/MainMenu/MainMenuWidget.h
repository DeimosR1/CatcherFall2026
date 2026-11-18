// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UIPMultiplayerInstanceSubsystem;
/**
 * 
 */
UCLASS()
class UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	
private:
	//Changes between different children, hiding and showing them.
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* MainSwitcher;
	
	UPROPERTY()
	class UCGameInstance* CGameInstance;
	
	//LOGIN//
	
private:
	UPROPERTY(meta=(BindWidget))
	class UButton* LoginButton;
	
	UPROPERTY(meta=(BindWidget))
	class UMatchMakingWidget* MatchMakingWidget;
	
	UPROPERTY(meta=(BindWidget))
	class UWaitingWidget* WaitingWidget;
	
	UFUNCTION()
	void SwitchToMatchMakingWidget();
	
	UFUNCTION()
	void LoginButtonClicked();
	
	UFUNCTION()
	void LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMsg);
	
	UFUNCTION()
	void WaitForNewSessionCreate();
	UFUNCTION()
	void WaitForFindAndJoinSession();
};

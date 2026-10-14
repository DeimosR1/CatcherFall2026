// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuTestWidget.generated.h"

class UIPMultiplayerInstanceSubsystem;
/**
 * 
 */
UCLASS()
class UMainMenuTestWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* JoinServerButton;
	UPROPERTY(meta=(BindWidget))
	class UButton* CreateServerButton;
	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* IPTextBox;
	
	UPROPERTY(EditDefaultsOnly)
	UWorld* LevelToLoad;
	
	UFUNCTION()
	void OnJoinServerButtonClicked();
	UFUNCTION()
	void OnCreateServerButtonClicked();
	
	class UIPMultiplayerInstanceSubsystem* GetIPMultiplayerInstanceSubsystem() const {return /*nullptr;}*/ GetGameInstance()->GetSubsystem<UIPMultiplayerInstanceSubsystem>();}
	
	FName GetLevelName() {return FName(LevelToLoad->GetName());}
};

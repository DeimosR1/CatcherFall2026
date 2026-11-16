// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuController.generated.h"

/**
 * 
 */
UCLASS()
class AMainMenuController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void AcknowledgePossession(class APawn* NewPawn) override;
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> WidgetClass;
	
	UPROPERTY()
	class UUserWidget* MenuWidget;
	
};

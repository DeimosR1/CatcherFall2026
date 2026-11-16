// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuController.h"

#include "LevelEditorMenuContext.h"
#include "Blueprint/UserWidget.h"

void AMainMenuController::AcknowledgePossession(class APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	MenuWidget = CreateWidget(this, WidgetClass);
	if (MenuWidget)
	{
		MenuWidget->AddToViewport();
	}
}

void AMainMenuController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeUIOnly());
}

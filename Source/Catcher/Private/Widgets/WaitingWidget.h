

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "WaitingWidget.generated.h"

/**
 * 
 */
UCLASS()
class UWaitingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ConfigureWaiting(const FString& WaitingStr, bool bAllowCancel);
	FOnButtonClickedEvent& ClearAndGetButtonClickedEvent();
	
private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* WaitingInfoText;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* CancelButton;
	
};

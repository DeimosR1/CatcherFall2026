
#include "WaitingWidget.h"

#include "Components/TextBlock.h"

void UWaitingWidget::ConfigureWaiting(const FString& WaitingStr, bool bAllowCancel)
{
	WaitingInfoText->SetText(FText::FromString(WaitingStr));
	CancelButton->SetVisibility(bAllowCancel ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

FOnButtonClickedEvent& UWaitingWidget::ClearAndGetButtonClickedEvent()
{
	CancelButton->OnClicked.Clear();
	return CancelButton->OnClicked;
}

// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPMatchResultWidget.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"

void UPvPMatchResultWidget::InitFor(bool bInPlayerWon)
{
	bPlayerWon = bInPlayerWon;
}

void UPvPMatchResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	SetKeyboardFocus();
}

FReply UPvPMatchResultWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Enter)
	{
		OnPlayAgain.Broadcast();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

TSharedRef<SWidget> UPvPMatchResultWidget::RebuildWidget()
{
	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 48);
	const FSlateFontInfo PromptFont = FCoreStyle::GetDefaultFontStyle("Bold", 20);

	const FText TitleText = bPlayerWon
		? FText::FromString(TEXT("VICTORY"))
		: FText::FromString(TEXT("DEFEAT"));

	const FLinearColor TitleColor = bPlayerWon
		? FLinearColor(0.2f, 1.f, 0.3f)
		: FLinearColor(1.f, 0.2f, 0.2f);

	return SNew(SOverlay)
	+ SOverlay::Slot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
		.BorderBackgroundColor(FLinearColor(0.f, 0.f, 0.f, 0.8f))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 0, 0, 24)
			[
				SNew(STextBlock)
				.Text(TitleText)
				.Font(TitleFont)
				.ColorAndOpacity(TitleColor)
			]

			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Press ENTER to play again")))
				.Font(PromptFont)
				.ColorAndOpacity(FLinearColor::White)
			]
		]
	];
}

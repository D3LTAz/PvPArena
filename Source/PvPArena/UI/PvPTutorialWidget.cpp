// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPTutorialWidget.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"

void UPvPTutorialWidget::InitFor(bool bInStandalone)
{
	bStandalone = bInStandalone;
}

void UPvPTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	SetKeyboardFocus();
}

FReply UPvPTutorialWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Enter)
	{
		OnDismissed.Broadcast();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

TSharedRef<SWidget> UPvPTutorialWidget::RebuildWidget()
{
	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 32);
	const FSlateFontInfo BodyFont = FCoreStyle::GetDefaultFontStyle("Regular", 16);
	const FSlateFontInfo PromptFont = FCoreStyle::GetDefaultFontStyle("Bold", 20);

	return SNew(SOverlay)
	+ SOverlay::Slot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
		.BorderBackgroundColor(FLinearColor(0.f, 0.f, 0.f, 0.75f))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(700.f)
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 0, 0, 24)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("PvP Arena -- Practice Mode")))
					.Font(TitleFont)
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 0, 0, 8)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("You're facing one AI-controlled bot in a small ring.")))
					.Font(BodyFont)
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 0, 0, 24)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("It won't engage for a few seconds after the match starts.")))
					.Font(BodyFont)
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 4)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("WASD -- Move")))
					.Font(BodyFont)
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 4)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Mouse -- Look")))
					.Font(BodyFont)
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 4)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Space -- Jump")))
					.Font(BodyFont)
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 4)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("FIRE BUILD 4 — press F to shoot (LMB too)")))
					.Font(BodyFont)
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 4)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Q -- Switch Weapon (Pistol / Rifle / Sniper)")))
					.Font(BodyFont)
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 4, 0, 32)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("V -- Toggle 1st / 3rd Person Camera")))
					.Font(BodyFont)
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(bStandalone ? TEXT("Press ENTER to return to the menu") : TEXT("Press ENTER to begin")))
					.Font(PromptFont)
					.ColorAndOpacity(FLinearColor(1.f, 0.85f, 0.1f))
				]
			]
		]
	];
}

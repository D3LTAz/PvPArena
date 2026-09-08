// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPAIMatchSetupWidget.h"
#include "PvPAIDifficultyProfile.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Styling/CoreStyle.h"

void UPvPAIMatchSetupWidget::InitFor(const TArray<UPvPAIDifficultyProfile*>& InAvailableProfiles)
{
	AvailableProfiles.Reset();
	for (UPvPAIDifficultyProfile* Profile : InAvailableProfiles)
	{
		if (Profile)
		{
			AvailableProfiles.Add(Profile);
		}
	}
}

FReply UPvPAIMatchSetupWidget::HandleProfileClicked(UPvPAIDifficultyProfile* Profile)
{
	OnDifficultySelected.Broadcast(Profile);
	return FReply::Handled();
}

FReply UPvPAIMatchSetupWidget::HandleBackClicked()
{
	OnBack.Broadcast();
	return FReply::Handled();
}

TSharedRef<SWidget> UPvPAIMatchSetupWidget::RebuildWidget()
{
	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 32);
	const FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Bold", 18);

	TSharedRef<SVerticalBox> ButtonList = SNew(SVerticalBox);

	for (UPvPAIDifficultyProfile* Profile : AvailableProfiles)
	{
		if (!Profile)
		{
			continue;
		}

		ButtonList->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(0, 8)
		[
			SNew(SBox)
			.WidthOverride(320.f)
			.HeightOverride(56.f)
			[
				SNew(SButton)
				.OnClicked(FOnClicked::CreateUObject(this, &UPvPAIMatchSetupWidget::HandleProfileClicked, Profile))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(Profile->DifficultyName)
					.Font(ButtonFont)
					.ColorAndOpacity(FLinearColor::White)
				]
			]
		];
	}

	ButtonList->AddSlot()
	.AutoHeight()
	.HAlign(HAlign_Center)
	.Padding(0, 24, 0, 0)
	[
		SNew(SBox)
		.WidthOverride(320.f)
		.HeightOverride(48.f)
		[
			SNew(SButton)
			.OnClicked(FOnClicked::CreateUObject(this, &UPvPAIMatchSetupWidget::HandleBackClicked))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Back")))
				.Font(ButtonFont)
				.ColorAndOpacity(FLinearColor::White)
			]
		]
	];

	return SNew(SOverlay)
	+ SOverlay::Slot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
		.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.03f, 1.f))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 0, 0, 32)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Select Difficulty")))
				.Font(TitleFont)
				.ColorAndOpacity(FLinearColor::White)
			]

			+ SVerticalBox::Slot().AutoHeight()
			[
				ButtonList
			]
		]
	];
}

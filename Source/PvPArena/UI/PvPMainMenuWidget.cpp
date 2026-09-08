// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPMainMenuWidget.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Styling/CoreStyle.h"

namespace PvPMenuUI
{
	// Palette lifted from the approved title-screen reference (PvP Arena
	// Title Screen.pdf): near-black background, white/orange title,
	// muted-gray secondary text.
	static const FLinearColor BackgroundColor(0.035f, 0.035f, 0.04f, 1.f);
	static const FLinearColor OrangeAccent(0.95f, 0.58f, 0.12f, 1.f);
	static const FLinearColor MutedGray(0.5f, 0.5f, 0.52f, 1.f);
	static const FLinearColor RowLineColor(0.16f, 0.16f, 0.18f, 1.f);

	static TSharedRef<SWidget> MakeSeparatorLine(float WidthPx, float HeightPx, const FLinearColor& Color)
	{
		return SNew(SBox)
			.WidthOverride(WidthPx)
			.HeightOverride(HeightPx)
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
				.BorderBackgroundColor(Color)
			];
	}

	// One numbered menu row: "01   TUTORIAL   >". Disabled rows (bEnabled =
	// false, no OnClicked bound) render muted and non-interactive -- used for
	// "1V1", which isn't built in this repo (see class comment).
	static TSharedRef<SWidget> MakeMenuRow(const TCHAR* Number, const TCHAR* Label, bool bEnabled, const FSlateFontInfo& NumberFont, const FSlateFontInfo& LabelFont, FOnClicked OnClicked)
	{
		const FLinearColor LabelColor = bEnabled ? FLinearColor::White : MutedGray;

		TSharedRef<SWidget> RowContent = SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 20, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Number))
				.Font(NumberFont)
				.ColorAndOpacity(MutedGray)
			]
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Label))
				.Font(LabelFont)
				.ColorAndOpacity(LabelColor)
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(bEnabled ? TEXT(">") : TEXT("")))
				.Font(LabelFont)
				.ColorAndOpacity(OrangeAccent)
			];

		TSharedRef<SWidget> Row = bEnabled
			? StaticCastSharedRef<SWidget>(
				SNew(SButton)
				.ButtonStyle(FCoreStyle::Get(), "NoBorder")
				.ContentPadding(FMargin(4.f, 14.f))
				.OnClicked(OnClicked)
				[
					RowContent
				])
			: StaticCastSharedRef<SWidget>(
				SNew(SBox)
				.Padding(FMargin(4.f, 14.f))
				[
					RowContent
				]);

		return SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			[
				Row
			]
			+ SVerticalBox::Slot().AutoHeight()
			[
				MakeSeparatorLine(560.f, 1.f, RowLineColor)
			];
	}
}

FReply UPvPMainMenuWidget::HandleTutorialClicked()
{
	OnTutorialRequested.Broadcast();
	return FReply::Handled();
}

FReply UPvPMainMenuWidget::HandleOneVOneClicked()
{
	OnOneVOneRequested.Broadcast();
	return FReply::Handled();
}

FReply UPvPMainMenuWidget::HandlePlayClicked()
{
	OnPlayRequested.Broadcast();
	return FReply::Handled();
}

FReply UPvPMainMenuWidget::HandleSettingsClicked()
{
	OnSettingsRequested.Broadcast();
	return FReply::Handled();
}

FReply UPvPMainMenuWidget::HandleQuitClicked()
{
	OnQuitRequested.Broadcast();
	return FReply::Handled();
}

TSharedRef<SWidget> UPvPMainMenuWidget::RebuildWidget()
{
	using namespace PvPMenuUI;

	const FSlateFontInfo EyebrowFont = FCoreStyle::GetDefaultFontStyle("Bold", 12);
	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 64);
	const FSlateFontInfo SubtitleFont = FCoreStyle::GetDefaultFontStyle("Regular", 14);
	const FSlateFontInfo RowNumberFont = FCoreStyle::GetDefaultFontStyle("Bold", 14);
	const FSlateFontInfo RowLabelFont = FCoreStyle::GetDefaultFontStyle("Bold", 20);
	const FSlateFontInfo FooterFont = FCoreStyle::GetDefaultFontStyle("Regular", 12);

	return SNew(SOverlay)

	// --- Background ---
	+ SOverlay::Slot()
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
		.BorderBackgroundColor(BackgroundColor)
	]

	// --- Main content column (left-aligned, per the reference) ---
	+ SOverlay::Slot()
	.HAlign(HAlign_Left)
	.VAlign(VAlign_Center)
	.Padding(90.f, 0.f, 0.f, 0.f)
	[
		SNew(SBox)
		.WidthOverride(600.f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 10)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("MILESTONE 2 -- DEMO BUILD")))
				.Font(EyebrowFont)
				.ColorAndOpacity(OrangeAccent)
			]

			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("PVP")))
				.Font(TitleFont)
				.ColorAndOpacity(FLinearColor::White)
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 20)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("ARENA")))
				.Font(TitleFont)
				.ColorAndOpacity(OrangeAccent)
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 16)
			[
				MakeSeparatorLine(56.f, 3.f, OrangeAccent)
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 36)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("TACTICAL PVP COMBAT -- 1V1 -- 2V2 -- 3V3")))
				.Font(SubtitleFont)
				.ColorAndOpacity(MutedGray)
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8)
			[
				MakeSeparatorLine(560.f, 1.f, RowLineColor)
			]

			+ SVerticalBox::Slot().AutoHeight()
			[
				MakeMenuRow(TEXT("01"), TEXT("TUTORIAL"), true, RowNumberFont, RowLabelFont,
					FOnClicked::CreateUObject(this, &UPvPMainMenuWidget::HandleTutorialClicked))
			]

			+ SVerticalBox::Slot().AutoHeight()
			[
				MakeMenuRow(TEXT("02"), TEXT("1V1 (COMING SOON)"), false, RowNumberFont, RowLabelFont,
					FOnClicked::CreateUObject(this, &UPvPMainMenuWidget::HandleOneVOneClicked))
			]

			+ SVerticalBox::Slot().AutoHeight()
			[
				MakeMenuRow(TEXT("03"), TEXT("1V1 VS AI"), true, RowNumberFont, RowLabelFont,
					FOnClicked::CreateUObject(this, &UPvPMainMenuWidget::HandlePlayClicked))
			]

			+ SVerticalBox::Slot().AutoHeight()
			[
				MakeMenuRow(TEXT("04"), TEXT("SETTINGS"), true, RowNumberFont, RowLabelFont,
					FOnClicked::CreateUObject(this, &UPvPMainMenuWidget::HandleSettingsClicked))
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 24, 0, 0)
			[
				SNew(SBox).WidthOverride(200.f).HeightOverride(40.f)
				[
					SNew(SButton)
					.OnClicked(FOnClicked::CreateUObject(this, &UPvPMainMenuWidget::HandleQuitClicked))
					.HAlign(HAlign_Left).VAlign(VAlign_Center)
					.ButtonStyle(FCoreStyle::Get(), "NoBorder")
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Quit")))
						.Font(SubtitleFont)
						.ColorAndOpacity(MutedGray)
					]
				]
			]
		]
	]

	// --- Bottom-right build tag ---
	+ SOverlay::Slot()
	.HAlign(HAlign_Right)
	.VAlign(VAlign_Bottom)
	.Padding(0.f, 0.f, 32.f, 24.f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("PC -- BUILD 0.2.0-DEMO")))
		.Font(FooterFont)
		.ColorAndOpacity(MutedGray)
	]

	// --- Bottom accent bar (simplified stand-in for the reference's hazard stripe) ---
	+ SOverlay::Slot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Bottom)
	[
		SNew(SBox)
		.HeightOverride(6.f)
		[
			SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor(OrangeAccent)
		]
	];
}

// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPHUDWidget.h"
#include "PvPCharacter.h"
#include "PvPHealthComponent.h"
#include "PvPWeaponComponent.h"
#include "PvPWeaponData.h"
#include "PvPDeathmatchGameState.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"

void UPvPHUDWidget::InitFor(APvPCharacter* InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;
}

void UPvPHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(false);
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

FText UPvPHUDWidget::GetHealthText() const
{
	if (const APvPCharacter* Character = OwnerCharacter.Get())
	{
		if (const UPvPHealthComponent* Health = Character->GetHealthComponent())
		{
			return FText::FromString(FString::Printf(TEXT("HP: %.0f / %.0f"), Health->CurrentHealth, Health->MaxHealth));
		}
	}
	return FText::FromString(TEXT("HP: --"));
}

FText UPvPHUDWidget::GetWeaponText() const
{
	if (const APvPCharacter* Character = OwnerCharacter.Get())
	{
		if (const UPvPWeaponComponent* Weapon = Character->GetWeaponComponent())
		{
			if (Weapon->WeaponData)
			{
				return Weapon->WeaponData->WeaponName;
			}
		}
	}
	return FText::FromString(TEXT("No Weapon"));
}

FText UPvPHUDWidget::GetAmmoText() const
{
	if (const APvPCharacter* Character = OwnerCharacter.Get())
	{
		if (const UPvPWeaponComponent* Weapon = Character->GetWeaponComponent())
		{
			const int32 MaxAmmo = Weapon->WeaponData ? Weapon->WeaponData->MaxAmmo : 0;
			return FText::FromString(FString::Printf(TEXT("Ammo: %d / %d   [BUILD4 F=fire]"), Weapon->CurrentAmmo, MaxAmmo));
		}
	}
	return FText::FromString(TEXT("Ammo: --"));
}

FText UPvPHUDWidget::GetScoreText() const
{
	if (const APvPCharacter* Character = OwnerCharacter.Get())
	{
		if (const APvPDeathmatchGameState* GS = Character->GetWorld() ? Character->GetWorld()->GetGameState<APvPDeathmatchGameState>() : nullptr)
		{
			const int32 PlayerScore = GS->TeamScores.IsValidIndex(0) ? GS->TeamScores[0] : 0;
			const int32 BotScore = GS->TeamScores.IsValidIndex(1) ? GS->TeamScores[1] : 0;
			return FText::FromString(FString::Printf(TEXT("You: %d   Bot: %d"), PlayerScore, BotScore));
		}
	}
	return FText::FromString(TEXT("You: 0   Bot: 0"));
}

TSharedRef<SWidget> UPvPHUDWidget::RebuildWidget()
{
	const FSlateFontInfo StatFont = FCoreStyle::GetDefaultFontStyle("Bold", 18);
	const FSlateFontInfo ScoreFont = FCoreStyle::GetDefaultFontStyle("Bold", 24);

	return SNew(SOverlay)
	// Pure display, no interactive elements -- must NOT intercept mouse
	// input, or clicks (e.g. Left Click to fire) get swallowed by this
	// full-screen overlay before they ever reach the game/Enhanced Input.
	.Visibility(EVisibility::HitTestInvisible)

	// Bottom-left: health / weapon / ammo
	+ SOverlay::Slot()
	.HAlign(HAlign_Left)
	.VAlign(VAlign_Bottom)
	.Padding(24.f)
	[
		SNew(SBox)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[
				SNew(STextBlock)
				.Text_UObject(this, &UPvPHUDWidget::GetHealthText)
				.Font(StatFont)
				.ColorAndOpacity(FLinearColor(0.2f, 1.f, 0.3f))
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[
				SNew(STextBlock)
				.Text_UObject(this, &UPvPHUDWidget::GetWeaponText)
				.Font(StatFont)
				.ColorAndOpacity(FLinearColor::White)
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[
				SNew(STextBlock)
				.Text_UObject(this, &UPvPHUDWidget::GetAmmoText)
				.Font(StatFont)
				.ColorAndOpacity(FLinearColor::White)
			]
		]
	]

	// Top-center: score
	+ SOverlay::Slot()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Top)
	.Padding(0.f, 24.f)
	[
		SNew(STextBlock)
		.Text_UObject(this, &UPvPHUDWidget::GetScoreText)
		.Font(ScoreFont)
		.ColorAndOpacity(FLinearColor(1.f, 0.85f, 0.1f))
	];
}

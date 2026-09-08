// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPSettingsWidget.h"
#include "PvPSettingsSaveGame.h"
#include "PvPPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "AudioDevice.h"
#include "GameFramework/GameUserSettings.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SCheckBox.h"
#include "Styling/CoreStyle.h"

void UPvPSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResolutionPresets = {
		FIntPoint(1280, 720),
		FIntPoint(1920, 1080),
		FIntPoint(2560, 1440),
	};

	LoadCurrentSettings();
}

void UPvPSettingsWidget::LoadCurrentSettings()
{
	if (UPvPSettingsSaveGame* Save = Cast<UPvPSettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(UPvPSettingsSaveGame::SlotName, 0)))
	{
		PendingSensitivity = Save->MouseSensitivity;
		PendingMasterVolume = Save->MasterVolume;
		bPendingInvertY = Save->bInvertY;
	}

	if (UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr)
	{
		PendingWindowMode = UserSettings->GetFullscreenMode();

		const FIntPoint CurrentRes = UserSettings->GetScreenResolution();
		ResolutionIndex = ResolutionPresets.IndexOfByPredicate([&CurrentRes](const FIntPoint& P) { return P == CurrentRes; });
		if (ResolutionIndex == INDEX_NONE)
		{
			ResolutionIndex = 1; // default to 1920x1080 if the current res isn't one of our presets
		}
	}
}

void UPvPSettingsWidget::SaveAndApplySettings()
{
	if (UPvPSettingsSaveGame* Save = Cast<UPvPSettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(UPvPSettingsSaveGame::StaticClass())))
	{
		Save->MouseSensitivity = PendingSensitivity;
		Save->MasterVolume = PendingMasterVolume;
		Save->bInvertY = bPendingInvertY;
		UGameplayStatics::SaveGameToSlot(Save, UPvPSettingsSaveGame::SlotName, 0);
	}

	if (GEngine)
	{
		if (FAudioDeviceHandle AudioDevice = GEngine->GetMainAudioDevice())
		{
			AudioDevice->SetTransientPrimaryVolume(PendingMasterVolume);
		}
	}

	if (APvPPlayerController* PC = Cast<APvPPlayerController>(GetOwningPlayer()))
	{
		PC->MouseSensitivity = PendingSensitivity;
		PC->bInvertY = bPendingInvertY;
	}
}

void UPvPSettingsWidget::HandleSensitivityChanged(float NewValue)
{
	PendingSensitivity = FMath::Clamp(NewValue, 0.1f, 3.f);
	SaveAndApplySettings();
}

void UPvPSettingsWidget::HandleMasterVolumeChanged(float NewValue)
{
	PendingMasterVolume = FMath::Clamp(NewValue, 0.f, 1.f);
	SaveAndApplySettings();
}

void UPvPSettingsWidget::HandleInvertYChanged(ECheckBoxState NewState)
{
	bPendingInvertY = (NewState == ECheckBoxState::Checked);
	SaveAndApplySettings();
}

FReply UPvPSettingsWidget::HandleWindowModeClicked()
{
	switch (PendingWindowMode)
	{
	case EWindowMode::Windowed:           PendingWindowMode = EWindowMode::WindowedFullscreen; break;
	case EWindowMode::WindowedFullscreen: PendingWindowMode = EWindowMode::Fullscreen; break;
	default:                              PendingWindowMode = EWindowMode::Windowed; break;
	}

	if (UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr)
	{
		UserSettings->SetFullscreenMode(PendingWindowMode);
		UserSettings->ApplySettings(false);
	}

	return FReply::Handled();
}

FReply UPvPSettingsWidget::HandleResolutionClicked()
{
	ResolutionIndex = (ResolutionIndex + 1) % ResolutionPresets.Num();

	if (UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr)
	{
		UserSettings->SetScreenResolution(ResolutionPresets[ResolutionIndex]);
		UserSettings->ApplySettings(false);
	}

	return FReply::Handled();
}

FReply UPvPSettingsWidget::HandleBackClicked()
{
	OnBack.Broadcast();
	return FReply::Handled();
}

FText UPvPSettingsWidget::GetSensitivityText() const
{
	return FText::FromString(FString::Printf(TEXT("Mouse Sensitivity: %.2f"), PendingSensitivity));
}

FText UPvPSettingsWidget::GetMasterVolumeText() const
{
	return FText::FromString(FString::Printf(TEXT("Master Volume: %.0f%%"), PendingMasterVolume * 100.f));
}

FText UPvPSettingsWidget::GetWindowModeText() const
{
	switch (PendingWindowMode)
	{
	case EWindowMode::Windowed:           return FText::FromString(TEXT("Window Mode: Windowed"));
	case EWindowMode::WindowedFullscreen: return FText::FromString(TEXT("Window Mode: Windowed Fullscreen"));
	default:                              return FText::FromString(TEXT("Window Mode: Fullscreen"));
	}
}

FText UPvPSettingsWidget::GetResolutionText() const
{
	const FIntPoint& Res = ResolutionPresets.IsValidIndex(ResolutionIndex) ? ResolutionPresets[ResolutionIndex] : FIntPoint(1920, 1080);
	return FText::FromString(FString::Printf(TEXT("Resolution: %dx%d"), Res.X, Res.Y));
}

TSharedRef<SWidget> UPvPSettingsWidget::RebuildWidget()
{
	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 32);
	const FSlateFontInfo LabelFont = FCoreStyle::GetDefaultFontStyle("Bold", 16);

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
			SNew(SBox)
			.WidthOverride(420.f)
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 0, 0, 32)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Settings")))
					.Font(TitleFont)
					.ColorAndOpacity(FLinearColor::White)
				]

				// --- Mouse sensitivity ---
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 8)
				[
					SNew(STextBlock)
					.Text_UObject(this, &UPvPSettingsWidget::GetSensitivityText)
					.Font(LabelFont)
					.ColorAndOpacity(FLinearColor::White)
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 16)
				[
					SNew(SSlider)
					.MinValue(0.1f)
					.MaxValue(3.f)
					.Value_UObject(this, &UPvPSettingsWidget::GetSensitivityValue)
					.OnValueChanged_UObject(this, &UPvPSettingsWidget::HandleSensitivityChanged)
				]

				// --- Master volume ---
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 8)
				[
					SNew(STextBlock)
					.Text_UObject(this, &UPvPSettingsWidget::GetMasterVolumeText)
					.Font(LabelFont)
					.ColorAndOpacity(FLinearColor::White)
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 16)
				[
					SNew(SSlider)
					.MinValue(0.f)
					.MaxValue(1.f)
					.Value_UObject(this, &UPvPSettingsWidget::GetMasterVolumeValue)
					.OnValueChanged_UObject(this, &UPvPSettingsWidget::HandleMasterVolumeChanged)
				]

				// --- Invert Y ---
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 16)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 8, 0)
					[
						SNew(SCheckBox)
						.IsChecked_UObject(this, &UPvPSettingsWidget::GetInvertYState)
						.OnCheckStateChanged_UObject(this, &UPvPSettingsWidget::HandleInvertYChanged)
					]
					+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Invert Y")))
						.Font(LabelFont)
						.ColorAndOpacity(FLinearColor::White)
					]
				]

				// --- Window mode / resolution ---
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 8)
				[
					SNew(SBox).WidthOverride(320.f).HeightOverride(44.f)
					[
						SNew(SButton)
						.OnClicked(FOnClicked::CreateUObject(this, &UPvPSettingsWidget::HandleWindowModeClicked))
						.HAlign(HAlign_Center).VAlign(VAlign_Center)
						[
							SNew(STextBlock).Text_UObject(this, &UPvPSettingsWidget::GetWindowModeText).Font(LabelFont).ColorAndOpacity(FLinearColor::White)
						]
					]
				]
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 8, 0, 24)
				[
					SNew(SBox).WidthOverride(320.f).HeightOverride(44.f)
					[
						SNew(SButton)
						.OnClicked(FOnClicked::CreateUObject(this, &UPvPSettingsWidget::HandleResolutionClicked))
						.HAlign(HAlign_Center).VAlign(VAlign_Center)
						[
							SNew(STextBlock).Text_UObject(this, &UPvPSettingsWidget::GetResolutionText).Font(LabelFont).ColorAndOpacity(FLinearColor::White)
						]
					]
				]

				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
				[
					SNew(SBox).WidthOverride(320.f).HeightOverride(48.f)
					[
						SNew(SButton)
						.OnClicked(FOnClicked::CreateUObject(this, &UPvPSettingsWidget::HandleBackClicked))
						.HAlign(HAlign_Center).VAlign(VAlign_Center)
						[
							SNew(STextBlock).Text(FText::FromString(TEXT("Back"))).Font(LabelFont).ColorAndOpacity(FLinearColor::White)
						]
					]
				]
			]
		]
	];
}

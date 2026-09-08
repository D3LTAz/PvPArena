// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PvPSettingsWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPvPOnSettingsBackSignature);

/**
 * Settings, scoped to exactly four things: mouse sensitivity, master
 * volume, invert-Y, window mode/resolution. Sensitivity/volume/invert-Y
 * persist via UPvPSettingsSaveGame; window mode/resolution go through
 * UGameUserSettings directly. Changes apply live as each control moves,
 * not just on Back.
 */
UCLASS()
class UPvPSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPvPOnSettingsBackSignature OnBack;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;

private:
	float PendingSensitivity = 1.f;
	float PendingMasterVolume = 1.f;
	bool bPendingInvertY = false;
	int32 ResolutionIndex = 0;
	TEnumAsByte<EWindowMode::Type> PendingWindowMode = EWindowMode::Windowed;

	TArray<FIntPoint> ResolutionPresets;

	void LoadCurrentSettings();
	void SaveAndApplySettings();

	FReply HandleBackClicked();
	FReply HandleWindowModeClicked();
	FReply HandleResolutionClicked();

	void HandleSensitivityChanged(float NewValue);
	void HandleMasterVolumeChanged(float NewValue);
	void HandleInvertYChanged(ECheckBoxState NewState);

	float GetSensitivityValue() const { return PendingSensitivity; }
	float GetMasterVolumeValue() const { return PendingMasterVolume; }
	ECheckBoxState GetInvertYState() const { return bPendingInvertY ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; }

	FText GetSensitivityText() const;
	FText GetMasterVolumeText() const;
	FText GetWindowModeText() const;
	FText GetResolutionText() const;
};

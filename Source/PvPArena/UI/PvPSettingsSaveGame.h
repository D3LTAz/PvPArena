// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PvPSettingsSaveGame.generated.h"

/**
 * The settings UGameUserSettings doesn't cover: mouse sensitivity, master
 * volume, invert-Y. Window mode/resolution go through UGameUserSettings
 * directly instead (its own persistence, no need to duplicate here).
 */
UCLASS()
class UPvPSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	static const FString SlotName;

	UPROPERTY()
	float MouseSensitivity = 1.f;

	UPROPERTY()
	float MasterVolume = 1.f;

	UPROPERTY()
	bool bInvertY = false;
};

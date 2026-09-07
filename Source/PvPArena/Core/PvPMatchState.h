// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PvPMatchState.generated.h"

/**
 * Match flow state, driven by APvPDeathmatchGameMode (server-only) and
 * replicated to clients via APvPDeathmatchGameState::CurrentMatchState.
 * AGameModeBase (our base class, per the Stage 1 scaffold) has no built-in
 * match-state machine -- that only exists on the fuller AGameMode -- so this
 * is a minimal purpose-built replacement.
 */
UENUM(BlueprintType)
enum class EPvPMatchState : uint8
{
	WaitingToStart,
	InProgress,
	Ending
};

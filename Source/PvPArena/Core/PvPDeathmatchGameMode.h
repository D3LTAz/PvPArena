// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PvPMatchState.h"
#include "PvPDeathmatchGameMode.generated.h"

/**
 *  Server-only. Owns match flow (waiting-to-start -> in-progress -> ending).
 *  AGameModeBase has no built-in match-state machine (unlike the fuller
 *  AGameMode), so SetMatchState() below is a minimal purpose-built
 *  replacement that writes through to the replicated GameState. No team
 *  assignment or spawn logic yet -- that lands in Phase E once
 *  formats/team sizes are wired up.
 */
UCLASS(abstract)
class APvPDeathmatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	/** Constructor */
	APvPDeathmatchGameMode();

	virtual void BeginPlay() override;

protected:

	/** Server-only. Updates CurrentMatchState on APvPDeathmatchGameState. */
	void SetMatchState(EPvPMatchState NewState);
};

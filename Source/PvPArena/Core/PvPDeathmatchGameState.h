// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Matchmaking/PvPMatchFormat.h"
#include "PvPMatchState.h"
#include "PvPDeathmatchGameState.generated.h"

/**
 * Replicated match state: per-team scores, match clock, and the format
 * (1v1/2v2/3v3) selected during matchmaking. Deathmatch is always 2 teams
 * regardless of format, so TeamScores is fixed at size 2. Read by the HUD on
 * all clients -- AGameModeBase never replicates, so client-visible state
 * belongs here, not on the GameMode.
 */
UCLASS()
class APvPDeathmatchGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	APvPDeathmatchGameState();

	/** Score for each of the 2 teams. Index 0 = Team A, Index 1 = Team B. */
	UPROPERTY(ReplicatedUsing = OnRep_TeamScores, BlueprintReadOnly, Category = "Match")
	TArray<int32> TeamScores;

	/** Seconds remaining in the current match. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Match")
	float MatchTimeRemaining = 0.f;

	/** Format selected during matchmaking (1v1/2v2/3v3). */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Match")
	EPvPMatchFormat CurrentMatchFormat = EPvPMatchFormat::OneVOne;

	/** Waiting-to-start / in-progress / ending. Set by APvPDeathmatchGameMode. */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentMatchState, BlueprintReadOnly, Category = "Match")
	EPvPMatchState CurrentMatchState = EPvPMatchState::WaitingToStart;

	/** Adds Amount to the given team's score (0 or 1). Server-only. */
	void AddTeamScore(int32 TeamIndex, int32 Amount = 1);

	UFUNCTION()
	void OnRep_TeamScores();

	UFUNCTION()
	void OnRep_CurrentMatchState();

	//~ Begin AActor interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End AActor interface
};

// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPDeathmatchGameState.h"
#include "Net/UnrealNetwork.h"

APvPDeathmatchGameState::APvPDeathmatchGameState()
{
	TeamScores.Init(0, 2);
}

void APvPDeathmatchGameState::AddTeamScore(int32 TeamIndex, int32 Amount)
{
	if (!HasAuthority() || !TeamScores.IsValidIndex(TeamIndex))
	{
		return;
	}

	TeamScores[TeamIndex] += Amount;
	OnRep_TeamScores();
}

void APvPDeathmatchGameState::OnRep_TeamScores()
{
	// Placeholder hook for HUD score updates.
}

void APvPDeathmatchGameState::OnRep_CurrentMatchState()
{
	// Placeholder hook for HUD match-state updates.
}

void APvPDeathmatchGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APvPDeathmatchGameState, TeamScores);
	DOREPLIFETIME(APvPDeathmatchGameState, MatchTimeRemaining);
	DOREPLIFETIME(APvPDeathmatchGameState, CurrentMatchFormat);
	DOREPLIFETIME(APvPDeathmatchGameState, CurrentMatchState);
}

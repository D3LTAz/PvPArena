// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPDeathmatchGameMode.h"
#include "PvPDeathmatchGameState.h"
#include "PvPPlayerState.h"
#include "PvPArena.h"

APvPDeathmatchGameMode::APvPDeathmatchGameMode()
{
	GameStateClass = APvPDeathmatchGameState::StaticClass();
	PlayerStateClass = APvPPlayerState::StaticClass();
}

void APvPDeathmatchGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetMatchState(EPvPMatchState::WaitingToStart);
}

void APvPDeathmatchGameMode::SetMatchState(EPvPMatchState NewState)
{
	if (APvPDeathmatchGameState* PvPGameState = GetGameState<APvPDeathmatchGameState>())
	{
		PvPGameState->CurrentMatchState = NewState;
		PvPGameState->OnRep_CurrentMatchState();

		UE_LOG(LogPvPArena, Log, TEXT("Match state -> %s"), *UEnum::GetValueAsString(NewState));
	}
}

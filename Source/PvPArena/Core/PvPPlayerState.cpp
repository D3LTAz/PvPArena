// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPPlayerState.h"
#include "Net/UnrealNetwork.h"

APvPPlayerState::APvPPlayerState()
{
}

void APvPPlayerState::AddKill()
{
	if (HasAuthority())
	{
		++Kills;
	}
}

void APvPPlayerState::AddDeath()
{
	if (HasAuthority())
	{
		++Deaths;
	}
}

void APvPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APvPPlayerState, Kills);
	DOREPLIFETIME(APvPPlayerState, Deaths);
	DOREPLIFETIME(APvPPlayerState, TeamIndex);
}

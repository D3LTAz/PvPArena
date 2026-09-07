// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PvPPlayerState.generated.h"

/** Replicated per-player stats: kills, deaths, team index. */
UCLASS()
class APvPPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	APvPPlayerState();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	int32 Kills = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Stats")
	int32 Deaths = 0;

	/** 0 or 1 for M1's 2-team deathmatch. -1 = unassigned. */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Team")
	int32 TeamIndex = -1;

	/** Server-only. */
	void AddKill();

	/** Server-only. */
	void AddDeath();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

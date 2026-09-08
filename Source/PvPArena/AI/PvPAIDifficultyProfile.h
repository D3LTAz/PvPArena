// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PvPAIDifficultyProfile.generated.h"

/**
 * Data-driven AI difficulty: every number APvPAIController tunes its
 * behavior with, in one Blueprint-authorable asset. Selecting a difficulty
 * in the pre-match setup screen swaps which profile gets applied to the
 * spawned bot -- no branching in the AI/weapon/replication code itself.
 */
UCLASS(BlueprintType)
class UPvPAIDifficultyProfile : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty")
	FText DifficultyName;

	/** Max distance at which the bot will fire on its target. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty")
	float EngageRange = 2000.f;

	/** The bot stops closing the distance once within this range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty")
	float PreferredStandoffRange = 800.f;

	/** Seconds after acquiring a target before the bot will fire. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty")
	float EngageDelayAfterTargetAcquired = 2.5f;

	/** Minimum seconds between fire attempts, independent of weapon fire rate. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty")
	float FireAttemptCooldown = 2.f;

	/** Chance [0-1] that a fire attempt is skipped (simulated miss/hesitation). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Difficulty", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MissChance = 0.5f;
};

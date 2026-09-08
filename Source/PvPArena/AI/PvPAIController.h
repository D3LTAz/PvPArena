// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PvPAIController.generated.h"

class UPvPAIDifficultyProfile;

/**
 * Minimal practice-bot AI: no Behavior Tree/EQS assets, just a direct
 * approach-and-fire Tick loop against one assigned target. Deliberately
 * simple -- this exists to give the practice-vs-AI ring an opponent, not to
 * be a general-purpose combat AI system.
 *
 * Difficulty is entirely data: ApplyDifficultyProfile() copies every tunable
 * below from a UPvPAIDifficultyProfile asset. The Tick loop itself never
 * branches on difficulty -- it just reads whatever these fields currently
 * hold, whether that's the class defaults or an applied profile.
 */
UCLASS()
class APvPAIController : public AAIController
{
	GENERATED_BODY()

public:
	APvPAIController();

	/** Called by the GameMode right after spawning/possessing the bot. */
	void SetTargetPawn(APawn* InTargetPawn);

	/** Copies every tunable below from Profile. Safe to call with nullptr (no-op). */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void ApplyDifficultyProfile(const UPvPAIDifficultyProfile* Profile);

	virtual void Tick(float DeltaSeconds) override;

	/** Max distance at which the bot will fire on its target. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float EngageRange = 2000.f;

	/** The bot stops closing the distance once within this range. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float PreferredStandoffRange = 800.f;

	/** Seconds after SetTargetPawn() before the bot will fire -- gives a
	 * freshly (re)spawned player a moment to get oriented instead of being
	 * shot on the frame the bot appears. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float EngageDelayAfterTargetAcquired = 2.5f;

	/** Minimum seconds between fire *attempts*, independent of the equipped
	 * weapon's own fire rate -- the main difficulty lever. Weapon fire rate
	 * still applies on top of this (whichever is slower wins). */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float FireAttemptCooldown = 2.0f;

	/** Chance [0-1] that a fire attempt is skipped entirely (simulated miss/
	 * hesitation), on top of FireAttemptCooldown. 0 = never skips. */
	UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MissChance = 0.5f;

private:
	TWeakObjectPtr<APawn> TargetPawn;

	/** World time (seconds) at which SetTargetPawn() was called. */
	double TargetAcquiredTimeSeconds = 0.0;

	/** World time (seconds) of the last fire attempt (hit or missed roll). */
	double LastFireAttemptTimeSeconds = -1000.0;
};

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PvPAIController.generated.h"

/**
 * Minimal practice-bot AI: no Behavior Tree/EQS assets, just a direct
 * approach-and-fire Tick loop against one assigned target. Deliberately
 * simple -- this exists to give the practice-vs-AI ring an opponent, not to
 * be a general-purpose combat AI system.
 */
UCLASS()
class APvPAIController : public AAIController
{
	GENERATED_BODY()

public:
	APvPAIController();

	/** Called by the GameMode right after spawning/possessing the bot. */
	void SetTargetPawn(APawn* InTargetPawn);

	virtual void Tick(float DeltaSeconds) override;

	/** Max distance at which the bot will fire on its target. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float EngageRange = 2000.f;

	/** The bot stops closing the distance once within this range. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float PreferredStandoffRange = 800.f;

private:
	TWeakObjectPtr<APawn> TargetPawn;
};

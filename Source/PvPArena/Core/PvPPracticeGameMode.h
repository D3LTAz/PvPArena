// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PvPDeathmatchGameMode.h"
#include "PvPPracticeGameMode.generated.h"

class APvPAIController;

/**
 * Single-player practice mode: one human player vs. one AI-controlled bot.
 * Concrete (not abstract) and sets its own DefaultPawnClass/
 * PlayerControllerClass in the constructor via ConstructorHelpers, pointing
 * at the existing BP_ThirdPersonCharacter/BP_ThirdPersonPlayerController --
 * so it's usable directly as GlobalDefaultGameMode with no new Blueprint
 * authoring required.
 *
 * The bot does NOT spawn automatically on a timer -- BeginPlay only sets
 * WaitingToStart. The match (and the bot) only actually starts once
 * BeginMatch() is called, which APvPPlayerController does when the player
 * dismisses the tutorial/start overlay (Enter). This keeps the player from
 * getting shot at while they're still reading the instructions.
 */
UCLASS()
class APvPPracticeGameMode : public APvPDeathmatchGameMode
{
	GENERATED_BODY()

public:
	APvPPracticeGameMode();

	/** Transitions to InProgress and spawns the bot. Called by APvPPlayerController on tutorial dismissal. */
	UFUNCTION(BlueprintCallable, Category = "Practice")
	void BeginMatch();

protected:
	virtual void BeginPlay() override;

	/** Pawn class used for the AI bot. Defaults to the same class as the human player. */
	UPROPERTY(EditDefaultsOnly, Category = "Practice")
	TSubclassOf<APawn> BotPawnClass;

	UPROPERTY(EditDefaultsOnly, Category = "Practice")
	TSubclassOf<APvPAIController> BotControllerClass;

	/** Distance in front of the player to spawn the bot. Note: the practice
	 * ring (Lvl_PracticeRing) has a ~1200-unit wall radius, so this must stay
	 * well under that or the bot spawns outside/inside the ring wall -- the
	 * actual "don't insta-kill the player on spawn" fix is
	 * APvPAIController::EngageDelayAfterTargetAcquired, not spawn distance. */
	UPROPERTY(EditDefaultsOnly, Category = "Practice")
	float BotSpawnDistance = 900.f;

private:
	void SpawnBot();

	FTimerHandle SpawnBotTimerHandle;
	bool bMatchStarted = false;
};

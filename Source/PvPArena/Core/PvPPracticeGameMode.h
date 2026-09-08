// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PvPDeathmatchGameMode.h"
#include "PvPPracticeGameMode.generated.h"

class APvPAIController;
class UPvPAIDifficultyProfile;

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

	/** Must be called before BeginMatch() to take effect -- applied to the bot's controller when it spawns. Safe to call with nullptr (bot keeps its class defaults). */
	UFUNCTION(BlueprintCallable, Category = "Practice")
	void SetDifficultyProfile(UPvPAIDifficultyProfile* Profile);

	/** In-place rematch: resets scores, respawns both combatants at full health, re-arms the bot's engagement delay. Keeps the same difficulty. Called by APvPPlayerController::HandlePlayAgain() instead of reloading the level, so "play again" is actually a rematch, not a trip back to the main menu. */
	UFUNCTION(BlueprintCallable, Category = "Practice")
	void RestartMatch();

protected:
	virtual void BeginPlay() override;

	/** Prefers the "PlayerSpawn"-tagged APlayerStart for the human player's initial spawn (falls back to Super's default selection if none exists) -- keeps the initial spawn consistent with HandleRespawn()'s tagged-start logic now that there are two PlayerStarts in the level. */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

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

	/** First combatant (player = team 0, bot = team 1, tracked via
	 * APvPDeathmatchGameState::TeamScores) to reach this many kills wins. */
	UPROPERTY(EditDefaultsOnly, Category = "Practice")
	int32 KillsToWin = 3;

private:
	void SpawnBot();

	/** Bound to every combatant's UPvPHealthComponent::OnDeath -- scores the kill and checks the win condition. */
	UFUNCTION()
	void HandleAnyCombatantDeath(AActor* VictimActor, AController* InstigatorController);

	/** Bound to the player's APvPCharacter::OnRespawned -- re-arms the bot's engagement delay. See BeginMatch() for why. */
	UFUNCTION()
	void HandlePlayerRespawned();

	void EndMatchWithResult(bool bPlayerWon);

	FTimerHandle SpawnBotTimerHandle;
	bool bMatchStarted = false;
	bool bMatchEnded = false;

	TWeakObjectPtr<APvPAIController> BotControllerRef;

	UPROPERTY()
	TObjectPtr<UPvPAIDifficultyProfile> PendingDifficultyProfile;
};

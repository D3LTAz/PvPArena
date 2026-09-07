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
 */
UCLASS()
class APvPPracticeGameMode : public APvPDeathmatchGameMode
{
	GENERATED_BODY()

public:
	APvPPracticeGameMode();

protected:
	virtual void BeginPlay() override;

	/** Pawn class used for the AI bot. Defaults to the same class as the human player. */
	UPROPERTY(EditDefaultsOnly, Category = "Practice")
	TSubclassOf<APawn> BotPawnClass;

	UPROPERTY(EditDefaultsOnly, Category = "Practice")
	TSubclassOf<APvPAIController> BotControllerClass;

	/** Distance in front of the player to spawn the bot. */
	UPROPERTY(EditDefaultsOnly, Category = "Practice")
	float BotSpawnDistance = 1000.f;

private:
	void SpawnBot();

	FTimerHandle SpawnBotTimerHandle;
};

// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPPracticeGameMode.h"
#include "PvPAIController.h"
#include "PvPCharacter.h"
#include "PvPPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "PvPArena.h"

APvPPracticeGameMode::APvPPracticeGameMode()
{
	// Pure C++ classes -- see APvPCharacter/APvPPlayerController's class
	// comments for why this doesn't go through a Blueprint subclass.
	DefaultPawnClass = APvPCharacter::StaticClass();
	BotPawnClass = APvPCharacter::StaticClass();
	PlayerControllerClass = APvPPlayerController::StaticClass();
	BotControllerClass = APvPAIController::StaticClass();
}

void APvPPracticeGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Bot spawn is deferred to BeginMatch() -- see class comment. Player
	// pawn spawn/possession happens automatically as part of the normal
	// GameMode flow regardless.
}

void APvPPracticeGameMode::BeginMatch()
{
	if (bMatchStarted)
	{
		return;
	}
	bMatchStarted = true;

	SetMatchState(EPvPMatchState::InProgress);

	// Give the player pawn one extra frame to be fully settled before spawning the bot.
	GetWorldTimerManager().SetTimer(SpawnBotTimerHandle, this, &APvPPracticeGameMode::SpawnBot, 0.2f, false);
}

void APvPPracticeGameMode::SpawnBot()
{
	UWorld* World = GetWorld();
	if (!World || !BotPawnClass)
	{
		UE_LOG(LogPvPArena, Error, TEXT("Cannot spawn practice bot -- no BotPawnClass resolved."));
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);

	FVector SpawnLocation = FVector::ZeroVector;
	if (PlayerPawn)
	{
		SpawnLocation = PlayerPawn->GetActorLocation() + PlayerPawn->GetActorForwardVector() * BotSpawnDistance;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APawn* BotPawn = World->SpawnActor<APawn>(BotPawnClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (!BotPawn)
	{
		UE_LOG(LogPvPArena, Error, TEXT("Failed to spawn practice bot pawn."));
		return;
	}

	APvPAIController* BotController = World->SpawnActor<APvPAIController>(BotControllerClass ? *BotControllerClass : APvPAIController::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
	if (BotController)
	{
		BotController->Possess(BotPawn);
		BotController->SetTargetPawn(PlayerPawn);
		UE_LOG(LogPvPArena, Log, TEXT("Practice bot spawned and possessed."));
	}
}

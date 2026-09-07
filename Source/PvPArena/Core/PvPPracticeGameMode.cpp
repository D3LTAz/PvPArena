// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPPracticeGameMode.h"
#include "PvPAIController.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "PvPArena.h"

APvPPracticeGameMode::APvPPracticeGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PawnFinder(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PawnFinder.Succeeded())
	{
		DefaultPawnClass = PawnFinder.Class;
		BotPawnClass = PawnFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> ControllerFinder(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonPlayerController"));
	if (ControllerFinder.Succeeded())
	{
		PlayerControllerClass = ControllerFinder.Class;
	}

	BotControllerClass = APvPAIController::StaticClass();
}

void APvPPracticeGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Give the player pawn a moment to spawn/possess before spawning the bot.
	GetWorldTimerManager().SetTimer(SpawnBotTimerHandle, this, &APvPPracticeGameMode::SpawnBot, 0.5f, false);
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

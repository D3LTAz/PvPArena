// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPPracticeGameMode.h"
#include "PvPAIController.h"
#include "PvPCharacter.h"
#include "PvPPlayerController.h"
#include "PvPHealthComponent.h"
#include "PvPDeathmatchGameState.h"
#include "PvPAIDifficultyProfile.h"
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

void APvPPracticeGameMode::SetDifficultyProfile(UPvPAIDifficultyProfile* Profile)
{
	PendingDifficultyProfile = Profile;
}

void APvPPracticeGameMode::BeginMatch()
{
	if (bMatchStarted)
	{
		return;
	}
	bMatchStarted = true;

	SetMatchState(EPvPMatchState::InProgress);

	if (APvPCharacter* PlayerChar = Cast<APvPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
	{
		if (UPvPHealthComponent* Health = PlayerChar->GetHealthComponent())
		{
			Health->OnDeath.AddDynamic(this, &APvPPracticeGameMode::HandleAnyCombatantDeath);
		}
	}

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
		BotController->ApplyDifficultyProfile(PendingDifficultyProfile);
		BotControllerRef = BotController;
		UE_LOG(LogPvPArena, Log, TEXT("Practice bot spawned and possessed (difficulty=%s)."),
			PendingDifficultyProfile ? *PendingDifficultyProfile->DifficultyName.ToString() : TEXT("default"));
	}

	if (APvPCharacter* BotChar = Cast<APvPCharacter>(BotPawn))
	{
		if (UPvPHealthComponent* Health = BotChar->GetHealthComponent())
		{
			Health->OnDeath.AddDynamic(this, &APvPPracticeGameMode::HandleAnyCombatantDeath);
		}
	}
}

void APvPPracticeGameMode::HandleAnyCombatantDeath(AActor* VictimActor, AController* InstigatorController)
{
	if (bMatchEnded)
	{
		return;
	}

	APvPDeathmatchGameState* GS = GetGameState<APvPDeathmatchGameState>();
	if (!GS)
	{
		return;
	}

	const APawn* VictimPawn = Cast<APawn>(VictimActor);
	const bool bVictimIsPlayer = VictimPawn && VictimPawn->IsPlayerControlled();

	// Award the kill to whichever side did NOT die: team 0 = player, team 1 = bot.
	const int32 ScoringTeam = bVictimIsPlayer ? 1 : 0;
	GS->AddTeamScore(ScoringTeam, 1);

	const int32 PlayerScore = GS->TeamScores.IsValidIndex(0) ? GS->TeamScores[0] : 0;
	const int32 BotScore = GS->TeamScores.IsValidIndex(1) ? GS->TeamScores[1] : 0;

	UE_LOG(LogPvPArena, Log, TEXT("Score -- You: %d  Bot: %d"), PlayerScore, BotScore);

	if (PlayerScore >= KillsToWin)
	{
		EndMatchWithResult(true);
	}
	else if (BotScore >= KillsToWin)
	{
		EndMatchWithResult(false);
	}
}

void APvPPracticeGameMode::EndMatchWithResult(bool bPlayerWon)
{
	bMatchEnded = true;
	SetMatchState(EPvPMatchState::Ending);

	// Stop the bot from continuing to chase/fire once the match is decided.
	if (APvPAIController* BotController = BotControllerRef.Get())
	{
		BotController->SetTargetPawn(nullptr);
	}

	if (APvPPlayerController* PC = Cast<APvPPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		PC->ShowMatchResult(bPlayerWon);
	}

	UE_LOG(LogPvPArena, Log, TEXT("Match ended -- player %s."), bPlayerWon ? TEXT("won") : TEXT("lost"));
}

// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPPracticeGameMode.h"
#include "PvPAIController.h"
#include "PvPCharacter.h"
#include "PvPPlayerController.h"
#include "PvPHealthComponent.h"
#include "PvPDeathmatchGameState.h"
#include "PvPAIDifficultyProfile.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "PvPArena.h"

namespace PvPPracticeGameModePrivate
{
	static APlayerStart* FindTaggedPlayerStart(UWorld* World, FName Tag)
	{
		APlayerStart* Fallback = nullptr;
		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			Fallback = *It;
			if (It->PlayerStartTag == Tag)
			{
				return *It;
			}
		}
		return Fallback;
	}
}

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

AActor* APvPPracticeGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (APlayerStart* Start = PvPPracticeGameModePrivate::FindTaggedPlayerStart(GetWorld(), FName(TEXT("PlayerSpawn"))))
	{
		return Start;
	}

	return Super::ChoosePlayerStart_Implementation(Player);
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

		// Re-arm the bot's engagement delay whenever the player respawns --
		// otherwise TargetAcquiredTimeSeconds is still whatever it was from
		// the original SetTargetPawn() call at bot spawn, so the delay has
		// long since elapsed and the bot can fire the instant the player
		// reappears.
		PlayerChar->OnRespawned.AddDynamic(this, &APvPPracticeGameMode::HandlePlayerRespawned);
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

	// Spawn at the "BotSpawn"-tagged PlayerStart -- a fixed, known-good
	// position inside the ring, rather than an offset from the player's
	// current location/facing (which could place the bot outside the ring
	// wall depending on where the player was standing, and which is what
	// HandleRespawn() also now uses, so initial spawn and respawn agree).
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	if (APlayerStart* BotStart = PvPPracticeGameModePrivate::FindTaggedPlayerStart(World, FName(TEXT("BotSpawn"))))
	{
		SpawnLocation = BotStart->GetActorLocation();
		SpawnRotation = BotStart->GetActorRotation();
	}
	else if (PlayerPawn)
	{
		// Fallback if the level has no BotSpawn-tagged start.
		SpawnLocation = PlayerPawn->GetActorLocation() + PlayerPawn->GetActorForwardVector() * BotSpawnDistance;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APawn* BotPawn = World->SpawnActor<APawn>(BotPawnClass, SpawnLocation, SpawnRotation, SpawnParams);
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

void APvPPracticeGameMode::HandlePlayerRespawned()
{
	if (APvPAIController* BotController = BotControllerRef.Get())
	{
		if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			BotController->SetTargetPawn(PlayerPawn);
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

void APvPPracticeGameMode::RestartMatch()
{
	APvPDeathmatchGameState* GS = GetGameState<APvPDeathmatchGameState>();
	if (!GS)
	{
		return;
	}

	bMatchEnded = false;
	GS->ResetScores();
	SetMatchState(EPvPMatchState::InProgress);

	// HandleRespawn() is safe to call on a currently-alive character (it
	// just re-teleports/re-heals it) as well as a dead one -- whoever WON
	// the last match is still alive wherever the fight ended, so this needs
	// to reset both combatants unconditionally, not just revive a corpse.
	if (APvPCharacter* PlayerChar = Cast<APvPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
	{
		PlayerChar->HandleRespawn();
	}

	if (APvPAIController* BotController = BotControllerRef.Get())
	{
		if (APvPCharacter* BotChar = Cast<APvPCharacter>(BotController->GetPawn()))
		{
			BotChar->HandleRespawn();
		}

		// HandlePlayerRespawned() (bound to the player's OnRespawned) already
		// re-arms this, but the bot's own respawn above doesn't retarget --
		// belt-and-suspenders in case ordering ever changes.
		if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			BotController->SetTargetPawn(PlayerPawn);
		}
	}

	UE_LOG(LogPvPArena, Log, TEXT("Match restarted (rematch, same difficulty)."));
}

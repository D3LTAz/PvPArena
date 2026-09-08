// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPAIController.h"
#include "PvPCharacter.h"
#include "PvPWeaponComponent.h"
#include "PvPAIDifficultyProfile.h"
#include "GameFramework/Pawn.h"

APvPAIController::APvPAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APvPAIController::SetTargetPawn(APawn* InTargetPawn)
{
	TargetPawn = InTargetPawn;
	TargetAcquiredTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
}

void APvPAIController::ApplyDifficultyProfile(const UPvPAIDifficultyProfile* Profile)
{
	if (!Profile)
	{
		return;
	}

	EngageRange = Profile->EngageRange;
	PreferredStandoffRange = Profile->PreferredStandoffRange;
	EngageDelayAfterTargetAcquired = Profile->EngageDelayAfterTargetAcquired;
	FireAttemptCooldown = Profile->FireAttemptCooldown;
	MissChance = Profile->MissChance;
}

void APvPAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* ControlledPawn = GetPawn();
	APawn* Target = TargetPawn.Get();
	if (!ControlledPawn || !Target)
	{
		return;
	}

	const FVector ToTarget = Target->GetActorLocation() - ControlledPawn->GetActorLocation();
	const float Distance = ToTarget.Size();
	const FVector Direction = ToTarget.GetSafeNormal();

	// Face the target -- Fire()'s eye rotation reads the controller's
	// control rotation, so this is what actually aims the bot's shots.
	SetControlRotation(Direction.Rotation());

	if (Distance > PreferredStandoffRange)
	{
		ControlledPawn->AddMovementInput(Direction, 1.0f);
	}

	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	const double TimeSinceAcquired = Now - TargetAcquiredTimeSeconds;
	const double TimeSinceLastAttempt = Now - LastFireAttemptTimeSeconds;

	if (Distance <= EngageRange
		&& TimeSinceAcquired >= EngageDelayAfterTargetAcquired
		&& TimeSinceLastAttempt >= FireAttemptCooldown)
	{
		LastFireAttemptTimeSeconds = Now;

		// Difficulty lever: skip the attempt entirely some of the time,
		// simulating a miss/hesitation, independent of weapon spread.
		if (FMath::FRand() >= MissChance)
		{
			if (APvPCharacter* PvPCharacter = Cast<APvPCharacter>(ControlledPawn))
			{
				if (UPvPWeaponComponent* Weapon = PvPCharacter->GetWeaponComponent())
				{
					// The bot has no reload and shares the same empty-clip gate
					// as the player, but nothing ever presses "Q" for it --
					// left alone, an emptied bot just stands there attempting
					// to fire nothing for the rest of the match.
					if (Weapon->CurrentAmmo <= 0)
					{
						Weapon->CycleWeapon();
					}
					else
					{
						Weapon->Fire();
					}
				}
			}
		}
	}
}

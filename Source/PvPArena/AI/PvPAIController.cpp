// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPAIController.h"
#include "PvPCharacter.h"
#include "PvPWeaponComponent.h"
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

	const double TimeSinceAcquired = (GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0) - TargetAcquiredTimeSeconds;
	if (Distance <= EngageRange && TimeSinceAcquired >= EngageDelayAfterTargetAcquired)
	{
		if (APvPCharacter* PvPCharacter = Cast<APvPCharacter>(ControlledPawn))
		{
			if (UPvPWeaponComponent* Weapon = PvPCharacter->GetWeaponComponent())
			{
				Weapon->Fire();
			}
		}
	}
}

// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"

UPvPHealthComponent::UPvPHealthComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UPvPHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

void UPvPHealthComponent::ServerApplyDamage(float Amount, AController* InstigatorController)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || bIsDead || Amount <= 0.f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.f, MaxHealth);
	OnRep_CurrentHealth();

	if (CurrentHealth <= 0.f)
	{
		bIsDead = true;
		OnDeath.Broadcast(GetOwner(), InstigatorController);
	}
}

void UPvPHealthComponent::ResetHealth()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	bIsDead = false;
	CurrentHealth = MaxHealth;
	OnRep_CurrentHealth();
}

void UPvPHealthComponent::OnRep_CurrentHealth()
{
	// Placeholder hook for HUD health bar updates.
}

void UPvPHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPvPHealthComponent, CurrentHealth);
}

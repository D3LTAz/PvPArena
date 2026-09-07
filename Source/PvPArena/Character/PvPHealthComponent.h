// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PvPHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPvPOnDeathSignature, AActor*, InstigatorActor, AController*, InstigatorController);

/**
 * Replicated CurrentHealth/MaxHealth. Server-authoritative only -- no client
 * can reduce its own health. Broadcasts OnDeath, consumed by the owning
 * character (ragdoll/hide + respawn timer stub) and later the GameMode
 * (Phase E scoring).
 */
UCLASS(ClassGroup=(PvPArena), meta=(BlueprintSpawnableComponent))
class UPvPHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPvPHealthComponent();

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.f;

	/** Server-only. Reduces CurrentHealth and broadcasts OnDeath if it reaches 0. */
	void ServerApplyDamage(float Amount, AController* InstigatorController);

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FPvPOnDeathSignature OnDeath;

	UFUNCTION()
	void OnRep_CurrentHealth();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

private:
	bool bIsDead = false;
};

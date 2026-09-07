// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PvPWeaponComponent.generated.h"

class UPvPWeaponData;

/**
 * Attached to APvPCharacter. Fire() is called locally on the owning client,
 * which sends a ServerFire RPC; the server performs an authoritative line
 * trace and applies damage via UPvPHealthComponent::ServerApplyDamage, then
 * NetMulticasts a confirmed-hit event so all clients render the same
 * tracer/impact -- clients never resolve damage themselves.
 *
 * No lag-compensation rewind buffer (see the 2026-09-07 pivot decision in
 * docs/decisions.md) -- that exists to compensate for *other players'* ping,
 * and the only "other combatant" right now is a server-controlled AI bot.
 */
UCLASS(ClassGroup=(PvPArena), meta=(BlueprintSpawnableComponent))
class UPvPWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPvPWeaponComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UPvPWeaponData> WeaponData;

	/** The 2-3 weapon loadout available to cycle through. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TArray<TObjectPtr<UPvPWeaponData>> AvailableWeapons;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo = 0;

	/** Called locally on the owning client (or directly by AI logic -- both paths funnel through the same ServerFire RPC). */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();

	/** Swaps to a different weapon's data (used for the 2-3 weapon loadout). Cosmetic-only on the client; damage/range always reads WeaponData server-side. */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(UPvPWeaponData* NewWeaponData);

	/** Advances to the next weapon in AvailableWeapons, wrapping around. */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void CycleWeapon();

	UFUNCTION()
	void OnRep_CurrentAmmo();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerFire(FVector_NetQuantize Origin, FVector_NetQuantizeNormal Direction);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastConfirmedHit(FVector_NetQuantize TraceStart, FVector_NetQuantize ImpactPoint, bool bHit);

private:
	float LastFireTime = -1000.f;

	/** Server-only. Traces from Origin along Direction and applies damage on hit. */
	void PerformServerTrace(const FVector& Origin, const FVector& Direction);
};

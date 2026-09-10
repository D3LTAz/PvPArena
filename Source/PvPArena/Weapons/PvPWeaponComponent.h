// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PvPWeaponComponent.generated.h"

class UPvPWeaponData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPvPOnWeaponEquippedSignature, UPvPWeaponData*, NewWeaponData);

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

	/** Refills CurrentAmmo to WeaponData's MaxAmmo for the currently equipped weapon. Server-authoritative like Fire() -- calls the trace-side logic directly when the owner HasAuthority() (listen-server/AI), otherwise routes through ServerReload. No reload time/animation gate yet -- instant, same "functional not polished" bar as the rest of the weapon system. */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	/** Fires whenever EquipWeapon() changes WeaponData (including the initial equip in BeginPlay) -- the owning character listens to this to swap its held-weapon mesh. */
	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FPvPOnWeaponEquippedSignature OnWeaponEquipped;

	UFUNCTION()
	void OnRep_CurrentAmmo();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerFire(FVector_NetQuantize Origin, FVector_NetQuantizeNormal Direction);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastConfirmedHit(FVector_NetQuantize TraceStart, FVector_NetQuantize ImpactPoint, bool bHit);

private:
	float LastFireTime = -1000.f;

	/** Server-only. Traces from Origin along Direction and applies damage on hit. */
	void PerformServerTrace(const FVector& Origin, const FVector& Direction);
};

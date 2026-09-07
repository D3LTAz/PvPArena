// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PvPWeaponData.generated.h"

class USkeletalMesh;
class UNiagaraSystem;
class USoundBase;

/**
 * Blueprint-authorable weapon config: damage, fire rate, range, spread, ammo,
 * and soft references to cosmetic assets. No weapon numbers live in C++ --
 * one data asset instance = one weapon.
 */
UCLASS(BlueprintType)
class UPvPWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FText WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Damage = 20.f;

	/** Shots per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireRate = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float MaxRange = 10000.f;

	/** Half-angle cone spread in degrees, applied around the aim direction. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float SpreadDegrees = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 MaxAmmo = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Cosmetic")
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Cosmetic")
	TSoftObjectPtr<UNiagaraSystem> MuzzleFlashFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Cosmetic")
	TSoftObjectPtr<USoundBase> FireSound;

	float GetFireInterval() const { return FireRate > 0.f ? (1.f / FireRate) : 0.2f; }
};

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PvPWeaponData.generated.h"

class USkeletalMesh;
class UStaticMesh;
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

	/** Real skeletal weapon mesh -- unused until proper art exists (see PlaceholderMesh). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Cosmetic")
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;

	/** Graybox stand-in shown on the character until WeaponMesh has real art. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Cosmetic")
	TSoftObjectPtr<UStaticMesh> PlaceholderMesh;

	/** Local scale applied to PlaceholderMesh (X = barrel/length axis). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Cosmetic")
	FVector PlaceholderScale = FVector(1.f, 0.2f, 0.2f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Cosmetic")
	TSoftObjectPtr<UNiagaraSystem> MuzzleFlashFX;

	/** Socket name on WeaponMesh (or PlaceholderMesh) to attach MuzzleFlashFX to. Leave None to spawn at the trace origin unattached -- safe default until a real weapon mesh's actual socket name is known. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Cosmetic")
	FName MuzzleSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Cosmetic")
	TSoftObjectPtr<USoundBase> FireSound;

	float GetFireInterval() const { return FireRate > 0.f ? (1.f / FireRate) : 0.2f; }
};

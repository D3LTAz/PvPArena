// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPWeaponComponent.h"
#include "PvPWeaponData.h"
#include "PvPHealthComponent.h"
#include "PvPCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Components/MeshComponent.h"
#include "DrawDebugHelpers.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "PvPArena.h"

UPvPWeaponComponent::UPvPWeaponComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UPvPWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	UPvPWeaponData* InitialWeapon = WeaponData ? WeaponData : (AvailableWeapons.Num() > 0 ? AvailableWeapons[0] : nullptr);
	EquipWeapon(InitialWeapon);
}

void UPvPWeaponComponent::EquipWeapon(UPvPWeaponData* NewWeaponData)
{
	WeaponData = NewWeaponData;
	CurrentAmmo = WeaponData ? WeaponData->MaxAmmo : 0;
	OnRep_CurrentAmmo();
	OnWeaponEquipped.Broadcast(WeaponData);
}

void UPvPWeaponComponent::CycleWeapon()
{
	if (AvailableWeapons.Num() == 0)
	{
		return;
	}

	const int32 CurrentIndex = WeaponData ? AvailableWeapons.IndexOfByKey(WeaponData) : INDEX_NONE;
	const int32 NextIndex = (CurrentIndex == INDEX_NONE) ? 0 : (CurrentIndex + 1) % AvailableWeapons.Num();
	EquipWeapon(AvailableWeapons[NextIndex]);
}

void UPvPWeaponComponent::Reload()
{
	AActor* Owner = GetOwner();
	if (!Owner || !WeaponData)
	{
		return;
	}

	// Same listen-server RPC caveat as Fire() -- call the authoritative path
	// directly when we already have authority instead of routing through a
	// Server RPC that can be a no-op with no remote connection.
	if (Owner->HasAuthority())
	{
		CurrentAmmo = WeaponData->MaxAmmo;
		OnRep_CurrentAmmo();
		UE_LOG(LogPvPArena, Log, TEXT("'%s' reloaded (weapon=%s ammo=%d)."),
			*GetNameSafe(Owner), *WeaponData->GetName(), CurrentAmmo);
	}
	else
	{
		ServerReload();
	}
}

void UPvPWeaponComponent::ServerReload_Implementation()
{
	if (!WeaponData)
	{
		return;
	}

	CurrentAmmo = WeaponData->MaxAmmo;
	OnRep_CurrentAmmo();
	UE_LOG(LogPvPArena, Log, TEXT("'%s' reloaded (weapon=%s ammo=%d)."),
		*GetNameSafe(GetOwner()), *WeaponData->GetName(), CurrentAmmo);
}

void UPvPWeaponComponent::Fire()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogPvPArena, Warning, TEXT("WeaponComponent::Fire() called with no Owner."));
		return;
	}

	if (!WeaponData)
	{
		UE_LOG(LogPvPArena, Warning, TEXT("WeaponComponent::Fire() on '%s' -- WeaponData is null, nothing equipped."), *GetNameSafe(Owner));
	}

	const float FireInterval = WeaponData ? WeaponData->GetFireInterval() : 0.2f;
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	if (Now - LastFireTime < FireInterval)
	{
		return;
	}

	// Client-side early-out to avoid a pointless RPC when visibly empty --
	// CurrentAmmo is replicated, so this is stale-safe; the server (in
	// PerformServerTrace) is still the actual authority on whether the shot
	// counts, same as damage.
	if (CurrentAmmo <= 0)
	{
		return;
	}

	LastFireTime = Now;

	if (Owner->HasAuthority())
	{
		--CurrentAmmo;
		OnRep_CurrentAmmo();
	}

	UE_LOG(LogPvPArena, Log, TEXT("'%s' fired (weapon=%s ammo=%d)."),
		*GetNameSafe(Owner), WeaponData ? *WeaponData->GetName() : TEXT("none"), CurrentAmmo);

	FVector EyeLocation;
	FRotator EyeRotation;
	Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector Direction = EyeRotation.Vector();
	if (WeaponData && WeaponData->SpreadDegrees > 0.f)
	{
		Direction = FMath::VRandCone(Direction, FMath::DegreesToRadians(WeaponData->SpreadDegrees));
	}

	// Listen-server / AI: call the trace directly. A Server RPC on a
	// replicated component can be dropped when there is no remote
	// connection, which left CurrentAmmo stuck at max even after Fire().
	if (Owner->HasAuthority())
	{
		PerformServerTrace(EyeLocation, Direction);
	}
	else
	{
		ServerFire(EyeLocation, Direction);
	}
}

void UPvPWeaponComponent::ServerFire_Implementation(FVector_NetQuantize Origin, FVector_NetQuantizeNormal Direction)
{
	UE_LOG(LogPvPArena, Verbose, TEXT("ServerFire_Implementation on '%s'."), *GetNameSafe(GetOwner()));
	PerformServerTrace(Origin, Direction);
}

void UPvPWeaponComponent::PerformServerTrace(const FVector& Origin, const FVector& Direction)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}

	const float Range = WeaponData ? WeaponData->MaxRange : 10000.f;
	const FVector End = Origin + Direction * Range;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(PvPWeaponTrace), true, Owner);
	Params.AddIgnoredActor(Owner);

	FHitResult Hit;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Origin, End, ECC_Pawn, Params);

	if (bHit)
	{
		UE_LOG(LogPvPArena, Log, TEXT("'%s' trace hit '%s'."), *GetNameSafe(Owner), *GetNameSafe(Hit.GetActor()));

		if (UPvPHealthComponent* HitHealth = Hit.GetActor() ? Hit.GetActor()->FindComponentByClass<UPvPHealthComponent>() : nullptr)
		{
			APawn* OwnerPawn = Cast<APawn>(Owner);
			AController* InstigatorController = OwnerPawn ? OwnerPawn->GetController() : nullptr;
			const float DamageDealt = WeaponData ? WeaponData->Damage : 10.f;
			HitHealth->ServerApplyDamage(DamageDealt, InstigatorController);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.5f, FColor::Red,
					FString::Printf(TEXT("HIT %s for %.0f (hp now %.0f)"), *GetNameSafe(Hit.GetActor()), DamageDealt, HitHealth->CurrentHealth));
			}
		}
		else if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.5f, FColor::Yellow,
				FString::Printf(TEXT("Hit %s (no health component)"), *GetNameSafe(Hit.GetActor())));
		}
	}
	else
	{
		UE_LOG(LogPvPArena, Verbose, TEXT("'%s' trace missed."), *GetNameSafe(Owner));

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::White, TEXT("Shot fired (no hit)"));
		}
	}

	MulticastConfirmedHit(Origin, bHit ? FVector(Hit.ImpactPoint) : FVector(End), bHit);
}

void UPvPWeaponComponent::MulticastConfirmedHit_Implementation(FVector_NetQuantize TraceStart, FVector_NetQuantize ImpactPoint, bool bHit)
{
	// Placeholder: no impact FX authored yet, so draw a debug tracer. Muzzle
	// flash/sound below are real cosmetic hooks -- no-ops until a
	// UPvPWeaponData instance actually has MuzzleFlashFX/FireSound assigned
	// (see docs/claude-code-prompt-asset-integration.md), so this doesn't
	// wait on any specific asset to be safe to leave in.
#if ENABLE_DRAW_DEBUG
	if (UWorld* World = GetWorld())
	{
		DrawDebugLine(World, TraceStart, ImpactPoint, bHit ? FColor::Red : FColor::Yellow, false, 1.5f, 0, 3.f);
		if (bHit)
		{
			DrawDebugSphere(World, ImpactPoint, 15.f, 8, FColor::Red, false, 1.5f);
		}
	}
#endif

	if (!WeaponData)
	{
		return;
	}

	UMeshComponent* MuzzleMesh = nullptr;
	if (const APvPCharacter* PvPCharacter = Cast<APvPCharacter>(GetOwner()))
	{
		MuzzleMesh = PvPCharacter->GetActiveWeaponMeshComponent();
	}

	const bool bHaveSocket = MuzzleMesh && WeaponData->MuzzleSocketName != NAME_None && MuzzleMesh->DoesSocketExist(WeaponData->MuzzleSocketName);

	if (UNiagaraSystem* MuzzleFlash = WeaponData->MuzzleFlashFX.LoadSynchronous())
	{
		if (bHaveSocket)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlash, MuzzleMesh, WeaponData->MuzzleSocketName,
				FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
		}
		else
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, MuzzleFlash, TraceStart);
		}
	}

	if (USoundBase* Sound = WeaponData->FireSound.LoadSynchronous())
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, TraceStart);
	}
}

void UPvPWeaponComponent::OnRep_CurrentAmmo()
{
	// Placeholder hook for HUD ammo counter updates.
}

void UPvPWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPvPWeaponComponent, CurrentAmmo);
}

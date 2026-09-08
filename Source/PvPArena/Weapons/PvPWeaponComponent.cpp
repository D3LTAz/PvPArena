// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPWeaponComponent.h"
#include "PvPWeaponData.h"
#include "PvPHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
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

void UPvPWeaponComponent::Fire()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	const float FireInterval = WeaponData ? WeaponData->GetFireInterval() : 0.2f;
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	if (Now - LastFireTime < FireInterval)
	{
		return;
	}
	LastFireTime = Now;

	FVector EyeLocation;
	FRotator EyeRotation;
	Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector Direction = EyeRotation.Vector();
	if (WeaponData && WeaponData->SpreadDegrees > 0.f)
	{
		Direction = FMath::VRandCone(Direction, FMath::DegreesToRadians(WeaponData->SpreadDegrees));
	}

	ServerFire(EyeLocation, Direction);
}

void UPvPWeaponComponent::ServerFire_Implementation(FVector_NetQuantize Origin, FVector_NetQuantizeNormal Direction)
{
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
		if (UPvPHealthComponent* HitHealth = Hit.GetActor() ? Hit.GetActor()->FindComponentByClass<UPvPHealthComponent>() : nullptr)
		{
			APawn* OwnerPawn = Cast<APawn>(Owner);
			AController* InstigatorController = OwnerPawn ? OwnerPawn->GetController() : nullptr;
			HitHealth->ServerApplyDamage(WeaponData ? WeaponData->Damage : 10.f, InstigatorController);
		}
	}

	MulticastConfirmedHit(Origin, bHit ? FVector(Hit.ImpactPoint) : FVector(End), bHit);
}

void UPvPWeaponComponent::MulticastConfirmedHit_Implementation(FVector_NetQuantize TraceStart, FVector_NetQuantize ImpactPoint, bool bHit)
{
	// Placeholder: no muzzle/impact FX authored yet, so draw a debug tracer.
#if ENABLE_DRAW_DEBUG
	if (UWorld* World = GetWorld())
	{
		DrawDebugLine(World, TraceStart, ImpactPoint, bHit ? FColor::Red : FColor::Yellow, false, 0.5f, 0, 1.5f);
	}
#endif
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

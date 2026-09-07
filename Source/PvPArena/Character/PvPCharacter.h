// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TimerManager.h"
#include "PvPCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UPvPHealthComponent;
class UPvPWeaponComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  Player-controllable character. Standard UCharacterMovementComponent --
 *  no custom movement tech. Owns UPvPHealthComponent and UPvPWeaponComponent.
 *
 *  Concrete (not abstract) -- the mesh/anim/input asset references are
 *  resolved directly via ConstructorHelpers against existing template
 *  content (SKM_Manny_Simple, ABP_Unarmed, IA_*) rather than requiring a
 *  Blueprint subclass. This sidesteps the BP_ThirdPersonCharacter parent-
 *  class redirect breakage logged in docs/decisions.md.
 */
UCLASS()
class APvPCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Replicated health, server-authoritative damage, OnDeath delegate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPvPHealthComponent> HealthComponent;

	/** Server-authoritative hitscan fire */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPvPWeaponComponent> WeaponComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* FireAction;

	/** Handle to the placeholder respawn timer started on death (Phase E replaces this with real respawn-at-PlayerStart). */
	FTimerHandle RespawnTimerHandle;

public:

	/** Constructor */
	APvPCharacter();

protected:

	virtual void BeginPlay() override;

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for fire input */
	void HandleFireInput(const FInputActionValue& Value);

	/** Bound directly to the Q key via the legacy raw-key input path (no Input Action asset needed) -- cycles the 2-3 weapon loadout. */
	void HandleCycleWeaponInput();

	/** Bound to HealthComponent::OnDeath. Hides/disables the character and starts the respawn timer stub. */
	UFUNCTION()
	void HandleDeath(AActor* InstigatorActor, AController* InstigatorController);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns HealthComponent subobject **/
	FORCEINLINE UPvPHealthComponent* GetHealthComponent() const { return HealthComponent; }

	/** Returns WeaponComponent subobject **/
	FORCEINLINE UPvPWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }
};

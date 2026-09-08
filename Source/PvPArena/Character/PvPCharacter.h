// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TimerManager.h"
#include "Camera/PlayerCameraManager.h"
#include "PvPCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UPvPHealthComponent;
class UPvPWeaponComponent;
class UPvPWeaponData;
class UStaticMeshComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPvPOnRespawnedSignature);

UENUM(BlueprintType)
enum class ECameraViewMode : uint8
{
	ThirdPerson,
	FirstPerson
};

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

	/** Follow camera (third-person) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** First-person camera, attached to the capsule at eye height (animation-independent). Inactive by default. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	/** Replicated health, server-authoritative damage, OnDeath delegate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPvPHealthComponent> HealthComponent;

	/** Server-authoritative hitscan fire */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPvPWeaponComponent> WeaponComponent;

	/** Graybox stand-in for the currently equipped weapon (see UPvPWeaponData::PlaceholderMesh) -- swapped by HandleWeaponEquipped(). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;

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

	/** Handle to the respawn timer started on death. */
	FTimerHandle RespawnTimerHandle;

	/** Seconds between death and respawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	float RespawnDelaySeconds = 3.f;

	/** True = blend camera views via SetViewTargetWithBlend. False = instant cut. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	bool bSmoothCameraTransition = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "bSmoothCameraTransition", ClampMin = "0.0"))
	float CameraBlendTime = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TEnumAsByte<EViewTargetBlendFunction> CameraBlendFunction = VTBlend_Cubic;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	ECameraViewMode CurrentViewMode = ECameraViewMode::ThirdPerson;

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

	/**
	 * DIAGNOSTIC ONLY. Bound directly to LeftMouseButton via the same legacy
	 * raw-key path already proven to work for Q/V -- bypasses Enhanced Input
	 * entirely. If this never logs on click, the click isn't reaching the
	 * PlayerController's input component at all (a Slate/viewport/mouse-
	 * capture problem). If it DOES log but HandleFireInput still doesn't,
	 * the problem is specific to IA_Fire's Enhanced Input trigger evaluation.
	 * Remove once the fire-input bug is resolved.
	 */
	void HandleLegacyFireKeyDiagnostic();

	/** Bound to WeaponComponent::OnWeaponEquipped. Swaps WeaponMeshComponent's mesh/scale to match. */
	UFUNCTION()
	void HandleWeaponEquipped(UPvPWeaponData* NewWeaponData);

	/** Single source of truth for what changes between 1st/3rd person. */
	void ApplyViewMode(ECameraViewMode NewMode);

	/** Bound to HealthComponent::OnDeath. Hides/disables the character and starts the respawn timer. */
	UFUNCTION()
	void HandleDeath(AActor* InstigatorActor, AController* InstigatorController);

public:

	/** Fires RespawnDelaySeconds after death, or callable directly (e.g. APvPPracticeGameMode::RestartMatch()). Public so the GameMode can force an immediate respawn without waiting on the timer. Picks a PlayerStart by APlayerStart::PlayerStartTag -- "PlayerSpawn" for player-controlled, "BotSpawn" for AI-controlled -- falling back to any PlayerStart if no tagged match exists. Broadcasts OnRespawned so systems like the AI's engagement delay can re-arm. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void HandleRespawn();

	/** Broadcast at the end of HandleRespawn(). APvPPracticeGameMode listens on the player's instance to re-arm the bot's engagement delay -- otherwise the bot can fire on you the instant you reappear, since its delay is measured from when it first acquired you as a target, not from your last respawn. */
	UPROPERTY(BlueprintAssignable)
	FPvPOnRespawnedSignature OnRespawned;

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

	/** Toggles between 1st- and 3rd-person view. Bound to V by default. */
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ToggleCameraView();

	/** Explicitly set the view mode (no-ops if already in NewMode). */
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetCameraViewMode(ECameraViewMode NewMode);

	UFUNCTION(BlueprintPure, Category = "Camera")
	bool IsFirstPerson() const { return CurrentViewMode == ECameraViewMode::FirstPerson; }

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

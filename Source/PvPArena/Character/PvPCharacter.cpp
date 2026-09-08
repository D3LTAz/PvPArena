// Copyright Epic Games, Inc. All Rights Reserved.

#include "PvPCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PvPHealthComponent.h"
#include "PvPWeaponComponent.h"
#include "PvPWeaponData.h"
#include "Animation/AnimInstance.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerController.h"
#include "PvPArena.h"

APvPCharacter::APvPCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// First-person camera -- attached to the capsule (not a head socket) so
	// it's animation-independent; eye height is set in BeginPlay once
	// BaseEyeHeight is valid. Inactive until toggled via F11.
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetAutoActivate(false);

	// Replicated health / server-authoritative damage
	HealthComponent = CreateDefaultSubobject<UPvPHealthComponent>(TEXT("HealthComponent"));

	// Server-authoritative hitscan fire
	WeaponComponent = CreateDefaultSubobject<UPvPWeaponComponent>(TEXT("WeaponComponent"));

	// Mesh/anim placeholder -- resolved directly here rather than via a
	// Blueprint subclass (see the class comment for why).
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshFinder(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple"));
	if (MeshFinder.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshFinder.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimFinder(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed"));
	if (AnimFinder.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimFinder.Class);
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionFinder(TEXT("/Game/Input/Actions/IA_Jump"));
	if (JumpActionFinder.Succeeded()) { JumpAction = JumpActionFinder.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionFinder(TEXT("/Game/Input/Actions/IA_Move"));
	if (MoveActionFinder.Succeeded()) { MoveAction = MoveActionFinder.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionFinder(TEXT("/Game/Input/Actions/IA_Look"));
	if (LookActionFinder.Succeeded()) { LookAction = LookActionFinder.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> MouseLookActionFinder(TEXT("/Game/Input/Actions/IA_MouseLook"));
	if (MouseLookActionFinder.Succeeded()) { MouseLookAction = MouseLookActionFinder.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> FireActionFinder(TEXT("/Game/Input/Actions/IA_Fire"));
	if (FireActionFinder.Succeeded()) { FireAction = FireActionFinder.Object; }

	// 2-3 weapon loadout, cycled with Q (see HandleCycleWeaponInput).
	static ConstructorHelpers::FObjectFinder<UPvPWeaponData> PistolFinder(TEXT("/Game/Weapons/DA_Weapon_Pistol"));
	if (PistolFinder.Succeeded()) { WeaponComponent->AvailableWeapons.Add(PistolFinder.Object); }

	static ConstructorHelpers::FObjectFinder<UPvPWeaponData> RifleFinder(TEXT("/Game/Weapons/DA_Weapon_Rifle"));
	if (RifleFinder.Succeeded()) { WeaponComponent->AvailableWeapons.Add(RifleFinder.Object); }

	static ConstructorHelpers::FObjectFinder<UPvPWeaponData> SniperFinder(TEXT("/Game/Weapons/DA_Weapon_Sniper"));
	if (SniperFinder.Succeeded()) { WeaponComponent->AvailableWeapons.Add(SniperFinder.Object); }
}

void APvPCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &APvPCharacter::HandleDeath);
	}

	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight));
	ApplyViewMode(CurrentViewMode);
}

void APvPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APvPCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &APvPCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APvPCharacter::Look);

		// Firing
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &APvPCharacter::HandleFireInput);
	}
	else
	{
		UE_LOG(LogPvPArena, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

	// Weapon cycling and camera toggle: bound via the legacy raw-key path
	// rather than Input Action assets -- no IMC key-mapping required.
	PlayerInputComponent->BindKey(EKeys::Q, IE_Pressed, this, &APvPCharacter::HandleCycleWeaponInput);
	PlayerInputComponent->BindKey(EKeys::F11, IE_Pressed, this, &APvPCharacter::ToggleCameraView);
}

void APvPCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void APvPCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void APvPCharacter::HandleFireInput(const FInputActionValue& Value)
{
	if (WeaponComponent)
	{
		WeaponComponent->Fire();
	}
}

void APvPCharacter::HandleCycleWeaponInput()
{
	if (WeaponComponent)
	{
		WeaponComponent->CycleWeapon();
	}
}

void APvPCharacter::ToggleCameraView()
{
	const ECameraViewMode NewMode = (CurrentViewMode == ECameraViewMode::ThirdPerson)
		? ECameraViewMode::FirstPerson
		: ECameraViewMode::ThirdPerson;

	SetCameraViewMode(NewMode);
}

void APvPCharacter::SetCameraViewMode(ECameraViewMode NewMode)
{
	if (NewMode == CurrentViewMode)
	{
		return;
	}

	CurrentViewMode = NewMode;
	ApplyViewMode(NewMode);
}

void APvPCharacter::ApplyViewMode(ECameraViewMode NewMode)
{
	const bool bFirstPerson = (NewMode == ECameraViewMode::FirstPerson);

	// --- Active camera ---
	FollowCamera->SetActive(!bFirstPerson);
	FirstPersonCamera->SetActive(bFirstPerson);

	// --- Body visibility: hide from the LOCAL owner's own camera only --
	// other clients in multiplayer still see the full body either way. ---
	GetMesh()->SetOwnerNoSee(bFirstPerson);

	// --- Rotation behavior ---
	bUseControllerRotationYaw = bFirstPerson;
	GetCharacterMovement()->bOrientRotationToMovement = !bFirstPerson;

	// --- Apply the view change (instant or blended, same call either way) ---
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (bSmoothCameraTransition)
		{
			PC->SetViewTargetWithBlend(this, CameraBlendTime, CameraBlendFunction);
		}
		else
		{
			PC->SetViewTarget(this);
		}
	}
}

void APvPCharacter::HandleDeath(AActor* InstigatorActor, AController* InstigatorController)
{
	// Placeholder: hide + disable collision/movement on death. Real
	// ragdoll and respawn-at-PlayerStart logic lands in Phase E once the
	// GameMode owns team spawns -- this is only the C3 stub.
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->DisableMovement();
	}

	GetWorldTimerManager().SetTimer(RespawnTimerHandle, FTimerDelegate::CreateLambda([WeakThis = TWeakObjectPtr<APvPCharacter>(this)]()
	{
		if (APvPCharacter* StrongThis = WeakThis.Get())
		{
			UE_LOG(LogPvPArena, Log, TEXT("Respawn timer stub elapsed for '%s' -- real respawn lands in Phase E."), *GetNameSafe(StrongThis));
		}
	}), 3.0f, false);
}

void APvPCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void APvPCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void APvPCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void APvPCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

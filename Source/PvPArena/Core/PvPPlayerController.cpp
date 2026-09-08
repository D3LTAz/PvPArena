// Copyright Epic Games, Inc. All Rights Reserved.


#include "PvPPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "PvPTutorialWidget.h"
#include "PvPHUDWidget.h"
#include "PvPMatchResultWidget.h"
#include "PvPPracticeGameMode.h"
#include "PvPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PvPArena.h"
#include "Widgets/Input/SVirtualJoystick.h"

APvPPlayerController::APvPPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultIMCFinder(TEXT("/Game/Input/IMC_Default"));
	if (DefaultIMCFinder.Succeeded())
	{
		DefaultMappingContexts.Add(DefaultIMCFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MouseLookIMCFinder(TEXT("/Game/Input/IMC_MouseLook"));
	if (MouseLookIMCFinder.Succeeded())
	{
		MobileExcludedMappingContexts.Add(MouseLookIMCFinder.Object);
	}
}

void APvPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogPvPArena, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}

	if (IsLocalPlayerController())
	{
		TutorialWidget = CreateWidget<UPvPTutorialWidget>(this, UPvPTutorialWidget::StaticClass());
		if (TutorialWidget)
		{
			TutorialWidget->AddToViewport(100);
			TutorialWidget->OnDismissed.AddDynamic(this, &APvPPlayerController::HandleDismissTutorial);

			bShowMouseCursor = true;
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(TutorialWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
		}
	}
}

void APvPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool APvPPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void APvPPlayerController::HandleDismissTutorial()
{
	if (!TutorialWidget)
	{
		return;
	}

	TutorialWidget->RemoveFromParent();
	TutorialWidget = nullptr;

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	if (APvPCharacter* PvPChar = Cast<APvPCharacter>(GetPawn()))
	{
		HUDWidgetInstance = CreateWidget<UPvPHUDWidget>(this, UPvPHUDWidget::StaticClass());
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->InitFor(PvPChar);
			HUDWidgetInstance->AddToViewport(10);
		}
	}

	if (APvPPracticeGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<APvPPracticeGameMode>() : nullptr)
	{
		GM->BeginMatch();
	}
}

void APvPPlayerController::ShowMatchResult(bool bPlayerWon)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MatchResultWidget)
	{
		return;
	}

	MatchResultWidget = CreateWidget<UPvPMatchResultWidget>(this, UPvPMatchResultWidget::StaticClass());
	if (MatchResultWidget)
	{
		MatchResultWidget->InitFor(bPlayerWon);
		MatchResultWidget->AddToViewport(100);
		MatchResultWidget->OnPlayAgain.AddDynamic(this, &APvPPlayerController::HandlePlayAgain);

		bShowMouseCursor = true;
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(MatchResultWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}
}

void APvPPlayerController::HandlePlayAgain()
{
	UGameplayStatics::OpenLevel(this, TEXT("Lvl_PracticeRing"));
}

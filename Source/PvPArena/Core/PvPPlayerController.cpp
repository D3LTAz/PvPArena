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
#include "PvPMainMenuWidget.h"
#include "PvPAIMatchSetupWidget.h"
#include "PvPSettingsWidget.h"
#include "PvPAIDifficultyProfile.h"
#include "PvPSettingsSaveGame.h"
#include "PvPPracticeGameMode.h"
#include "PvPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AudioDevice.h"
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

	static ConstructorHelpers::FObjectFinder<UPvPAIDifficultyProfile> EasyFinder(TEXT("/Game/AI/DA_Difficulty_Easy"));
	if (EasyFinder.Succeeded()) { AvailableDifficultyProfiles.Add(EasyFinder.Object); }

	static ConstructorHelpers::FObjectFinder<UPvPAIDifficultyProfile> NormalFinder(TEXT("/Game/AI/DA_Difficulty_Normal"));
	if (NormalFinder.Succeeded()) { AvailableDifficultyProfiles.Add(NormalFinder.Object); }

	static ConstructorHelpers::FObjectFinder<UPvPAIDifficultyProfile> HardFinder(TEXT("/Game/AI/DA_Difficulty_Hard"));
	if (HardFinder.Succeeded()) { AvailableDifficultyProfiles.Add(HardFinder.Object); }
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

	if (!IsLocalPlayerController())
	{
		return;
	}

	// Settings that aren't UGameUserSettings' job (sensitivity/volume/invert-Y)
	// -- loaded here so they're correct even if the player never opens the
	// Settings screen this session.
	if (UPvPSettingsSaveGame* Save = Cast<UPvPSettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(UPvPSettingsSaveGame::SlotName, 0)))
	{
		MouseSensitivity = Save->MouseSensitivity;
		bInvertY = Save->bInvertY;

		if (GEngine)
		{
			if (FAudioDeviceHandle AudioDevice = GEngine->GetMainAudioDevice())
			{
				AudioDevice->SetTransientPrimaryVolume(Save->MasterVolume);
			}
		}
	}

	if (AvailableDifficultyProfiles.Num() > 0)
	{
		SelectedDifficultyProfile = AvailableDifficultyProfiles[0];
	}

	MainMenuWidget = CreateWidget<UPvPMainMenuWidget>(this, UPvPMainMenuWidget::StaticClass());
	if (MainMenuWidget)
	{
		MainMenuWidget->OnTutorialRequested.AddDynamic(this, &APvPPlayerController::HandleTutorialMenuRequested);
		MainMenuWidget->OnOneVOneRequested.AddDynamic(this, &APvPPlayerController::HandleOneVOneRequested);
		MainMenuWidget->OnPlayRequested.AddDynamic(this, &APvPPlayerController::HandlePlayRequested);
		MainMenuWidget->OnSettingsRequested.AddDynamic(this, &APvPPlayerController::HandleMenuSettingsRequested);
		MainMenuWidget->OnQuitRequested.AddDynamic(this, &APvPPlayerController::HandleQuitRequested);
		ShowUIOnlyWidget(MainMenuWidget);
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

void APvPPlayerController::ShowUIOnlyWidget(UUserWidget* Widget)
{
	if (!Widget)
	{
		return;
	}

	Widget->AddToViewport(100);

	bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(Widget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void APvPPlayerController::HandleTutorialMenuRequested()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->RemoveFromParent();
	}

	bTutorialIsStandalone = true;

	TutorialWidget = CreateWidget<UPvPTutorialWidget>(this, UPvPTutorialWidget::StaticClass());
	if (TutorialWidget)
	{
		TutorialWidget->OnDismissed.AddDynamic(this, &APvPPlayerController::HandleDismissTutorial);
		ShowUIOnlyWidget(TutorialWidget);
	}
}

void APvPPlayerController::HandleOneVOneRequested()
{
	// Real player-vs-player isn't built in this repo -- see the Milestone 2
	// scope decision in docs/decisions.md. The menu shows this destination
	// (matching the approved title-screen reference) but it's a no-op.
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Orange, TEXT("1v1 is not available in this build."));
	}
}

void APvPPlayerController::HandlePlayRequested()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->RemoveFromParent();
	}

	AIMatchSetupWidget = CreateWidget<UPvPAIMatchSetupWidget>(this, UPvPAIMatchSetupWidget::StaticClass());
	if (AIMatchSetupWidget)
	{
		TArray<UPvPAIDifficultyProfile*> Profiles;
		for (UPvPAIDifficultyProfile* Profile : AvailableDifficultyProfiles)
		{
			Profiles.Add(Profile);
		}
		AIMatchSetupWidget->InitFor(Profiles);
		AIMatchSetupWidget->OnDifficultySelected.AddDynamic(this, &APvPPlayerController::HandleDifficultySelected);
		AIMatchSetupWidget->OnBack.AddDynamic(this, &APvPPlayerController::HandleSetupBack);
		ShowUIOnlyWidget(AIMatchSetupWidget);
	}
}

void APvPPlayerController::HandleMenuSettingsRequested()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->RemoveFromParent();
	}

	SettingsWidget = CreateWidget<UPvPSettingsWidget>(this, UPvPSettingsWidget::StaticClass());
	if (SettingsWidget)
	{
		SettingsWidget->OnBack.AddDynamic(this, &APvPPlayerController::HandleSettingsBack);
		ShowUIOnlyWidget(SettingsWidget);
	}
}

void APvPPlayerController::HandleQuitRequested()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}

void APvPPlayerController::HandleSetupBack()
{
	if (AIMatchSetupWidget)
	{
		AIMatchSetupWidget->RemoveFromParent();
		AIMatchSetupWidget = nullptr;
	}

	if (MainMenuWidget)
	{
		ShowUIOnlyWidget(MainMenuWidget);
	}
}

void APvPPlayerController::HandleSettingsBack()
{
	if (SettingsWidget)
	{
		SettingsWidget->RemoveFromParent();
		SettingsWidget = nullptr;
	}

	if (MainMenuWidget)
	{
		ShowUIOnlyWidget(MainMenuWidget);
	}
}

void APvPPlayerController::HandleDifficultySelected(UPvPAIDifficultyProfile* SelectedProfile)
{
	SelectedDifficultyProfile = SelectedProfile;

	if (AIMatchSetupWidget)
	{
		AIMatchSetupWidget->RemoveFromParent();
		AIMatchSetupWidget = nullptr;
	}

	bTutorialIsStandalone = false;

	TutorialWidget = CreateWidget<UPvPTutorialWidget>(this, UPvPTutorialWidget::StaticClass());
	if (TutorialWidget)
	{
		TutorialWidget->OnDismissed.AddDynamic(this, &APvPPlayerController::HandleDismissTutorial);
		ShowUIOnlyWidget(TutorialWidget);
	}
}

void APvPPlayerController::HandleDismissTutorial()
{
	if (!TutorialWidget)
	{
		return;
	}

	TutorialWidget->RemoveFromParent();
	TutorialWidget = nullptr;

	if (bTutorialIsStandalone)
	{
		// Opened directly from the menu just to read the controls -- go back
		// to the menu rather than starting a match.
		if (MainMenuWidget)
		{
			ShowUIOnlyWidget(MainMenuWidget);
		}
		return;
	}

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
		GM->SetDifficultyProfile(SelectedDifficultyProfile);
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
		MatchResultWidget->OnPlayAgain.AddDynamic(this, &APvPPlayerController::HandlePlayAgain);
		ShowUIOnlyWidget(MatchResultWidget);
	}
}

void APvPPlayerController::HandlePlayAgain()
{
	UGameplayStatics::OpenLevel(this, TEXT("Lvl_PracticeRing"));
}

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PvPPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class UPvPTutorialWidget;
class UPvPHUDWidget;
class UPvPMatchResultWidget;
class UPvPMainMenuWidget;
class UPvPAIMatchSetupWidget;
class UPvPSettingsWidget;
class UPvPAIDifficultyProfile;

/**
 *  Input possession and local HUD ownership. Routing "ready"/format-picker UI
 *  events to the matchmaking subsystem is added in Phase G once
 *  UPvPMatchmakingSubsystem exists.
 *
 *  Concrete (not abstract) -- DefaultMappingContexts is resolved directly via
 *  ConstructorHelpers against IMC_Default rather than requiring a Blueprint
 *  subclass, for the same reason as APvPCharacter.
 */
UCLASS()
class APvPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	APvPPlayerController();

protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Shown first (BeginPlay), before anything else -- see HandlePlayRequested/HandleSettingsRequested/HandleQuitRequested. */
	UPROPERTY()
	TObjectPtr<UPvPMainMenuWidget> MainMenuWidget;

	/** Shown after "Practice vs AI" -- difficulty picker. */
	UPROPERTY()
	TObjectPtr<UPvPAIMatchSetupWidget> AIMatchSetupWidget;

	/** Shown after "Settings" from the main menu. */
	UPROPERTY()
	TObjectPtr<UPvPSettingsWidget> SettingsWidget;

	/** Shown after a difficulty is picked, dismissed via Enter -- see HandleDismissTutorial(). */
	UPROPERTY()
	TObjectPtr<UPvPTutorialWidget> TutorialWidget;

	/** Shown once the tutorial is dismissed and the match starts. */
	UPROPERTY()
	TObjectPtr<UPvPHUDWidget> HUDWidgetInstance;

	/** Shown when the match ends -- see ShowMatchResult(). */
	UPROPERTY()
	TObjectPtr<UPvPMatchResultWidget> MatchResultWidget;

	/** The 3 selectable AI difficulty tiers, resolved via ConstructorHelpers. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TArray<TObjectPtr<UPvPAIDifficultyProfile>> AvailableDifficultyProfiles;

	/** Picked in AIMatchSetupWidget; read by APvPPracticeGameMode::BeginMatch(). Defaults to the first available profile so a direct level launch (bypassing the menu) still works. */
	UPROPERTY(BlueprintReadOnly, Category = "AI")
	TObjectPtr<UPvPAIDifficultyProfile> SelectedDifficultyProfile;

public:
	/** Loaded from UPvPSettingsSaveGame in BeginPlay; applied by APvPCharacter::DoLook. */
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	float MouseSensitivity = 1.f;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	bool bInvertY = false;

protected:

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

	/** Shared by every full-screen menu widget: shows it, sets UI-only input mode with focus on it. */
	void ShowUIOnlyWidget(UUserWidget* Widget);

	UFUNCTION() void HandleTutorialMenuRequested();
	UFUNCTION() void HandleOneVOneRequested();
	UFUNCTION() void HandlePlayRequested();
	UFUNCTION() void HandleMenuSettingsRequested();
	UFUNCTION() void HandleQuitRequested();
	UFUNCTION() void HandleDifficultySelected(UPvPAIDifficultyProfile* SelectedProfile);
	UFUNCTION() void HandleSetupBack();
	UFUNCTION() void HandleSettingsBack();

	/** True when TutorialWidget was opened directly from the main menu (Tutorial destination) rather than as a pre-match step -- dismissing it returns to the menu instead of starting a match. */
	bool bTutorialIsStandalone = false;

	/** Bound to TutorialWidget::OnDismissed. Either returns to the main menu (standalone) or starts the match (pre-match step). */
	UFUNCTION()
	void HandleDismissTutorial();

public:

	/** Called by APvPPracticeGameMode when the match ends. Hides the HUD and shows the result screen. */
	void ShowMatchResult(bool bPlayerWon);

protected:

	/** Bound to MatchResultWidget::OnPlayAgain. Reloads the current level. */
	UFUNCTION()
	void HandlePlayAgain();

};

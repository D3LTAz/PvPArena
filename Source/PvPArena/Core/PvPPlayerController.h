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

	/** Shown on BeginPlay, dismissed via Enter -- see HandleDismissTutorial(). */
	UPROPERTY()
	TObjectPtr<UPvPTutorialWidget> TutorialWidget;

	/** Shown once the tutorial is dismissed and the match starts. */
	UPROPERTY()
	TObjectPtr<UPvPHUDWidget> HUDWidgetInstance;

	/** Shown when the match ends -- see ShowMatchResult(). */
	UPROPERTY()
	TObjectPtr<UPvPMatchResultWidget> MatchResultWidget;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

	/** Bound to TutorialWidget::OnDismissed. Removes TutorialWidget, shows the HUD, and starts the match. */
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

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PvPMatchResultWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPvPOnPlayAgainSignature);

/**
 * Full-screen "Victory" / "Defeat" overlay shown when the match ends
 * (first to APvPPracticeGameMode::KillsToWin). Enter reloads the level via
 * OnPlayAgain -- same NativeOnKeyDown pattern as UPvPTutorialWidget, for the
 * same reason (FInputModeUIOnly routes keys to the focused widget, not to
 * any PlayerController InputComponent binding).
 */
UCLASS()
class UPvPMatchResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitFor(bool bInPlayerWon);

	UPROPERTY(BlueprintAssignable)
	FPvPOnPlayAgainSignature OnPlayAgain;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	bool bPlayerWon = false;
};

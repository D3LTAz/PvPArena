// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PvPTutorialWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPvPOnTutorialDismissedSignature);

/**
 * Full-screen start/tutorial overlay shown before the practice match begins.
 * Pure C++ Slate layout (RebuildWidget) rather than a UMG Widget Blueprint --
 * no visual editor authoring required, matches the rest of this codebase's
 * "everything in C++/data assets, nothing hand-authored in-editor" approach.
 *
 * Enter is handled here (NativeOnKeyDown), not via the PlayerController's
 * InputComponent -- the controller sets FInputModeUIOnly while this is up,
 * which routes keyboard input to the focused widget, not to game input
 * bindings. OnDismissed is what APvPPlayerController actually listens to.
 */
UCLASS()
class UPvPTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPvPOnTutorialDismissedSignature OnDismissed;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PvPTutorialWidget.generated.h"

/**
 * Full-screen start/tutorial overlay shown before the practice match begins.
 * Pure C++ Slate layout (RebuildWidget) rather than a UMG Widget Blueprint --
 * no visual editor authoring required, matches the rest of this codebase's
 * "everything in C++/data assets, nothing hand-authored in-editor" approach.
 * Dismissed by APvPPlayerController on Enter, which then starts the match.
 */
UCLASS()
class UPvPTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
};

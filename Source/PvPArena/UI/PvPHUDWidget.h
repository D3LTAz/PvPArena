// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PvPHUDWidget.generated.h"

class APvPCharacter;

/**
 * Persistent gameplay HUD: health, ammo, current weapon, score. Pure C++
 * Slate layout with lambda-bound live text (Slate re-evaluates these every
 * paint, so no manual per-frame update code is needed) -- shown once the
 * tutorial overlay is dismissed and the match actually starts.
 */
UCLASS()
class UPvPHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Must be called right after CreateWidget(), before AddToViewport(). */
	void InitFor(APvPCharacter* InOwnerCharacter);

protected:
	virtual void NativeConstruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	TWeakObjectPtr<APvPCharacter> OwnerCharacter;

	FText GetHealthText() const;
	FText GetWeaponText() const;
	FText GetAmmoText() const;
	FText GetScoreText() const;
};

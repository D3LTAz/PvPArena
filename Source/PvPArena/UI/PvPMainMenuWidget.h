// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PvPMainMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPvPOnTutorialRequestedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPvPOnOneVOneRequestedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPvPOnPlayRequestedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPvPOnSettingsRequestedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPvPOnQuitRequestedSignature);

/**
 * Boot-to-menu shell, laid out to match the approved title-screen reference
 * (PvP Arena Title Screen.pdf): eyebrow build tag, stacked PVP/ARENA title,
 * numbered destination list, build tag bottom-right. Four destinations per
 * the reference -- Tutorial, 1v1, 1v1 vs AI, Settings -- but "1v1" (real
 * player-vs-player) isn't built in this repo (see the Milestone 2 scope
 * decision in docs/decisions.md), so it's shown disabled rather than
 * omitted, to keep the menu honest about what's actually playable without
 * dropping the reference's intended shape.
 */
UCLASS()
class UPvPMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPvPOnTutorialRequestedSignature OnTutorialRequested;

	UPROPERTY(BlueprintAssignable)
	FPvPOnOneVOneRequestedSignature OnOneVOneRequested;

	UPROPERTY(BlueprintAssignable)
	FPvPOnPlayRequestedSignature OnPlayRequested;

	UPROPERTY(BlueprintAssignable)
	FPvPOnSettingsRequestedSignature OnSettingsRequested;

	UPROPERTY(BlueprintAssignable)
	FPvPOnQuitRequestedSignature OnQuitRequested;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	FReply HandleTutorialClicked();
	FReply HandleOneVOneClicked();
	FReply HandlePlayClicked();
	FReply HandleSettingsClicked();
	FReply HandleQuitClicked();
};

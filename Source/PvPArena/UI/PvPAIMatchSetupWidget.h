// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PvPAIMatchSetupWidget.generated.h"

class UPvPAIDifficultyProfile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPvPOnDifficultySelectedSignature, UPvPAIDifficultyProfile*, SelectedProfile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPvPOnSetupBackSignature);

/**
 * Difficulty picker shown after "Practice vs AI" and before the tutorial/
 * match. One button per profile in AvailableProfiles (set via InitFor
 * before AddToViewport), plus Back.
 */
UCLASS()
class UPvPAIMatchSetupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitFor(const TArray<UPvPAIDifficultyProfile*>& InAvailableProfiles);

	UPROPERTY(BlueprintAssignable)
	FPvPOnDifficultySelectedSignature OnDifficultySelected;

	UPROPERTY(BlueprintAssignable)
	FPvPOnSetupBackSignature OnBack;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY()
	TArray<TObjectPtr<UPvPAIDifficultyProfile>> AvailableProfiles;

	FReply HandleProfileClicked(UPvPAIDifficultyProfile* Profile);
	FReply HandleBackClicked();
};

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PvPMatchFormat.generated.h"

/** Match formats selectable during matchmaking. Deathmatch is always 2 teams. */
UENUM(BlueprintType)
enum class EPvPMatchFormat : uint8
{
	OneVOne,
	TwoVTwo,
	ThreeVThree
};

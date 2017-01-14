// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyMenuGameMode.generated.h"

UCLASS(BlueprintType)
class AStrategyMenuGameMode : public AGameModeBase
{
	GENERATED_UCLASS_BODY()
public:

	// Begin GameMode interface
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, class AActor* StartSpot) override;
	// End GameMode interface
};


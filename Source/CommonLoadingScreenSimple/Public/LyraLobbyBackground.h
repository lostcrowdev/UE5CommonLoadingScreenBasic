// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "LyraLobbyBackground.generated.h"

UCLASS()
class COMMONLOADINGSCREENSIMPLE_API ALyraLobbyBackground : public AActor
{
	GENERATED_BODY()

public:
	ALyraLobbyBackground();

	UFUNCTION(BlueprintPure, Category="Lobby")
	TSoftObjectPtr<UWorld> GetBackgroundLevel() const { return BackgroundLevel; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lobby")
	TSoftObjectPtr<UWorld> BackgroundLevel;
};
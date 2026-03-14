// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraLoadingScreenSubsystem.h"

#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraLoadingScreenSubsystem)

class UUserWidget;

//////////////////////////////////////////////////////////////////////
// ULyraLoadingScreenSubsystem

ULyraLoadingScreenSubsystem::ULyraLoadingScreenSubsystem()
{
}

void ULyraLoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

TSubclassOf<UUserWidget> ULyraLoadingScreenSubsystem::GetLoadingScreenContentWidget() const
{
	return LoadingScreenWidgetClass;
}


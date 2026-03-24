// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "UObject/WeakInterfacePtr.h"

#include "LoadingScreenManager.generated.h"

#define UE_API COMMONLOADINGSCREENBASIC_API

template <typename InterfaceType> class TScriptInterface;

class FSubsystemCollectionBase;
class IInputProcessor;
class ILoadingProcessInterface;
class SWidget;
class UObject;
class UWorld;
struct FFrame;
struct FWorldContext;

/**
 * Dynamic multicast delegate broadcast when the loading screen is shown or hidden.
 * bIsVisible is true when the screen appears and false when it is fully removed
 * (including after the texture-streaming hold has expired).
 * Bind to this from Blueprint instead of calling Remove from Parent on the widget.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingScreenVisibilityChangedBPDelegate, bool, bIsVisible);

/**
 * Handles showing/hiding the loading screen
 */
UCLASS(MinimalAPI)
class ULoadingScreenManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//~USubsystem interface
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UE_API virtual void Deinitialize() override;
	UE_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~End of USubsystem interface

	//~FTickableObjectBase interface
	UE_API virtual void Tick(float DeltaTime) override;
	UE_API virtual ETickableTickType GetTickableTickType() const override;
	UE_API virtual bool IsTickable() const override;
	UE_API virtual TStatId GetStatId() const override;
	UE_API virtual UWorld* GetTickableGameObjectWorld() const override;
	//~End of FTickableObjectBase interface

	UFUNCTION(BlueprintCallable, Category=LoadingScreen)
	FString GetDebugReasonForShowingOrHidingLoadingScreen() const
	{
		return DebugReasonForShowingOrHidingLoadingScreen;
	}

	/** Returns true when the loading screen is currently being shown */
	UFUNCTION(BlueprintCallable, Category=LoadingScreen)
	bool GetLoadingScreenDisplayStatus() const
	{
		return bCurrentlyShowingLoadingScreen;
	}

	/**
	 * Call this from Blueprint once your splash screens have finished playing.
	 * The loading screen will not show until this has been called, allowing
	 * splash screens to play first in packaged builds without being interrupted.
	 * After the first call, subsequent map loads work normally without needing
	 * to call this again.
	 */
	UFUNCTION(BlueprintCallable, Category=LoadingScreen)
	UE_API void AllowLoadingScreen();

	/** Returns true if the loading screen is currently allowed to show */
	UFUNCTION(BlueprintCallable, Category=LoadingScreen)
	UE_API bool IsLoadingScreenAllowed() const { return bLoadingScreenAllowed; }

	/**
	 * Blueprint-assignable delegate fired whenever the loading screen visibility changes.
	 * bIsVisible=true  → the loading screen widget has just been added to the viewport.
	 * bIsVisible=false → the loading screen widget has been fully removed from the viewport,
	 *                    including after the texture-streaming hold delay has expired.
	 *
	 * Bind to this event from Blueprint (e.g. in your HUD or GameMode) and react to
	 * bIsVisible=false here instead of calling Remove from Parent on W_LoadingScreen_Host.
	 * The C++ manager owns the widget lifecycle; driving removal from Blueprint will race
	 * against the streaming hold and leave orphaned widgets on screen.
	 */
	UPROPERTY(BlueprintAssignable, Category=LoadingScreen)
	FOnLoadingScreenVisibilityChangedBPDelegate OnLoadingScreenVisibilityChanged;

	/** Called when the loading screen visibility changes (C++ observers) */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadingScreenVisibilityChangedDelegate, bool);
	FORCEINLINE FOnLoadingScreenVisibilityChangedDelegate& OnLoadingScreenVisibilityChangedDelegate() { return LoadingScreenVisibilityChanged; }

	UE_API void RegisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface);
	UE_API void UnregisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface);
	
private:
	UE_API void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);
	UE_API void HandlePostLoadMap(UWorld* World);

	/** Determines if we should show or hide the loading screen. Called every frame. */
	UE_API void UpdateLoadingScreen();

	/** Returns true if we need to be showing the loading screen. */
	UE_API bool CheckForAnyNeedToShowLoadingScreen();

	/** Returns true if we want to be showing the loading screen (if we need to or are artificially forcing it on for other reasons). */
	UE_API bool ShouldShowLoadingScreen();

	/** Returns true if we are in the initial loading flow before this screen should be used */
	UE_API bool IsShowingInitialLoadingScreen() const;

	/** Shows the loading screen. Sets up the loading screen widget on the viewport */
	UE_API void ShowLoadingScreen();

	/** Hides the loading screen. The loading screen widget will be destroyed */
	UE_API void HideLoadingScreen();

	/** Removes the widget from the viewport */
	UE_API void RemoveWidgetFromViewport();

	/** Prevents input from being used in-game while the loading screen is visible */
	UE_API void StartBlockingInput();

	/** Resumes in-game input, if blocking */
	UE_API void StopBlockingInput();

	UE_API void ChangePerformanceSettings(bool bEnabingLoadingScreen);

private:
	/** Delegate broadcast when the loading screen visibility changes (C++ only) */
	FOnLoadingScreenVisibilityChangedDelegate LoadingScreenVisibilityChanged;

	/** A reference to the loading screen widget we are displaying (if any) */
	TSharedPtr<SWidget> LoadingScreenWidget;

	/** Input processor to eat all input while the loading screen is shown */
	TSharedPtr<IInputProcessor> InputPreProcessor;

	/** External loading processors, components maybe actors that delay the loading. */
	TArray<TWeakInterfacePtr<ILoadingProcessInterface>> ExternalLoadingProcessors;

	/** The reason why the loading screen is up (or not) */
	FString DebugReasonForShowingOrHidingLoadingScreen;

	/** Last logged reason, to avoid spamming identical messages */
	FString LastDebugReasonLogged;

	/** The time when we started showing the loading screen */
	double TimeLoadingScreenShown = 0.0;

	/** The time the loading screen most recently wanted to be dismissed (might still be up due to a min display duration requirement) **/
	double TimeLoadingScreenLastDismissed = -1.0;

	/** The time until the next log for why the loading screen is still up */
	double TimeUntilNextLogHeartbeatSeconds = 0.0;

	/** True when we are between PreLoadMap and PostLoadMap */
	bool bCurrentlyInLoadMap = false;

	/** True when the loading screen is currently being shown */
	bool bCurrentlyShowingLoadingScreen = false;

	/**
	 * Suppresses the loading screen until AllowLoadingScreen() is called from Blueprint.
	 * This allows splash screens to play first in packaged builds.
	 * Defaults to false so the loading screen is held off on startup.
	 */
	bool bLoadingScreenAllowed = false;

	/**
	 * Set to true after AllowLoadingScreen() has been called once.
	 * Subsequent map loads bypass the suppression check so normal
	 * in-game loading screens work without needing to call AllowLoadingScreen() again.
	 */
	bool bHasCompletedInitialStartup = false;
};

#undef UE_API
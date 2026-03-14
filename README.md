# UE5LoadingScreen

A standalone Unreal Engine plugin based on the Common Loading Screen system from Epic's Lyra Starter Game. This plugin strips out Lyra-specific dependencies and provides a clean, reusable loading screen solution for any UE5 project.

## Overview

The original Common Loading Screen plugin is tightly coupled to Lyra's game systems (Experience system, Developer Settings, Primary Asset pipeline). This project decouples it into a standalone plugin that can be dropped into any project without Lyra as a dependency.

### What Was Grabbed From Lyra

- CommonLoadingScreen plugin source (LoadingScreenManager, LoadingProcessTask, CommonLoadingScreenSettings, startup screen)
- LyraLobbyBackground — the actor placed in a level that triggers the loading screen and streams in a background world
- LyraDevelopmentStatics — utility functions for finding classes and PIE worlds (Lyra developer settings functions removed)
- LyraLoadingScreenSubsystem — a game instance subsystem from LyraGame/UI/Foundation that tracks the current loading screen widget class and broadcasts changes across map transitions
- Loading screen widgets (W_LoadingScreen_Host, W_LoadingScreen_Default_Content, W_LoadingScreenReasonDebugText, etc.)

### What Was Changed From Lyra

- Removed LyraDeveloperSettings dependency entirely
- Removed ShouldSkipDirectlyToGameplay, ShouldLoadCosmeticBackgrounds, and CanPlayerBotsAttack functions which relied on Lyra-specific editor settings
- Simplified LyraLobbyBackground to hold a soft world reference directly on the actor — no Primary Asset / Data Asset pipeline required
- Background level is set directly on the placed actor in the level via the Background Level property
- Replaced the LYRAGAME_API macro in LyraLoadingScreenSubsystem with COMMONLOADINGSCREEN_API
- Replaced CommonTextBlock in W_LoadingScreenReasonDebugText with a standard TextBlock to remove the Common UI plugin dependency
- Plugin content reorganized into Blueprints, UI, and Examples folders

---

## Requirements

- Unreal Engine 5.7.4
- Visual Studio 2022

---

## Setup

### 1. Copy the plugin

Copy the Plugins/CommonLoadingScreen folder into your project's Plugins directory.

### 2. Enable the plugin

Open your project, go to Plugins and enable Common Loading Screen.

### 3. Assign a loading screen widget

This is a required step. The plugin does not know what your loading screen UI should look like, so you must assign a widget class before it will display.

Go to Project Settings > Game > Common Loading Screen and assign W_LoadingScreen_Host under Loading Screen Widget.

Without this step the loading screen will fall back to a blank placeholder.

### 4. Place the background actor

Add BP_LobbyBackground to your frontend map. Select it and set the Background Level property in the Details panel to the world you want streamed in as the background during loading.

### 5. Place the main menu actor

Add BP_MainMenu to your frontend map. Select it and set the Level Name property to the level you want to load when the player clicks Play. Defaults to Level001.

---

## How It Works

When the frontend map opens, BP_LobbyBackground fires on BeginPlay, shows the loading screen, and streams in the background sublevel. Once the sublevel is visible the loading screen hides and the main menu appears on top of the 3D background.

When the player clicks Play, BP_MainMenu calls Open Level with the configured level name. The LoadingScreenManager intercepts the map load automatically and shows the loading screen widget for the duration of the load. When the new level finishes loading the screen hides.

The LoadingProcessTask is created at runtime by the Loading Screen Manager to keep the loading screen visible during the level stream. It is not a saved asset — this is expected behavior.

---

## License

This project is based on code originally written by Epic Games, Inc. and is subject to the [Unreal Engine EULA](https://www.unrealengine.com/eula).
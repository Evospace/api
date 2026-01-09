// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MainMenuGui.generated.h"

/**
 * C++ base class for the Main Menu GUI widget.
 * The concrete Blueprint (e.g. MainMenuGui) is expected to implement the actual game loading flow.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class EVOSPACE_API UMainMenuGui : public UUserWidget {
  GENERATED_BODY()

  public:
  /**
   * Implement in Blueprint: starts loading a game/session by its name (save slot name, etc.).
   */
  UFUNCTION(BlueprintImplementableEvent, Category = "Evospace|MainMenu")
  void LoadGameByName(const FString &SaveName);

  /**
   * Optional convenience wrapper to call the Blueprint event from Blueprints or C++.
   */
  UFUNCTION(BlueprintCallable, Category = "Evospace|MainMenu")
  void RequestLoadGameByName(const FString &SaveName) { LoadGameByName(SaveName); }
};

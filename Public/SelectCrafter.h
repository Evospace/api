// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "Evospace/Blocks/BlockWidget.h"
#include "CoreMinimal.h"
#include "Recipe.h"
#include "Public/AbstractCrafter.h"
#include "Public/StaticItem.h"

#include <Dom/JsonObject.h>

#include "SelectCrafter.generated.h"

class USectorProxy;
class AMainPlayerController;
class UInventoryContainer;
class UCoreAccessor;

UCLASS(BlueprintType)
class USelectCrafter : public UAbstractCrafter {
  using Self = USelectCrafter;
  EVO_CODEGEN_INSTANCE(SelectCrafter)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UAbstractCrafter>("SelectCrafter") //@class SelectCrafter : AbstractCrafter
      .endClass();
  }
  GENERATED_BODY()

  public:
  USelectCrafter();
  virtual UCoreAccessor *CoreInit() override;

  virtual void SelectRecipeIndex(APlayerController *pc, int32 i) override;

  virtual void SelectRecipe(APlayerController *pc, const URecipe *recipe) override;

  void RecipeSelectionPostprocess(APlayerController *pc);

  virtual void FindRecipe(APlayerController *pc, const UStaticItem *item) override;

  virtual void ResetRecipe() override;

  virtual void SaveSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr) const override;
  virtual void LoadSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr) override;

  virtual void CopyOnReplace(UBlockLogic *from) override;

  void GenerateOutput(bool bonus);

  void ProcessRecipe();
  bool CanGenerateOutput();
  void ResetCrafterState();

  virtual void BlockBeginPlay() override;

  virtual void SetRenderable(AColumn *sector) override;

  protected:
  virtual void Tick() override;

  public:
  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;
};

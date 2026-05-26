#pragma once

#include "Qr/Loc.h"
#include "Qr/Prototype.h"
#include "StaticChapter.generated.h"

class UStaticQuest;

UCLASS(BlueprintType)
class UStaticChapter : public UPrototype {
  GENERATED_BODY()

  public:
  using Self = UStaticChapter;
  PROTOTYPE_CODEGEN(StaticChapter, StaticChapter)

  virtual void lua_reg(lua_State *L) const override;
  virtual UClass *GetSuperProto() const override { return StaticClass(); }
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual void ModLoadFinalize() override;
  virtual void Release() override;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FLoc Label;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<FLoc> DescriptionParts;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<UStaticQuest *> Quests;

  /** Chapter becomes available when all listed quests are completed. Empty means always available. */
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<UStaticQuest *> RequiredQuests;

  /** True when the chapter is available to the player in the current save. */
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  bool Unlocked = false;

  std::optional<luabridge::LuaRef> OnUnlock;
  std::optional<luabridge::LuaRef> OnComplete;

  private:
  TArray<FName> PendingQuestNames;
  TArray<FName> PendingRequiredQuestNames;
};

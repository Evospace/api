#pragma once
#include "Containers/Array.h"
#include "CoreMinimal.h"
#include "Prototype.h"
#include "Evospace/Ensure.h"
#include "Evospace/JsonObjectLibrary.h"
#include "Modding/Mod.h"
#include "ThirdParty/luabridge/LuaBridge.h"

#include <string>
#include <array>

#include "DB.generated.h"

class UPrototype;
namespace evo {
class LegacyLuaState;
class ModLoadingLuaState;
} // namespace evo
class UDB;

class UJsonObjectLibrary;
class UMod;
class ModLoadingContext;

class ModLoadingContext {
  public:
  ModLoadingContext(evo::LegacyLuaState *lua, UDB *);
  ~ModLoadingContext() = default;

  ModLoadingContext(const ModLoadingContext &c) = delete;
  ModLoadingContext &operator=(const ModLoadingContext &c) = delete;

  evo::LegacyLuaState *lua_state;

  UDB *loader;
};

enum class ModTickLoadStatus {
  NotChecked,
  Ok,
  Error,
  Download
};

UCLASS()
class EVOSPACE_API UDB : public UInstance {
  GENERATED_BODY()
  public:
  EVO_CODEGEN_INSTANCE(DB)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UDB, UInstance>("DB") //@class DB : Instance
      //direct:
      //---Register Prototype in DB
      //---@param proto Prototype Prototype to register
      //function DB:reg(proto) end
      .addFunction("reg", [](UDB *self, UPrototype *proto) {
        if (ensure_log(proto, FString("Trying to register nullptr from ") + UTF8_TO_TCHAR(self->mCurrentMod->mName.data()))) {
          self->RegisterPrototype(self->mCurrentMod, proto);
        }
      })
      .addFunction("from_table", [](UDB *self, const luabridge::LuaRef &table) {
        self->mJsonObjectLibrary->ObjectFromTable(self->mCurrentMod, table);
      })
      //direct:
      //---Register mod table
      //---@param table table Mod table
      //function DB:mod(table) end
      .addFunction("mod", [](UDB *self, const luabridge::LuaRef &table) {
        self->ModInitTable(table);
      })
      .addFunction("objects", [](UDB *self) {
        return self->GetPrototypes();
      })
      .endClass();
  }

  public:
  UDB();

  UPROPERTY(VisibleAnywhere)
  UJsonObjectLibrary *mJsonObjectLibrary = nullptr;

  UTexture2D *FindIco(const FName &name) const;

  UPROPERTY(VisibleAnywhere)
  TMap<FName, UTexture2D *> IconTextures;

  UPROPERTY(VisibleAnywhere)
  TArray<int64> mSubscribedIds;

  void LoadDirectory(const FString &path, const FString &dir);
  bool LoadSync();

  std::vector<UObject *> GetPrototypes() const;

  bool SelectLocalization(const FString &locale);

  TArray<FString> GetMods();

  void Release();

  std::optional<luabridge::LuaRef> lastRegisteredMod;

  void ModInitTable(const luabridge::LuaRef &table);

  static UDB *GetMainGameModLoader();

  void RegisterPrototype(const UMod *owner, UPrototype *proto) const;

  void LoadSettings();
  void SaveSettings();

  bool AllSequence(ModLoadingContext &cotext);

  int VanillaJsonCount = 0;

  UPROPERTY()
  UMod *mCurrentMod = nullptr;

  UPROPERTY()
  TArray<UMod *> mMods;

  ModTickLoadStatus current_mod_status = ModTickLoadStatus::NotChecked;

  UPROPERTY()
  TArray<FString> error_log;

  UFUNCTION(BlueprintCallable)
  int32 GetPhase() const { return phase; }

  private:
  static bool LoadLoc(const FString &path, const FString &locale, bool isSource);

  bool LoadVanillaPrepareMods(ModLoadingContext &context);
  bool LuaTickCaller(ModLoadingContext &context, const std::string &function_name, int32 seq);
  bool SubscriptionLoading(ModLoadingContext &context);
  bool Init(ModLoadingContext &context, int32 seq);
  bool PostInit(ModLoadingContext &context, int32 seq);
  bool PreInit(ModLoadingContext &context, int32 seq);
  bool ResearchPostprocess(ModLoadingContext &context);
  bool LuaPostprocess(ModLoadingContext &context);
  bool CollectingItems(ModLoadingContext &context);
  bool ModContentLoad(int phase);

  UPROPERTY()
  TArray<UMod *> lostMods;

  UPROPERTY(VisibleAnywhere)
  int current_mod = 0;

  UPROPERTY(VisibleAnywhere)
  int phase = 0;
};

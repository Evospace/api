// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Evospace/SerializableJson.h"
#include "ThirdParty/luabridge/luabridge.h"

#include <Dom/JsonObject.h>

#include <optional>
#include <string>
#include <vector>

#include "Mod.generated.h"

class ModLoadingContext;

UCLASS(BlueprintType)
class UMod : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:
  UMod() {
  }

  virtual ~UMod() override {
  }

  std::string mName;

  std::string mVersion;

  std::string mTitle;

  std::string mEvospaceVersion;

  std::string mCreatedBy;

  std::string mDescription;

  std::string mAuthor;

  FString mPath;

  bool mEnabled = true;

  bool loaded_without_errors = false;

  std::vector<std::string> dependencies;

  std::optional<luabridge::LuaRef> mod_table;

  void lua_state_close();

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  bool DeserializeFromDirectory(const FString &s, ModLoadingContext &context);

  UFUNCTION(BlueprintCallable)
  FString Name() const { return UTF8_TO_TCHAR(mName.data()); }

  UFUNCTION(BlueprintCallable)
  FString Version() const { return UTF8_TO_TCHAR(mVersion.data()); }

  UFUNCTION(BlueprintCallable)
  FString Title() const { return UTF8_TO_TCHAR(mTitle.data()); }

  UFUNCTION(BlueprintCallable)
  FString EvospaceVersion() const {
    return UTF8_TO_TCHAR(mEvospaceVersion.data());
  }

  UFUNCTION(BlueprintCallable)
  FString CreatedBy() const { return UTF8_TO_TCHAR(mCreatedBy.data()); }

  UFUNCTION(BlueprintCallable)
  FString Description() const { return UTF8_TO_TCHAR(mDescription.data()); }

  UFUNCTION(BlueprintCallable)
  FString Author() const { return UTF8_TO_TCHAR(mAuthor.data()); }

  UFUNCTION(BlueprintCallable)
  bool Enabled() const { return mEnabled; }

  UFUNCTION(BlueprintCallable)
  bool LoadedWithoutErrors() const { return loaded_without_errors; }

  UFUNCTION(BlueprintCallable)
  TArray<FString> Dependencies() const {
    TArray<FString> arr;
    for (auto &a : dependencies) {
      arr.Add(UTF8_TO_TCHAR(mName.data()));
    }
    return arr;
  }
};

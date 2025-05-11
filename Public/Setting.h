#pragma once
#include "Evospace/JsonHelper.h"
#include "Evospace/Misc/AssetOwner.h"
#include "Evospace/Misc/EvoConverter.h"

#include <Public/Prototype.h>
#include "Setting.generated.h"

UCLASS()
class USetting : public UPrototype {
  using Self = USetting;
  GENERATED_BODY()
  EVO_OWNER(Setting)
  EVO_CODEGEN_DB(Setting, Setting)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<USetting, UPrototype>("Setting") //@class Setting : Prototype
      .addProperty("restart", &Self::Restart) //@field boolean
      .addProperty("set_action", &Self::SetAction) //@field function
      .addProperty("label", &Self::Label) //@field string
      .addProperty("type", &Self::Type) //@field string
      .addProperty("category", &Self::Category) //@field string
      .addProperty("string_value", &Self::StringValue) //@field string
      .addProperty("default_string_value", &Self::DefaultStringValue) //@property string
      .addProperty("string_options", &Self::StringOptions) //@field string[]
      .addProperty("int_value", &Self::IntValue) //@field integer
      .addProperty("int_default_value", &Self::DefaultIntValue) //@field integer
      .addProperty("max_value", &Self::MaxValue) //@field integer
      .addProperty("min_value", &Self::MinValue) //@field integer
      .addProperty("key_binding", EVO_NAME_GET_SET(mKeyBinging)) //@field string
      .addProperty(
        "default_key", [](USetting *self) //@field string
        -> std::string {
          auto & chord = self->mDefaultKey;
          FString s = FString("") + (chord.bAlt ? "Alt+" : "") + (chord.bCtrl ? "Ctrl+" : "") + (chord.bShift ? "Shift+" : "") + (chord.bCmd ? "Cmd+" : "") + chord.Key.ToString();
          return TCHAR_TO_UTF8(*s); },
        [](USetting *self, std::string_view s) { self->mDefaultKey = json_helper::ParseKeystrokeToKeyChord(UTF8_TO_TCHAR(s.data())); })
      .addStaticFunction("update_widgets", &Self::UpdateSettingsWidgets)
      .endClass();
  }

  public:
  FString GetStringValue() const {
    return UTF8_TO_TCHAR(StringValue.data());
  }

  FString GetCategory() const {
    return UTF8_TO_TCHAR(Category.data());
  }

  virtual void Release() override;
  bool TryMakeAction();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool IsValueChanged() const;

  UFUNCTION(BlueprintCallable)
  void MakeAction();

  UFUNCTION(BlueprintCallable)
  void ApplySetting();

  UFUNCTION(BlueprintCallable)
  void ResetValue();

  FString GetLabel() const;

  void ApplyInputKey();

  virtual void OnObjectFromTable() override;

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  static void UpdateSettingsWidgets();

  public:
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  FInputChord mKey;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  FInputChord mDefaultKey;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  FName mKeyBinging;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  bool Restart = false;

  std::string DefaultStringValue;
  std::string StringValue;

  std::string Category;
  std::string Type;

  int DefaultIntValue = 0;
  int IntValue = 0;
  bool BoolValue = false;

  std::string Label;

  int MinValue = 0;
  int MaxValue = 0;
  std::vector<std::string> StringOptions;

  std::optional<luabridge::LuaRef> SetAction;
  std::optional<luabridge::LuaRef> GetAction;
};
#pragma once
#include "MainGameInstance.h"
#include "Evospace/JsonHelper.h"
#include "Qr/CommonConverter.h"

#include <Public/Prototype.h>
#include "Setting.generated.h"

class UFilterableWidget;

UCLASS()
class USetting : public UPrototype {
  using Self = USetting;
  GENERATED_BODY()
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
      .addProperty("key_binding", QR_NAME_GET_SET(mKeyBinging)) //@field string
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
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
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

  UFUNCTION(BlueprintCallable)
  static USetting *GetSettingBP(const FString &name) {
    auto cdo = Cast<UPrototype>(USetting::StaticClass()->GetDefaultObject());
    auto o = cdo->get_or_register(name, *UMainGameInstance::Singleton->mJsonObjectLibrary);
    return Cast<USetting>(o);
  }

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

  UFUNCTION(BlueprintCallable)
  UFilterableWidget *CreateWidget(UWidget *owner) const;

  std::string DefaultStringValue;
  std::string StringValue;

  std::string Category;
  std::string Type;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  int DefaultIntValue = 0;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  int IntValue = 0;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  bool BoolValue = false;

  std::string Label;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  int MinValue = 0;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  int MaxValue = 0;

  std::vector<std::string> StringOptions;

  std::optional<luabridge::LuaRef> SetAction;
  std::optional<luabridge::LuaRef> GetAction;
};
#include "Setting.h"

#include "MainGameInstance.h"
#include "Evospace/Gui/ModConfigWidget.h"
#include "Evospace/Gui/SettingWidgetConstructors.h"
#include "GameFramework/InputSettings.h"
#include "Modding/LuaState.h"

class UModConfigControlBase;
struct FInputActionKeyMapping;
class UInputSettings;

void USetting::Release() {
  SetAction.reset();
  GetAction.reset();
}

bool USetting::TryMakeAction() {
  IntValue = std::clamp(IntValue, MinValue, MaxValue);

  if (SetAction.has_value()) {
    MakeAction();
    return true;
  }

  return false;
}

bool USetting::IsValueChanged() const {
  if (Type == "Key") {
    return mKey != mDefaultKey;
  }
  if (Type == "Slider" || Type == "Int") {
    return DefaultIntValue != IntValue;
  }
  if (Type == "String" || Type == "Buttons") {
    return DefaultStringValue != StringValue;
  }
  return false;
}

void USetting::MakeAction() {
  evo::LuaState::CallAndHandleError(SetAction, this);
}

void USetting::ApplySetting() {
  if (Type == "Key") {
    ApplyInputKey();
  }

  if (Type != "Buttons" && SetAction.has_value()) {
    MakeAction();
  }
}

void USetting::ResetValue() {
  if (Type == "Key") {
    mKey = mDefaultKey;
  } else if (Type == "String" || Type == "Buttons") {
    StringValue = DefaultStringValue;
  } else if (Type == "Slider" || Type == "Int") {
    IntValue = DefaultIntValue;
  }

  ApplySetting();
}

FString USetting::GetLabel() const {
  auto gi = UMainGameInstance::Singleton;
  return gi->GetLocalizedKeyTable(UTF8_TO_TCHAR(Label.data()), TEXT("ui"));
}

void USetting::ApplyInputKey() {
  UInputSettings *InputSettings = const_cast<UInputSettings *>(GetDefault<UInputSettings>());
  if (!InputSettings) return;

  bool axis = true;
  {
    TArray<FInputActionKeyMapping> ActionMappings = InputSettings->GetActionMappings();
    for (int32 i = 0; i < ActionMappings.Num(); i++) {
      if (ActionMappings[i].ActionName == mKeyBinging) {
        InputSettings->RemoveActionMapping(ActionMappings[i]);
        axis = false;
      }
    }
  }
  if (axis) {
    TArray<FInputAxisKeyMapping> Axis = InputSettings->GetAxisMappings();
    for (int32 i = 0; i < Axis.Num(); i++) {
      if (Axis[i].AxisName == mKeyBinging && static_cast<int>(Axis[i].Scale) == DefaultIntValue) {
        InputSettings->RemoveAxisMapping(Axis[i]);
      }
    }
  }

  if (axis) {
    FInputAxisKeyMapping NewMapping;
    NewMapping.AxisName = mKeyBinging;
    NewMapping.Key = mKey.Key;
    NewMapping.Scale = DefaultIntValue;

    LOG(INFO_LL) << "Apply key binding " << NewMapping.Key.ToString() << " for " << mKeyBinging;

    InputSettings->AddAxisMapping(NewMapping);
  } else {
    FInputActionKeyMapping NewMapping;
    NewMapping.ActionName = mKeyBinging;
    NewMapping.Key = mKey.Key;
    NewMapping.bShift = mKey.bShift;
    NewMapping.bCtrl = mKey.bCtrl;
    NewMapping.bAlt = mKey.bAlt;
    NewMapping.bCmd = mKey.bCmd;

    FString s = FString("") + (NewMapping.bAlt ? "Alt+" : "") + (NewMapping.bCtrl ? "Ctrl+" : "") + (NewMapping.bShift ? "Shift+" : "") + (NewMapping.bCmd ? "Cmd+" : "") + NewMapping.Key.ToString();
    LOG(INFO_LL) << "Apply key binding " << s << " for " << mKeyBinging;

    InputSettings->AddActionMapping(NewMapping);
  }
  InputSettings->SaveKeyMappings();
}

void USetting::OnObjectFromTable() {
  mKey = mDefaultKey;
  StringValue = DefaultStringValue;
  IntValue = DefaultIntValue;
}

bool USetting::SerializeJson(TSharedPtr<FJsonObject> json) {
  if (mKey != mDefaultKey)
    json_helper::TrySet(json, "Key", mKey);

  if (StringValue != DefaultStringValue)
    json_helper::TrySet(json, "StringValue", StringValue);

  if (IntValue != DefaultIntValue)
    json_helper::TrySet(json, "IntValue", IntValue);

  return true;
}

bool USetting::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryGet(json, "Key", mKey);
  json_helper::TryGet(json, "StringValue", StringValue);
  json_helper::TryGet(json, "IntValue", IntValue);
  return true;
}

void USetting::UpdateSettingsWidgets() {
  for (TObjectIterator<UModConfigControlBase> it; it; ++it) {
    if (it->IsTemplate() || !it->mSetting) {
      continue;
    }
    it->ReloadControl();
    it->ReloadControlBP();
  }
}

UFilterableWidget *USetting::CreateWidget(UWidget *owner) const {
  UFilterableWidget *wid = nullptr;
  if (Type == "Int") {
    wid = SettingWidgetConstructors::CreateInt(owner, this, this->GetStringValue(), this->IntValue);
  } else if (this->Type == "Buttons") {
    wid = SettingWidgetConstructors::CreateButtons(owner, this, this->GetStringValue(), this->IntValue);
  } else if (this->Type == "Slider") {
    wid = SettingWidgetConstructors::CreateSlider(owner, this, this->GetStringValue(), this->IntValue);
  } else if (this->Type == "Key") {
    wid = SettingWidgetConstructors::CreateKey(owner, this, this->GetStringValue(), this->mKey);
  } else if (this->Type == "String") {
    wid = SettingWidgetConstructors::CreateSelectBox(owner, this, this->GetStringValue(), UTF8_TO_TCHAR(this->StringValue.data()));
  } else if (this->Type == "Bool") {
    wid = SettingWidgetConstructors::CreateBool(owner, this, this->GetStringValue(), this->BoolValue);
  } else if (this->Type == "") { // Type is static so this is some old setting
    LOG(WARN_LL) << "Setting " << this->GetName() << " is not used";
  }

  return wid;
}
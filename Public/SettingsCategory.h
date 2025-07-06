// #pragma once
// #include "Qr/Prototype.h"
// #include "Evospace/Misc/EvoConverter.h"
// #include "SettingsCategory.generated.h"
//
// class USetting;
//
// UCLASS()
// class USettingsCategory : public UPrototype {
//   using Self = USettingsCategory;
//   GENERATED_BODY()
//   PROTOTYPE_CODEGEN(SettingsCategory, SettingsCategory)
//   virtual void lua_reg(lua_State *L) const override {
//     luabridge::getGlobalNamespace(L)
//     .deriveClass<USettingsCategory, UPrototype>("SettingsCategory") //@class SettingsCategory : Prototype
//     .addProperty("settings", EVO_ARRAY_GET_SET(Settings)) //@field Setting[]
//     .addProperty("label", &Self::Label) //@field string
//     .addProperty("file_name", &Self::FileName) //@field string
//     .addFunction("add_setting", [](Self * self, USetting * setting) { self->Settings.Add(setting);  })
//     .endClass();
//   }
// public:
//
//   UPROPERTY(VisibleAnywhere)
//   TArray<USetting *> Settings;
//
//   std::string Label;
//   std::string FileName;
// };
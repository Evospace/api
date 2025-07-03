#pragma once

#include "ThirdParty/lua/lua.h"
#include "ThirdParty/luabridge/luabridge.h"
#include "Evospace/Common.h"
#include "Public/Dimension.h"
#include "Public/MainGameInstance.h"
#include "Evospace/Blocks/BlockActor.h"
#include "CommonConverter.h"
#include "Loc.h"
#include "GuiTextHelper.h"
#include "Public/ItemData.h"
#include "Sound/SoundClass.h"

class UIcoGenerator;
struct FItemData;

inline void registerComponentClasses(lua_State *L) {
  using namespace luabridge;

  getGlobalNamespace(L)
    .beginClass<UObject>("Object")
    .addStaticFunction(
      "find", +[](std::string_view name) { return FindObject<UObject>(MainGameOwner<UObject>::Get(), UTF8_TO_TCHAR(name.data())); })
    .addStaticFunction("cast", [](UObject *obj) { return obj; })
    .addProperty("name", [](const UObject *self) { return qr::to_string(self->GetName()); })
    .addFunction("get_class", [](UObject *obj) { return obj->GetClass(); })
    .addStaticFunction(
      "get_class", +[]() { return UObject::StaticClass(); })
    .addFunction("__tostring", [](UObject *obj) -> std::string { return TCHAR_TO_UTF8(*("(Object: " + obj->GetName() + ")")); })
    .endClass();

  getGlobalNamespace(L)
    .beginClass<UIcoGenerator>("IcoGenerator")
    //.addStaticFunction("combine", &UIcoGenerator::combine)
    .endClass();

  getGlobalNamespace(L)
    .deriveClass<UTexture2D, UObject>("Texture")
    .addStaticFunction("find", &evo::LuaState::FindTexture)
    .addProperty("name", [](const UTexture2D *self) { return qr::to_string(self->GetName()); })
    .endClass();

  getGlobalNamespace(L)
    .deriveClass<UMaterialInterface, UObject>("Material")
    .addProperty("name", [](const UMaterialInterface *self) { return qr::to_string(self->GetName()); })
    .addStaticFunction("load", &evo::LuaState::LuaLoadObject<UMaterialInterface>)
    .endClass();

  getGlobalNamespace(L)
    .deriveClass<USoundClass, UObject>("SoundClass")
    .addStaticFunction("load", &evo::LuaState::LuaLoadObject<USoundClass>)
    .addProperty("name", [](const USoundClass *self) { return qr::to_string(self->GetName()); })
    .addProperty(
      "volume", [](USoundClass *self) //@field numeric
      { return self->Properties.Volume; },
      [](USoundClass *self, float volume) { self->Properties.Volume = volume; })
    .endClass();

  getGlobalNamespace(L)
    .deriveClass<UStaticMesh, UObject>("StaticMesh")
    .addStaticFunction("load", &evo::LuaState::LuaLoadObject<UStaticMesh>)
    .addProperty("name", [](const UStaticMesh *self) { return qr::to_string(self->GetName()); })
    .endClass();

  getGlobalNamespace(L)
    .deriveClass<UClass, UObject>("Class")
    .addStaticFunction("find", &evo::LuaState::FindClass)
    .addStaticFunction("load", &evo::LuaState::LoadClass)
    .addStaticFunction("cast", [](UObject *obj) { return Cast<UClass>(obj); })
    .addProperty("name", [](const UClass *self) { return qr::to_string(self->GetName()); })
    .addFunction("is_child_of", [](const UClass *self, const UClass *other) {
      return self->IsChildOf(other);
    })
    .endClass();

  getGlobalNamespace(L)
    .beginClass<APlayerController>("PlayerController")
    .endClass();

  getGlobalNamespace(L)
    .beginClass<FKto>("Kto")
    .addStaticFunction("new", [](std::string_view key, std::string_view table) {
      auto kt = FKto();
      kt.Key = UTF8_TO_TCHAR(key.data());
      kt.Table = UTF8_TO_TCHAR(table.data());
      return kt;
    })
    .addFunction("get", [](FKto *kto) -> std::string {
      const auto gi = UMainGameInstance::Singleton;
      return TCHAR_TO_UTF8(*gi->GetLocalizedKeyTable(kto->Key, kto->Table));
    })
    .endClass();

  getGlobalNamespace(L)
    .beginClass<FLoc>("LocData")
    .addStaticFunction("set", [](std::string_view key, std::string_view value) {
      UMainGameInstance::SetLocalizationData(UTF8_TO_TCHAR(key.data()), UTF8_TO_TCHAR(value.data()));
    })
    .endClass();

  getGlobalNamespace(L)
    .beginClass<FLoc>("Loc")
    .addStaticFunction("new", [](std::string_view key, std::string_view table) {
      auto kt = FLoc();
      kt.Data.Key = UTF8_TO_TCHAR(key.data());
      kt.Data.Table = UTF8_TO_TCHAR(table.data());
      return kt;
    })
    .addStaticFunction("get", [](std::string_view key, std::string_view table) -> std::string {
      const auto gi = UMainGameInstance::Singleton;
      return TCHAR_TO_UTF8(*gi->GetLocalizedKeyTable(TCHAR_TO_UTF8(key.data()), TCHAR_TO_UTF8(table.data())));
    })
    .addStaticFunction("gui_number", [](float value) -> std::string {
      return TCHAR_TO_UTF8(*UGuiTextHelper::FloatToGuiCount(value, 1, false, 1).ToString());
    })
    .addFunction("get", [](FLoc *loc) -> std::string {
      const auto gi = UMainGameInstance::Singleton;
      return TCHAR_TO_UTF8(*gi->GetLocalizedLoc(*loc));
    })
    .endClass();

  FItemData::lua_reg(L);

  getGlobalNamespace(L)
    .beginClass<UActorComponent>("ActorComponent")
    .addProperty("name", [](const UActorComponent *self) { return qr::to_string(self->GetName()); })
    .endClass();

  getGlobalNamespace(L)
    .deriveClass<AActor, UObject>("Actor")
    .addFunction(
      "add_actor_component",
      [](AActor *actor, UClass *componentClass, const std::string &componentName) {
        if (actor && componentClass) {
          UActorComponent *newComponent = NewObject<UActorComponent>(actor, componentClass, FName(UTF8_TO_TCHAR(componentName.data())));
          if (newComponent) {
            newComponent->RegisterComponent();
            actor->AddInstanceComponent(newComponent);
            return newComponent;
          }
        }
        return static_cast<UActorComponent *>(nullptr);
      })
    .addFunction("set_field_object", [](AActor *actor, std::string_view field_name, UObject *object) {
      if (ensure(actor && field_name != "")) {
        auto prop = FindFProperty<FObjectPropertyBase>(actor->GetClass(), UTF8_TO_TCHAR(field_name.data()));
        if (prop) {
          prop->SetObjectPropertyValue_InContainer(actor, object);
        }
        return true;
      }
      return false;
    })
    .endClass();

  getGlobalNamespace(L)
    .deriveClass<ABlockActor, AActor>("BlockActor")
    .addProperty("logic", &ABlockActor::mBlockLogic)
    .endClass();
}

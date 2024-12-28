#pragma once
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ThirdParty/lua/lua.h"
#include "ThirdParty/luabridge/luabridge.h"
#include "Evospace/Common.h"
#include "Evospace/Dimension.h"
#include "Evospace/MainGameInstance.h"
#include "Evospace/Blocks/BlockActor.h"
#include "Evospace/Item/ItemData.h"
#include "Evospace/Item/ItemLogic.h"
#include "Evospace/Item/Recipe.h"

class UIcoGenerator;

inline void registerComponentClasses(lua_State *L) {
  using namespace luabridge;

  getGlobalNamespace(L)
    .beginClass<UObject>("Object")
    .addStaticFunction(
      "find", +[](std::string_view name) { return FindObject<UObject>(MainGameOwner<UObject>::Get(), UTF8_TO_TCHAR(name.data())); })
    .addStaticFunction("cast", [](UObject *obj) { return obj; })
    .addFunction("get_name", [](UObject *obj) { return std::string(TCHAR_TO_UTF8(*obj->GetName())); })
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
    .endClass();

  getGlobalNamespace(L)
    .deriveClass<UMaterialInterface, UObject>("Material")
    .addStaticFunction("load", &evo::LuaState::FindMaterial)
    .endClass();

  getGlobalNamespace(L)
    .deriveClass<UStaticMesh, UObject>("StaticMesh")
    .endClass();

  getGlobalNamespace(L)
    .deriveClass<UClass, UObject>("Class")
    .addStaticFunction("find", &evo::LuaState::FindClass)
    .addStaticFunction("load", &evo::LuaState::LoadClass)
    .addStaticFunction("cast", [](UObject *obj) { return Cast<UClass>(obj); })
    .addFunction(
      "get_name", +[](const UClass *cl) { return std::string(TCHAR_TO_UTF8(*cl->GetName())); })
    .addFunction("is_child_of", [](const UClass *self, const UClass *other) {
      return self->IsChildOf(other);
    })
    .endClass();

  getGlobalNamespace(L)
    .beginClass<APlayerController>("PlayerController")
    .endClass();

  getGlobalNamespace(L)
    .beginClass<FKeyTableObject>("Loc")
    .addStaticFunction("new", [](std::string_view key, std::string_view table) {
      auto kt = FKeyTableObject();
      kt.Key = UTF8_TO_TCHAR(key.data());
      kt.Table = UTF8_TO_TCHAR(table.data());
      return kt;
    })
    .addStaticFunction("new_param", [](std::string_view key, std::string_view table, float param) {
      auto kt = FKeyTableObject();
      kt.Key = UTF8_TO_TCHAR(key.data());
      kt.Table = UTF8_TO_TCHAR(table.data());
      kt.Value = param;
      kt.Args = 1;
      return kt;
    })
    .endClass();

  getGlobalNamespace(L)
    .beginClass<FItemData>("ItemData")
    .addStaticFunction(
      "new_empty", +[]() { return FItemData(); })
    .addStaticFunction(
      "new",
      +[](UStaticItem *item, int64 count) { return FItemData(item, count); })
    .addStaticFunction(
      "new_zero", +[](UStaticItem *item) { return FItemData(item, 0); })
    .addProperty("count", &FItemData::mValue)
    .addProperty("item", &FItemData::mItem)
    .endClass();

  getGlobalNamespace(L)
    .beginClass<UActorComponent>("ActorComponent")
    .addProperty(
      "name",
      [](UActorComponent *comp) {
        return std::string(TCHAR_TO_UTF8(*comp->GetName()));
      })
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

  getGlobalNamespace(L)
    .deriveClass<UMainGameInstance, UObject>("Game")
    .addStaticFunction("set_localization", [](std::string_view s) {
      UMainGameInstance::ChangeLocalization(UTF8_TO_TCHAR(s.data()));
    })
    .endClass();
}

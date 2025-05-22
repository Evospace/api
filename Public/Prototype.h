// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "ThirdParty/luabridge/LuaBridge.h"
#include "StaticLogger.h"
#include "Evospace/JsonObjectLibrary.h"
#include "Evospace/MainGameOwner.h"
#include "Evospace/SerializableJson.h"

#include "prototype.generated.h"

class UMod;
class Base;

#define __EVO_COMMON_CODEGEN(type)                             \
  public:                                                      \
  virtual UClass *lua_reg_type() {                             \
    return U##type::StaticClass();                             \
  }                                                            \
  static U##type *lua_codegen_cast(UObject *parent_inst) {     \
    return Cast<U##type>(parent_inst);                         \
  }                                                            \
  virtual luabridge::LuaRef to_luaref(lua_State *L) override { \
    return luabridge::LuaRef(L, this);                         \
  }

/*.addStaticFunction("get", &evo::DB::get<U##type>) */ /*.addStaticFunction("get_derived", &evo::DB::get_derived<U##type>) */
#define EVO_CODEGEN_DB(type, topmost_not_prototype)                                                                                                                                     \
  __EVO_COMMON_CODEGEN(type)                                                                                                                                                            \
  virtual UPrototype *get_or_register(const FString &obj_name, IRegistrar &registry) override {                                                                                         \
    return _get_or_register<U##topmost_not_prototype, U##type>(obj_name, registry);                                                                                                     \
  }                                                                                                                                                                                     \
  virtual void lua_reg_internal(lua_State *L) const override {                                                                                                                          \
    LOG(INFO_LL) << "Registering lua prototype " << TEXT(#type);                                                                                                                        \
    luabridge::getGlobalNamespace(L)                                                                                                                                                    \
      .beginClass<U##type>(#type)                                                                                                                                                       \
      .addStaticFunction(                                                                                                                                                               \
        "find", +[](std::string_view name) { return UDbStorage::Singleton->Find<U##topmost_not_prototype>(UTF8_TO_TCHAR(name.data())); })                                               \
      .addStaticFunction("cast", &U##type::lua_codegen_cast)                                                                                                                            \
      .addStaticFunction("get_class", []() { return U##type::StaticClass(); })                                                                                                          \
      .addFunction("__tostring", &U##type::ToString)                                                                                                                                    \
      .addFunction("__eq", &U##type::LuaEquals)                                                                                                                                         \
      .endClass();                                                                                                                                                                      \
    if (!U##type::StaticClass()->HasAnyClassFlags(CLASS_Abstract)) {                                                                                                                    \
      luabridge::getGlobalNamespace(L)                                                                                                                                                  \
        .beginClass<U##type>(#type)                                                                                                                                                     \
        .addStaticFunction(                                                                                                                                                             \
          "new", +[](std::string_view newName) { return NewObject<U##type>(UDbStorage::Singleton->GetOuter(U##topmost_not_prototype::StaticClass()), UTF8_TO_TCHAR(newName.data())); }) \
        .endClass();                                                                                                                                                                    \
    }                                                                                                                                                                                   \
  }                                                                                                                                                                                     \
  bool LuaEquals(const UObject &other) const {                                                                                                                                          \
    return this == &other;                                                                                                                                                              \
  }

#define EVO_CODEGEN_INSTANCE_ALIAS(type, alias)                                                                                          \
  __EVO_COMMON_CODEGEN(type)                                                                                                             \
  virtual void lua_reg_internal(lua_State *L) const override {                                                                           \
    LOG(INFO_LL) << "Registering lua instance " << TEXT(#alias);                                                                         \
    luabridge::getGlobalNamespace(L)                                                                                                     \
      .beginClass<U##type>(#alias)                                                                                                       \
      .addStaticFunction("cast", &U##type::lua_codegen_cast)                                                                             \
      .addStaticFunction("get_class", []() { return U##type::StaticClass(); })                                                           \
      .addFunction("__tostring", &U##type::ToString)                                                                                     \
      .addFunction("__eq", &U##type::LuaEquals)                                                                                          \
      .endClass();                                                                                                                       \
    if (!U##type::StaticClass()->HasAnyClassFlags(CLASS_Abstract)) {                                                                     \
      luabridge::getGlobalNamespace(L)                                                                                                   \
        .beginClass<U##type>(#alias)                                                                                                     \
        .addStaticFunction(                                                                                                              \
          "new", +[](UInstance *parent, std::string_view newName) { return NewObject<U##type>(parent, UTF8_TO_TCHAR(newName.data())); }) \
        .addStaticFunction(                                                                                                              \
          "new_simple", +[]() { return NewObject<U##type>(); })                                                                          \
        .endClass();                                                                                                                     \
    }                                                                                                                                    \
  }                                                                                                                                      \
  bool LuaEquals(const UObject &other) const {                                                                                           \
    return this == &other;                                                                                                               \
  }

#define EVO_CODEGEN_INSTANCE(type)                                                                                                       \
  __EVO_COMMON_CODEGEN(type)                                                                                                             \
  virtual void lua_reg_internal(lua_State *L) const override {                                                                           \
    LOG(INFO_LL) << "Registering lua instance " << TEXT(#type);                                                                          \
    luabridge::getGlobalNamespace(L)                                                                                                     \
      .beginClass<U##type>(#type)                                                                                                        \
      .addStaticFunction("cast", &U##type::lua_codegen_cast)                                                                             \
      .addStaticFunction("get_class", []() { return U##type::StaticClass(); })                                                           \
      .addFunction("__tostring", &U##type::ToString)                                                                                     \
      .addFunction("__eq", &U##type::LuaEquals)                                                                                          \
      .endClass();                                                                                                                       \
    if (!U##type::StaticClass()->HasAnyClassFlags(CLASS_Abstract)) {                                                                     \
      luabridge::getGlobalNamespace(L)                                                                                                   \
        .beginClass<U##type>(#type)                                                                                                      \
        .addStaticFunction(                                                                                                              \
          "new", +[](UInstance *parent, std::string_view newName) { return NewObject<U##type>(parent, UTF8_TO_TCHAR(newName.data())); }) \
        .addStaticFunction(                                                                                                              \
          "new_simple", +[]() { return NewObject<U##type>(); })                                                                          \
        .endClass();                                                                                                                     \
    }                                                                                                                                    \
  }                                                                                                                                      \
  bool LuaEquals(const UObject &other) const {                                                                                           \
    return this == &other;                                                                                                               \
  }

#define EVO_CODEGEN_ACCESSOR(type)                                                \
  __EVO_COMMON_CODEGEN(type)                                                      \
  virtual void lua_reg_internal(lua_State *L) const override {                    \
    LOG(INFO_LL) << "Registering lua instance " << TEXT(#type);                   \
    luabridge::getGlobalNamespace(L)                                              \
      .beginClass<U##type>(#type)                                                 \
      .addStaticFunction("cast", &U##type::lua_codegen_cast)                      \
      .addStaticFunction("get_class", []() { return U##type::StaticClass(); })    \
      .addFunction("__tostring", &U##type::ToString)                              \
      .addFunction("__eq", &U##type::LuaEquals)                                   \
      .endClass();                                                                \
    if (!U##type::StaticClass()->HasAnyClassFlags(CLASS_Abstract)) {              \
      luabridge::getGlobalNamespace(L)                                            \
        .beginClass<U##type>(#type)                                               \
        .addStaticFunction(                                                       \
          "new", +[](UBlockLogic *parent, std::string_view newName) {             \
            auto acc = NewObject<U##type>(parent, UTF8_TO_TCHAR(newName.data())); \
            parent->RegisterAccessor(acc);                                        \
            return acc;                                                           \
          })                                                                      \
        .endClass();                                                              \
    }                                                                             \
  }                                                                               \
  bool LuaEquals(const UObject &other) const {                                    \
    return this == &other;                                                        \
  }

// /*.addStaticFunction("get", &evo::DB::get<type>)   */ /* .addStaticFunction("get_derived", &evo::DB::get_derived<type>)  */
// #define EVO_LUA_CODEGEN_EMPTY(type, parent, name)                                                                           \
//   public:                                                                                                                   \
//   static void lua_reg(lua_State *L) {                                                                                       \
//     LOG(INFO_LL) << "Registering lua " << #name;                                                                            \
//     luabridge::getGlobalNamespace(L)                                                                                        \
//       .deriveClass<type, parent>(#name)                                                                                     \
//       .addStaticFunction(                                                                                                   \
//         "find", +[](std::string_view name) { return FindObject<type>(EVO_ASSET_OWNER(), UTF8_TO_TCHAR(name.data())); })     \
//       .addStaticFunction("cast", &type::lua_codegen_cast)                                                                   \
//       .addStaticFunction(                                                                                                   \
//         "new", +[](std::string_view newName) { return NewObject<type>(EVO_ASSET_OWNER(), UTF8_TO_TCHAR(newName.data())); }) \
//       .addStaticFunction("get_class", []() { return type::StaticClass(); })                                                 \
//       .endClass();                                                                                                          \
//   }

UCLASS(Abstract)
class UPrototype : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:
  static UPrototype *lua_codegen_cast(UObject *parent_inst) { return Cast<UPrototype>(parent_inst); }
  virtual UClass *lua_reg_type() { return UPrototype::StaticClass(); }
  virtual void lua_reg_internal(lua_State *L) const {}
  virtual void lua_reg(lua_State *L) const {
    LOG(INFO_LL) << "Registering lua Prototype";
    luabridge::getGlobalNamespace(L)
      //@comment Database record
      .deriveClass<UPrototype, UObject>("Prototype") //@class Prototype : Object
      .addProperty("name", [](const UPrototype *self) //@field string Object name
                   {
                     return std::string(TCHAR_TO_UTF8(*self->GetName()));
                   })
      .endClass();
  }
  virtual luabridge::LuaRef to_luaref(lua_State *L) {
    checkNoEntry();
    return luabridge::LuaRef(nullptr, nullptr);
  }

  virtual UClass *GetSuperProto() const {
    checkNoEntry();
    return nullptr;
  }

  UPROPERTY(VisibleAnywhere)
  const UMod *mOwningMod = nullptr;

  //No code
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override {
    return true;
  }

  virtual void BeginDestroy() override {
    LOG(INFO_LL) << "Destroying Proto " << GetFullName();
    Super::BeginDestroy();
  }

  virtual bool ProtoValidCheck() { return true; }
  virtual void Release() {}
  virtual void MarkIncomplete() {}
  virtual void OnObjectFromTable() {}

  virtual void ModLoadFinalize() {}

  virtual std::string ToString() const {
    return TCHAR_TO_UTF8(*("(" + GetClass()->GetName() + ": " + GetName() + ")"));
  }

  virtual UPrototype *get_or_register(const FString &obj_name, IRegistrar &registry) {
    checkNoEntry();
    return nullptr;
  }

  protected:
  template <typename BaseType, typename RealType>
  inline UPrototype *_get_or_register(const FString &obj_name, IRegistrar &registry) {
    auto obj = Cast<BaseType>(registry.Find(*obj_name, BaseType::StaticClass()));
    if (!obj) {
      auto owner = registry.GetOuter(BaseType::StaticClass());
      obj = NewObject<BaseType>(owner, RealType::StaticClass(), *obj_name);
      LOG(INFO_LL) << "Register " << BaseType::StaticClass()->GetName() << " " << obj_name;
      registry.Register(obj);
    }

    return obj;
  }
};

UCLASS(Abstract)
class UInstance : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:
  static UInstance *lua_codegen_cast(UObject *parent_inst) { return Cast<UInstance>(parent_inst); }
  virtual UClass *lua_reg_type() { return UInstance::StaticClass(); }
  virtual void lua_reg_internal(lua_State *L) const {
    LOG(INFO_LL) << "Registering lua Instance";
    luabridge::getGlobalNamespace(L)
      .deriveClass<UInstance, UObject>("Instance")
      .endClass();
  };
  virtual void lua_reg(lua_State *L) const {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UInstance, UObject>("Instance")
      .endClass();
  }
  virtual luabridge::LuaRef to_luaref(lua_State *L) {
    checkNoEntry();
    return luabridge::LuaRef(nullptr, nullptr);
  };

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override {
    return true;
  }

  virtual void LuaCleanup() {}

  virtual std::string ToString() const {
    return TCHAR_TO_UTF8(*("(" + GetClass()->GetName() + ": " + GetName() + ")"));
  }
};
#pragma once
#include "CoreMinimal.h"
#include "Evospace/CoordinateSystem.h"
#include "ThirdParty/luabridge/luabridge.h"

inline void registerMathClasses(lua_State *L) {
  // FColor
  luabridge::getGlobalNamespace(L)
    .beginClass<FColor>("Color")
    .addStaticFunction("new", [](uint8 r, uint8 g, uint8 b, uint8 a) { return FColor(r, g, b, a); })
    .addProperty("r", &FColor::R, true)
    .addProperty("g", &FColor::G, true)
    .addProperty("b", &FColor::B, true)
    .addProperty("a", &FColor::A, true)
    .addStaticProperty(
      "red", +[]() { return FColor::Red; })
    .addStaticProperty(
      "green", +[]() { return FColor::Green; })
    .addStaticProperty(
      "blue", +[]() { return FColor::Blue; })
    .addStaticProperty(
      "yellow", +[]() { return FColor::Yellow; })
    .addStaticProperty(
      "cyan", +[]() { return FColor::Cyan; })
    .addStaticProperty(
      "magenta", +[]() { return FColor::Magenta; })
    .addStaticProperty(
      "white", +[]() { return FColor::White; })
    .addStaticProperty(
      "black", +[]() { return FColor::Black; })
    .addStaticProperty(
      "gray", +[]() { return FColor(128, 128, 128, 255); })
    .addStaticProperty(
      "orange", +[]() { return FColor::Orange; })
    .addStaticProperty(
      "purple", +[]() { return FColor::Purple; })
    .addStaticProperty(
      "transparent", +[]() { return FColor::Transparent; })
    .addFunction("__tostring", [](FColor &c) -> std::string {
      return TCHAR_TO_UTF8(*("(Color: r=" + FString::FromInt(c.R) + " g=" + FString::FromInt(c.G) + " b=" + FString::FromInt(c.B) + ")"));
    })
    .endClass();

  // FQuat
  luabridge::getGlobalNamespace(L)
    .beginClass<FQuat>("FQuat")
    .addStaticFunction("new", [](float x, float y, float z, float w) { return FQuat(x, y, z, w); })
    .addStaticProperty("identity", []() -> FQuat { return FQuat::Identity; })
    .addProperty("x", &FQuat::X, true)
    .addProperty("y", &FQuat::Y, true)
    .addProperty("z", &FQuat::Z, true)
    .addProperty("w", &FQuat::W, true)
    .addFunction("get_forward_vector", &FQuat::GetForwardVector)
    .addFunction("get_right_vector", &FQuat::GetRightVector)
    .addFunction("get_up_vector", &FQuat::GetUpVector)
    .addFunction("__tostring", [](FQuat &c) -> std::string {
      auto rot = c.Rotator();
      return TCHAR_TO_UTF8(*("(Quat: yaw=" + FString::FromInt(rot.Yaw) + " pitch=" + FString::FromInt(rot.Pitch) + " roll=" + FString::FromInt(rot.Roll) + ")"));
    })
    .endClass();

  // FTransform
  luabridge::getGlobalNamespace(L)
    .beginClass<FTransform>("FTransform")
    .addStaticFunction("new", []() { return FTransform(); })
    .addFunction("set_location", &FTransform::SetLocation)
    .addFunction("set_rotation", &FTransform::SetRotation)
    .addFunction("set_scale", &FTransform::SetScale3D)
    .addFunction("get_location", &FTransform::GetLocation)
    .addFunction("get_rotation", &FTransform::GetRotation)
    .addFunction("get_scale", &FTransform::GetScale3D)
    .endClass();

  luabridge::getGlobalNamespace(L)
    .beginClass<Vec3i>("Vec3i")
    .addStaticFunction(
      "new", +[](int32 x, int32 y, int32 z) { return Vec3i(x, y, z); })
    .addStaticProperty(
      "one", +[]() { return Vec3i(1); })
    .addStaticProperty(
      "zero", +[]() { return Vec3i(0); })
    .addStaticProperty(
      "up", +[]() { return Side::Up; })
    .addStaticProperty(
      "down", +[]() { return Side::Down; })
    .addStaticProperty(
      "left", +[]() { return Side::Left; })
    .addStaticProperty(
      "right", +[]() { return Side::Right; })
    .addStaticProperty(
      "back", +[]() { return Side::Back; })
    .addStaticProperty(
      "front", +[]() { return Side::Front; })
    .addProperty("x", &Vec3i::X, true)
    .addProperty("y", &Vec3i::Y, true)
    .addProperty("z", &Vec3i::Z, true)
    .addFunction("__tostring", [](Vec3i &c) -> std::string {
      return TCHAR_TO_UTF8(*("(Vec3i: x=" + FString::FromInt(c.X) + " y=" + FString::FromInt(c.Y) + " z=" + FString::FromInt(c.Z) + ")"));
    })
    .addFunction("__add", [](Vec3i &a, Vec3i &b) { return Vec3i(a.X + b.X, a.Y + b.Y, a.Z + b.Z); })
    .addFunction("__eq", [](Vec3i &a, Vec3i &b) { return a.X == b.X && a.Y == b.Y && a.Z == b.Z; })
    .addFunction("__sub", [](Vec3i &a, Vec3i &b) { return Vec3i(a.X - b.X, a.Y - b.Y, a.Z - b.Z); })
    .addFunction("__mul", [](Vec3i &a, Vec3i &b) { return Vec3i(a.X * b.X, a.Y * b.Y, a.Z * b.Z); })
    .addFunction("__div", [](Vec3i &a, Vec3i &b) { return Vec3i(a.X / b.X, a.Y / b.Y, a.Z / b.Z); })
    .endClass();

  luabridge::getGlobalNamespace(L)
    .beginClass<Vec2i>("Vec2i")
    .addStaticFunction(
      "new", +[](int32 x, int32 y) { return Vec2i(x, y); })
    .addStaticProperty(
      "zero", +[]() { return Vec2i(0); })
    .addStaticProperty(
      "one", +[]() { return Vec2i(1); })
    .addProperty("x", &Vec2i::X, true)
    .addProperty("y", &Vec2i::Y, true)
    .addFunction("__eq", [](Vec2i &a, Vec2i &b) { return a.X == b.X && a.Y == b.Y; })
    .addFunction("__add", [](Vec2i &a, Vec2i &b) { return Vec2i(a.X + b.X, a.Y + b.Y); })
    .addFunction("__sub", [](Vec2i &a, Vec2i &b) { return Vec2i(a.X - b.X, a.Y - b.Y); })
    .addFunction("__mul", [](Vec2i &a, Vec2i &b) { return Vec2i(a.X * b.X, a.Y * b.Y); })
    .addFunction("__div", [](Vec2i &a, Vec2i &b) { return Vec2i(a.X / b.X, a.Y / b.Y); })
    .addFunction("__tostring", [](Vec2i &c) -> std::string {
      return TCHAR_TO_UTF8(*("(Vec2i: x=" + FString::FromInt(c.X) + " y=" + FString::FromInt(c.Y) + ")"));
    })
    .endClass();

  luabridge::getGlobalNamespace(L)
    .beginClass<FVector>("Vec3")
    .addStaticFunction(
      "new", +[](float x, float y, float z) { return FVector(x, y, z); })
    .addStaticProperty(
      "one", +[]() { return FVector(1); })
    .addStaticProperty(
      "zero", +[]() { return FVector(0); })
    .addStaticProperty(
      "up", +[]() { return FVector(Side::Up.X, Side::Up.Y, Side::Up.Z); })
    .addStaticProperty(
      "down", +[]() { return FVector(Side::Down.X, Side::Down.Y, Side::Down.Z); })
    .addStaticProperty(
      "left", +[]() { return FVector(Side::Left.X, Side::Left.Y, Side::Left.Z); })
    .addStaticProperty(
      "right", +[]() { return FVector(Side::Right.X, Side::Right.Y, Side::Right.Z); })
    .addStaticProperty(
      "back", +[]() { return FVector(Side::Back.X, Side::Back.Y, Side::Back.Z); })
    .addStaticProperty(
      "front", +[]() { return FVector(Side::Front.X, Side::Front.Y, Side::Front.Z); })
    .addProperty("x", &FVector::X, true)
    .addProperty("y", &FVector::Y, true)
    .addProperty("z", &FVector::Z, true)
    .addFunction("__eq", [](FVector &a, FVector &b) { return FMath::IsNearlyEqual(a.X, b.X) && FMath::IsNearlyEqual(a.Y, b.Y) && FMath::IsNearlyEqual(a.Z, b.Z); })
    .addFunction("__add", [](FVector &a, FVector &b) { return FVector(a.X + b.X, a.Y + b.Y, a.Z + b.Z); })
    .addFunction("__sub", [](FVector &a, FVector &b) { return FVector(a.X - b.X, a.Y - b.Y, a.Z - b.Z); })
    .addFunction("__mul", [](FVector &a, FVector &b) { return FVector(a.X * b.X, a.Y * b.Y, a.Z * b.Z); })
    .addFunction("__div", [](FVector &a, FVector &b) { return FVector(a.X / b.X, a.Y / b.Y, a.Z / b.Z); })
    .addFunction("__tostring", [](FVector &c) -> std::string {
      return TCHAR_TO_UTF8(*("(Vec3: x=" + FString::SanitizeFloat(c.X) + " y=" + FString::SanitizeFloat(c.Y) + " z=" + FString::SanitizeFloat(c.Z) + ")"));
    })
    .endClass();

  luabridge::getGlobalNamespace(L)
    .beginNamespace("cs")
    .addFunction(
      "bp2sp", +[](const Vec3i &bpos) { return cs::WBtoS(bpos, gSectorSize); })
    .addFunction(
      "w2bp", +[](const FVector &world) { return cs::WtoWB(world); })
    .addFunction(
      "bp2w", +[](const Vec3i &bpos) { return cs::WBtoW(bpos); })
    .addFunction(
      "w2sp", +[](const FVector &world) { return cs::WBtoS(world, gSectorSize); })
    .addProperty(
      "sector_size", +[]() { return gSectorSize; })
    .endNamespace();
}

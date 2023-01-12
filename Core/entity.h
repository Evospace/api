
// Copyright (c) 2017 - 2022, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Evospace/SerializableJson.h"
#include "Evospace/Shared/Core/prototype.h"

#include "entity.generated.h"

class ADimension;
class UItem;
class AActor;

UCLASS(Abstract)
/**
 * @brief Placeable entity in world. Can be block or prop
 */
class  UEntity : public UPrototype, public ISerializableJson {
    GENERATED_BODY()

  public:
    virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

    virtual AActor *CreateSelector() const { return nullptr; };

    virtual bool Spawn(ADimension *dimension, const FTransform &tr) const {
        return false;
    };

    bool is_surface = false;
    
public:
    EVO_LUA_CODEGEN_DB(UEntity, Entity);
    static void RegisterLua(lua_State *L);
};

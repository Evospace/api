// Copyright (c) 2017 - 2022, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "Evospace/BreakResult.h"
#include "Evospace/Common.h"
#include "Evospace/CoordinateSystem.h"
#include "Evospace/SerializableJson.h"
#include "Evospace/Blocks/AccessorComponent.h"
#include "CoreMinimal.h"
#include "Evospace/Shared/bases.h"

#include "block_logic.generated.h"

DECLARE_STATS_GROUP(
    TEXT("BLOCKLOGIC_Game"), STATGROUP_BLOCKLOGIC, STATCAT_Advanced
);

class UHudWidget;
class URootBlockComponent;

class ADimension;
class AItemLogic;
class UStaticItem;
class UInventoryAccess;
class UStaticBlock;
class UBaseAccessor;

UCLASS(BlueprintType)
class EVOSPACE_API UBlockLogic : public UObject,
                                 public ISerializableJson,
                                 public evo::BaseHelper<UBlockLogic> {
    GENERATED_BODY()

    // Lua api
  public:
    

  public:
    UBlockLogic();

    // Events
  public:
    virtual void OnNeighborBlockAdded(UBlockLogic *block, const Vec3i &pos);
    virtual void OnNeighborBlockRemoved(UBlockLogic *block, const Vec3i &pos);
    virtual void OnSideAccessorAdded(
        UBaseAccessor *accessor, const Vec3i &side, const Vec3i &pos
    );
    virtual void OnSideAccessorRemoved(
        UBaseAccessor *accessor, const Vec3i &side, const Vec3i &pos
    );
    virtual void OnSpawnedByItem(AItemLogic *item);
    virtual EBreakResult OnRemovedByItem(AItemLogic *item);

    //
  public:
    virtual Vec3i GetRotationLocks() const;

    virtual bool IsHandleRecipeSelection() const;
    virtual void HandleRecipeSelection(UStaticItem *item);

    virtual void SetActor(AActor *actor);
    void DefferedPaintApply();
    virtual void RemoveActor();

    virtual void LuaBlock();
    virtual void BlockDestruction();

    virtual bool IsBlockTicks() const;
    virtual void Tick();
    virtual void TickAccessor();

    /**
     * @brief Test if position is suitable for this block placing
     *
     * Some places are not suitable for block placing but still not occupied.
     * For example when such block will connect two pipe systems with different
     * content
     * @param dim dimension instance for test
     * @param pos position for test
     * @return
     */
    virtual bool CheckPlaceble(ADimension *dim, FVector3i pos);

    UFUNCTION(BlueprintCallable)
    /**
     * @brief Try to awake actor in world for animation playing
     */
    virtual void AwakeAnimation() const;

    UFUNCTION(BlueprintCallable)
    /**
     * @brief Try to make sleep actor in world for stopping animation playing
     */
    virtual void SleepAnimation() const;

    // No any code
    virtual bool SaveSettings(
        TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr
    );

    // No any code
    virtual bool LoadSettings(
        TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr
    );

    UFUNCTION(BlueprintCallable)
    virtual FString SaveSettings(AMainPlayerController *mpc = nullptr);

    UFUNCTION(BlueprintCallable)
    virtual void
    LoadSettings(const FString &json, AMainPlayerController *mpc = nullptr);

    UFUNCTION(BlueprintCallable)
    void PaintBlock(UMaterialInterface *mat);

    // Absolute
    Vec3i GetBlockPos() const;

    // Absolute
    void SetBlockPos(const Vec3i &pos);

    virtual UBlockLogic *GetRootBlock();

    FQuat GetBlockQuat() const;
    void SetBlockQuat(const FQuat &r);

    float DeltaTime = 0.05f;

    UFUNCTION(BlueprintCallable)
    virtual TSubclassOf<UBlockWidget> GetWidgetClass() const;

    UFUNCTION(BlueprintCallable)
    virtual EBlockWidgetType GetWidgetType() const;

    // No any code
    virtual void BlockBeginPlay();

    // No any code
    virtual void BlockEndPlay();

    FTransform GetActorTransform() const;
    FTransform GetTransform() const;

    void SetActorRotation(FQuat param1);
    void SetOwner(void *param1);
    void Destroy();

    UBaseAccessor *GetSideAccessor(UClass *type, Vec3i side, Vec3i pos);

    template <class Ty_> Ty_ *GetSideAccessor(Vec3i side, Vec3i pos) {
        return Cast<Ty_>(GetSideAccessor(Ty_::StaticClass(), side, pos));
    }

    UBaseAccessor *GetCoreAccessor(UClass *type);

    template <class Ty_> Ty_ *GetCoreAccessor() {
        return Cast<Ty_>(GetCoreAccessor(Ty_::StaticClass()));
    }

    template <typename Ty_>
    Ty_ *CreateDefaultAccessor(UObject *Outer, FName SubobjectFName) {
        auto v = CreateDefaultSubobject<Ty_>(Outer, SubobjectFName);
        AddAccessor(v);
        return v;
    }

    template <typename Ty_> Ty_ *CreateDefaultAccessor(FName SubobjectFName) {
        auto v = CreateDefaultSubobject<Ty_>(SubobjectFName);
        AddAccessor(v);
        return v;
    }

  public:
    ADimension *GetDimension();
    void SetDimention(ADimension *dim);

    //=====================

    virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
    virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

    void SetStaticBlock(const UStaticBlock *static_block);

    virtual const UStaticBlock *GetStaticBlock() const;

    // ************************************* //

    virtual void OpenWidget(class UHudWidget *widget);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnAction"))
    void ReceiveOnAction();

    virtual void
    OnAction(const FHitResult &hit, const Vec3i &side, AItemLogic *item);

    virtual int32 DropItems(UInventoryAccess *inventory);

    void AddAccessor(UBaseAccessor *c);

    virtual TArray<UBaseAccessor *> & GetAccessors();

    virtual AActor *GetActor();

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    const UStaticBlock *mStaticBlock = nullptr;

    UPROPERTY(VisibleAnywhere)
    AActor *mActor = nullptr;

    UPROPERTY(VisibleAnywhere)
    FVector3i mPos = {};

    UPROPERTY(VisibleAnywhere)
    FQuat mQuat = FQuat(EForceInit::ForceInitToZero);

    UPROPERTY(VisibleAnywhere)
    TArray<UBaseAccessor *> mAccessors;

    UPROPERTY()
    ADimension *mDimension;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface *mPaintMaterial;

    static std::function<void(lua_State *)> GetRegisterLambda() { return [](lua_State *) {};  }
};
EVO_REGISTER_STATIC(UBlockLogic, BlockLogic);

UCLASS(BlueprintType)
class EVOSPACE_API UPartBlockLogic : public UBlockLogic {
    GENERATED_BODY()

  public:
    virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
    virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

    UBlockLogic *mParentBlock;

    virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

    virtual void OpenWidget(class UHudWidget *widget) override;

    virtual UBlockLogic *GetRootBlock() override;

    virtual TArray<UBaseAccessor *> & GetAccessors() override;

    virtual AActor *GetActor() override;

    virtual void SetActor(AActor *actor);

    virtual void
    OnAction(const FHitResult &hit, const Vec3i &side, AItemLogic *item);
};
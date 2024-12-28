// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Prototype.h"
#include "Evospace/BreakResult.h"
#include "Evospace/CoordinameMinimal.h"
#include "Evospace/Vector.h"

#include "BlockLogic.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class UStaticCover;
class UCoreAccessor;
class AMainPlayerController;
class ABlockActor;
class UBlockWidget;

class UHudWidget;
class URootBlockComponent;

class ADimension;
class AItemLogic;
class UStaticItem;
class UInventoryAccess;
class UStaticBlock;
class UAccessor;

UCLASS(BlueprintType)
/**
 * 
 */
class EVOSPACE_API UBlockLogic : public UInstance {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(BlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UBlockLogic, UInstance>("BlockLogic")
      .addFunction("reg", &UBlockLogic::RegisterAccessor)
      .addProperty("static_block", &UBlockLogic::mStaticBlock)
      .endClass();
  }

  protected:
  // events

  UBlockLogic();

  public:
  //    ?
  //   BlockBeginPlay
  virtual void NeighborBlockAdded(UBlockLogic *block, const Vec3i &pos);
  virtual void NeighborBlockRemoved(UBlockLogic *block, const Vec3i &pos);

  virtual void SideAccessorAdded(UAccessor *accessor, const Vec3i &side, const Vec3i &pos);
  virtual void SideAccessorRemoved(UAccessor *accessor, const Vec3i &side, const Vec3i &pos);

  virtual void SpawnedByItem(AItemLogic *item);
  virtual EBreakResult RemovedByItem(AItemLogic *item);

  virtual Vec3i GetRotationLocks() const;

  virtual bool IsHandleRecipeSelection() const;
  virtual void HandleRecipeSelection(UStaticItem *item);
  void ShowAccessors();

  virtual void SetRenderable();

  virtual void SetActor(ABlockActor *actor);
  void DeferredPaintApply() const;
  virtual void RemoveActorOrRenderable();

  virtual void LuaBlock();
  virtual void BlockDestruction();

  virtual bool IsBlockTicks() const;
  virtual void Tick();
  virtual void TickAccessor();

  ADimension *GetDim() const { return mDimension; }

  virtual void CopyOnReplace(UBlockLogic *from);

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
  virtual void SaveSettings(TSharedPtr<FJsonObject> json,
                            AMainPlayerController *mpc = nullptr);

  // No any code
  virtual void LoadSettings(TSharedPtr<FJsonObject> json,
                            AMainPlayerController *mpc = nullptr);

  UFUNCTION(BlueprintCallable)
  virtual FString SaveSettings(AMainPlayerController *mpc = nullptr);

  UFUNCTION(BlueprintCallable)
  virtual void LoadSettings(const FString &json, AMainPlayerController *mpc = nullptr);

  UFUNCTION(BlueprintCallable)
  void PaintBlock(UMaterialInterface *mat);

  const Vec3i &GetBlockPos() const;

  void Init(const Vec3i &pos, const FQuat &r, const UStaticBlock *block, ADimension *dim);

  virtual UBlockLogic *GetPartRootBlock();

  virtual UBlockLogic *GetWidgetRootBlock();

  const FQuat &GetBlockQuat() const;
  void SetBlockQuat(const FQuat &r);

  void SetActorMobility(EComponentMobility::Type movable);

  float DeltaTime = 0.05f;

  UPROPERTY(VisibleAnywhere)
  TMap<const UStaticCover *, UHierarchicalInstancedStaticMeshComponent *> mAccessorCover;

  UFUNCTION(BlueprintCallable)
  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const;

  UFUNCTION(BlueprintCallable)
  virtual EBlockWidgetType GetWidgetType() const;

  // No any code
  virtual void BlockBeginPlay();

  // No any code
  virtual void BlockEndPlay();

  FTransform GetTransformLocation() const;

  FTransform GetTransform() const;

  FVector GetLocation() const;

  void SetOwner(void *param1);

  UAccessor *GetSideAccessor(UClass *type, Vec3i side, Vec3i pos);

  template <class Ty_>
  Ty_ *GetSideAccessor(FVector3i side, FVector3i pos) {
    return Cast<Ty_>(GetSideAccessor(Ty_::StaticClass(), side, pos));
  }

  template <typename Ty_>
  Ty_ *CreateDefaultAccessor(UObject *Outer, FName SubobjectFName) {
    auto v = CreateDefaultSubobject<Ty_>(Outer, SubobjectFName);
    RegisterAccessor(v);
    return v;
  }

  template <typename Ty_>
  Ty_ *CreateDefaultAccessor(FName SubobjectFName) {
    auto v = CreateDefaultSubobject<Ty_>(SubobjectFName);
    RegisterAccessor(v);
    return v;
  }

  public:
  //=====================

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  virtual const UStaticBlock *GetStaticBlock() const;

  // ************************************* //

  virtual void OpenWidget(class UHudWidget *widget);

  virtual void OnAction(const FHitResult &hit, const Vec3i &side, AItemLogic *item);

  //    .
  virtual int32 DropItems(UInventoryAccess *inventory);

  void SpawnDropItems(APlayerController *pc);

  void RegisterAccessor(UAccessor *c);

  virtual TArray<UAccessor *> GetAccessors();

  virtual ABlockActor *GetActor();

  virtual bool IsNoActor() const;

  virtual bool IsPart() { return false; }

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  bool mRenderable = false;

  UPROPERTY(VisibleAnywhere)
  FVector3i mPos = {};

  UPROPERTY(VisibleAnywhere)
  FQuat mQuat = FQuat(EForceInit::ForceInitToZero);

  UPROPERTY(VisibleAnywhere)
  TArray<UAccessor *> mAccessors;

  // Core
  protected:
  UPROPERTY(VisibleAnywhere)
  UCoreAccessor *mCore = nullptr;
  TFunction<UCoreAccessor *()> mCoreInit;

  public:
  UFUNCTION(BlueprintCallable)
  virtual UCoreAccessor *GetCoreAccessor();

  // Monitor
  protected:
  UPROPERTY(VisibleAnywhere)
  UCoreAccessor *mMonitor = nullptr;
  TFunction<UCoreAccessor *()> mMonitorInit;

  public:
  UFUNCTION(BlueprintCallable)
  virtual UCoreAccessor *GetMonitorAccessor();

  //mutable int32 mFromLastWakeup = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UMaterialInterface *mPaintMaterial = nullptr;

  protected:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  ABlockActor *mActor = nullptr;

  private:
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
  const UStaticBlock *mStaticBlock = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  ADimension *mDimension = nullptr;
};

UCLASS(BlueprintType)
class EVOSPACE_API UPartBlockLogic : public UBlockLogic {
  GENERATED_BODY()

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY()
  UBlockLogic *mParentBlock;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual void OpenWidget(class UHudWidget *widget) override;

  virtual UBlockLogic *GetPartRootBlock() override;

  virtual TArray<UAccessor *> GetAccessors() override;

  virtual UCoreAccessor *GetCoreAccessor();

  virtual UCoreAccessor *GetMonitorAccessor();

  virtual bool IsPart() override { return true; }

  virtual ABlockActor *GetActor() override;

  virtual void SetActor(ABlockActor *actor);

  virtual void OnAction(const FHitResult &hit, const Vec3i &side, AItemLogic *item);
};
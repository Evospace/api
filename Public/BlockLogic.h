// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "Qr/Prototype.h"
#include "Evospace/BreakResult.h"
#include "Evospace/Common.h"
#include "Qr/CoordinameMinimal.h"
#include "Qr/Vector.h"
#include "Public/LogicProgram.h"
#include "Public/LogicContext.h"
#include "Evospace/Misc/CoverWrapper.h"

#include "BlockLogic.generated.h"

class USectorProxy;
class UHierarchicalInstancedStaticMeshComponent;
class UStaticCover;
class UCoreAccessor;
class AMainPlayerController;
class ABlockActor;
class UBlockWidget;

class UHudWidget;
class URootBlockComponent;

class ADimension;
class ASector;
class AItemLogic;
class UStaticItem;
class UInventoryAccess;
class UStaticBlock;
class UAccessor;
class AColumn;
class ULogicContext;
class ULogicSettings;

UCLASS(BlueprintType)
class UBlockLogic : public UInstance {
  GENERATED_BODY()
  using Self = UBlockLogic;
  EVO_CODEGEN_INSTANCE(BlockLogic)

  private:
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UBlockLogic, UInstance>("BlockLogic") //@class BlockLogic : Instance
      //direct:
      //--- Register Accessor
      //--- @param accessor Accessor Accessor instance to register
      //function BlockLogic:reg(accessor) end
      .addFunction("reg", &UBlockLogic::RegisterAccessor)
      .addProperty("map_register", &UBlockLogic::bRegisterOnMap)
      .addProperty("static_block", &UBlockLogic::mStaticBlock) //@field StaticBlock comment
      .addFunction("find_accessor", &UBlockLogic::GetAccessor)
      .endClass();
  }

  protected:
  UBlockLogic();

  public:
  // Lifecycle
  virtual void BlockBeginPlay();
  virtual void BlockEndPlay();
  virtual void SpawnBlockPostprocess();
  virtual void BlockDestruction();

  // Neighbor and accessor events
  virtual void NeighborBlockAdded(UBlockLogic *block, const Vec3i &pos);
  virtual void NeighborBlockRemoved(UBlockLogic *block, const Vec3i &pos);

  // Item interactions
  virtual void SpawnedByItem(AItemLogic *item);
  virtual EBreakResult RemovedByItem(AItemLogic *item);

  // Ticking
  virtual bool IsBlockTicks() const;
  virtual void Tick();
  virtual void TickAccessor();

  // Rotation and placement
  virtual Vec3i GetRotationLocks() const;
  virtual void RotationPostprocess();
  virtual bool CheckPlaceble(ADimension *dim, FVector3i pos);
  virtual void CopyOnReplace(UBlockLogic *from);

  // Recipe selection
  virtual bool IsHandleRecipeSelection() const;
  virtual void HandleRecipeSelection(UStaticItem *item);

  // Actor and rendering
  virtual void SetRenderable(AColumn *sector);
  virtual void SetActor(ABlockActor *actor);
  virtual void RemoveActorOrRenderable();
  virtual ABlockActor *GetActor();
  void SetActorMobility(EComponentMobility::Type movable);

  // Transform and position
  const Vec3i &GetBlockPos() const;
  const FQuat &GetBlockQuat() const;
  void SetBlockQuat(const FQuat &r);
  FTransform GetTransformLocation() const;
  FTransform GetTransform() const;
  FVector GetLocation() const;
  UFUNCTION(BlueprintCallable)
  FVector3i GetWorldPosition() const;

  // Initialization and ownership
  void Init(const Vec3i &pos, const FQuat &r, const UStaticBlock *block, ADimension *dim);
  void SetOwner(void *param1);

  // Accessors and helpers
  void RegisterAccessor(UAccessor *c);
  virtual TArray<UAccessor *> GetAccessors();
  UAccessor *GetAccessor(const std::string &name);
  UAccessor *GetSideAccessor(UClass *type, Vec3i side, Vec3i pos);
  template <class Ty_>
  Ty_ *GetSideAccessor(FVector3i side, FVector3i pos) {
    return Cast<Ty_>(GetSideAccessor(Ty_::StaticClass(), side, pos));
  }
  template <typename Ty_>
  Ty_ *EvoDefaultAccessor(UObject *Outer, FName SubobjectFName) {
    auto v = CreateDefaultSubobject<Ty_>(Outer, SubobjectFName);
    RegisterAccessor(v);
    return v;
  }
  template <typename Ty_>
  Ty_ *EvoDefaultAccessor(FName SubobjectFName) {
    auto v = CreateDefaultSubobject<Ty_>(SubobjectFName);
    RegisterAccessor(v);
    return v;
  }

  // Widgets and UI
  UFUNCTION(BlueprintCallable)
  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const;
  UFUNCTION(BlueprintCallable)
  virtual TSubclassOf<UBlockWidget> GetHoverWidgetClass() const;
  UFUNCTION(BlueprintCallable)
  virtual EBlockWidgetType GetWidgetType() const;
  virtual void OpenWidget(class UHudWidget *widget);

  // Actions
  UFUNCTION(BlueprintCallable)
  virtual bool HasAction() const { return false; }
  UFUNCTION(BlueprintCallable)
  void SimulateAction();
  virtual void OnAction(const FHitResult &hit, const Vec3i &side, AItemLogic *item);

  // Inventory
  virtual int32 DropItems(UInventoryAccess *inventory);
  void SpawnDropItems(APlayerController *pc);

  // Animation and visuals
  UFUNCTION(BlueprintCallable)
  virtual void AwakeAnimation() const;
  UFUNCTION(BlueprintCallable)
  virtual void SleepAnimation() const;
  UFUNCTION(BlueprintCallable)
  void PaintBlock(UMaterialInterface *mat);
  void DeferredPaintApply() const;

  // Logic and signals
  virtual void PopulateLogicOutput(class ULogicContext *ctx) const;
  virtual void ApplyLogicInput(const class ULogicContext *ctx);
  UFUNCTION(BlueprintCallable)
  virtual ULogicSettings *GetLogicSettings();
  UFUNCTION(BlueprintCallable)
  virtual UCoreAccessor *GetCoreAccessor();
  UFUNCTION(BlueprintCallable)
  virtual UCoreAccessor *GetMonitorAccessor();
  UFUNCTION(BlueprintCallable)
  const UStaticItem *GetNetworkSignal() const;
  virtual const UStaticBlock *GetStaticBlock() const;

  // Serialization
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
  virtual void SaveSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr) const;
  virtual void LoadSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr);
  UFUNCTION(BlueprintCallable)
  virtual FString SaveSettings(AMainPlayerController *mpc = nullptr);
  UFUNCTION(BlueprintCallable)
  virtual void LoadSettings(const FString &json, AMainPlayerController *mpc = nullptr);

  // Hierarchy helpers
  virtual UBlockLogic *GetPartRootBlock();
  virtual UBlockLogic *GetWidgetRootBlock();
  virtual bool IsPart() { return false; }

  // World/Dimension
  ADimension *GetDim() const { return mDimension; }

  // State controls
  void SetAccessorsInstances(bool show);
  void SetWorking(bool working);

  // Variables (public)
  float DeltaTime = 0.05f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UMaterialInterface *mPaintMaterial = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  ABlockActor *mActor = nullptr;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  const UStaticBlock *mStaticBlock = nullptr;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  const UStaticItem *NetworkSignal = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bRegisterOnMap = false;

  UPROPERTY(VisibleAnywhere)
  TMap<const UStaticCover *, UHierarchicalInstancedStaticMeshComponent *> mAccessorCover;

  UPROPERTY(VisibleAnywhere)
  FVector3i mPos = {};

  UPROPERTY(VisibleAnywhere)
  FQuat mQuat = FQuat(EForceInit::ForceInitToZero);

  UPROPERTY(VisibleAnywhere)
  TArray<UAccessor *> Accessors;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float LoadRatio;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  bool mRenderable = false;

  RCoverWrapper Cover;

  protected:
  // Core
  UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
  UCoreAccessor *Core = nullptr;
  // Per-class core initializer; override to provide custom core accessor
  virtual UCoreAccessor *CoreInit();

  // Monitor
  UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
  UCoreAccessor *Monitor = nullptr;
  // Per-class monitor initializer; override to provide custom monitor accessor
  virtual UCoreAccessor *MonitorInit();

  // Logic I/O configuration: which signals to export/import and control rules
  UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
  ULogicSettings *LogicSettings = nullptr;

  // Dimension
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  ADimension *mDimension = nullptr;

  // Internal state
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  bool Working;

  TArray<int32> AccessorInstances;
};

UCLASS(BlueprintType)
class UPartBlockLogic : public UBlockLogic {
  GENERATED_BODY()

  public:
  // Serialization
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  // Widgets and UI
  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;
  virtual void OpenWidget(class UHudWidget *widget) override;

  // Hierarchy and accessors
  virtual UBlockLogic *GetPartRootBlock() override;
  virtual TArray<UAccessor *> GetAccessors() override;

  // Core accessors
  virtual UCoreAccessor *GetCoreAccessor() override;
  virtual UCoreAccessor *GetMonitorAccessor() override;

  // Part marker
  virtual bool IsPart() override { return true; }

  // Actor overrides
  virtual ABlockActor *GetActor() override;
  virtual void SetActor(ABlockActor *actor);

  // Actions
  virtual void OnAction(const FHitResult &hit, const Vec3i &side, AItemLogic *item);

  // Variables
  UPROPERTY()
  UBlockLogic *mParentBlock;
};
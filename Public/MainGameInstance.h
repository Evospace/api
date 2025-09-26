// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "MapWidgetData.h"
#include "Public/WorldGenerator.h"
#include "SteamGameInstance.h"
#include "Modding/LegacyLuaState.h"
#include "Qr/LocalizationTable.h"

#include <Containers/Array.h>
#include <Containers/Map.h>
#include <Engine/DPICustomScalingRule.h>
#include <Engine/GameInstance.h>

#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#endif

#include "MainGameInstance.generated.h"

class UEngineData;
class UMapgenData;
class UNeiComponent;
class URegionMap;
class AViewCapture;
class UDB;
class UInventoryAccess;
class UUserWidget;
class UStaticBlock;
class UStaticItem;
class UAssetOwner;
class UMapObjectManager;
class UStaticMesh;

UENUM(BlueprintType)
enum class EJoyImageFormats : uint8 {
  JPG UMETA(DisplayName = "JPG        "),
  PNG UMETA(DisplayName = "PNG        "),
  BMP UMETA(DisplayName = "BMP        "),
  ICO UMETA(DisplayName = "ICO        "),
  EXR UMETA(DisplayName = "EXR        "),
  ICNS UMETA(DisplayName = "ICNS        ")
};

USTRUCT(BlueprintType)
struct FScreenResolution {
  GENERATED_BODY()

  public:
  uint32 Width;
  uint32 Height;
};

UCLASS()
class UCustomDPI : public UDPICustomScalingRule {
  GENERATED_BODY()

  public:
  virtual float GetDPIScaleBasedOnSize(FIntPoint Size) const override;
};

UCLASS()
class UMainGameInstance : public USteamGameInstance {
  GENERATED_BODY()
  void lua_reg(lua_State *L) {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UMainGameInstance, UObject>("Game") //@class Game : Object
      .addProperty("engine_data", &UMainGameInstance::EngineData) //@field EngineData
      .addProperty("mapgen_data", &UMainGameInstance::MapgenData) //@field MapgenData
      .addStaticFunction("get_supported_resolutions", &UMainGameInstance::GetAllSupportedResolutions) //@field EngineData
      .addProperty("localization", &UMainGameInstance::GetLocalization, &UMainGameInstance::SetLocalization) //@field string
      .addProperty("build_string", &UMainGameInstance::GetBuildLuaString) //@field string
      .endClass();
    lua_reg_detail(L);
  }
  void lua_reg_detail(lua_State *L);

  public:
  UMainGameInstance();

  static inline UMainGameInstance *Singleton = nullptr;

  UFUNCTION(BlueprintCallable)
  UInventoryAccess *GetNamedInventory(const FString &name);

  UFUNCTION(BlueprintCallable)
  static TArray<UStaticTip *> GetAllStaticTips();

  UFUNCTION(BlueprintCallable)
  UWorldGenerator *GetWorldGenerator() const;

  UFUNCTION(BlueprintCallable)
  TArray<UWorldGenerator *> GetWorldGeneratorList();

  UFUNCTION(BlueprintCallable)
  void SetWorldGenerator(UWorldGenerator *gen);

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UMapWidgetData *MapWidgetData;

  private:
  UPROPERTY()
  TArray<UWorldGenerator *> mWorldGenerators;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UWorldGenerator *mWorldGenerator;

  public:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UNeiComponent *mNei;

  //TODO: what is it?
  UFUNCTION(BlueprintCallable, BlueprintPure)
  static float GetNextTime(float time, float delta);

  //TODO: check usage
  UFUNCTION(BlueprintCallable)
  static void SetDPI(float scale);

  //TODO: check usage
  static float DPIScale;

  template <typename _Ty>
  static _Ty *GetOrCreatePrototype(const FString &name) {
    auto cdo = Cast<_Ty>(_Ty::StaticClass()->GetDefaultObject());
    auto o = cdo->get_or_register(name, *Singleton->mJsonObjectLibrary);
    return Cast<_Ty>(o);
  }

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static FString StringFromInt64(const int64 s);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static FString StringHexFromInt64(const int64 s);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static FString StringBinFromInt64(int64 s);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static float Smoothstep(float edge0, float edge1, float x);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static bool IsValidSteamString(const FString &s);

  UFUNCTION(BlueprintCallable)
  static void ClipboardPaste(FString &dest);

  UFUNCTION(BlueprintCallable)
  static void ClipboardCopy(const FString &s);

  /** Get the coordinates of the center of the player's screen / viewport.
   * Returns false if the operation could not occur */
  UFUNCTION(BlueprintPure)
  static bool Viewport__GetCenterOfViewport(const APlayerController *ThePC, float &PosX, float &PosY);

  UFUNCTION(BlueprintCallable, BlueprintCosmetic, Meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "WidgetClass"))
  static UUserWidget *GetFirstWidgetOfClass(UObject *WorldContextObject, TSubclassOf<UUserWidget> WidgetClass, bool TopLevelOnly);

  /** Only hardware dependent, no smoothing */
  UFUNCTION(BlueprintCallable)
  static void GraphicsSettings__SetFrameRateToBeUnbound();

  /** Set the Max Frame Rate. Min value is 10. */
  UFUNCTION(BlueprintCallable)
  static void GraphicsSettings__SetFrameRateCap(float NewValue);

  UFUNCTION(BlueprintCallable)
  void OnWindowFocusChanged(bool bIsFocused);

  UDbStorage *GetObjectLibrary();

  UFUNCTION(BlueprintCallable)
  void PostModLoad();

  UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
  TArray<FSteamUGCDetails> mSubscriptionDetails;

  UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
  bool mIsSubscriptionQueryDone = false;

  UFUNCTION(BlueprintCallable)
  static void SetSoundClassVolume(USoundClass *sound_class, float volume);

  UFUNCTION(BlueprintCallable)
  static float GetSoundClassVolume(USoundClass *sound_class);

  UFUNCTION(BlueprintCallable)
  static bool IsSaveExists(FString name);

  UFUNCTION(BlueprintCallable)
  static bool DeleteSave(FString name);

  UFUNCTION(BlueprintCallable)
  static FString GetBuildString();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static bool IsDemoVersion();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static bool DemoFilterCategory(FName cat);

  std::string GetBuildLuaString() const;

  UFUNCTION(BlueprintCallable)
  static UTexture2D *Victory_LoadTexture2D_FromFile(const FString &FullFilePath, bool &IsValid, int32 &Width, int32 &Height);

  static UTexture2D *LoadTexture2DFromFile(const FString &FullFilePath);

  static UTexture2D *Victory_LoadTexture2D_FromArray(const TArray<FColor> &RawFileData, bool &IsValid, int32 &Width, int32 &Height);

  UFUNCTION(BlueprintCallable)
  static TArray<FAssetData> ListAllAssets();

  UFUNCTION(BlueprintCallable)
  static UClass *GetClassFromAssetData(const FAssetData &InAssetData);

  UFUNCTION(BlueprintCallable)
  static void SetCameraFromOriginAndBoxExtent(AActor *Camera, const FVector &Origin, const FVector &BoxExtent);

  static bool Victory_SavePixels(const FString &FullFilePath, int32 Width, int32 Height, const TArray<FColor> &ImagePixels, bool sRGB);

  UFUNCTION(BlueprintCallable)
  static int32 GreatestCommonDivisor(int32 a, int32 b);

  UFUNCTION(BlueprintCallable)
  static TArray<FString> GetLocalizations();

  UFUNCTION(BlueprintCallable)
  static void ChangeLocalization(FString target);

  void SetLocalization(std::string culture);
  std::string GetLocalization() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  static void OpenDiscord();

  UFUNCTION(BlueprintCallable)
  static TArray<FString> GetErrors();

  UFUNCTION(BlueprintCallable)
  static bool HasErrors();

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  static bool ContainsAllResearches(const TSet<UStaticResearchRecipe *> where, const TArray<UStaticResearchRecipe *> what);

  UFUNCTION(BlueprintCallable)
  void SetFilterString(const FString &str);

  UPROPERTY(BlueprintReadWrite)
  TArray<UStaticItem *> FilteredItems;

  UPROPERTY(BlueprintReadWrite)
  TArray<UStaticBlock *> FilteredBlocks;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  bool mTickFailed = false;

  UPROPERTY(BlueprintReadWrite)
  int32 VanillaJsonCount = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  int32 ModdedJsonCount = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  int32 ModdedAssetCount = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  FString LoadingMessage = "";

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  AViewCapture *mCaptureActor;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  UEngineData *EngineData;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  UMapgenData *MapgenData;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UMapObjectManager *MapObjectManager;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static FLinearColor IndexToColor(int32 index);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static FLinearColor HashToColor(const UStaticItem *Pointer);

  static FLinearColor VoidToColor(const void *Pointer);

  static std::vector<std::string> GetAllSupportedResolutions();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static FVector2D ViewportSize();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static float GetDPI();

  //TODO: move to subsystem
  static void ResearchReset();

  UFUNCTION(BlueprintCallable)
  FString GetCurrentRHI();

  UFUNCTION(BlueprintCallable)
  FString GetLastLoadingMessage() const;

  UPROPERTY(BlueprintReadWrite)
  bool mSteamworksIsReady = false;

  static bool IsPIEPaused() {
#if WITH_EDITOR
    if (GEditor) {
      if (GEditor->PlayWorld) {
        return GEditor->PlayWorld->IsPaused();
      }
    }
#endif

    return false;
  }

  protected:
  virtual void OnStart() override;

  UFUNCTION(BlueprintCallable)
  void ModLoadingFinalStep();

  bool LoadTick(float Delta);

  virtual void Shutdown() override;

  FString mFilterString;

  public:
  // Legacy aliases to subsystem-owned objects. Will be removed after refactor.

  UObject *GetAssetOwner() const;

  FTSTicker::FDelegateHandle TickDelegateHandle;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UDB *DB;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UDbStorage *mJsonObjectLibrary;

  UPROPERTY()
  TMap<UClass *, UAssetOwner *> mAssetTypeOwners;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FDateTime mLastSave = {};

  TArray<UStaticItem *> mUnfilteredItems;
  TArray<UStaticBlock *> mUnfilteredBlocks;
};

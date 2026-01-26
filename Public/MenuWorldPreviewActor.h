// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Qr/Vector.h"

#include "MenuWorldPreviewActor.generated.h"

class ADimension;
class USectorArea;
class USurfaceDefinition;

UCLASS(BlueprintType, Blueprintable)
class EVOSPACE_API AMenuWorldPreviewActor : public AActor {
  GENERATED_BODY()

  public:
  AMenuWorldPreviewActor();

  UFUNCTION(BlueprintCallable, Category = "Evospace|MenuWorld")
  bool InitializePreview();

  UFUNCTION(BlueprintCallable, Category = "Evospace|MenuWorld")
  void UpdatePreviewArea();

  UFUNCTION(BlueprintCallable, Category = "Evospace|MenuWorld")
  void SetFixedSectorPos(FVector3i NewSectorPos);

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evospace|MenuWorld")
  FString SaveName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evospace|MenuWorld")
  FString SurfaceName = TEXT("Temperate");

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evospace|MenuWorld")
  int32 SectorRadius = 4;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evospace|MenuWorld")
  bool bInitializeOnBeginPlay = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evospace|MenuWorld")
  bool bUpdateSectorsFromActor = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evospace|MenuWorld")
  bool bSpawnDimensionIfMissing = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evospace|MenuWorld")
  TSubclassOf<ADimension> DimensionClass;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Evospace|MenuWorld")
  ADimension *Dimension = nullptr;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Evospace|MenuWorld")
  USectorArea *SectorArea = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evospace|MenuWorld")
  FVector3i FixedSectorPos;

  protected:
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaSeconds) override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  private:
  UFUNCTION()
  void OnModsLoaded();
  ADimension *FindOrSpawnDimension();
  USurfaceDefinition *LoadSurfaceFromTemp();

  bool bModsLoaded = false;
  bool bInitializationAttempted = false;
};

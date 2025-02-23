#pragma once
#include "Prototype.h"
#include "ChangelogStorage.generated.h"

class UAutosizeInventory;
class UDB;

USTRUCT(BlueprintType)
struct FChangelog {
  GENERATED_BODY()

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  FString label;
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  FString description;
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  FDateTime dt;
};

UCLASS(BlueprintType)
/**
 * 
 */
class UChangelogStorage : public UObject {
  GENERATED_BODY()
  public:
  UChangelogStorage();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static FChangelog GetChangelog(int32 index);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static FString GetChangelogMini(int32 index, FDateTime &dt);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static int32 GetChangelogCount();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static bool CheckAndUpdateLastEntry();

  static const std::vector<FChangelog> &GetStorage();
};
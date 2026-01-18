#pragma once
#include "Qr/Prototype.h"
#include "ChangelogStorage.generated.h"

class UAutosizeInventory;
class UDB;

USTRUCT(BlueprintType)
struct FChangelogCategory {
  GENERATED_BODY()

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  FString name;
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  TArray<FString> items;
};

USTRUCT(BlueprintType)
struct FChangelog {
  GENERATED_BODY()

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  FString label;
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  FString description;
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  FDateTime dt;
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  int32 isImportant = 0;
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  TArray<FString> tags;
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  TArray<FChangelogCategory> category;
};

UCLASS(BlueprintType)
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
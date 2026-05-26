// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Misc/Paths.h"

#include "SaveSourceTypes.generated.h"

UENUM(BlueprintType)
enum class ESaveSourceType : uint8 {
  LocalSlot = 0,
  ShippingBuiltIn = 1,
  NetworkArchive = 2,
  WorkshopPackage = 3,
};

USTRUCT(BlueprintType)
struct FSaveSourceDescriptor {
  GENERATED_BODY()

  UPROPERTY(BlueprintReadWrite, Category = "Evospace|Save")
  ESaveSourceType SourceType = ESaveSourceType::LocalSlot;

  /** Local save slot name under Saved/SaveGames. */
  UPROPERTY(BlueprintReadWrite, Category = "Evospace|Save")
  FString SlotName;

  /** Shipping built-in save id (Content/SaveGames/<BuiltInPackageId>). */
  UPROPERTY(BlueprintReadWrite, Category = "Evospace|Save")
  FString BuiltInPackageId;

  /** Packed save bytes for network/import sources. */
  UPROPERTY(BlueprintReadWrite, Category = "Evospace|Save")
  TArray<uint8> ArchiveData;

  /** Optional CRC32 of ArchiveData; validated when non-zero. */
  UPROPERTY(BlueprintReadWrite, Category = "Evospace|Save")
  int32 ArchiveCrc = 0;

  /** Steam Workshop published file id. */
  UPROPERTY(BlueprintReadWrite, Category = "Evospace|Save")
  int64 WorkshopPublishedFileId = 0;

  /** Absolute or project-relative path to workshop/shipping package root or archive file. */
  UPROPERTY(BlueprintReadWrite, Category = "Evospace|Save")
  FString PackagePath;

  /** Optional path relative to PackagePath or workshop install folder. */
  UPROPERTY(BlueprintReadWrite, Category = "Evospace|Save")
  FString RelativeContentPath;

  /** Display name stored in session data; defaults per source type when empty. */
  UPROPERTY(BlueprintReadWrite, Category = "Evospace|Save")
  FString DisplaySaveName;

  static FSaveSourceDescriptor MakeLocalSlot(const FString &InSlotName) {
    FSaveSourceDescriptor descriptor;
    descriptor.SourceType = ESaveSourceType::LocalSlot;
    descriptor.SlotName = InSlotName;
    descriptor.DisplaySaveName = InSlotName;
    return descriptor;
  }

  static FSaveSourceDescriptor MakeNetworkArchive(const TArray<uint8> &InArchiveData, const FString &InDisplaySaveName,
                                                  int32 InArchiveCrc = 0) {
    FSaveSourceDescriptor descriptor;
    descriptor.SourceType = ESaveSourceType::NetworkArchive;
    descriptor.ArchiveData = InArchiveData;
    descriptor.ArchiveCrc = InArchiveCrc;
    descriptor.DisplaySaveName = InDisplaySaveName;
    return descriptor;
  }

  static FSaveSourceDescriptor MakeShippingBuiltIn(const FString &InBuiltInPackageId,
                                                   const FString &InDisplaySaveName = FString()) {
    FSaveSourceDescriptor descriptor;
    descriptor.SourceType = ESaveSourceType::ShippingBuiltIn;
    descriptor.BuiltInPackageId = InBuiltInPackageId;
    descriptor.DisplaySaveName = InDisplaySaveName.IsEmpty() ? InBuiltInPackageId : InDisplaySaveName;
    return descriptor;
  }

  static FSaveSourceDescriptor MakeWorkshopPackage(int64 InPublishedFileId, const FString &InDisplaySaveName = FString(),
                                                   const FString &InRelativeContentPath = FString()) {
    FSaveSourceDescriptor descriptor;
    descriptor.SourceType = ESaveSourceType::WorkshopPackage;
    descriptor.WorkshopPublishedFileId = InPublishedFileId;
    descriptor.RelativeContentPath = InRelativeContentPath;
    descriptor.DisplaySaveName =
      InDisplaySaveName.IsEmpty() ? FString::Printf(TEXT("Workshop %lld"), InPublishedFileId) : InDisplaySaveName;
    return descriptor;
  }

  static FSaveSourceDescriptor MakeWorkshopPackageFromPath(const FString &InPackagePath,
                                                           const FString &InDisplaySaveName = FString()) {
    FSaveSourceDescriptor descriptor;
    descriptor.SourceType = ESaveSourceType::WorkshopPackage;
    descriptor.PackagePath = InPackagePath;
    descriptor.DisplaySaveName = InDisplaySaveName.IsEmpty() ? FPaths::GetBaseFilename(InPackagePath) : InDisplaySaveName;
    return descriptor;
  }
};

USTRUCT(BlueprintType)
struct FPreparedSaveContext {
  GENERATED_BODY()

  UPROPERTY(BlueprintReadOnly, Category = "Evospace|Save")
  ESaveSourceType SourceType = ESaveSourceType::LocalSlot;

  /** Absolute path to staged save root (SaveGames/<StagingSlotName>). */
  UPROPERTY(BlueprintReadOnly, Category = "Evospace|Save")
  FString StagingRootPath;

  /** Slot folder name under SaveGames used for runtime load (currently always "temp"). */
  UPROPERTY(BlueprintReadOnly, Category = "Evospace|Save")
  FString StagingSlotName;

  /** Save name written into session data and UI. */
  UPROPERTY(BlueprintReadOnly, Category = "Evospace|Save")
  FString DisplaySaveName;

  /** True when the origin must not be written back in place (network/shipping/workshop). */
  UPROPERTY(BlueprintReadOnly, Category = "Evospace|Save")
  bool ReadOnly = false;

  UPROPERTY(BlueprintReadOnly, Category = "Evospace|Save")
  int32 ArchiveCrc = 0;
};

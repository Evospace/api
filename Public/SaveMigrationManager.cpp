// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/SaveMigrationManager.h"

#include "Engine/GameInstance.h"
#include "Qr/StaticSaveHelpers.h"
#include "Logging/StructuredLog.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/FileManager.h"
#include "Public/SurfaceDefinition.h"
#include "Public/RegionMap.h"
#include <Serialization/ArchiveLoadCompressedProxy.h>
#include <Serialization/ArchiveSaveCompressedProxy.h>
#include <Serialization/BufferArchive.h>

namespace {
static const auto TierArray = {
  "Copper",
  "Steel",
  "Aluminium",
  "StainlessSteel",
  "Titanium",
  "HardMetal",
  "Neutronium",
};
}

void USaveMigrationManager::RunMigrationsIfNeeded(const FString &saveName, UGameInstance *GameInstance) {
  if (!GameInstance) {
    LOG(ERROR_LL) << "SaveMigrationManager: GameInstance is null";
    return;
  }

  bool gameSessionFallback = false;
  UGameSessionData *loadedSession = UStaticSaveHelpers::LoadGameSessionData(GameInstance, saveName);
  if (!loadedSession) {
    LOG(ERROR_LL) << "SaveMigrationManager: No GameSessionData found for save '" << saveName << "'";
    loadedSession = NewObject<UGameSessionData>(GameInstance, TEXT("GameSessionData"));
    loadedSession->Version = FVersionStruct{ 0, 19, 0, 0, TEXT("?") };
    loadedSession->Mods = {};
    gameSessionFallback = true;
  }

  const FVersionStruct saveVersion = loadedSession->Version;

  struct FMigrator {
    FVersionStruct Target;
    TFunction<bool(const FString &, UGameInstance *GameInstance)> Action;
  };

  TArray<FMigrator> migrators;
  migrators.Reserve(8);

  // Convert Logic.json to Logic.bin for saves from 0.18.2
  migrators.Add({ FVersionStruct{ 0, 19, 0, 1, TEXT("*") }, [](const FString &saveName, UGameInstance *GameInstance) {
                   const FString saveRoot = FPaths::ProjectSavedDir() / TEXT("SaveGames") / saveName;
                   const FString logicJsonPath = saveRoot / TEXT("Dimension") / TEXT("Logic.json");
                   const FString logicBinPath = saveRoot / TEXT("Dimension") / TEXT("Logic.bin");

                   // Load Logic.json
                   FString jsonStr;
                   if (!FFileHelper::LoadFileToString(jsonStr, *logicJsonPath)) {
                     return true; // No Logic.json to convert
                   }

                   if (jsonStr.IsEmpty()) {
                     LOG(WARN_LL) << "SaveMigrationManager: Logic.json is empty for save '" << saveName << "'";
                     return true;
                   }

                   // Write to compressed binary format (same as current save format)
                   FBufferArchive uncompressedData;
                   FMemoryWriter memoryWriter(uncompressedData, true);
                   int32 version = 0; // Use version 0 as in current saves
                   memoryWriter << version;
                   memoryWriter << jsonStr;

                   // Compress the data with ZLIB
                   TArray<uint8> compressedData;
                   FArchiveSaveCompressedProxy compressor(compressedData, FName("ZLIB"));
                   compressor << uncompressedData;
                   compressor.Flush();

                   // Save the compressed data as Logic.bin
                   if (!FFileHelper::SaveArrayToFile(compressedData, *logicBinPath)) {
                     LOG(ERROR_LL) << "SaveMigrationManager: Failed to save Logic.bin for save '" << saveName << "'";
                     return false;
                   }

                   LOG(INFO_LL) << "SaveMigrationManager: Converted Logic.json to Logic.bin for save '" << saveName << "'";

                   RemoveSingleStaticBlocksFromLogicJson(saveName);

                   return true;
                 } });

  // Rename Dimension to Temperate, move Player.json and Preview.jpg to save root, rename Dimension.json to
  // GameSessionData.json
  migrators.Add(
    { FVersionStruct{ 0, 20, 1, 8, TEXT("*") }, [](const FString &saveName, UGameInstance *GameInstance) {
       IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
       IFileManager &FileManager = IFileManager::Get();

       auto saveRoot = FPaths::ProjectSavedDir() / TEXT("SaveGames") / saveName;

       const FString dimensionDir = saveRoot / TEXT("Dimension");
       const FString temperateDir = saveRoot / TEXT("Temperate");

       if (!FileManager.Move(*temperateDir, *dimensionDir)) {
         LOG(ERROR_LL) << "SaveMigrationManager: Failed to rename 'Dimension' to 'Temperate' for '" << saveName
                       << "'";
         return false;
       }

       const FString playerSrc = temperateDir / TEXT("player.json");
       const FString playerDst = saveRoot / TEXT("Player.json");
       if (PlatformFile.FileExists(*playerSrc)) {
         if (!PlatformFile.MoveFile(*playerDst, *playerSrc)) {
           LOG(ERROR_LL) << "SaveMigrationManager: Failed to move Player.json to save root for '" << saveName
                         << "'";
           return false;
         }
       }

       const FString previewSrc = temperateDir / TEXT("Preview.jpg");
       const FString previewDst = saveRoot / TEXT("Preview.jpg");
       if (PlatformFile.FileExists(*previewSrc)) {
         if (!PlatformFile.MoveFile(*previewDst, *previewSrc)) {
           LOG(ERROR_LL) << "SaveMigrationManager: Failed to move Preview.jpg to save root for '" << saveName
                         << "'";
           return false;
         }
       }

       // Move Dimension.json into save root
       const FString dimJsonSrc = temperateDir / TEXT("Dimension.json");
       const FString dimJsonDst = saveRoot / TEXT("GameSessionData.json");
       if (PlatformFile.FileExists(*dimJsonSrc)) {
         if (!PlatformFile.MoveFile(*dimJsonDst, *dimJsonSrc)) {
           LOG(ERROR_LL) << "SaveMigrationManager: Failed to move Dimension.json to save root for '"
                         << saveName << "'";
           return false;
         }
       }

       // Load GameSessionData.json
       const FString gameSessionDataPath = saveRoot / TEXT("GameSessionData.json");
       FString gameSessionDataStr;
       if (FFileHelper::LoadFileToString(gameSessionDataStr, *gameSessionDataPath)) {
         TSharedPtr<FJsonObject> gameSessionDataObj;
         TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(gameSessionDataStr);
         if (FJsonSerializer::Deserialize(reader, gameSessionDataObj) && gameSessionDataObj.IsValid()) {
           // Extract "Deposits" field if it exists

           FString generatorName;
           TSharedPtr<FJsonObject> gameSessionDataFieldObj;
           if (TSharedPtr<FJsonValue> gameSessionDataFieldValue =
                 gameSessionDataObj->TryGetField(TEXT("GameSessionData"))) {
             gameSessionDataFieldObj = gameSessionDataFieldValue->AsObject();
           }
           if (!gameSessionDataFieldObj.IsValid() ||
               !gameSessionDataFieldObj->TryGetStringField(TEXT("Generator"), generatorName)) {
             LOG(ERROR_LL) << "SaveMigrationManager: Failed to get Generator from "
                              "GameSessionData.GameSessionData for '"
                           << saveName << "'";
           }

           if (TSharedPtr<FJsonValue> depositsValue = gameSessionDataObj->TryGetField(TEXT("Deposits"))) {
             // Remove "Deposits" from the main object
             gameSessionDataObj->RemoveField(TEXT("Deposits"));

             // Write the rest of GameSessionData.json back (without Deposits)
             FString updatedGameSessionDataStr;
             const auto writer = TJsonWriterFactory<>::Create(&updatedGameSessionDataStr);
             FJsonSerializer::Serialize(gameSessionDataObj.ToSharedRef(), writer);
             FFileHelper::SaveStringToFile(updatedGameSessionDataStr, *gameSessionDataPath);

             // Write Deposits to a new JSON file (for now, just as a separate object)
             TSharedPtr<FJsonObject> depositsObj = MakeShared<FJsonObject>();
             depositsObj->SetField(TEXT("Deposits"), depositsValue);

             USurfaceDefinition *surfaceDefinition = NewObject<USurfaceDefinition>(GameInstance);
             surfaceDefinition->GeneratorName = *generatorName;
             surfaceDefinition->Initialize();

             surfaceDefinition->RegionMap->DeserializeJson(depositsValue->AsObject());

             UStaticSaveHelpers::SaveSurfaceDefinition(saveName, TEXT("Temperate"), surfaceDefinition);
           }
         }
       }

       return true;
     } });

  // Update Temperate/SurfaceDefinition.json: set CurrentOre = 10 * InitialCapacity for every source, remove
  // ExtractedCount for every source
  migrators.Add({ FVersionStruct{ 0, 20, 1, 16, TEXT("*") }, [](const FString &saveName, UGameInstance *GameInstance) {
                   const FString saveRoot = FPaths::ProjectSavedDir() / TEXT("SaveGames") / saveName;
                   const FString surfacePath = saveRoot / TEXT("Temperate") / TEXT("SurfaceDefinition.json");

                   FString jsonStr;
                   if (!FFileHelper::LoadFileToString(jsonStr, *surfacePath)) {
                     return true; // Nothing to migrate
                   }

                   TSharedPtr<FJsonObject> rootObj;
                   TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(jsonStr);
                   if (!FJsonSerializer::Deserialize(reader, rootObj) || !rootObj.IsValid()) {
                     LOG(ERROR_LL) << "SaveMigrationManager: Failed to parse SurfaceDefinition.json for '"
                                   << saveName << "'";
                     return false;
                   }

                   TSharedPtr<FJsonObject> surfaceObj;
                   if (TSharedPtr<FJsonValue> surfaceVal = rootObj->TryGetField(TEXT("SurfaceDefinition"))) {
                     surfaceObj = surfaceVal->AsObject();
                   }
                   if (!surfaceObj.IsValid()) {
                     return true; // Unexpected structure; skip
                   }

                   TSharedPtr<FJsonObject> regionMapObj;
                   if (TSharedPtr<FJsonValue> regionMapVal = surfaceObj->TryGetField(TEXT("RegionMap"))) {
                     regionMapObj = regionMapVal->AsObject();
                   }
                   if (!regionMapObj.IsValid()) {
                     return true; // No regions to process
                   }

                   const TArray<TSharedPtr<FJsonValue>> *knownArrayPtr = nullptr;
                   if (!regionMapObj->TryGetArrayField(TEXT("Known"), knownArrayPtr) || !knownArrayPtr) {
                     return true; // Nothing known
                   }

                   // We'll clone to allow modification
                   TArray<TSharedPtr<FJsonValue>> knownArray = *knownArrayPtr;
                   for (TSharedPtr<FJsonValue> &entryVal : knownArray) {
                     if (!entryVal.IsValid())
                       continue;
                     TSharedPtr<FJsonObject> entryObj = entryVal->AsObject();
                     if (!entryObj.IsValid())
                       continue;
                     TSharedPtr<FJsonObject> valueObj;
                     if (TSharedPtr<FJsonValue> valueVal = entryObj->TryGetField(TEXT("Value"))) {
                       valueObj = valueVal->AsObject();
                     }
                     if (!valueObj.IsValid())
                       continue;

                     const TArray<TSharedPtr<FJsonValue>> *sourcesPtr = nullptr;
                     if (!valueObj->TryGetArrayField(TEXT("Sources"), sourcesPtr) || !sourcesPtr)
                       continue;
                     TArray<TSharedPtr<FJsonValue>> sources = *sourcesPtr;
                     for (TSharedPtr<FJsonValue> &srcVal : sources) {
                       if (!srcVal.IsValid())
                         continue;
                       TSharedPtr<FJsonObject> srcObj = srcVal->AsObject();
                       if (!srcObj.IsValid())
                         continue;

                       // Remove legacy field
                       srcObj->RemoveField(TEXT("ExtractedCount"));

                       // Read InitialCapacity
                       double initialCapacityD = 0.0;
                       int64 initialCapacityI = 0;
                       if (srcObj->TryGetNumberField(TEXT("InitialCapacity"), initialCapacityD)) {
                         initialCapacityI = static_cast<int64>(initialCapacityD);
                       }
                       // Set CurrentOre = 10 * InitialCapacity (only if InitialCapacity is present)
                       if (initialCapacityI > 0) {
                         const double currentOreD = static_cast<double>(initialCapacityI * 10);
                         srcObj->SetNumberField(TEXT("CurrentOre"), currentOreD);
                       }
                     }
                     // Write back possibly modified sources array
                     valueObj->SetArrayField(TEXT("Sources"), sources);
                   }

                   // Write back possibly modified known array
                   regionMapObj->SetArrayField(TEXT("Known"), knownArray);

                   // Save updated JSON
                   FString outStr;
                   const auto writer = TJsonWriterFactory<>::Create(&outStr);
                   FJsonSerializer::Serialize(rootObj.ToSharedRef(), writer);
                   if (!FFileHelper::SaveStringToFile(outStr, *surfacePath)) {
                     LOG(ERROR_LL) << "SaveMigrationManager: Failed to write SurfaceDefinition.json for '"
                                   << saveName << "'";
                     return false;
                   }

                   return true;
                 } });

  // Update Temperate/SurfaceDefinition.json: for each region set FertileLayer/ OilLayer subregions
  // CurrentValue = 10 * InitialCapacity
  migrators.Add({ FVersionStruct{ 0, 20, 1, 36, TEXT("*") },
                  [](const FString &saveName, UGameInstance *GameInstance) {
                    const FString saveRoot = FPaths::ProjectSavedDir() / TEXT("SaveGames") / saveName;
                    const FString surfacePath = saveRoot / TEXT("Temperate") / TEXT("SurfaceDefinition.json");

                    FString jsonStr;
                    if (!FFileHelper::LoadFileToString(jsonStr, *surfacePath)) {
                      return true; // Nothing to migrate
                    }

                    TSharedPtr<FJsonObject> rootObj;
                    TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(jsonStr);
                    if (!FJsonSerializer::Deserialize(reader, rootObj) || !rootObj.IsValid()) {
                      LOG(ERROR_LL) << "SaveMigrationManager: Failed to parse SurfaceDefinition.json for '"
                                    << saveName << "'";
                      return false;
                    }

                    TSharedPtr<FJsonObject> surfaceObj;
                    if (TSharedPtr<FJsonValue> surfaceVal = rootObj->TryGetField(TEXT("SurfaceDefinition"))) {
                      surfaceObj = surfaceVal->AsObject();
                    }
                    if (!surfaceObj.IsValid()) {
                      return true; // Unexpected structure; skip
                    }

                    TSharedPtr<FJsonObject> regionMapObj;
                    if (TSharedPtr<FJsonValue> regionMapVal = surfaceObj->TryGetField(TEXT("RegionMap"))) {
                      regionMapObj = regionMapVal->AsObject();
                    }
                    if (!regionMapObj.IsValid()) {
                      return true; // No regions to process
                    }

                    const TArray<TSharedPtr<FJsonValue>> *knownArrayPtr = nullptr;
                    if (!regionMapObj->TryGetArrayField(TEXT("Known"), knownArrayPtr) || !knownArrayPtr) {
                      return true; // Nothing known
                    }

                    auto updateLayer = [](TSharedPtr<FJsonObject> layerObj) {
                      if (!layerObj.IsValid())
                        return;
                      const TArray<TSharedPtr<FJsonValue>> *subsPtr = nullptr;
                      if (!layerObj->TryGetArrayField(TEXT("InitialCapacity"), subsPtr) || !subsPtr)
                        return;
                      TArray<TSharedPtr<FJsonValue>> subs = *subsPtr;
                      for (TSharedPtr<FJsonValue> &subVal : subs) {
                        if (!subVal.IsValid())
                          continue;
                        TSharedPtr<FJsonObject> subObj = subVal->AsObject();
                        if (!subObj.IsValid())
                          continue;
                        double initialCapacityD = 0.0;
                        int64 initialCapacityI = 0;
                        if (subObj->TryGetNumberField(TEXT("InitialCapacity"), initialCapacityD)) {
                          initialCapacityI = static_cast<int64>(initialCapacityD);
                        }
                        if (initialCapacityI > 0) {
                          const double currentVal = static_cast<double>(initialCapacityI * 10);
                          subObj->SetNumberField(TEXT("CurrentValue"), currentVal);
                        }
                      }
                      layerObj->RemoveField(TEXT("InitialCapacity"));
                      layerObj->SetArrayField(TEXT("Data"), subs);
                    };

                    // Clone Known to allow modification
                    TArray<TSharedPtr<FJsonValue>> knownArray = *knownArrayPtr;
                    for (TSharedPtr<FJsonValue> &entryVal : knownArray) {
                      if (!entryVal.IsValid())
                        continue;
                      TSharedPtr<FJsonObject> entryObj = entryVal->AsObject();
                      if (!entryObj.IsValid())
                        continue;
                      TSharedPtr<FJsonObject> regionObj;
                      if (TSharedPtr<FJsonValue> valueVal = entryObj->TryGetField(TEXT("Value"))) {
                        regionObj = valueVal->AsObject();
                      }
                      if (!regionObj.IsValid())
                        continue;

                      // FertileLayer
                      if (TSharedPtr<FJsonValue> fertileVal = regionObj->TryGetField(TEXT("FertileLayer"))) {
                        updateLayer(fertileVal->AsObject());
                      }
                      // OilLayer
                      if (TSharedPtr<FJsonValue> oilVal = regionObj->TryGetField(TEXT("OilLayer"))) {
                        updateLayer(oilVal->AsObject());
                      }
                    }

                    // Write back possibly modified known array
                    regionMapObj->SetArrayField(TEXT("Known"), knownArray);

                    // Save updated JSON
                    FString outStr;
                    const auto writer = TJsonWriterFactory<>::Create(&outStr);
                    FJsonSerializer::Serialize(rootObj.ToSharedRef(), writer);
                    if (!FFileHelper::SaveStringToFile(outStr, *surfacePath)) {
                      LOG(ERROR_LL) << "SaveMigrationManager: Failed to write SurfaceDefinition.json for '"
                                    << saveName << "'";
                      return false;
                    }

                    return true;
                  } });

  // Fix broken ore sources with empty Item/Prop by setting them to Pyrite
  migrators.Add({ FVersionStruct{ 0, 20, 1, 123, TEXT("*") }, [](const FString &saveName, UGameInstance *GameInstance) {
                   const FString saveRoot = FPaths::ProjectSavedDir() / TEXT("SaveGames") / saveName;
                   const FString surfacePath = saveRoot / TEXT("Temperate") / TEXT("SurfaceDefinition.json");

                   FString jsonStr;
                   if (!FFileHelper::LoadFileToString(jsonStr, *surfacePath)) {
                     return true; // Nothing to migrate
                   }

                   TSharedPtr<FJsonObject> rootObj;
                   TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(jsonStr);
                   if (!FJsonSerializer::Deserialize(reader, rootObj) || !rootObj.IsValid()) {
                     LOG(ERROR_LL) << "SaveMigrationManager: Failed to parse SurfaceDefinition.json for '"
                                   << saveName << "'";
                     return false;
                   }

                   TSharedPtr<FJsonObject> surfaceObj;
                   if (TSharedPtr<FJsonValue> surfaceVal = rootObj->TryGetField(TEXT("SurfaceDefinition"))) {
                     surfaceObj = surfaceVal->AsObject();
                   }
                   if (!surfaceObj.IsValid()) {
                     return true; // Unexpected structure; skip
                   }

                   TSharedPtr<FJsonObject> regionMapObj;
                   if (TSharedPtr<FJsonValue> regionMapVal = surfaceObj->TryGetField(TEXT("RegionMap"))) {
                     regionMapObj = regionMapVal->AsObject();
                   }
                   if (!regionMapObj.IsValid()) {
                     return true; // No regions to process
                   }

                   const TArray<TSharedPtr<FJsonValue>> *knownArrayPtr = nullptr;
                   if (!regionMapObj->TryGetArrayField(TEXT("Known"), knownArrayPtr) || !knownArrayPtr) {
                     return true; // Nothing known
                   }

                   // Clone Known to allow modification
                   TArray<TSharedPtr<FJsonValue>> knownArray = *knownArrayPtr;
                   for (TSharedPtr<FJsonValue> &entryVal : knownArray) {
                     if (!entryVal.IsValid())
                       continue;
                     TSharedPtr<FJsonObject> entryObj = entryVal->AsObject();
                     if (!entryObj.IsValid())
                       continue;
                     TSharedPtr<FJsonObject> regionObj;
                     if (TSharedPtr<FJsonValue> valueVal = entryObj->TryGetField(TEXT("Value"))) {
                       regionObj = valueVal->AsObject();
                     }
                     if (!regionObj.IsValid())
                       continue;

                     const TArray<TSharedPtr<FJsonValue>> *sourcesPtr = nullptr;
                     if (!regionObj->TryGetArrayField(TEXT("Sources"), sourcesPtr) || !sourcesPtr)
                       continue;
                     TArray<TSharedPtr<FJsonValue>> sources = *sourcesPtr;
                     for (TSharedPtr<FJsonValue> &srcVal : sources) {
                       if (!srcVal.IsValid())
                         continue;
                       TSharedPtr<FJsonObject> srcObj = srcVal->AsObject();
                       if (!srcObj.IsValid())
                         continue;

                       FString itemStr, propStr;
                       const bool itemEmpty = !srcObj->TryGetStringField(TEXT("Item"), itemStr) || itemStr.IsEmpty();
                       const bool propEmpty = !srcObj->TryGetStringField(TEXT("Prop"), propStr) || propStr.IsEmpty();

                       if (itemEmpty && propEmpty) {
                         srcObj->SetStringField(TEXT("Item"), TEXT("PyriteOre"));
                         srcObj->SetStringField(TEXT("Prop"), TEXT("PyriteCluster"));
                       }
                     }
                     // Write back possibly modified sources array
                     regionObj->SetArrayField(TEXT("Sources"), sources);
                   }

                   // Write back possibly modified known array
                   regionMapObj->SetArrayField(TEXT("Known"), knownArray);

                   // Save updated JSON
                   FString outStr;
                   const auto writer = TJsonWriterFactory<>::Create(&outStr);
                   FJsonSerializer::Serialize(rootObj.ToSharedRef(), writer);
                   if (!FFileHelper::SaveStringToFile(outStr, *surfacePath)) {
                     LOG(ERROR_LL) << "SaveMigrationManager: Failed to write SurfaceDefinition.json for '"
                                   << saveName << "'";
                     return false;
                   }

                   return true;
                 } });

  migrators.Add({ FVersionStruct{ 0, 20, 1, 140, TEXT("*") }, [](const FString &saveName, UGameInstance *GameInstance) {
                   for (const auto &tier : TierArray) {
                     ReplaceBlocksInLogicJson(saveName, FString(tier) + "Scaffold", "Scaffold");
                     ReplaceBlocksInLogicJson(saveName, FString(tier) + "Corner", "Corner");
                     ReplaceBlocksInLogicJson(saveName, FString(tier) + "Beam", "Beam");
                   }
                   ReplaceBlocksInLogicJson(saveName, "WoodenStairs", "Stairs");
                   return true;
                 } });

  // Rename InitialCapacity -> Yield in Temperate/SurfaceDefinition.json for FertileLayer and OilLayer
  migrators.Add({ FVersionStruct{ 0, 20, 1, 176, TEXT("*") }, [](const FString &saveName, UGameInstance *GameInstance) {
                   const FString saveRoot = FPaths::ProjectSavedDir() / TEXT("SaveGames") / saveName;
                   const FString surfacePath = saveRoot / TEXT("Temperate") / TEXT("SurfaceDefinition.json");

                   FString jsonStr;
                   if (!FFileHelper::LoadFileToString(jsonStr, *surfacePath)) {
                     return true; // Nothing to migrate
                   }

                   TSharedPtr<FJsonObject> rootObj;
                   TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(jsonStr);
                   if (!FJsonSerializer::Deserialize(reader, rootObj) || !rootObj.IsValid()) {
                     LOG(ERROR_LL) << "SaveMigrationManager: Failed to parse SurfaceDefinition.json for '"
                                   << saveName << "'";
                     return false;
                   }

                   TSharedPtr<FJsonObject> surfaceObj;
                   if (TSharedPtr<FJsonValue> surfaceVal = rootObj->TryGetField(TEXT("SurfaceDefinition"))) {
                     surfaceObj = surfaceVal->AsObject();
                   }
                   if (!surfaceObj.IsValid()) {
                     return true; // Unexpected structure; skip
                   }

                   TSharedPtr<FJsonObject> regionMapObj;
                   if (TSharedPtr<FJsonValue> regionMapVal = surfaceObj->TryGetField(TEXT("RegionMap"))) {
                     regionMapObj = regionMapVal->AsObject();
                   }
                   if (!regionMapObj.IsValid()) {
                     return true; // No regions to process
                   }

                   const TArray<TSharedPtr<FJsonValue>> *knownArrayPtr = nullptr;
                   if (!regionMapObj->TryGetArrayField(TEXT("Known"), knownArrayPtr) || !knownArrayPtr) {
                     return true; // Nothing known
                   }

                   auto renameInLayer = [](TSharedPtr<FJsonObject> layerObj, float multiplier) {
                     if (!layerObj.IsValid())
                       return;

                     // Preferred path: Data array exists
                     const TArray<TSharedPtr<FJsonValue>> *dataPtr = nullptr;
                     if (layerObj->TryGetArrayField(TEXT("Data"), dataPtr) && dataPtr) {
                       TArray<TSharedPtr<FJsonValue>> data = *dataPtr;
                       for (TSharedPtr<FJsonValue> &subVal : data) {
                         if (!subVal.IsValid())
                           continue;
                         TSharedPtr<FJsonObject> subObj = subVal->AsObject();
                         if (!subObj.IsValid())
                           continue;
                         double initialCapacity = 0.0;
                         if (subObj->TryGetNumberField(TEXT("InitialCapacity"), initialCapacity)) {
                           subObj->SetNumberField(TEXT("Yield"), initialCapacity * multiplier);
                           subObj->RemoveField(TEXT("InitialCapacity"));
                         }
                       }
                       layerObj->SetArrayField(TEXT("Data"), data);
                       return;
                     }
                   };

                   // Clone Known to allow modification
                   TArray<TSharedPtr<FJsonValue>> knownArray = *knownArrayPtr;
                   for (TSharedPtr<FJsonValue> &entryVal : knownArray) {
                     if (!entryVal.IsValid())
                       continue;
                     TSharedPtr<FJsonObject> entryObj = entryVal->AsObject();
                     if (!entryObj.IsValid())
                       continue;
                     TSharedPtr<FJsonObject> regionObj;
                     if (TSharedPtr<FJsonValue> valueVal = entryObj->TryGetField(TEXT("Value"))) {
                       regionObj = valueVal->AsObject();
                     }
                     if (!regionObj.IsValid())
                       continue;

                     if (TSharedPtr<FJsonValue> fertileVal = regionObj->TryGetField(TEXT("FertileLayer"))) {
                       renameInLayer(fertileVal->AsObject(), 0.3f);
                     }
                     if (TSharedPtr<FJsonValue> oilVal = regionObj->TryGetField(TEXT("OilLayer"))) {
                       renameInLayer(oilVal->AsObject(), 0.05f);
                     }
                   }

                   // Write back possibly modified known array
                   regionMapObj->SetArrayField(TEXT("Known"), knownArray);

                   // Save updated JSON
                   FString outStr;
                   const auto writer = TJsonWriterFactory<>::Create(&outStr);
                   FJsonSerializer::Serialize(rootObj.ToSharedRef(), writer);
                   if (!FFileHelper::SaveStringToFile(outStr, *surfacePath)) {
                     LOG(ERROR_LL) << "SaveMigrationManager: Failed to write SurfaceDefinition.json for '"
                                   << saveName << "'";
                     return false;
                   }

                   return true;
                 } });

  // Rename FluetedColumn -> Column (designable migration)
  migrators.Add({ FVersionStruct{ 0, 20, 1, 190, TEXT("*") }, [](const FString &saveName, UGameInstance *GameInstance) {
                   ReplaceBlocksInLogicJson(saveName, "FluetedColumn", "Column");
                   return true;
                 } });

  // Migrators end
  // ================================
  // ================================

  int applied = 0;
  for (const FMigrator &m : migrators) {
    if (m.Target > saveVersion) {
      const FString versionStr = UGameSessionData::VersionToString(m.Target);
      LOG(INFO_LL) << "SaveMigrationManager: Applying migrator for version " << versionStr;
      if (!m.Action(saveName, GameInstance)) {
        LOG(ERROR_LL) << "SaveMigrationManager: Failed to apply migrator for version " << versionStr;
        return;
      }
      ++applied;
    }
  }

  if (gameSessionFallback) {
    loadedSession->Initialize(GameInstance, saveName, true, true, true, "Default", FName("WorldGeneratorRivers"));
    UStaticSaveHelpers::SaveGameSessionData(saveName, loadedSession);
  }

  if (applied > 0) {
    LOG(INFO_LL) << "SaveMigrationManager: Applied " << applied << " migrators (save version "
                 << UGameSessionData::VersionToString(saveVersion) << ")";
  }
}

bool USaveMigrationManager::ReplaceBlocksInLogicJson(const FString &saveName, const FString &oldBlockName, const FString &newBlockName) {
  if (oldBlockName.IsEmpty() || newBlockName.IsEmpty()) {
    LOG(ERROR_LL) << "SaveMigrationManager: Block names cannot be empty";
    return false;
  }

  if (oldBlockName == newBlockName) {
    LOG(WARN_LL) << "SaveMigrationManager: Old and new block names are the same, no changes needed";
    return true;
  }

  const FString saveRoot = FPaths::ProjectSavedDir() / TEXT("SaveGames") / saveName;
  const FString logicBinPath = saveRoot / TEXT("Temperate") / TEXT("Logic.bin");

  // Load and decompress Logic.bin
  TArray<uint8> binaryData;
  if (!UStaticSaveHelpers::LoadFileToArray(binaryData, *logicBinPath)) {
    LOG(ERROR_LL) << "SaveMigrationManager: Failed to load Logic.bin for save '" << saveName << "'";
    return false;
  }

  if (binaryData.Num() == 0) {
    LOG(ERROR_LL) << "SaveMigrationManager: Logic.bin is empty for save '" << saveName << "'";
    return false;
  }

  // Decompress the data
  FArchiveLoadCompressedProxy decompressor(binaryData, FName("ZLIB"));
  if (decompressor.GetError()) {
    LOG(ERROR_LL) << "SaveMigrationManager: Failed to decompress Logic.bin for save '" << saveName << "'";
    return false;
  }

  FBufferArchive decompressedData;
  decompressor << decompressedData;

  if (decompressedData.Num() == 0) {
    LOG(ERROR_LL) << "SaveMigrationManager: Decompressed Logic.bin is empty for save '" << saveName << "'";
    return false;
  }

  // Read version and JSON string
  FMemoryReader reader(decompressedData, true);
  if (reader.IsError()) {
    LOG(ERROR_LL) << "SaveMigrationManager: Failed to create reader for Logic.bin data in save '" << saveName << "'";
    return false;
  }

  reader.Seek(0);
  int32 version = 0;
  FString jsonStr;
  reader << version;
  reader << jsonStr;

  if (jsonStr.IsEmpty()) {
    LOG(ERROR_LL) << "SaveMigrationManager: JSON data is empty in Logic.bin for save '" << saveName << "'";
    return false;
  }

  // Parse JSON
  TSharedPtr<FJsonObject> rootObj;
  TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(jsonStr);
  if (!FJsonSerializer::Deserialize(jsonReader, rootObj) || !rootObj.IsValid()) {
    LOG(ERROR_LL) << "SaveMigrationManager: Failed to parse JSON from Logic.bin for save '" << saveName << "'";
    return false;
  }

  // Replace block names
  int32 replacementsCount = 0;
  for (const auto &it : rootObj->Values) {
    TSharedPtr<FJsonObject> blockObj = it.Value->AsObject();
    if (blockObj.IsValid()) {
      FString currentBlockName;
      if (blockObj->TryGetStringField(TEXT("StaticBlock"), currentBlockName) &&
          currentBlockName == oldBlockName) {
        blockObj->SetStringField(TEXT("StaticBlock"), newBlockName);
        replacementsCount++;
      }
    }
  }

  if (replacementsCount == 0) {
    LOG(INFO_LL) << "SaveMigrationManager: No blocks with name '" << oldBlockName << "' found in Logic.bin for save '" << saveName << "'";
    return true; // Not an error, just nothing to replace
  }

  // Serialize modified JSON back to string
  FString modifiedJsonStr;
  const auto writer = TJsonWriterFactory<>::Create(&modifiedJsonStr);
  if (!FJsonSerializer::Serialize(rootObj.ToSharedRef(), writer)) {
    LOG(ERROR_LL) << "SaveMigrationManager: Failed to serialize modified JSON for save '" << saveName << "'";
    return false;
  }

  // Write back to compressed format (same as original Logic.bin structure)
  FBufferArchive uncompressedData;
  FMemoryWriter memoryWriter(uncompressedData, true);
  memoryWriter << version; // Keep the same version
  memoryWriter << modifiedJsonStr;

  // Compress the data with ZLIB
  TArray<uint8> finalCompressedData;
  FArchiveSaveCompressedProxy compressor(finalCompressedData, FName("ZLIB"));
  compressor << uncompressedData;
  compressor.Flush();

  // Save the compressed data back to Logic.bin
  if (!FFileHelper::SaveArrayToFile(finalCompressedData, *logicBinPath)) {
    LOG(ERROR_LL) << "SaveMigrationManager: Failed to save modified Logic.bin for save '" << saveName << "'";
    return false;
  }

  LOG(INFO_LL) << "SaveMigrationManager: Successfully replaced " << replacementsCount << " blocks from '"
               << oldBlockName << "' to '" << newBlockName << "' in Logic.bin for save '" << saveName << "'";

  return true;
}

bool USaveMigrationManager::RemoveSingleStaticBlocksFromLogicJson(const FString &saveName) {
  FString logicBinPath = FPaths::ProjectSavedDir() + TEXT("SaveGames/") + saveName + TEXT("/Dimension/Logic.bin");

  if (!FPaths::FileExists(logicBinPath)) {
    LOG(ERROR_LL) << "SaveMigrationManager: Logic.bin not found for save '" << saveName << "'";
    return false;
  }

  // Read the compressed Logic.bin file
  TArray<uint8> compressedData;
  if (!FFileHelper::LoadFileToArray(compressedData, *logicBinPath)) {
    LOG(ERROR_LL) << "SaveMigrationManager: Failed to load Logic.bin for save '" << saveName << "'";
    return false;
  }

  // Decompress the data
  FArchiveLoadCompressedProxy decompressor(compressedData, FName("ZLIB"));
  if (decompressor.GetError()) {
    LOG(ERROR_LL) << "SaveMigrationManager: Failed to decompress Logic.bin for save '" << saveName << "'";
    return false;
  }

  FBufferArchive uncompressedData;
  decompressor << uncompressedData;
  decompressor.Flush();

  if (uncompressedData.Num() == 0) {
    LOG(ERROR_LL) << "SaveMigrationManager: Decompressed Logic.bin is empty for save '" << saveName << "'";
    return false;
  }

  // Read the decompressed data
  FMemoryReader reader(uncompressedData);
  reader.Seek(0);
  int32 version = 0;
  FString jsonStr;
  reader << version;
  reader << jsonStr;

  if (jsonStr.IsEmpty()) {
    LOG(ERROR_LL) << "SaveMigrationManager: JSON data is empty in Logic.bin for save '" << saveName << "'";
    return false;
  }

  // Parse JSON
  TSharedPtr<FJsonObject> rootObj;
  TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(jsonStr);
  if (!FJsonSerializer::Deserialize(jsonReader, rootObj) || !rootObj.IsValid()) {
    LOG(ERROR_LL) << "SaveMigrationManager: Failed to parse JSON from Logic.bin for save '" << saveName << "'";
    return false;
  }

  // Remove "SingleStaticBlock" from StaticBlock values
  int32 modificationsCount = 0;
  for (const auto &it : rootObj->Values) {
    TSharedPtr<FJsonObject> blockObj = it.Value->AsObject();
    if (blockObj.IsValid()) {
      FString currentBlockName;
      if (blockObj->TryGetStringField(TEXT("StaticBlock"), currentBlockName)) {
        FString modifiedBlockName = currentBlockName.Replace(TEXT("SingleStaticBlock"), TEXT("")).Replace(TEXT("StaticBlock"), TEXT(""));
        if (modifiedBlockName != currentBlockName) {
          blockObj->SetStringField(TEXT("StaticBlock"), modifiedBlockName);
          modificationsCount++;
        }
      }
    }
  }

  if (modificationsCount == 0) {
    LOG(INFO_LL) << "SaveMigrationManager: No StaticBlock values contained 'SingleStaticBlock' in Logic.bin for save '" << saveName << "'";
    return true; // Not an error, just nothing to modify
  }

  // Serialize modified JSON back to string
  FString modifiedJsonStr;
  const auto writer = TJsonWriterFactory<>::Create(&modifiedJsonStr);
  if (!FJsonSerializer::Serialize(rootObj.ToSharedRef(), writer)) {
    LOG(ERROR_LL) << "SaveMigrationManager: Failed to serialize modified JSON for save '" << saveName << "'";
    return false;
  }

  // Write back to compressed format (same as original Logic.bin structure)
  FBufferArchive finalUncompressedData;
  FMemoryWriter memoryWriter(finalUncompressedData, true);
  memoryWriter << version; // Keep the same version
  memoryWriter << modifiedJsonStr;

  // Compress the data with ZLIB
  TArray<uint8> finalCompressedData;
  FArchiveSaveCompressedProxy compressor(finalCompressedData, FName("ZLIB"));
  compressor << finalUncompressedData;
  compressor.Flush();

  // Save the compressed data back to Logic.bin
  if (!FFileHelper::SaveArrayToFile(finalCompressedData, *logicBinPath)) {
    LOG(ERROR_LL) << "SaveMigrationManager: Failed to save modified Logic.bin for save '" << saveName << "'";
    return false;
  }

  LOG(INFO_LL) << "SaveMigrationManager: Successfully removed 'SingleStaticBlock' from " << modificationsCount
               << " StaticBlock values in Logic.bin for save '" << saveName << "'";

  return true;
}

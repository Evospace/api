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

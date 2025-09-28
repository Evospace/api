// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/SaveMigrationManager.h"

#include "Qr/StaticSaveHelpers.h"
#include "Logging/StructuredLog.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/FileManager.h"
#include "Public/SurfaceDefinition.h"
#include "Public/RegionMap.h"

void USaveMigrationManager::RunMigrationsIfNeeded(const FString &saveName, UGameInstance *GameInstance) {
  if (!GameInstance) {
    UE_LOGFMT(LogLoad, Error, "SaveMigrationManager: GameInstance is null");
    return;
  }

  UGameSessionData *loadedSession = UStaticSaveHelpers::LoadGameSessionData(saveName);
  if (!loadedSession) {
    UE_LOGFMT(LogLoad, Warning, "SaveMigrationManager: No GameSessionData found for save '{0}'", saveName);
    return;
  }

  const FVersionStruct saveVersion = loadedSession->Version;

  struct FMigrator {
    FVersionStruct Target;
    TFunction<bool(const FString &)> Action;
  };

  TArray<FMigrator> migrators;
  migrators.Reserve(8);

  migrators.Add({
    FVersionStruct{0, 20, 1, 8, TEXT("*")},
    [](const FString &saveName) {
      IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
      IFileManager& FileManager = IFileManager::Get();

      auto saveRoot = FPaths::ProjectSavedDir() / TEXT("SaveGames") / saveName;

      const FString dimensionDir = saveRoot / TEXT("Dimension");
      const FString temperateDir = saveRoot / TEXT("Temperate");

      if (!FileManager.Move(*temperateDir, *dimensionDir)) {
        LOG(ERROR_LL) << "SaveMigrationManager: Failed to rename 'Dimension' to 'Temperate' for '" << saveName << "'";
        return false;
      }

      const FString playerSrc = temperateDir / TEXT("player.json");
      const FString playerDst = saveRoot / TEXT("Player.json");
      if (PlatformFile.FileExists(*playerSrc)) {
        if (!PlatformFile.MoveFile(*playerDst, *playerSrc)) {
          LOG(ERROR_LL) << "SaveMigrationManager: Failed to move Player.json to save root for '" << saveName << "'";
          return false;
        }
      }

      const FString previewSrc = temperateDir / TEXT("Preview.jpg");
      const FString previewDst = saveRoot / TEXT("Preview.jpg");
      if (PlatformFile.FileExists(*previewSrc)) {
        if (!PlatformFile.MoveFile(*previewDst, *previewSrc)) {
          LOG(ERROR_LL) << "SaveMigrationManager: Failed to move Preview.jpg to save root for '" << saveName << "'";
          return false;
        }
      }

      // Move Dimension.json into save root
      const FString dimJsonSrc = temperateDir / TEXT("Dimension.json");
      const FString dimJsonDst = saveRoot / TEXT("GameSessionData.json");
      if (PlatformFile.FileExists(*dimJsonSrc)) {
        if (!PlatformFile.MoveFile(*dimJsonDst, *dimJsonSrc)) {
          LOG(ERROR_LL) << "SaveMigrationManager: Failed to move Dimension.json to save root for '" << saveName << "'";
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
          if (TSharedPtr<FJsonValue> gameSessionDataFieldValue = gameSessionDataObj->TryGetField(TEXT("GameSessionData"))) {
            gameSessionDataFieldObj = gameSessionDataFieldValue->AsObject();
          }
          if (!gameSessionDataFieldObj.IsValid() || !gameSessionDataFieldObj->TryGetStringField(TEXT("Generator"), generatorName)) {
            LOG(ERROR_LL) << "SaveMigrationManager: Failed to get Generator from GameSessionData.GameSessionData for '" << saveName << "'";
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

            USurfaceDefinition *surfaceDefinition = NewObject<USurfaceDefinition>();
            surfaceDefinition->GeneratorName = *generatorName;
            surfaceDefinition->Initialize();

            surfaceDefinition->RegionMap->DeserializeJson(depositsValue->AsObject());

            UStaticSaveHelpers::SaveSurfaceDefinition(saveName, TEXT("Temperate"), surfaceDefinition);
          }
        }
      }

      return true;
    }
  });

  int applied = 0;
  for (const FMigrator &m : migrators) {
    if (m.Target > saveVersion) {
      const FString versionStr = UGameSessionData::VersionToString(m.Target);
      LOG(INFO_LL) << "SaveMigrationManager: Applying migrator for version " << versionStr;
      if(!m.Action(saveName)) {
        LOG(ERROR_LL) << "SaveMigrationManager: Failed to apply migrator for version " << versionStr;
        return;
      }
      ++applied;
    }
  }

  if(applied > 0) {
    LOG(INFO_LL) << "SaveMigrationManager: Applied " << applied << " migrators (save version " << UGameSessionData::VersionToString(saveVersion) << ")";
  }
}



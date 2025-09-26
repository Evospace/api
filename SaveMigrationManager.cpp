// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/SaveMigrationManager.h"

#include "Qr/StaticSaveHelpers.h"
#include "Logging/StructuredLog.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/FileManager.h"

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

      const FString playerSrc = saveRoot / TEXT("player.json");
      const FString playerDst = saveRoot / TEXT("Player.json");
      if (PlatformFile.FileExists(*playerSrc)) {
        if (!PlatformFile.MoveFile(*playerDst, *playerSrc)) {
          LOG(ERROR_LL) << "SaveMigrationManager: Failed to move Player.json to save root for '" << saveName << "'";
          return false;
        }
      }

      const FString previewSrc = saveRoot / TEXT("Preview.jpg");
      const FString previewDst = saveRoot / TEXT("Preview.jpg");
      if (PlatformFile.FileExists(*previewSrc)) {
        if (!PlatformFile.MoveFile(*previewDst, *previewSrc)) {
          LOG(ERROR_LL) << "SaveMigrationManager: Failed to move Preview.jpg to save root for '" << saveName << "'";
          return false;
        }
      }

      // Move Dimension.json into save root
      const FString dimJsonSrc = temperateDir / TEXT("Dimension.json");
      const FString dimJsonDst = saveRoot / TEXT("Deposits.json");
      if (PlatformFile.FileExists(*dimJsonSrc)) {
        if (!PlatformFile.MoveFile(*dimJsonDst, *dimJsonSrc)) {
          LOG(ERROR_LL) << "SaveMigrationManager: Failed to move Dimension.json to save root for '" << saveName << "'";
          return false;
        }
      }

      // Extract "GameSessionData" from Dimension.json and save to GameSessionData.json in save root
      if (PlatformFile.FileExists(*dimJsonDst)) {
        FString dimJsonContent;
        if (FFileHelper::LoadFileToString(dimJsonContent, *dimJsonDst)) {
          TSharedPtr<FJsonObject> dimJsonObject;
          TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(dimJsonContent);
          if (FJsonSerializer::Deserialize(reader, dimJsonObject) && dimJsonObject.IsValid()) {
            TSharedPtr<FJsonObject> gameSessionDataObj = MakeShared<FJsonObject>();
            for (const auto& Pair : dimJsonObject->Values) {
              gameSessionDataObj->SetField(Pair.Key, Pair.Value);
            }
            FString gameSessionDataStr;
            TSharedRef<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&gameSessionDataStr);
            if (FJsonSerializer::Serialize(gameSessionDataObj.ToSharedRef(), writer)) {
              const FString gameSessionDataPath = saveRoot / TEXT("GameSessionData.json");
              if (!FFileHelper::SaveStringToFile(gameSessionDataStr, *gameSessionDataPath)) {
                LOG(ERROR_LL) << "SaveMigrationManager: Failed to write GameSessionData.json for '" << saveName << "'";
                return false;
              }
            } else {
              LOG(ERROR_LL) << "SaveMigrationManager: Failed to serialize GameSessionData for '" << saveName << "'";
              return false;
            }

            if (dimJsonObject.IsValid() && dimJsonObject->HasField(TEXT("GameSessionData"))) {
                dimJsonObject->RemoveField(TEXT("GameSessionData"));
                FString updatedDimJsonStr;
                TSharedRef<TJsonWriter<>> updatedWriter = TJsonWriterFactory<>::Create(&updatedDimJsonStr);
                if (FJsonSerializer::Serialize(dimJsonObject.ToSharedRef(), updatedWriter)) {
                  if (!FFileHelper::SaveStringToFile(updatedDimJsonStr, *dimJsonDst)) {
                    LOG(ERROR_LL) << "SaveMigrationManager: Failed to update Deposits.json after removing GameSessionData for '" << saveName << "'";
                    return false;
                  }
                } else {
                  LOG(ERROR_LL) << "SaveMigrationManager: Failed to serialize updated Deposits.json for '" << saveName << "'";
                  return false;
                }
              }
          } else {
            LOG(ERROR_LL) << "SaveMigrationManager: Failed to parse Deposits.json for '" << saveName << "'";
            return false;
          }
        } else {
          LOG(ERROR_LL) << "SaveMigrationManager: Failed to load Deposits.json for '" << saveName << "'";
          return false;
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



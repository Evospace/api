// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#include "StaticLogger.h"

#include "HAL/PlatformFilemanager.h"
#include "Misc/OutputDevice.h"
#include "Misc/ScopeLock.h"
#include "Misc/Char.h"
#include "Misc/Paths.h"

#include <memory>

FSimpleLogger FSimpleLogger::static_logger = FSimpleLogger();

void FSimpleLogger::StartFileLogging(const FString &FileName) {
  FScopeLock guard(&FileLock);
  const FString LogDir = FPaths::ProjectLogDir();
  LogFilePath = FPaths::Combine(LogDir, FileName);
  if (LogFilePath.IsEmpty()) {
    bWriteToDisk = false;
    return;
  }

  IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
  if (!LogDir.IsEmpty() && !PlatformFile.DirectoryExists(*LogDir)) {
    PlatformFile.CreateDirectoryTree(*LogDir);
  }
  LogFileHandle.Reset(PlatformFile.OpenWrite(*LogFilePath, false, true));
  bWriteToDisk = LogFileHandle.IsValid();
  if (!bWriteToDisk) {
    LogFilePath.Empty();
  }
}

void FSimpleLogger::StopFileLogging() {
  FScopeLock guard(&FileLock);
  LogFileHandle.Reset();
  bWriteToDisk = false;
  LogFilePath.Empty();
}

void FSimpleLogger::WriteLineToDisk(const FString &Line) {
  FScopeLock guard(&FileLock);
  if (!bWriteToDisk || !LogFileHandle) {
    return;
  }

  FString DiskLine = Line;
  DiskLine += LINE_TERMINATOR;
  FTCHARToUTF8 Converter(*DiskLine);
  if (Converter.Length() > 0) {
    LogFileHandle->Write(reinterpret_cast<const uint8 *>(Converter.Get()), Converter.Length());
  }
  LogFileHandle->Flush();
}
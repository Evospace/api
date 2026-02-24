// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "Containers/Deque.h"
#include "HAL/CriticalSection.h"
#include "HAL/FileManager.h"
#include "Qr/Vector.h"
#include "Logging/StructuredLog.h"
#include "Templates/UniquePtr.h"

#include <string>

#ifndef LOG_LEVEL
#define LOG(level) FLogHelper(FSimpleLogger::static_logger, level)
#endif

#ifndef LOG_NOTHING
#define LOG_NOTHING(level) true ? (void)0 : cpplog::helpers::VoidStreamClass() & LOG(level)
#endif

enum ELogLevel {
  ERROR_LL,
  WARN_LL,
  INFO_LL,
  TRACE_LL,
  ELogLevel_Count
};

class FSimpleLogger {
  public:
  void Log(ELogLevel LogLevel, const FString &Message) {
    FString LogEntry = GetLogLevelString(LogLevel) + TEXT(" : ") + Message;
    if (LogLevel <= UnrealLogLevel) {
      if (LogLevel == WARN_LL) {
        UE_LOGFMT(LogTemp, Warning, "{0}", *Message);
      } else if (LogLevel == ERROR_LL) {
        UE_LOGFMT(LogTemp, Warning, "{0}", *Message);
      } else {
        UE_LOGFMT(LogTemp, Log, "{0}", *Message);
      }
    }
    if (LogLevel == ERROR_LL) {
      if (ErrorEntries.Num() >= MaxStoredLogEntries) {
        ErrorEntries.PopFirst();
      }
      ErrorEntries.PushLast(Message);
    } else if (LogLevel == WARN_LL) {
      if (WarnEntries.Num() >= MaxStoredLogEntries) {
        WarnEntries.PopFirst();
      }
      WarnEntries.PushLast(Message);
    }
    LastLogEntry = MoveTemp(LogEntry);
    ++perLevelCount[LogLevel];
    ++LogVersion;
    WriteLineToDisk(LastLogEntry);
  }

  void Clear() {
    perLevelCount.Empty();
    perLevelCount.SetNumZeroed(ELogLevel_Count);
    LastLogEntry = "";
    ErrorEntries.Empty();
    WarnEntries.Empty();
    LogVersion++;
  }

  int32 GetLogVersion() const { return LogVersion; }

  static FSimpleLogger static_logger;

  FSimpleLogger() { Clear(); }

  bool HasErrors() const { return perLevelCount[ERROR_LL] > 0; }

  bool HasWarnings() const { return perLevelCount[WARN_LL] > 0; }

  const FString &GetLastMessage() const { return LastLogEntry; }

  TArray<FString> GetErrors() const {
    TArray<FString> arr;
    for (auto &s : ErrorEntries) {
      arr.Add(s);
    }
    return arr;
  }

  TArray<FString> GetWarnings() const {
    TArray<FString> arr;
    for (auto &s : WarnEntries) {
      arr.Add(s);
    }
    return arr;
  }

  const TArray<int32> &GetLevels() const { return perLevelCount; }

  void StartFileLogging(const FString &FileName);
  void StopFileLogging();

  void SetUnrealLogLevel(ELogLevel Level) { UnrealLogLevel = Level; }
  ELogLevel GetUnrealLogLevel() const { return UnrealLogLevel; }

  private:
  TArray<int32> perLevelCount;

  FString LastLogEntry;

  static constexpr int32 MaxStoredLogEntries = 100;

  TDeque<FString> ErrorEntries;
  TDeque<FString> WarnEntries;

  int32 LogVersion = 0;

  FCriticalSection FileLock;
  TUniquePtr<IFileHandle> LogFileHandle;
  FString LogFilePath;
  bool bWriteToDisk = false;
  ELogLevel UnrealLogLevel = INFO_LL;

  void WriteLineToDisk(const FString &Line);
  void RotateLogs(IPlatformFile &PlatformFile, const FString &BasePath, int32 MaxFiles);

  FString GetLogLevelString(ELogLevel LogLevel) const {
    switch (LogLevel) {
    case ERROR_LL:
      return TEXT("ERROR");
    case WARN_LL:
      return TEXT("WARN");
    case INFO_LL:
      return TEXT("INFO");
    case TRACE_LL:
      return TEXT("TRACE");
    default:
      return TEXT("UNKNOWN");
    }
  }
};

class FLogHelper {
  public:
  FLogHelper(FSimpleLogger &InLogger, ELogLevel InLogLevel) : Logger(InLogger), LogLevel(InLogLevel) {}

  FLogHelper &operator<<(const FString &Message) {
    Buffer += Message;
    return *this;
  }

  FLogHelper &operator<<(const TCHAR *Message) {
    Buffer += Message;
    return *this;
  }

  FLogHelper &operator<<(const std::string &Message) {
    Buffer += UTF8_TO_TCHAR(Message.data());
    return *this;
  }

  FLogHelper &operator<<(const char *Message) {
    if (Message == nullptr) {
      Buffer += TEXT("<null>");
      return *this;
    }
    Buffer += UTF8_TO_TCHAR(Message);
    return *this;
  }

  template <size_t N>
  FLogHelper &operator<<(const char (&Message)[N]) {
    Buffer += UTF8_TO_TCHAR(Message);
    return *this;
  }

  FLogHelper &operator<<(TCHAR *const Message) {
    if (Message == nullptr) {
      Buffer += TEXT("<null>");
      return *this;
    }
    Buffer += Message;
    return *this;
  }

  template <typename T>
  FLogHelper &operator<<(const T &Value) {
    Buffer += FString::Printf(TEXT("%s"), *TTypeToString(Value));
    return *this;
  }

  ~FLogHelper() { Logger.Log(LogLevel, Buffer); }

  private:
  FSimpleLogger &Logger;
  ELogLevel LogLevel;
  FString Buffer;

  template <typename T>
  FString TTypeToString(const T &Value) const {
    return FString::Printf(TEXT("%s"), *FString::FromInt(Value));
  }

  FString TTypeToString(const FName &Value) const { return Value.ToString(); }

  FString TTypeToString(const bool &Value) const { return Value ? "true" : "false"; }

  FString TTypeToString(const FVector3i &Value) const { return Value.ToString(); }
};
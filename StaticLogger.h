// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "Containers/Deque.h"
#include "Evospace/Vector.h"
#include "Logging/StructuredLog.h"

#include <string>

#ifndef LOG_LEVEL
#define LOG(level) \
  FLogHelper(FSimpleLogger::static_logger, level)
#endif

#ifndef LOG_NOTHING
#define LOG_NOTHING(level) \
  true ? (void)0 : cpplog::helpers::VoidStreamClass() & LOG(level)
#endif

enum ELogLevel {
  ERROR_LL,
  WARN_LL,
  INFO_LL,
  ELogLevel_Count
};

class FSimpleLogger {
  public:
  void Log(ELogLevel LogLevel, const FString &Message) {
    FString LogEntry = GetLogLevelString(LogLevel) + TEXT(" : ") + Message;
    if (LogLevel == WARN_LL) {
      UE_LOGFMT(LogTemp, Warning, "{0}", *Message);
    } else if (LogLevel == ERROR_LL) {
      UE_LOGFMT(LogTemp, Error, "{0}", *Message);
    } else {
      UE_LOGFMT(LogTemp, Log, "{0}", *Message);
    }
    LogEntries.PushLast(MoveTemp(LogEntry));
    ++perLevelCount[LogLevel];
  }

  const TDeque<FString> &GetLogEntries() const {
    return LogEntries;
  }

  static FSimpleLogger static_logger;

  FSimpleLogger() {
    perLevelCount.SetNumZeroed(ELogLevel_Count);
  }

  bool HasErrors() const {
    return perLevelCount[ERROR_LL] > 0;
  }

  const FString &GetLastMessage() const {
    static FString dummy = "";
    if (LogEntries.IsEmpty()) {
      return dummy;
    }
    return LogEntries.Last();
  }

  const TArray<int32> &GetLevels() const {
    return perLevelCount;
  }

  private:
  TArray<int32> perLevelCount;
  TDeque<FString> LogEntries;

  FString GetLogLevelString(ELogLevel LogLevel) const {
    switch (LogLevel) {
    case ERROR_LL:
      return TEXT("ERROR");
    case WARN_LL:
      return TEXT("WARN");
    case INFO_LL:
      return TEXT("INFO");
    default:
      return TEXT("UNKNOWN");
    }
  }
};

class FLogHelper {
  public:
  FLogHelper(FSimpleLogger &InLogger, ELogLevel InLogLevel)
    : Logger(InLogger), LogLevel(InLogLevel) {
  }

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
    Buffer += UTF8_TO_TCHAR(Message);
    return *this;
  }

  template <size_t N>
  FLogHelper &operator<<(const char (&Message)[N]) {
    Buffer += UTF8_TO_TCHAR(Message);
    return *this;
  }

  FLogHelper &operator<<(TCHAR *const Message) {
    Buffer += Message;
    return *this;
  }


  template <typename T>
  FLogHelper &operator<<(const T &Value) {
    Buffer += FString::Printf(TEXT("%s"), *TTypeToString(Value));
    return *this;
  }

  ~FLogHelper() {
    Logger.Log(LogLevel, Buffer);
  }

  private:
  FSimpleLogger &Logger;
  ELogLevel LogLevel;
  FString Buffer;

  template <typename T>
  FString TTypeToString(const T &Value) const {
    return FString::Printf(TEXT("%s"), *FString::FromInt(Value));
  }

  FString TTypeToString(const bool &Value) const {
    return Value ? "true" : "false";
  }

  FString TTypeToString(const FVector3i &Value) const {
    return Value.ToString();
  }
};
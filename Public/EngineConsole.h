#pragma once

#include "Prototype.h"
#include "EngineConsole.generated.h"

UCLASS()
class UEngineConsole : public UInstance {
  GENERATED_BODY()
  using Self = UEngineConsole;
  EVO_CODEGEN_INSTANCE(EngineConsole);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      //@comment Engine console
      .deriveClass<UEngineConsole, UInstance>("Console") //@class Console : Instance
      //direct:
      //--- Executes engine console command
      //--- @param command string
      //function Console.run(command) end
      .addStaticFunction("run", &Self::Run)
      //direct:
      //--- Get engine console variable
      //--- @param variable string
      //--- @return string
      //function Console.get_string(variable) end
      .addStaticFunction("get_string", &Self::GetString)
      //direct:
      //--- Get engine console variable
      //--- @param variable string
      //--- @return number
      //function Console.get_float(variable) end
      .addStaticFunction("get_float", &Self::GetFloat)
      //direct:
      //--- Get engine console variable
      //--- @param variable string
      //--- @return integer
      //function Console.get_int(variable) end
      .addStaticFunction("get_int", &Self::GetInt)
      .endClass();
  }

  static int32 GetInt(std::string_view s) {
    if (IConsoleVariable *CVar = IConsoleManager::Get().FindConsoleVariable(UTF8_TO_TCHAR(s.data()))) {
      auto res = CVar->GetInt();
      LOG(INFO_LL) << s.data() << " == " << res;
      return res;
    }

    return 0;
  }

  static std::string GetString(std::string_view s) {
    if (IConsoleVariable *CVar = IConsoleManager::Get().FindConsoleVariable(UTF8_TO_TCHAR(s.data()))) {
      auto res = CVar->GetString();
      LOG(INFO_LL) << s.data() << " ==" << res;
      return TCHAR_TO_UTF8(*res);
    }

    return "";
  }

  static bool GetBool(std::string_view s) {
    if (IConsoleVariable *CVar = IConsoleManager::Get().FindConsoleVariable(UTF8_TO_TCHAR(s.data()))) {
      auto res = CVar->GetBool();
      LOG(INFO_LL) << s.data() << " == " << res;
      return res;
    }

    return "";
  }

  static float GetFloat(std::string_view s) {
    if (IConsoleVariable *CVar = IConsoleManager::Get().FindConsoleVariable(UTF8_TO_TCHAR(s.data()))) {
      auto res = CVar->GetFloat();
      LOG(INFO_LL) << s.data() << " == " << res;
      return res;
    }

    return 0.f;
  }

  static void Run(std::string_view s) {
    if (GEngine && GEngine->GameViewport) {
      if (const auto world = GEngine->GameViewport->GetWorld()) {
        if (auto controller = world->GetFirstPlayerController()) {
          FString result = controller->ConsoleCommand(UTF8_TO_TCHAR(s.data()), false);
          LOG(INFO_LL) << result;
        }
      }
    }
  }
};
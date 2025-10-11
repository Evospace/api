#include "EngineData.h"
#include "Dimension.h"
#include "EngineUtils.h"
#include "Evospace/JsonHelper.h"
#include "Evospace/Gui/SettingsConfirmationWidget.h"
#include "Evospace/Player/MainPlayerController.h"
#include "Evospace/World/SectorArea.h"
#include "Public/MainGameInstance.h"
#include "Evospace/WorldEntities/WorldFeaturesManager.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Qr/Setting.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Qr/ModLoadingSubsystem.h"
#include "HAL/IConsoleManager.h"

class USettingsConfirmationWidget;
bool IsPointInRect(const FPlatformRect &Rect, const FVector2D &Point) {
  return Point.X >= Rect.Left && Point.X < Rect.Right && Point.Y >= Rect.Top && Point.Y < Rect.Bottom;
}

void UEngineData::FixResolution() {
  if (Windowed == EWindowMode::WindowedFullscreen) {
    FDisplayMetrics DisplayMetrics;
    FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
    ResolutionX = DisplayMetrics.PrimaryDisplayWidth;
    ResolutionY = DisplayMetrics.PrimaryDisplayHeight;
  }

  if (Windowed == EWindowMode::Fullscreen) {
    uint32 x = ResolutionX;
    uint32 y = ResolutionY;
    RHIGetSupportedResolution(x, y);
    ResolutionX = x;
    ResolutionY = y;
  }

  if (Windowed == EWindowMode::Windowed) {
    FDisplayMetrics DisplayMetrics;
    FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
    if (ResolutionX > DisplayMetrics.PrimaryDisplayWidth) {
      ResolutionX = DisplayMetrics.PrimaryDisplayWidth;
    }
    if (ResolutionY > DisplayMetrics.PrimaryDisplayHeight) {
      ResolutionY = DisplayMetrics.PrimaryDisplayHeight;
    }
  }
}

void UEngineData::ShowConfirmationDialog() {
  if (!GEngine->GetEngineSubsystem<UModLoadingSubsystem>()->IsContentLoaded())
    return;

  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (ensure(UserSettings)) {
    OldResolutionX = UserSettings->GetScreenResolution().X;
    OldResolutionY = UserSettings->GetScreenResolution().Y;
    OldWindowed = static_cast<int32>(UserSettings->GetFullscreenMode());

    FixResolution();

    UserSettings->SetFullscreenMode(static_cast<EWindowMode::Type>(Windowed));
    UserSettings->SetScreenResolution({ ResolutionX, ResolutionY });
    UserSettings->ApplyResolutionSettings(false);
  }

  auto pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
  auto widget = CreateWidget<USettingsConfirmationWidget>(
    pc, LoadObject<UClass>(nullptr, TEXT("/Game/Gui/SettingsConfirmationDialog.SettingsConfirmationDialog_C")), TEXT("SettingsConfirmationDialog"));
  widget->ed = this;
  widget->AddToViewport(10000);
}

void UEngineData::ConfirmSettings() {
  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (ensure(UserSettings)) {
    UserSettings->ConfirmVideoMode();
  }
  ApplyData();
}

void UEngineData::CancelSettings() {
  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (ensure(UserSettings)) {
    UserSettings->RevertVideoMode();
  }
  if (auto set = QrFind<USetting>("Fullscreen")) {
    std::vector opts = { "Fullscreen", "WindowedFullscreen", "Windowed", "Windowed" };
    set->StringValue = opts[OldWindowed];
  }
  if (auto set = QrFind<USetting>("Resolution")) {
    FStringFormatOrderedArguments args;
    args.Add(OldResolutionX);
    args.Add(OldResolutionY);
    set->StringValue = qr::to_string(FString::Format(TEXT("{0}x{1}"), args));
  }
  FixResolution();
  USetting::UpdateSettingsWidgets();
  ApplyData();
}

void UEngineData::ApplyData() const {
  // Apply global rendering console variables once per settings application
  {
    auto SetCVar = [](const TCHAR *Name, float Value) {
      if (IConsoleVariable *CVar = IConsoleManager::Get().FindConsoleVariable(Name)) {
        CVar->Set(Value, ECVF_SetByConsole);
      }
    };

    SetCVar(TEXT("r.AOGlobalDistanceField"), 0);
    SetCVar(TEXT("r.Substrate"), 0);
    SetCVar(TEXT("r.GBufferFormat"), 1);
    SetCVar(TEXT("r.Streaming.PoolSize"), 512);
    SetCVar(TEXT("r.DynamicRes.OperationMode"), 0);
  }

  if (Fps == 0) {
    UMainGameInstance::GraphicsSettings__SetFrameRateToBeUnbound();
  } else {
    UMainGameInstance::GraphicsSettings__SetFrameRateCap(Fps);
  }

  UMainGameInstance::SetDPI(Dpi);

  ApplyControllerData();

  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (UserSettings) {
    UserSettings->ApplySettings(false);
    UserSettings->SaveSettings();
  }

  ADimension *dim = nullptr;
  for (TActorIterator<ADimension> It(GetWorld()); It; ++It) {
    if (It) {
      dim = *It;
      break;
    }
  }
  
  // Can be null in the main menu
  if (dim) {
    dim->DimensionPropComponent->SetRangeMultiplier(PropsMul);
    dim->DimensionPropComponent->SetLodMultiplier(PropsQuality);
    dim->DimensionPropComponent->SetDetailShadowsEnabled(DetailShadows);
  }

  AWorldFeaturesManager *wfm = nullptr;
  for (TActorIterator<AWorldFeaturesManager> It(GetWorld()); It; ++It) {
    if (It) {
      wfm = *It;
      break;
    }
  }

  if (wfm) {
    wfm->UpdateSettings();
  }
}

void UEngineData::ApplyControllerData() const {
  auto pc = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
  if (pc && pc->PreloadFinished) {
    pc->SectorArea->SetRadius(LoadingRange);

    pc->AltHotbar = AltHotbar;
    pc->CtrlHotbar = CtrlHotbar;
    pc->ShiftHotbar = ShiftHotbar;
  }
}
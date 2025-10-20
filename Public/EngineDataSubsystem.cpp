#include "EngineDataSubsystem.h"
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

void UEngineDataSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);
}

void UEngineDataSubsystem::Deinitialize() {
  Super::Deinitialize();
}

void UEngineDataSubsystem::FixResolution() {
  if (Storage.Windowed == EWindowMode::WindowedFullscreen) {
    FDisplayMetrics DisplayMetrics;
    FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
    Storage.ResolutionX = DisplayMetrics.PrimaryDisplayWidth;
    Storage.ResolutionY = DisplayMetrics.PrimaryDisplayHeight;
  }

  if (Storage.Windowed == EWindowMode::Fullscreen) {
    uint32 x = Storage.ResolutionX;
    uint32 y = Storage.ResolutionY;
    RHIGetSupportedResolution(x, y);
    Storage.ResolutionX = x;  
    Storage.ResolutionY = y;
  }

  if (Storage.Windowed == EWindowMode::Windowed) {
    FDisplayMetrics DisplayMetrics;
    FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
    if (Storage.ResolutionX > DisplayMetrics.PrimaryDisplayWidth) {
      Storage.ResolutionX = DisplayMetrics.PrimaryDisplayWidth;
    }
    if (Storage.ResolutionY > DisplayMetrics.PrimaryDisplayHeight) {
      Storage.ResolutionY = DisplayMetrics.PrimaryDisplayHeight;
    }
  }
}

void UEngineDataSubsystem::ShowConfirmationDialog() {
  if (!GEngine->GetEngineSubsystem<UModLoadingSubsystem>()->IsContentLoaded())
    return;

  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (ensure(UserSettings)) {
    Storage.OldResolutionX = UserSettings->GetScreenResolution().X;
    Storage.OldResolutionY = UserSettings->GetScreenResolution().Y;
    Storage.OldWindowed = static_cast<int32>(UserSettings->GetFullscreenMode());

    FixResolution();

    UserSettings->SetFullscreenMode(static_cast<EWindowMode::Type>(Storage.Windowed));
    UserSettings->SetScreenResolution({ Storage.ResolutionX, Storage.ResolutionY });
    UserSettings->ApplyResolutionSettings(false);
  }

  UWorld *World = GetGameInstance()->GetWorld();
  auto pc = UGameplayStatics::GetPlayerController(World, 0);
  auto widget = CreateWidget<USettingsConfirmationWidget>(
    pc, LoadObject<UClass>(nullptr, TEXT("/Game/Gui/SettingsConfirmationDialog.SettingsConfirmationDialog_C")), TEXT("SettingsConfirmationDialog"));
  widget->AddToViewport(10000);
}

void UEngineDataSubsystem::ConfirmSettings() {
  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (ensure(UserSettings)) {
    UserSettings->ConfirmVideoMode();
  }
  ApplyData();
}

void UEngineDataSubsystem::CancelSettings() {
  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (ensure(UserSettings)) {
    UserSettings->RevertVideoMode();
  }
  if (auto set = QrFind<USetting>("Fullscreen")) {
    std::vector opts = { "Fullscreen", "WindowedFullscreen", "Windowed", "Windowed" };
    set->StringValue = opts[Storage.OldWindowed];
  }
  if (auto set = QrFind<USetting>("Resolution")) {
    FStringFormatOrderedArguments args;
    args.Add(Storage.OldResolutionX);
    args.Add(Storage.OldResolutionY);
    set->StringValue = qr::to_string(FString::Format(TEXT("{0}x{1}"), args));
  }
  FixResolution();
  USetting::UpdateSettingsWidgets();
  ApplyData();
}

void UEngineDataSubsystem::ApplyData() const {
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

  if (initial || LastAppliedData.Fps != Storage.Fps) {
    if (Storage.Fps == 0) {
      UMainGameInstance::GraphicsSettings__SetFrameRateToBeUnbound();
    } else {
      UMainGameInstance::GraphicsSettings__SetFrameRateCap(Storage.Fps);
    }
  }

  if (initial || LastAppliedData.Dpi != Storage.Dpi) {
    UMainGameInstance::SetDPI(Storage.Dpi);
  }

  ApplyControllerData();

  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (UserSettings) {
    UserSettings->ApplySettings(false);
    UserSettings->SaveSettings();
  }

  UWorld *World = GetGameInstance()->GetWorld();
  ADimension *dim = nullptr;
  for (TActorIterator<ADimension> It(World); It; ++It) {
    if (It) {
      dim = *It;
      break;
    }
  }

  // Can be null in the main menu
  if (dim) {
    if (initial || LastAppliedData.PropsMul != Storage.PropsMul) {
      dim->DimensionPropComponent->SetRangeMultiplier(Storage.PropsMul);
    }
    if (initial || LastAppliedData.PropsQuality != Storage.PropsQuality) {
      dim->DimensionPropComponent->SetLodMultiplier(Storage.PropsQuality);
    }
    if (initial || LastAppliedData.DetailShadows != Storage.DetailShadows) {
      dim->DimensionPropComponent->SetDetailShadowsEnabled(Storage.DetailShadows);
    }
  }

  AWorldFeaturesManager *wfm = nullptr;
  for (TActorIterator<AWorldFeaturesManager> It(World); It; ++It) {
    if (It) {
      wfm = *It;
      break;
    }
  }

  if (wfm) {
    wfm->UpdateSettings(Storage, LastAppliedData, initial);
  }

  LastAppliedData = Storage;
  initial = false;
}

void UEngineDataSubsystem::ApplyControllerData() const {
  UWorld *World = GetGameInstance()->GetWorld();
  auto pc = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(World, 0));
  if (pc && pc->PreloadFinished) {
    pc->SectorArea->SetRadius(Storage.LoadingRange);

    pc->AltHotbar = Storage.AltHotbar;
    pc->CtrlHotbar = Storage.CtrlHotbar;
    pc->ShiftHotbar = Storage.ShiftHotbar;
  }
}


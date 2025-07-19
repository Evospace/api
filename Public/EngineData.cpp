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


class USettingsConfirmationWidget;bool IsPointInRect(const FPlatformRect &Rect, const FVector2D &Point) {
  return Point.X >= Rect.Left && Point.X < Rect.Right && Point.Y >= Rect.Top && Point.Y < Rect.Bottom;
}

void UEngineData::ShowConfirmationDialog() {
  auto pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
  auto widget = CreateWidget<USettingsConfirmationWidget>(pc, LoadObject<UClass>(nullptr, TEXT("/Game/Gui/SettingsConfirmationDialog.SettingsConfirmationDialog_C")), TEXT("SettingsConfirmationDialog"));
  widget->ed = this;
  widget->AddToViewport(10000);
}

void UEngineData::ConfirmSettings() {
  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (ensure(UserSettings)) {
    UserSettings->ConfirmVideoMode();
    UserSettings->SaveSettings();
  }
}

void UEngineData::CancelSettings() {
  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (ensure(UserSettings)) {
    UserSettings->RevertVideoMode();
    UserSettings->SaveSettings();
  }
}

void UEngineData::ApplyData() const {
  if (Fps == 0) {
    UMainGameInstance::GraphicsSettings__SetFrameRateToBeUnbound();
  } else {
    UMainGameInstance::GraphicsSettings__SetFrameRateCap(Fps);
  }

  UMainGameInstance::SetDPI(Dpi);

  ApplyControllerData();

  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (UserSettings) {

    UserSettings->ValidateSettings();

    EWindowMode::Type fsm;
    FIntPoint resolution;

    if (UnconfirmedResolution) {
      resolution = FIntPoint(ResolutionX, ResolutionY);
      fsm = static_cast<EWindowMode::Type>(Windowed);
    } else {
      fsm = UserSettings->GetLastConfirmedFullscreenMode();
      resolution = UserSettings->GetLastConfirmedScreenResolution();
    }

    auto current_res = UserSettings->GetScreenResolution();
    auto current_fsm = UserSettings->GetFullscreenMode();

    UGameViewportClient *Viewport = GEngine->GameViewport;
    if (Viewport) {
      FString Cmd = FString::Printf(TEXT("r.SetRes %dx%d%s"),
                                    resolution.X,
                                    resolution.Y,
                                    (fsm == EWindowMode::Fullscreen           ? TEXT("f")
                                     : fsm == EWindowMode::WindowedFullscreen ? TEXT("wf")
                                                                              : TEXT("w")));
      Viewport->ConsoleCommand(*Cmd);
    }
  }

  ADimension *dim = nullptr;
  for (TActorIterator<ADimension> It(GetWorld()); It; ++It) {
    if (It) {
      dim = *It;
      break;
    }
  }

  if (dim) {
    dim->DimensionPropComponent->SetRangeMultiplier(PropsMul);
    dim->DimensionPropComponent->SetLodMultiplier(PropsQuality);
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
  if (pc && pc->BeginPlayFired) {
    pc->SectorArea->SetRadius(LoadingRange);
    pc->PerformanceGraph = PerformanceGraph;
    pc->Performance = Performance;

    pc->AltHotbar = AltHotbar;
    pc->CtrlHotbar = CtrlHotbar;
    pc->ShiftHotbar = ShiftHotbar;
  }
}
#include "EngineData.h"
#include "Dimension.h"
#include "EngineUtils.h"
#include "Evospace/JsonHelper.h"
#include "Evospace/Player/MainPlayerController.h"
#include "Evospace/World/SectorArea.h"
#include "Public/MainGameInstance.h"
#include "Evospace/WorldEntities/WorldFeaturesManager.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

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
    auto confirmed = UserSettings->GetLastConfirmedScreenResolution();
    auto resolution = FIntPoint(ResolutionX, ResolutionY);
    auto current_res = UserSettings->GetScreenResolution();
    auto current_fsm = UserSettings->GetFullscreenMode();

    if (Windowed == EWindowMode::WindowedFullscreen) {
      // Получаем текущее разрешение основного монитора
      FDisplayMetrics DisplayMetrics;
      FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
      FIntPoint DesktopResolution(DisplayMetrics.PrimaryDisplayWidth, DisplayMetrics.PrimaryDisplayHeight);
      resolution = DesktopResolution;
      UserSettings->SetScreenResolution(resolution);

      UserSettings->ApplySettings(false);
      UserSettings->SaveSettings();
    } else if (current_fsm != static_cast<EWindowMode::Type>(Windowed) || current_res != resolution) {
      UserSettings->SetScreenResolution(resolution);
      UserSettings->SetFullscreenMode(static_cast<EWindowMode::Type>(Windowed));
      UserSettings->ApplySettings(false);
      UserSettings->SaveSettings();
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
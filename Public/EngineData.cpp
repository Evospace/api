#include "EngineData.h"
#include "Dimension.h"
#include "EngineUtils.h"
#include "Evospace/JsonHelper.h"
#include "Evospace/Player/MainPlayerController.h"
#include "Evospace/World/SectorArea.h"
#include "Public/MainGameInstance.h"
#include "Evospace/WorldEntities/WorldFeaturesManager.h"

void UEngineData::ApplyData() const {
  if (Fps == 0) {
    UMainGameInstance::GraphicsSettings__SetFrameRateToBeUnbound();
  } else {
    UMainGameInstance::GraphicsSettings__SetFrameRateCap(Fps);
  }

  UMainGameInstance::SetDPI(Dpi);

  ApplyControllerData();

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
    pc->Performance = Performance;

    pc->AltHotbar = AltHotbar;
    pc->CtrlHotbar = CtrlHotbar;
    pc->ShiftHotbar = ShiftHotbar;
  }
}
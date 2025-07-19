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

class USettingsConfirmationWidget;
bool IsPointInRect(const FPlatformRect &Rect, const FVector2D &Point) {
  return Point.X >= Rect.Left && Point.X < Rect.Right && Point.Y >= Rect.Top && Point.Y < Rect.Bottom;
}

// Устанавливает размер окна в физических пикселях (без учета DPI)
static void SetLogicalWindowSize(int32 Width, int32 Height) {
    TSharedPtr<SWindow> Window = FSlateApplication::Get().GetActiveTopLevelWindow();
    if (Window.IsValid()) {
        Window->Resize(FVector2D(Width, Height));
    }
}

void UEngineData::ShowConfirmationDialog() {

  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (ensure(UserSettings)) {
    // Сохраняем старые значения для ручного отката
    OldResolutionX = UserSettings->GetScreenResolution().X;
    OldResolutionY = UserSettings->GetScreenResolution().Y;
    OldWindowed = static_cast<int32>(UserSettings->GetFullscreenMode());

    int32 targetX = ResolutionX;
    int32 targetY = ResolutionY;
    // Больше не делим на DPI
    UserSettings->SetFullscreenMode(static_cast<EWindowMode::Type>(Windowed));
    UserSettings->SetScreenResolution({ targetX, targetY });
    UserSettings->ApplyResolutionSettings(false); // Применяем изменения сразу
    // Принудительно меняем размер окна
    if (Windowed == EWindowMode::Windowed) {
      SetLogicalWindowSize(ResolutionX, ResolutionY);
    }
  }

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
  ApplyData();
}

void UEngineData::CancelSettings() {
  UGameUserSettings *UserSettings = GEngine->GetGameUserSettings();
  if (ensure(UserSettings)) {
    int32 targetX = OldResolutionX;
    int32 targetY = OldResolutionY;
    // Больше не делим на DPI
    UserSettings->SetFullscreenMode(static_cast<EWindowMode::Type>(OldWindowed));
    UserSettings->SetScreenResolution({ targetX, targetY });
    UserSettings->ApplyResolutionSettings(false);
    UserSettings->SaveSettings();
    // Принудительно меняем размер окна
    if (OldWindowed == EWindowMode::Windowed) {
      SetLogicalWindowSize(OldResolutionX, OldResolutionY);
    }

    if (auto set = QrFind<USetting>("Fullscreen")) {
      std::vector opts = {"Fullscreen", "WindowedFullscreen", "Windowed", "Windowed"};
      set->StringValue = opts[UserSettings->GetFullscreenMode()];
    }
    if (auto set = QrFind<USetting>("Resolution")) {
      auto res =  UserSettings->GetScreenResolution();
      FStringFormatOrderedArguments args;
      args.Add(res.X);
      args.Add(res.Y);
      set->StringValue = qr::to_string(FString::Format(TEXT("{0}x{1}"), args));
    }
    USetting::UpdateSettingsWidgets();
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
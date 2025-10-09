#include "Public/LazyGameSessionData.h"
#include "Public/GameSessionSubsystem.h"
#include "Public/MainGameInstance.h"
#include "Qr/GameInstanceHelper.h"

#include <Engine/World.h>

UGameSessionSubsystem *FLazyGameSession::Get(const UObject *WorldContextObject) const {
  if (!gss) {
    auto &gi = UGameInstanceHelper::GetGameInstance(WorldContextObject);
    gss = gi.GetSubsystem<UGameSessionSubsystem>();
  }
  return gss;
}
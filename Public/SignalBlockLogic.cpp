#include "SignalBlockLogic.h"

ULogicSignal *USignalBlockLogic::GetSignal() const {
  if (Signal)
    return Signal;
  if (ULogicSignal *init = NewObject<ULogicSignal>(this, ULogicSignal::StaticClass(), TEXT("Signal"))) {
    Signal = init;
    return Signal;
  }
  return nullptr;
}
#include "SignalBlockLogic.h"
#include "Public/LogicSignal.h"

ULogicSignal *USignalBlockLogic::GetSignal() {
  if (Signal)
    return Signal;
  if (ULogicSignal *init = NewObject<ULogicSignal>(this, TEXT("Signal"))) {
    Signal = init;
    return Signal;
  }
  return nullptr;
}
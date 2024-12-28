#pragma once

#define ACCESSOR_FORWARD_CODEGEN(NAME) \
  class U##NAME##Accessor;             \
  class U##NAME##InputAccessor;        \
  class U##NAME##OutputAccessor;       \
  class U##NAME##ConductorAccessor;

#define ACCESSOR_CLASS_CODEGEN(NAME)                                        \
  U##NAME##Accessor *, U##NAME##InputAccessor *, U##NAME##OutputAccessor *, \
    U##NAME##ConductorAccessor *

class EventSystem;
class UPrototype;
class UStaticItem;
class UAutoCrafter;
class USelectCrafter;
class UAdminElectricGeneratorBlockLogic;
class UBedBlockLogic;
class UDoorBlockLogic;
class UFenceBlockLogic;
class UAbstractCrafter;
class URecipe;
class UStaticTip;
class UBlockLogic;
class UStaticBlock;
class UStaticObject;
ACCESSOR_FORWARD_CODEGEN(Fluid)

ACCESSOR_FORWARD_CODEGEN(Electric)
ACCESSOR_FORWARD_CODEGEN(Heat)
ACCESSOR_FORWARD_CODEGEN(Kinetic)
ACCESSOR_FORWARD_CODEGEN(Data)

namespace evo {
namespace PrototypeHelper {
  // inline std::tuple<
  //   UPrototype *, UEntity *, UBlockLogic *, UTieredBlock *, UFilteringBlock *,
  //   UCrafterBase *, USelectCrafter *, UAutoCrafter *, UStaticItem *,
  //   URecipeDictionary *, UStaticBlock *, UTesselator *, UTesselatorCube *,
  //   UTesselatorMarching *, UItemLogic *, UBlockBuilder *, USphereBreaker *, USphereBuilder *,
  //   UBaseAccessor *, UBaseInventoryAccessor *, UInventoryReader *,
  //   UInventoryAccess *, UBaseInventory *, UInventory *, USingleSlotInventory *,
  //   UInventoryContainer *, UInternalInventory *, URecipeInventory *,
  //   UPropsGenerator *, ULayeringGenerator *, UBiome *, UStaticAttach *,
  //   UPropList *, UStaticResearch *, UStaticChapter *, UBedLogic *, UDoorLogic *,
  //   ULampLogic *, UFenceLogic *, USurfaceBuilder *, UPlaneBuilder *,
  //   UFissionReactor *, UFusionReactor *, URobotArm *, UFilteringRobotArm *,
  //   UPump *, UDiode *, UFilteringPump *, UStorageBlock *, UChest *, UItemRack *,
  //   UConductor *, UHeatConductor *, UKineticConductor *, UFluidConductor *,
  //   UDataConductor *, UElectricConductor *, UFluidContainer *, USwitch *,
  //   UKineticSwitch *, UHeatSwitch *, UFluidSwitch *, UDataSwitch *,
  //   UElectricSwitch *, UBatteryBox *, UConveyor *, USplitter *, USorter *,
  //   UPortal *, USignBlock *, UMonitor *, UComputer *, UQuantumComputer *,
  //   ULuaBlock *, UMinerBase *, UDrillingRig *, UPumpjack *,
  //   UDeconstructorCrafter *, UDumpCrafter *, UDumpAnyCrafter *, ULogicCircuit *,
  //   ULogicDisplay *, ULogicController *, ULogicInterface *, UCoreAccessor *,
  //   UHandGenerator *, ACCESSOR_CLASS_CODEGEN(Fluid),
  //   ACCESSOR_CLASS_CODEGEN(Electric), ACCESSOR_CLASS_CODEGEN(Heat),
  //   ACCESSOR_CLASS_CODEGEN(Kinetic), ACCESSOR_CLASS_CODEGEN(Data)>
  //   classes_backup;

  inline std::tuple<UPrototype *, UStaticObject *, UStaticItem *, UStaticBlock *, UBlockLogic *, UStaticTip *, URecipe *, UBlockLogic *, UAbstractCrafter *, UFenceBlockLogic *, UDoorBlockLogic *, UBedBlockLogic *, UAdminElectricGeneratorBlockLogic *, USelectCrafter *, UAutoCrafter *, EventSystem *> classes;

#undef ACCESSOR_FORWARD_CODEGEN
#undef ACCESSOR_CLASS_CODEGEN

} // namespace PrototypeHelper
} // namespace evo
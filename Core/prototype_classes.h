#pragma once

class UPrototype;
class UEntity;
class UBlockLogic;
class UTieredBlock;
class UFilteringBlock;
class UCrafterBase;
class USelectCrafter;
class UAutoCrafter;
class UItem;
class URecipeDictionary;
class UBlock;
class UTesselator;
class UTesselatorCube;
class UTesselatorMarching;
class UItemLogic;
class UBlockBuilder;
class UBaseAccessor;
class UBaseInventoryAccessor;
class UInventoryReader;
class UInventoryAccess;
class UBaseInventory;
class UInventory;
class UInventoryContainer;
class UInternalInventory;
class URecipeInventory;

class UPropsGenerator;
class ULayeringGenerator;
class UBiome;
class UStaticAttach;
class UPropList;
class UStaticResearch;
class UStaticChapter;

class UBedLogic;
class UDoorLogic;
class ULampLogic;
class UFenceLogic;

class USurfaceBuilder;
class UPlaneBuilder;

class UFissionReactor;
class UFusionReactor;

class URobotArm;
class UFilteringRobotArm;

class UPump;
class UDiode;
class UFilteringPump;

class UStorageBlock;
class UChest;
class UItemRack;

class UConductor;
class UHeatConductor;
class UKineticConductor;
class UFluidConductor;
class UDataConductor;
class UElectricConductor;

class UFluidContainer;

class USwitch;
class UKineticSwitch;
class UHeatSwitch;
class UFluidSwitch;
class UDataSwitch;
class UElectricSwitch;

class UBatteryBox;

class UConveyor;
class USplitter;
class USorter;

class UPortal;

class USignBlock;
class UMonitor;

class UComputer;
class UQuantumComputer;

class UMinerBase;
class UDrillingRig;
class UPumpjack;

class ULuaBlock;

class UDeconstructorCrafter;
class UDumpCrafter;
class UDumpAnyCrafter;

class ULogicCircuit;
class ULogicDisplay;
class ULogicController;
class ULogicInterface;

namespace evo { namespace prototype_helper {
inline std::tuple<
    UPrototype *, UEntity *, UBlockLogic *, UTieredBlock *, UFilteringBlock *, UCrafterBase *,
    USelectCrafter *, UAutoCrafter *, UItem *, URecipeDictionary *, UBlock *,
    UTesselator *, UTesselatorCube *, UTesselatorMarching *, UItemLogic *,
    UBlockBuilder *, UBaseAccessor *, UBaseInventoryAccessor *,
    UInventoryReader *, UInventoryAccess *, UBaseInventory *, UInventory *,
    UInventoryContainer *, UInternalInventory *, URecipeInventory *,
    UPropsGenerator *, ULayeringGenerator *, UBiome *, UStaticAttach *,
    UPropList *, UStaticResearch *, UStaticChapter *, UBedLogic *, UDoorLogic *,
    ULampLogic *, UFenceLogic *, USurfaceBuilder *, UPlaneBuilder *,
    UFissionReactor *, UFusionReactor *, URobotArm *, UFilteringRobotArm *,
    UPump *, UDiode *, UFilteringPump *, UStorageBlock *, UChest *, UItemRack *,
    UConductor *, UHeatConductor *, UKineticConductor *, UFluidConductor *,
    UDataConductor *, UElectricConductor *, UFluidContainer *, USwitch *,
    UKineticSwitch *, UHeatSwitch *, UFluidSwitch *, UDataSwitch *,
    UElectricSwitch *, UBatteryBox *, UConveyor *, USplitter *, USorter *,
    UPortal *, USignBlock *, UMonitor *, UComputer *, UQuantumComputer *,
    ULuaBlock *, UMinerBase *, UDrillingRig *, UPumpjack *, UDeconstructorCrafter *,
    UDumpCrafter *, UDumpAnyCrafter *, ULogicCircuit *, ULogicDisplay *, ULogicController *, ULogicInterface *>
    classes;
}} // namespace evo::prototype_helper
// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlockLogic.h"
#include "RailLinkable.h"
#include "RailNodeBlockLogic.generated.h"

class URailNetwork;
class UBlockLogic;
class ABlockActor;
UCLASS()
class URailNodeBlockLogic : public UBlockLogic, public IRailLinkable {
  GENERATED_BODY()

  public:
  URailNodeBlockLogic();

  virtual URailNodeBlockLogic *GetRailNode() override { return this; }

  virtual void BlockBeginPlay() override;
  virtual void BlockEndPlay() override;
  virtual void SetActor(ABlockActor *actor) override;
  virtual void RemoveActorOrRenderable() override;
  virtual void NeighborBlockAdded(UBlockLogic *block, const Vec3i &pos) override;
  virtual void NeighborBlockRemoved(UBlockLogic *block, const Vec3i &pos) override;

  virtual bool IsStation() const { return false; }

  /** Neighbor rail nodes (undirected; both sides are linked together by URailNetwork::TryAddSegment). */
  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TSet<URailNodeBlockLogic *> LinkedRailNodes;

  const TSet<URailNodeBlockLogic *> &GetLinkedRailNodes() const { return LinkedRailNodes; }

  void RailLink(URailNodeBlockLogic *Other);
  void RailUnlink(URailNodeBlockLogic *Other);
  void UnlinkAllRail();

  void TryAddSegmentTo(URailNodeBlockLogic *Other);
  void TryRemoveSegmentTo(URailNodeBlockLogic *Other);

  protected:
  void EnsureNetworkRegistered();
};

// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "Public/Net/NetTransport.h"

class FNetBlobChannel {
  public:
  void StartSend(FNetPeerId Peer, uint32 BlobId, TArray<uint8> Data);

  void Service(INetTransport &Transport);

  bool HandleMessage(INetTransport &Transport, FNetPeerId Peer, const TArray<uint8> &Bytes);

  void RemovePeer(FNetPeerId Peer);

  void Reset();

  static bool OwnsMessageType(uint8 Type);

  TFunction<void(FNetPeerId Peer, uint32 BlobId, const TArray<uint8> &Data)> OnReceived;
  TFunction<void(FNetPeerId Peer, uint32 BlobId, float Pct)> OnSendProgress;
  TFunction<void(FNetPeerId Peer, uint32 BlobId, float Pct)> OnRecvProgress;

  private:
  struct FSend {
    uint32 BlobId = 0;
    TArray<uint8> Data;
    int32 ChunkSize = 0;
    int32 ChunkCount = 0;
    int32 NextToSend = 0; // next chunk index not yet handed to the transport
    int32 AckedThrough = -1; // highest chunk index the receiver has confirmed
    uint32 Crc = 0;
    bool bStarted = false; // Start message sent
  };

  struct FRecv {
    uint32 BlobId = 0;
    int32 TotalBytes = 0;
    int32 ChunkSize = 0;
    int32 ChunkCount = 0;
    uint32 Crc = 0;
    int32 Received = 0; // contiguous chunks received (== next expected index)
    int32 LastAcked = -1;
    TArray<uint8> Buffer;
    bool bDone = false;
  };

  struct FPeerBlobs {
    TArray<FSend> Sends; // FIFO; only the front one is actively transmitting
    TMap<uint32, FRecv> Recvs;
  };

  void ServiceSend(INetTransport &Transport, FNetPeerId Peer, FSend &Send);
  void SendStart(INetTransport &Transport, FNetPeerId Peer, const FSend &Send);
  void SendChunk(INetTransport &Transport, FNetPeerId Peer, const FSend &Send, int32 ChunkIndex);

  void OnStart(INetTransport &Transport, FNetPeerId Peer, uint32 BlobId, int32 TotalBytes, int32 ChunkSize, uint32 Crc);
  void OnChunk(INetTransport &Transport, FNetPeerId Peer, uint32 BlobId, int32 ChunkIndex, const uint8 *Bytes, int32 NumBytes);
  void OnAck(FNetPeerId Peer, uint32 BlobId, int32 ReceivedThrough);
  void OnDone(FNetPeerId Peer, uint32 BlobId);

  TMap<FNetPeerId, FPeerBlobs> Peers;
};

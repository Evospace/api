// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "Public/Net/NetTransport.h"

/**
 * Multiplayer Layer 3 — generic reliable blob transfer (ai/todo/lan_multiplayer_entry_plan.md).
 *
 * Ships arbitrary byte blobs (config / save archives) to a peer over an INetTransport's
 * RELIABLE channel. Because that channel is already reliable + in-order + retransmitting,
 * this layer does NOT re-implement loss recovery — it only adds what the transport can't:
 *   - chunking, to keep each datagram under the MTU,
 *   - cumulative-ack windowing, for end-to-end back-pressure (bound bytes in flight so a
 *     multi-MB save doesn't blast the socket buffer at once),
 *   - a whole-blob CRC32, as an integrity sanity check on reassembly.
 *
 * In-order delivery means the receiver's contiguous chunk count is all the state it needs;
 * the ack is just "I have through chunk N". One sender queue + receiver map per peer, so
 * transfers to different peers run concurrently.
 *
 * Wire (over Reliable), each message [u8 type][...]:
 *   Start [u32 blobId][i32 totalBytes][i32 chunkSize][u32 crc32]
 *   Chunk [u32 blobId][i32 chunkIndex][bytes]
 *   Ack   [u32 blobId][i32 receivedThrough]      (receiver -> sender, drives the window)
 *   Done  [u32 blobId]                           (receiver -> sender, CRC verified)
 *
 * Pure logic: the owner passes the transport into each call, so the channel never holds a
 * transport pointer that could dangle.
 */
class FNetBlobChannel {
  public:
  /** Queue a blob for delivery to Peer. Chunks are pushed during Service(). */
  void StartSend(FNetPeerId Peer, uint32 BlobId, TArray<uint8> Data);

  /** Push pending chunks (within the window) for every active send. Call each tick. */
  void Service(INetTransport &Transport);

  /** Route an inbound blob message. Returns false if Bytes is not a blob message. */
  bool HandleMessage(INetTransport &Transport, FNetPeerId Peer, const TArray<uint8> &Bytes);

  /** Drop all transfer state for one peer (on disconnect). */
  void RemovePeer(FNetPeerId Peer);

  /** Drop all state (on stop). */
  void Reset();

  /** True if the first byte of a message belongs to this channel. */
  static bool OwnsMessageType(uint8 Type);

  // Owner-supplied callbacks (game thread).
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

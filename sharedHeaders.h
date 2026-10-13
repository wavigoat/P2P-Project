#ifndef SHARED_H
#define SHARED_H

#include <string>
#include <vector>
#include <cstdint>
using namespace std;

// Message types as defined in Protocol Description
enum class MessageType : uint8_t {
    CHOKE = 0,
    UNCHOKE = 1,
    INTERESTED = 2,
    NOT_INTERESTED = 3,
    HAVE = 4,
    BITFIELD = 5,
    REQUEST = 6,
    PIECE = 7
};

// Common configuration loaded from Common.cfg
struct CommonConfig {
    int numberOfPreferredNeighbors;
    int unchokingInterval;
    int optimisticUnchokingInterval;
    string fileName;
    long fileSize;
    int pieceSize;
    int numPieces; // Derived via ceiling division
};

// Peer info loaded from PeerInfo.cfg
struct PeerInfo {
    int peerId;
    string hostName;
    int port;
    bool hasFile;
};

// State maintained for each neighbor peer
struct NeighborState {
    int peerId;
    bool bitfieldInitialized = false;
    vector<bool> bitfield;
    bool peerInterestedInMe = false;
    bool iAmChokingPeer = true;
    bool peerChokingMe = true;
    long bytesReceivedInterval = 0;
    int outstandingRequestPiece = -1;
};

#endif
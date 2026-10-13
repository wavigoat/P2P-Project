#include <iostream>
#include "../config.h"

using namespace std;

int main() {
    cout << "--- Testing CommonConfig ---" << endl;
    CommonConfig common = Config::loadCommonConfig("Common.cfg");
    cout << "Preferred Neighbors: " << common.numberOfPreferredNeighbors << endl;
    cout << "Unchoking Interval: " << common.unchokingInterval << endl;
    cout << "Optimistic Interval: " << common.optimisticUnchokingInterval << endl;
    cout << "File Name: " << common.fileName << endl;
    cout << "File Size: " << common.fileSize << endl;
    cout << "Piece Size: " << common.pieceSize << endl;
    cout << "Derived Num Pieces: " << common.numPieces << endl;

    cout << "\n--- Testing PeerInfo ---" << endl;
    vector<PeerInfo> peers = Config::loadPeerInfo("PeerInfo.cfg");
    for (const auto& peer : peers) {
        cout << "ID: " << peer.peerId 
                  << ", Host: " << peer.hostName 
                  << ", Port: " << peer.port 
                  << ", HasFile: " << (peer.hasFile ? "Yes" : "No") << endl;
    }

    cout << "\n--- Testing Predecessors for Peer 1002 ---" << endl;
    auto preds = Config::getPredecessors(1002, peers);
    for (const auto& p : preds) {
        cout << "Predecessor ID: " << p.peerId << endl;
    }

    return 0;
}
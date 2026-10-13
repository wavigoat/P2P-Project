#include "config.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <unordered_set>

using namespace std;

/**
Loads and parses Common.cfg containing global protocol parameters
We use boolean flags to track every required key. If a mandatory key is missing from the file, we catch it
immediately instead of running with uninitialized data
*/
CommonConfig Config::loadCommonConfig(const string& filepath) {
    CommonConfig config;
    ifstream file(filepath);
    
    // If Common.cfg isn't found, bail out instantly. A peer process cannot function without knowing its piece size, file name, or unchoking interval
    if (!file.is_open()) {
        cerr << "Error: Could not open configuration file " << filepath << endl;
        exit(1);
    }

    string key, value;
    bool foundPreferred = false, foundUnchoke = false, foundOpt = false;
    bool foundName = false, foundSize = false, foundPieceSize = false;

    while (file >> key >> value) {
        try {
            if (key == "NumberOfPreferredNeighbors") {
                config.numberOfPreferredNeighbors = stoi(value);
                foundPreferred = true;
            } else if (key == "UnchokingInterval") {
                config.unchokingInterval = stoi(value);
                foundUnchoke = true;
            } else if (key == "OptimisticUnchokingInterval") {
                config.optimisticUnchokingInterval = stoi(value);
                foundOpt = true;
            } else if (key == "FileName") {
                config.fileName = value;
                foundName = true;
            } else if (key == "FileSize") {
                config.fileSize = stol(value);
                foundSize = true;
            } else if (key == "PieceSize") {
                config.pieceSize = stoi(value);
                foundPieceSize = true;
            }
        } catch (const invalid_argument& e) {
            cerr << "Error: Non-numeric value where a number was expected for '" << key << "': " << value << endl;
            exit(1);
        } catch (const out_of_range& e) {
            cerr << "Error: Numeric value out of range for '" << key << "': " << value << endl;
            exit(1);
        }
    }

    if (!foundPreferred || !foundUnchoke || !foundOpt || !foundName || !foundSize || !foundPieceSize) {
        cerr << "Error: Missing one or more required parameters in " << filepath << endl;
        exit(1);
    }

    if (config.fileSize <= 0) {
        cerr << "Error: FileSize must be greater than 0." << endl;
        exit(1);
    }
    if (config.pieceSize <= 0) {
        cerr << "Error: PieceSize must be greater than 0." << endl;
        exit(1);
    }
    /**
    Number of preferred neighbors can technically be 0 (relying solely on optimistic unchoking), 
    so we allow >= 0. However, timer intervals MUST be strictly positive (> 0). If an unchoking interval were 0,
    the timer thread would sleep for 0 seconds, spinning in an infinite loop and pegging CPU at 100%.
    */
    if (config.numberOfPreferredNeighbors < 0 || config.unchokingInterval <= 0 || config.optimisticUnchokingInterval <= 0) {
        cerr << "Error: Interval values must be positive, and neighbor counts cannot be negative." << endl;
        exit(1);
    }
    /**
    Why ceil - plain integer division truncates any fractional remainder
    example from project description: FileSize = 10,000,232 and PieceSize = 32,768
    Integer division would yield 305 pieces and completely drop final 5,992-byte piece
    Using ceil ensures we correctly allocate room and index entries for trailing partial piece
    */
    config.numPieces = ceil(static_cast<double>(config.fileSize) / config.pieceSize);
    return config;
}

/**
Loads and parses PeerInfo.cfg containing network metadata for all participating peers
We explicitly check file.fail() vs file.eof() to ensure a malformed line in the middle of the file triggers
an error instead of silently truncating the rest of the peer list
*/
vector<PeerInfo> Config::loadPeerInfo(const string& filepath) {
    vector<PeerInfo> peers;
    ifstream file(filepath);
    
    if (!file.is_open()) {
        cerr << "Error: Could not open peer info file " << filepath << endl;
        exit(1);
    }

    int id, port, hasFile;
    string host;
    unordered_set<int> seenIds;

    while (file >> id >> host >> port >> hasFile) {
        if (seenIds.count(id)) {
            cerr << "Error: Duplicate peer ID " << id << " found in " << filepath << endl;
            exit(1);
        }
        seenIds.insert(id);

        if (hasFile != 0 && hasFile != 1) {
            cerr << "Error: Invalid hasFile flag '" << hasFile << "' for peer ID " << id << " (must be 0 or 1)" << endl;
            exit(1);
        }

        if (port <= 0 || port > 65535) {
            cerr << "Error: Invalid port number " << port << " for peer ID " << id << endl;
            exit(1);
        }

        peers.push_back({id, host, port, hasFile == 1});
    }

    // If stream failed before hitting EOF, there was a malformed line (e.g., text where number was expected) that broke parsing
    if (file.fail() && !file.eof()) {
        cerr << "Error: Malformed line or parse error in " << filepath << endl;
        exit(1);
    }

    if (peers.empty()) {
        cerr << "Error: " << filepath << " is empty." << endl;
        exit(1);
    }

    return peers;
}

// Finds and returns PeerInfo record corresponding to local peer ID
PeerInfo Config::getPeerInfo(int myId, const vector<PeerInfo>& peers) {
    for (const auto& peer : peers) {
        if (peer.peerId == myId) return peer;
    }
    cerr << "Error: Peer ID " << myId << " not found in PeerInfo.cfg" << endl;
    exit(1);
}

// Returns a list of all peer entries listed BEFORE given peer ID in PeerInfo.cfg
vector<PeerInfo> Config::getPredecessors(int myId, const vector<PeerInfo>& peers) {
    vector<PeerInfo> preds;
    // P2P protocol rule states that a peer only initiates outgoing TCP connections to peers that appear before it in PeerInfo.cfg
    // Peers listed after us will connect to us instead, so we break out of the loop the moment we reach our own ID
    for (const auto& peer : peers) {
        if (peer.peerId == myId) break;
        preds.push_back(peer);
    }
    return preds;
}
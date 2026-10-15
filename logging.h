
#ifndef LOGGING_H
#define LOGGING_H

#include <fstream>
#include <mutex>
#include <string>

class Logger {
public:
    explicit Logger(int peerId);
    void log(const std::string& event);
    void connectedTo(int peerId);
    void connectedFrom(int peerId);
    void preferredNeighbors(const std::string& peerIds);
    void optimisticallyUnchoked(int peerId);
    void unchokedBy(int peerId);
    void chokedBy(int peerId);
    void receivedHave(int peerId, int pieceIndex);
    void receivedInterested(int peerId);
    void receivedNotInterested(int peerId);
    void downloadedPiece(int peerId, int pieceIndex, int pieceCount);
    void completeFile();

private:
    int peerId_;
    std::ofstream file_;
    std::mutex mutex_;
};

#endif

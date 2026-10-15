
#include "logging.h"
#include <ctime>
#include <stdexcept>

using namespace std;

//constructor for logger
Logger::Logger(int peerId) : peerId_(peerId),
    file_("log_peer_" + to_string(peerId) + ".log", ios::app) {

    if (!file_) { //check if file opened successfully
        throw runtime_error("Unable to open log file"); //throw exception if file cannot be opened
    }
}
//write log message to file with timestamp
void Logger::log(const string& event) {
    time_t currentTime = time(nullptr); //get current time
    tm localTime{}; //create a tm struct to hold local time
    // Convert to local time in a thread-safe manner
    #ifdef _WIN32 // Use localtime_s on Windows
        localtime_s(&localTime, &currentTime);
    #else // Use localtime_r on POSIX systems
        localtime_r(&currentTime, &localTime);
    #endif
    // Format the timestamp as a string
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp),
             "%Y-%m-%d %H:%M:%S", &localTime);

    lock_guard<mutex> lock(mutex_); //lock mutex so that only one thread can write to the log file at a time    

    file_ << "[" << timestamp << "]: " << event << endl; //write the timestamp and event to the log file
}
//log connection to peer
void Logger::connectedTo(int peerId) {
    //log connection to peer
    log("Peer " + to_string(peerId_) +
        " makes a connection to Peer " + to_string(peerId) + ".");
}
//log connection from second peer
void Logger::connectedFrom(int peerId) {
    log("Peer " + to_string(peerId_) +
        " is connected from Peer " + to_string(peerId) + ".");
}
//record when a Have message is received from a peer, indicating that the peer has a specific piece of the file.
void Logger::receivedHave(int peerId, int pieceIndex) {
    log("Peer " + to_string(peerId_) +
        " received the 'have' message from " +
        to_string(peerId) + " for the piece " +
        to_string(pieceIndex) + ".");
}
//record when current peer successfully downloads file
void Logger::downloadedPiece(int peerId, int pieceIndex, int pieceCount) {
    log("Peer " + to_string(peerId_) + //peerID identifies the peer that provided the piece
        " has downloaded the piece " +
        to_string(pieceIndex) + " from " + //pieceindex identifies the specific piece that was downloaded
        to_string(peerId) +
        ". Now the number of pieces it has is " +
        to_string(pieceCount) + "."); //piececount represents total number of pieces owned
}
//records when peer has successfully downloaded the complete file
void Logger::completeFile() {
    log("Peer " + to_string(peerId_) +
        " has downloaded the complete file.");
}

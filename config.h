#ifndef CONFIG_H
#define CONFIG_H

#include "sharedHeaders.h"
#include <vector>

using namespace std;

class Config {
public:
    static CommonConfig loadCommonConfig(const string& filepath = "Common.cfg");
    static vector<PeerInfo> loadPeerInfo(const string& filepath = "PeerInfo.cfg");
    
    static PeerInfo getPeerInfo(int myId, const vector<PeerInfo>& peers);
    static vector<PeerInfo> getPredecessors(int myId, const vector<PeerInfo>& peers);
};

#endif 
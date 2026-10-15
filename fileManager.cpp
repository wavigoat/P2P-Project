//file handling, store complete and partial files
#include "fileManager.h"
#include <filesystem>

FileManager::FileManager(const CommonConfig& config,
	     const PeerInfo& self,
	     const std::string& workingDirectory){
	fileSize = config.fileSize;
	pieceSize = config.pieceSize;
	numPieces = config.numPieces;
	filePath = std::filesystem::path(workingDirectory) / 
		("peer_" + std::to_string(self.peerId)) / 
		config.fileName;
}

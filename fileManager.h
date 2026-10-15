#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <vector>
#include "sharedHeaders.h"
#include <fstream>

class FileManager {
private:
	//We store some config info here
	std::uint64_t fileSize;
	std::size_t pieceSize;
	int numPieces;

	std::filesystem::path filePath;
	std::fstream file;

public:
	FileManager(
		const CommonConfig& config,
		const PeerInfo& self,
		const std::string& workingDirectory = ".");
	
	std::size_t pieceLength(int index) const;
	std::vector<std::uint8_t> readPiece(int index);
	void writePiece(int index, std::vector<std::uint8_t> data);
};

#endif

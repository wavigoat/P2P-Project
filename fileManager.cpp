//file handling, store complete and partial files
#include "fileManager.h"
#include <filesystem>
#include <fstream>

FileManager::FileManager(const CommonConfig& config,
	     const PeerInfo& self,
	     const std::string& workingDirectory){
	fileSize = config.fileSize;
	pieceSize = config.pieceSize;
	numPieces = config.numPieces;
	filePath = std::filesystem::path(workingDirectory) / 
		("peer_" + std::to_string(self.peerId)) / 
		config.fileName;
	//We have to make sure the seed file exists before opening it
	if(!std::filesystem::exists(filePath)) {
		throw std::runtime_error("No seed file with the name " + filePath.string() + " exists!");
	}

	file.open(filePath, std::ios::in | std::ios::out | std::ios::binary);
	if(!file.is_open()) {
		throw std::runtime_error("Failed to open: " + filePath.string());
	}
}

std::size_t FileManager::pieceLength(int index) const {
	if (index < 0 || index >= numPieces) throw std::out_of_range("No piece at index");
	std::uint64_t offset = static_cast<std::uint64_t>(index) * pieceSize;
	return static_cast<std::size_t>(std::min<std::uint64_t>(pieceSize, fileSize - offset));
}

std::vector<std::uint8_t> FileManager::readPiece(int index){
	std::vector<std::uint8_t> pieceData(pieceLength(index));
	
	file.seekg(static_cast<std::streamoff>(index) * pieceSize, std::ios::beg);
	if(!file) throw std::runtime_error("Failed to seek piece");
	file.read(reinterpret_cast<char*>(pieceData.data()),
		static_cast<std::streamsize>(pieceData.size()));
	if(!file) throw std::runtime_error("Failed to read piece");
	return pieceData;
}

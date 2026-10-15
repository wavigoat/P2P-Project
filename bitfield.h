#ifndef BITFIELD_H
#define BITFIELD_H

#include <vector>

class Bitfield {
private:
	std::vector<bool> pieces;

public:
	Bitfield(int numPieces, bool hasFile);
	bool hasPiece(int index) const;
	void addPiece(int index);
	int pieceCount() const;
	bool isCopmlete() const;
};

#endif

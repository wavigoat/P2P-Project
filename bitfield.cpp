// interested/not interested
#include "bitfield.h"
#include <vector>
#include <algorithm>

Bitfield::Bitfield(int numPieces, bool hasFile){
	pieces = std::vector<bool>(numPieces, hasFile);
}

bool Bitfield::hasPiece(int index) const{
	if(pieces[index]) return true;	
	return false;
}

void Bitfield::addPiece(int index){
	if(pieces[index] == false) pieces[index] = true;
}

int Bitfield::pieceCount() const{
	return pieces.size();
}

bool Bitfield::isComplete() const{
	bool complete = std::all_of(pieces.begin(), pieces.end(), [](bool p) {return p;});
	return complete;

}

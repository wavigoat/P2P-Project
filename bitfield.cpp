// interested/not interested
#include "bitfield.h"
#include <vector>

Bitfield::Bitfield(int numPieces, bool hasFile){
	pieces = std::vector<bool>(numPieces, hasFile);
}

bool Bitfield::hasPiece(int index) const{
	if(pieces[index]) return true;	
	return false;
}

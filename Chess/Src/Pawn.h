
#pragma once

class Pawn: public Piece
{
private:
protected:
public:
	Pawn(EWhichTeam eWhichTeam);
	~Pawn();

	void LegalTileIndexesICanMoveTo(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr);
	void setTexture();
	void CheckAndAddTile(int iSpotToCheck, int iCurrentColumn, DynamicArray<int>* pOurArr);
	void getAttackTileIndexes(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr);
};
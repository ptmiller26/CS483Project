#pragma once

class Knight: public Piece
{
private:
protected:
public:
	Knight(EWhichTeam eWhichTeam);
	~Knight();

	void LegalTileIndexesICanMoveTo(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr);
	void setTexture();
	void CheckAndAddTile(int iSpotToCheck, DynamicArray<int>* pOurArr);
	void getAttackTileIndexes(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr);
	void CheckAndAddAttackTile(int iSpotToCheck, int iCurrentColumn, DynamicArray<int>* pOurArr);
};
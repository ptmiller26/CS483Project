
#pragma once

class King: public Piece
{
private:
protected:
public:
	King(EWhichTeam eWhichTeam);
	~King();

	void LegalTileIndexesICanMoveTo(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr);
	void setTexture();
	void CheckAndAddTile(int iSpotToCheck, int iCurrentColumn, DynamicArray<int>* pOurArr);
	void getAttackTileIndexes(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr);
	void CheckAndAddAttackTile(int iSpotToCheck, int iCurrentColumn, DynamicArray<int>* pOurArr);
};
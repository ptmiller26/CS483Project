
#pragma once

class Queen: public Piece
{
private:
protected:
public:
	Queen(EWhichTeam eWhichTeam);
	~Queen();

	void LegalTileIndexesICanMoveTo(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr);
	void setTexture();
	bool CheckIfTileIsInBounds(int iSpotToCheck);
	void CheckInOneDirectionAndAddTiles(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr, EWhichDirection eDirection);
	void CheckInOneDirectionAndAddAttackTiles(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr, EWhichDirection eDirection);
	void getAttackTileIndexes(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr);
};
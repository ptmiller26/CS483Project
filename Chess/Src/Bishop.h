
#pragma once

class Bishop: public Piece
{
private:
protected:
public:
	Bishop(EWhichTeam eWhichTeam);
	~Bishop();

	void LegalTileIndexesICanMoveTo(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr);
	void setTexture();
	bool CheckIfTileIsInBounds(int iSpotToCheck);
	void CheckInOneDirectionAndAddTiles(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr, EWhichDirection eDirection);
	void CheckInOneDirectionAndAddAttackTiles(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr, EWhichDirection eDirection);
	void getAttackTileIndexes(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr);
};
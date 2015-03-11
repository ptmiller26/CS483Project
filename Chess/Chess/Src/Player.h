
#pragma once

class Player
{
private:
protected:
public:
	bool					bIAmInCheck;
	int						iScore;
	Piece **				lPlayersChessPieces;
	EWhichTeam				eMyTeamColor;
	DynamicArray<Piece*>*	lPlayersChessPiecesInventory;
	DynamicArray<int>*		lBoardIndexesOfPiecesInPlay;
	DynamicArray<int>*		lOpposingPlayersThreateningTiles;
	

	Player();
	~Player();

	void GivePlayerHisChessPieces();
	void fillBoardIndexArray();
	void recheckBoardIndexArray();
	void checkAttackTiles();
	void checkBoardForThreatenedPieces();
	void RemovePieceFromPieceArray(Piece* piece);

	bool IAmInCheck();
	bool IAmInCheckmate();
	bool AreAnyPiecesThreatened();
};
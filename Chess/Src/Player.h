
#pragma once

class Player
{
private:
protected:
public:
	Player(EWhichTeam eWhichTeam);
	~Player();

	void GivePlayerHisChessPieces();

	void fillBoardIndexArray();

	void recheckBoardIndexArray();

	void checkAttackTiles();

	void checkBoardForThreatenedPieces();

	bool IAmInCheck();
	bool IAmInCheckmate();

	void RemovePieceFromPieceArray(Piece* piece);

	bool AreAnyPiecesThreatened();

	int				iScore;

	Piece **		lPlayersChessPieces;
	EWhichTeam		eMyTeamColor;

	DynamicArray<Piece*>* lPlayersChessPiecesInventory;
	DynamicArray<int>* lBoardIndexesOfPiecesInPlay;
	DynamicArray<int>* lOpposingPlayersThreateningTiles;

	bool			bIAmInCheck;
};
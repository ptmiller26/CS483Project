
#pragma once

class Tile;

class Piece
{
private:
protected:
public:
	bool				bIAmDead;
	bool				bIAmTheSelectedPiece;
	bool				bMyFirstMove;				// set to true initially, then set to false for the rest of the game
	bool				bIAmThreatened;
	bool				bIHaveBackup;				// is true if piece has pieces protecting it (ONLY CALCULATED FOR AI)

	int					iPieceValue;
	int					iBestPieceICanKillValue;
	int					iBestPieceICanKillTile;
	int					iMaxNumMoves;

	string				sPieceName;
	EWhichTeam			eTeamColor;
	EPieceType			ePieceType;
	EvoGUIRectWidget *	pChessPieceWidget;
	EvoTexture *		pChessPieceTexture;
	DynamicArray<int>*	pTilesOfPiecesICanKill;


	Piece();
	~Piece();

	void toggleFirstMoveOff();
	void destroyTexture();
	void MakeSureNoHops();
	void ResetBestKillValues();
	void clone();

	virtual void getAttackTileIndexes(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr) {};
	virtual void setTexture() =0;
	virtual void LegalTileIndexesICanMoveTo(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr) =0;	// "pure virtual function" - we therefore cannot create instance variable of type "piece"
	// by setting this function to zero, we are forcing the program to only make pieces of type K, Q, B, N, R, or P
	// this makes Piece an Abstract Base Class
	// even though we can't create objects of type Piece, we can still create pointers that point to objects of type Piece and we can mess with them like this...
	// Rook ourRook
	// Piece* ourPiece = &ourRook;
	// ourPiece->functionSpecificToRookClass();
	
	int getMaxNumMoves();
	string GetPieceName();
};


#include "stdafx.h"
extern Gameboard * pGameboard;

Piece::Piece()
{
	bIAmDead = false;
	bIAmTheSelectedPiece = false;
	bMyFirstMove = true;
	bIAmThreatened = false;
	iBestPieceICanKillTile = -1;
	iBestPieceICanKillValue = 0;
	pTilesOfPiecesICanKill = new DynamicArray<int>(16);
}

Piece::~Piece()
{

}

string Piece::GetPieceName()
{
	return sPieceName;
}

void Piece::toggleFirstMoveOff()
{
	bMyFirstMove = false;
}

int Piece::getMaxNumMoves()
{
	return iMaxNumMoves;
}

/*void Piece::destroyTexture()
{
	RESMGR->DestroyTexture(pChessPieceTexture);
}*/

void Piece::ResetBestKillValues()
{
	iBestPieceICanKillTile = -1;
	iBestPieceICanKillValue = 0;
}
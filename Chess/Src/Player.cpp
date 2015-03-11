#include "stdafx.h"

extern Gameboard	*pGameboard;

Player::Player(EWhichTeam eWhichTeam)
{
	iScore = 39;
	eMyTeamColor = eWhichTeam;
	lPlayersChessPieces = new Piece*[16];
	lPlayersChessPiecesInventory = new DynamicArray<Piece*>(16);
	//lAIsChessPieces = new Piece*[16];
	GivePlayerHisChessPieces();
	lBoardIndexesOfPiecesInPlay = new DynamicArray<int>(16);
	bIAmInCheck = false;
}

Player::~Player()
{

}

void Player::checkBoardForThreatenedPieces()
{
	for (int i = 0; i < lBoardIndexesOfPiecesInPlay->iCount; i++)
	{
		if (lOpposingPlayersThreateningTiles->Contains(lBoardIndexesOfPiecesInPlay->Arr[i]))
		{
			pGameboard->pGameboardArrayOfTiles[lBoardIndexesOfPiecesInPlay->Arr[i]].pPieceOnThisTile->bIAmThreatened = true;
			// "Our first priority (as the AI) should be the move this threatened piece!!!"
		}
		else
		{
			pGameboard->pGameboardArrayOfTiles[lBoardIndexesOfPiecesInPlay->Arr[i]].pPieceOnThisTile->bIAmThreatened = false;
		}
	}
}

void Player::checkAttackTiles()
{
	DynamicArray<int>* tempArr = new DynamicArray<int>(64);
	for (int i = 0; i < 64; i++)
	{
		Piece* currentPiece = pGameboard->pGameboardArrayOfTiles[i].pPieceOnThisTile;
		if (currentPiece != NULL && currentPiece->eTeamColor != this->eMyTeamColor)
		{
			if (currentPiece->ePieceType == ePawnType || currentPiece->ePieceType == eKnightType || currentPiece->ePieceType == eKingType ||
				currentPiece->ePieceType == eBishopType || currentPiece->ePieceType == eRookType || currentPiece->ePieceType == eQueenType)
			{
				currentPiece->getAttackTileIndexes(i, tempArr);
			}
			else
			{
				currentPiece->LegalTileIndexesICanMoveTo(i, tempArr);
			}
		}
	}
	lOpposingPlayersThreateningTiles = tempArr;
}

void Player::fillBoardIndexArray()
{
	for (int i = 0; i < 64; i++)
	{
		if (pGameboard->pGameboardArrayOfTiles[i].pPieceOnThisTile != NULL && pGameboard->pGameboardArrayOfTiles[i].pPieceOnThisTile->eTeamColor == this->eMyTeamColor)
		{
			lBoardIndexesOfPiecesInPlay->AddItem(i);
		}
	}
}

void Player::recheckBoardIndexArray()	// after each move, this function rechecks each player's array of pieces and updates it
{
	DynamicArray<int>* tempArr = new DynamicArray<int>(lPlayersChessPiecesInventory->iCount);
	iScore = 0;
	for (int i = 0; i < 64; i++)
	{
		// 2/5/2014 putting a check in here that clears the array of kill tiles if pTilesOfPiecesICanKill->iCount is greater than 0
		if (pGameboard->pGameboardArrayOfTiles[i].pPieceOnThisTile != NULL &&
			pGameboard->pGameboardArrayOfTiles[i].pPieceOnThisTile->pTilesOfPiecesICanKill->iCount > 0)
		{
			pGameboard->pGameboardArrayOfTiles[i].pPieceOnThisTile->pTilesOfPiecesICanKill->ClearArray();
		}

		if (pGameboard->pGameboardArrayOfTiles[i].pPieceOnThisTile != NULL && pGameboard->pGameboardArrayOfTiles[i].pPieceOnThisTile->eTeamColor == this->eMyTeamColor)
		{
			tempArr->AddItem(i);
			if (pGameboard->pGameboardArrayOfTiles[i].pPieceOnThisTile->ePieceType != eKingType)
			{
				iScore += pGameboard->pGameboardArrayOfTiles[i].pPieceOnThisTile->iPieceValue;
			}
		}
	}
	lBoardIndexesOfPiecesInPlay = tempArr;
}

void Player::RemovePieceFromPieceArray(Piece* piece)
{
	for (int i = 0; i < lPlayersChessPiecesInventory->iCount; i++)
	{
		if (lPlayersChessPiecesInventory->Arr[i]->ePieceType == piece->ePieceType)
		{
			lPlayersChessPiecesInventory->RemoveItem(i);
			break;
		}
	}
}

void Player::GivePlayerHisChessPieces()
{
	for (int i = 0; i < 16; ++i)
	{
		Piece *myPiece = NULL;
		switch (i)
		{
		case 0:
			myPiece = new Rook(eMyTeamColor);
			lPlayersChessPieces[eFirstRook] = myPiece;
			lPlayersChessPiecesInventory->AddItem(myPiece);
			break;
		case 1:
			myPiece = new Knight(eMyTeamColor);
			lPlayersChessPieces[eFirstKnight] = myPiece;
			lPlayersChessPiecesInventory->AddItem(myPiece);
			break;
		case 2:
			myPiece = new Bishop(eMyTeamColor);
			lPlayersChessPieces[eFirstBishop] = myPiece;
			lPlayersChessPiecesInventory->AddItem(myPiece);
			break;
		case 3:
			myPiece = new Queen(eMyTeamColor);
			lPlayersChessPieces[eQueen] = myPiece;
			lPlayersChessPiecesInventory->AddItem(myPiece);
			break;
		case 4:
			myPiece = new King(eMyTeamColor);
			lPlayersChessPieces[eKing] = myPiece;
			lPlayersChessPiecesInventory->AddItem(myPiece);
			break;
		case 5:
			myPiece = new Bishop(eMyTeamColor);
			lPlayersChessPieces[eSecondBishop] = myPiece;
			lPlayersChessPiecesInventory->AddItem(myPiece);
			break;
		case 6:
			myPiece = new Knight(eMyTeamColor);
			lPlayersChessPieces[eSecondKnight] = myPiece;
			lPlayersChessPiecesInventory->AddItem(myPiece);
			break;
		case 7:
			myPiece = new Rook(eMyTeamColor);
			lPlayersChessPieces[eSecondRook] = myPiece;
			lPlayersChessPiecesInventory->AddItem(myPiece);
			break;
		default:
			myPiece = new Pawn(eMyTeamColor);
			lPlayersChessPieces[i] = myPiece;
			lPlayersChessPiecesInventory->AddItem(myPiece);
		}
		myPiece->eTeamColor = eMyTeamColor;
	}
}

bool Player::AreAnyPiecesThreatened()
{
	for (int i = 0; i < lBoardIndexesOfPiecesInPlay->iCount; i++)
	{
		if (pGameboard->pGameboardArrayOfTiles[lBoardIndexesOfPiecesInPlay->Arr[i]].pPieceOnThisTile->bIAmThreatened == true)
		{
			return true;
		}
	}
	return false;
}

bool Player::IAmInCheck()
{
	for (int i = 0; i < lBoardIndexesOfPiecesInPlay->iCount; i++)
	{
		int iOpposingThreatLevel;
		if (eMyTeamColor == eWhite)
		{
			iOpposingThreatLevel = pGameboard->pGameboardArrayOfTiles[lBoardIndexesOfPiecesInPlay->Arr[i]].iAIThreatLevel;
		}
		else
		{
			iOpposingThreatLevel = pGameboard->pGameboardArrayOfTiles[lBoardIndexesOfPiecesInPlay->Arr[i]].iPlayerThreatLevel;
		}

		if (pGameboard->pGameboardArrayOfTiles[lBoardIndexesOfPiecesInPlay->Arr[i]].pPieceOnThisTile->ePieceType == eKingType && 
		iOpposingThreatLevel > 0)
		{
			return true;
		}
	}
	return false;
}
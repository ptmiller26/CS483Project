#include "stdafx.h"

extern Gameboard * pGameboard;

Pawn::Pawn(EWhichTeam eWhichTeam)
{
	sPieceName = "Pawn";
	ePieceType = ePawnType;
	eTeamColor = eWhichTeam;
	iMaxNumMoves = 4;
	pChessPieceWidget = new EvoGUIRectWidget();
	iPieceValue = 1;
	setTexture();
}

Pawn::~Pawn()
{

}

void Pawn::CheckAndAddTile(int iSpotToCheck, int iCurrentColumn, DynamicArray<int>* pOurArr)
{
	int iSpotToCheckColumn = iSpotToCheck % 8;
	if (iSpotToCheck > -1 && iSpotToCheck < 64 && (abs(iCurrentColumn - iSpotToCheckColumn) < 2))
	{
		if ((pGameboard->pGameboardArrayOfTiles[iSpotToCheck].pPieceOnThisTile == NULL) || (pGameboard->pGameboardArrayOfTiles[iSpotToCheck].pPieceOnThisTile->eTeamColor != this->eTeamColor))
		{
			pOurArr->AddItem(iSpotToCheck);
		}
	}
}

void Pawn::LegalTileIndexesICanMoveTo(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr)
{
	int iCurrentTileColumn = iCurrentLocationOnBoard % 8;
	int i = 1;
	if (eTeamColor == eWhite)
	{
		i = -1;
	}
	if (pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*8)].pPieceOnThisTile == NULL)
	{
		CheckAndAddTile(iCurrentLocationOnBoard + (i*8), iCurrentTileColumn, pOurArr);
		if (pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard].pPieceOnThisTile->bMyFirstMove == true && pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*16)].pPieceOnThisTile == NULL)
		{
			CheckAndAddTile(iCurrentLocationOnBoard + (i*16), iCurrentTileColumn, pOurArr);
		}
	}
	if (iCurrentTileColumn == 0)
	{
		if (eTeamColor == eWhite)
		{
			if ((pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*7)].pPieceOnThisTile != NULL) && 
				(pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*7)].pPieceOnThisTile->eTeamColor != this->eTeamColor))
			{
				CheckAndAddTile(iCurrentLocationOnBoard + (i*7), iCurrentTileColumn, pOurArr);
			}
		}
		else
		{
			if ((pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*9)].pPieceOnThisTile != NULL) && 
				(pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*9)].pPieceOnThisTile->eTeamColor != this->eTeamColor))
			{
				CheckAndAddTile(iCurrentLocationOnBoard + (i*9), iCurrentTileColumn, pOurArr);
			}
		}
		
	}
	if (iCurrentTileColumn == 7)
	{
		if (eTeamColor == eWhite)
		{
			if ((pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*9)].pPieceOnThisTile != NULL) && 
				(pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*9)].pPieceOnThisTile->eTeamColor != this->eTeamColor))
			{
				CheckAndAddTile(iCurrentLocationOnBoard + (i*9), iCurrentTileColumn, pOurArr);
			}
		}
		else
		{
			if ((pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*7)].pPieceOnThisTile != NULL) && 
				(pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*7)].pPieceOnThisTile->eTeamColor != this->eTeamColor))
			{
				CheckAndAddTile(iCurrentLocationOnBoard + (i*7), iCurrentTileColumn, pOurArr);
			}
		}
		
	}
	if (iCurrentTileColumn > 0 && iCurrentTileColumn < 7)	// one of the middle columns
	{
		if ((pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*7)].pPieceOnThisTile != NULL) && 
			(pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*7)].pPieceOnThisTile->eTeamColor != this->eTeamColor))
		{
			CheckAndAddTile(iCurrentLocationOnBoard + (i*7), iCurrentTileColumn, pOurArr);
		}
		if ((pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*9)].pPieceOnThisTile != NULL) && 
			(pGameboard->pGameboardArrayOfTiles[iCurrentLocationOnBoard + (i*9)].pPieceOnThisTile->eTeamColor != this->eTeamColor))
		{
			CheckAndAddTile(iCurrentLocationOnBoard + (i*9), iCurrentTileColumn, pOurArr);
		}
	}
}

void Pawn::setTexture()
{
	if (eTeamColor == eBlack)
	{
		pChessPieceTexture = RESMGR->CreateTexture("Images\\blackpawn.png", NULL, false);
	}
	else
	{
		pChessPieceTexture = RESMGR->CreateTexture("Images\\whitepawn.png", NULL, false);
	}
}

void Pawn::getAttackTileIndexes(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr)
{
	int iCurrentTileColumn = iCurrentLocationOnBoard % 8;
	int i = -1;
	if (eTeamColor == eBlack)
	{
		i = 1;
	}
	if (iCurrentTileColumn == 0 && eTeamColor == eWhite)			// Left-most column of board
	{
		if (pOurArr->Contains(iCurrentLocationOnBoard + (i*7)) == false)
		{
			pOurArr->AddItem(iCurrentLocationOnBoard + (i*7));
		}
	}
	else if (iCurrentTileColumn == 7 && eTeamColor == eWhite)		// Right-most column of board
	{
		if (pOurArr->Contains(iCurrentLocationOnBoard + (i*9)) == false)
		{
			pOurArr->AddItem(iCurrentLocationOnBoard + (i*9));
		}
	}
	else if (iCurrentTileColumn == 0 && eTeamColor == eBlack)
	{
		if (pOurArr->Contains(iCurrentLocationOnBoard + (i*9)) == false)
		{
			pOurArr->AddItem(iCurrentLocationOnBoard + (i*9));
		}
	}
	else if (iCurrentTileColumn == 7 && eTeamColor == eBlack)
	{
		if (pOurArr->Contains(iCurrentLocationOnBoard + (i*7)) == false)
		{
			pOurArr->AddItem(iCurrentLocationOnBoard + (i*7));
		}
	}
	else									// One of the middle columns of the board
	{
		if (pOurArr->Contains(iCurrentLocationOnBoard + (i*7)) == false)
		{
			pOurArr->AddItem(iCurrentLocationOnBoard + (i*7));
		}
		if (pOurArr->Contains(iCurrentLocationOnBoard + (i*9)) == false)
		{
			pOurArr->AddItem(iCurrentLocationOnBoard + (i*9));
		}
	}
}
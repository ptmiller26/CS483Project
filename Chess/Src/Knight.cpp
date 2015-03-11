#include "stdafx.h"

extern Gameboard * pGameboard;

Knight::Knight(EWhichTeam eWhichTeam)
{
	sPieceName = "Knight";
	ePieceType = eKnightType;
	eTeamColor = eWhichTeam;
	iMaxNumMoves = 8;
	pChessPieceWidget = new EvoGUIRectWidget();
	iPieceValue = 3;
	setTexture();
}

Knight::~Knight()
{

}

void Knight::CheckAndAddTile(int iSpotToCheck, DynamicArray<int>* pOurArr)
{
	if (iSpotToCheck > -1 && iSpotToCheck < 64)
	{
		if ((pGameboard->pGameboardArrayOfTiles[iSpotToCheck].pPieceOnThisTile == NULL) || (pGameboard->pGameboardArrayOfTiles[iSpotToCheck].pPieceOnThisTile->eTeamColor != this->eTeamColor))
		{
			if (pOurArr->Contains(iSpotToCheck) == false)
			{
				pOurArr->AddItem(iSpotToCheck);
			}
		}
	}
}

void Knight::LegalTileIndexesICanMoveTo(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr)
{
	int iCurrentTileColumn = iCurrentLocationOnBoard % 8;

	if (iCurrentTileColumn != 0)
	{
		// Check North move (1)
		CheckAndAddTile(iCurrentLocationOnBoard - 17, pOurArr);
		// Check South move (1)
		CheckAndAddTile(iCurrentLocationOnBoard + 15, pOurArr);
	}
	
	if (iCurrentTileColumn != 7)
	{
		// Check North move (2)
		CheckAndAddTile(iCurrentLocationOnBoard - 15, pOurArr);
		// Check South move (2)
		CheckAndAddTile(iCurrentLocationOnBoard + 17, pOurArr);
	}

	if (iCurrentTileColumn != 7 && iCurrentTileColumn != 6)
	{
		// Check East move (1)
		CheckAndAddTile(iCurrentLocationOnBoard - 6, pOurArr);
		// Check East move (2)
		CheckAndAddTile(iCurrentLocationOnBoard + 10, pOurArr);
	}

	if (iCurrentTileColumn != 0 && iCurrentTileColumn != 1)
	{
		// Check West move (1)
		CheckAndAddTile(iCurrentLocationOnBoard - 10, pOurArr);
		// Check West move (2)
		CheckAndAddTile(iCurrentLocationOnBoard + 6, pOurArr);
	}
}

void Knight::setTexture()
{
	if (eTeamColor == eBlack)
	{
		pChessPieceTexture = RESMGR->CreateTexture("Images\\blackknight.png", NULL, false);
	}
	else
	{
		pChessPieceTexture = RESMGR->CreateTexture("Images\\whiteknight.png", NULL, false);
	}
}

void Knight::CheckAndAddAttackTile(int iSpotToCheck, int iCurrentColumn, DynamicArray<int>* pOurArr)
{
	if (iSpotToCheck > -1 && iSpotToCheck < 64)
	{
		if (pOurArr->Contains(iSpotToCheck) == false)
		{
			pOurArr->AddItem(iSpotToCheck);
		}
	}
}

void Knight::getAttackTileIndexes(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr)
{
	int iCurrentTileColumn = iCurrentLocationOnBoard % 8;

	if (iCurrentTileColumn != 0)
	{
		// Check North move (1)
		CheckAndAddAttackTile(iCurrentLocationOnBoard - 17, iCurrentTileColumn, pOurArr);
		// Check South move (1)
		CheckAndAddAttackTile(iCurrentLocationOnBoard + 15, iCurrentTileColumn, pOurArr);
	}

	if (iCurrentTileColumn != 7)
	{
		// Check North move (2)
		CheckAndAddAttackTile(iCurrentLocationOnBoard - 15, iCurrentTileColumn, pOurArr);
		// Check South move (2)
		CheckAndAddAttackTile(iCurrentLocationOnBoard + 17, iCurrentTileColumn, pOurArr);
	}

	if (iCurrentTileColumn != 7 && iCurrentTileColumn != 6)
	{
		// Check East move (1)
		CheckAndAddAttackTile(iCurrentLocationOnBoard - 6, iCurrentTileColumn, pOurArr);
		// Check East move (2)
		CheckAndAddAttackTile(iCurrentLocationOnBoard + 10, iCurrentTileColumn, pOurArr);
	}

	if (iCurrentTileColumn != 0 && iCurrentTileColumn != 1)
	{
		// Check West move (1)
		CheckAndAddAttackTile(iCurrentLocationOnBoard - 10, iCurrentTileColumn, pOurArr);
		// Check West move (2)
		CheckAndAddAttackTile(iCurrentLocationOnBoard + 6, iCurrentTileColumn, pOurArr);
	}
}
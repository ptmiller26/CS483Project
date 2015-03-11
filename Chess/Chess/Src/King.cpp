#include "stdafx.h"

extern Gameboard * pGameboard;

King::King(EWhichTeam eWhichTeam)
{
	sPieceName = "King";
	ePieceType = eKingType;
	eTeamColor = eWhichTeam;
	iMaxNumMoves = 8;
	pChessPieceWidget = new EvoGUIRectWidget();
	iPieceValue = 100;
	setTexture();
}

King::~King()
{
	
}

void King::CheckAndAddTile(int iSpotToCheck, int iCurrentColumn, DynamicArray<int>* pOurArr)
{
	int iSpotToCheckColumn = iSpotToCheck % 8;
	int iOpposingThreatLevel;
	if (eTeamColor == eWhite)
	{
		iOpposingThreatLevel = pGameboard->pGameboardArrayOfTiles[iSpotToCheck].iAIThreatLevel;
	}
	else
	{
		iOpposingThreatLevel = pGameboard->pGameboardArrayOfTiles[iSpotToCheck].iPlayerThreatLevel;
	}
	if (iSpotToCheck > -1 && iSpotToCheck < 64 && (abs(iCurrentColumn - iSpotToCheckColumn) < 2))
	{
		if (iOpposingThreatLevel == 0 && ( (pGameboard->pGameboardArrayOfTiles[iSpotToCheck].pPieceOnThisTile == NULL) || (pGameboard->pGameboardArrayOfTiles[iSpotToCheck].pPieceOnThisTile->eTeamColor != this->eTeamColor) ) )
		{
			if (pOurArr->Contains(iSpotToCheck) == false)
			{
				pOurArr->AddItem(iSpotToCheck);
			}
		}
	}
}

void King::LegalTileIndexesICanMoveTo(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr)
{
	int iCurrentTileColumn = iCurrentLocationOnBoard % 8;

	// Check North
	CheckAndAddTile(iCurrentLocationOnBoard - 8, iCurrentTileColumn, pOurArr);

	// Check South
	CheckAndAddTile(iCurrentLocationOnBoard + 8, iCurrentTileColumn, pOurArr);

	// Check East
	CheckAndAddTile(iCurrentLocationOnBoard + 1, iCurrentTileColumn, pOurArr);

	// Check West
	CheckAndAddTile(iCurrentLocationOnBoard - 1, iCurrentTileColumn, pOurArr);

	// Check North East
	CheckAndAddTile(iCurrentLocationOnBoard - 7, iCurrentTileColumn, pOurArr);

	// Check South East
	CheckAndAddTile(iCurrentLocationOnBoard + 9, iCurrentTileColumn, pOurArr);

	// Check North West
	CheckAndAddTile(iCurrentLocationOnBoard - 9, iCurrentTileColumn, pOurArr);

	// Check South West
	CheckAndAddTile(iCurrentLocationOnBoard + 7, iCurrentTileColumn, pOurArr);
}

void King::setTexture()
{
	if (eTeamColor == eBlack)
	{
		pChessPieceTexture = RESMGR->CreateTexture("Images\\blackking.png", NULL, false);
	}
	else
	{
		pChessPieceTexture = RESMGR->CreateTexture("Images\\whiteking.png", NULL, false);
	}
}

void King::CheckAndAddAttackTile(int iSpotToCheck, int iCurrentColumn, DynamicArray<int>* pOurArr)
{
	int iSpotToCheckColumn = iSpotToCheck % 8;
	if (iSpotToCheck > -1 && iSpotToCheck < 64 && (abs(iCurrentColumn - iSpotToCheckColumn) < 2))
	{
		if (pOurArr->Contains(iSpotToCheck) == false)
		{
			pOurArr->AddItem(iSpotToCheck);
		}
	}
}

void King::getAttackTileIndexes(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr)
{
	int iCurrentTileColumn = iCurrentLocationOnBoard % 8;

	// Check North
	CheckAndAddAttackTile(iCurrentLocationOnBoard - 8, iCurrentTileColumn, pOurArr);

	// Check South
	CheckAndAddAttackTile(iCurrentLocationOnBoard + 8, iCurrentTileColumn, pOurArr);

	// Check East
	CheckAndAddAttackTile(iCurrentLocationOnBoard + 1, iCurrentTileColumn, pOurArr);

	// Check West
	CheckAndAddAttackTile(iCurrentLocationOnBoard - 1, iCurrentTileColumn, pOurArr);

	// Check North East
	CheckAndAddAttackTile(iCurrentLocationOnBoard - 7, iCurrentTileColumn, pOurArr);

	// Check South East
	CheckAndAddAttackTile(iCurrentLocationOnBoard + 9, iCurrentTileColumn, pOurArr);

	// Check North West
	CheckAndAddAttackTile(iCurrentLocationOnBoard - 9, iCurrentTileColumn, pOurArr);

	// Check South West
	CheckAndAddAttackTile(iCurrentLocationOnBoard + 7, iCurrentTileColumn, pOurArr);
}
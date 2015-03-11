#include "stdafx.h"

extern Gameboard * pGameboard;

Bishop::Bishop(EWhichTeam eWhichTeam)
{
	sPieceName = "Bishop";
	ePieceType = eBishopType;
	eTeamColor = eWhichTeam;
	iMaxNumMoves = 13;
	pChessPieceWidget = new EvoGUIRectWidget();
	iPieceValue = 3;
	setTexture();
}

Bishop::~Bishop()
{
	
}

bool Bishop::CheckIfTileIsInBounds(int iSpotToCheck)
{
	if (iSpotToCheck > -1 && iSpotToCheck < 64)	// if it's in bounds
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Bishop::CheckInOneDirectionAndAddTiles(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr, EWhichDirection eDirection)
{
	for (int i = 1; i < 8; i++)
	{
		int iSpotToCheck;
		switch(eDirection)
		{
		case NE:
			iSpotToCheck = iCurrentLocationOnBoard - (i*7);
			break;
		case SE:
			iSpotToCheck = iCurrentLocationOnBoard + (i*9);
			break;
		case NW:
			iSpotToCheck = iCurrentLocationOnBoard - (i*9);
			break;
		case SW:
			iSpotToCheck = iCurrentLocationOnBoard + (i*7);
			break;
		}

		if ((eDirection == NE || eDirection == SE) && iCurrentLocationOnBoard % 8 == 7)	// so that we can't go through the sides of the board heading east
		{
			break;
		}

		if ((eDirection == NW || eDirection == SW) && iCurrentLocationOnBoard % 8 == 0) // so that we can't go through the sides of the board heading west
		{
			break;
		}

		if (CheckIfTileIsInBounds(iSpotToCheck) == true)
		{
			if (pGameboard->pGameboardArrayOfTiles[iSpotToCheck].pWhatIfPiece != NULL || (pGameboard->pGameboardArrayOfTiles[iSpotToCheck].pPieceOnThisTile != NULL &&
				pGameboard->pGameboardArrayOfTiles[iSpotToCheck].bMovingFromThisTile == false) )
				// "if there's a "what if piece" on the tile in question OR
				//  if there is an actual piece on the tile in question that isn't going anywhere
				//  then we should add it to array and break from this loop"
			{
				if (pGameboard->pGameboardArrayOfTiles[iSpotToCheck].pPieceOnThisTile->eTeamColor != this->eTeamColor)	// if this tile's piece is the other teams, we can move to it and take that fucker out
				{
					if (pOurArr->Contains(iSpotToCheck) == false)
					{
						pOurArr->AddItem(iSpotToCheck);
					}
				}
				break;								// break out of this for loop (WE CAN'T GO ANY FURTHER NO MATTER WHAT TYPE OF PIECE WE JUST RAN INTO)
			}
			else	// if the tile we are currently checking doesn't have a piece on it, then ADD IT TO THE MOVE ARRAY BABY!!
			{
				if (pOurArr->Contains(iSpotToCheck) == false)
				{
					pOurArr->AddItem(iSpotToCheck);
				}
				if ((eDirection == NE || eDirection == SE) && iSpotToCheck % 8 == 7)	// if we are checking EAST, and are now on the right-most column of the board, then we can't go any further, so we should break out of this for loop
				{
					break;
				}
				if ((eDirection == NW || eDirection == SW) && iSpotToCheck % 8 == 0)	// if we are checking WEST, and are now on the left-most column of the board, then we can't go any further, so we should break out of this for loop
				{
					break;
				}
			}
		}
	}
}

void Bishop::LegalTileIndexesICanMoveTo(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr)
{
	int iCurrentTileColumn = iCurrentLocationOnBoard % 8;

	CheckInOneDirectionAndAddTiles(iCurrentLocationOnBoard, pOurArr, NE);
	CheckInOneDirectionAndAddTiles(iCurrentLocationOnBoard, pOurArr, SE);
	CheckInOneDirectionAndAddTiles(iCurrentLocationOnBoard, pOurArr, NW);
	CheckInOneDirectionAndAddTiles(iCurrentLocationOnBoard, pOurArr, SW);
}

void Bishop::setTexture()
{
	if (eTeamColor == eBlack)
	{
		pChessPieceTexture = RESMGR->CreateTexture("Images\\blackbishop.png", NULL, false);
	}
	else
	{
		pChessPieceTexture = RESMGR->CreateTexture("Images\\whitebishop.png", NULL, false);
	}
}

void Bishop::getAttackTileIndexes(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr)
{
	int iCurrentTileColumn = iCurrentLocationOnBoard % 8;

	CheckInOneDirectionAndAddAttackTiles(iCurrentLocationOnBoard, pOurArr, NE);
	CheckInOneDirectionAndAddAttackTiles(iCurrentLocationOnBoard, pOurArr, SE);
	CheckInOneDirectionAndAddAttackTiles(iCurrentLocationOnBoard, pOurArr, NW);
	CheckInOneDirectionAndAddAttackTiles(iCurrentLocationOnBoard, pOurArr, SW);
}

void Bishop::CheckInOneDirectionAndAddAttackTiles(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr, EWhichDirection eDirection)
{
	for (int i = 1; i < 8; i++)
	{
		int iSpotToCheck;
		switch(eDirection)
		{
		case NE:
			iSpotToCheck = iCurrentLocationOnBoard - (i*7);
			break;
		case SE:
			iSpotToCheck = iCurrentLocationOnBoard + (i*9);
			break;
		case NW:
			iSpotToCheck = iCurrentLocationOnBoard - (i*9);
			break;
		case SW:
			iSpotToCheck = iCurrentLocationOnBoard + (i*7);
			break;
		}

		if ((eDirection == NE || eDirection == SE) && iCurrentLocationOnBoard % 8 == 7)	// so that we can't go through the sides of the board heading east
		{
			break;
		}

		if ((eDirection == NW || eDirection == SW) && iCurrentLocationOnBoard % 8 == 0) // so that we can't go through the sides of the board heading west
		{
			break;
		}

		if (CheckIfTileIsInBounds(iSpotToCheck) == true)
		{
			if (pGameboard->pGameboardArrayOfTiles[iSpotToCheck].pPieceOnThisTile != NULL)	// if there's a piece on the tile we are checking
			{
				if (pOurArr->Contains(iSpotToCheck) == false)
				{
					pOurArr->AddItem(iSpotToCheck);
				}
				break;								// break out of this for loop (WE CAN'T GO ANY FURTHER NO MATTER WHAT TYPE OF PIECE WE JUST RAN INTO)
			}
			else	// if the tile we are currently checking doesn't have a piece on it, then ADD IT TO THE MOVE ARRAY BABY!!
			{
				if (pOurArr->Contains(iSpotToCheck) == false)
				{
					pOurArr->AddItem(iSpotToCheck);
				}
				if ((eDirection == NE || eDirection == SE) && iSpotToCheck % 8 == 7)	// if we are checking EAST, and are now on the right-most column of the board, then we can't go any further, so we should break out of this for loop
				{
					break;
				}
				if ((eDirection == NW || eDirection == SW) && iSpotToCheck % 8 == 0)	// if we are checking WEST, and are now on the left-most column of the board, then we can't go any further, so we should break out of this for loop
				{
					break;
				}
			}
		}
	}
}
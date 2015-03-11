#include "stdafx.h"

extern Gameboard * pGameboard;

Queen::Queen(EWhichTeam eWhichTeam)
{
	sPieceName = "Queen";
	ePieceType = eQueenType;
	eTeamColor = eWhichTeam;
	iMaxNumMoves = 27;
	pChessPieceWidget = new EvoGUIRectWidget();
	iPieceValue = 9;
	setTexture();
}

Queen::~Queen()
{

}

bool Queen::CheckIfTileIsInBounds(int iSpotToCheck)
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

void Queen::CheckInOneDirectionAndAddTiles(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr, EWhichDirection eDirection)
{
	for (int i = 1; i < 8; i++)
	{
		int iSpotToCheck;
		switch(eDirection)
		{
		case N:
			iSpotToCheck = iCurrentLocationOnBoard - (i*8);
			break;
		case S:
			iSpotToCheck = iCurrentLocationOnBoard + (i*8);
			break;
		case E:
			iSpotToCheck = iCurrentLocationOnBoard + i;
			break;
		case W:
			iSpotToCheck = iCurrentLocationOnBoard - i;
			break;
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

		if ((eDirection == NE || eDirection == SE || eDirection == E) && iCurrentLocationOnBoard % 8 == 7)	// so that we can't go through the sides of the board heading east
		{
			break;
		}

		if ((eDirection == NW || eDirection == SW || eDirection == W) && iCurrentLocationOnBoard % 8 == 0) // so that we can't go through the sides of the board heading west
		{
			break;
		}

		if (CheckIfTileIsInBounds(iSpotToCheck) == true)
		{
			if (pGameboard->pGameboardArrayOfTiles[iSpotToCheck].pPieceOnThisTile != NULL)	// if there's a piece on the tile we are checking
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
				if ((eDirection == NE || eDirection == SE || eDirection == E) && iSpotToCheck % 8 == 7)	// if we are checking EAST, and are now on the right-most column of the board, then we can't go any further, so we should break out of this for loop
				{
					break;
				}
				if ((eDirection == NW || eDirection == SW || eDirection == W) && iSpotToCheck % 8 == 0)	// if we are checking WEST, and are now on the left-most column of the board, then we can't go any further, so we should break out of this for loop
				{
					break;
				}
			}
		}
	}
}

void Queen::LegalTileIndexesICanMoveTo(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr)
{
	int iCurrentTileColumn = iCurrentLocationOnBoard % 8;

	CheckInOneDirectionAndAddTiles(iCurrentLocationOnBoard, pOurArr, N);
	CheckInOneDirectionAndAddTiles(iCurrentLocationOnBoard, pOurArr, S);
	CheckInOneDirectionAndAddTiles(iCurrentLocationOnBoard, pOurArr, E);
	CheckInOneDirectionAndAddTiles(iCurrentLocationOnBoard, pOurArr, W);
	CheckInOneDirectionAndAddTiles(iCurrentLocationOnBoard, pOurArr, NE);
	CheckInOneDirectionAndAddTiles(iCurrentLocationOnBoard, pOurArr, SE);
	CheckInOneDirectionAndAddTiles(iCurrentLocationOnBoard, pOurArr, NW);
	CheckInOneDirectionAndAddTiles(iCurrentLocationOnBoard, pOurArr, SW);
}

void Queen::setTexture()
{
	if (eTeamColor == eBlack)
	{
		pChessPieceTexture = RESMGR->CreateTexture("Images\\blackqueen.png", NULL, false);
	}
	else
	{
		pChessPieceTexture = RESMGR->CreateTexture("Images\\whitequeen.png", NULL, false);
	}
}

void Queen::CheckInOneDirectionAndAddAttackTiles(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr, EWhichDirection eDirection)
{
	for (int i = 1; i < 8; i++)
	{
		int iSpotToCheck;
		switch(eDirection)
		{
		case N:
			iSpotToCheck = iCurrentLocationOnBoard - (i*8);
			break;
		case S:
			iSpotToCheck = iCurrentLocationOnBoard + (i*8);
			break;
		case E:
			iSpotToCheck = iCurrentLocationOnBoard + i;
			break;
		case W:
			iSpotToCheck = iCurrentLocationOnBoard - i;
			break;
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

		if ((eDirection == NE || eDirection == SE || eDirection == E) && iCurrentLocationOnBoard % 8 == 7)	// so that we can't go through the sides of the board heading east
		{
			break;
		}

		if ((eDirection == NW || eDirection == SW || eDirection == W) && iCurrentLocationOnBoard % 8 == 0) // so that we can't go through the sides of the board heading west
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
				if ((eDirection == NE || eDirection == SE || eDirection == E) && iSpotToCheck % 8 == 7)	// if we are checking EAST, and are now on the right-most column of the board, then we can't go any further, so we should break out of this for loop
				{
					break;
				}
				if ((eDirection == NW || eDirection == SW || eDirection == W) && iSpotToCheck % 8 == 0)	// if we are checking WEST, and are now on the left-most column of the board, then we can't go any further, so we should break out of this for loop
				{
					break;
				}
			}
		}
	}
}

void Queen::getAttackTileIndexes(int iCurrentLocationOnBoard, DynamicArray<int>* pOurArr)
{
	int iCurrentTileColumn = iCurrentLocationOnBoard % 8;

	CheckInOneDirectionAndAddAttackTiles(iCurrentLocationOnBoard, pOurArr, N);
	CheckInOneDirectionAndAddAttackTiles(iCurrentLocationOnBoard, pOurArr, S);
	CheckInOneDirectionAndAddAttackTiles(iCurrentLocationOnBoard, pOurArr, E);
	CheckInOneDirectionAndAddAttackTiles(iCurrentLocationOnBoard, pOurArr, W);
	CheckInOneDirectionAndAddAttackTiles(iCurrentLocationOnBoard, pOurArr, NE);
	CheckInOneDirectionAndAddAttackTiles(iCurrentLocationOnBoard, pOurArr, SE);
	CheckInOneDirectionAndAddAttackTiles(iCurrentLocationOnBoard, pOurArr, NW);
	CheckInOneDirectionAndAddAttackTiles(iCurrentLocationOnBoard, pOurArr, SW);
}
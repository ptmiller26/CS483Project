#include "stdafx.h"
#include <algorithm>

extern EvoFont			cFont;
extern EvoGUI			gGUI;

extern Player			* pPlayer;
extern Player			* pAI;

extern Gameboard		* pGameboard;

extern EWhoseTurn		eWhoseTurn;
extern EGameState		eGameState;

extern EMovingState		eMovingState;

extern ECheckState		eCheckState;

extern bool				bSomeoneWon;

extern bool				bThereAreNoKillMovesForAI;

//bool					bIsThereAnimation;
bool					bVertical;
bool					bHorizontal;
bool					bDiagonal;
bool					bAttackOccurred;


int						iSelectedTile;
int						iStartingTileAnimate;
int						iEndingTileAnimate;
int						iHighestValuePieceToTake;
int						iBestMoveInArray;

// measures overall number of tiles each player's pieces "control" ----> each tile's control is determined by:
// +1 for each tile that the player can move to (if it has a piece or not on it)
// an additional plus 'x' if the tile has an opposing player's: PAWN (+1), ROOK (+5), KNIGHT (+3), BISHOP (+3), QUEEN (+9), KING (+1000)
int						iAIThreatLevelOverall;
int						iPlayerThreatLevelOverall;

float					intervalX;
float					intervalY;
float					unitVector;
float					fXs;
float					fYs;
float					fXe;
float					fYe;
float					*fXCurrentForAnimation = new float();
float					*fYCurrentForAnimation = new float();

Gameboard::Gameboard()
{
	pGameboardArrayOfTiles = new Tile[64];	// static array of 64 tiles GAMEBOARD!!!
	for (int i = 0; i < 64; ++i)
	{
		pGameboardArrayOfTiles[i].iLocationOnTable = i;
		pGameboardArrayOfTiles[i].setXYCoordinates();
	}
	SetBorderTileFlags();
	iSelectedTile = -1;
	eMovingState = eChoosingPiece;
	bAIWins = false;
	bPlayerWins = false;
	iHighestValuePieceToTake = 0;
}

Gameboard::~Gameboard()
{

}

void Gameboard::RemoveAllPieces()
{
	for (int i = 0; i < 64; i++)
	{
		if (pGameboardArrayOfTiles[i].pPieceOnThisTile != NULL)
		{
			pGameboardArrayOfTiles[i].RemovePiece();
		}
	}
}

Tile* Gameboard::getGameboardInstance()
{
	return pGameboardArrayOfTiles;
}

void Gameboard::SetBorderTileFlags()
{
	// I want the following tiles to throw up an "edge" flag... 0-7, 56-63, , 24, 32, 40, 48, 15, 23, 31, 39, 47, and 55
	for (int i = 0; i < 64; i++)
	{
		if (i > -1 && i < 8)
		{
			pGameboardArrayOfTiles[i].bEdgeTile = true;
		}
		else if (i > 55 && i < 64)
		{
			pGameboardArrayOfTiles[i].bEdgeTile = true;
		}
		else if (i == 8 || i == 15 || i == 16 || i == 23 || i == 24 || i == 31 || i == 32 || i == 39 || i == 40 || i == 47 || i == 48 || i == 55)
		{
			pGameboardArrayOfTiles[i].bEdgeTile = true;
		}
	}
}

void Gameboard::SetPiecesUpOnTheBoard(Player* player1, Player* AI)
{
	int iSpecialPieceAddition = 56;
	int iPawnPieceAddition = 40;

	// laying down the special pieces
	for (int i = 0; i < 8; ++i)
	{
		pGameboardArrayOfTiles[i].pPieceOnThisTile = AI->lPlayersChessPieces[i];								// AI's special pieces
		pGameboardArrayOfTiles[i + iSpecialPieceAddition].pPieceOnThisTile = player1->lPlayersChessPieces[i];	// Player's special pieces
	}

	// laying down the pawns
	for (int i = 8; i < 16; ++i)
	{
		pGameboardArrayOfTiles[i].pPieceOnThisTile = AI->lPlayersChessPieces[i];								// AI's pawns
		pGameboardArrayOfTiles[i + iPawnPieceAddition].pPieceOnThisTile = player1->lPlayersChessPieces[i];		// Player's pawns
	}
	RecalculateAllTileThreatLevels();
}

void Gameboard::InitializeGameBoard()
{
	for (int i = 0; i < 64; ++i)
	{
		if (pGameboardArrayOfTiles[i].pPieceOnThisTile != NULL)
		{
			pGameboardArrayOfTiles[i].AddInitializedTileToGUI();
		}
	}
}

void Gameboard::DisplayPossibleTilesToMoveTo()		// running through all of the indexes to move to and displaying those that are supposed to be displayed
{
	DynamicArray<int>* movesArr = new DynamicArray<int>(pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->getMaxNumMoves());
	pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->LegalTileIndexesICanMoveTo(iSelectedTile, movesArr);

	if (eWhoseTurn == eAIsTurn && movesArr->iCount == 0)
	{
		// RIGHT HERE
	}

	DynamicArray<int>* filledTilesArr = new DynamicArray<int>(pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->getMaxNumMoves());
	for (int x = 0; x < movesArr->iCount; x++)
	{
		if (pGameboardArrayOfTiles[movesArr->Arr[x]].pPieceOnThisTile != NULL) // if the tile has a piece on it
		{
			filledTilesArr->AddItem(movesArr->Arr[x]);
		}
	}

	int i = 0;
	while (movesArr->Arr[i] < 64 && movesArr->Arr[i] > -1)
	{
		if (pGameboardArrayOfTiles[movesArr->Arr[i]].pPieceOnThisTile == NULL || pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->eTeamColor != pGameboardArrayOfTiles[movesArr->Arr[i]].pPieceOnThisTile->eTeamColor)
		{
			pGameboardArrayOfTiles[movesArr->Arr[i]].AddMovementTileToGUI();
			pGameboardArrayOfTiles[movesArr->Arr[i]].ToggleMovementTileOn();	// movesArr returns the tile numbers that the chosen piece can move to
		}
		i++;
	}
}

void Gameboard::CalculateSelectedTile(long lParam)
{
	iSelectedTileCol = CalculateSelectedCol(lParam);
	iSelectedTileRow = CalculateSelectedRow(lParam);
	if (iSelectedTileRow != -1 && iSelectedTileCol != -1)
	{
		iSelectedTile = (8 * iSelectedTileRow) + iSelectedTileCol;
		
		if (pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile != NULL)	// if the tile you clicked on has a piece in it, then start displaying the possible moves/selected piece
		{
			if ((eWhoseTurn == ePlayersTurn && pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->eTeamColor == eWhite) || 
				(eWhoseTurn == eAIsTurn && pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->eTeamColor == eBlack))
			{
				pGameboardArrayOfTiles[iSelectedTile].ToggleSelectedTileOn();
				pGameboardArrayOfTiles[iSelectedTile].AddSelectedTileToGUI();
				eMovingState = eChoosingDestination;
				DisplayPossibleTilesToMoveTo();
			}
		}
	}
}

/************************************************************************/
/* ONLY CALLED BY AI
	Returns TRUE:
		1) if it has found a piece that it can take, or
		2) if there are no pieces that the AI can take (bThereAreNoKillMovesForAI == true)
	Returns FALSE:
		1) if the selected piece can't make any moves, or
		2) if the selected piece CAN move but it hasn't been confirmed that bThereAreNoKillMovesForAI equals true yet */
/************************************************************************/
bool Gameboard::IsThisPieceAbleToMove(int selectedTileIndex)
{
	iSelectedTile = selectedTileIndex;

	DynamicArray<int>* temp = getIndexesAIPieceCanMoveTo(pGameboardArrayOfTiles[selectedTileIndex].pPieceOnThisTile);
	if (temp != NULL)
	{
		// WE NEED TO RANDOMLY CHOOSE WHICH MOVE ITS GOING TO MAKE
		
		if (bThereAreNoKillMovesForAI == true)	// gone through all possible AI moves and AI can't take any of the player's pieces ...
		{

			// THIS SECTION RANDOMLY SELECTS A MOVE FROM THE CHOSEN PIECE
			int theMove = rand() % temp->iCount;
			if (pAI->lOpposingPlayersThreateningTiles->Contains(temp->Arr[theMove]) == false)
			{
				MoveAIPiece(temp->Arr[theMove]);
			}
			else
			{
				free(temp);
				return false;
			}
			bThereAreNoKillMovesForAI = false;
			free(temp);
			return true;
		}
		else
		{
			// LOOKING FOR PIECE THAT AI CAN TAKE
			for (int i = 0; i < temp->iCount; i++)
			{
				if (pGameboardArrayOfTiles[temp->Arr[i]].pPieceOnThisTile == NULL || 
					pGameboardArrayOfTiles[temp->Arr[i]].pPieceOnThisTile->eTeamColor != eWhite)
				{
					continue;
				}
				else	// hits else if we have found a player piece that the AI can take
				{
					// "If this piece has a higher value than the current 'highest' valued piece, then this new piece is now the one to take"
					if (pGameboardArrayOfTiles[temp->Arr[i]].pPieceOnThisTile->iPieceValue > iHighestValuePieceToTake)
					{
						iHighestValuePieceToTake = pGameboardArrayOfTiles[temp->Arr[i]].pPieceOnThisTile->iPieceValue;
						iBestMoveInArray = temp->Arr[i];
					}
					continue;
				}
				// FOUND A PIECE THAT AI CAN TAKE
				/*MoveAIPiece(temp->Arr[i]);
				pAI->recheckBoardIndexArray();
				pPlayer->recheckBoardIndexArray();
				eWhoseTurn = ePlayersTurn;
				bThereAreNoKillMovesForAI = false;
				free(temp);
				return true;*/
			}

			// after going through all the possible moves, is there a piece to take?
			// if the "highest value" is above zero, then that means that there is at least one piece we can take out
			if (iHighestValuePieceToTake > 0)
			{
				MoveAIPiece(iBestMoveInArray);
				pAI->recheckBoardIndexArray();
				pPlayer->recheckBoardIndexArray();
				if (pAI->IAmInCheck() == true)
				{
					eCheckState = eAIinCheck;
				}
				if (pPlayer->IAmInCheck() == true)
				{
					eCheckState = ePlayerInCheck;
				}
				pAI->checkAttackTiles();
				pAI->checkBoardForThreatenedPieces();
				RecalculateAllTileThreatLevels();
				eWhoseTurn = ePlayersTurn;
				bThereAreNoKillMovesForAI = false;
				free(temp);
				iHighestValuePieceToTake = 0;
				return true;
			}
		}
		free(temp);
		return false;
	}
	else
	{
		free(temp);
		return false;
	}
	
}

/************************************************************************/
/* Returns true if the passed in AI piece can take one one of the Player's pieces
	Also stores the values of the best piece value and tile of the best Player piece the AI can take */
/************************************************************************/
bool Gameboard::CanAIPieceKill(Piece *pPiece, int iCurrentTile)
{
	iSelectedTile = iCurrentTile;
	DynamicArray<int>* temp = getIndexesAIPieceCanMoveTo(pPiece);
	if (temp != NULL)
	{
		for (int i = 0; i < temp->iCount; i++)
		{
			if (pGameboardArrayOfTiles[temp->Arr[i]].pPieceOnThisTile != NULL && pGameboardArrayOfTiles[temp->Arr[i]].pPieceOnThisTile->eTeamColor == eWhite)
			{
				if (pGameboardArrayOfTiles[iCurrentTile].pPieceOnThisTile->pTilesOfPiecesICanKill->Contains(temp->Arr[i]) == false)
				{
					pGameboardArrayOfTiles[iCurrentTile].pPieceOnThisTile->pTilesOfPiecesICanKill->AddItem(temp->Arr[i]);
				}
				if (pGameboardArrayOfTiles[temp->Arr[i]].pPieceOnThisTile->iPieceValue > pPiece->iBestPieceICanKillValue)
				{
					pPiece->iBestPieceICanKillValue = pGameboardArrayOfTiles[temp->Arr[i]].pPieceOnThisTile->iPieceValue;
					pPiece->iBestPieceICanKillTile = temp->Arr[i];
				}
			}
		}
	}
	else
	{
		return false;
	}

	if (pPiece->iBestPieceICanKillValue > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Gameboard::MoveAIPiece(int iTileToMoveTo)
{
	if (WillThisMovePutAIInCheck(iTileToMoveTo) == false)
	{
		if (pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->bMyFirstMove == true)
		{
			pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->toggleFirstMoveOff();
		}

		// Killing a Player piece
		if (pGameboardArrayOfTiles[iTileToMoveTo].pPieceOnThisTile != NULL && pGameboardArrayOfTiles[iTileToMoveTo].pPieceOnThisTile->eTeamColor == eWhite)		// removing a white piece
		{
			pPlayer->RemovePieceFromPieceArray(pGameboardArrayOfTiles[iTileToMoveTo].pPieceOnThisTile);
			if (pGameboardArrayOfTiles[iTileToMoveTo].pPieceOnThisTile->ePieceType == eKingType)
			{
				bSomeoneWon = true;
				bAIWins = true;
			}
			pGameboardArrayOfTiles[iTileToMoveTo].RemovePiece();			// AI TAKES A PIECE FROM PLAYER
			pGameboardArrayOfTiles[iTileToMoveTo].pPieceOnThisTile = NULL;
			SetAnimationVariables(pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile, iSelectedTile, iTileToMoveTo);
			bAttackOccurred = true;
			eMovingState = eAnimating;
		}

		// No Killing, simply just a movement
		else
		{
			SetAnimationVariables(pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile, iSelectedTile, iTileToMoveTo);
			eMovingState = eAnimating;
		}
	}
	else
	{

		/*DynamicArray<int>* tempArr = new DynamicArray<int>(27);
		do 
		{
			int temp = rand() % pAI->lBoardIndexesOfPiecesInPlay->iCount;
			pGameboardArrayOfTiles[pAI->lBoardIndexesOfPiecesInPlay->Arr[temp]].pPieceOnThisTile->LegalTileIndexesICanMoveTo(pAI->lBoardIndexesOfPiecesInPlay->Arr[temp], tempArr);
			if (tempArr->iCount > 0)
			{
				int temp2 = rand() % tempArr->iCount;
				iSelectedTile = pAI->lBoardIndexesOfPiecesInPlay->Arr[temp];
				MoveAIPiece(tempArr->Arr[temp2]);
			}
		} while (tempArr->iCount == 0);
		free(tempArr);*/
		
		// AT THIS POINT WE MUST GO THRU ALL MOVES THAT COULD RESULT IN THE KING NOT BEING BLOCKED

		/*DynamicArray<int>* pListOfZeroThreatTilesForAI = new DynamicArray<int>(64);
		getListOfUnoccupiedThreatFreeTilesForAI(pListOfZeroThreatTilesForAI);
		moveRandomAIPieceToUnthreatenedTile(pListOfZeroThreatTilesForAI);*/
	}
}

bool Gameboard::WillThisMovePutAIInCheck(int EndTile) // (iSelectedTile is the Starting Tile and it's ubiquitous in this class)
{
	if (pGameboardArrayOfTiles[EndTile].pPieceOnThisTile != NULL && pGameboardArrayOfTiles[EndTile].pPieceOnThisTile->eTeamColor == eWhite)	// AKA "we're taking a player piece"
	{
		return false;
	}

	pGameboardArrayOfTiles[EndTile].pWhatIfPiece = pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile;
	pGameboardArrayOfTiles[iSelectedTile].bMovingFromThisTile = true;

	RecalculateAllTileThreatLevels();
	pGameboardArrayOfTiles[EndTile].pWhatIfPiece = NULL;
	pGameboardArrayOfTiles[iSelectedTile].bMovingFromThisTile = false;

	if (iPlayerThreatLevelOverall > 1000)
	{	
		return true;
	}
	else
	{
		return false;
	}
}

/************************************************************************/
/* Returns true when it has found an unthreatened tile to move to... it also takes care of the actual moving process */
/************************************************************************/
bool Gameboard::MoveThreatenedAIPiece(int iIndexOfThreatenedPiece)
{
	DynamicArray<int>* pEscapeTiles = new DynamicArray<int>(28);
	
	pGameboardArrayOfTiles[iIndexOfThreatenedPiece].pPieceOnThisTile->LegalTileIndexesICanMoveTo(iIndexOfThreatenedPiece, pEscapeTiles);

	// "if one of the possible escape tiles is not threatened by the Player, then make that move"
	for (int i = 0; i < pEscapeTiles->iCount; i++)
	{
		if (pAI->lOpposingPlayersThreateningTiles->Contains(pEscapeTiles->Arr[i]) == false)
		{
			iSelectedTile = iIndexOfThreatenedPiece;
			MoveAIPiece(pEscapeTiles->Arr[i]);
			return true;
		}
	}
	return false;
}

DynamicArray<int>* Gameboard::getIndexesAIPieceCanMoveTo(Piece* selectedPiece)
{
	DynamicArray<int>* movesArr = new DynamicArray<int>(selectedPiece->getMaxNumMoves());
	selectedPiece->LegalTileIndexesICanMoveTo(iSelectedTile, movesArr);
	if (movesArr->iCount > 0)
	{
		return movesArr;
	}
	return NULL;
}

void Gameboard::MovePlayerPiece(long lParam)	// always called when it is the movement click (no matter if move is legal
{
	int iMoveToRow = CalculateSelectedRow(lParam);
	int iMoveToCol = CalculateSelectedCol(lParam);
	int iNewTileIndex = (8 * iMoveToRow) + iMoveToCol;
	if (iMoveToRow != -1 && iMoveToCol != -1 && pGameboardArrayOfTiles[iNewTileIndex].bMovementTile == true)
	{	
		if (pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->bMyFirstMove == true)
		{
			pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->toggleFirstMoveOff();
		}
		if (pGameboardArrayOfTiles[iNewTileIndex].pPieceOnThisTile != NULL && pGameboardArrayOfTiles[iNewTileIndex].pPieceOnThisTile->eTeamColor != pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->eTeamColor)
		{
			if (eWhoseTurn == ePlayersTurn)		// AI's piece is getting removed from their array of pieces
			{
				pAI->RemovePieceFromPieceArray(pGameboardArrayOfTiles[iNewTileIndex].pPieceOnThisTile);
			}
			else								// PLAYER's piece is getting removed from array of pieces
			{
				pPlayer->RemovePieceFromPieceArray(pGameboardArrayOfTiles[iNewTileIndex].pPieceOnThisTile);
			}
			if (pGameboardArrayOfTiles[iNewTileIndex].pPieceOnThisTile->ePieceType == eKingType)
			{
				if (pGameboardArrayOfTiles[iNewTileIndex].pPieceOnThisTile->eTeamColor == eBlack)
				{
					bSomeoneWon = true;
					bPlayerWins = true;
				}
				else
				{
					bSomeoneWon = true;
					bAIWins = true;
				}
			}
			pGameboardArrayOfTiles[iNewTileIndex].RemovePiece();			// PLAYER TAKES PIECE FROM AI
			pGameboardArrayOfTiles[iNewTileIndex].pPieceOnThisTile = NULL;
		}
		SetAnimationVariables(pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile, iSelectedTile, iNewTileIndex);
		eMovingState = eAnimating;
	}
	else	//  TURNING OFF DISPLAY BORDERS (PLAYER HAS TRIED TO SELECT AN ILLEGAL SPOT)
	{
		eMovingState = eChoosingPiece;
		pGameboardArrayOfTiles[iSelectedTile].ToggleSelectedTileOff();
		pGameboardArrayOfTiles[iSelectedTile].RemoveBorder();
		ToggleOffMovementTiles();
	}
}

void Gameboard::SwapPieceFromOldToNewTile()
{
	swap(pGameboardArrayOfTiles[iEndingTileAnimate].pPieceOnThisTile, pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile);

	//pGameboardArrayOfTiles[iEndingTileAnimate].AddInitializedTileToGUI();
	//ToggleMovingStateOff();
	pGameboardArrayOfTiles[iSelectedTile].ToggleSelectedTileOff();
	pGameboardArrayOfTiles[iSelectedTile].RemoveBorder();
	ToggleOffMovementTiles();
	pAI->recheckBoardIndexArray();
	pPlayer->recheckBoardIndexArray();
	RecalculateAllTileThreatLevels();

	if (pAI->IAmInCheck() == true)
	{
		eCheckState = eAIinCheck;
	}
	if (pPlayer->IAmInCheck() == true)
	{
		eCheckState = ePlayerInCheck;
	}

	if (eWhoseTurn == ePlayersTurn && bAttackOccurred == true)
	{
		eWhoseTurn = eAIsTurn;
		//bAttackOccurred = false;
	}
	bAttackOccurred = false;
	
	if (eWhoseTurn == ePlayersTurn)
	{
		pAI->checkAttackTiles();
		pAI->checkBoardForThreatenedPieces();
	}
	//eMovingState = eChoosingPiece;
	if (eWhoseTurn == ePlayersTurn)
	{
		bThereAreNoKillMovesForAI = false;		// BIG CHANGE
		eWhoseTurn = eAIsTurn;
	}
	else if (eWhoseTurn == eAIsTurn)
	{
		eWhoseTurn = ePlayersTurn;
	}
}

void Gameboard::SetAnimationVariables(Piece* piece, int startingTile, int endingTile)		// This function mainly deals with setting the animation variables to what they are supposed to be
{
	iStartingTileAnimate = startingTile;
	iEndingTileAnimate = endingTile;
	*fXCurrentForAnimation = pGameboardArrayOfTiles[iStartingTileAnimate].fXcoordinate;
	*fYCurrentForAnimation = pGameboardArrayOfTiles[iStartingTileAnimate].fYcoordinate;
	int tempStartCol = pGameboardArrayOfTiles[startingTile].iLocationOnTable % 8;
	float fXs = 227.5f + (tempStartCol * 106);

	int tempStartRow = pGameboardArrayOfTiles[startingTile].iLocationOnTable / 8;
	float fYs = 55 + (tempStartRow * 75.5f);

	int tempEndCol = pGameboardArrayOfTiles[endingTile].iLocationOnTable % 8;
	float fXe = 227.5f + (tempEndCol * 106);

	int tempEndRow = pGameboardArrayOfTiles[endingTile].iLocationOnTable / 8;
	float fYe = 55 + (tempEndRow * 75.5f);

	unitVector = sqrt(   (   pow(abs(fYe - fYs), (float)2) + pow(abs(fXe - fXs), (float) 2)   )   );

	intervalX = (fXe - fXs) / unitVector;
	intervalY = (fYe - fYs) / unitVector;

	if (intervalX == 0)				// moving VERTICALLY
	{
		bVertical = true;
	}
	else if(intervalY == 0)			// moving HORIZONTALLY
	{
		bHorizontal = true;
	}
	else							// moving DIAGONALLY
	{
		bDiagonal = true;
	}
}

void Gameboard::Animation(float animateVariable)
{
	*fXCurrentForAnimation += (intervalX * animateVariable);
	*fYCurrentForAnimation += (intervalY * animateVariable);
	pGameboardArrayOfTiles[iStartingTileAnimate].pPieceOnThisTile->pChessPieceWidget->SetPosition( *fXCurrentForAnimation, *fYCurrentForAnimation );
}

int Gameboard::CalculateSelectedRow(long lParam)
{
	long lRowSpot = lParam % 45351750;		// 45,351,750 is the LENGTH of the board from the top of the first row to the bottom of the last row (North to South)
											// each tile is 5,251,072 high
	long lTemp = -1;
	if (lRowSpot > (3473568 + (0*5251072)) && lRowSpot < (8520519 + (0*5251072)))		// Row 0
	{
		lTemp = 0;
	}
	else if (lRowSpot > (3473568 + (1*5251072)) && lRowSpot < (8520519 + (1*5251072)))	// Row 1
	{
		lTemp = 1;
	}
	else if (lRowSpot > (3473568 + (2*5251072)) && lRowSpot < (8520519 + (2*5251072)))	// Row 2
	{
		lTemp = 2;
	}
	else if (lRowSpot > (3473568 + (3*5251072)) && lRowSpot < (8520519 + (3*5251072)))	// Row 3
	{
		lTemp = 3;
	}
	else if (lRowSpot > (3473568 + (4*5251072)) && lRowSpot < (8520519 + (4*5251072)))	// Row 4
	{
		lTemp = 4;
	}
	else if (lRowSpot > (3473568 + (5*5251072)) && lRowSpot < (8520519 + (5*5251072)))	// Row 5
	{
		lTemp = 5;
	}
	else if (lRowSpot > (3473568 + (6*5251072)) && lRowSpot < (8520519 + (6*5251072)))	// Row 6
	{
		lTemp = 6;
	}
	else if (lRowSpot > (3473568 + (7*5251072)) && lRowSpot < (8520519 + (7*5251072)))	// Row 7
	{
		lTemp = 7;
	}
	return lTemp;
}

int Gameboard::CalculateSelectedCol(long lParam)
{
	long lColumnSpot = lParam % 1024;					// 1024 is the WIDTH of the board from the left side of the first column to the right side of the last column
	long lTemp = -1;
	if (lColumnSpot > 158 && lColumnSpot < 245)			// Column 0
	{
		lTemp = 0;
	}
	else if (lColumnSpot > 244 && lColumnSpot < 330)	// Column 1
	{
		lTemp = 1;
	}
	else if (lColumnSpot > 329 && lColumnSpot < 415)	// Column 2
	{
		lTemp = 2;
	}
	else if (lColumnSpot > 414 && lColumnSpot < 500)	// Column 3
	{
		lTemp = 3;
	}
	else if (lColumnSpot > 499 && lColumnSpot < 585)	// Column 4
	{
		lTemp = 4;
	}
	else if (lColumnSpot > 584 && lColumnSpot < 670)	// Column 5
	{
		lTemp = 5;
	}
	else if (lColumnSpot > 669 && lColumnSpot < 755)	// Column 6
	{
		lTemp = 6;
	}
	else if (lColumnSpot > 754 && lColumnSpot < 840)	// Column 7
	{
		lTemp = 7;
	}
	return lTemp;
}

void Gameboard::ToggleOffMovementTiles()
{
	for (int i = 0; i < 64; ++i)
	{
		if (pGameboardArrayOfTiles[i].bMovementTile == true)
		{
			pGameboardArrayOfTiles[i].RemoveBorder();
		}
		pGameboardArrayOfTiles[i].ToggleMovementTileOff();
	}
}

/************************************************************************/
/* This function does two things:
	1) Recalculates the AI and Player threat levels for each individual tile ("iAIThreatLevel" and "iPlayerThreatLevel")
	2) Rechecks all of the pieces in play and calculates the boolean "bIHaveBackup"*/
/************************************************************************/
void Gameboard::RecalculateAllTileThreatLevels()
{
	iAIThreatLevelOverall = 0;
	iPlayerThreatLevelOverall = 0;
	for (int x = 0; x < 64; x++)
	{
		// resetting both threat levels
		pGameboardArrayOfTiles[x].iAIThreatLevel = 0;
		pGameboardArrayOfTiles[x].iPlayerThreatLevel = 0;
		if (pGameboardArrayOfTiles[x].pPieceOnThisTile != NULL)
		{
			pGameboardArrayOfTiles[x].pPieceOnThisTile->bIHaveBackup = false;
		}
	}
	for (int i = 0; i < 64; i++)
	{
		if (pGameboardArrayOfTiles[i].pPieceOnThisTile != NULL && pGameboardArrayOfTiles[i].bMovingFromThisTile == false)
		{
			DynamicArray<int>* attackTilesForThisOnePiece = new DynamicArray<int>(pGameboardArrayOfTiles[i].pPieceOnThisTile->iMaxNumMoves);
			pGameboardArrayOfTiles[i].pPieceOnThisTile->getAttackTileIndexes(i, attackTilesForThisOnePiece);
			if (attackTilesForThisOnePiece->iCount > 0)
			{
				for (int x = 0; x < attackTilesForThisOnePiece->iCount; x++)
				{
					if (pGameboardArrayOfTiles[i].pPieceOnThisTile->eTeamColor == eBlack)
					{
						pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].iAIThreatLevel++;
						iAIThreatLevelOverall++;
						if (pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].pPieceOnThisTile != NULL &&
							pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].pPieceOnThisTile->eTeamColor == eWhite &&
							pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].bMovingFromThisTile == false)
						{
							EPieceType eType = pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].pPieceOnThisTile->ePieceType;
							switch (eType)
							{
							case ePawnType:
								iAIThreatLevelOverall++;
								break;
							case eBishopType:
								iAIThreatLevelOverall = iAIThreatLevelOverall + 3;
								break;
							case eKnightType:
								iAIThreatLevelOverall = iAIThreatLevelOverall + 3;
								break;
							case eRookType:
								iAIThreatLevelOverall = iAIThreatLevelOverall + 5;
								break;
							case eQueenType:
								iAIThreatLevelOverall = iAIThreatLevelOverall + 9;
								break;
							case eKingType:
								iAIThreatLevelOverall = iAIThreatLevelOverall + 1000;
								break;
							}
						}
						if (pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].pPieceOnThisTile != NULL && 
							pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].pPieceOnThisTile->eTeamColor == eBlack &&
							pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].bMovingFromThisTile == false)
						{
							pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].pPieceOnThisTile->bIHaveBackup = true;
						}	
						
					}
					else if (pGameboardArrayOfTiles[i].pPieceOnThisTile->eTeamColor == eWhite)
					{
						pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].iPlayerThreatLevel++;
						iPlayerThreatLevelOverall++;
						if (pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].pPieceOnThisTile != NULL &&
							pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].pPieceOnThisTile->eTeamColor == eBlack &&
							pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].bMovingFromThisTile == false)
						{
							EPieceType eType = pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].pPieceOnThisTile->ePieceType;
							switch (eType)
							{
							case ePawnType:
								iPlayerThreatLevelOverall++;
								break;
							case eBishopType:
								iPlayerThreatLevelOverall = iPlayerThreatLevelOverall + 3;
								break;
							case eKnightType:
								iPlayerThreatLevelOverall = iPlayerThreatLevelOverall + 3;
								break;
							case eRookType:
								iPlayerThreatLevelOverall = iPlayerThreatLevelOverall + 5;
								break;
							case eQueenType:
								iPlayerThreatLevelOverall = iPlayerThreatLevelOverall + 9;
								break;
							case eKingType:
								iPlayerThreatLevelOverall = iPlayerThreatLevelOverall + 1000;
								break;
							}
						}
						if (pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].pPieceOnThisTile != NULL && 
							pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].pPieceOnThisTile->eTeamColor == eWhite &&
							pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].bMovingFromThisTile == false)
						{
							pGameboardArrayOfTiles[attackTilesForThisOnePiece->Arr[x]].pPieceOnThisTile->bIHaveBackup = true;
						}	
					}
				}
			}
			free(attackTilesForThisOnePiece);
		}
	}
}

int Gameboard::getAIsMostProtectedTile()
{
	int iHottestTileValue = 0;
	int iHottestTile;
	for (int i = 0; i < 64; i++)
	{
		if (pGameboardArrayOfTiles[i].iAIThreatLevel > iHottestTileValue)
		{
			iHottestTileValue = pGameboardArrayOfTiles[i].iAIThreatLevel;
			iHottestTile = i;
		}
		else if (pGameboardArrayOfTiles[i].iAIThreatLevel == iHottestTileValue)
		{
			int coinFlip = rand() % 2;
			if (coinFlip == 0)
			{
				iHottestTile = i;
			}
		}
	}
	return iHottestTile;
}

int Gameboard::getAIsMostProtectedUnoccupiedTile()
{
	int iHottestTileValue = 0;
	int iHottestTile;
	for (int i = 0; i < 64; i++)
	{
		if (pGameboardArrayOfTiles[i].pPieceOnThisTile == NULL && pGameboardArrayOfTiles[i].iAIThreatLevel > iHottestTileValue)
		{
			iHottestTileValue = pGameboardArrayOfTiles[i].iAIThreatLevel;
			iHottestTile = i;
		}
		else if (pGameboardArrayOfTiles[i].pPieceOnThisTile == NULL && pGameboardArrayOfTiles[i].iAIThreatLevel == iHottestTileValue)
		{
			int coinFlip = rand() % 2;
			if (coinFlip == 0)
			{
				iHottestTile = i;
			}
		}
	}
	return iHottestTile;
}

int Gameboard::getPlayersMostProtectedTile()
{
	int iHottestTileValue = 0;
	int iHottestTile;
	for (int i = 0; i < 64; i++)
	{
		if (pGameboardArrayOfTiles[i].iPlayerThreatLevel > iHottestTileValue)
		{
			iHottestTileValue = pGameboardArrayOfTiles[i].iPlayerThreatLevel;
			iHottestTile = i;
		}
		else if (pGameboardArrayOfTiles[i].iPlayerThreatLevel == iHottestTileValue)
		{
			int coinFlip = rand() % 2;
			if (coinFlip == 0)
			{
				iHottestTile = i;
			}
		}
	}
	return iHottestTile;
}

int Gameboard::getPlayersMostProtectedUnoccupiedTile()
{
	int iHottestTileValue = 0;
	int iHottestTile;
	for (int i = 0; i < 64; i++)
	{
		if (pGameboardArrayOfTiles[i].pPieceOnThisTile == NULL && pGameboardArrayOfTiles[i].iPlayerThreatLevel > iHottestTileValue)
		{
			iHottestTileValue = pGameboardArrayOfTiles[i].iPlayerThreatLevel;
			iHottestTile = i;
		}
		else if (pGameboardArrayOfTiles[i].pPieceOnThisTile == NULL && pGameboardArrayOfTiles[i].iPlayerThreatLevel == iHottestTileValue)
		{
			int coinFlip = rand() % 2;
			if (coinFlip == 0)
			{
				iHottestTile = i;
			}
		}
	}
	return iHottestTile;
}

void Gameboard::getListOfUnoccupiedThreatFreeTilesForAI(DynamicArray<int>* pArr)
{
	for (int i = 0 ; i < 64; i++)
	{
		if (pGameboardArrayOfTiles[i].iPlayerThreatLevel == 0 && pGameboardArrayOfTiles[i].pPieceOnThisTile == NULL)
		{
			pArr->AddItem(i);
		}
	}
}

void Gameboard::moveRandomAIPieceToUnthreatenedTile(DynamicArray<int>* pListOfUnthreatenedTiles)
{
	int iRandomZeroThreatTile;
	DynamicArray<int>* pPiecesAbleToMoveToSpot;

	do
	{
		pPiecesAbleToMoveToSpot = new DynamicArray<int>(16); // created just for it to return something
		iRandomZeroThreatTile = rand() % pListOfUnthreatenedTiles->iCount;
		getPiecesThatCanMoveToThisTile(pListOfUnthreatenedTiles->Arr[iRandomZeroThreatTile], pPiecesAbleToMoveToSpot, eAI);
	}
	while(pPiecesAbleToMoveToSpot->iCount == 0/*pGameboardArrayOfTiles[pListOfUnthreatenedTiles->Arr[iRandomZeroThreatTile]].iAIThreatLevel == 0*/);

	bThereAreNoKillMovesForAI = false;
	if (pPiecesAbleToMoveToSpot->iCount == 1)
	{
		iSelectedTile = pPiecesAbleToMoveToSpot->Arr[0];
	}
	else
	{
		int iRand = rand() % pPiecesAbleToMoveToSpot->iCount;
		iSelectedTile = pPiecesAbleToMoveToSpot->Arr[iRand];
		// we must choose randomly
	}
	MoveAIPiece(pListOfUnthreatenedTiles->Arr[iRandomZeroThreatTile]);

	int x = 0;
	free(pPiecesAbleToMoveToSpot);
}

void Gameboard::getPiecesThatCanMoveToThisTile(int iTileIndex, DynamicArray<int>* pArrOfPieceIndexes, EPlayerOrAI ePlayerOrAI)
{
	if (ePlayerOrAI == eAI)
	{
		for (int i = 0; i < pAI->lBoardIndexesOfPiecesInPlay->iCount; i++)
		{
			DynamicArray<int>* tempMovesArr = new DynamicArray<int>(27);
			int iCurrentTile = pAI->lBoardIndexesOfPiecesInPlay->Arr[i];
			pGameboardArrayOfTiles[iCurrentTile].pPieceOnThisTile->LegalTileIndexesICanMoveTo(iCurrentTile, tempMovesArr);
			if (tempMovesArr->iCount > 0)
			{
				for (int j = 0; j < tempMovesArr->iCount; j++)
				{
					if (tempMovesArr->Arr[j] == iTileIndex)
					{
						// WE HAVE A WINNER!!!!
						pArrOfPieceIndexes->AddItem(iCurrentTile);
						break;
					}
				}
			}
			free(tempMovesArr);
		}
	}
	else
	{
		for (int i = 0; i < pPlayer->lBoardIndexesOfPiecesInPlay->iCount; i++)
		{
			DynamicArray<int>* tempMovesArr = new DynamicArray<int>(27);
			int iCurrentTile = pPlayer->lBoardIndexesOfPiecesInPlay->Arr[i];
			pGameboardArrayOfTiles[iCurrentTile].pPieceOnThisTile->LegalTileIndexesICanMoveTo(iCurrentTile, tempMovesArr);
			if (tempMovesArr->iCount > 0)
			{
				for (int j = 0; j < tempMovesArr->iCount; j++)
				{
					if (tempMovesArr->Arr[j] == iTileIndex)
					{
						// WE HAVE A WINNER!!!!
						pArrOfPieceIndexes->AddItem(iCurrentTile);
						break;
					}
				}
			}
			free(tempMovesArr);
		}
	}
	
}

/*int Gameboard::getTileThatIsThreateningTheAIKing(int iAIKingTile)
{
	DynamicArray<int>* pPiecesThatCanTakeKing = new DynamicArray<int>(16); // created just for it to return something
	getPiecesThatCanMoveToThisTile(iAIKingTile, pPiecesThatCanTakeKing, ePlayer);
	if (pPiecesThatCanTakeKing->iCount == 1)
	{
		return pPiecesThatCanTakeKing->Arr[0];
	}
	else
	{
		// don't know
	}
}*/

int Gameboard::getTileThatIsThreateningThisAIPiece(int iPreyTile)
{
	DynamicArray<int>* pPredatorPieces = new DynamicArray<int>(pAI->lBoardIndexesOfPiecesInPlay->iCount);
	getPiecesThatCanMoveToThisTile(iPreyTile, pPredatorPieces, ePlayer);
	if (pPredatorPieces->iCount == 1)
	{
		return pPredatorPieces->Arr[0];
	}
	else
	{
		return -1;
		// don't know
	}
}

void Gameboard::getLineOfFireTiles(int iPreyTile, int iPredatorTile, EPieceType ePreyPieceType, EPieceType ePredatorPieceType, DynamicArray<int>* pLineOfFireTiles)
{
	// we know that a "line of fire" must be straight
	int iPreyRow = getRowOfTile(iPreyTile);
	int iPreyColumn = iPreyTile % 8;

	int iPredatorRow = getRowOfTile(iPredatorTile);
	int iPredatorColumn = iPredatorTile % 8;

	int iLOFmultiplier;	// (from predator to prey)

	if (ePredatorPieceType == eQueenType || ePredatorPieceType == eBishopType || ePredatorPieceType == eRookType)
	{
		if (iPredatorColumn > iPreyColumn)			// predator is right of prey
		{
			if (iPredatorRow > iPreyRow)
			{
				// Predator is SE of Prey
				iLOFmultiplier = -9;
			}
			else if (iPredatorRow == iPreyRow)
			{
				// Predator is E of Prey
				iLOFmultiplier = -1;
			}
			else
			{
				// Predator is NE of Prey
				iLOFmultiplier = 7;
			}
		}
		else if (iPredatorColumn == iPreyColumn)	// predator is directly above/below prey
		{
			if (iPredatorRow > iPreyRow)
			{
				// Predator is S of Prey
				iLOFmultiplier = -8;
			}
			else
			{
				// Predator is N of Prey
				iLOFmultiplier = 8;
			}
		}
		else										// predator is left of prey
		{
			if (iPredatorRow > iPreyRow)
			{
				// Predator is SW of Prey
				iLOFmultiplier = -7;
			}
			else if (iPredatorRow == iPreyRow)
			{
				// Predator is W of Prey
				iLOFmultiplier = 1;
			}
			else
			{
				// Predator is NW of Prey
				iLOFmultiplier = 9;
			}
		}

		int iCurrentTile = iPredatorTile;
		while (iCurrentTile != iPreyTile)
		{
			iCurrentTile = iCurrentTile + iLOFmultiplier;
			if (iCurrentTile != iPreyTile)
			{
				pLineOfFireTiles->AddItem(iCurrentTile);
			}
		}
	}
	else if (ePredatorPieceType == eKnightType)
	{
		return;
	}
}

int Gameboard::getRowOfTile(int iTileIndex)
{
	if (iTileIndex < 8 && iTileIndex > -1)
	{
		return 0;
	}
	else if (iTileIndex < 16 && iTileIndex > 7)
	{
		return 1;
	}
	else if (iTileIndex < 24 && iTileIndex > 15)
	{
		return 2;
	}
	else if (iTileIndex < 32 && iTileIndex > 23)
	{
		return 3;
	}
	else if (iTileIndex < 40 && iTileIndex > 31)
	{
		return 4;
	}
	else if (iTileIndex < 48 && iTileIndex > 39)
	{
		return 5;
	}
	else if (iTileIndex < 56 && iTileIndex > 47)
	{
		return 6;
	}
	else
	{
		return 7;
	}
}
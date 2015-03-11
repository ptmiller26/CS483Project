#include "stdafx.h"

extern Gameboard	*pGameboard;
extern ECheckState	eCheckState;
extern int			iSelectedTile;
extern bool			bThereAreNoKillMovesForAI;

AIPlayer::AIPlayer()
{
	eMyTeamColor = eBlack;
	GivePlayerHisChessPieces();
}

AIPlayer::~AIPlayer()
{

}

void AIPlayer::RandomlyMoveRandomPiece()
{
	DynamicArray<int>* pListOfZeroThreatTilesForAI = new DynamicArray<int>(64);
	pGameboard->getListOfUnoccupiedThreatFreeTilesForAI(pListOfZeroThreatTilesForAI);
	pGameboard->moveRandomAIPieceToUnthreatenedTile(pListOfZeroThreatTilesForAI);
}

void AIPlayer::DoThisWhenAIKingIsInCheck()
{
	int iIndexOfKing;
	// must deal with this immediately
	for (int i = 0; i < this->lBoardIndexesOfPiecesInPlay->iCount; i++)
	{
		if (pGameboard->pGameboardArrayOfTiles[this->lBoardIndexesOfPiecesInPlay->Arr[i]].pPieceOnThisTile->ePieceType == eKingType)
		{
			iIndexOfKing = this->lBoardIndexesOfPiecesInPlay->Arr[i];
			break;
		}
	}
	if (pGameboard->MoveThreatenedAIPiece(iIndexOfKing) == false)
	{
		int iThreateningTile = pGameboard->getTileThatIsThreateningThisAIPiece(iIndexOfKing);

		// can we take the threatening piece????
		DynamicArray<int>* pPiecesThatCanKillPredator = new DynamicArray<int>(this->lBoardIndexesOfPiecesInPlay->iCount);
		if (CanIKillPredator(iIndexOfKing, iThreateningTile, pPiecesThatCanKillPredator) == false)
		{
			DynamicArray<int>* pLineOfFireTiles = new DynamicArray<int>(8);
			CanPieceBlockThreat(iIndexOfKing, iThreateningTile, pLineOfFireTiles, eKingType);

			// if MoveThreatenedPiece() fails with the King, then we need to figure out how to block it with other pieces

			// For now, I'm just gonna randomly move another piece so that the program still runs smoothly
			//RandomlyMoveARandomPiece();
			eCheckState = eAIinCheckmate;
		}
		
		/*pGameboard->getPiecesThatCanMoveToThisTile(iThreateningTile, pPiecesThatCanKillPredator, eAI);
		if (pPiecesThatCanKillPredator->iCount == 1)
		{
			iSelectedTile = pPiecesThatCanKillPredator->Arr[0];
			pGameboard->MoveAIPiece(iThreateningTile);
			eCheckState = eNoCheck;
			return;
		}
		else if (pPiecesThatCanKillPredator->iCount > 1)
		{
			int iLowestValue = 10;
			int iLowestValueTile = -1;
			for (int x = 0; x < pPiecesThatCanKillPredator->iCount; x++)
			{
				if (pGameboard->pGameboardArrayOfTiles[pPiecesThatCanKillPredator->Arr[x]].pPieceOnThisTile->iPieceValue < iLowestValue)
				{
					iLowestValueTile = pPiecesThatCanKillPredator->Arr[x];
				}
			}
			iSelectedTile = iLowestValueTile;
			pGameboard->MoveAIPiece(iThreateningTile);
			eCheckState = eNoCheck;
			return;
		}
		else
		{
			
		}*/
	}
	else
	{
		eCheckState = eNoCheck;
	}

	return;	// returning here because we don't want to have to deal with anything else after dealing with the threatened king
}


/************************************************************************/
/* Steps of this key function:
	1) Are any of the AI pieces threatened?
		1a) Yes -> Moves the threatened piece (not fully completed) and end turn
		1b) No  -> Move on to check if any other AI pieces can kill Player pieces or simply move in general
	2) Can any AI pieces take out a piece belonging to the Player?
		2a) Yes -> Take out that piece and end turn
		2b) No  -> Set bThereAreNoKillMovesForAI equal to "true"
	3) Pick a random piece of the remaining AI pieces and send it over to the gameboard function "IsThisPieceAbleToMove()"
		3a) Returns false -> this function will keep trying until it finds a piece that can move
		3b) Returns true  -> we have successfully moved a piece and ended the AI's turn */
/************************************************************************/
void AIPlayer::SimulateTurn()
{
	if (eCheckState == eAIinCheck)
	{
		DoThisWhenAIKingIsInCheck();
		return;
	}

	int iCurrentTile;

	// DEFENSIVE PART OF SIMULATEAISTURN()
	if (this->AreAnyPiecesThreatened() == true)
	{
		DefensiveStrategy();
	}

	// OFFENSIVE PART OF SIMULATEAISTURN() ("No AI pieces are threatened")
	else
	{

		//pCopyOfGameboard = new Gameboard(*pGameboard);	// RIGHT HERE AFTER LUNCH 2/10/2014
		//CanIGetPlayerInCheckOnNextTurn(pCopyOfGameboard);
		// if I can... make that move
		// if I cannot... then go about all other checks

		DynamicArray<int>* potentialKillingPiecesArray = new DynamicArray<int>(16);

		for (int i = 0; i < this->lBoardIndexesOfPiecesInPlay->iCount; i++)
		{
			iCurrentTile = this->lBoardIndexesOfPiecesInPlay->Arr[i];
			Piece *pPiece = pGameboard->pGameboardArrayOfTiles[iCurrentTile].pPieceOnThisTile;
			if (pGameboard->CanAIPieceKill(pPiece, iCurrentTile) == true)
			{
				potentialKillingPiecesArray->AddItem(iCurrentTile);
			}
		}
		if (potentialKillingPiecesArray->iCount == 0)
		{
			bThereAreNoKillMovesForAI = true;
		}
		if (potentialKillingPiecesArray->iCount > 0)
		{
			// there is at least 1 AI piece that can kill
			for (int i = 0; i < potentialKillingPiecesArray->iCount; i++)
			{
				int iKillerTile = potentialKillingPiecesArray->Arr[i];
				int iPreyTile = pGameboard->pGameboardArrayOfTiles[iKillerTile].pPieceOnThisTile->iBestPieceICanKillTile;
				if (pGameboard->pGameboardArrayOfTiles[iPreyTile].iPlayerThreatLevel == 0)
				{
					iSelectedTile = iKillerTile;
					pGameboard->MoveAIPiece(pGameboard->pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->iBestPieceICanKillTile);
					pGameboard->pGameboardArrayOfTiles[iKillerTile].pPieceOnThisTile->ResetBestKillValues();
					return;
					// take the piece
				}
				else
				{
					if (pGameboard->pGameboardArrayOfTiles[iPreyTile].pPieceOnThisTile->iPieceValue > 
						pGameboard->pGameboardArrayOfTiles[iKillerTile].pPieceOnThisTile->iPieceValue)
					{
						iSelectedTile = iKillerTile;
						pGameboard->MoveAIPiece(pGameboard->pGameboardArrayOfTiles[iSelectedTile].pPieceOnThisTile->iBestPieceICanKillTile);
						pGameboard->pGameboardArrayOfTiles[iKillerTile].pPieceOnThisTile->ResetBestKillValues();
						return;
					}
					else
					{
						if (CanITakePlayerPieceCleanly() == false)	// if AI can't sneak a Player piece cleanly...
						{
							// THIS IS THE POINT IN TIME WHEN THE PROGRAM RANDOMLY PICKS AN AI PIECE TO MOVE
							RandomlyMoveRandomPiece();
						}
						// don't take the piece
						//RandomlyMoveARandomPiece();
					}
				}
				pGameboard->pGameboardArrayOfTiles[iKillerTile].pPieceOnThisTile->ResetBestKillValues();
			}
		}
		else
		{
			if (CanITakePlayerPieceCleanly() == false)	// if AI can't sneak a Player piece cleanly...
			{
				// THIS IS THE POINT IN TIME WHEN THE PROGRAM RANDOMLY PICKS AN AI PIECE TO MOVE
				RandomlyMoveRandomPiece();
			}
		}

		/*if (pGameboard->IsThisPieceAbleToMove(iCurrentTile) == true)	// if the AI is able to take one of the player's pieces, it will do so
		{
			// if the piece is able to move to at least 1 spot, it will get in here (break out of this for loop)
			// NB: will get in here IF AND ONLY IF bThereAreNoKillMovesForAI is true!!!!
		}
		
		if (eWhoseTurn == eAIsTurn && bAttackOccurred == false)		// if after we've gone through the entire board and we've concluded that the AI cannot take any of the Player's pieces, then we will make a random move
		{
			
		}*/
	}
}

bool AIPlayer::CanWeKillThePredators(DynamicArray<int>* pPredators)
{
	for (int i = 0; i < pPredators->iCount; i++)
	{
		if (CanITakePlayerPieceCleanly(pPredators->Arr[i]) == true)
		{
			return true;
		}
	}

	for (int i = 0; i <pPredators->iCount; i++)
	{
		if (CanITakePlayerPieceMessy(pPredators->Arr[i]) == true)
		{
			return true;
		}
	}
	return false;
}

bool AIPlayer::CanOurMVPKill(int iTileOfMVP)
{
	if (pGameboard->CanAIPieceKill(pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile, iTileOfMVP) == true)
	{
		// IF TRUE: The Player piece is unprotected ("so we will take it")
		if (pGameboard->pGameboardArrayOfTiles[pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile->iBestPieceICanKillTile].iPlayerThreatLevel == 0)
		{
			iSelectedTile = iTileOfMVP;
			pGameboard->MoveAIPiece(pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile->iBestPieceICanKillTile);
			pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile->ResetBestKillValues();
			return true;
		}
		pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile->ResetBestKillValues();
	}
	return false;
}

void AIPlayer::TryToMoveAllThreatenedPieces(int iTileOfMVP, DynamicArray<int>* tilesOfThreatenedPieces)
{
	bool didMovementWork = false;
	bool bGoneThroughAllThreatenedTiles = false;
	int ndx = -1;

	do 
	{
		if (ndx == -1)
		{
			// if this threatened piece is unprotected, we either have to move it, or protect it!
			if (pGameboard->pGameboardArrayOfTiles[iTileOfMVP].iAIThreatLevel == 0)
			{
				didMovementWork = pGameboard->MoveThreatenedAIPiece(iTileOfMVP);
			}
			ndx++;
		}
		else
		{
			if (pGameboard->pGameboardArrayOfTiles[tilesOfThreatenedPieces->Arr[ndx]].iAIThreatLevel == 0)
			{
				didMovementWork = pGameboard->MoveThreatenedAIPiece(tilesOfThreatenedPieces->Arr[ndx++]  /*iTileOfMVP*/);
			}
			else
			{
				ndx++;
			}

			if (ndx == tilesOfThreatenedPieces->iCount)
			{
				bGoneThroughAllThreatenedTiles = true;
				break;
			}
		}
	} while (didMovementWork == false);

	if (didMovementWork == true)
	{
		return;
	}

	if (bGoneThroughAllThreatenedTiles == true && pGameboard->pGameboardArrayOfTiles[iTileOfMVP].iAIThreatLevel == 0 &&
		pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile->ePieceType != ePawnType)
	{
		// "if the best piece can't move AND it doesn't have any AI backup... then we need to look to block the piece!!!!"
		int iThreateningTile = pGameboard->getTileThatIsThreateningThisAIPiece(iTileOfMVP);

		DynamicArray<int>* pLineOfFireTiles = new DynamicArray<int>(8);
		if (CanPieceBlockThreat(iTileOfMVP, iThreateningTile, pLineOfFireTiles, pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile->ePieceType) == false)
		{
			RandomlyMoveRandomPiece();
		}
	}
	else if (didMovementWork == false)
	{
		// THIS IS THE POINT IN TIME WHEN THE PROGRAM RANDOMLY PICKS AN AI PIECE TO MOVE
		RandomlyMoveRandomPiece();
	}
}

void AIPlayer::DefensiveStrategy()
{
	DynamicArray<int>* tilesOfThreatenedPieces = new DynamicArray<int>(15);
	int iTileOfMVP = GetThreatenedPieces(tilesOfThreatenedPieces);
	
	DynamicArray<int>* pPredators = new DynamicArray<int>(16);
	GetPredatorTiles(pPredators, tilesOfThreatenedPieces);
	
	// at this point we have the indexes of all of the "Predator" tiles in pPredators
	CanWeKillThePredators(pPredators);

	// IF TRUE: This MVP piece can kill a Player piece
	if (CanOurMVPKill(iTileOfMVP) == false)
	{
		TryToMoveAllThreatenedPieces(iTileOfMVP, tilesOfThreatenedPieces);
	}
}

void AIPlayer::OffensiveStrategy()
{

}

/************************************************************************/
/* Returns the tile of the MVP                                                                     */
/************************************************************************/
int AIPlayer::GetThreatenedPieces(DynamicArray<int>* tilesOfThreatenedPieces)
{
	int iValueOfTheMostValuablePiece = 0;
	int iTileOfMVP = -1;

	for (int i = 0; i < this->lBoardIndexesOfPiecesInPlay->iCount; i++)
	{
		if (pGameboard->pGameboardArrayOfTiles[this->lBoardIndexesOfPiecesInPlay->Arr[i]].pPieceOnThisTile->bIAmThreatened == true)
		{
			tilesOfThreatenedPieces->AddItem(this->lBoardIndexesOfPiecesInPlay->Arr[i]);
			if (pGameboard->pGameboardArrayOfTiles[this->lBoardIndexesOfPiecesInPlay->Arr[i]].pPieceOnThisTile->iPieceValue > iValueOfTheMostValuablePiece)
			{
				iValueOfTheMostValuablePiece = pGameboard->pGameboardArrayOfTiles[this->lBoardIndexesOfPiecesInPlay->Arr[i]].pPieceOnThisTile->iPieceValue;
				iTileOfMVP = this->lBoardIndexesOfPiecesInPlay->Arr[i];
			}
		}
	}

	if (iValueOfTheMostValuablePiece == 100)
	{
		this->bIAmInCheck = true;
	}

	return iTileOfMVP;
}

void AIPlayer::GetPredatorTiles(DynamicArray<int>* pPredatorTiles, DynamicArray<int>* tilesOfThreatenedPieces)
{
	for (int i = 0; i < tilesOfThreatenedPieces->iCount; i++)
	{
		int iPredator = pGameboard->getTileThatIsThreateningThisAIPiece(tilesOfThreatenedPieces->Arr[i]);
		if (pPredatorTiles->Contains(iPredator) == false)
		{
			pPredatorTiles->AddItem(iPredator);
		}
	}
}


/************************************************************************/
/* Returns TRUE if an AI piece can take one of the Player pieces cleanly
		Also... if it returns TRUE, this function also takes care of the movement of the killer piece and the prey piece
   Returns FALSE if there are no clean takes for the AI
   
   Also, to note, this function is GENERAL... it's not testing just one piece, it's TESTING ALL AI PIECES*/
/************************************************************************/
bool AIPlayer::CanITakePlayerPieceCleanly()
{
	for (int i = 0; i < this->lBoardIndexesOfPiecesInPlay->iCount; i++)
	{
		int pieceIndex = this->lBoardIndexesOfPiecesInPlay->Arr[i];
		pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->ResetBestKillValues();
		if (pGameboard->CanAIPieceKill(pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile, pieceIndex) == true &&
			pGameboard->pGameboardArrayOfTiles[pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->iBestPieceICanKillTile].iPlayerThreatLevel == 0)
		{
			iSelectedTile = pieceIndex;
			pGameboard->MoveAIPiece(pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->iBestPieceICanKillTile);
			pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->ResetBestKillValues();
			return true;
		}
	}
	return false;
}


/************************************************************************/
/* This is the more specific function that checks INDIVIDUAL AND SPECIFIC PREDATOR PIECE                                                                     */
/************************************************************************/
bool AIPlayer::CanITakePlayerPieceCleanly(int iPredatorPiece)
{
	for (int i = 0; i < this->lBoardIndexesOfPiecesInPlay->iCount; i++)
	{
		int pieceIndex = this->lBoardIndexesOfPiecesInPlay->Arr[i];
		pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->ResetBestKillValues();
		if (pGameboard->CanAIPieceKill(pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile, pieceIndex) == true &&
			pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->pTilesOfPiecesICanKill->Contains(iPredatorPiece) == true &&
			/*pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->iBestPieceICanKillTile == iPredatorTile &&*/
			pGameboard->pGameboardArrayOfTiles[pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->iBestPieceICanKillTile].iPlayerThreatLevel == 0)
		{
			iSelectedTile = pieceIndex;
			pGameboard->MoveAIPiece(iPredatorPiece);
			pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->ResetBestKillValues();
			return true;
		}
	}
	return false;
}


/************************************************************************/
/* Doesn't have to be a "clean take" ... HOWEVER, it must be an ADVANTAGEOUS take                                                                     */
/************************************************************************/
bool AIPlayer::CanITakePlayerPieceMessy(int iPredatorPiece)
{
	for (int i = 0; i < this->lBoardIndexesOfPiecesInPlay->iCount; i++)
	{
		int pieceIndex = this->lBoardIndexesOfPiecesInPlay->Arr[i];
		pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->ResetBestKillValues();
		if (pGameboard->CanAIPieceKill(pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile, pieceIndex) == true &&
			pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->pTilesOfPiecesICanKill->Contains(iPredatorPiece) == true
			/*pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->iBestPieceICanKillTile == iPredatorTile*/)
		{
			int iRatio = GetAdvantageRatioOfKill(pieceIndex, iPredatorPiece);
			if (iRatio > 0)	// "if the player piece is worth more than the AI piece, then have the AI piece take it"
			{
				iSelectedTile = pieceIndex;
				pGameboard->MoveAIPiece(iPredatorPiece);
				pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->ResetBestKillValues();
				return true;
			}	
		}
	}
	return false;
}


/************************************************************************/
/* Returns TRUE:
		An AI piece OF A LESSER VALUE THAN THE ONE THREATENED can block the threatened piece (takes care of the movement as well)
   Returns FALSE:
		When either an AI piece is not capable of blocking, or if there is a piece able to block, but that piece is of higher value*/
/************************************************************************/
bool AIPlayer::CanPieceBlockThreat(int iPreyTile, int iPredatorTile, DynamicArray<int>* pLineOfFireTiles, EPieceType ePieceType)
{
	pGameboard->getLineOfFireTiles(iPreyTile, iPredatorTile, ePieceType, pGameboard->pGameboardArrayOfTiles[iPredatorTile].pPieceOnThisTile->ePieceType, pLineOfFireTiles);

	if (pLineOfFireTiles->iCount > 0)
	{
		for (int i = 0; i < pLineOfFireTiles->iCount; i++)
		{
			DynamicArray<int>* pPiecesAble = new DynamicArray<int>(this->lBoardIndexesOfPiecesInPlay->iCount);
			pGameboard->getPiecesThatCanMoveToThisTile(pLineOfFireTiles->Arr[i], pPiecesAble, eAI);
			if (pPiecesAble->iCount == 0)
			{
				continue;	// "if no pieces can move to this tile, then continue going through the other LOF tiles"
			}
			else if (pPiecesAble->iCount == 1)
			{
				// "A single piece can move to one of the line of fire tiles"
				if (pPiecesAble->Arr[0] != iPreyTile)
				{
					if (pGameboard->pGameboardArrayOfTiles[pLineOfFireTiles->Arr[i]].iAIThreatLevel > 0 || 
						(pGameboard->pGameboardArrayOfTiles[iPredatorTile].pPieceOnThisTile->iPieceValue > pGameboard->pGameboardArrayOfTiles[pPiecesAble->Arr[0]].pPieceOnThisTile->iPieceValue))
					{
						// "if the piece that will block is either A) backed up by other pieces OR B) is of lesser value than the predator piece
						iSelectedTile = pPiecesAble->Arr[0];
						pGameboard->MoveAIPiece(pLineOfFireTiles->Arr[i]);
						eCheckState = eNoCheck;
						return true;
					}
				}

			}
			else
			{	// if there are more than 1 pieces capable of blocking
				int iLowestValuablePieceValue = 10;
				int iLowestValuablePieceTile = -1;
				for (int x = 0; x < pPiecesAble->iCount; x++)
				{
					if (pGameboard->pGameboardArrayOfTiles[pPiecesAble->Arr[x]].pPieceOnThisTile->iPieceValue < iLowestValuablePieceValue)
					{
						iLowestValuablePieceTile = pPiecesAble->Arr[x];
					}
				}

				iSelectedTile = iLowestValuablePieceTile;
				pGameboard->MoveAIPiece(pLineOfFireTiles->Arr[i]);
				eCheckState = eNoCheck;
				return true;
			}
		}
	}
	return false;
}


/************************************************************************/
/* Returns true if it has found an AI piece that can kill a the predator piece
	(It also takes care of the movement) 
   Return false if it can't kill the predator piece*/
/************************************************************************/
bool AIPlayer::CanIKillPredator(int iPreyTile, int iPredatorTile, DynamicArray<int>* pPiecesThatCanKillPredator)
{
	pGameboard->getPiecesThatCanMoveToThisTile(iPredatorTile, pPiecesThatCanKillPredator, eAI);
	if (pPiecesThatCanKillPredator->iCount == 1)		// "if there is exactly one AI piece that can kill the predator"
	{
		iSelectedTile = pPiecesThatCanKillPredator->Arr[0];
		pGameboard->MoveAIPiece(iPredatorTile);
		eCheckState = eNoCheck;
		return true;
	}
	else if (pPiecesThatCanKillPredator->iCount > 1)	// "if there are more than one AI pieces that can kill the predator... move shittiest"
	{
		int iLowestValue = 10;
		int iLowestValueTile = -1;
		for (int x = 0; x < pPiecesThatCanKillPredator->iCount; x++)
		{
			if (pGameboard->pGameboardArrayOfTiles[pPiecesThatCanKillPredator->Arr[x]].pPieceOnThisTile->iPieceValue < iLowestValue)
			{
				iLowestValueTile = pPiecesThatCanKillPredator->Arr[x];
			}
		}
		iSelectedTile = iLowestValueTile;
		pGameboard->MoveAIPiece(iPredatorTile);
		eCheckState = eNoCheck;
		return true;
	}
	else												// "if there are no AI pieces that can take out the predator... return false"
	{
		return false;
	}
}


/************************************************************************/
/* Returns the difference between the predator value and the prey value
   In other words, this is returning "How much stronger the predator piece is than the prey piece"
   
   If +: Predator is stronger than Prey
   If -: Prey is stronger than Predator */
/************************************************************************/
int AIPlayer::GetAdvantageRatioOfKill(int iAIPreyTile, int iPlayerPredatorTile)
{
	int preyValue = pGameboard->pGameboardArrayOfTiles[iAIPreyTile].pPieceOnThisTile->iPieceValue;
	int predatorValue = pGameboard->pGameboardArrayOfTiles[iPlayerPredatorTile].pPieceOnThisTile->iPieceValue;

	int ratio = predatorValue - preyValue;
	return ratio;
}
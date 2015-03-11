#include "stdafx.h"

extern EvoFont cFont;
extern EvoGUI gGUI;

Player				* pPlayer;
Player				* pAI;

Gameboard			* pGameboard;

EWhoseTurn			eWhoseTurn;
EGameState			eGameState;
ECheckState			eCheckState;

bool				bSomeoneWon;
bool				bThereAreNoKillMovesForAI;

extern bool			bVertical;
extern bool			bHorizontal;
extern bool			bDiagonal;
extern bool			bAttackOccurred;

extern int			iSelectedTile;
extern float		intervalX;
extern float		intervalY;
extern float		unitVector;
extern int			iStartingTileAnimate;
extern int			iEndingTileAnimate;

extern float		fXs;
extern float		fYs;
extern float		fXe;
extern float		fYe;
extern float		*fXCurrentForAnimation;
extern float		*fYCurrentForAnimation;

EMovingState		eMovingState;

Game::Game() // When drinkingGame variable is created in Main.cpp, it will automatically create 5 players-each player has 50/50 chance of being M or F
{
	pPlayer = new Player(eWhite);
	pAI = new Player(eBlack);
	pGameboard = new Gameboard();
	pGameboard->SetPiecesUpOnTheBoard(pPlayer, pAI);
	pPlayer->fillBoardIndexArray();
	pAI->fillBoardIndexArray();
	bLeftClickDown = false;
	bLeftClickUp = false;
	bSomeoneWon = false;
	iSelectedPiece = 0;
	eWhoseTurn = ePlayersTurn;
	eGameState = eStartOfProgram;
	eCheckState = eNoCheck;
	eMovingState = eChoosingPiece;
	bThereAreNoKillMovesForAI = false;
}

Game::~Game()
{

}

void PromptStart()
{

}

void Game::Initialize()
{
	AddChessBoardWidget();
	pGameboard->InitializeGameBoard();
	eGameState = eInGame;
}

void Game::Tick(float fSecondsPassed)
{
	if (eGameState != eEndOfGame)
	{
		DisplayWhoseTurn();
		DisplayDirections();
		DisplayNumPiecesLeft();
	}
	if (eCheckState != eNoCheck)
	{
		if (eCheckState == eAIinCheck)
		{
			cFont.DrawTextA(1080, 100, "AI in Check");
		}
		else if (eCheckState == ePlayerInCheck)
		{
			cFont.DrawTextA(1080, 590, "You in Check");
		}
	}
	if (bSomeoneWon == true)
	{
		DisplayWinner();
	}
	if (eMovingState == eAnimating)
	{
		if (fSecondsPassed > 0.5)
		{
			fSecondsPassed = 0.0008f;
		}
		float animateVariable = fSecondsPassed * unitVector;
		int x = iEndingTileAnimate;
		float y = *fXCurrentForAnimation;
		float z = *fYCurrentForAnimation;
		if ( abs(*fXCurrentForAnimation - pGameboard->pGameboardArrayOfTiles[iEndingTileAnimate].fXcoordinate) < 1 && abs(*fYCurrentForAnimation - pGameboard->pGameboardArrayOfTiles[iEndingTileAnimate].fYcoordinate) < 1 )
		{
			eMovingState = eChoosingPiece;
			pGameboard->SwapPieceFromOldToNewTile();
			if (eWhoseTurn == eAIsTurn)
			{
				SimulateAIsTurn();
			}
		}
		else
		{
			pGameboard->Animation(animateVariable);
		}
	}
	else
	{
		
	}
}

void Game::DisplayNumPiecesLeft()
{
	char PlayersPieces[27];
	char AIsPieces[27];

	sprintf(PlayersPieces, "Player's Pieces Left: %i", pPlayer->lPlayersChessPiecesInventory->iCount);
	sprintf(AIsPieces, "AI's Pieces Left: %i", pAI->lPlayersChessPiecesInventory->iCount);

	cFont.DrawTextA(975, 675, PlayersPieces);
	cFont.DrawTextA(975, 20, AIsPieces);
}

void Game::DisplayDirections()
{
	char AIsScore[15];
	char PlayersScore[20];

	sprintf(AIsScore, "AI Score: %i", pAI->iScore);
	sprintf(PlayersScore, "Player Score: %i", pPlayer->iScore);

	cFont.DrawTextA(560, 20, AIsScore);
	cFont.DrawTextA(540, 675, PlayersScore);
	//cFont.DrawTextA(400, 675, "If it's your turn, select a piece and move it");
}

void Game::DisplayWhoseTurn()
{
	if (eWhoseTurn == ePlayersTurn)
	{
		cFont.DrawTextA(50, 675, "Player's Turn!!!");
	}
	else
	{
		cFont.DrawTextA(50, 20, "AI/Opponent's Turn!!!");
	}
}

void Game::KeyPressed(char key, long lParam)
{
	if (eGameState != eEndOfGame && eMovingState != eAnimating)
	{
		if (key == 1)						// left click down
		{
			if (bLeftClickDown == false)	// SELECTION CLICK
			{
				pGameboard->CalculateSelectedTile(lParam);
				if (eMovingState == eChoosingDestination)
				{
					bLeftClickDown = true;
				}
			}
			else							// MOVEMENT CLICK
			{
				pGameboard->MovePlayerPiece(lParam);
				bLeftClickDown = false;
			}
		}
		else if (key == 0)					// left click up
		{
			bLeftClickUp = true;
		}
		else
		{
			int x = 0;
		}
	}
}

void Game::AddChessBoardWidget()
{
	pChessBoardWidget = new EvoGUIRectWidget();
	EvoTexture *pChessPieceTexture = RESMGR->CreateTexture("Images\\chessboardgood.png", NULL, false);
	pChessBoardWidget->SetAll(200, 50, 850, 600, pChessPieceTexture);
	gGUI.AddWidget(pChessBoardWidget);
}

void Game::RemoveWidget() // Removes the chess board (NEVER GOING TO BE USED)
{
	gGUI.RemoveWidget(pChessBoardWidget);
}

void Game::DisplayWinner()
{
	if (pGameboard->bPlayerWins == true || pGameboard->bAIWins == true)
	{
		cFont.SetScale(4.0);
		if (pGameboard->bPlayerWins == true)
		{
			cFont.DrawTextA(575, 300, "You win!");
		}
		else
		{
			cFont.DrawTextA(525, 300, "The AI beat you!");
		}
		cFont.SetScale(2.0);

		if (eGameState != eEndOfGame)	// ONLY WANNA DO THIS ONE TIME
		{
			RemoveWidget();
			pGameboard->RemoveAllPieces();
		}

		eGameState = eEndOfGame;
	}
}

void Game::AIKingIsInCheck()
{
	int iIndexOfKing;
	// must deal with this immediately
	for (int i = 0; i < pAI->lBoardIndexesOfPiecesInPlay->iCount; i++)
	{
		if (pGameboard->pGameboardArrayOfTiles[pAI->lBoardIndexesOfPiecesInPlay->Arr[i]].pPieceOnThisTile->ePieceType == eKingType)
		{
			iIndexOfKing = pAI->lBoardIndexesOfPiecesInPlay->Arr[i];
			break;
		}
	}
	if (pGameboard->MoveThreatenedAIPiece(iIndexOfKing) == false)
	{
		int iThreateningTile = pGameboard->getTileThatIsThreateningThisAIPiece(iIndexOfKing);

		// can we take the threatening piece????
		DynamicArray<int>* pPiecesThatCanKillPredator = new DynamicArray<int>(pAI->lBoardIndexesOfPiecesInPlay->iCount);
		if (CanAIKillPredator(iIndexOfKing, iThreateningTile, pPiecesThatCanKillPredator) == false)
		{
			DynamicArray<int>* pLineOfFireTiles = new DynamicArray<int>(8);
			CanAIPieceBlockThreat(iIndexOfKing, iThreateningTile, pLineOfFireTiles, eKingType);

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
void Game::SimulateAIsTurn()
{
	if (eCheckState == eAIinCheck)
	{
		AIKingIsInCheck();
		return;
	}

	int iCurrentTile;

	// DEFENSIVE PART OF SIMULATEAISTURN()
	if (pAI->AreAnyPiecesThreatened() == true)
	{
		int iValueOfTheMostValuablePiece = 0;
		int iTileOfMVP;
		DynamicArray<int>* tilesOfThreatenedPieces = new DynamicArray<int>(15);
		for (int i = 0; i < pAI->lBoardIndexesOfPiecesInPlay->iCount; i++)
		{
			if (pGameboard->pGameboardArrayOfTiles[pAI->lBoardIndexesOfPiecesInPlay->Arr[i]].pPieceOnThisTile->bIAmThreatened == true)
			{
				tilesOfThreatenedPieces->AddItem(pAI->lBoardIndexesOfPiecesInPlay->Arr[i]);
				if (pGameboard->pGameboardArrayOfTiles[pAI->lBoardIndexesOfPiecesInPlay->Arr[i]].pPieceOnThisTile->iPieceValue > iValueOfTheMostValuablePiece)
				{
					iValueOfTheMostValuablePiece = pGameboard->pGameboardArrayOfTiles[pAI->lBoardIndexesOfPiecesInPlay->Arr[i]].pPieceOnThisTile->iPieceValue;
					iTileOfMVP = pAI->lBoardIndexesOfPiecesInPlay->Arr[i];
				}
			}
		}

		if (iValueOfTheMostValuablePiece == 100)
		{
			pAI->bIAmInCheck = true;
		}

		DynamicArray<int>* pPredators = new DynamicArray<int>(16);
		for (int i = 0; i < tilesOfThreatenedPieces->iCount; i++)
		{
			int iPredator = pGameboard->getTileThatIsThreateningThisAIPiece(tilesOfThreatenedPieces->Arr[i]);
			if (pPredators->Contains(iPredator) == false)
			{
				pPredators->AddItem(iPredator);
			}
		}

		// at this point we have the indexes of all of the "Predator" tiles

		for (int i = 0; i < pPredators->iCount; i++)
		{
			if (CanAITakePlayerPieceCleanly(pPredators->Arr[i]) == true)
			{
				return;
			}
		}

		for (int i = 0; i <pPredators->iCount; i++)
		{
			if (CanAITakePlayerPieceMessy(pPredators->Arr[i]) == true)
			{
				return;
			}
		}

		// newly added at 5:48PM on Monday 1/27/14 (POSSIBLY TAKE THIS PART OUT!!!! 2/5/2014 at 1:59PM)
		/*if (CanAITakeAPlayerPieceCleanly() == true)
		{
			return;
		}*/

		// IF TRUE: This MVP piece can kill a Player piece
		if (pGameboard->CanAIPieceKill(pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile, iTileOfMVP) == true)
		{
			// IF TRUE: The Player piece is unprotected ("so we will take it")
			if (pGameboard->pGameboardArrayOfTiles[pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile->iBestPieceICanKillTile].iPlayerThreatLevel == 0)
			{
				iSelectedTile = iTileOfMVP;
				pGameboard->MoveAIPiece(pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile->iBestPieceICanKillTile);
				pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile->ResetBestKillValues();
				return;
			}
			pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile->ResetBestKillValues();
		}

		// RIGHT NOW 1:31PM 1/24/14 ... if the threatened piece can take a piece (and if the "prey" piece is unprotected)
		// THEN IT WILL TAKE IT

		// Now we need to figure out what we do after we realize the threatened piece can't take an unprotected Player piece

		bool didMovementWork = false;
		bool bGoneThroughAllThreatenedTiles = false;
		//bool bIHaveBackup = false;
		int ndx = -1;
		
		// On Wednesday 1/29/14... try to use "canAIPieceMoveToThisTile()" in these here parts
		// already have a good function for this!!!! ... it's called getPiecesThatCanMoveToThisTile() in Gameboard.cpp
		// use this on Wednesday!!!!
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
		// RIGHT HERE WE NEED TO ADD THE FUNCTION THAT CHECKS "CanAITakePlayerPieceCleanly()"

		// THIS IS FOR WHEN IMPORTANT PIECES ARE THREATENED AND CAN'T MOVE... NOT IMPLEMENTED YET (1/27/14)
		// FOR WHEN I WANT TO IMPLEMENT BLOCKING
		if (bGoneThroughAllThreatenedTiles == true && pGameboard->pGameboardArrayOfTiles[iTileOfMVP].iAIThreatLevel == 0 &&
			pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile->ePieceType != ePawnType)
		{
			// "if the best piece can't move AND it doesn't have any AI backup... then we need to look to block the piece!!!!"
			int iThreateningTile = pGameboard->getTileThatIsThreateningThisAIPiece(iTileOfMVP);
			
			DynamicArray<int>* pLineOfFireTiles = new DynamicArray<int>(8);
			if (CanAIPieceBlockThreat(iTileOfMVP, iThreateningTile, pLineOfFireTiles, pGameboard->pGameboardArrayOfTiles[iTileOfMVP].pPieceOnThisTile->ePieceType) == false)
			{
				RandomlyMoveARandomPiece();
			}
		}
		else if (didMovementWork == false)
		{
			// THIS IS THE POINT IN TIME WHEN THE PROGRAM RANDOMLY PICKS AN AI PIECE TO MOVE
			RandomlyMoveARandomPiece();
		}
	}

	// OFFENSIVE PART OF SIMULATEAISTURN() ("No AI pieces are threatened")
	else
	{
		DynamicArray<int>* potentialKillingPiecesArray = new DynamicArray<int>(16);

		for (int i = 0; i < pAI->lBoardIndexesOfPiecesInPlay->iCount; i++)
		{
			iCurrentTile = pAI->lBoardIndexesOfPiecesInPlay->Arr[i];
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
						if (CanAITakePlayerPieceCleanly() == false)	// if AI can't sneak a Player piece cleanly...
						{
							// THIS IS THE POINT IN TIME WHEN THE PROGRAM RANDOMLY PICKS AN AI PIECE TO MOVE
							RandomlyMoveARandomPiece();
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
			if (CanAITakePlayerPieceCleanly() == false)	// if AI can't sneak a Player piece cleanly...
			{
				// THIS IS THE POINT IN TIME WHEN THE PROGRAM RANDOMLY PICKS AN AI PIECE TO MOVE
				RandomlyMoveARandomPiece();
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

/************************************************************************/
/* Returns TRUE:
		An AI piece OF A LESSER VALUE THAN THE ONE THREATENED can block the threatened piece (takes care of the movement as well)
   Returns FALSE:
		When either an AI piece is not capable of blocking, or if there is a piece able to block, but that piece is of higher value*/
/************************************************************************/
bool Game::CanAIPieceBlockThreat(int iPreyTile, int iPredatorTile, DynamicArray<int>* pLineOfFireTiles, EPieceType ePieceType)
{
	pGameboard->getLineOfFireTiles(iPreyTile, iPredatorTile, ePieceType, pGameboard->pGameboardArrayOfTiles[iPredatorTile].pPieceOnThisTile->ePieceType, pLineOfFireTiles);

	if (pLineOfFireTiles->iCount > 0)
	{
		for (int i = 0; i < pLineOfFireTiles->iCount; i++)
		{
			DynamicArray<int>* pPiecesAble = new DynamicArray<int>(pAI->lBoardIndexesOfPiecesInPlay->iCount);
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
bool::Game::CanAIKillPredator(int iPreyTile, int iPredatorTile, DynamicArray<int>* pPiecesThatCanKillPredator)
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

void Game::RandomlyMoveARandomPiece()
{
	DynamicArray<int>* pListOfZeroThreatTilesForAI = new DynamicArray<int>(64);
	pGameboard->getListOfUnoccupiedThreatFreeTilesForAI(pListOfZeroThreatTilesForAI);
	pGameboard->moveRandomAIPieceToUnthreatenedTile(pListOfZeroThreatTilesForAI);
}

/************************************************************************/
/* Returns TRUE if an AI piece can take one of the Player pieces cleanly
		Also... if it returns TRUE, this function also takes care of the movement of the killer piece and the prey piece
   Returns FALSE if there are no clean takes for the AI
   
   Also, to note, this function is GENERAL... it's not testing just one piece, it's TESTING ALL AI PIECES*/
/************************************************************************/
bool Game::CanAITakePlayerPieceCleanly()
{
	for (int i = 0; i < pAI->lBoardIndexesOfPiecesInPlay->iCount; i++)
	{
		int pieceIndex = pAI->lBoardIndexesOfPiecesInPlay->Arr[i];
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
bool Game::CanAITakePlayerPieceCleanly(int iPredatorTile)
{
	for (int i = 0; i < pAI->lBoardIndexesOfPiecesInPlay->iCount; i++)
	{
		int pieceIndex = pAI->lBoardIndexesOfPiecesInPlay->Arr[i];
		pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->ResetBestKillValues();
		if (pGameboard->CanAIPieceKill(pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile, pieceIndex) == true &&
			pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->pTilesOfPiecesICanKill->Contains(iPredatorTile) == true &&
			/*pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->iBestPieceICanKillTile == iPredatorTile &&*/
			pGameboard->pGameboardArrayOfTiles[pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->iBestPieceICanKillTile].iPlayerThreatLevel == 0)
		{
			iSelectedTile = pieceIndex;
			pGameboard->MoveAIPiece(iPredatorTile);
			pGameboard->pGameboardArrayOfTiles[pieceIndex].pPieceOnThisTile->ResetBestKillValues();
			return true;
		}
	}
	return false;
}

/************************************************************************/
/* Doesn't have to be a "clean take" ... HOWEVER, it must be an ADVANTAGEOUS take                                                                     */
/************************************************************************/
bool Game::CanAITakePlayerPieceMessy(int iPredatorPiece)
{
	for (int i = 0; i < pAI->lBoardIndexesOfPiecesInPlay->iCount; i++)
	{
		int pieceIndex = pAI->lBoardIndexesOfPiecesInPlay->Arr[i];
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
/* Returns the difference between the predator value and the prey value
   In other words, this is returning "How much stronger the predator piece is than the prey piece"
   
   If +: Predator is stronger than Prey
   If -: Prey is stronger than Predator */
/************************************************************************/
int Game::GetAdvantageRatioOfKill(int iAIPreyTile, int iPlayerPredatorTile)
{
	int preyValue = pGameboard->pGameboardArrayOfTiles[iAIPreyTile].pPieceOnThisTile->iPieceValue;
	int predatorValue = pGameboard->pGameboardArrayOfTiles[iPlayerPredatorTile].pPieceOnThisTile->iPieceValue;

	int ratio = predatorValue - preyValue;
	return ratio;
}
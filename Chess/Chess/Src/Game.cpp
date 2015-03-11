#include "stdafx.h"

extern EvoFont cFont;
extern EvoGUI gGUI;

HumanPlayer			* pPlayer;
AIPlayer			* pAI;

Gameboard			* pGameboard;
Gameboard			* pCopyOfGameboard;

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
	pPlayer = new HumanPlayer();
	pAI = new AIPlayer();
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
				pAI->SimulateTurn();
				//SimulateAIsTurn();
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
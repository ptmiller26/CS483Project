
#pragma once

void PromptStart();

class Game
{
private:
	bool				bLeftClickDown;
	bool				bLeftClickUp;
	int					iSelectedPiece;
	EvoGUIRectWidget *	pChessBoardWidget;
protected:
public:
	Game();
	~Game();

	void Initialize();

	void Tick(float fSecondsPassed);

	void KeyPressed(char key, long lParam);

	void DisplayNumPiecesLeft();

	void DisplayBoard();

	void DrawRows();

	void DrawColumns();

	void DisplayDirections();

	void AddChessBoardWidget();

	void RemoveWidget();

	void DisplayWinner();

	void DisplayWhoseTurn();

	void SimulateAIsTurn();

	void RandomlyMoveARandomPiece();

	bool CanAITakePlayerPieceCleanly();						// checks all pieces
	bool CanAITakePlayerPieceCleanly(int iPredatorPiece);	// checks only this specific predator piece
	bool CanAITakePlayerPieceMessy(int iPredatorPiece);		// does not have to be a clean take

	//bool CanAITakeThisSpecificPlayerPieceCleanly(int iPredatorTile);

	void AIKingIsInCheck();

	int GetAdvantageRatioOfKill(int iAIPreyTile, int iPlayerPredatorTile);

	bool CanAIPieceBlockThreat(int iPreyTile, int iPredatorTile, DynamicArray<int>* pLineOfFireTiles, EPieceType ePieceType);

	bool CanAIKillPredator(int iPreyTile, int iPredatorTile, DynamicArray<int>* pPiecesThatCanKillPredator);
};

#pragma once

class Gameboard
{
private:
protected:
public:
	
	bool	bPlayerWins;
	bool	bAIWins;

	int		iSelectedTileRow;
	int		iSelectedTileCol;
	int		*PointerToMovesArray;
	int		PossibleMovesArray[64];

	Tile	*pGameboardArrayOfTiles;



	Gameboard();
	Gameboard(const Gameboard& gameboard);
	~Gameboard();

	void InitializeGameBoard();
	void SetPiecesUpOnTheBoard(HumanPlayer* player1, AIPlayer* AI);
	void DisplayTheTilesOnTheScreen();	// MAIN FUNCTION THAT INDIVIDUALLY DISPLAYS EACH PIECE
	void DisplayPossibleTilesToMoveTo();
	void CalculateSelectedTile(long lParam);
	void MovePlayerPiece(long lParam);	// make a player be one of the parameters so we can access their array of pieces
	void ToggleOffMovementTiles();
	void SetBorderTileFlags();
	void MoveAIPiece(int iTileToMoveTo);
	void RemoveAllPieces();
	void SetAnimationVariables(Piece* piece, int startingTile, int endingTile);
	void Animation(float animateVariable);
	void SwapPieceFromOldToNewTile();
	void RecalculateAllTileThreatLevels();
	void getListOfUnoccupiedThreatFreeTilesForAI(DynamicArray<int>* pArr);
	void moveRandomAIPieceToUnthreatenedTile(DynamicArray<int>* pListOfUnthreatenedTiles);
	void getPiecesThatCanMoveToThisTile(int iTileIndex, DynamicArray<int>* pArrOfPieceIndexes, EPlayerOrAI ePlayerOrAI);
	void getLineOfFireTiles(int iPreyTile, int iPredatorTile, EPieceType ePreyPieceType, EPieceType ePredatorPieceType, DynamicArray<int>* pLineOfFireTiles);

	bool IsThisPieceAbleToMove(int selectedTileIndex); // THIS IS FOR THE AI SIMULATION
	bool MoveThreatenedAIPiece(int iIndexOfThreatenedPiece);
	bool WillThisMovePutAIInCheck(int EndTile);
	bool CanAIPieceKill(Piece *pPiece, int iCurrentTile);

	int CalculateSelectedRow(long lParam);
	int CalculateSelectedCol(long lParam);
	int getAIsMostProtectedTile();
	int getAIsMostProtectedUnoccupiedTile();
	int getPlayersMostProtectedTile();
	int getPlayersMostProtectedUnoccupiedTile();
	int getTileThatIsThreateningThisAIPiece(int iPreyTile);
	int getRowOfTile(int iTileIndex);

	DynamicArray<int>* getIndexesAIPieceCanMoveTo(Piece* selectedPiece);

	Tile* getGameboardInstance();
};
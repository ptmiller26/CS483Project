
#pragma once

class Gameboard
{
private:
protected:
public:
	
	bool bPlayerWins;
	bool bAIWins;

	Gameboard();
	~Gameboard();

	int iSelectedTileRow;
	int iSelectedTileCol;

	int *PointerToMovesArray;
	int PossibleMovesArray[64];

	void InitializeGameBoard();

	void SetPiecesUpOnTheBoard(Player* player1, Player* AI);

	void DisplayTheTilesOnTheScreen();	// MAIN FUNCTION THAT INDIVIDUALLY DISPLAYS EACH PIECE

	void DisplayPossibleTilesToMoveTo();

	void CalculateSelectedTile(long lParam);
	int CalculateSelectedRow(long lParam);
	int CalculateSelectedCol(long lParam);

	bool IsThisPieceAbleToMove(int selectedTileIndex); // THIS IS FOR THE AI SIMULATION

	void MovePlayerPiece(long lParam);	// make a player be one of the parameters so we can access their array of pieces

	void ToggleOffMovementTiles();

	void SetBorderTileFlags();

	void MoveAIPiece(int iTileToMoveTo);

	bool MoveThreatenedAIPiece(int iIndexOfThreatenedPiece);

	void RemoveAllPieces();

	void SetAnimationVariables(Piece* piece, int startingTile, int endingTile);
	void Animation(float animateVariable);

	DynamicArray<int>* getIndexesAIPieceCanMoveTo(Piece* selectedPiece);

	void SwapPieceFromOldToNewTile();

	Tile* getGameboardInstance();

	Tile *					pGameboardArrayOfTiles;

	//void AIPieceTakePlayerPiece();
	bool CanAIPieceKill(Piece *pPiece, int iCurrentTile);

	void RecalculateAllTileThreatLevels();

	int getAIsMostProtectedTile();
	int getAIsMostProtectedUnoccupiedTile();

	int getPlayersMostProtectedTile();
	int getPlayersMostProtectedUnoccupiedTile();

	void getListOfUnoccupiedThreatFreeTilesForAI(DynamicArray<int>* pArr);

	void moveRandomAIPieceToUnthreatenedTile(DynamicArray<int>* pListOfUnthreatenedTiles);

	void getPiecesThatCanMoveToThisTile(int iTileIndex, DynamicArray<int>* pArrOfPieceIndexes, EPlayerOrAI ePlayerOrAI);

	bool WillThisMovePutAIInCheck(int EndTile);

	//int getTileThatIsThreateningTheAIKing(int iAIKingTile);

	int getTileThatIsThreateningThisAIPiece(int iPreyTile);

	void getLineOfFireTiles(int iPreyTile, int iPredatorTile, EPieceType ePreyPieceType, EPieceType ePredatorPieceType, DynamicArray<int>* pLineOfFireTiles);

	int getRowOfTile(int iTileIndex);
};
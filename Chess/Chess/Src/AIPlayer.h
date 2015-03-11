
#pragma once

class Piece;

class AIPlayer: public Player
{
private:
protected:
public:
	AIPlayer();
	~AIPlayer();

	void RandomlyMoveRandomPiece();
	void DoThisWhenAIKingIsInCheck();
	void SimulateTurn();
	void DefensiveStrategy();
	void OffensiveStrategy();
	void GetPredatorTiles(DynamicArray<int>* pPredatorTiles, DynamicArray<int>* tilesOfThreatenedPieces);
	void TryToMoveAllThreatenedPieces(int iTileOfMVP, DynamicArray<int>* tilesOfThreatenedPieces);
	
	bool CanITakePlayerPieceCleanly();
	bool CanITakePlayerPieceCleanly(int iPredatorPiece);
	bool CanITakePlayerPieceMessy(int iPredatorPiece);
	bool CanPieceBlockThreat(int iPreyTile, int iPredatorTile, DynamicArray<int>* pLineOfFireTiles, EPieceType ePieceType);
	bool CanIKillPredator(int iPreyTile, int iPredatorTile, DynamicArray<int>* pPiecesThatCanKillPredator);
	bool CanWeKillThePredators(DynamicArray<int>* pPredators);
	bool CanOurMVPKill(int iTileOfMVP);

	int GetAdvantageRatioOfKill(int iAIPreyTile, int iPlayerPredatorTile);
	int GetThreatenedPieces(DynamicArray<int>* tilesOfThreatenedPieces);
	
};
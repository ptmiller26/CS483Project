
#pragma once

class Tile
{
private:
protected:
public:
	Tile();
	~Tile();

	bool					bSelectedTile;
	bool					bMovementTile;
	bool					bEdgeTile;
	bool					bMovingFromThisTile;	// denotes "DON'T COUNT THIS TILE" (ACT AS IF IT'S EMPTY)

	int						iLocationOnTable; // <--- this is the index of the game board array <-- it's going to help with the placement of the tile on the board
	int						iAIThreatLevel;			// how many AI pieces can move to this tile
	int						iPlayerThreatLevel;		// how many player pieces can move to this tile
	float					fXcoordinate;
	float					fYcoordinate;
	Piece *					pPieceOnThisTile;
	EvoGUIRectWidget *		pSelectedPieceWidget;
	EvoTexture *			pSelectedPieceTexture;

	Piece*					pWhatIfPiece;	// used for looking ahead

	void setXYCoordinates();
	
	void AddInitializedTileToGUI();

	void RemoveBorder();
	void RemovePiece();

	void AddSelectedTileToGUI();
	void AddMovementTileToGUI();

	void ToggleSelectedTileOn() {bSelectedTile = true; }
	void ToggleSelectedTileOff() {bSelectedTile = false; }

	void ToggleMovementTileOn() { bMovementTile = true; }
	void ToggleMovementTileOff() { bMovementTile = false; }
};
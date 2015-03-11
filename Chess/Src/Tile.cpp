#include "stdafx.h"
extern Gameboard * pGameboard;

extern EvoGUI gGUI;

Tile::Tile()
{
	pPieceOnThisTile = NULL;
	pWhatIfPiece = NULL;
	bMovementTile = false;						
	bSelectedTile = false;
	bEdgeTile = false;
	iAIThreatLevel = 0;
	iPlayerThreatLevel = 0;
	bMovingFromThisTile = false;
}

Tile::~Tile()
{

}

void Tile::setXYCoordinates()
{
	int temp = iLocationOnTable % 8;
	fXcoordinate = 227.5f + (temp * 106);

	int temp2 = iLocationOnTable / 8;
	fYcoordinate = 55 + (temp2 * 75.5f);
}

void Tile::AddInitializedTileToGUI()
{
	if (pPieceOnThisTile->ePieceType == ePawnType)
	{
		pPieceOnThisTile->pChessPieceWidget->SetAll(fXcoordinate, fYcoordinate, 85, 60, pPieceOnThisTile->pChessPieceTexture);
	}
	else if (pPieceOnThisTile->ePieceType == eRookType && pPieceOnThisTile->eTeamColor == eWhite)
	{
		pPieceOnThisTile->pChessPieceWidget->SetAll(fXcoordinate, fYcoordinate, 65, 60, pPieceOnThisTile->pChessPieceTexture);
	}
	else if (pPieceOnThisTile->ePieceType == eKingType && pPieceOnThisTile->eTeamColor == eWhite)
	{
		pPieceOnThisTile->pChessPieceWidget->SetAll(fXcoordinate, fYcoordinate, 55, 50, pPieceOnThisTile->pChessPieceTexture);
	}
	else if (pPieceOnThisTile->ePieceType == eKnightType && pPieceOnThisTile->eTeamColor == eBlack)
	{
		pPieceOnThisTile->pChessPieceWidget->SetAll(fXcoordinate, fYcoordinate, 60, 55, pPieceOnThisTile->pChessPieceTexture);
	}
	else
	{
		pPieceOnThisTile->pChessPieceWidget->SetAll(fXcoordinate, fYcoordinate, 90, 90, pPieceOnThisTile->pChessPieceTexture);
	}
	gGUI.AddWidget(pPieceOnThisTile->pChessPieceWidget);
}

void Tile::AddSelectedTileToGUI()
{
	pSelectedPieceWidget = new EvoGUIRectWidget();
	pSelectedPieceTexture = RESMGR->CreateTexture("Images\\selectedtile.png", NULL, false);
	pSelectedPieceWidget->SetAll(fXcoordinate - 30, fYcoordinate - 9, 110, 75, pSelectedPieceTexture);
	gGUI.AddWidget(pSelectedPieceWidget);
}

void Tile::AddMovementTileToGUI()
{
	pSelectedPieceWidget = new EvoGUIRectWidget();
	pSelectedPieceTexture = RESMGR->CreateTexture("Images\\selectedtilegreen.png", NULL, false);
	pSelectedPieceWidget->SetAll(fXcoordinate - 30, fYcoordinate - 9, 110, 75, pSelectedPieceTexture);
	gGUI.AddWidget(pSelectedPieceWidget);
}

void Tile::RemoveBorder()
{
	gGUI.RemoveWidget(pSelectedPieceWidget);
}

void Tile::RemovePiece()
{
	gGUI.RemoveWidget(pPieceOnThisTile->pChessPieceWidget);
}
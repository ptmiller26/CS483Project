#include "stdafx.h"

extern Gameboard	*pGameboard;

HumanPlayer::HumanPlayer()
{
	eMyTeamColor = eWhite;
	GivePlayerHisChessPieces();
}

HumanPlayer::~HumanPlayer()
{

}
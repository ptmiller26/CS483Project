
#pragma once

enum EWhichTeam
{
	eBlack,
	eWhite,
};

enum EWhichDirection
{
	N,
	S,
	E,
	W,
	NE,
	SE,
	NW,
	SW,
};

enum EGameState
{
	eStartOfProgram,
	eInGame,
	eEndOfGame,
};

enum EPiecePlacement
{
	eFirstRook,
	eFirstKnight,
	eFirstBishop,
	eQueen,
	eKing,
	eSecondBishop,
	eSecondKnight,
	eSecondRook,
};

enum EWhoseTurn
{
	ePlayersTurn,
	eAIsTurn,
};

enum EMovingState
{
	eChoosingPiece,
	eChoosingDestination,
	eAnimating,
};

enum EPieceType
{
	ePawnType,		// 0
	eRookType,		// 1
	eKnightType,	// 2
	eBishopType,	// 3
	eQueenType,		// 4
	eKingType,		// 5
};

enum ECheckState
{
	eNoCheck,
	eAIinCheck,
	ePlayerInCheck,
	eAIinCheckmate,
	ePlayerinCheckmate,
};

enum EPlayerOrAI
{
	ePlayer,
	eAI,
};

#include "stdafx.h"

Card::Card(ECardSuit eSuit, ECardNumber eNumber)
{

	eCardSuit = eSuit;
	eCardNumber = eNumber;

	// Sorts the name of the card
	switch (eNumber)
	{
	case eAce:
		fChenValue = 10;
		iCardOrder = 1;				// I made Ace have a position in the 1 slot
		iCardStrength = 13;			// iCardStrength is on a scale of 1-13, 2 being the weakest and Ace being the strongest
		break;
	case e2:
		fChenValue = 1;
		iCardOrder = 2;
		iCardStrength = 1;
		break;
	case e3:
		fChenValue = 1.5;
		iCardOrder = 3;
		iCardStrength = 2;
		break;
	case e4:
		fChenValue = 2;
		iCardOrder = 4;
		iCardStrength = 3;
		break;
	case e5:
		fChenValue = 2.5;
		iCardOrder = 5;
		iCardStrength = 4;
		break;
	case e6:
		fChenValue = 3;
		iCardOrder = 6;
		iCardStrength = 5;
		break;
	case e7:
		fChenValue = 3.5;
		iCardOrder = 7;
		iCardStrength = 6;
		break;
	case e8:
		fChenValue = 4;
		iCardOrder = 8;
		iCardStrength = 7;
		break;
	case e9:
		fChenValue = 4.5;
		iCardOrder = 9;
		iCardStrength = 8;
		break;
	case e10:
		fChenValue = 5;
		iCardOrder = 10;
		iCardStrength = 9;
		break;
	case eJack:
		fChenValue = 6;
		iCardOrder = 11;
		iCardStrength = 10;
		break;
	case eQueen:
		fChenValue = 7;
		iCardOrder = 12;
		iCardStrength = 11;
		break;
	case eKing:
		fChenValue = 8;
		iCardOrder = 13;
		iCardStrength = 12;
		break;
	}
}

Card::~Card()
{

}


// Returns the full name of the card (i.e. "4 of Spades" or "King of Hearts"
string Card::getFullCardName()
{
	sFullCardName = getCardName() + " of " + getCardSuit();
	return sFullCardName;
}

string Card::getCardSuit()
{
	switch (eCardSuit)
	{
	case eHearts:
		return "Hearts";
		break;
	case eSpades:
		return "Spades";
		break;
	case eClubs:
		return "Clubs";
		break;
	case eDiamonds:
		return "Diamonds";
		break;
	}
}

string Card::getCardName()
{
	switch (eCardNumber)
	{
	case eAce:
		return "Ace";
		break;
	case e2:
		return "2";
		break;
	case e3:
		return "3";
		break;
	case e4:
		return "4";
		break;
	case e5:
		return "5";
		break;
	case e6:
		return "6";
		break;
	case e7:
		return "7";
		break;
	case e8:
		return "8";
		break;
	case e9:
		return "9";
		break;
	case e10:
		return "10";
		break;
	case eJack:
		return "Jack";
		break;
	case eQueen:
		return "Queen";
		break;
	case eKing:
		return "King";
		break;
	}
}
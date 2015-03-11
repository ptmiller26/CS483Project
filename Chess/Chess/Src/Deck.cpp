#include "stdafx.h"

Deck::Deck()
{
	pDeck = new DynamicArray<Card*>(52);
}

Deck::~Deck()
{

}

void Deck::FillDeck()
{
	Card* pCard;
	ECardSuit eSuit = eHearts;
	ECardNumber eNumber = eAce;

	for (int eSuitInt = eHearts; eSuitInt != eLastSuit; eSuitInt++)
	{
		ECardSuit eSuit = static_cast<ECardSuit>(eSuitInt);
		for (int eNumberInt = eAce; eNumberInt != eLastCard; eNumberInt++)
		{
			ECardNumber eNumber = static_cast<ECardNumber>(eNumberInt);
			pCard = new Card(eSuit, eNumber);
			pDeck->AddItem(pCard);
		}
	}
}
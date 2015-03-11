
#pragma once

class Card;

class Deck
{
private:
protected:
public:
	Deck();
	~Deck();
	void FillDeck();
	DynamicArray<Card*>* pDeck;
	DynamicArray<Card*>* pBurnPile;
};
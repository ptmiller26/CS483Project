
#pragma once

class Deck;

class Card
{
private:
protected:
public:
	string sFullCardName;
	ECardSuit eCardSuit;
	ECardNumber eCardNumber;
	float fChenValue;
	int iCardOrder;
	int iCardStrength;
	Card() {};
	Card(ECardSuit eSuit, ECardNumber eNumber);
	~Card();
	string getFullCardName();
	string getCardSuit();
	string getCardName();
};

#pragma once

void PromptStart();

class Game
{
private:
	bool				bLeftClickDown;
	bool				bLeftClickUp;
	int					iSelectedPiece;
	EvoGUIRectWidget *	pChessBoardWidget;
protected:
public:
	Game();
	~Game();

	void Initialize();
	void Tick(float fSecondsPassed);
	void KeyPressed(char key, long lParam);
	void DisplayNumPiecesLeft();
	void DisplayBoard();
	void DisplayDirections();
	void AddChessBoardWidget();
	void RemoveWidget();
	void DisplayWinner();
	void DisplayWhoseTurn();
};
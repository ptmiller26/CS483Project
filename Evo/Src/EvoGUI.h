#ifndef _EVOGUI_H_
#define _EVOGUI_H_

// Interface management class

class EvoGUIWidget;

class EvoGUI
{
   public:
	EvoGUI();
	~EvoGUI();

	void AddWidget(EvoGUIWidget *pWidget);
	void RemoveWidget(EvoGUIWidget *pWidget);

	void Render();

   protected:
	std::vector<EvoGUIWidget *> m_aWidgets;
};

#endif
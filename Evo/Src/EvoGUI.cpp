#include "EvoStdAfx.h"

EvoGUI::EvoGUI()
{

}

EvoGUI::~EvoGUI()
{

}

void EvoGUI::AddWidget(EvoGUIWidget *pWidget)
{
	m_aWidgets.push_back(pWidget);
}

void EvoGUI::RemoveWidget(EvoGUIWidget *pWidget)
{
	for (uint ui = 0; ui < m_aWidgets.size(); ui++)
	{
		if (m_aWidgets[ui] == pWidget)
		{
			m_aWidgets.erase(m_aWidgets.begin() + ui);
			return;
		}
	}
}

void EvoGUI::Render()
{
	for (uint ui = 0; ui < m_aWidgets.size(); ui++)
	{
		m_aWidgets[ui]->Render();
	}
}
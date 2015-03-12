#include "EvoStdAfx.h"

EvoGUITextWidget::EvoGUITextWidget()
{
	m_pFont		= NULL;
	m_uiFlags	= 0;
	m_fX		= 0;
	m_fY		= 0;
	m_fScale	= 1.0f;
}

EvoGUITextWidget::~EvoGUITextWidget()
{

}

void EvoGUITextWidget::Render()
{
	assert(m_pFont);

	if(!IsEnabled())
		return;

	float fOldScale = m_pFont->GetScale();
	m_pFont->SetScale(m_fScale);

	m_pFont->DrawText(m_fX, m_fY, m_String.GetBuffer(), m_uiFlags);

	m_pFont->SetScale(fOldScale);
}
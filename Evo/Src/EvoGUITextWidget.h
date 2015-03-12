#ifndef _EVOGUITEXTWIDGET_H_
#define _EVOGUITEXTWIDGET_H_

class EvoGUITextWidget : public EvoGUIWidget
{
   public:
	EvoGUITextWidget();
	~EvoGUITextWidget();

	void SetPosition(float fX, float fY);
	
	// bCompact should only be set if you want the string to be reallocated if the new string is smaller than the current string.
	void SetText(char *szText, bool bCompact);

	// For now these are the EvoFont flags to center, etc.
	void SetFlags(uint uiFlags);	
	
	void SetFont(EvoFont *pFont);

	void SetScale(float fScale);

	void Render();

   protected:
	EvoFont *	m_pFont;
	EvoString	m_String;
	uint		m_uiFlags;
	float		m_fX;
	float		m_fY;
	float		m_fScale;
};

// Inlines

inline void EvoGUITextWidget::SetPosition(float fX, float fY)
{
	m_fX = fX;
	m_fY = fY;
}

inline void EvoGUITextWidget::SetText(char *szText, bool bCompact)
{
	m_String.Copy(szText, bCompact);
}

inline void EvoGUITextWidget::SetFlags(uint uiFlags)
{
	m_uiFlags = uiFlags;
}

inline void EvoGUITextWidget::SetFont(EvoFont *pFont)
{
	m_pFont = pFont;
}

inline void EvoGUITextWidget::SetScale(float fScale)
{
	m_fScale = fScale;
}

#endif
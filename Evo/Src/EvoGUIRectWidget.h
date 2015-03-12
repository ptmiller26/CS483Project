#ifndef _EVOGUIRECTWIDGET_H_
#define _EVOGUIRECTWIDGET_H_

class EvoGUIRectWidget : public EvoGUIWidget
{
   public:
	EvoGUIRectWidget();
	virtual ~EvoGUIRectWidget();
	
	void SetPosition(float fX, float fY);

	void SetWidthAndHeight(float fWidth, float fHeight);

	void SetTexture(EvoTexture *pTex);

	void SetAll(float fX, float fY, float fWidth, float fHeight, EvoTexture *pTex);

	// Pure virtual Render function
	virtual void Render();

   protected:
	void RebuildGeometry();

   protected:
	bool			m_bDirty;

	float			m_fX;
	float			m_fY;
	float			m_fWidth;
	float			m_fHeight;
	EvoColor		m_Color;
	EvoTexture *	m_pTexture;
	EvoBuffer *		m_pVB;
	EvoBuffer *		m_pIB;

	EvoShader *				m_pShader;
	EvoInputLayoutHandle	m_hInputLayout;
};

inline void EvoGUIRectWidget::SetPosition(float fX, float fY)
{
	m_fX = fX;
	m_fY = fY;
	m_bDirty = true;
}

inline void EvoGUIRectWidget::SetWidthAndHeight(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_bDirty = true;
}

inline void EvoGUIRectWidget::SetTexture(EvoTexture *pTex)
{
	m_pTexture = pTex;
}

inline void EvoGUIRectWidget::SetAll(float fX, float fY, float fWidth, float fHeight, EvoTexture *pTex)
{
	m_fX		= fX;
	m_fY		= fY;
	m_fWidth	= fWidth;
	m_fHeight	= fHeight;
	m_pTexture	= pTex;

	m_bDirty = true;
}


#endif
#include "EvoStdAfx.h"

EvoRenderTarget::EvoRenderTarget()
{
	m_pRenderTexture		= NULL;
	m_pDepthStencilTexture	= NULL;
#if defined(EVO_RENDERER_DX9)
	m_pOldDepthStencil		= NULL;
	m_pOldRenderTarget		= NULL;
#endif
}

EvoRenderTarget::~EvoRenderTarget()
{

}

void EvoRenderTarget::SetTextures(EvoTexture *pRender, EvoTexture *pDepthStencil)
{
	m_pRenderTexture			= pRender;
	m_pDepthStencilTexture		= pDepthStencil;
}
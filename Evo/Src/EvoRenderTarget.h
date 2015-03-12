#ifndef _EVORENDERTARGET_H_
#define _EVORENDERTARGET_H_

class EvoRenderTarget
{
   public:
	EvoRenderTarget();
	~EvoRenderTarget();

	void SetTextures(EvoTexture *pRender, EvoTexture *pDepthStencil);

	EvoTexture *GetRenderTexture()			{ return m_pRenderTexture;			}
	EvoTexture *GetDepthStencilTexture()	{ return m_pDepthStencilTexture;	}

   public:
    EvoTexture *m_pRenderTexture;
	EvoTexture *m_pDepthStencilTexture;

#if defined(EVO_RENDERER_DX9)
	IDirect3DSurface9 *m_pOldRenderTarget;
	IDirect3DSurface9 *m_pOldDepthStencil;

	friend class EvoDX9Renderer;
#endif
};


#endif
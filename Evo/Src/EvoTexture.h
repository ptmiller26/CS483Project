#ifndef _EVOTEXTURE_H_
#define _EVOTEXTURE_H_

class EvoTexture
{
   public:
#if defined(EVO_RENDERER_DX9)
	void SetTexture(IDirect3DBaseTexture9 *pTex)	{ m_pTexture = pTex; } 
	IDirect3DBaseTexture9 *GetTexture()				{ return m_pTexture; }
	// Only used for textures created as depth/stencil, as ATI can't use textures as depth/stencils
	// so we'll just use surfaces for all depth/stencils
	IDirect3DSurface9 *GetSurface()					{ return m_pSurface; }
#endif

#if defined(EVO_RENDERER_DX10)
	void SetTexture(ID3D10Texture2D *pTex)		{ m_pTexture = pTex;	} 
	ID3D10Texture2D *GetTexture()				{ return m_pTexture;	}
	ID3D10ShaderResourceView *GetRV()			{ return m_pRV;			}
	ID3D10ShaderResourceView **GetRVPtrPtr()	{ return &m_pRV;		}
#endif

	// Set texture name
	void SetName(const char *pszName);
	const char *GetName()						{ return m_pszName;	 }

	void *Lock(EvoLock::ELock eLock, bool bWait, uint *puiRowPitch);
	void Unlock();

	void GetInfo(uint *puiWidth, uint *puiHeight);

   protected:
	EvoTexture();
	~EvoTexture();

   protected:
    char *					m_pszName;

#if defined(EVO_RENDERER_DX9)
	IDirect3DBaseTexture9 *	m_pTexture;
	IDirect3DSurface9 *		m_pSurface;
#endif

#if defined(EVO_RENDERER_DX10)
	ID3D10Texture2D	*			m_pTexture;
	ID3D10ShaderResourceView *	m_pRV;
#endif

	friend class EvoResourceManager;
};


#endif
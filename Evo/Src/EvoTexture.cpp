#include "EvoStdAfx.h"

EvoTexture::EvoTexture()
{
	m_pTexture	= NULL;
#if defined(EVO_RENDERER_DX9)
	m_pSurface	= NULL;
#elif defined(EVO_RENDERER_DX10)
	m_pRV		= NULL;
#endif
	m_pszName	= NULL;
}

EvoTexture::~EvoTexture()
{
	SAFE_DELETE(m_pszName);
}

void EvoTexture::SetName(const char *pszName)
{
	SAFE_DELETE(m_pszName);

	uint uiLen = strlen(pszName)+1;
	m_pszName = new char[uiLen];
	strcpy(m_pszName, pszName);
}

void *EvoTexture::Lock(EvoLock::ELock eLock, bool bWait, uint *puiRowPitch)
{
#if defined(EVO_RENDERER_DX10)
	uint uiLockFlags = 0;
	D3D10_MAPPED_TEXTURE2D MappedTex;

	if (!bWait)
		uiLockFlags |= D3D10_MAP_FLAG_DO_NOT_WAIT;

	m_pTexture->Map(0, (D3D10_MAP)eLock, uiLockFlags, &MappedTex);

	*puiRowPitch = MappedTex.RowPitch;

	return MappedTex.pData;
#endif

#if defined(EVO_RENDERER_DX9)
	uint uiFlags = 0;

	/*if (eLock == EvoLock::DISCARD)
		uiFlags = D3DLOCK_DISCARD;
	else*/ if (eLock == EvoLock::READ)
		uiFlags = D3DLOCK_READONLY;
	else if (eLock == EvoLock::READ)
		uiFlags = D3DLOCK_READONLY;

	if (!bWait)
		uiFlags |= D3DLOCK_DONOTWAIT;


	IDirect3DTexture9 *pTex;
	m_pTexture->QueryInterface(IID_IDirect3DBaseTexture9, (void**)&pTex);
	

	D3DLOCKED_RECT LockedRect;
	pTex->LockRect(0, &LockedRect, NULL, uiFlags);
	*puiRowPitch = LockedRect.Pitch;
	
	pTex->Release();

	return LockedRect.pBits;
#endif 

}

void EvoTexture::Unlock()
{
#if defined(EVO_RENDERER_DX9)
	IDirect3DTexture9 *pTex;
	m_pTexture->QueryInterface(IID_IDirect3DBaseTexture9, (void**)&pTex);

	pTex->UnlockRect(0);

	pTex->Release();
#endif

#if defined(EVO_RENDERER_DX10)
	m_pTexture->Unmap(0);
#endif
}


void EvoTexture::GetInfo(uint *puiWidth, uint *puiHeight)
{
#if defined(EVO_RENDERER_DX10)
	D3D10_TEXTURE2D_DESC Desc;
	m_pTexture->GetDesc(&Desc);

	*puiWidth = Desc.Width;
	*puiHeight = Desc.Height;
#endif

#if defined(EVO_RENDERER_DX9)
	IDirect3DTexture9 *pTex;
	m_pTexture->QueryInterface(IID_IDirect3DBaseTexture9, (void**)&pTex);

	D3DSURFACE_DESC Desc;
	pTex->GetLevelDesc(0, &Desc);

	*puiWidth = Desc.Width;
	*puiHeight = Desc.Height;

	pTex->Release();
#endif
}
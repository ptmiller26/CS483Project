#include "EvoStdAfx.h"

#if defined(EVO_RENDERER_DX9)

EvoDX9BufferManager::EvoDX9BufferManager()
{

}

EvoDX9BufferManager::~EvoDX9BufferManager()
{

}

EvoBuffer *EvoDX9BufferManager::CreateBuffer(uint uiBytes, uint uiStride, void *pData, EvoBind::EBind eBind, EvoUsage::EUsage eUsage, EvoCpuAccess::ECpuAccess eCPUAccess, bool bGenMips, bool bCubeTexture)
{
	EvoBuffer *pBuffer = new EvoBuffer();

	uint uiUsage	= 0;
	EvoLock::ELock eLock = EvoLock::WRITE;

	D3DPOOL D3DPool = D3DPOOL_MANAGED;

	if (eCPUAccess == EvoCpuAccess::WRITE)
		uiUsage |= D3DUSAGE_WRITEONLY;

	if (eUsage == EvoUsage::DYNAMIC)
	{
		uiUsage |= D3DUSAGE_DYNAMIC;
		D3DPool = D3DPOOL_DEFAULT;	// Can't create dynamic managed surfaces.
		eLock = EvoLock::DISCARD;
	}

	if (eBind == EvoBind::INDEX_BUFFER)
	{
		IDirect3DIndexBuffer9 *pIB = NULL;
		D3DFORMAT D3DFormat;
		if (uiStride == 4)
			D3DFormat = D3DFMT_INDEX32;
		else if (uiStride == 2)
			D3DFormat = D3DFMT_INDEX16;
		else assert(0);

		EVOD3DDEVICE->CreateIndexBuffer(uiBytes, uiUsage, D3DFormat, D3DPool, &pIB, NULL);

		pIB->QueryInterface(IID_IDirect3DResource9, (void **)&pBuffer->m_pBuffer);

		pIB->Release();
	}
	else if (eBind == EvoBind::VERTEX_BUFFER)
	{
		IDirect3DVertexBuffer9 *pVB = NULL;

		EVOD3DDEVICE->CreateVertexBuffer(uiBytes, uiUsage, 0, D3DPool, &pVB, NULL);

		pVB->QueryInterface(IID_IDirect3DResource9, (void **)&pBuffer->m_pBuffer);

		pVB->Release();
	}

	pBuffer->m_uiStride = uiStride;

	if (pData)
	{
		void *pLockedData = pBuffer->Lock(eLock, true);
		memcpy(pLockedData, pData, uiBytes);
		pBuffer->Unlock();
	}

	return pBuffer;
}

EvoBuffer *EvoDX9BufferManager::CreateStagingBuffer(EvoBuffer *pBuffer)
{
	return NULL;
}

void EvoDX9BufferManager::DestroyBuffer(EvoBuffer *pBuffer)
{
	delete pBuffer;
}

#endif
#include "EvoStdAfx.h"

EvoBuffer::EvoBuffer()
{
#if defined(EVO_RENDERER_DX10)
	m_pBuffer = NULL;
	m_pBufferCopy = NULL;
#endif
}

EvoBuffer::~EvoBuffer()
{
#if defined(EVO_RENDERER_DX10)
	if (m_pBufferCopy)
	{
		EVO->GetRenderer()->GetBufferMgr()->DestroyBuffer(m_pBufferCopy);
		m_pBufferCopy = NULL;
	}

	SAFE_RELEASE(m_pBuffer);
#endif
}

void *EvoBuffer::Lock(EvoLock::ELock eLock, bool bWait)
{
#if defined(EVO_RENDERER_DX10)
	uint uiLockFlags = 0;
	void *pData = NULL;
	
	if (!bWait)
		uiLockFlags |= D3D10_MAP_FLAG_DO_NOT_WAIT;

	if (m_bLockable)
	{
		m_pBuffer->Map((D3D10_MAP)eLock, uiLockFlags, &pData);
	}
	else
	{
		m_pBufferCopy = EVO->GetRenderer()->GetBufferMgr()->CreateStagingBuffer(this);
		m_pBufferCopy->GetBuffer()->Map((D3D10_MAP)eLock, uiLockFlags, &pData);
	}

	return pData;
#endif

#if defined(EVO_RENDERER_DX9)
	BYTE *pbData = NULL;
	uint uiLockFlags = 0;

	if (eLock == EvoLock::DISCARD)
		uiLockFlags = D3DLOCK_DISCARD;
	else if (eLock == EvoLock::READ)
		uiLockFlags = D3DLOCK_READONLY;

	if (!bWait)
		uiLockFlags |= D3DLOCK_DONOTWAIT;

	if (m_pBuffer->GetType() == D3DRTYPE_INDEXBUFFER)
	{
		IDirect3DIndexBuffer9 *pIB = NULL;
		m_pBuffer->QueryInterface(IID_IDirect3DIndexBuffer9, (void**)&pIB);

		pIB->Lock(0, 0, (void**)&pbData, uiLockFlags);

		pIB->Release();
	}
	else if (m_pBuffer->GetType() == D3DRTYPE_VERTEXBUFFER)
	{
		IDirect3DVertexBuffer9 *pVB = NULL;
		m_pBuffer->QueryInterface(IID_IDirect3DVertexBuffer9, (void**)&pVB);

		pVB->Lock(0, 0, (void**)&pbData, uiLockFlags);

		pVB->Release();
	}

	return pbData;
#endif

}

void EvoBuffer::Unlock()
{
#if defined(EVO_RENDERER_DX10)
	if (m_pBufferCopy)
	{
		m_pBufferCopy->GetBuffer()->Unmap();
		EVO->GetRenderer()->GetBufferMgr()->DestroyBuffer(m_pBufferCopy);
		m_pBufferCopy = NULL;
	}
	else
	{
		m_pBuffer->Unmap();
	}
#endif

#if defined(EVO_RENDERER_DX9)
	if (m_pBuffer->GetType() == D3DRTYPE_INDEXBUFFER)
	{
		IDirect3DIndexBuffer9 *pIB = NULL;
		m_pBuffer->QueryInterface(IID_IDirect3DIndexBuffer9, (void**)&pIB);

		pIB->Unlock();

		pIB->Release();
	}
	else if (m_pBuffer->GetType() == D3DRTYPE_VERTEXBUFFER)
	{
		IDirect3DVertexBuffer9 *pVB = NULL;
		m_pBuffer->QueryInterface(IID_IDirect3DVertexBuffer9, (void**)&pVB);

		pVB->Unlock();

		pVB->Release();
	}
#endif
}


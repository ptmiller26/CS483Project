#include "EvoStdAfx.h"

#if defined(EVO_RENDERER_DX10)

EvoDX10BufferManager::EvoDX10BufferManager()
{

}

EvoDX10BufferManager::~EvoDX10BufferManager()
{

}

EvoBuffer *EvoDX10BufferManager::CreateBuffer(uint uiBytes, uint uiPitch, void *pData, EvoBind::EBind eBind, EvoUsage::EUsage eUsage, EvoCpuAccess::ECpuAccess eCPUAccess, bool bGenMips, bool bCubeTexture)
{
	EvoBuffer *pBuffer = new EvoBuffer();

	D3D10_BUFFER_DESC Desc;
	Desc.BindFlags = eBind;
	Desc.ByteWidth = uiBytes;
	Desc.CPUAccessFlags = eCPUAccess;
	Desc.Usage = (D3D10_USAGE)eUsage;
	Desc.MiscFlags = 0;

	if (bGenMips)
		Desc.MiscFlags |= D3D10_RESOURCE_MISC_GENERATE_MIPS;

	if (bCubeTexture)
		Desc.MiscFlags |= D3D10_RESOURCE_MISC_TEXTURECUBE;

	D3D10_SUBRESOURCE_DATA InitialData;
	InitialData.SysMemPitch = uiPitch;
	InitialData.pSysMem = pData;
	InitialData.SysMemSlicePitch = 0;

	EVOD3DDEVICE->CreateBuffer(&Desc, &InitialData, &pBuffer->m_pBuffer);

	if (eCPUAccess == EvoCpuAccess::NONE)
		pBuffer->m_bLockable = false;
	else pBuffer->m_bLockable = true;

	pBuffer->m_uiStride = uiPitch;

	return pBuffer;
}

EvoBuffer *EvoDX10BufferManager::CreateStagingBuffer(EvoBuffer *pBuffer)
{
	EvoBuffer *pBufferCopy = new EvoBuffer;

	D3D10_BUFFER_DESC Desc;
	pBuffer->GetBuffer()->GetDesc(&Desc);

	Desc.Usage = (D3D10_USAGE)EvoUsage::STAGING;
	Desc.CPUAccessFlags = EvoCpuAccess::READ | EvoCpuAccess::WRITE;
	Desc.BindFlags = 0;

	HRESULT hr = EVOD3DDEVICE->CreateBuffer(&Desc, NULL, &pBufferCopy->m_pBuffer);

	EVOD3DDEVICE->CopyResource(pBufferCopy->m_pBuffer, pBuffer->m_pBuffer);

	pBufferCopy->m_bLockable = true;
	pBufferCopy->m_uiStride = pBuffer->m_uiStride;

	return pBufferCopy;
}

void EvoDX10BufferManager::DestroyBuffer(EvoBuffer *pBuffer)
{
	delete pBuffer;
}

#endif
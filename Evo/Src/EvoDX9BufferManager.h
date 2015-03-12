#ifndef _EVODX9BUFFERMANAGER_H_
#define _EVODX9BUFFERMANAGER_H_

class EvoBuffer;

#if defined(EVO_RENDERER_DX9)

class EvoDX9BufferManager
{
   public:
	EvoDX9BufferManager();
	~EvoDX9BufferManager();

	EvoBuffer *CreateBuffer(uint uiBytes, uint uiPitch, void *pData, EvoBind::EBind eBind, EvoUsage::EUsage eUsage, EvoCpuAccess::ECpuAccess eCPUAccess, bool bGenMips, bool bCubeTexture);
	void DestroyBuffer(EvoBuffer *pBuffer);

	// Create a readable staging buffer from pBuffer
	EvoBuffer *CreateStagingBuffer(EvoBuffer *pBuffer);

   protected:

	friend class EvoBuffer;
};

#endif


#endif
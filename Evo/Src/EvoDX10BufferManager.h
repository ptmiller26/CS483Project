#ifndef _EVODX10BUFFERMANAGER_H_
#define _EVODX10BUFFERMANAGER_H_

class EvoBuffer;

#if defined(EVO_RENDERER_DX10)

class EvoDX10BufferManager
{
   public:
	EvoDX10BufferManager();
	~EvoDX10BufferManager();

	EvoBuffer *CreateBuffer(uint uiBytes, uint uiPitch, void *pData, EvoBind::EBind eBind, EvoUsage::EUsage eUsage, EvoCpuAccess::ECpuAccess eCPUAccess, bool bGenMips, bool bCubeTexture);
	void DestroyBuffer(EvoBuffer *pBuffer);

	// Create a readable staging buffer from pBuffer
	EvoBuffer *CreateStagingBuffer(EvoBuffer *pBuffer);

   protected:

	friend class EvoBuffer;
};

#endif


#endif
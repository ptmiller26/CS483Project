#ifndef _EVOBUFFER_H_
#define _EVOBUFFER_H_

class EvoBuffer
{
   protected:
	EvoBuffer();
	~EvoBuffer();

   public:

	void *Lock(EvoLock::ELock eLock, bool bWait);
	void Unlock();

	uint GetStride();

#if defined(EVO_RENDERER_DX10)
	ID3D10Buffer *GetBuffer()			{ return m_pBuffer; }
	ID3D10Buffer **GetBufferPtrPtr()	{ return &m_pBuffer; }
#endif

#if defined(EVO_RENDERER_DX9)
	IDirect3DResource9 *GetBuffer()			{ return m_pBuffer; }
	IDirect3DVertexBuffer9 *GetBufferAsVB();
	IDirect3DIndexBuffer9 *GetBufferAsIB();
#endif

   protected:
#if defined(EVO_RENDERER_DX10)
	bool m_bLockable;

	ID3D10Buffer *m_pBuffer;

	// Only gets used if can't lock this buffer (which will be most)
	// Gets created upon Lock() and destroyed upon Unlock()
	EvoBuffer *m_pBufferCopy;
	friend class EvoDX10BufferManager;
#endif

#if defined(EVO_RENDERER_DX9)
	IDirect3DResource9 *m_pBuffer;
#endif

	uint m_uiStride;

	friend class EvoDX9BufferManager;
};


// Inlines

inline uint EvoBuffer::GetStride()
{
	return m_uiStride;
}

#if defined(EVO_RENDERER_DX9)
inline IDirect3DVertexBuffer9 *EvoBuffer::GetBufferAsVB()
{
	IDirect3DVertexBuffer9 *pVB = NULL;
	m_pBuffer->QueryInterface(IID_IDirect3DVertexBuffer9, (void**)&pVB);

	return pVB;
}

inline IDirect3DIndexBuffer9 *EvoBuffer::GetBufferAsIB()
{
	IDirect3DIndexBuffer9 *pIB = NULL;
	m_pBuffer->QueryInterface(IID_IDirect3DIndexBuffer9, (void**)&pIB);

	return pIB;
}
#endif

#endif
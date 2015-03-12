#ifndef _EVODX10RENDERER_H_
#define _EVODX10RENDERER_H_

class EvoDepthStencilState;
class EvoBlendState;

#if defined (EVO_RENDERER_DX10)

class EvoDX10Renderer
{
   public:
	EvoDX10Renderer();
	~EvoDX10Renderer();

	bool Initialize(HWND hWindow, const EvoInitData& InitData);

	void Begin();
	
	void End();

	void Present();

	void Clear();

	void SetVB(EvoBuffer *pBuffer, uint uiSlot, uint uiStride, uint uiOffset);
	void SetVBs(EvoBuffer **ppBuffer, uint uiSlot, uint uiCnt, uint *puiStrides, uint *puiOffsets);

	// uiStride is 2 or 4
	void SetIB(EvoBuffer *pBuffer, uint uiStride, uint uiOffset);

	void BindToPS(uint uiSlot, EvoTexture *pTexture);

	void SetPrimitiveType(EvoPrimitive::EEvoPrimitive ePrimitive);

	void DrawIndexed(uint uiVertCnt, uint uiIndexCnt, uint uiStartIndex, int iOffset);

	ID3D10Device *	GetDevice();

	EvoDX10BufferManager *GetBufferMgr();

   protected:
	IDXGISwapChain *			m_pSwapChain;
	ID3D10Device *				m_pDevice;

	ID3D10RenderTargetView *	m_pDefaultRTView;
	ID3D10Texture2D *           m_pDefaultDepthStencil;
	ID3D10DepthStencilView *    m_pDefaultDSView;


	EvoDepthStencilState *		m_pDefaultDepthStencilState;
	EvoBlendState *				m_pDefaultBlendState;

	EvoDX10BufferManager		m_BufferMgr;
};


// Inlines
inline void EvoDX10Renderer::Begin()
{

}
	
inline void EvoDX10Renderer::End()
{

}

inline ID3D10Device *EvoDX10Renderer::GetDevice()
{
	return m_pDevice;
}

inline EvoDX10BufferManager *EvoDX10Renderer::GetBufferMgr()
{
	return &m_BufferMgr;
}

inline void EvoDX10Renderer::SetVB(EvoBuffer *pBuffer, uint uiSlot, uint uiStride, uint uiOffset)
{
	m_pDevice->IASetVertexBuffers(uiSlot, 1, pBuffer->GetBufferPtrPtr(), &uiStride, &uiOffset);
}

/*
inline void EvoDX10Renderer::SetVBs(EvoBuffer **ppBuffer, uint uiSlot, uint uiCnt, uint *puiStrides, uint *puiOffsets)
{

	m_pDevice->IASetVertexBuffers(uiSlot, uiCnt, ppBuffer
}
*/

inline void EvoDX10Renderer::SetIB(EvoBuffer *pBuffer, uint uiStride, uint uiOffset)
{
	DXGI_FORMAT eFormat;

	if (uiStride == 2)
		eFormat = DXGI_FORMAT_R16_UINT;
	else eFormat = DXGI_FORMAT_R32_UINT;

	m_pDevice->IASetIndexBuffer(pBuffer->GetBuffer(), eFormat, uiOffset);
}

inline void EvoDX10Renderer::SetPrimitiveType(EvoPrimitive::EEvoPrimitive ePrimitive)
{
	m_pDevice->IASetPrimitiveTopology((D3D10_PRIMITIVE_TOPOLOGY)ePrimitive);
}

inline void EvoDX10Renderer::DrawIndexed(uint uiVertCnt, uint uiIndexCnt, uint uiStartIndex, int iOffset)
{
	m_pDevice->DrawIndexed(uiIndexCnt, uiStartIndex, iOffset);
}

inline void EvoDX10Renderer::BindToPS(uint uiSlot, EvoTexture *pTexture)
{
	if (pTexture->GetRV())
	{
		m_pDevice->PSSetShaderResources(uiSlot, 1, pTexture->GetRVPtrPtr());
	}
	else
	{
		assert(0);
	}
}

#endif
#endif

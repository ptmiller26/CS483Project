#ifndef _EVODX9RENDERER_H_
#define _EVODX9RENDERER_H_

#if defined (EVO_RENDERER_DX9)

class EvoDX9Renderer
{
   public:
	EvoDX9Renderer();
	~EvoDX9Renderer();

	bool Initialize(HWND hWindow, const EvoInitData& InitData);

	void Begin();
	
	void End();

	void Present();

	void Clear(EvoColor &Color);

	void SetVB(EvoBuffer *pBuffer, uint uiSlot, uint uiStride, uint uiOffset);
	void SetVBs(EvoBuffer **ppBuffer, uint uiSlot, uint uiCnt, uint *puiStrides, uint *puiOffsets);

	// uiStride is 2 or 4
	void SetIB(EvoBuffer *pBuffer, uint uiStride, uint uiOffset);

	void SetInputLayout(EvoInputLayout *pLayout);

	void BindToPS(uint uiSlot, EvoTexture *pTexture);
	void UnbindPS(uint uiSlot);

	void SetPrimitiveType(EvoPrimitive::EEvoPrimitive ePrimitive);

	void DrawIndexed(uint uiVertCnt, uint uiIndexCnt, uint uiStartIndex, int iOffset);

	void BeginRenderTarget(uint uiIndex, EvoRenderTarget *pRT);
	void EndRenderTarget(uint uiIndex, EvoRenderTarget *pRT);

	LPDIRECT3DDEVICE9 GetDevice();

	EvoDX9BufferManager *GetBufferMgr();

   protected:
	LPDIRECT3D9					m_pD3D;
    LPDIRECT3DDEVICE9			m_pDevice;

	D3DVIEWPORT9				m_Viewport;

	EvoDX9BufferManager			m_BufferMgr;

	D3DPRIMITIVETYPE			m_ePrimitiveType;
	uint						m_uiIndicesPerPrimitive;
	uint						m_iPrimitiveModifier;
};


// Inlines
inline void EvoDX9Renderer::Begin()
{
	m_pDevice->BeginScene();
}

inline void EvoDX9Renderer::End()
{
	m_pDevice->EndScene();
}

inline LPDIRECT3DDEVICE9 EvoDX9Renderer::GetDevice()
{
	return m_pDevice;
}

inline EvoDX9BufferManager *EvoDX9Renderer::GetBufferMgr()
{
	return &m_BufferMgr;
}

inline void EvoDX9Renderer::SetVB(EvoBuffer *pBuffer, uint uiSlot, uint uiStride, uint uiOffset)
{
	m_pDevice->SetStreamSource(uiSlot, pBuffer->GetBufferAsVB(), uiOffset, uiStride);
}

inline void EvoDX9Renderer::SetVBs(EvoBuffer **ppBuffer, uint uiSlot, uint uiCnt, uint *puiStrides, uint *puiOffsets)
{

}

// uiStride is 2 or 4
inline void EvoDX9Renderer::SetIB(EvoBuffer *pBuffer, uint uiStride, uint uiOffset)
{
	m_pDevice->SetIndices(pBuffer->GetBufferAsIB());
}

inline void EvoDX9Renderer::BindToPS(uint uiSlot, EvoTexture *pTexture)
{
	m_pDevice->SetTexture(uiSlot, pTexture->GetTexture());
}

inline void EvoDX9Renderer::UnbindPS(uint uiSlot)
{
	m_pDevice->SetTexture(uiSlot, NULL);
}

inline void EvoDX9Renderer::SetInputLayout(EvoInputLayout *pLayout)
{
	m_pDevice->SetVertexDeclaration(pLayout->GetRendererSpecific());
}

inline void EvoDX9Renderer::SetPrimitiveType(EvoPrimitive::EEvoPrimitive ePrimitive)
{
	if (ePrimitive == EvoPrimitive::TRIANGLELIST)
	{
		m_ePrimitiveType = D3DPT_TRIANGLELIST;
		m_uiIndicesPerPrimitive = 3;
		m_iPrimitiveModifier = 0;
	}
	else if (ePrimitive == EvoPrimitive::LINELIST)
	{
		m_ePrimitiveType = D3DPT_LINELIST;
		m_uiIndicesPerPrimitive = 2;
		m_iPrimitiveModifier = 0;
	}
	else if (ePrimitive == EvoPrimitive::TRIANGLESTRIP)
	{
		m_ePrimitiveType = D3DPT_TRIANGLESTRIP;
		m_uiIndicesPerPrimitive = 1;
		m_iPrimitiveModifier = -2;
	}
}

inline void EvoDX9Renderer::DrawIndexed(uint uiVertCnt, uint uiIndexCnt, uint uiStartIndex, int iOffset)
{
	uint uiPrimCnt = uiIndexCnt/m_uiIndicesPerPrimitive + m_iPrimitiveModifier;

	m_pDevice->DrawIndexedPrimitive(m_ePrimitiveType, iOffset, 0, uiVertCnt, uiStartIndex, uiPrimCnt);
}

#endif
#endif
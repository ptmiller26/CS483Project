#ifndef _EVODEPTHSTENCILSTATE_H_
#define _EVODEPTHSTENCILSTATE_H_

class EvoDepthStencilState
{
   public:
	EvoDepthStencilState();
	~EvoDepthStencilState();

	void SetDepth(bool bDepthEnable, bool bDepthWriteEnable, EvoComparison::EComparison eDepthCompare);

	void SetStencilRef(uint uiRef);

   protected:
	void Apply();

   protected:
    bool m_bDepthEnable;
	bool m_bDepthWriteEnable;
	EvoComparison::EComparison m_eDepthCompare;

	bool m_bStencilEnable;
	uchar m_ucStencilReadMask;
	uchar m_ucStencilWriteMask;

	EvoStencilOp::EStencilOp m_eFrontFaceStencilFailOp;
    EvoStencilOp::EStencilOp m_eFrontFaceStencilDepthFailOp;
    EvoStencilOp::EStencilOp m_eFrontFaceStencilPassOp;
    EvoComparison::EComparison m_eFrontFaceStencilCompare;

	EvoStencilOp::EStencilOp m_eBackFaceStencilFailOp;
    EvoStencilOp::EStencilOp m_eBackFaceStencilDepthFailOp;
    EvoStencilOp::EStencilOp m_eBackFaceStencilPassOp;
    EvoComparison::EComparison m_eBackFaceStencilCompare;

	uint m_uiStencilRef;

	bool m_bDirty;

#if defined(EVO_RENDERER_DX10)
	ID3D10DepthStencilState *m_pState;
#endif

	friend class EvoEngine;
};

// Inlines

inline void EvoDepthStencilState::SetStencilRef(uint uiRef)
{
	// No need to mark as dirty for this function for DX10
	m_uiStencilRef = uiRef;
}

#endif
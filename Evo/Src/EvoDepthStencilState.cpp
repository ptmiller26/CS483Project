#include "EvoStdAfx.h"

EvoDepthStencilState::EvoDepthStencilState()
{
	m_bDepthEnable			= true;
	m_bDepthWriteEnable		= true;
	m_eDepthCompare			= EvoComparison::LESS;

	m_bStencilEnable		= false;
	m_ucStencilReadMask		= 0xff;
	m_ucStencilWriteMask	= 0xff;

	m_eFrontFaceStencilFailOp		= EvoStencilOp::KEEP;
    m_eFrontFaceStencilDepthFailOp	= EvoStencilOp::KEEP;
    m_eFrontFaceStencilPassOp		= EvoStencilOp::KEEP;
	m_eFrontFaceStencilCompare		= EvoComparison::ALWAYS;

	m_eBackFaceStencilFailOp		= EvoStencilOp::KEEP;
    m_eBackFaceStencilDepthFailOp	= EvoStencilOp::KEEP;
    m_eBackFaceStencilPassOp		= EvoStencilOp::KEEP;
    m_eBackFaceStencilCompare		= EvoComparison::ALWAYS;

	m_bDirty = true;

#if defined(EVO_RENDERER_DX10)
	m_pState = NULL;
#endif

}

EvoDepthStencilState::~EvoDepthStencilState()
{

}

void EvoDepthStencilState::SetDepth(bool bDepthEnable, bool bDepthWriteEnable, EvoComparison::EComparison eDepthCompare)
{
	m_bDepthEnable			= bDepthEnable;
	m_bDepthWriteEnable		= bDepthWriteEnable;
	m_eDepthCompare			= eDepthCompare;

	m_bDirty = true;
}

void EvoDepthStencilState::Apply()
{
#if defined(EVO_RENDERER_DX10)
	if (m_bDirty || !m_pState)
	{
		D3D10_DEPTH_STENCIL_DESC Desc;

		Desc.DepthEnable = m_bDepthEnable;
		Desc.DepthWriteMask = m_bDepthWriteEnable == true ? D3D10_DEPTH_WRITE_MASK_ALL : D3D10_DEPTH_WRITE_MASK_ZERO;
		Desc.DepthFunc = (D3D10_COMPARISON_FUNC )m_eDepthCompare;
		Desc.StencilEnable = m_bStencilEnable;
		Desc.StencilReadMask = m_ucStencilReadMask;
		Desc.StencilWriteMask = m_ucStencilWriteMask;
		Desc.FrontFace.StencilFailOp = (D3D10_STENCIL_OP)m_eFrontFaceStencilFailOp;
		Desc.FrontFace.StencilDepthFailOp = (D3D10_STENCIL_OP)m_eFrontFaceStencilDepthFailOp;
		Desc.FrontFace.StencilPassOp = (D3D10_STENCIL_OP)m_eFrontFaceStencilPassOp;
		Desc.FrontFace.StencilFunc = (D3D10_COMPARISON_FUNC )m_eFrontFaceStencilCompare;
		Desc.BackFace.StencilFailOp = (D3D10_STENCIL_OP)m_eBackFaceStencilFailOp;
		Desc.BackFace.StencilDepthFailOp = (D3D10_STENCIL_OP)m_eBackFaceStencilDepthFailOp;
		Desc.BackFace.StencilPassOp = (D3D10_STENCIL_OP)m_eBackFaceStencilPassOp;
		Desc.BackFace.StencilFunc = (D3D10_COMPARISON_FUNC )m_eBackFaceStencilCompare;

		SAFE_RELEASE(m_pState);
		EVOD3DDEVICE->CreateDepthStencilState(&Desc, &m_pState);
	}

	EVOD3DDEVICE->OMSetDepthStencilState(m_pState, m_uiStencilRef);
#endif

#if defined(EVO_RENDERER_DX9)
	EVOD3DDEVICE->SetRenderState(D3DRS_ZENABLE, m_bDepthEnable);
	EVOD3DDEVICE->SetRenderState(D3DRS_ZWRITEENABLE , m_bDepthWriteEnable);
#endif
}
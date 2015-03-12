#include "EvoStdAfx.h"

EvoBlendState::EvoBlendState()
{
	m_bAlphaToCoverageEnable = false;
	memset(m_abBlendEnable, 0, sizeof(BOOL)*8);
	m_eSrcBlend		= EvoBlend::ONE;
	m_eDestBlend	= EvoBlend::ZERO;
	m_eBlendOp		= EvoBlendOp::ADD;
	m_eSrcBlendAlpha	= EvoBlend::ONE;
	m_eDestBlendAlpha	= EvoBlend::ZERO;
	m_eBlendOpAlpha		= EvoBlendOp::ADD;
	memset(m_auiRenderTargetWriteMask, 1 | 2 | 4 | 8, sizeof(uchar)*8);

	m_bDirty = true;

#if defined(EVO_RENDERER_DX10)
	m_pState = NULL;
#endif

}

EvoBlendState::~EvoBlendState()
{
#if defined(EVO_RENDERER_DX10)
	SAFE_RELEASE(m_pState);
#endif
}

void EvoBlendState::SetAlphaToCoverage(bool bEnable)
{
	m_bAlphaToCoverageEnable	= bEnable;
	m_bDirty					= true;
}

void EvoBlendState::SetBlendEnable(uint uiIndex, bool bEnable)
{
	m_abBlendEnable[uiIndex]	= bEnable;
	m_bDirty					= true;
}

void EvoBlendState::SetBlend(EvoBlend::EEvoBlend eSrc, EvoBlend::EEvoBlend eDst, EvoBlendOp::EEvoBlendOp eOp)
{
	m_eSrcBlend		= eSrc;
	m_eDestBlend	= eDst;
	m_eBlendOp		= eOp;
	m_bDirty		= true;
}

void EvoBlendState::SetBlendAlpha(EvoBlend::EEvoBlend eSrc, EvoBlend::EEvoBlend eDst, EvoBlendOp::EEvoBlendOp eOp)
{
	m_eSrcBlendAlpha	= eSrc;
	m_eDestBlendAlpha	= eDst;
	m_eBlendOpAlpha		= eOp;
	m_bDirty			= true;
}

void EvoBlendState::SetRenderTargetMask(uint uiIndex, char cMask)
{
	m_auiRenderTargetWriteMask[uiIndex] = cMask;
	m_bDirty							= true;
}

void EvoBlendState::Apply()
{
#if defined(EVO_RENDERER_DX10)
	if (m_bDirty || !m_pState)
	{
		SAFE_RELEASE(m_pState);
		EVOD3DDEVICE->CreateBlendState((const D3D10_BLEND_DESC *)&m_bAlphaToCoverageEnable, &m_pState);
	}

	float afBlendFactors[4] = {0,0,0,0};
	uint uiSampleMask   = 0xffffffff;

	EVOD3DDEVICE->OMSetBlendState(m_pState, afBlendFactors, uiSampleMask);
#endif

}
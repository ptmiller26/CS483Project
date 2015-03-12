#ifndef _EVOBLENDSTATE_H_
#define _EVOBLENDSTATE_H_

class EvoBlendState
{
   public:
	EvoBlendState();
	~EvoBlendState();

	void SetAlphaToCoverage(bool bEnable);
	void SetBlendEnable(uint uiIndex, bool bEnable);
	void SetBlend(EvoBlend::EEvoBlend eSrc, EvoBlend::EEvoBlend eDst, EvoBlendOp::EEvoBlendOp eOp);
	void SetBlendAlpha(EvoBlend::EEvoBlend eSrc, EvoBlend::EEvoBlend eDst, EvoBlendOp::EEvoBlendOp eOp);
	void SetRenderTargetMask(uint uiIndex, char cMask);

   protected:
	void Apply();

   protected:
	BOOL m_bAlphaToCoverageEnable;
    BOOL m_abBlendEnable[8];
	EvoBlend::EEvoBlend m_eSrcBlend;
    EvoBlend::EEvoBlend m_eDestBlend;
    EvoBlendOp::EEvoBlendOp m_eBlendOp;
    EvoBlend::EEvoBlend m_eSrcBlendAlpha;
    EvoBlend::EEvoBlend m_eDestBlendAlpha;
	EvoBlendOp::EEvoBlendOp m_eBlendOpAlpha;
    uchar m_auiRenderTargetWriteMask[8];

	bool m_bDirty;

#if defined(EVO_RENDERER_DX10)
	ID3D10BlendState *m_pState;
#endif

	friend class EvoEngine;
};

// Inlines


#endif
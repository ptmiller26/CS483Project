#ifndef _EVOSHADER_H_
#define _EVOSHADER_H_

enum EEvoShaderBinding
{
	EVO_SHADER_WORLD,
	EVO_SHADER_VIEW,
	EVO_SHADER_PROJECTION,
	EVO_SHADER_WORLDVIEW,
	EVO_SHADER_VIEWPROJECTION,
	EVO_SHADER_WORLDVIEWPROJECTION,
	EVO_SHADER_INVWORLD,
	EVO_SHADER_INVVIEW,

	EV0_SHADER_BONEMATRIXARRAY,

	NUM_EVO_SHADER_BINDINGS,
};

extern char *szEvoShaderBindingName[NUM_EVO_SHADER_BINDINGS];

class EvoShader
{
   public:
	bool Begin(uint *puiNumPasses, uint uiFlags = 0);
	bool End();

	bool BeginPass(uint uiPass);
	bool EndPass();

	bool SetTechnique(char *szTechnique);
	bool SetTechnique(int iTechnique);

	char *GetCurrentTechniqueName();

	bool SetValue(char *szVarName, void *pData, uint uiSize );
	bool SetMatrixPointerArray(EEvoShaderBinding eBinding, EvoMatrix **pMatrices, uint uiCnt );
	bool SetMatrixArray(EEvoShaderBinding eBinding, EvoMatrix *pMatrices, uint uiCnt );
	bool SetTexture(char *szVarName, EvoTexture *pTexture);
	bool SetMatrix(char *szVarName, EvoMatrix &mat);
	bool SetMatrixArray(char *szVarName, EvoMatrix *pMatrices, uint uiCnt);


	bool GetTextureParam(unsigned int uiIndex, EvoString *pString, uint *puiSlot);

	const char *GetFilename()	{ return strFilename.GetBuffer();	}

	static bool SetSharedValue(char *szVarName, void *pData, uint uiSize);
	static bool SetSharedMatrix(char *szVarName, EvoMatrix &mat);
	static bool SetSharedMatrixArray(char *szVarName, EvoMatrix *pMatrices, uint uiCnt);

   protected:
	EvoShader();
	~EvoShader();

	void Initialize(char *szFilename);
	void BuildShaderBindings();
	void ApplyShaderBindings();

   protected:
	EvoString strFilename;

#if defined(EVO_RENDERER_DX9)
	static ID3DXEffectPool *ms_pEffectPool;

	std::vector< std::pair<D3DXHANDLE, EEvoShaderBinding> > m_aShaderBindings;

	ID3DXEffect *		m_pEffect;
	D3DXEFFECT_DESC		m_EffectDesc;
	char *				m_pCurrentTechnique;
#endif

#if defined(EVO_RENDERER_DX10)
	std::vector< std::pair<ID3D10EffectVariable *, EEvoShaderBinding> > m_aShaderBindings;
	
	//stdext::hash_map<std::string, ID3D10InputLayout *> m_InputLayoutMap;

	ID3D10Effect *				m_pEffect;
	D3D10_EFFECT_DESC			m_EffectDesc;
	ID3D10EffectTechnique *		m_pCurrentTechnique;
	ID3D10EffectPass *			m_pCurrentPass;
#endif

	friend class EvoResourceManager;
};

// Inlines
#if defined(EVO_RENDERER_DX9)
#include "EvoShader_DX9.inl"
#endif

#if defined(EVO_RENDERER_DX10)
#include "EvoShader_DX10.inl"
#endif

#endif
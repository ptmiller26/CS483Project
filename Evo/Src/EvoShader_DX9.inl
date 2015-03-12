#if defined (EVO_RENDERER_DX9)

inline bool EvoShader::Begin(uint *puiNumPasses, uint uiFlags)
{
	m_pEffect->Begin(puiNumPasses, uiFlags);
	ApplyShaderBindings();

/*	D3DXTECHNIQUE_DESC Desc;
	m_pEffect->GetTechniqueDesc(m_pEffect->GetCurrentTechnique(), &Desc);

	stdext::hash_map<std::string, IDirect3DVertexDeclaration9 *>::iterator iter = m_InputLayoutMap.find(Desc.Name);

	if (iter != m_InputLayoutMap.end())
	{
		EVOD3DDEVICE->SetVertexDeclaration( iter->second );
	}
*/
	return true;
}

inline bool EvoShader::BeginPass(uint uiPass)
{
	return m_pEffect->BeginPass(uiPass) == D3D_OK;
}

inline bool EvoShader::End()
{
	return m_pEffect->End() == D3D_OK;
}

inline bool EvoShader::EndPass()
{
	return m_pEffect->EndPass() == D3D_OK;
}

inline bool EvoShader::SetValue(char *szVarName, void *pData, uint uiSize )
{
	return m_pEffect->SetValue(szVarName, pData, uiSize) == D3D_OK;
}

inline bool EvoShader::SetMatrixPointerArray(EEvoShaderBinding eBinding, EvoMatrix **pMatrices, uint uiCnt )
{
	for (uint ui = 0; ui < m_aShaderBindings.size(); ui++)
	{
		if (m_aShaderBindings[ui].second == EV0_SHADER_BONEMATRIXARRAY)
		{
			return m_pEffect->SetMatrixPointerArray(m_aShaderBindings[ui].first, (const D3DXMATRIX **)pMatrices, uiCnt) == D3D_OK;
		}
	}

	return false;
}

inline bool EvoShader::SetTexture(char *szVarName, EvoTexture *pTexture)
{
	return m_pEffect->SetTexture(szVarName, pTexture->GetTexture()) == D3D_OK;	
}

inline bool EvoShader::SetMatrix(char *szVarName, EvoMatrix &mat)
{
	return m_pEffect->SetMatrix(szVarName, mat.GetD3DMatrix()) == D3D_OK;	
}

inline bool EvoShader::SetMatrixArray(char *szVarName, EvoMatrix *pMatrices, uint uiCnt)
{
	return m_pEffect->SetMatrixArray(szVarName, (const D3DXMATRIX *)pMatrices, uiCnt) == D3D_OK;	
}

inline bool EvoShader::SetTechnique(char *szTechnique)
{
	return m_pEffect->SetTechnique(szTechnique) == S_OK;	
}

inline bool EvoShader::SetTechnique(int iTechnique)
{
	return m_pEffect->SetTechnique(m_pEffect->GetTechnique(iTechnique)) == S_OK;	
}

inline char *EvoShader::GetCurrentTechniqueName()
{
	static D3DXTECHNIQUE_DESC s_Desc;
	m_pEffect->GetTechniqueDesc(m_pEffect->GetCurrentTechnique(), &s_Desc);

	return (char*)s_Desc.Name;
}

#endif
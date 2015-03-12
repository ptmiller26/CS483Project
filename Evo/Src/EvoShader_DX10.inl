#if defined(EVO_RENDERER_DX10)

inline bool EvoShader::Begin(uint *puiNumPasses, uint uiFlags)
{
	D3D10_TECHNIQUE_DESC TechniqueDesc;
	m_pCurrentTechnique->GetDesc(&TechniqueDesc);

	*puiNumPasses = TechniqueDesc.Passes;

	ApplyShaderBindings();

	stdext::hash_map<std::string, ID3D10InputLayout *>::iterator iter = m_InputLayoutMap.find(TechniqueDesc.Name);

	if (iter != m_InputLayoutMap.end())
	{
		EVOD3DDEVICE->IASetInputLayout( iter->second );
	}

	return true;
}

inline bool EvoShader::BeginPass(uint uiPass)
{
	m_pCurrentPass = m_pCurrentTechnique->GetPassByIndex(uiPass);

	HRESULT hr = m_pCurrentPass->Apply(NULL);

	return hr == S_OK;
	
}

inline bool EvoShader::End()
{
	return true;
}

inline bool EvoShader::EndPass()
{
	m_pCurrentPass = NULL;
	return true;
}

inline bool EvoShader::SetValue(char *szVarName, void *pData, uint uiSize )
{
	HRESULT hr = m_pEffect->GetVariableByName(szVarName)->SetRawValue(pData, 0, uiSize);
	return hr == S_OK;
}

inline bool EvoShader::SetMatrixPointerArray(EEvoShaderBinding eBinding, EvoMatrix **pMatrices, uint uiCnt )
{
	for (uint ui = 0; ui < m_aShaderBindings.size(); ui++)
	{
		if (m_aShaderBindings[ui].second == EV0_SHADER_BONEMATRIXARRAY)
		{
			for (uint ui2 = 0; ui2 < uiCnt; ui2++)
			{
				m_aShaderBindings[ui].first->AsMatrix()->SetMatrixArray((float*)pMatrices[ui2], ui2, 1);
			}
			return true;
		}
	}

	return false;
}

inline bool EvoShader::SetMatrixArray(EEvoShaderBinding eBinding, EvoMatrix *pMatrices, uint uiCnt )
{
	for (uint ui = 0; ui < m_aShaderBindings.size(); ui++)
	{
		if (m_aShaderBindings[ui].second == EV0_SHADER_BONEMATRIXARRAY)
		{
			m_aShaderBindings[ui].first->AsMatrix()->SetMatrixArray((float*)pMatrices, 0, uiCnt);
			return true;
		}
	}

	return false;
}

inline bool EvoShader::SetTexture(char *szVarName, EvoTexture *pTexture)
{
	m_pEffect->GetVariableByName(szVarName)->AsShaderResource()->SetResource(pTexture->GetRV());
}

inline bool EvoShader::SetTechnique(char *szTechnique)
{
	m_pCurrentTechnique = m_pEffect->GetTechniqueByName(szTechnique);

	return m_pCurrentTechnique != NULL;
}

#endif
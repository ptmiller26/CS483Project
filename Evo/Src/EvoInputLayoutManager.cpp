#include "EvoStdAfx.h"

bool EvoInputLayoutMappings::AreEquivalent(EvoInputElements &IE)
{
	if (m_InputElements.GetElementCount() == IE.GetElementCount())
	{
		for (uint ui = 0; ui < m_InputElements.GetElementCount(); ui++)
		{
			if (memcmp(&m_InputElements.m_aInputElements[ui], &IE.m_aInputElements[ui], sizeof(EvoInputElements::InputElement)) != 0)
			{
				return false;
			}
		}
		return true;
	}

	return false;
}

EvoInputLayoutManager::EvoInputLayoutManager()
{

}

EvoInputLayoutManager::~EvoInputLayoutManager()
{

}

EvoInputLayoutHandle EvoInputLayoutManager::RegisterInputElements(EvoInputElements InputElements)
{
	for (uint ui = 0; ui < m_aLayoutMappings.size(); ui++)
	{
		if (m_aLayoutMappings[ui].AreEquivalent(InputElements))
		{
			return (EvoInputLayoutHandle)ui;
		}
	}

	// Didn't find one, so add a new one
	EvoInputLayoutMappings ILM;
	ILM.m_InputElements = InputElements;
	m_aLayoutMappings.push_back(ILM);

	return (EvoInputLayoutHandle)(m_aLayoutMappings.size() - 1);

}

const EvoInputLayout *EvoInputLayoutManager::GetLayout(EvoInputLayoutHandle hInputLayout, EvoShader *pShader, char *szTechnique)
{
	// NOTE: Could preprocess these strings into the shader class to avoid assembling it here
	static EvoString s_strShaderAndTechnique;
	s_strShaderAndTechnique.Copy((char*)pShader->GetFilename(), false);
	s_strShaderAndTechnique.Concat(szTechnique, false);

	stdext::hash_map<std::string, EvoInputLayout *>::iterator iter = m_aLayoutMappings[hInputLayout].m_ShaderTechniqueLayoutMap.find(s_strShaderAndTechnique.GetBuffer());

	if (iter != m_aLayoutMappings[hInputLayout].m_ShaderTechniqueLayoutMap.end())
	{
		return iter->second;
	}
	else
	{
		// Create new Input Layout
		EvoInputLayout *pIL = new EvoInputLayout(pShader, szTechnique, &m_aLayoutMappings[hInputLayout].m_InputElements);
		m_aLayoutMappings[hInputLayout].m_ShaderTechniqueLayoutMap.insert(std::pair <std::string, EvoInputLayout *>(s_strShaderAndTechnique.GetBuffer(),pIL));
		return pIL;
	}
}


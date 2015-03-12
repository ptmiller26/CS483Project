#include "EvoStdAfx.h"

#if defined(EVO_RENDERER_DX10)

char *szEvoShaderBindingName[NUM_EVO_SHADER_BINDINGS] = 
{
	"WORLD", 
	"VIEW",
	"PROJECTION",
	"WORLDVIEW",
	"VIEWPROJECTION",
	"WORLDVIEWPROJECTION",
	"INVWORLD",
	"INVVIEW",
	"BONEMATRIXARRAY"
};

EvoShader::EvoShader()
{
	m_pEffect				= NULL;
	m_pCurrentTechnique		= NULL;
	m_pCurrentPass			= NULL;
}

EvoShader::~EvoShader()
{

}

void EvoShader::Initialize(char *szFilename)
{
	uint uiShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    uiShaderFlags |= D3D10_SHADER_DEBUG;
#endif

	LPD3D10BLOB pBlob = NULL;

	D3DX10CreateEffectFromFile( szFilename, NULL, NULL, "fx_4_0", uiShaderFlags, 0, EVOD3DDEVICE, NULL, NULL, &m_pEffect, &pBlob, NULL );

	if (pBlob)
	{
		char *pErrors = (char*)pBlob->GetBufferPointer();
		OutputDebugString(pErrors);
		pBlob->Release();
	}

	m_pEffect->GetDesc(&m_EffectDesc);

	BuildShaderBindings();

	strFilename.StripPath(szFilename);
}
/*
void EvoShader::SetInputElements(char *szTechnique, EvoInputElements InputElements)
{
	ID3D10InputLayout *pInputLayout = NULL;
	ID3D10EffectTechnique *pTechnique = m_pEffect->GetTechniqueByName(szTechnique);

	D3D10_TECHNIQUE_DESC TechniqueDesc;
	pTechnique->GetDesc(&TechniqueDesc);

	bool bIsValid = pTechnique->IsValid() == TRUE;

    D3D10_PASS_DESC PassDesc;
    pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );

	HRESULT hr = EVOD3DDEVICE->CreateInputLayout(InputElements.GetRendererSpecificFormat(), InputElements.GetElementCount(), PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout);

	m_InputLayoutMap.insert(std::pair <std::string, ID3D10InputLayout *>(szTechnique,pInputLayout));
}*/

void EvoShader::BuildShaderBindings()
{
	ID3D10EffectVariable * pEffectVariable;

	for (uint ui = 0; ui < NUM_EVO_SHADER_BINDINGS; ui++)
	{
		pEffectVariable = NULL;
		pEffectVariable = m_pEffect->GetVariableBySemantic ( szEvoShaderBindingName[ui] );

		if (pEffectVariable)
		{
			m_aShaderBindings.push_back(std::make_pair(pEffectVariable, (EEvoShaderBinding)ui));
		}
	}
}

void EvoShader::ApplyShaderBindings()
{
	for (uint ui = 0; ui < m_aShaderBindings.size(); ui++)
	{
		switch (m_aShaderBindings[ui].second)
		{
			case EVO_SHADER_WORLD:
			{
				m_aShaderBindings[ui].first->AsMatrix()->SetMatrix((float*)EVO->GetTransform(EVO_TRANSFORM_WORLD));
				break;
			}

			case EVO_SHADER_VIEW:
			{
				m_aShaderBindings[ui].first->AsMatrix()->SetMatrix((float*)EVO->GetTransform(EVO_TRANSFORM_VIEW));
				break;
			}

			case EVO_SHADER_PROJECTION:
			{
				m_aShaderBindings[ui].first->AsMatrix()->SetMatrix((float*)EVO->GetTransform(EVO_TRANSFORM_PROJECTION));
				break;
			}

			case EVO_SHADER_WORLDVIEW:
			{
				EvoMatrix m;
				EVO->GetTransform(EVO_TRANSFORM_WORLD)->Multiply(EVO->GetTransform(EVO_TRANSFORM_VIEW), &m);
				m_aShaderBindings[ui].first->AsMatrix()->SetMatrix((float*)&m);
				break;
			}

			case EVO_SHADER_VIEWPROJECTION:
			{
				EvoMatrix m;
				EVO->GetTransform(EVO_TRANSFORM_VIEW)->Multiply(EVO->GetTransform(EVO_TRANSFORM_PROJECTION), &m);
				m_aShaderBindings[ui].first->AsMatrix()->SetMatrix((float*)&m);
				break;
			}

			case EVO_SHADER_WORLDVIEWPROJECTION:
			{
				EvoMatrix m;
				EVO->GetTransform(EVO_TRANSFORM_WORLD)->Multiply(EVO->GetTransform(EVO_TRANSFORM_VIEW), &m);
				m.Multiply(EVO->GetTransform(EVO_TRANSFORM_PROJECTION));
				m_aShaderBindings[ui].first->AsMatrix()->SetMatrix((float*)&m);
				break;
			}

			case EVO_SHADER_INVWORLD:
			{
				EvoMatrix m;
				EVO->GetTransform(EVO_TRANSFORM_WORLD)->Multiply(EVO->GetTransform(EVO_TRANSFORM_WORLD), &m);
				m.Inverse();
				m_aShaderBindings[ui].first->AsMatrix()->SetMatrix((float*)&m);
				break;
			}

			case EVO_SHADER_INVVIEW:
			{
				EvoMatrix m;
				EVO->GetTransform(EVO_TRANSFORM_WORLD)->Multiply(EVO->GetTransform(EVO_TRANSFORM_VIEW), &m);
				m.Inverse();
				m_aShaderBindings[ui].first->AsMatrix()->SetMatrix((float*)&m);
				break;
			}

		}
	}
}

#endif
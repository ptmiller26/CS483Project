#include "EvoStdAfx.h"

#if defined(EVO_RENDERER_DX9)

ID3DXEffectPool *EvoShader::ms_pEffectPool = NULL;


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
	m_pEffect = NULL;
}

EvoShader::~EvoShader()
{

}

void EvoShader::Initialize(char *szFilename)
{
	if (ms_pEffectPool == NULL)
	{
		HRESULT hRes = D3DXCreateEffectPool(&ms_pEffectPool);
	}

	uint uiShaderFlags =  0;//D3DXSHADER_USE_LEGACY_D3DX9_31_DLL;

	D3DXCreateEffectFromFile( EVOD3DDEVICE, szFilename, NULL, NULL, uiShaderFlags, ms_pEffectPool, &m_pEffect, NULL );

	m_pEffect->GetDesc(&m_EffectDesc);

	BuildShaderBindings();

	strFilename.StripPath(szFilename);
}

/*
void EvoShader::SetInputElements(char *szTechnique, EvoInputElements InputElements)
{
	IDirect3DVertexDeclaration9 *pVDecl = NULL;
	EVOD3DDEVICE->CreateVertexDeclaration(InputElements.GetRendererSpecificFormat(), &pVDecl);

	m_InputLayoutMap.insert(std::pair <std::string, IDirect3DVertexDeclaration9 *>(szTechnique,pVDecl));
}

void EvoShader::SetInputElements(int iTechnique, EvoInputElements InputElements)
{
	D3DXTECHNIQUE_DESC Desc;
	m_pEffect->GetTechniqueDesc(  m_pEffect->GetTechnique(iTechnique), &Desc);

	SetInputElements((char*)Desc.Name, InputElements);
}
*/
void EvoShader::BuildShaderBindings()
{
	D3DXHANDLE hParam;

	for (uint ui = 0; ui < NUM_EVO_SHADER_BINDINGS; ui++)
	{
		hParam = m_pEffect->GetParameterBySemantic ( NULL, szEvoShaderBindingName[ui] );

		if (hParam)
		{
			m_aShaderBindings.push_back(std::make_pair(hParam, (EEvoShaderBinding)ui));
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
				m_pEffect->SetMatrix(m_aShaderBindings[ui].first, (const D3DXMATRIX *)EVO->GetTransform(EVO_TRANSFORM_WORLD));
				break;
			}

			case EVO_SHADER_VIEW:
			{
				m_pEffect->SetMatrix(m_aShaderBindings[ui].first, (const D3DXMATRIX *)EVO->GetTransform(EVO_TRANSFORM_VIEW));
				break;
			}
	
			case EVO_SHADER_INVVIEW:
			{
				m_pEffect->SetMatrix(m_aShaderBindings[ui].first, (const D3DXMATRIX *)EVO->GetTransform(EVO_TRANSFORM_INVVIEW));
				break;
			}

			case EVO_SHADER_PROJECTION:
			{
				m_pEffect->SetMatrix(m_aShaderBindings[ui].first, (const D3DXMATRIX *)EVO->GetTransform(EVO_TRANSFORM_PROJECTION));
				break;
			}

			case EVO_SHADER_WORLDVIEW:
			{
				EvoMatrix m;
				EVO->GetTransform(EVO_TRANSFORM_WORLD)->Multiply(EVO->GetTransform(EVO_TRANSFORM_VIEW), &m);
				m_pEffect->SetMatrix(m_aShaderBindings[ui].first, (const D3DXMATRIX *)&m);
				break;
			}

			case EVO_SHADER_VIEWPROJECTION:
			{
				EvoMatrix m;
				EVO->GetTransform(EVO_TRANSFORM_VIEW)->Multiply(EVO->GetTransform(EVO_TRANSFORM_PROJECTION), &m);
				m_pEffect->SetMatrix(m_aShaderBindings[ui].first, (const D3DXMATRIX *)&m);
				break;
			}

			case EVO_SHADER_WORLDVIEWPROJECTION:
			{
				EvoMatrix m;
				EVO->GetTransform(EVO_TRANSFORM_WORLD)->Multiply(EVO->GetTransform(EVO_TRANSFORM_VIEW), &m);
				m.Multiply(EVO->GetTransform(EVO_TRANSFORM_PROJECTION));
				m_pEffect->SetMatrix(m_aShaderBindings[ui].first, (const D3DXMATRIX *)&m);
				break;
			}

			case EVO_SHADER_INVWORLD:
			{
				EvoMatrix m;
				m = *EVO->GetTransform(EVO_TRANSFORM_WORLD);
				m.Inverse();
				m_pEffect->SetMatrix(m_aShaderBindings[ui].first, (const D3DXMATRIX *)&m);
				break;
			}

		}
	}
}

bool EvoShader::GetTextureParam(unsigned int uiIndex, EvoString *pString, uint *puiSlot)
{
	D3DXHANDLE hParam = NULL;
	uint uiParamIndex = 0;
	uint uiTextureIndex = 0;
	D3DXPARAMETER_DESC ParamDesc;

	do
	{
		hParam = m_pEffect->GetParameter(NULL, uiParamIndex);

		m_pEffect->GetParameterDesc(hParam, &ParamDesc);

		if (ParamDesc.Type == D3DXPT_TEXTURE || ParamDesc.Type == D3DXPT_TEXTURECUBE)
		{
			if (uiIndex == uiTextureIndex)
			{
				pString->Copy((char*)ParamDesc.Name, false);

				D3DXHANDLE hAnnot = m_pEffect->GetAnnotationByName(hParam, "Slot");
				assert(hAnnot && "Shader must have Slot annotation");
				m_pEffect->GetValue(hAnnot, puiSlot, D3DX_DEFAULT);

				return true;
			}
			else
			{
				uiTextureIndex++;
			}
		}

		uiParamIndex++;

	} while (hParam != NULL);

	return false;
}

bool EvoShader::SetSharedValue(char *szVarName, void *pData, uint uiSize)
{
	std::vector<EvoShader *> *paShaders = RESMGR->GetShaders();

	for (uint ui = 0; ui < paShaders->size(); ui++)
	{
		if (paShaders->at(ui)->SetValue(szVarName, pData, uiSize))
		{
			return true;
		}
	}

	return false;
}

bool EvoShader::SetSharedMatrix(char *szVarName, EvoMatrix &mat)
{
	std::vector<EvoShader *> *paShaders = RESMGR->GetShaders();
	for (uint ui = 0; ui < paShaders->size(); ui++)
	{
		if (paShaders->at(ui)->SetMatrix(szVarName, mat))
			return true;
	}
	return false;
}

bool EvoShader::SetSharedMatrixArray(char *szVarName, EvoMatrix *pMatrices, uint uiCnt)
{
	std::vector<EvoShader *> *paShaders = RESMGR->GetShaders();
	for (uint ui = 0; ui < paShaders->size(); ui++)
	{
		if (paShaders->at(ui)->SetMatrixArray(szVarName, pMatrices, uiCnt))
			return true;
	}
	return false;
}


#endif

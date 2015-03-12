#include "EvoStdAfx.h"

EvoInputLayout::EvoInputLayout(EvoShader *pShader, char *szTechnique, EvoInputElements *pInputElements)
{
#if defined(EVO_RENDERER_DX9)
	HRESULT hr = EVOD3DDEVICE->CreateVertexDeclaration(pInputElements->GetRendererSpecificFormat(), &m_pInputLayout);
#endif

#if defined(EVO_RENDERER_DX10)
	pShader->GetInputSignatureInfo(szTechnique, iPass, &pInputSig, &iSigSize);
	HRESULT hr = EVOD3DDEVICE->CreateInputLayout(pInputElements->GetRendererSpecificFormat(), pInputElements->GetElementCount(), pInputSig, iSigSize, &m_pInputLayout);
#endif
}

EvoInputLayout::~EvoInputLayout()
{

}
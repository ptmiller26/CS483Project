#include "EvoStdAfx.h"

struct VertStruct
{
	float fX, fY, fZ;
	D3DCOLOR Color;
	float fU, fV;
};

EvoGUIRectWidget::EvoGUIRectWidget()
{
	m_pIB = NULL;
	m_pVB = NULL;
	m_pTexture = NULL;

	m_Color = EvoColor(255, 255, 255, 255);

	unsigned short *pIBData = new unsigned short[6];

	pIBData[0] = pIBData[5] = 0;
	pIBData[1] = 1;
	pIBData[2] = pIBData[3] = 2;
	pIBData[4] = 3;

	m_pIB = EVO->CreateBuffer(6*2, 2, pIBData, EvoBind::INDEX_BUFFER, EvoUsage::DYNAMIC, EvoCpuAccess::WRITE, false, false);
	delete pIBData;

	VertStruct *pVBData = new VertStruct[4];

	pVBData[0].fX = 0.0f;
	pVBData[0].fY = 0.0f;
	pVBData[0].fZ = 0.0f;
	pVBData[0].fU = 0.0f;
	pVBData[0].fV = 0.0f;
	pVBData[0].Color = 0;

	pVBData[1].fX = 0.0f;
	pVBData[1].fY = 0.0f;
	pVBData[1].fZ = 0.0f;
	pVBData[1].fU = 0.0f;
	pVBData[1].fV = 0.0f;
	pVBData[1].Color = 0;

	pVBData[2].fX = 0.0f;
	pVBData[2].fY = 0.0f;
	pVBData[2].fZ = 0.0f;
	pVBData[2].fU = 0.0f;
	pVBData[2].fV = 0.0f;
	pVBData[2].Color = 0;

	pVBData[3].fX = 0.0f;
	pVBData[3].fY = 0.0f;
	pVBData[3].fZ = 0.0f;
	pVBData[3].fU = 0.0f;
	pVBData[3].fV = 0.0f;
	pVBData[3].Color = 0;

	m_pVB = EVO->CreateBuffer(4*sizeof(VertStruct), sizeof(VertStruct), pVBData, EvoBind::VERTEX_BUFFER, EvoUsage::DYNAMIC, EvoCpuAccess::WRITE, false, false);
	delete pVBData;

	m_pShader = RESMGR->CreateShader("Screenspace.fx");

	EvoInputElements InputElements;
	InputElements.AddElement(EvoInputElementUsage::POSITION, 0, EvoFormat::R32G32B32_FLOAT, 0, 0, false, 0);
	InputElements.AddElement(EvoInputElementUsage::COLOR, 0, EvoFormat::R8G8B8A8_UNORM, 0, 12, false, 0);
	InputElements.AddElement(EvoInputElementUsage::TEXCOORD, 0, EvoFormat::R32G32_FLOAT, 0, 16, false, 0);

	m_hInputLayout = EVO->GetInputLayoutMgr()->RegisterInputElements(InputElements);
}

EvoGUIRectWidget::~EvoGUIRectWidget()
{

}

void EvoGUIRectWidget::Render()
{
	if(!IsEnabled())
		return;
	if (m_bDirty)
	{
		RebuildGeometry();
	}

	m_pShader->SetTechnique("TScreenspace");

	uint uiNumShaderPasses = 0;
	m_pShader->Begin(&uiNumShaderPasses);

	EVO->SetInputLayout(m_hInputLayout, m_pShader, m_pShader->GetCurrentTechniqueName());

	EVO->SetIB(m_pIB, m_pIB->GetStride(), 0);
	EVO->SetVB(m_pVB, 0, m_pVB->GetStride(), 0);
	EVO->SetPrimitiveType(EvoPrimitive::TRIANGLELIST);

	for (uint ui = 0; ui < uiNumShaderPasses; ui++)
	{
		m_pShader->BeginPass(ui);

		EVO->BindToPS(0, m_pTexture);
		EVO->DrawIndexed(4, 6, 0, 0);

		m_pShader->EndPass();
	}
	m_pShader->End();
}

void EvoGUIRectWidget::RebuildGeometry()
{
	VertStruct *pV = (VertStruct *)m_pVB->Lock(EvoLock::DISCARD, true);

	float fModifier = -0.5f;

	pV[0].fX = m_fX + fModifier;
	pV[0].fY = m_fY + fModifier;
	pV[0].fZ = 0.0f;
	pV[0].fU = 0.0f;
	pV[0].fV = 0.0f;
	pV[0].Color = m_Color.GetD3DColor();

	pV[1].fX = m_fX + m_fWidth + fModifier;
	pV[1].fY = m_fY + fModifier;
	pV[1].fZ = 0.0f;
	pV[1].fU = 1.0f;
	pV[1].fV = 0.0f;
	pV[1].Color = m_Color.GetD3DColor();

	pV[2].fX = m_fX + m_fWidth + fModifier;
	pV[2].fY = m_fY + m_fHeight + fModifier;
	pV[2].fZ = 0.0f;
	pV[2].fU = 1.0f;
	pV[2].fV = 1.0f;
	pV[2].Color = m_Color.GetD3DColor();

	pV[3].fX = m_fX + fModifier;
	pV[3].fY = m_fY + m_fHeight + fModifier;
	pV[3].fZ = 0.0f;
	pV[3].fU = 0.0f;
	pV[3].fV = 1.0f;
	pV[3].Color = m_Color.GetD3DColor();

	m_pVB->Unlock();

	m_bDirty = false;
}
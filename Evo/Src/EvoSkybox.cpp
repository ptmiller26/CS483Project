#include "EvoStdAfx.h"

EvoSkybox::EvoSkybox()
{

}

EvoSkybox::~EvoSkybox()
{

}

	// Pass in filename of unit cube model
void EvoSkybox::Initialize(char *szFilename)
{
	m_pNode = RESMGR->LoadGrannyScene(szFilename);
	m_pNode->Update(0.0f);

	m_DSState.SetDepth(false, false, EvoComparison::ALWAYS);
}

void EvoSkybox::Render(EvoCamera *pCamera)
{
	float fFOV, fAspect, fNear, fFar;
	pCamera->GetParams(&fFOV, &fAspect, &fNear, &fFar);

	EvoCamera NewCamera;
	NewCamera.SetParams(fFOV, fAspect, 0.001f, 2.0f);
	NewCamera.SetWorldMatrix(pCamera->GetWorldMatrix());
	
	EvoPoint3 pt3Translate;
	pCamera->GetTranslate(&pt3Translate);
	pt3Translate.Scale(-1.0f);
	NewCamera.Translate(pt3Translate);

	EVO->PushCamera(&NewCamera);
	EVO->PushDepthStencilState(&m_DSState);

	m_pNode->Render();

	EVO->PopDepthStencilState();
	EVO->PopCamera();
}

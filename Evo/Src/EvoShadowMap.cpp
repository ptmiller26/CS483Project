#include "EvoStdAfx.h"
#include "EvoShadowMap.h"

//---------------------------------------------------------------------------
EvoShadowMap::EvoShadowMap() : 
m_pRenderedTexture(NULL),
m_pDepthStencil(NULL),
m_pRenderTarget(NULL)
{
}
//---------------------------------------------------------------------------
EvoShadowMap::~EvoShadowMap()
{
	Shutdown();
}
//---------------------------------------------------------------------------
void EvoShadowMap::Init(int width, int height)
{
	m_TextureWidth = width;
	m_TextureHeight= height;
	assert(m_TextureWidth==m_TextureHeight);	//Only support square ShadowMaps
																								//R32_FLOAT//R8G8B8A8_UNORM
	m_pRenderedTexture = RESMGR->CreateTexture(m_TextureWidth, m_TextureHeight, 0, 1, EvoFormat::R32_FLOAT,
									EvoUsage::IMMUTABLE, EvoCpuAccess::NONE, EvoBind::RENDER_TARGET, false, NULL); 
	assert(m_pRenderedTexture);	

	m_pDepthStencil = RESMGR->CreateTexture(m_TextureWidth, m_TextureHeight, 1, 1, EvoFormat::D24_UNORM_S8_UINT,
									EvoUsage::IMMUTABLE, EvoCpuAccess::NONE, EvoBind::DEPTH_STENCIL, false, NULL);

	m_pRenderTarget = new EvoRenderTarget();
	m_pRenderTarget->SetTextures(m_pRenderedTexture, m_pDepthStencil);

	m_pLightCamera = new EvoCamera;

	m_pShader = RESMGR->CreateShader("ShadowMap.fx");

	//Set Shader constants
	float fShadowMapSizeInverse = 1.0f/m_TextureWidth;
	m_pShader->SetValue("fShadowMapSize",&m_TextureWidth,sizeof(float));
	m_pShader->SetValue("fShadowMapSizeInverse",&fShadowMapSizeInverse, sizeof(float));
}
//---------------------------------------------------------------------------
void EvoShadowMap::Shutdown()
{
	RESMGR->DestroyTexture(m_pRenderedTexture);
	RESMGR->DestroyTexture(m_pDepthStencil);
	SAFE_DELETE(m_pRenderTarget);
	SAFE_DELETE(m_pLightCamera);
}
//---------------------------------------------------------------------------
void EvoShadowMap::Render()
{
	assert(m_pRenderTarget);
	EvoColor ClearColor(0,0,0,255);
	EVO->BeginRenderTarget(0, m_pRenderTarget); 
	EVO->PushClearColor(ClearColor);
	EVO->Clear();

	EVO->SetShaderOverride(m_pShader,"ShadowMapRenderShadow","ShadowMapRenderShadowSkinned");
	//Render Scene from Lights point of view
	EVO->PushCamera(m_pLightCamera);

	//Override shader to render depth only..??
	//Render Visible Entities
	/*const EntityList &VisibleEnts = WORLD->GetVisibleEnts(m_pLightCamera);
	for(uint i=0;i<VisibleEnts.size();i++)
		VisibleEnts[i]->Render();*/

	EVO->PopCamera();
	EVO->PopClearColor();
	EVO->SetShaderOverride(NULL,NULL,NULL);

	EVO->EndRenderTarget(0, m_pRenderTarget); 
}
//---------------------------------------------------------------------------
void EvoShadowMap::SetShadowCamera(const EvoCamera *pCamera)
{
	*m_pLightCamera = *pCamera;
}
//---------------------------------------------------------------------------
void EvoShadowMap::UpdateShaderConstants(EvoCamera *pCamera)
{
	//World to View
	EvoMatrix mtxView = *pCamera->GetViewMatrix();
	EvoMatrix mtxInvView = *pCamera->GetInvViewMatrix();
	
	//Get light parameters from the light camera.
	EvoPoint3 f3LightVPos;		   // Light position in view space
	m_pLightCamera->GetTranslate(&f3LightVPos);
	f3LightVPos.Multiply(&mtxView);
	EvoShader::SetSharedValue("f3LightVPos",&f3LightVPos,sizeof(EvoPoint3));

	EvoPoint4 f3LightVDir;		   // Light direction in view space
	m_pLightCamera->GetFacingVector((EvoPoint3 *)&f3LightVDir);
	f3LightVDir.m_fW = 0.0f;
	f3LightVDir.Multiply(&mtxView);
	f3LightVDir.Unitize();
	EvoShader::SetSharedValue("f3LightVDir",&f3LightVDir,sizeof(EvoPoint3));

	// Compute the matrix to transform from view space to light projection space.  
	// mtxViewToLightProj = The inverse of view matrix * view matrix of light * light projection matrix
	// mtxViewToLightProj = InvViewCam * LightView * LightProj;
	EvoMatrix mtxViewToLightProj;  // Transform from view space to light projection space
	mtxViewToLightProj = mtxInvView;
	mtxViewToLightProj.Multiply(m_pLightCamera->GetViewMatrix());
	mtxViewToLightProj.Multiply(m_pLightCamera->GetProjMatrix());
	EvoShader::SetSharedMatrix("mtxViewToLightProj",mtxViewToLightProj);

	float fCosTheta = 0.0f;		   // Cosine of theta of the spot light
	EvoShader::SetSharedValue("fCosTheta",&fCosTheta,sizeof(float));

	float fShadowMapSizeInverse = 1.0f/m_TextureWidth;
	EvoShader::SetSharedValue("fShadowMapSize",&m_TextureWidth,sizeof(float));
	EvoShader::SetSharedValue("fShadowMapSizeInverse",&fShadowMapSizeInverse, sizeof(float));

}
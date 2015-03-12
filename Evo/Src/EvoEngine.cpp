#include "EvoStdAfx.h"

EvoEngine *EvoEngine::ms_pInstance = NULL;

EvoEngine::EvoEngine()
{
	m_pRenderer			= NULL;
	m_pShaderOverride	= NULL;
}

EvoEngine::~EvoEngine()
{
    SAFE_DELETE(m_pRenderer);
}

EvoEngine * EvoEngine::GetInstance()
{
	if (!ms_pInstance)
	{
		ms_pInstance = new EvoEngine();
	}

	return ms_pInstance;
}

bool EvoEngine::Initialize(char *szIniFile, HWND hWindow, HINSTANCE hInst)
{
	m_hWnd			= hWindow;
	m_hInstance		= hInst;

	if (!m_InitData.Load(szIniFile))
		return false;
	
	// Init Renderer
	m_pRenderer = new EvoRenderer();
	m_pRenderer->Initialize(hWindow, m_InitData);

	// Setup Default World matrix and Camera
	EvoMatrix m;
	m.SetIdentity();
	SetTransform(EVO_TRANSFORM_WORLD, &m);

	EvoCamera *pDefaultCamera = new EvoCamera();
	pDefaultCamera->SetParams(DEGTORAD(60.0f), 16.0f/9.0f, 0.01f, 1000.0f);
	PushCamera(pDefaultCamera);

	// Default Primitive
	m_eCurrentPrimitive = EvoPrimitive::TRIANGLELIST;

	// Default State
	EvoDepthStencilState *pDefaultDepthStencilState = new EvoDepthStencilState();
	EvoBlendState *pDefaultBlendState = new EvoBlendState();
	PushDepthStencilState(pDefaultDepthStencilState);
	PushBlendState(pDefaultBlendState);
	
	// Default Clear Color
	PushClearColor(EvoColor(16, 16, 64, 255));

	// Clear backbuffers
	Clear();
	
	return true;
}

void EvoEngine::Shutdown()
{

}


void EvoEngine::Present()
{
	m_pRenderer->Present();
}

void EvoEngine::Clear()
{
	m_pRenderer->Clear(m_ClearColorStack.top());
}

EvoBuffer *EvoEngine::CreateBuffer(uint uiBytes, uint uiPitch, void *pData, EvoBind::EBind eBind, EvoUsage::EUsage eUsage, EvoCpuAccess::ECpuAccess eCPUAccess, bool bGenMips, bool bCubeTexture)
{
	return m_pRenderer->GetBufferMgr()->CreateBuffer(uiBytes, uiPitch, pData, eBind, eUsage, eCPUAccess, bGenMips, bCubeTexture);
}

void EvoEngine::DestroyBuffer(EvoBuffer *pBuffer)
{
	m_pRenderer->GetBufferMgr()->DestroyBuffer(pBuffer);
}

#include "EvoStdAfx.h"

#if defined (EVO_RENDERER_DX9)

EvoDX9Renderer::EvoDX9Renderer()
{
	m_pD3D		= NULL;
    m_pDevice	= NULL;
}

EvoDX9Renderer::~EvoDX9Renderer()
{
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pD3D);
}

bool EvoDX9Renderer::Initialize(HWND hWindow, const EvoInitData& InitData)
{
	D3DPRESENT_PARAMETERS Parameters;

	Parameters.BackBufferWidth = InitData.m_uiScreenWidth;
	Parameters.BackBufferHeight = InitData.m_uiScreenHeight;
	Parameters.BackBufferFormat = D3DFMT_A8R8G8B8;

	Parameters.BackBufferCount = InitData.m_uiBufferCount;
	Parameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	Parameters.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Parameters.SwapEffect = D3DSWAPEFFECT_FLIP;
	Parameters.hDeviceWindow = hWindow;
	Parameters.Windowed = !InitData.m_bFullscreen;
	Parameters.EnableAutoDepthStencil = TRUE;
	Parameters.AutoDepthStencilFormat = D3DFMT_D24S8;
	Parameters.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	Parameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	Parameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	m_pD3D = NULL;
	m_pDevice = NULL;

	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Parameters, &m_pDevice);

	m_Viewport.X = 0;
	m_Viewport.Y = 0;
	m_Viewport.Width = InitData.m_uiScreenWidth;
	m_Viewport.Height = InitData.m_uiScreenHeight;
	m_Viewport.MinZ = 0.0f;
	m_Viewport.MaxZ = 1.0f;

	m_pDevice->SetViewport(&m_Viewport);

	return false;
}

void EvoDX9Renderer::Present()
{
	m_pDevice->Present(NULL, NULL, NULL, NULL);
}

void EvoDX9Renderer::Clear(EvoColor &Color)
{
	m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, Color.GetD3DColor(), 1.0f, 0); 
}

void EvoDX9Renderer::BeginRenderTarget(uint uiIndex, EvoRenderTarget *pRT)
{
	HRESULT hRes;

	m_pDevice->GetRenderTarget(uiIndex, &pRT->m_pOldRenderTarget);

	if (uiIndex == 0)
		m_pDevice->GetDepthStencilSurface(&pRT->m_pOldDepthStencil);

	if (pRT->GetRenderTexture())
	{
		IDirect3DTexture9 *pTex9 = NULL;
		IDirect3DSurface9 *pSurface = NULL;

		pRT->GetRenderTexture()->GetTexture()->QueryInterface(IID_IDirect3DTexture9, (void **)&pTex9);
		pTex9->GetSurfaceLevel(0, &pSurface);
		
		hRes = m_pDevice->SetRenderTarget(uiIndex, pSurface);

		pSurface->Release();
		pTex9->Release();
	}
	else
	{
		m_pDevice->SetRenderTarget(uiIndex, NULL);
	}

	if (pRT->GetDepthStencilTexture())
	{
		hRes = m_pDevice->SetDepthStencilSurface(pRT->GetDepthStencilTexture()->GetSurface());
	}
	else
	{
		m_pDevice->SetDepthStencilSurface(NULL);
	}
}

void EvoDX9Renderer::EndRenderTarget(uint uiIndex, EvoRenderTarget *pRT)
{
	m_pDevice->SetRenderTarget(uiIndex, pRT->m_pOldRenderTarget);

	if (uiIndex == 0)
		m_pDevice->SetDepthStencilSurface(pRT->m_pOldDepthStencil);

	SAFE_RELEASE(pRT->m_pOldRenderTarget);
	SAFE_RELEASE(pRT->m_pOldDepthStencil);
}


#endif
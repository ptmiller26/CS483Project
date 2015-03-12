#include "EvoStdAfx.h"

#if defined (EVO_RENDERER_DX10)

EvoDX10Renderer::EvoDX10Renderer()
{
	m_pSwapChain	= NULL;
	m_pDevice		= NULL;

	m_pDefaultDepthStencilState	= NULL;
	m_pDefaultBlendState		= NULL;
}

EvoDX10Renderer::~EvoDX10Renderer()
{

}

bool EvoDX10Renderer::Initialize(HWND hWindow, const EvoInitData& InitData)
{
	DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = InitData.m_uiBufferCount;
	sd.BufferDesc.Width = InitData.m_uiScreenWidth;
    sd.BufferDesc.Height = InitData.m_uiScreenHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWindow;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
	if (InitData.m_bFullscreen)
		sd.Windowed = FALSE;
    else sd.Windowed = TRUE;
 
    if( FAILED( D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, /*D3D10_CREATE_DEVICE_DEBUG*/0, D3D10_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice)))
        return false;

	// Create a render target view
    ID3D10Texture2D *pBackBuffer;
    if( FAILED( m_pSwapChain->GetBuffer(0, __uuidof( ID3D10Texture2D ), (LPVOID*)&pBackBuffer)))
        return false;

    HRESULT hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pDefaultRTView);
    pBackBuffer->Release();

    if( FAILED( hr ) )
        return false;

	// Create Depthbuffer and depthstencil view
	// Create depth stencil texture
    D3D10_TEXTURE2D_DESC descDepth;
    descDepth.Width = InitData.m_uiScreenX;
    descDepth.Height = InitData.m_uiScreenY;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D10_USAGE_DEFAULT;
    descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

    hr = m_pDevice->CreateTexture2D( &descDepth, NULL, &m_pDefaultDepthStencil );
    if( FAILED(hr) )
        return false;

    // Create the depth stencil view
    D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    
	hr = m_pDevice->CreateDepthStencilView( m_pDefaultDepthStencil, &descDSV, &m_pDefaultDSView );
    
	if( FAILED(hr) )
        return false;

   
    m_pDevice->OMSetRenderTargets( 1, &m_pDefaultRTView, m_pDefaultDSView );

    D3D10_VIEWPORT vp;
    vp.Width = InitData.m_uiScreenX;
    vp.Height = InitData.m_uiScreenY;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pDevice->RSSetViewports( 1, &vp );

	m_pDefaultDepthStencilState = new EvoDepthStencilState();
	m_pDefaultBlendState = new EvoBlendState();

	return false;
}

void EvoDX10Renderer::Present()
{
	m_pSwapChain->Present(0, 0);
}

void EvoDX10Renderer::Clear()
{
	float afClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
    m_pDevice->ClearRenderTargetView( m_pDefaultRTView, afClearColor );

	m_pDevice->ClearDepthStencilView( m_pDefaultDSView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0 );
}

#endif
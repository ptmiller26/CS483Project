#include "EvoStdAfx.h"

EvoResourceManager *EvoResourceManager::ms_pInstance = NULL;

uint ComputeSize(uint uiWidth, uint uiHeight, uint uiMips, uint uiBytesPerPixel)
{
	if (uiMips == 0)
		uiMips = 1;

	uint uiSize = 0;

	while (uiMips != 0)
	{
		uiSize += uiWidth*uiHeight*uiBytesPerPixel;

		uiWidth = uiWidth >> 1;
		uiHeight = uiHeight >> 1;

		uiMips--;
	}

	return uiSize;
}

//--------------------------------------------------------------------------------------
// Return the BPP for a particular format
//--------------------------------------------------------------------------------------
uint BitsPerPixel( EvoFormat::EFormat eFormat )
{
	uint uiFmt = (uint)eFormat;

	if (uiFmt >= EvoFormat::R8G8B8A8_TYPELESS && uiFmt <= EvoFormat::R8G8B8A8_SINT)
		return 32;

	if (uiFmt >= EvoFormat::R32G32B32A32_TYPELESS && uiFmt <= EvoFormat::R32G32B32A32_SINT)
		return 128;

	// DXT1
	if (uiFmt >= EvoFormat::BC1_TYPELESS && uiFmt <= EvoFormat::BC1_UNORM_SRGB)
        return 4;

	// DXT2 - DXT5
	if (uiFmt >= EvoFormat::BC2_TYPELESS && uiFmt <= EvoFormat::BC5_SNORM)
        return  8;

	assert(0);
	return 0;
}

//--------------------------------------------------------------------------------------
// Get surface information for a particular format
//--------------------------------------------------------------------------------------
void GetSurfaceInfo( uint width, uint height, EvoFormat::EFormat eFormat, uint* pNumBytes, uint* pRowBytes, uint* pNumRows )
{
    uint numBytes = 0;
    uint rowBytes = 0;
    uint numRows  = 0;

    // From the DXSDK docs:
    //
    //     When computing DXTn compressed sizes for non-square textures, the 
    //     following formula should be used at each mipmap level:
    //
    //         max(1, width ÷ 4) x max(1, height ÷ 4) x 8(DXT1) or 16(DXT2-5)
    //
    //     The pitch for DXTn formats is different from what was returned in 
    //     Microsoft DirectX 7.0. It now refers the pitch of a row of blocks. 
    //     For example, if you have a width of 16, then you will have a pitch 
    //     of four blocks (4*8 for DXT1, 4*16 for DXT2-5.)"

    if( eFormat >= EvoFormat::BC1_TYPELESS && eFormat <= EvoFormat::BC5_SNORM )
    {
        // Note: we support width and/or height being 0 in order to compute
        // offsets in functions like CBufferLockEntry::CopyBLEToPerfectSizedBuffer().
        int numBlocksWide = 0;
        if( width > 0 )
            numBlocksWide = MAX( 1, width / 4 );
        int numBlocksHigh = 0;
        if( height > 0 )
            numBlocksHigh = MAX( 1, height / 4 );
        //int numBlocks = numBlocksWide * numBlocksHigh;
        int numBytesPerBlock = ( BitsPerPixel(eFormat) == 4 ? 8 : 16 );
        rowBytes = numBlocksWide * numBytesPerBlock;
        numRows = numBlocksHigh;
    }
    else
    {
        UINT bpp = BitsPerPixel( eFormat );
        rowBytes = ( width * bpp + 7) / 8; // round up to nearest byte
        numRows = height;
    }
    numBytes = rowBytes * numRows;
    if( pNumBytes != NULL )
        *pNumBytes = numBytes;
    if( pRowBytes != NULL )
        *pRowBytes = rowBytes;
    if( pNumRows != NULL )
        *pNumRows = numRows;
}

#if defined(EVO_RENDERER_DX9)
D3DFORMAT ToD3DFormat(EvoFormat::EFormat eFormat)
{
	switch( eFormat )
    {
		case EvoFormat::R32G32B32A32_FLOAT:
    	    return D3DFMT_A32B32G32R32F;
		case EvoFormat::R16G16B16A16_UNORM:
    		return D3DFMT_A16B16G16R16;
		case EvoFormat::R16G16B16A16_FLOAT:
    		return D3DFMT_A16B16G16R16F;
		case EvoFormat::R32G32_FLOAT:
			return D3DFMT_G32R32F;
		case EvoFormat::R8G8B8A8_UNORM:
    		return D3DFMT_A8R8G8B8;
		case EvoFormat::R16G16_UNORM:
    		return D3DFMT_G16R16;
		case EvoFormat::R16G16_FLOAT:
			return D3DFMT_G16R16F;
		case EvoFormat::R32_FLOAT:
			return D3DFMT_R32F;
		case EvoFormat::R16_FLOAT:
			return D3DFMT_R16F;
		case EvoFormat::A8_UNORM:
			return D3DFMT_A8;
    	case EvoFormat::R8_UNORM:
    	    return D3DFMT_P8;
    	case EvoFormat::BC1_UNORM:
    	    return D3DFMT_DXT1;
    	case EvoFormat::BC2_UNORM:
    	    return D3DFMT_DXT3;
    	case EvoFormat::BC3_UNORM_SRGB:
    	    return D3DFMT_DXT5;
		case EvoFormat::D24_UNORM_S8_UINT:
			return D3DFMT_D24S8;
		case EvoFormat::D32_FLOAT:
			return D3DFMT_D32;

    	default:
    	    assert(0); // unhandled format - Add it to the list!
    	    return (D3DFORMAT)0xffffffff;
    }
}
#endif



EvoResourceManager::EvoResourceManager()
{
	m_pStringMap = new stdext::hash_map<std::string, EvoTextureContainer *>;
}

EvoResourceManager::~EvoResourceManager()
{
	EvoTextureContainer *lpTP = NULL;

	while (!m_pStringMap->empty())
	{
		stdext::hash_map<std::string, EvoTextureContainer *>::iterator iter;
		iter = m_pStringMap->begin();
		lpTP = iter->second;
		SAFE_DELETE(lpTP);
		m_pStringMap->erase(iter);
	}

	SAFE_DELETE(m_pStringMap);
}

EvoResourceManager * EvoResourceManager::GetInstance()
{
	if (!ms_pInstance)
	{
		ms_pInstance = new EvoResourceManager();
	}

	return ms_pInstance;
}
/*
EvoNode *EvoResourceManager::LoadGrannyScene(const char *szFilename)
{
	EvoNode *pRootNode = new EvoNode();

	EvoGranny *pGranny = new EvoGranny();
	pGranny->Initialize(szFilename);

	pRootNode->SetRenderable(pGranny);

	return pRootNode;
}*/


#if defined(EVO_RENDERER_DX9)
EvoTexture *EvoResourceManager::CreateTexture(char *F, const char *lpTexPath, bool bStageTexture)
{
	EvoTexture *pTexture = new EvoTexture;

	EvoTextureContainer *lpTP = NULL;

	char lpFilename[512];
	if (lpTexPath)
		strcpy(lpFilename, lpTexPath);
	else lpFilename[0] = NULL;
	strcat(lpFilename, F);
	strupr(lpFilename);

	stdext::hash_map<std::string, EvoTextureContainer *>::iterator iter = m_pStringMap->find(lpFilename);

	if (iter != m_pStringMap->end())
	{
		lpTP = iter->second;
		// Texture Has Already been Loaded before, Return previous Instance instead of reloading
		pTexture->m_pTexture = lpTP->m_pTexture;
		int NewRef = lpTP->m_pTexture->AddRef();
		return pTexture;
	}
	else
	{
		// Check search paths
		int iSearchCnt = m_aSearchPaths[EvoSearchPath::TEXTURE].size();
		
		for (int i = iSearchCnt-1; i >= 0; i--)
		{
			strcpy(lpFilename,m_aSearchPaths[EvoSearchPath::TEXTURE][i]);
			strcat(lpFilename, F);
			strupr(lpFilename);
			stdext::hash_map<std::string, EvoTextureContainer *>::iterator iter = m_pStringMap->find(lpFilename);

			if (iter != m_pStringMap->end())
			{
				lpTP = iter->second;
				// Texture Has Already been Loaded before, Return previous Instance instead of reloading
				pTexture->m_pTexture = lpTP->m_pTexture;
				int NewRef = lpTP->m_pTexture->AddRef();
				return pTexture;
			}
		}
	}

	// Reset filename
	if (lpTexPath)
		strcpy(lpFilename, lpTexPath);
	else lpFilename[0] = NULL;
	strcat(lpFilename, F);
	strupr(lpFilename);

	D3DFORMAT SurfaceFormat;

	SurfaceFormat = D3DFMT_UNKNOWN; //D3DFMT_A8R8G8B8;  
	
	lpTP = new EvoTextureContainer;

	D3DXIMAGE_INFO ImageInfo;
	
	HRESULT hResult = D3DXGetImageInfoFromFile(lpFilename, &ImageInfo);

	if (FAILED(hResult))
	{	
		int iSearchCnt = m_aSearchPaths[EvoSearchPath::TEXTURE].size();

		for (int i = iSearchCnt-1; i >= 0 && FAILED(hResult); i--)
		{
			strcpy(lpFilename,m_aSearchPaths[EvoSearchPath::TEXTURE][i]);
			strcat(lpFilename, F);
			hResult = D3DXGetImageInfoFromFile(lpFilename, &ImageInfo);
		}
	}
	
	strcpy(lpTP->m_szTextureName, lpFilename);

	if (ImageInfo.ResourceType == D3DRTYPE_CUBETEXTURE)
	{
		IDirect3DCubeTexture9 *pCubeTex;
		hResult = D3DXCreateCubeTextureFromFileEx(EVOD3DDEVICE, lpFilename, D3DX_DEFAULT, 1/*D3DX_FROM_FILE*/, 0, SurfaceFormat, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &ImageInfo, NULL, &pCubeTex);
		if (FAILED(hResult))
		{
			delete pTexture;
			return NULL;
		}
		pCubeTex->QueryInterface(IID_IDirect3DBaseTexture9, (void**)&lpTP->m_pTexture);
		pCubeTex->Release();
	}
	else
	{
		IDirect3DTexture9 *pTex;
		hResult = D3DXCreateTextureFromFileEx(EVOD3DDEVICE, lpFilename, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_FROM_FILE, 0, SurfaceFormat, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &ImageInfo, NULL, &pTex);
		if (FAILED(hResult))
		{
			delete pTexture;
			return NULL;
		}
		pTex->QueryInterface(IID_IDirect3DBaseTexture9, (void**)&lpTP->m_pTexture);
		pTex->Release();
	}

	if (FAILED(hResult))
		{
			delete pTexture;
			return NULL;
		}

	pTexture->m_pTexture = lpTP->m_pTexture;

	strupr(lpTP->m_szTextureName);
	m_pStringMap->insert(std::pair <std::string, EvoTextureContainer *>(lpTP->m_szTextureName,lpTP));

	return pTexture;
}

EvoTexture *EvoResourceManager::CreateTexture(uint uiWidth, uint uiHeight, uint uiMipLevels, uint uiArraySize, EvoFormat::EFormat eFormat, EvoUsage::EUsage eUsage, EvoCpuAccess::ECpuAccess eAccess, EvoBind::EBind eBind, bool bAllowGenMips, void *pData)
{
	EvoTexture *pTexture = new EvoTexture;

	D3DFORMAT D3DFormat = ToD3DFormat(eFormat); 
	uint uiUsage = 0;
	D3DPOOL D3DPool = D3DPOOL_MANAGED;
	if (eUsage == EvoUsage::DYNAMIC)
		uiUsage |= D3DUSAGE_DYNAMIC;
	if (eBind & EvoBind::RENDER_TARGET)
	{
		uiUsage |= D3DUSAGE_RENDERTARGET;
		D3DPool = D3DPOOL_DEFAULT;
	}

	if (eBind & EvoBind::DEPTH_STENCIL)
	{
		uiUsage |= D3DUSAGE_DEPTHSTENCIL;
		D3DPool = D3DPOOL_DEFAULT;

		EVOD3DDEVICE->CreateDepthStencilSurface(uiWidth, uiHeight, D3DFormat, D3DMULTISAMPLE_NONE, 0, FALSE, &pTexture->m_pSurface, NULL);
	}
	else
	{
		IDirect3DTexture9 *pTex = NULL;
		EVOD3DDEVICE->CreateTexture(uiWidth, uiHeight, uiMipLevels, uiUsage, D3DFormat, D3DPool, &pTex, NULL);
		pTex->QueryInterface(IID_IDirect3DBaseTexture9, (void**)&pTexture->m_pTexture);
		pTex->Release();
	}

	if (pData)
	{
		uint uiSize = ComputeSize(uiWidth, uiHeight, uiMipLevels, BitsPerPixel(eFormat)/8);
		uint uiPitch;
		void *pPixels = pTexture->Lock(EvoLock::DISCARD, true, &uiPitch);
		memcpy(pPixels, pData, uiSize);
		pTexture->Unlock();
	}
	return pTexture;
}

void EvoResourceManager::DestroyTexture(EvoTexture *&pTex)
{
	int Flag = -1;
	EvoTextureContainer *lpTP = NULL;

	stdext::hash_map<std::string, EvoTextureContainer *>::iterator iter = m_pStringMap->begin();

	while (iter != m_pStringMap->end())
	{
		lpTP = iter->second;
		if (pTex->GetTexture() == lpTP->m_pTexture) {
			Flag = 1;
			break;
		}
		iter++;
	}
	
	if (Flag != -1) {
		ULONG RefCnt = 1;
		
		RefCnt = lpTP->m_pTexture->Release();	

		if(RefCnt == 0) {
			m_pStringMap->erase(iter);
			delete lpTP;
		}
	}

	SAFE_DELETE(pTex);
}

#endif

#if defined(EVO_RENDERER_DX10)
bool EvoResourceManager::CreateTexture(char *F, const char *lpTexPath, bool bStageTexture, EvoTexture *pTexture)
{
	EvoTextureContainer *lpTP = NULL;

	char lpFilename[512];
	if (lpTexPath)
		strcpy(lpFilename, lpTexPath);
	else lpFilename[0] = NULL;
	strcat(lpFilename, F);
	strupr(lpFilename);

	stdext::hash_map<std::string, EvoTextureContainer *>::iterator iter = m_pStringMap->find(lpFilename);

	if (iter != m_pStringMap->end())
	{
		lpTP = iter->second;
		// Texture Has Already been Loaded before, Return previous Instance instead of reloading
		pTexture->m_pTexture = lpTP->m_pTexture;
		int NewRef = lpTP->m_pTexture->AddRef();

		D3D10_TEXTURE2D_DESC LoadedTexDesc;
		pTexture->m_pTexture->GetDesc(&LoadedTexDesc);

		D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
		SRVDesc.Format = LoadedTexDesc.Format;
		if (LoadedTexDesc.MiscFlags & D3D10_RESOURCE_MISC_TEXTURECUBE)
		{
			SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
		}
		else
		{
			SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		}
		SRVDesc.Texture2D.MipLevels = LoadedTexDesc.MipLevels;
		EVOD3DDEVICE->CreateShaderResourceView(pTexture->m_pTexture, &SRVDesc, &pTexture->m_pRV);

		return true;
	}

	lpTP = new EvoTextureContainer;
	
	strcpy(lpTP->m_szTextureName, lpFilename);

	// HACK - For now, check for dds' first.  Should remove this though

	EvoString strDDS(lpFilename, EVOSTRING_COPY);
	strDDS.ReplaceExt("dds");

	D3DX10_IMAGE_LOAD_INFO LoadInfo;
    LoadInfo.Width = D3DX10_FROM_FILE;
    LoadInfo.Height  = D3DX10_FROM_FILE;
    LoadInfo.Depth  = D3DX10_FROM_FILE;
    LoadInfo.FirstMipLevel = 0;
    LoadInfo.MipLevels = D3DX10_FROM_FILE;
	if (bStageTexture)
	{
		LoadInfo.Usage = D3D10_USAGE_STAGING;
		LoadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
		LoadInfo.BindFlags = 0;
	}
	else
	{
		LoadInfo.Usage = D3D10_USAGE_DEFAULT;
		LoadInfo.CpuAccessFlags = 0;
		LoadInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	}

    LoadInfo.MiscFlags = 0;
	LoadInfo.Format = DXGI_FORMAT_FROM_FILE;//DXGI_FORMAT_UNKNOWN;
    LoadInfo.Filter = D3DX10_FILTER_NONE;
    LoadInfo.MipFilter = D3DX10_FILTER_NONE;
    LoadInfo.pSrcInfo = NULL;

	HRESULT hResult = D3DX10CreateTextureFromFile(EVOD3DDEVICE, strDDS.GetBuffer(), &LoadInfo, NULL, (ID3D10Resource**)&lpTP->m_pTexture, NULL);

	if (FAILED(hResult))
	{
		// Now try to load original filename
		assert(0);
		//hResult = D3DX10CreateTextureFromFile(EVOD3DDEVICE, lpFilename, &LoadInfo, NULL, (ID3D10Resource**)&lpTP->m_pTexture, NULL);
	}

	if (FAILED(hResult))
		return false;

	pTexture->m_pTexture = lpTP->m_pTexture;

	if (!bStageTexture)
	{
		D3D10_TEXTURE2D_DESC LoadedTexDesc;
		pTexture->m_pTexture->GetDesc(&LoadedTexDesc);

		D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
		SRVDesc.Format = LoadedTexDesc.Format;
		if (LoadedTexDesc.MiscFlags & D3D10_RESOURCE_MISC_TEXTURECUBE)
		{
			SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
		}
		else
		{
			SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		}
		SRVDesc.Texture2D.MipLevels = LoadedTexDesc.MipLevels;
		EVOD3DDEVICE->CreateShaderResourceView(pTexture->m_pTexture, &SRVDesc, &pTexture->m_pRV);

	}

	m_pStringMap->insert(std::pair <std::string, EvoTextureContainer *>(lpTP->m_szTextureName,lpTP));

	return true;
}

bool EvoResourceManager::CreateTexture(uint uiWidth, uint uiHeight, uint uiMipLevels, uint uiArraySize, EvoFormat::EFormat eFormat, EvoUsage::EUsage eUsage, EvoCpuAccess::ECpuAccess eAccess, EvoBind::EBind eBind, bool bAllowGenMips, void *pData, EvoTexture *pTexture)
{
	D3D10_TEXTURE2D_DESC TexDesc;
    TexDesc.Width =              uiWidth;
    TexDesc.Height =             uiHeight;
    TexDesc.MipLevels =          uiMipLevels;
    TexDesc.ArraySize =          uiArraySize;
    TexDesc.Format =             (DXGI_FORMAT)eFormat;
    TexDesc.SampleDesc.Count =   1;
    TexDesc.SampleDesc.Quality = 0;
    TexDesc.Usage =              (D3D10_USAGE)eUsage;
    TexDesc.BindFlags =          eBind;
    TexDesc.CPUAccessFlags =     eAccess;
    TexDesc.MiscFlags =          0;

	D3D10_SUBRESOURCE_DATA *pInitialData = new D3D10_SUBRESOURCE_DATA[uiMipLevels];

	BYTE* pSrcBits = (BYTE *)pData;
	int iWidth = (int)uiWidth;
	int iHeight = (int)uiHeight;

	for (uint ui = 0; ui < uiMipLevels; ui++)
	{
		uint uiNumBytes, uiRowBytes, uiNumRows;
		GetSurfaceInfo( iWidth, iHeight, eFormat, &uiNumBytes, &uiRowBytes, &uiNumRows );
        pInitialData[ui].pSysMem = (void*)pSrcBits;
        pInitialData[ui].SysMemPitch = uiRowBytes;

        pSrcBits += uiNumBytes;
        iWidth = iWidth >> 1;
        iHeight = iHeight >> 1;
        if( iWidth == 0 )
            iWidth = 1;
        if( iHeight == 0 )
            iHeight = 1;
	}

	EVOD3DDEVICE->CreateTexture2D(&TexDesc, pInitialData, &pTexture->m_pTexture);

	D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
    SRVDesc.Format = TexDesc.Format;
    SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = TexDesc.MipLevels;
	EVOD3DDEVICE->CreateShaderResourceView(pTexture->m_pTexture, &SRVDesc, &pTexture->m_pRV);

	return true;
}

void EvoResourceManager::DestroyTexture(ID3D10Texture2D *lpTexture, int Instances)
{
	int Flag = -1;
	EvoTextureContainer *lpTP = NULL;

	stdext::hash_map<std::string, EvoTextureContainer *>::iterator iter = m_pStringMap->begin();

	while (iter != m_pStringMap->end())
	{
		lpTP = iter->second;
		if (lpTexture == lpTP->m_pTexture) {
			Flag = 1;
			break;
		}
		iter++;
	}
	
	if (Flag != -1) {
		ULONG RefCnt = 1;
		
		RefCnt = lpTP->m_pTexture->Release();	

		if(RefCnt == 0) {
			m_pStringMap->erase(iter);
			delete lpTP;
		}
	}
}

#endif

void EvoResourceManager::DestroyTexture(char *lpName, int Instances)
{
	EvoTextureContainer *lpTP = NULL;

	stdext::hash_map<std::string, EvoTextureContainer *>::iterator iter = m_pStringMap->find(lpName);

	if (iter != m_pStringMap->end())
	{
		lpTP = iter->second;

		for (int i = 0; i < Instances; i++) {
			ULONG RefCnt = 1;
		
			RefCnt = lpTP->m_pTexture->Release();	

			if(RefCnt == 0) {
				m_pStringMap->erase(iter);
				delete lpTP;
			}
		}
	}
}

EvoShader *EvoResourceManager::CreateShader(const char *szFilename)
{
	EvoShader *pShader = NULL;
	EvoString ShaderString;

#if defined (EVO_RENDERER_DX9)
	ShaderString.Concat("Shaders\\DX9\\", false);
#endif

#if defined (EVO_RENDERER_DX10)
	ShaderString.Concat("Shaders\\", false);
#endif

	ShaderString.Concat((char*)szFilename, false);

	pShader = new EvoShader();
	pShader->Initialize(ShaderString.GetBuffer());

	m_aShaders.push_back(pShader);

	return pShader;
}

void EvoResourceManager::DestroyShader(EvoShader *pShader)
{
	for (uint ui = 0; ui < m_aShaders.size(); ui++)
	{
		if (m_aShaders[ui] == pShader)
		{
			m_aShaders.erase(m_aShaders.begin() + ui);
		}	
	}

	delete pShader;
}


void EvoResourceManager::AddSearchPath(EvoSearchPath::ESearchPath ePath, char *szPath)
{
	m_aSearchPaths[ePath].push_back(szPath);
}

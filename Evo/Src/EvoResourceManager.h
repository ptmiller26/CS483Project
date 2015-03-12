#ifndef _EVORESOURCEMANAGER_H_
#define _EVORESOURCEMANAGER_H_

namespace EvoSearchPath
{
	enum ESearchPath
	{
		TEXTURE,
		SHADER,
		NUM_TYPES
	};
};

#define RESMGR EvoResourceManager::GetInstance()

#define MAX_TEXTURE_NAME_LENGTH			256


struct EvoTextureContainer
{
	char m_szTextureName[MAX_TEXTURE_NAME_LENGTH];

#if defined(EVO_RENDERER_DX9)
	IDirect3DBaseTexture9 *m_pTexture;
#endif

#if defined(EVO_RENDERER_DX10)
	ID3D10Texture2D *m_pTexture;
#endif
};

class EvoResourceManager
{
   public:
	~EvoResourceManager();

	static EvoResourceManager *GetInstance();

	EvoNode *LoadGrannyScene(const char *szFilename);

	EvoShader *CreateShader(const char *szFilename);
	void DestroyShader(EvoShader *pShader);
	std::vector<EvoShader *> *GetShaders()				{ return &m_aShaders;	}


	EvoTexture *CreateTexture(char *szFilename, const char *szTexPath, bool bStageTexture);
	EvoTexture *CreateTexture(uint uiWidth, uint uiHeight, uint uiMipLevels, uint uiArraySize, EvoFormat::EFormat eFormat, EvoUsage::EUsage eUsage, EvoCpuAccess::ECpuAccess eAccess, EvoBind::EBind eBind, bool bAllowGenMips, void *pData);

#if defined(EVO_RENDERER_DX9)
	void DestroyTexture(EvoTexture *&pTex);
#endif
	
#if defined(EVO_RENDERER_DX10)
	void DestroyTexture(ID3D10Texture2D *pTexture, int Instances = 1);
#endif

	void DestroyTexture(char *lpName, int Instances = 1);

	void AddSearchPath(EvoSearchPath::ESearchPath ePath, char *szPath);

   protected:
    EvoResourceManager();

   protected:
	static EvoResourceManager *ms_pInstance;

	stdext::hash_map<std::string, EvoTextureContainer *> *m_pStringMap;

	std::vector<char *>	m_aSearchPaths[EvoSearchPath::NUM_TYPES];

	std::vector<EvoShader *> m_aShaders;
};

uint ComputeSize(uint uiWidth, uint uiHeight, uint uiMips, uint uiBytesPerPixel);
uint BitsPerPixel( EvoFormat::EFormat eFormat );
void GetSurfaceInfo( uint width, uint height, EvoFormat::EFormat eFormat, uint* pNumBytes, uint* pRowBytes, uint* pNumRows );

#endif
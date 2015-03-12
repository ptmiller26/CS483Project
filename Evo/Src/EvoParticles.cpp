#include "EvoStdAfx.h"

int EmitterSpr3dCreate (_t_frkPEmitter** pOutEmitter, char* sName, t_frkMatrix4* pLcsWcs, _t_frkPEmitterInfo* pEmitterInfo);
void SEmitterShaderCB(_t_frkPEmitter* pEmitter, float fTDelta);
int SEmitterEmitCB (_t_frkPEmitter** pEmitter, void** pAnmTrack, _t_frkPSystem** pSystem, char* sRefEmitterName, t_frkMatrix4* pLcsWcs, _t_frkPEmitterInfo* pInfo);
void EmitterSpr3dSubmitCB (_t_frkPEmitter* pEmitter, _t_frkParticle* pParticle);
void EmitSprSetRStateCB (_t_frkPEmitter* pEmitter, int bEnd);
void EmitterSpr3dProcessCB (_t_frkPEmitter* pEmitter, float fTDelta);
//void EmitSprSetBlendMode (int nBlendMode);

EvoParticles *g_pCurrentParticles = NULL;



EvoParticles::EvoParticles()
{
	m_pFrkPSys = NULL;
}

EvoParticles::~EvoParticles()
{

}

void EvoParticles::Initialize(char *szFilename, bool bLoop)
{
	nMaxSpr = 10000;
	nNumSpr = 0;
	nNumVerts = 0;
	nNumIndices = 0;
	nVertSize = sizeof(ParticleVert);
	nIBSize = nMaxSpr * 6 * sizeof (WORD);
	nVBSize = nMaxSpr * 4 * nVertSize;

	bVBLock = false;

	static int s_iHack = 0;
	char szTmp[16];
	EvoString strName;
	strName.Copy("PSys:", false);
	strName.Concat(itoa(s_iHack, szTmp, 10), false);
	s_iHack++;

	frkPSystemCreate(&m_pFrkPSys, strName.GetBuffer());

	m_mtxTransform.SetIdentity();

	EvoMatrix mtx;
	mtx.SetIdentity();

	t_frkMatrix4 *pLcsWcs = (t_frkMatrix4*)&mtx;

	/************************************************************************/
	//	Load the emitter description (PSB)
	/************************************************************************/
	_t_frkPEmitterInfo* pEDesc = LoadEmitterInfo(szFilename);
	GetCustomData(pEDesc);
	EmitterSpr3dCreate(&m_pEmitter, szFilename, pLcsWcs, pEDesc);

	frkPEmitterSetUserData (m_pEmitter, this);


	/************************************************************************/
	//	Setup if emitter is super emitter (it has a reference emitter associated)
	/************************************************************************/
	_t_frkAssetInfo* pRefAsset = NULL;
	frkPPropertyGetRefAsset (pEDesc, 0, &pRefAsset);
	if (frkPPropertyGetNumRefAssets(pEDesc) > 0 && 
		frkPAssetGetDataPointer(pRefAsset) == NULL)										
	{
		_t_frkPEmitter* pRefEmitter = NULL;
		_t_frkPEmitterInfo* pRefDesc = LoadEmitterInfo (frkPAssetGetKey(pRefAsset));
		EmitterSpr3dCreate(&pRefEmitter, "SEmitter1", NULL, pRefDesc);	
		frkPEmitterSetUserData (pRefEmitter, this);
		frkPAssetSetDataPointer(pRefAsset, (void*)pRefEmitter);		
		frkPEmitterSetCallback (m_pEmitter, ePSYSTEM_CB_SUPEREMITCB, SEmitterEmitCB);
	}																								
	/************************************************************************/
	//	Setup for an emitter that has an animation track associated
	/************************************************************************/
	_t_frkAnmTrk* pTrk = frkPPropertyGetAnmTrk (frkPEmitterGetPropertyInfo (m_pEmitter));		
	if(pTrk)												
	{																							
		m_pAnimationInstance = (_t_frkAnmInst*) malloc (frkPAnmGetInstMemSize ());				
		frkPAnmInit (m_pAnimationInstance, m_pEmitter);											
		frkPAnmSet (m_pAnimationInstance, pTrk);									
		frkPAnmSetLoop (m_pAnimationInstance, bLoop);												
	}																							
	/************************************************************************/
	//	Setup for an emitter that has a pixel shader associated
	/************************************************************************/
	int nShaderAssets = frkPPropertyGetNumShaderAssets (pEDesc);								
	if(nShaderAssets > 0)																			
	{																								
		_t_frkAssetInfo* pShaderAsset;																
		frkPPropertyGetShaderAsset (pEDesc, 0, &pShaderAsset);								
		if( pShaderAsset )																			
		{																							
			char* sShaderName = frkPAssetGetKey(pShaderAsset);											
//			graphicsHandler->LoadPixelShader(sShaderName);
			//m_pShader = RESMGR->CreateShader(sShaderName);
			
			frkPEmitterSetCallback (m_pEmitter, ePSYSTEM_CB_EPROCESS, SEmitterShaderCB);
		}																							
	}

	m_pShader = RESMGR->CreateShader("Particles.fx");
	m_pMaterial = new EvoMaterial();
	m_pMaterial->SetShader(m_pShader);

	/************************************************************************/
	//	Setup for an emitter that has Custom Data associated
	/************************************************************************/
//   	void* customData = NULL;
//   	customData = frkPPropertyGetCustomData(pEDesc);
//   	if(customData)
//   		getCustomData(pEDesc);

	/************************************************************************/
	//	Loading Forces From a File.
	/************************************************************************/
//	loadForcesFromFile("../Resources/forces.fcb");
	
	/************************************************************************/
	//	Register the emitter with the particle system
	/************************************************************************/
	frkPEmitterReg (m_pFrkPSys, m_pEmitter);

	// Uncomment for Live Tuner
	//frkTunerRegEmitter (szFilename, m_pEmitter );

	m_pVB = EVO->CreateBuffer(nVBSize, nVertSize, NULL, EvoBind::VERTEX_BUFFER, EvoUsage::DYNAMIC, EvoCpuAccess::WRITE, false, false);
	m_pIB = EVO->CreateBuffer(nIBSize, 2, NULL, EvoBind::INDEX_BUFFER, EvoUsage::DYNAMIC, EvoCpuAccess::WRITE, false, false);

	EvoInputElements InputElements;
	InputElements.AddElement(EvoInputElementUsage::POSITION, 0, EvoFormat::R32G32B32_FLOAT, 0, 0, false, 0);
	InputElements.AddElement(EvoInputElementUsage::COLOR, 0, EvoFormat::R8G8B8A8_UNORM, 0, 12, false, 0);
	InputElements.AddElement(EvoInputElementUsage::TEXCOORD, 0, EvoFormat::R32G32_FLOAT, 0, 16, false, 0);

	m_hInputLayout = EVO->GetInputLayoutMgr()->RegisterInputElements(InputElements);
}
/*
void EvoParticles::SetPosition(EvoPoint3 pt3)
{
	EvoMatrix m;
	m.SetTranslate(pt3);

	frkPEmitterSetMatrix (m_pEmitter, (t_frkMatrix4*) &m);
}*/

void EvoParticles::SetMatrix(EvoMatrix *pMtx)
{
	EvoMatrix m = *((EvoMatrix *)&m_CustomData.MatXForm);
	m.Multiply(pMtx);
	frkPEmitterSetMatrix (m_pEmitter, (t_frkMatrix4*) &m);
}

void EvoParticles::Update(float fDeltaT)
{
	g_pCurrentParticles = this;

	if (m_pAnimationInstance)
	{
		frkPAnmProcess (m_pAnimationInstance, fDeltaT);
	}

	frkPSystemProcess (m_pFrkPSys, fDeltaT);

	g_pCurrentParticles = NULL;
}

_t_frkPEmitterInfo* EvoParticles::LoadEmitterInfo(char* szFilename)
{
	// Load emitter description from disc if not found in Data Server Database
	_t_frkPEmitterInfo* pEDesc = NULL;//(_t_frkPEmitterInfo*)DataServerSearchEmitterDescs(szFilename);
	if (pEDesc)
		return pEDesc;
	
	// Load the PSB file and prepare it
	EvoFile *pFile = EvoFile::OpenFile(szFilename, EvoFile::READ);
	int len = pFile->GetSize();
	void* buf = malloc (len);
	pFile->ReadBytes(buf, len);
	pEDesc = (_t_frkPEmitterInfo*)buf;
	EvoFile::CloseFile(pFile);
	frkPSBPrepare(pEDesc);

	// Load associated assets
	LoadEmitterAssets(pEDesc);
	
	// Load associated animation assets
	_t_frkAnmTrk* pTrk = frkPPropertyGetAnmTrk (pEDesc);	
	if (pTrk)
	{
		LoadEmitterAnimationAssets(pTrk);
	}
	
	// Load reference emitter for a super emitter description
	int nRefAssets = frkPPropertyGetNumRefAssets (pEDesc);										
	if (nRefAssets > 0 )																				
	{																								
		_t_frkAssetInfo* pRefAsset = NULL;
		frkPPropertyGetRefAsset (pEDesc, 0, &pRefAsset);
		_t_frkPEmitterInfo* pRefDesc = NULL;//(_t_frkPEmitterInfo*)DataServerSearchEmitterDescs(frkPAssetGetKey(pRefAsset));
		if (pRefDesc == NULL)
		{
			LoadEmitterInfo(frkPAssetGetKey(pRefAsset));
		}
	}

	return pEDesc;
}
/******************************************************************************
Function	:	LoadAnimationAssets
Description	:	Loads the animation assets associated with an emitter.
Arguments	:	>animationTrack		: A pointer to the animation track associated
with the emitter. 
Return		:	None.
/*****************************************************************************/
void EvoParticles::LoadEmitterAnimationAssets(_t_frkAnmTrk* animationTrack)
{
	_t_frkPEmitterInfo* pInfo = NULL;

	frkPAnmResetTrkKeyTracer ();
	while( (pInfo = frkPAnmGetTrkKeyTracer (animationTrack)))
	{
		LoadEmitterAssets(pInfo);
		frkPAnmIncTrkKeyTracer ();
	}
}
/******************************************************************************
Function	:	LoadEmitterAssets
Description	:	Loads the Emitter assets associated with an emitter.
Arguments	:	[IN]emitterInfo	: A pointer to the Emitter Info structure.
Return		:	None.
/*****************************************************************************/
void EvoParticles::LoadEmitterAssets(_t_frkPEmitterInfo* emitterInfo)
{
	_t_frkAssetInfo* pAsset = NULL;
	int nType = 0;
	char* sKey = "";

	int nCount = frkPPropertyGetNumAssets(emitterInfo);

	for(int i = 0; i < nCount; i++)
	{
		frkPPropertyGetAsset(emitterInfo, i, &pAsset);
		if(pAsset)
		{
			nType = frkPAssetGetType(pAsset);
			sKey = frkPAssetGetKey(pAsset);
			if(nType == ePSYSTEM_ASSETTYPE_TEXTURE)
			{
				frkPAssetSetDataPointer(pAsset, RESMGR->CreateTexture(sKey, NULL, false)/*DataServerLoadTexture(sKey)*/);
			}
			else if(nType == ePSYSTEM_ASSETTYPE_MODEL)
			{
				frkPAssetSetDataPointer(pAsset, RESMGR->LoadGrannyScene(sKey)/*DataServerLoadModel(sKey)*/);
			}
		}
	}
}

int EmitterSpr3dCreate (_t_frkPEmitter** pOutEmitter, char* sName, t_frkMatrix4* pLcsWcs, _t_frkPEmitterInfo* pEmitterInfo)
{
	int nStatus = frkPEmitterCreate (pOutEmitter, sName, pLcsWcs, pEmitterInfo);
	if (nStatus == 0) {
		frkPEmitterSetCallback (*pOutEmitter, ePSYSTEM_CB_SUBMIT, EmitterSpr3dSubmitCB);
		frkPEmitterSetCallback (*pOutEmitter, ePSYSTEM_CB_EPROCESS, EmitterSpr3dProcessCB);
		//frkPEmitterSetCallback (*pOutEmitter, ePSYSTEM_CB_ERSTATE, EmitSprSetRStateCB);
	}
	return nStatus;
}

int SEmitterEmitCB (_t_frkPEmitter** pEmitter, void** pAnmTrack, _t_frkPSystem** pSystem, char* sRefEmitterName, t_frkMatrix4* pLcsWcs, _t_frkPEmitterInfo* pInfo) 
{
	int nRet = EmitterSpr3dCreate (pEmitter, "EEmitter", NULL, pInfo);
	if( nRet < 0)
		return -1;

	frkPEmitterSetUserData (*pEmitter, g_pCurrentParticles);

	*pSystem = g_pCurrentParticles->GetPSys();
	return 0;
}

/******************************************************************************
Function	:	SEmitterShaderCB
Description	:	Callback for Shader.
Arguments	:	
>pEmitter	:	[IN]A pointer to an Emitter
>fTDelta	:	[IN]Time Difference from Last Frame.
Return		:	None.
/*****************************************************************************/
void SEmitterShaderCB(_t_frkPEmitter* pEmitter, float fTDelta)
{
 	//graphicsHandler->ShaderPreRender ();
 	EmitterSpr3dProcessCB(pEmitter, fTDelta);
	//graphicsHandler->ShaderPostRender ();
}

void EmitterSpr3dSubmitCB (_t_frkPEmitter* pEmitter, _t_frkParticle* pParticle)
{
	EvoParticles *pEvoParticles = NULL;
	frkPEmitterGetUserData (pEmitter, (void**)&pEvoParticles);

	static void* pAssetCache = NULL;

	//LPDIRECT3DDEVICE9 pDevice = GHGetD3DDevice();
	_t_frkPEmitterInfo* pInfo = (_t_frkPEmitterInfo*)(*(frkPEmitterInfoPtr(pEmitter)));
	_t_frkAssetInfo* pAsset = (_t_frkAssetInfo*)((frkPPropertyCurAssetPtr (pInfo)));
	void* pAssetTex = NULL;

	// Flip Book Texture Animation
	if (*(frkPPropertyNumAssetsPtr (pInfo)) &&
		*(frkPPropertyFlagPtr(pInfo)) & (frkPSYSTEM_PROPERTYBIT_PANIMRANDOM | frkPSYSTEM_PROPERTYBIT_PANIMFLIPBOOK)) {

			pAsset = (_t_frkAssetInfo*)(frkPEmitterFlipBookAssetPtr (pEmitter, pInfo));

			pEvoParticles->GetMaterial()->SetTexture(0, (EvoTexture *)frkPAssetGetDataPointer(frkPPropertyGetCurAsset(pInfo)));
			//EVO->BindToPS(0, (EvoTexture *)frkPAssetGetDataPointer(frkPPropertyGetCurAsset(pInfo)));
			//GHGetD3DDevice()->SetTexture(0, (LPDIRECT3DTEXTURE9)frkPAssetGetDataPointer(frkPPropertyGetCurAsset(pInfo)));

			if(pAsset != pAssetCache)
			{
				//pDevice->SetRenderState (D3DRS_ZWRITEENABLE, FALSE);
//				EmitSprSetBlendMode ( *(frkPPropertyBlendPtr(pInfo)));
				//SprSystemProcess (pSprSystem);
				pEvoParticles->Render();
				//pDevice->SetRenderState (D3DRS_ZWRITEENABLE, TRUE);
				pAssetCache = pAsset;
			}
		}
	// Particle Radial Facing
	if (*(frkPPropertyFlagPtr(pInfo)) & frkPSYSTEM_PROPERTYBIT_FACINGERADIAL) {
		t_frkMatrix4* pLcsWcs = frkPEmitterMatrixPtr (pEmitter);
		EvoPoint3 VUp, VRight, VRot;
		EvoPoint3* pParticlePos = (EvoPoint3*)frkParticlePosPtr (pParticle);
		EvoPoint3* pEmitterPos = (EvoPoint3*)&pLcsWcs->m30;
		VUp = EvoPoint3 (1.0f, 0.0f, 0.0f);
		D3DXMATRIX LookMat;
		D3DXMatrixLookAtRH (&LookMat, (D3DXVECTOR3*)pParticlePos, (D3DXVECTOR3*)pEmitterPos, (D3DXVECTOR3*)&VUp);
		VRot = EvoPoint3 (LookMat._13, LookMat._23, LookMat._33);
		VRight = EvoPoint3 (LookMat._11, LookMat._21, LookMat._31);
		VUp = EvoPoint3 (LookMat._12, LookMat._22, LookMat._32);
		pEvoParticles->SprSetQuadAxis (&VUp, &VRight, &VRot);
	}
	
	if(*(frkPPropertyFlagPtr(pInfo)) & frkPSYSTEM_PROPERTYBIT_FACINGAXIALVEL)
	{
		EvoPoint3 VUp, VRight, VRot;
		D3DMATRIX WcsVcs;
		WcsVcs = *((D3DMATRIX*)EVO->GetTransform(EVO_TRANSFORM_VIEW));
		VRot = EvoPoint3 (WcsVcs._13, WcsVcs._23, WcsVcs._33);
		VRight = EvoPoint3 (WcsVcs._11, WcsVcs._21, WcsVcs._31);
		VUp = EvoPoint3 (frkParticleVelPtr(pParticle)->x, frkParticleVelPtr(pParticle)->y, frkParticleVelPtr(pParticle)->z);
		//D3DXVec3Normalize(&VUp, &VUp);
		VUp.Unitize();
		D3DXVec3Cross((D3DXVECTOR3*)&VRight, (D3DXVECTOR3*)&VUp, (D3DXVECTOR3*)&VRot);
		pEvoParticles->SprSetQuadAxis (&VUp, &VRight, &VRot);
	}

	if (*(frkPPropertyFlagPtr(pInfo)) & frkPSYSTEM_PROPERTYBIT_SHAPERECT)
		pEvoParticles->SetShapeIsRect(true);
	else
		pEvoParticles->SetShapeIsRect(false);

	pEvoParticles->SprSubmit ((D3DXVECTOR4*)frkParticlePosPtr (pParticle), 
		D3DXToRadian (*((float*)frkPPropertyRotPtr(pInfo))), *(frkParticleRotPtr(pParticle)), 
		(D3DXVECTOR2*)frkPPropertyScalePtr(pInfo), *(frkParticleScalePtr(pParticle)), 
		(D3DXVECTOR4*)frkPPropertyTintPtr(pInfo), (D3DXVECTOR4*)frkParticleColorPtr (pParticle),
		(D3DXVECTOR4*)frkPAssetGetUVs (pAsset));
}

/******************************************************************************
Function	:	EmitSprSetRStateCB
Description:	TODO
Arguments	:	TODO
Return		:	TODO
/*****************************************************************************/

void EmitSprSetRStateCB (_t_frkPEmitter* pEmitter, int bEnd)
{
	EvoParticles *pEvoParticles = NULL;
	frkPEmitterGetUserData (pEmitter, (void**)&pEvoParticles);

//	LPDIRECT3DDEVICE9 pDevice = GHGetD3DDevice();
	D3DXVECTOR3 VUp, VRight, VRot;
	_t_frkPEmitterInfo* pInfo = (_t_frkPEmitterInfo*)(*(frkPEmitterInfoPtr(pEmitter)));
	t_frkMatrix4* pLcsWcs = frkPEmitterMatrixPtr (pEmitter);
	unsigned int nPBits = *(frkPPropertyFlagPtr(pInfo));

	pEvoParticles->Render();

	if (bEnd) {
		//EmitSprSetBlendMode (frkPSYSTEM_EMITTERBLENDMODE_NORMAL);
//		pDevice->SetRenderState (D3DRS_ZWRITEENABLE, TRUE);
		return;
	}
	if ( nPBits & frkPSYSTEM_PROPERTYBIT_FACINGWAXISX) {
		VRot = D3DXVECTOR3 (1.0f, 0.0f, 0.0f);
		VRight = D3DXVECTOR3 (0.0f, 1.0f, 0.0f);
		VUp = D3DXVECTOR3 (0.0f, 0.0f, 1.0f);
	}
	else
	if ( nPBits & frkPSYSTEM_PROPERTYBIT_FACINGWAXISY) {
		VRot = D3DXVECTOR3 (0.0f, 1.0f, 0.0f);
		VRight = D3DXVECTOR3 (1.0f, 0.0f, 0.0f);
		VUp = D3DXVECTOR3 (0.0f, 0.0f, 1.0f);
	}
	else
	if ( nPBits & frkPSYSTEM_PROPERTYBIT_FACINGWAXISZ) {
		VRot = D3DXVECTOR3 (0.0f, 0.0f, 1.0f);
		VRight = D3DXVECTOR3 (1.0f, 0.0f, 0.0f);
		VUp = D3DXVECTOR3 (0.0f, 1.0f, 0.0f);
	}
	else
	if ( nPBits & frkPSYSTEM_PROPERTYBIT_FACINGEMITTER) {
		VRight = D3DXVECTOR3 (pLcsWcs->m20, pLcsWcs->m21, pLcsWcs->m22);
		VUp = D3DXVECTOR3 (pLcsWcs->m00, pLcsWcs->m01, pLcsWcs->m02);
		VRot = D3DXVECTOR3 (pLcsWcs->m10, pLcsWcs->m11, pLcsWcs->m12);
	}
	else
	if ( nPBits & frkPSYSTEM_PROPERTYBIT_FACINGAXIALX) {		
		D3DMATRIX WcsVcs;
		WcsVcs = *((D3DMATRIX*)EVO->GetTransform(EVO_TRANSFORM_VIEW));
		VRot = D3DXVECTOR3 (WcsVcs._13, WcsVcs._23, WcsVcs._33);
		VUp = D3DXVECTOR3 (pLcsWcs->m00, pLcsWcs->m01, pLcsWcs->m02);
		D3DXVec3Cross(&VRight, &VUp, &VRot);
		pEvoParticles->SprSetQuadAxis ((EvoPoint3*)&VUp, (EvoPoint3*)&VRight, (EvoPoint3*)&VRot);
	}
	else
	if ( nPBits & frkPSYSTEM_PROPERTYBIT_FACINGAXIALY) {
		D3DMATRIX WcsVcs;
		WcsVcs = *((D3DMATRIX*)EVO->GetTransform(EVO_TRANSFORM_VIEW));
		//g_pDevice->GetTransform(D3DTS_VIEW, &WcsVcs);
		VRot = D3DXVECTOR3 (WcsVcs._13, WcsVcs._23, WcsVcs._33);
		VUp = D3DXVECTOR3 (pLcsWcs->m10, pLcsWcs->m11, pLcsWcs->m12);
		D3DXVec3Cross(&VRight, &VUp, &VRot);
		pEvoParticles->SprSetQuadAxis ((EvoPoint3*)&VUp, (EvoPoint3*)&VRight, (EvoPoint3*)&VRot);
	}
	else
	if ( nPBits & frkPSYSTEM_PROPERTYBIT_FACINGAXIALZ) {
		D3DMATRIX WcsVcs;
		WcsVcs = *((D3DMATRIX*)EVO->GetTransform(EVO_TRANSFORM_VIEW));
		VRot = D3DXVECTOR3 (WcsVcs._13, WcsVcs._23, WcsVcs._33);
		VUp = D3DXVECTOR3 (pLcsWcs->m20, pLcsWcs->m21, pLcsWcs->m22);
		D3DXVec3Cross(&VRight, &VUp, &VRot);
		pEvoParticles->SprSetQuadAxis ((EvoPoint3*)&VUp, (EvoPoint3*)&VRight, (EvoPoint3*)&VRot);
	}
	else {
		D3DMATRIX WcsVcs;
		D3DMATRIX* pWcsVcs = &WcsVcs;
		//pDevice->GetTransform (D3DTS_VIEW, pWcsVcs);
		WcsVcs = *((D3DMATRIX*)EVO->GetTransform(EVO_TRANSFORM_VIEW));
		VRot = D3DXVECTOR3 (pWcsVcs->_13, pWcsVcs->_23, pWcsVcs->_33);
		VRight = D3DXVECTOR3 (pWcsVcs->_11, pWcsVcs->_21, pWcsVcs->_31);
		VUp = D3DXVECTOR3 (pWcsVcs->_12, pWcsVcs->_22, pWcsVcs->_32);
	}
	pEvoParticles->SprSetQuadAxis ((EvoPoint3*)&VUp, (EvoPoint3*)&VRight, (EvoPoint3*)&VRot);
//	pDevice->SetRenderState (D3DRS_ZWRITEENABLE, FALSE);
//	EmitSprSetBlendMode ( *(frkPPropertyBlendPtr(pInfo)));

//	_t_frkAssetInfo* pAsset = (_t_frkAssetInfo*)(frkPPropertyCurAssetPtr (pInfo));
//	GHGetD3DDevice()->SetTexture(0, (LPDIRECT3DTEXTURE9)frkPAssetGetDataPointer(frkPPropertyGetCurAsset(pInfo)));
	//EVO->BindToPS(0, (EvoTexture *)frkPAssetGetDataPointer(frkPPropertyGetCurAsset(pInfo)));
	//pEvoParticles->GetMaterial()->SetTexture(0, (EvoTexture *)frkPAssetGetDataPointer(frkPPropertyGetCurAsset(pInfo)));

}

/******************************************************************************
Function	:	EmitterSpr3dProcessCB
Description	:	TODO
Arguments	:	TODO
Return		:	TODO
/*****************************************************************************/
void EmitterSpr3dProcessCB (_t_frkPEmitter* pEmitter, float fTDelta)
{
	EvoParticles *pEvoParticles = NULL;
	frkPEmitterGetUserData (pEmitter, (void**)&pEvoParticles);

	_t_frkPEmitterInfo* pInfo = (_t_frkPEmitterInfo*)(*(frkPEmitterInfoPtr(pEmitter)));
	_t_frkAssetInfo* pAsset = (_t_frkAssetInfo*)(frkPPropertyCurAssetPtr (pInfo));

	pEvoParticles->GetMaterial()->SetTexture(0, (EvoTexture *)frkPAssetGetDataPointer(frkPPropertyGetCurAsset(pInfo)));

	EmitSprSetRStateCB (pEmitter, 0);
	frkPEmitterProcess (pEmitter, fTDelta);
	pEvoParticles->Render ();
	//EmitSprSetBlendMode (frkPSYSTEM_EMITTERBLENDMODE_NORMAL);
}

/******************************************************************************
Function	:	EmitSprSetBlendMode
Description:	TODO
Arguments	:	TODO
Return		:	TODO
/*****************************************************************************/
void EmitSprSetBlendMode (int nBlendMode)
{
	/*
	static int nBlendModeCache = frkPSYSTEM_EMITTERBLENDMODE_NORMAL;

	if (nBlendMode == nBlendModeCache)
		return;

	nBlendModeCache = nBlendMode;

//	LPDIRECT3DDEVICE9 pDevice = GHGetD3DDevice();
	switch (nBlendMode) {
		case frkPSYSTEM_EMITTERBLENDMODE_NORMAL:
			pDevice->SetRenderState (D3DRS_BLENDOP, D3DBLENDOP_ADD);
			pDevice->SetRenderState (D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState (D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			break;
		case frkPSYSTEM_EMITTERBLENDMODE_EMISSIVE:
			pDevice->SetRenderState (D3DRS_BLENDOP, D3DBLENDOP_ADD);
			pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
			pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		case frkPSYSTEM_EMITTERBLENDMODE_SUBTRACTIVE:
			pDevice->SetRenderState (D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
			pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			break;
		case frkPSYSTEM_EMITTERBLENDMODE_DARKMAP:
			pDevice->SetRenderState (D3DRS_BLENDOP, D3DBLENDOP_ADD);
			pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
			break;
		case frkPSYSTEM_EMITTERBLENDMODE_LIGHTMAP:
			pDevice->SetRenderState (D3DRS_BLENDOP, D3DBLENDOP_ADD);
			pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
			pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
			break;
	}
	*/
}

void EvoParticles::SprSetQuadAxis (EvoPoint3* pUp, EvoPoint3* pRight, EvoPoint3* pRot)
{
	m_AxisUpV = *((EvoPoint3*)pUp);
	m_AxisRightV = *((EvoPoint3*)pRight);
	m_AxisRot = *((EvoPoint3*)pRot);
}

int EvoParticles::SprSubmit (D3DXVECTOR4* pPos, float fRotMin, float fRot, D3DXVECTOR2* pScale, float fScale, D3DXVECTOR4* pTint, D3DXVECTOR4* pColor, D3DXVECTOR4* pUVs)
{
	if (nNumSpr >= nMaxSpr)
		return -1;

	if (!bVBLock)
	{
		pVertices = (ParticleVert*)m_pVB->Lock(EvoLock::DISCARD, false);
		pIndices = (WORD *)m_pIB->Lock(EvoLock::DISCARD, false);
		bVBLock = true;
	}

	D3DXMATRIX RotMat;
	D3DXMatrixRotationAxis( &RotMat, (D3DXVECTOR3 *)&m_AxisRot, fRot + fRotMin);

	D3DCOLOR DxColor;
	D3DXVECTOR4 V0, V1, V2, V3;

	D3DXVECTOR4 VRight, VUp;
	D3DXVec3Transform (&VRight, (D3DXVECTOR3*)&m_AxisRightV, &RotMat);
	D3DXVec3Transform (&VUp, (D3DXVECTOR3*)&m_AxisUpV, &RotMat);

	VRight *= (fScale * 0.5f * pScale->x);
	VUp *= (fScale * 0.5f * pScale->y);

	DxColor = D3DCOLOR_COLORVALUE (pColor->x*pTint->x, pColor->y*pTint->y, pColor->z*pTint->z, pColor->w*pTint->w);

	if(m_bIsShapeRect){
		// For Rectangle Shaped Particles
		V0 = (0.7071f * (-VRight + VUp)) + *pPos;
		V1 = (0.7071f * (-VRight - VUp)) + *pPos;
		V2 = (0.7071f * (VRight + VUp)) + *pPos;
		V3 = (0.7071f * (VRight - VUp)) + *pPos;
	}
	else
	{
		// For Diamond Shaped Particles
		V0 = -VRight + *pPos;
		V1 = VUp + *pPos;
		V2 = -VUp + *pPos;
		V3 = VRight + *pPos;
	}
	*pIndices = nNumVerts;
	pIndices++;
	nNumIndices++;

	*pIndices = nNumVerts;
	pIndices++;
	nNumIndices++;
	pVertices->Pos = (D3DXVECTOR3)V0;
	pVertices->Color = DxColor;
	pVertices->UV.x = pUVs->x;
	pVertices->UV.y = pUVs->y;
	pVertices++;
	nNumVerts++;

	*pIndices = nNumVerts;
	pIndices++;
	nNumIndices++;
	pVertices->Pos = (D3DXVECTOR3)V1;
	pVertices->Color = DxColor;
	pVertices->UV.x = pUVs->x;
	pVertices->UV.y = pUVs->w;
	pVertices++;
	nNumVerts++;

	*pIndices = nNumVerts;
	pIndices++;
	nNumIndices++;
	pVertices->Pos = (D3DXVECTOR3)V2;
	pVertices->Color = DxColor;
	pVertices->UV.x = pUVs->z;
	pVertices->UV.y = pUVs->y;
	pVertices++;
	nNumVerts++;

	*pIndices = nNumVerts;
	pIndices++;
	nNumIndices++;
	pVertices->Pos = (D3DXVECTOR3)V3;
	pVertices->Color = DxColor;
	pVertices->UV.x = pUVs->z;
	pVertices->UV.y = pUVs->w;
	pVertices++;
	nNumVerts++;

	// Repeat last vert twice for strip continuation.
	*pIndices = nNumVerts-1;
	pIndices++;
	nNumIndices++;

	nNumSpr++;
	return 0;
}

/******************************************************************************
Function	:	SprSystemProcess
Description	:	Processes the Sprite System
Arguments	:	[IN]Pointer to Sprite to be Processed.
Return		:	None.
/*****************************************************************************/
void EvoParticles::Render ()
{
	if (bVBLock) {
		m_pVB->Unlock();
		m_pIB->Unlock();
		bVBLock = false;
	}
	if (!nNumSpr)
		return;

	EVO->SetTransform(EVO_TRANSFORM_WORLD, &m_mtxTransform);

	m_pShader->SetTechnique("TAdditive");

	uint uiNumShaderPasses = 0;
	m_pShader->Begin(&uiNumShaderPasses);

	EVO->SetInputLayout(m_hInputLayout, m_pShader, m_pShader->GetCurrentTechniqueName());

	EVO->SetVB(m_pVB, 0, m_pVB->GetStride(), 0);
	EVO->SetIB(m_pIB, m_pIB->GetStride(), 0);
	EVO->SetPrimitiveType(EvoPrimitive::TRIANGLESTRIP);

	for (uint ui = 0; ui < uiNumShaderPasses; ui++)
	{
		m_pShader->BeginPass(ui);

		m_pMaterial->BindTextures();
		EVO->DrawIndexed(nNumVerts, nNumIndices, 0, 0);
		m_pMaterial->UnbindTextures();
		m_pShader->EndPass();
	}

	m_pShader->End();

	nNumSpr = 0;
	nNumVerts = 0;
	nNumIndices = 0;
}

void EvoParticles::GetCustomData(_t_frkPEmitterInfo* pEDesc)
{
	char* key = "";
	int type = 0;
	int dataSize = 0;
	int version = 0;
	void* customData = NULL;
//	ExportData* data = NULL;
	EmitterCustomData* pCustomData = NULL;

	_t_frkPChunk* pChunk = NULL;
	frkPCustomDataResetChunkTracer (pEDesc); 

	while(2>1)
	{
		pChunk = frkPCustomDataGetTracerChunk (pEDesc);
		frkPCustomDataIncChunkTracer();
		if(pChunk)
		{
			type = frkPChunkGetType (pChunk);
			/*if(type == 1 || type == 2)
			{
				key = frkPChunkGetKey (pChunk);
				dataSize = frkPChunkGetSize (pChunk);
				version = frkPChunkGetVersion (pChunk);
				customData = frkPChunkGetData (pChunk);

				data = (ExportData*)customData;
			}
			else*/ if(type == 3)
			{
				key = frkPChunkGetKey (pChunk);
				dataSize = frkPChunkGetSize (pChunk);
				version = frkPChunkGetVersion (pChunk);
				customData = frkPChunkGetData (pChunk);

				pCustomData = (EmitterCustomData*)customData;
				m_CustomData = *pCustomData;
			}
		}
		else
		{
			break;
		}
	}
}
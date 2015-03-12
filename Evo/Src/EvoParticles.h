#ifndef _EVOPARTICLES_H_
#define _EVOPARTICLES_H_

struct ParticleVert
{
	D3DXVECTOR3	Pos;
	DWORD		Color;
	D3DXVECTOR2	UV;
};

struct EmitterCustomData
{  
	int nKey;
	int nData;
	char sName[256];
	int  nBuff[10];
	t_frkMatrix4 MatXForm;	//	This matrix can be used to set emitter location
};

class EvoParticles
{
   public:
	EvoParticles();
	~EvoParticles();

	void Initialize(char *szFilename, bool bLoop);

	void Update(float fDeltaT);

	void Render();

	_t_frkPSystem *GetPSys()		{ return m_pFrkPSys;	}

	void SprSetQuadAxis (EvoPoint3* pUp, EvoPoint3* pRight, EvoPoint3* pRot);
	int SprSubmit (D3DXVECTOR4* pPos, float fRotMin, float fRot, D3DXVECTOR2* pScale, float fScale, D3DXVECTOR4* pTint, D3DXVECTOR4* pColor, D3DXVECTOR4* pUVs);
	void SetShapeIsRect(bool bShapeRect)	{ m_bIsShapeRect = bShapeRect;	}

	EvoMaterial *GetMaterial()				{ return m_pMaterial;	}

	void SetMatrix(EvoMatrix *pMtx);

	bool IsFinished();

   protected:
	_t_frkPEmitterInfo* LoadEmitterInfo(char* szFilename);
	void LoadEmitterAnimationAssets(_t_frkAnmTrk* animationTrack);
	void LoadEmitterAssets(_t_frkPEmitterInfo* emitterInfo);

	void GetCustomData(_t_frkPEmitterInfo* pEDesc);

   protected:
	EvoMaterial *	m_pMaterial;
	EvoShader *		m_pShader;

	EvoInputLayoutHandle m_hInputLayout;

	EvoBuffer *m_pIB;
	EvoBuffer *m_pVB;
	
	_t_frkPSystem*		m_pFrkPSys;
	_t_frkPEmitter*		m_pEmitter;
	_t_frkAnmInst*		m_pAnimationInstance;

	EvoPoint3	m_AxisUpV; 
	EvoPoint3	m_AxisRightV;
	EvoPoint3	m_AxisRot; 

	bool bVBLock;

	int nMaxSpr;
	int nNumSpr;
	int nNumVerts;
	int nNumIndices;
	int nIBSize;
	int nVBSize;
	int nVertSize;

	ParticleVert *			pVertices;
	WORD*					pIndices;

	bool m_bIsShapeRect;

	EvoMatrix m_mtxTransform;

	EmitterCustomData m_CustomData;
};

inline bool EvoParticles::IsFinished()
{
	if (frkPAnmIsEnded (m_pAnimationInstance) == 1)
		return true;
	else return false;
}

#endif
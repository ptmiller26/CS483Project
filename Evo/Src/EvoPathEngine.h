#ifndef _EVOPATHENGINE_H_
#define _EVOPATHENGINE_H_

#define PATHING EvoPathEngine::GetInstance()

#define PATHING_SCALAR		(10)

class EvoAgent;
class EvoPartitionedTerrainPathing;

class EvoPathEngineOutputStream : public iOutputStream
{
   public:
	EvoPathEngineOutputStream(const char *szFilename)
	{
		m_pFile = EvoFile::OpenFile(szFilename, EvoFile::WRITE);
	}

	virtual ~EvoPathEngineOutputStream()
	{
		EvoFile::CloseFile(m_pFile);
	}

    virtual void put(const char* data, tUnsigned32 dataSize)
	{
		m_pFile->WriteBytes(data, dataSize);
	}

   protected:
	EvoFile *m_pFile;
};

class EvoPathEngine
{
   public:
	~EvoPathEngine();
	
	static EvoPathEngine *GetInstance();

	void Initialize();

	void LoadMesh(char *szFilename);
	iMesh *GetMesh()					{ return m_pMesh;	}

	void LoadContent(char *szFilename, float fRot, float fScale, float fX, float fY, float fZ);
	void FinalizeContent();

	void OutputMesh(const char *szFilename);

	EvoAgent *CreateAgent(float fSize, EvoPoint3 pt3Pos);

	void AddToContext(EvoAgent *pAgent);
	void RemoveFromContext(EvoAgent *pAgent);

	iCollisionContext *GetContext()		{ return m_pContext;	}

   protected:
	EvoPathEngine();
	
	cPosition CreatePosition(const EvoPoint3 &pt3);

   protected:
	static EvoPathEngine *	ms_pInstance;
	iPathEngine *			m_pPathEngine;

	iMesh *					m_pMesh;
	iCollisionContext *		m_pContext;

	EvoPartitionedTerrainPathing *m_pTerrainPathing;
	friend class EvoAgent;
	friend class EvoPath;

	std::vector<const iFaceVertexMesh*> m_aContentParts;
};


inline cPosition EvoPathEngine::CreatePosition(const EvoPoint3 &pt3)
{
	static cPosition cPos;

	tSigned32 afPos[3] = {(tSigned32)(pt3.m_fX*PATHING_SCALAR), (tSigned32)(pt3.m_fZ*PATHING_SCALAR), /*(tSigned32)(pt3Pos.m_fY*PATHING_SCALAR)*/100};
	cPos = m_pMesh->positionFor3DPoint(afPos);

	return cPos;
}

#endif
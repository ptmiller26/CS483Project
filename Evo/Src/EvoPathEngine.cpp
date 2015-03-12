#include "EvoStdAfx.h"

EvoPathEngine *EvoPathEngine::ms_pInstance = NULL;

typedef iPathEngine* (__stdcall* tGetInterfaceFunction)(iErrorHandler*);

class cErrorHandler : public iErrorHandler
{
    cErrorHandler(){}
public:
    eAction handle(const char* error_type, const char* error_string, const char *const* attributes)
    {
		MessageBox(EVO->GetWindowHandle(), error_string, error_type, MB_OK);
        //ShowMessageBox(error_type,error_string);
        return CONTINUE;
    }
    static cErrorHandler& refInstance()
    {
        static cErrorHandler the_instance;
        return the_instance;
    }
};

iPathEngine* LoadPathEngine(const char* fileName, iErrorHandler* handler)
{
    char buffer[500];
    DWORD errorValue;
    HINSTANCE hInstance;
    hInstance = LoadLibrary(fileName);
    if(!hInstance)
    {
        errorValue = GetLastError();
        MessageBox(NULL, fileName, "Error: failed calling LoadLibrary() for", MB_OK);
        sprintf(buffer, "%d", errorValue);
        MessageBox(NULL, buffer, "GetLastError()", MB_OK);
        return 0;
    }
    FARPROC procAddress;
    SetLastError(0);
    procAddress = GetProcAddress(hInstance, (LPCSTR)1);
    if(!procAddress)
    {
        errorValue = GetLastError();
        MessageBox(NULL, fileName, "Error: Failed to obtain PathEngine entrypoint in", MB_OK);
        sprintf(buffer, "%d", errorValue);
        MessageBox(NULL, buffer, "GetLastError()", MB_OK);
        return 0;
    }

    tGetInterfaceFunction getInterfaceFunction = (tGetInterfaceFunction)procAddress;
    return getInterfaceFunction(handler);
}


class EvoPartitionedTerrainPathing : public iFaceVertexMesh
{
    long _originX, _originY;
    long _squareSize, _squaresAcross;

public:

    EvoPartitionedTerrainPathing(
            long originX, long originY,
            long squareSize, long squares
            ) :
     _originX(originX),
     _originY(originY),
     _squareSize(squareSize),
     _squaresAcross(squares)
    {
    }

// iFaceVertexMesh interface

    long faces() const
    {
        return _squaresAcross * _squaresAcross * 2;
    }
    long vertices() const
    {
        return (_squaresAcross + 1) * (_squaresAcross + 1);
    }
    long vertexIndex(long face, long vertexInFace) const
    {
        long baseVertex = face / 2 + face / 2 / _squaresAcross;
        if(face & 1)
        {
            switch(vertexInFace)
            {
            default:
//                invalid
            case 0:
                return baseVertex;
            case 1:
                return baseVertex + 1;
            case 2:
                return baseVertex + (_squaresAcross + 1) + 1;
            }
        }
        switch(vertexInFace)
        {
        default:
//            invalid
        case 0:
            return baseVertex;
        case 1:
            return baseVertex + (_squaresAcross + 1) + 1;
        case 2:
            return baseVertex + (_squaresAcross + 1);
        }
    }
    long vertexX(long vertex) const
    {
        return _originX + vertex / (_squaresAcross + 1) * _squareSize;
    }
    long vertexY(long vertex) const
    {
        return _originY + (vertex % (_squaresAcross + 1)) * _squareSize;
    }
    float vertexZ(long vertex) const
    {
        return 0.0f;
    }
    long faceAttribute(long face, long attributeIndex) const
    {
        if(attributeIndex == PE_FaceAttribute_SectionID)
        {
            return 0; // mark all faces as sectionID == 0 (first terrain layer)
        }
        return -1;
    }
};





EvoPathEngine::EvoPathEngine()
{
	m_pPathEngine = NULL;
	m_pMesh = NULL;
}

EvoPathEngine::~EvoPathEngine()
{

}

EvoPathEngine * EvoPathEngine::GetInstance()
{
	if (!ms_pInstance)
	{
		ms_pInstance = new EvoPathEngine();
		ms_pInstance->Initialize();
	}

	return ms_pInstance;
}

void EvoPathEngine::Initialize()
{
	m_pPathEngine = LoadPathEngine("pathengine", &cErrorHandler::refInstance());

	m_pTerrainPathing = new EvoPartitionedTerrainPathing(-100000, -100000, 20000, 10);

	m_aContentParts.push_back(m_pTerrainPathing);
}

void EvoPathEngine::LoadContent(char *szFilename, float fRot, float fScale, float fX, float fY, float fZ)
{
	EvoFile *pFile = EvoFile::OpenFile(szFilename, EvoFile::READ);
	uint uiSize = pFile->GetSize();
	
	char *pData = new char[uiSize];
	pFile->ReadBytes(pData, uiSize);
    iContentChunk* pChunk = m_pPathEngine->loadContentChunk(pData, uiSize);
	delete pData;
	EvoFile::CloseFile(pFile);


	iContentChunkInstance* pChunkInstance;
    pChunkInstance = pChunk->instantiate( (int)fRot, (int)fScale, (int)(fX*PATHING_SCALAR), (int)(fZ*PATHING_SCALAR), (fY*PATHING_SCALAR), -1);               

	m_aContentParts.push_back(pChunkInstance->getGroundPointer());
}
	
void EvoPathEngine::FinalizeContent()
{
	m_pMesh = m_pPathEngine->buildMeshFromContent(&m_aContentParts.front(), m_aContentParts.size(), 0);
	m_pContext = m_pMesh->newContext();
}


void EvoPathEngine::LoadMesh(char *szFilename)
{
	EvoFile *pFile = EvoFile::OpenFile(szFilename, EvoFile::READ);
	uint uiSize = pFile->GetSize();
	
	char *pData = new char[uiSize];
	pFile->ReadBytes(pData, uiSize);

	m_pMesh = m_pPathEngine->loadMeshFromBuffer("xml", pData, uiSize, NULL);

	delete pData;

	EvoFile::CloseFile(pFile);

	m_pContext = m_pMesh->newContext();
}

void EvoPathEngine::OutputMesh(const char *szFilename)
{

	m_pMesh->burnContextIntoMesh(m_pContext);
	EvoPathEngineOutputStream *pOS = new EvoPathEngineOutputStream(szFilename);

	m_pMesh->saveGround("xml", true, pOS);

	SAFE_DELETE(pOS);
}

EvoAgent *EvoPathEngine::CreateAgent(float fSize, EvoPoint3 pt3Pos)
{
	EvoAgent *pAgent = new EvoAgent();
	int iSize = (int)(fSize*PATHING_SCALAR);

	long Arr[]=
    {
            -iSize,iSize,
            iSize,iSize,
            iSize,-iSize,
            -iSize,-iSize,
    };

	pAgent->m_pShape = m_pPathEngine->newShape(sizeof(Arr)/sizeof(*Arr)/2,Arr);

	m_pMesh->generateCollisionPreprocessFor(pAgent->m_pShape, NULL);
	m_pMesh->generatePathfindPreprocessFor(pAgent->m_pShape, NULL);

	cPosition Position = CreatePosition(pt3Pos);

	pAgent->m_pAgent = m_pMesh->placeAgent(pAgent->m_pShape, Position);

	AddToContext(pAgent);

	return pAgent;
}

void EvoPathEngine::AddToContext(EvoAgent *pAgent)
{
	m_pContext->addAgent(pAgent->m_pAgent);
}

void EvoPathEngine::RemoveFromContext(EvoAgent *pAgent)
{
	m_pContext->removeAgent(pAgent->m_pAgent);
}

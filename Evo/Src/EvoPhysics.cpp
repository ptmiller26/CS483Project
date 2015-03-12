#include "EvoStdAfx.h"

EvoPhysics *EvoPhysics::ms_pInstance = NULL;

class UserStream : public NxStream
	{
	public:
								UserStream(const char* filename, bool load);
	virtual						~UserStream();

	virtual		NxU8			readByte()								const;
	virtual		NxU16			readWord()								const;
	virtual		NxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, NxU32 size)	const;

	virtual		NxStream&		storeByte(NxU8 b);
	virtual		NxStream&		storeWord(NxU16 w);
	virtual		NxStream&		storeDword(NxU32 d);
	virtual		NxStream&		storeFloat(NxReal f);
	virtual		NxStream&		storeDouble(NxF64 f);
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

				FILE*			fp;
	};

class MemoryWriteBuffer : public NxStream
	{
	public:
								MemoryWriteBuffer();
	virtual						~MemoryWriteBuffer();
				void			clear();

	virtual		NxU8			readByte()								const	{ NX_ASSERT(0);	return 0;	}
	virtual		NxU16			readWord()								const	{ NX_ASSERT(0);	return 0;	}
	virtual		NxU32			readDword()								const	{ NX_ASSERT(0);	return 0;	}
	virtual		float			readFloat()								const	{ NX_ASSERT(0);	return 0.0f;}
	virtual		double			readDouble()							const	{ NX_ASSERT(0);	return 0.0;	}
	virtual		void			readBuffer(void* buffer, NxU32 size)	const	{ NX_ASSERT(0);				}

	virtual		NxStream&		storeByte(NxU8 b);
	virtual		NxStream&		storeWord(NxU16 w);
	virtual		NxStream&		storeDword(NxU32 d);
	virtual		NxStream&		storeFloat(NxReal f);
	virtual		NxStream&		storeDouble(NxF64 f);
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

				NxU32			currentSize;
				NxU32			maxSize;
				NxU8*			data;
	};

class MemoryReadBuffer : public NxStream
	{
	public:
								MemoryReadBuffer(const NxU8* data);
	virtual						~MemoryReadBuffer();

	virtual		NxU8			readByte()								const;
	virtual		NxU16			readWord()								const;
	virtual		NxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, NxU32 size)	const;

	virtual		NxStream&		storeByte(NxU8 b)							{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeWord(NxU16 w)							{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeDword(NxU32 d)							{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeFloat(NxReal f)						{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeDouble(NxF64 f)						{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size)	{ NX_ASSERT(0);	return *this;	}

	mutable		const NxU8*		buffer;
	};

UserStream::UserStream(const char* filename, bool load) : fp(NULL)
	{
	fp = fopen(filename, load ? "rb" : "wb");
	}

UserStream::~UserStream()
	{
	if(fp)	fclose(fp);
	}

// Loading API
NxU8 UserStream::readByte() const
	{
	NxU8 b;
	size_t r = fread(&b, sizeof(NxU8), 1, fp);
	NX_ASSERT(r);
	return b;
	}

NxU16 UserStream::readWord() const
	{
	NxU16 w;
	size_t r = fread(&w, sizeof(NxU16), 1, fp);
	NX_ASSERT(r);
	return w;
	}

NxU32 UserStream::readDword() const
	{
	NxU32 d;
	size_t r = fread(&d, sizeof(NxU32), 1, fp);
	NX_ASSERT(r);
	return d;
	}

float UserStream::readFloat() const
	{
	NxReal f;
	size_t r = fread(&f, sizeof(NxReal), 1, fp);
	NX_ASSERT(r);
	return f;
	}

double UserStream::readDouble() const
	{
	NxF64 f;
	size_t r = fread(&f, sizeof(NxF64), 1, fp);
	NX_ASSERT(r);
	return f;
	}

void UserStream::readBuffer(void* buffer, NxU32 size)	const
	{
	size_t w = fread(buffer, size, 1, fp);
	NX_ASSERT(w);
	}

// Saving API
NxStream& UserStream::storeByte(NxU8 b)
	{
	size_t w = fwrite(&b, sizeof(NxU8), 1, fp);
	NX_ASSERT(w);
	return *this;
	}

NxStream& UserStream::storeWord(NxU16 w)
	{
	size_t ww = fwrite(&w, sizeof(NxU16), 1, fp);
	NX_ASSERT(ww);
	return *this;
	}

NxStream& UserStream::storeDword(NxU32 d)
	{
	size_t w = fwrite(&d, sizeof(NxU32), 1, fp);
	NX_ASSERT(w);
	return *this;
	}

NxStream& UserStream::storeFloat(NxReal f)
	{
	size_t w = fwrite(&f, sizeof(NxReal), 1, fp);
	NX_ASSERT(w);
	return *this;
	}

NxStream& UserStream::storeDouble(NxF64 f)
	{
	size_t w = fwrite(&f, sizeof(NxF64), 1, fp);
	NX_ASSERT(w);
	return *this;
	}

NxStream& UserStream::storeBuffer(const void* buffer, NxU32 size)
	{
	size_t w = fwrite(buffer, size, 1, fp);
	NX_ASSERT(w);
	return *this;
	}




MemoryWriteBuffer::MemoryWriteBuffer() : currentSize(0), maxSize(0), data(NULL)
	{
	}

MemoryWriteBuffer::~MemoryWriteBuffer()
	{
		if(data!=NULL)
		{
			delete[] data;
			data=NULL;
		}
	}

void MemoryWriteBuffer::clear()
	{
	currentSize = 0;
	}

NxStream& MemoryWriteBuffer::storeByte(NxU8 b)
	{
	storeBuffer(&b, sizeof(NxU8));
	return *this;
	}
NxStream& MemoryWriteBuffer::storeWord(NxU16 w)
	{
	storeBuffer(&w, sizeof(NxU16));
	return *this;
	}
NxStream& MemoryWriteBuffer::storeDword(NxU32 d)
	{
	storeBuffer(&d, sizeof(NxU32));
	return *this;
	}
NxStream& MemoryWriteBuffer::storeFloat(NxReal f)
	{
	storeBuffer(&f, sizeof(NxReal));
	return *this;
	}
NxStream& MemoryWriteBuffer::storeDouble(NxF64 f)
	{
	storeBuffer(&f, sizeof(NxF64));
	return *this;
	}
NxStream& MemoryWriteBuffer::storeBuffer(const void* buffer, NxU32 size)
	{
	NxU32 expectedSize = currentSize + size;
	if(expectedSize > maxSize)
		{
		maxSize = expectedSize + 4096;

		NxU8* newData = new NxU8[maxSize];
		NX_ASSERT(newData!=NULL);

		if(data)
			{
			memcpy(newData, data, currentSize);
			delete[] data;
			}
		data = newData;
		}
	memcpy(data+currentSize, buffer, size);
	currentSize += size;
	return *this;
	}


MemoryReadBuffer::MemoryReadBuffer(const NxU8* data) : buffer(data)
	{
	}

MemoryReadBuffer::~MemoryReadBuffer()
	{
	// We don't own the data => no delete
	}

NxU8 MemoryReadBuffer::readByte() const
	{
	NxU8 b;
	memcpy(&b, buffer, sizeof(NxU8));
	buffer += sizeof(NxU8);
	return b;
	}

NxU16 MemoryReadBuffer::readWord() const
	{
	NxU16 w;
	memcpy(&w, buffer, sizeof(NxU16));
	buffer += sizeof(NxU16);
	return w;
	}

NxU32 MemoryReadBuffer::readDword() const
	{
	NxU32 d;
	memcpy(&d, buffer, sizeof(NxU32));
	buffer += sizeof(NxU32);
	return d;
	}

float MemoryReadBuffer::readFloat() const
	{
	float f;
	memcpy(&f, buffer, sizeof(float));
	buffer += sizeof(float);
	return f;
	}

double MemoryReadBuffer::readDouble() const
	{
	double f;
	memcpy(&f, buffer, sizeof(double));
	buffer += sizeof(double);
	return f;
	}

void MemoryReadBuffer::readBuffer(void* dest, NxU32 size) const
	{
	memcpy(dest, buffer, size);
	buffer += size;
	}




EvoPhysics::EvoPhysics()
{

}

EvoPhysics::~EvoPhysics()
{

}

EvoPhysics * EvoPhysics::GetInstance()
{
	if (!ms_pInstance)
	{
		ms_pInstance = new EvoPhysics();
		ms_pInstance->Initialize();
	}

	return ms_pInstance;
}

void EvoPhysics::Initialize()
{
	m_pPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	m_pPhysicsSDK->setParameter(NX_CONTINUOUS_CD, 1);

	m_pPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect ("localhost", 5425);



	NxInitCooking();

	// Create a scene
	NxSceneDesc SceneDesc;
	//SceneDesc.simType = NX_SIMULATION_HW;
	SceneDesc.gravity = NxVec3(0.0f, -9.81f, 0.0f);
	m_pPhysicsScene = m_pPhysicsSDK->createScene(SceneDesc);

	m_pPhysicsMaterial = m_pPhysicsScene->getMaterialFromIndex(0); 
	m_pPhysicsMaterial->setRestitution(0.25f);
	m_pPhysicsMaterial->setStaticFriction(0.15f);
	m_pPhysicsMaterial->setDynamicFriction(0.55f);

	m_pPhysicsScene->setTiming(1.0f/60.0f, 8, NX_TIMESTEP_FIXED);

	//m_pPhysicsSDK->setParameter( NX_VISUALIZATION_SCALE, 1.0f);
	//m_pPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
}

void EvoPhysics::Update(float fDeltaT)
{
	m_pPhysicsScene->simulate(fDeltaT);
	m_pPhysicsScene->flushStream();
	m_pPhysicsScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

	for (uint ui = 0; ui < m_apObjects.size(); ui++)
	{
		if (m_apObjects[ui]->GetNode())
		{
			EvoMatrix *pMatrix = m_apObjects[ui]->GetNode()->GetMatrix();

			m_apObjects[ui]->m_pActor->getGlobalPose().getColumnMajor44((NxF32*)pMatrix);
		}
	}

	// Update the controllers
	m_ControllerManager.updateControllers();
}

void EvoPhysics::RemovePhysicalObject(EvoPhysicalObject *pObj)
{
	for (uint ui = 0; ui < m_apObjects.size(); ui++)
	{
		if (m_apObjects[ui] == pObj)
		{
			m_apObjects.erase(m_apObjects.begin() + ui);
			return;
		}
	}
}

bool EvoPhysics::RayCast(EvoPoint3 pt3Pos, EvoPoint3 pt3Dir, EvoPoint3 *ppt3HitPos, EvoPoint3 *ppt3HitNormal, bool bStaticOnly)
{
	NxRaycastHit RayHit;
	NxRay Ray;
	Ray.orig = *((NxVec3 *)&pt3Pos);
	Ray.dir = *((NxVec3 *)&pt3Dir);
	NxShape *pShape = NULL;

	if (bStaticOnly)
		pShape = GetScene()->raycastClosestShape(Ray, NX_STATIC_SHAPES, RayHit);
	else pShape = GetScene()->raycastClosestShape(Ray, NX_ALL_SHAPES, RayHit);

	*ppt3HitPos = *((EvoPoint3 *)&RayHit.worldImpact);
	*ppt3HitNormal = *((EvoPoint3 *)&RayHit.worldNormal);

	return pShape != NULL;
}

void EvoPhysics::RenderPhysics()
{
/*	EvoMatrix m;
	m.SetIdentity();

	EVO->SetTransform(EVO_TRANSFORM_WORLD, &m);

	const NxDebugRenderable *dbgRenderable = m_pPhysicsScene->getDebugRenderable();

	NxU32 NbTris = dbgRenderable->getNbTriangles();

	if (NbTris)
	{
		const NxDebugTriangle* Triangles = dbgRenderable->getTriangles();
		EVO->GetD3DDevice()->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		EVO->GetD3DDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, NbTris, Triangles, sizeof(NxDebugTriangle));
	}

	NxU32 NbLines = dbgRenderable->getNbLines();
	
	if (NbLines)
	{
		static uchar pBytes[1024*1024];
		uchar *pB = pBytes;
		const NxDebugLine* Lines = dbgRenderable->getLines();

		for (uint ui = 0; ui < NbLines; ui++)
		{
			memcpy(pB, &Lines[ui].p0, sizeof(EvoPoint3));
			pB += sizeof(EvoPoint3);
			memcpy(pB, &Lines[ui].color, sizeof(uint));
			pB += sizeof(uint);

			memcpy(pB, &Lines[ui].p1, sizeof(EvoPoint3));
			pB += sizeof(EvoPoint3);
			memcpy(pB, &Lines[ui].color, sizeof(uint));
			pB += sizeof(uint);
		}

		EVO->GetD3DDevice()->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		EVO->GetD3DDevice()->DrawPrimitiveUP(D3DPT_LINELIST, NbLines, pBytes, sizeof(EvoPoint3) + sizeof(uint));
	}

	NxU32 NbPoints = dbgRenderable->getNbPoints();

	if (NbPoints)
	{
		const NxDebugPoint* Points = dbgRenderable->getPoints();
		int iR = 0;
	}
*/
}

void EvoPhysics::SetContactReport(NxUserContactReport *pContactReport)
{
	m_pPhysicsScene->setUserContactReport(pContactReport);
}

void EvoPhysics::SetActorGroupPairFlags(uint uiGroup1, uint uiGroup2, uint uiFlags)
{
	m_pPhysicsScene->setActorGroupPairFlags(uiGroup1, uiGroup2, uiFlags);
}


// EvoPhysicalObject *************************************************************
EvoPhysicalObject::EvoPhysicalObject()
{
	m_pNode = NULL;
}

EvoPhysicalObject::~EvoPhysicalObject()
{
	PHYSICS->GetScene()->releaseActor(*m_pActor);
	m_pActor = NULL;
}

void EvoPhysicalObject::SetCollisionGroup(uint uiGroup)
{
	m_pActor->setGroup(uiGroup);

	NxShape **ppShapes = (NxShape **)m_pActor->getShapes();
	for (uint ui = 0; ui < m_pActor->getNbShapes(); ui++)
	{
		ppShapes[ui]->setGroup(uiGroup);
	}
}

EvoPhysicalObject *EvoPhysicalObject::CreatePlane(EvoPoint3 pt3Normal, float fDistance)
{
	EvoPhysicalObject *pObject = new EvoPhysicalObject();

	NxPlaneShapeDesc PlaneDesc;
	PlaneDesc.normal = *((NxVec3*)&pt3Normal);
	PlaneDesc.d = fDistance;

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&PlaneDesc);
	pObject->m_pActor = PHYSICS->GetScene()->createActor(ActorDesc);

	PHYSICS->AddPhysicalObject(pObject);

	return pObject;
}

EvoPhysicalObject *EvoPhysicalObject::CreateBox(EvoPoint3 pt3Pos, EvoPoint3 pt3Dimensions, EvoPoint3 pt3InitialVelocity)
{
	EvoPhysicalObject *pObject = new EvoPhysicalObject();

	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
//	BodyDesc.maxAngularVelocity	= 10.0f;
	BodyDesc.linearVelocity = *((NxVec3*)&pt3InitialVelocity);
	//BodyDesc.flags = NX_BF_DISABLE_GRAVITY;

	NxBoxShapeDesc BoxDesc;
	BoxDesc.dimensions		= *((NxVec3*)&pt3Dimensions);

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&BoxDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 10.0f;
	ActorDesc.globalPose.t  = *((NxVec3*)&pt3Pos);
	
	pObject->m_pActor = PHYSICS->GetScene()->createActor(ActorDesc);

	PHYSICS->AddPhysicalObject(pObject);

	return pObject;
}

EvoPhysicalObject *EvoPhysicalObject::CreateMesh(EvoNode *pNode, bool bStatic, char *szPMapFilename)
{
	NxTriangleMeshDesc MeshDesc;
	NxTriangleMeshShapeDesc MeshShapeDesc;

	EvoPhysicalObject *pObject = new EvoPhysicalObject();

	MeshDesc.numVertices = pNode->GetRenderable()->GetNumVerts();
	MeshDesc.numTriangles = pNode->GetRenderable()->GetNumFaces();
	MeshDesc.pointStrideBytes = pNode->GetRenderable()->GetVertStride();
	MeshDesc.triangleStrideBytes = pNode->GetRenderable()->GetFaceStride();
	MeshDesc.points = pNode->GetRenderable()->GetVerts();
	MeshDesc.triangles = pNode->GetRenderable()->GetFaces();
	MeshDesc.flags = 0;//NX_MF_16_BIT_INDICES;

	MemoryWriteBuffer buf;
	NxCookTriangleMesh(MeshDesc, buf);
  	MemoryReadBuffer readBuffer(buf.data);
  	MeshShapeDesc.meshData = PHYSICS->GetSDK()->createTriangleMesh(readBuffer);
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
//	BodyDesc.maxAngularVelocity	= 10.0f;

	NxPMap PMap;
	PMap.dataSize	= 0;
	PMap.data		= NULL;

	EvoFile *pPMapFile = EvoFile::OpenFile(szPMapFilename, EvoFile::READ);

	if (!pPMapFile)
	{
		if (!NxCreatePMap(PMap, *MeshShapeDesc.meshData, 64))
		{
			assert(0 && "Failed to create PMap");
		}
		pPMapFile = EvoFile::OpenFile(szPMapFilename, EvoFile::WRITE);
		pPMapFile->WriteBytes(PMap.data, PMap.dataSize);
	}
	else
	{
		PMap.dataSize = pPMapFile->GetSize();
		PMap.data       = new NxU8[PMap.dataSize];
		pPMapFile->ReadBytes(PMap.data, PMap.dataSize);
	}

	EvoFile::CloseFile(pPMapFile);

	MeshShapeDesc.meshData->loadPMap(PMap);

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&MeshShapeDesc);
	if (!bStatic)
	{
		ActorDesc.body			= &BodyDesc;
		ActorDesc.density		= 10.0f;
	}
	

	pObject->m_pActor = PHYSICS->GetScene()->createActor(ActorDesc);

	PHYSICS->AddPhysicalObject(pObject);

	return pObject;
}

EvoPhysicalObject *EvoPhysicalObject::CreateConvexHull(EvoNode *pNode)
{
	NxConvexMeshDesc ConvexMeshDesc;
	NxConvexShapeDesc ConvexMeshShapeDesc;

	EvoPhysicalObject *pObject = new EvoPhysicalObject();
/*
	EvoTriListGeom *pGeom = (EvoTriListGeom *)EvoNodeOps::GetRenderable(pNode);


	

	ConvexMeshDesc.numVertices = pGeom->GetNumVerts();
	ConvexMeshDesc.pointStrideBytes = sizeof(EvoPoint3);
	ConvexMeshDesc.points = pGeom->GetVertexPositions();
	ConvexMeshDesc.flags = NX_CF_COMPUTE_CONVEX;

	MemoryWriteBuffer buf;
	NxCookConvexMesh(ConvexMeshDesc, buf);
  	MemoryReadBuffer readBuffer(buf.data);
  	ConvexMeshShapeDesc.meshData = PHYSICS->GetSDK()->createConvexMesh(readBuffer);
*/
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
//	BodyDesc.maxAngularVelocity	= 10.0f;

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&ConvexMeshShapeDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 12.0f;

	pObject->m_pActor = PHYSICS->GetScene()->createActor(ActorDesc);

	pObject->m_pActor->setCMassOffsetLocalPosition(NxVec3(0, -3, 0));

	PHYSICS->AddPhysicalObject(pObject);

	return pObject;
}


void EvoPhysicalObject::SetPosition(EvoPoint3 pt3)
{
	m_pActor->setGlobalPosition(*((NxVec3 *)(&pt3)));
}

	

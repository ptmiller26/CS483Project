#ifndef _EVOPHYSICS_H_
#define _EVOPHYSICS_H_

#define PHYSICS EvoPhysics::GetInstance()

class EvoPhysicalObject;

class EvoPhysics
{
   public:
	~EvoPhysics();

	static EvoPhysics *GetInstance();

	void Update(float fDeltaT);

	// Visual debuggin
	void RenderPhysics();

	NxScene *GetScene()									{ return m_pPhysicsScene;		}
	void AddPhysicalObject(EvoPhysicalObject *pObj)		{ m_apObjects.push_back(pObj);	}
	void RemovePhysicalObject(EvoPhysicalObject *pObj);

	NxPhysicsSDK *GetSDK()								{ return m_pPhysicsSDK;			}

	ControllerManager *GetControllerManager()			{ return &m_ControllerManager;	}

	bool RayCast(EvoPoint3 pt3Pos, EvoPoint3 pt3Dir, EvoPoint3 *ppt3HitPos, EvoPoint3 *ppt3HitNormal, bool bStaticOnly);

	void SetContactReport(NxUserContactReport *pContactReport);
	void SetActorGroupPairFlags(uint uiGroup1, uint uiGroup2, uint uiFlags);

   protected:
	EvoPhysics();
	void Initialize();

   protected:
	static EvoPhysics	*ms_pInstance;

	NxPhysicsSDK		*m_pPhysicsSDK;
	NxScene				*m_pPhysicsScene;
	NxMaterial			*m_pPhysicsMaterial;

	ControllerManager	m_ControllerManager;

	std::vector<EvoPhysicalObject *>	m_apObjects;

	friend class EvoPhysicalObject;
};

class EvoPhysicalObject
{
   public:
	~EvoPhysicalObject();

	static EvoPhysicalObject *CreatePlane(EvoPoint3 pt3Normal = EvoPoint3(0.0f, 1.0f, 0.0f), float fDistance = 0.0f);
	static EvoPhysicalObject *CreateBox(EvoPoint3 pt3Pos, EvoPoint3 pt3Dimensions, EvoPoint3 pt3InitialVelocity = EvoPoint3(0.0f, 0.0f, 0.0f) );
	static EvoPhysicalObject *CreateMesh(EvoNode *pNode, bool bStatic, char *szPMapFilename);
	static EvoPhysicalObject *CreateConvexHull(EvoNode *pNode);

	void SetNode(EvoNode *pNode)	{ m_pNode = pNode;	}
	EvoNode *GetNode()				{ return m_pNode;	}

	NxActor *GetActor()				{ return m_pActor;	}

	void SetPosition(EvoPoint3 pt3);

	void AddTorque(EvoPoint3 pt3Torque);
	void AddForce(EvoPoint3 pt3Force);

	void SetLinearDamping(float fDamping);
	void SetAngularDamping(float fDamping);

	void SetCollisionGroup(uint uiGroup);

	void SetUserData(void *pData);
	
	void DisableCollisionsWith(EvoPhysicalObject *pObject);

   protected:
	EvoPhysicalObject();

   protected:
	NxActor *m_pActor;

	EvoNode *m_pNode;

	friend class EvoPhysics;
};

inline void EvoPhysicalObject::AddTorque(EvoPoint3 pt3Torque)
{
	m_pActor->addTorque(*((NxVec3 *)(&pt3Torque)), NX_FORCE );
}

inline void EvoPhysicalObject::AddForce(EvoPoint3 pt3Force)
{
	m_pActor->addForce(*((NxVec3 *)(&pt3Force)), NX_FORCE );
}

inline void EvoPhysicalObject::SetLinearDamping(float fDamping)
{
	m_pActor->setLinearDamping(fDamping);
}

inline void EvoPhysicalObject::SetAngularDamping(float fDamping)
{
	m_pActor->setAngularDamping(fDamping);
}

inline void EvoPhysicalObject::SetUserData(void *pData)
{
	m_pActor->userData = pData;
}

inline void EvoPhysicalObject::DisableCollisionsWith(EvoPhysicalObject *pObject)
{
	PHYSICS->GetScene()->setActorPairFlags(*m_pActor, *(pObject->m_pActor), NX_IGNORE_PAIR);
}





#endif
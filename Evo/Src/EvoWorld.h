#ifndef _EVOWORLD_H_
#define _EVOWORLD_H_

//class EvoEntity;

//typedef std::vector<EvoEntity *>	EntityList;
//typedef EntityList::iterator		EntityIterator;

class EvoWorld 
{
public:
    ~EvoWorld();

	void Init();
	void Shutdown(){};

/*	void Insert(EvoEntity *pEnt);
	void Remove(EvoEntity *pEnt);

	const EntityList& GetAllEnts() { return m_AllEntsList; };
	const EntityList& GetVisibleEnts(EvoCamera *pCam);

	void GetAllEntitiesWithFlagsSet(EntityList *pEntityList, uint uiFlags);
	void GetVisibleEntitiesWithFlagsSet(EntityList *pEntityList, uint uiFlags);

	void GetEntitiesIntersectingFrustumWithCastedDir(EntityList *pEntityList, EvoFrustum Frustum, EvoPoint3 pt3Dir);
*/
	static EvoWorld *GetInstance();
private:
	EvoWorld(){};

private:
	static EvoWorld *		ms_pInstance;

	//EntityList m_AllEntsList;	// List of all Entities in the world
	//EntityList m_VisibleEnts;   // Cached List of Visible Ents from last GetVisibleEnts();
};

#endif  //#ifndef _EVOWORLD_H_

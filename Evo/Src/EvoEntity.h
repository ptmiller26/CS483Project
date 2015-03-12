#ifndef _EVOENTITY_H_
#define _EVOENTITY_H_

#define ENTITY_CASTSHADOW		(1 << 0)
#define ENTITY_RECEIVESHADOW	(2 << 0)

class EvoEntity 
{
   public:
    EvoEntity();
	virtual ~EvoEntity()	{};

	virtual void Initialize(const char *szFilename, uint uiFlags);
	virtual void Render();
	virtual void Update(float fDeltaT);

	void SetFlags(uint uiFlags)		{ m_uiFlags = uiFlags;	}
	uint GetFlags()					{ return m_uiFlags;		}

	EvoNode *GetNode()				{ return m_pNode;	}

	const EvoBoundingBox &GetAABB()	{ return m_AABB;	}
	const EvoBoundingBox &GetOOBB()	{ return m_OOBB;	}

	void CalculateAABBFromOOBB();

   protected:
    uint		m_uiFlags;
	EvoNode *	m_pNode;

	// object space bounding box
	EvoBoundingBox m_OOBB;
	// world space bounding box (use CalculateAABB to update)
	EvoBoundingBox m_AABB;
};

#endif  //#ifndef _EVOENTITY_H_

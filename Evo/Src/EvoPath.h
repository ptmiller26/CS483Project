#ifndef _EVOPATH_H_
#define _EVOPATH_H_

class EvoPath
{
   public:
    EvoPath();
	~EvoPath();

	bool MakePath(iShape *pShape, EvoPoint3 pt3Start, EvoPoint3 pt3End);
	bool MakeCurvedPath(iShape *pShape, EvoPoint3 pt3Start, EvoPoint3 pt3End, EvoPoint3 pt3StartFacing);

	bool AdvanceAlongPath(iShape *pShape, float fDistance, float &fX, float &fY);

	int GetNumPoints()	{ return m_pPath->size();	}

	EvoPoint3 GetPosition(uint uiIndex);

	float GetLength()	{ return ((float)m_pPath->getLength())/PATHING_SCALAR;	}

	iPath *GetPath()	{ return m_pPath;	}

    protected:
	iPath *	m_pPath;

	friend class EvoPathEngine;
};

// Inlines

inline EvoPoint3 EvoPath::GetPosition(uint uiIndex)
{
	EvoPoint3 pt3;
	cPosition cPos = m_pPath->position(uiIndex);

	pt3.m_fX = ((float)cPos.x)/PATHING_SCALAR;
	pt3.m_fY = 0.0f;
	pt3.m_fZ = ((float)cPos.y)/PATHING_SCALAR;

	return pt3;
}

#endif
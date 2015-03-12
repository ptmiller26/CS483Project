#ifndef _EVOAGENT_H_
#define _EVOAGENT_H_

class EvoAgent
{
   public:
	bool MakePath(EvoPoint3 pt3Start, EvoPoint3 pt3End);
	bool MakeCurvedPath(EvoPoint3 pt3Start, EvoPoint3 pt3End, EvoPoint3 pt3StartFacing);

	bool AdvanceAlongPath(float fDistance, float &fX, float &fY);

	EvoPath *GetPath()				{ return m_pPath;		}
	void SetPath(EvoPath *pPath)	{ m_pPath = pPath;		}
	void DestroyPath()				{ SAFE_DELETE(m_pPath);	}

	iShape *GetShape()				{ return m_pShape;		}

   protected:
	EvoAgent();
	~EvoAgent();

   protected:
	iAgent *m_pAgent;
	iShape *m_pShape;
	EvoPath *m_pPath;

	friend class EvoPathEngine;
};


#endif
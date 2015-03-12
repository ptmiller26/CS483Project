#ifndef _EVOLINE_H_
#define _EVOLINE_H_

class EvoLine
{
   public:
	EvoLine();
	EvoLine(const EvoPoint3 &pt3Start, const EvoPoint3 &pt3End);

	~EvoLine();

	void SetStart(EvoPoint3 pt3)	{ m_pt3Start = pt3;	}
	void SetEnd(EvoPoint3 pt3)		{ m_pt3End = pt3;	}

	EvoPoint3 GetStart()			{ return m_pt3Start;	}
	EvoPoint3 GetEnd()				{ return m_pt3End;		}

	bool Intersect2D(const EvoLine &Line, EvoPoint3 *pIntersection, float* pfTime);

   protected:
	EvoPoint3 m_pt3Start;
	EvoPoint3 m_pt3End;
};

#endif
#ifndef _EVOBONEMODIFIER_H_
#define _EVOBONEMODIFIER_H_

class EvoGranny;

class EvoBoneModifier
{
   public:
	EvoBoneModifier();
	~EvoBoneModifier();

	bool Initialize(EvoGranny *pGrannyObject, char *szBoneName);

	EvoMatrix *GetMatrixModifier()	{ return &m_Matrix;	}

   protected:
	void Apply();	

   protected:
	EvoGranny *	m_pGrannyObject;
	uint		m_uiModelIndex;
	int			m_iBoneIndex;

	EvoMatrix	m_Matrix;

	friend class EvoGranny;
};

#endif
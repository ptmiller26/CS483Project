#ifndef _EVOBONEQUERY_H_
#define _EVOBONEQUERY_H_

class EvoGranny;

class EvoBoneQuery
{
   public:
	EvoBoneQuery();
	~EvoBoneQuery();

	bool Initialize(EvoGranny *pGrannyObject, char *szBoneName);

	void QueryWorldTransform(EvoMatrix *pMatrix);

   protected:
	EvoGranny *	m_pGrannyObject;
	uint		m_uiModelIndex;
	int			m_iBoneIndex;

	friend class EvoGranny;
};

#endif
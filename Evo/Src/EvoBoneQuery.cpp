#include "EvoStdAfx.h"

EvoBoneQuery::EvoBoneQuery()
{
	m_iBoneIndex	= -1;
	m_uiModelIndex	= 0;
}

EvoBoneQuery::~EvoBoneQuery()
{
	m_iBoneIndex = -1;
}

bool EvoBoneQuery::Initialize(EvoGranny *pGrannyObject, char *szBoneName)
{
	m_pGrannyObject = pGrannyObject;

	bool bFound = false;

	for (uint Idx = 0; Idx < m_pGrannyObject->m_apModels.size(); Idx++)
	{
		EvoGrannyModel* pModel = m_pGrannyObject->m_apModels[Idx];
		granny_skeleton *pSkeleton = GrannyGetSourceSkeleton(pModel->m_pGrannyInstance);
		if(GrannyFindBoneByName(pSkeleton, szBoneName, &m_iBoneIndex))
		{
			// The bone was found, it's index is iBoneIndex
			bFound = true;
			m_uiModelIndex = Idx;
			break;
		}
	}

	if (!bFound)
	{
		m_iBoneIndex = -1;
	}

	return bFound;
}

void EvoBoneQuery::QueryWorldTransform(EvoMatrix *pMatrix)
{
	if (m_iBoneIndex != -1)
	{
		float *pF = GrannyGetWorldPose4x4(m_pGrannyObject->m_apModels[m_uiModelIndex]->m_pWorldPose, m_iBoneIndex);

		memcpy(pMatrix, pF, sizeof(EvoMatrix));
	}
}
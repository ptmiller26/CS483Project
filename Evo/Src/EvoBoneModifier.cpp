#include "EvoStdAfx.h"

EvoBoneModifier::EvoBoneModifier()
{
	m_iBoneIndex	= -1;
	m_uiModelIndex	= 0;
}

EvoBoneModifier::~EvoBoneModifier()
{
	m_iBoneIndex = -1;
}

bool EvoBoneModifier::Initialize(EvoGranny *pGrannyObject, char *szBoneName)
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

void EvoBoneModifier::Apply()
{
	if (m_iBoneIndex != -1)
	{
		EvoMatrix m;

		float Quat[4];
		float Pos[3] = {0,0,0};
		float Scale[3][3] = {1,0,0,0,1,0,0,0,1};

		granny_transform *pTransform = GrannyGetLocalPoseTransform(m_pGrannyObject->m_pSharedLocalPose, m_iBoneIndex);
		GrannyBuildCompositeTransform4x4(pTransform, (granny_real32 *)&m);

		m.Multiply(&m_Matrix);

		D3DXQuaternionRotationMatrix((D3DXQUATERNION *)Quat, (D3DXMATRIX *)&m);

		pTransform->Orientation[0] = Quat[0];
		pTransform->Orientation[1] = Quat[1];
		pTransform->Orientation[2] = Quat[2];
		pTransform->Orientation[3] = Quat[3];

		EvoPoint3 pt3T;
		m.GetTranslate(&pt3T);
		pTransform->Position[0] =pt3T.m_fX;
		pTransform->Position[1] =pt3T.m_fY;
		pTransform->Position[2] =pt3T.m_fZ;
	}
}
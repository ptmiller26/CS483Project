#include "EvoStdAfx.h"
#include "EvoEntity.h"

//---------------------------------------------------------------------------
EvoEntity::EvoEntity() :
m_pNode(NULL) , m_uiFlags(0)
{

}
//---------------------------------------------------------------------------
void EvoEntity::Initialize(const char *szFilename, uint uiFlags)
{
	m_uiFlags = uiFlags;
	m_pNode = RESMGR->LoadGrannyScene(szFilename);
	m_pNode->Update(0.0f);

	((EvoGranny *)m_pNode->GetRenderable())->ComputeOBB(&m_OOBB);
}
//---------------------------------------------------------------------------
void EvoEntity::Render()
{
	m_pNode->Render();
}
//---------------------------------------------------------------------------
void EvoEntity::Update(float fDeltaT)
{
	m_pNode->Update(fDeltaT);

	CalculateAABBFromOOBB();
}

void EvoEntity::CalculateAABBFromOOBB(void)
{
	// transform OOBB points to world space
	EvoPoint3 pt3Transformed[8];

	for(int i=0;i<8;i++)
	{
		pt3Transformed[i] = m_OOBB.m_pPoints[i]*(*m_pNode->GetWorldMatrix());
	}
	// set new AABB
	m_AABB.Set(pt3Transformed, 8, sizeof(EvoPoint3));
}

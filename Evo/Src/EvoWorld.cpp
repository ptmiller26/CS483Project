#include "EvoStdAfx.h"
#include "EvoWorld.h"
#include "EvoEntity.h"

bool SweepIntersectionTest(const EvoBoundingBox &objectBB, const EvoFrustum &frustum, const EvoPoint3 &vSweepDir);

EvoWorld *EvoWorld::ms_pInstance = NULL;

//---------------------------------------------------------------------------
EvoWorld * EvoWorld::GetInstance()
{
	if (!ms_pInstance)
		ms_pInstance = new EvoWorld();

	return ms_pInstance;
}
//---------------------------------------------------------------------------
void EvoWorld::Init()
{
	m_AllEntsList.reserve(512);
}
//---------------------------------------------------------------------------
void EvoWorld::Insert(EvoEntity *pEnt)
{
	m_AllEntsList.push_back(pEnt);
}
//---------------------------------------------------------------------------
void EvoWorld::Remove(EvoEntity *pEnt)
{
	EntityIterator it;
	it = std::remove(m_AllEntsList.begin(), m_AllEntsList.end(), pEnt);
	m_AllEntsList.erase(it,m_AllEntsList.end());
}
//---------------------------------------------------------------------------
const EntityList& EvoWorld::GetVisibleEnts(EvoCamera *pCam)
{
	return m_AllEntsList;
}

void EvoWorld::GetAllEntitiesWithFlagsSet(EntityList *pEntityList, uint uiFlags)
{
	for (uint ui = 0; ui < m_AllEntsList.size(); ui ++)
	{
		if (m_AllEntsList[ui]->GetFlags() & uiFlags)
			pEntityList->push_back(m_AllEntsList[ui]);
	}
}

void EvoWorld::GetVisibleEntitiesWithFlagsSet(EntityList *pEntityList, uint uiFlags)
{
	for (uint ui = 0; ui < m_AllEntsList.size(); ui ++)
	{
		if (m_AllEntsList[ui]->GetFlags() & uiFlags)
			pEntityList->push_back(m_AllEntsList[ui]);
	}
}

void EvoWorld::GetEntitiesIntersectingFrustumWithCastedDir(EntityList *pEntityList, EvoFrustum frustum, EvoPoint3 pt3Dir)
{
	EvoPoint3 vDir = pt3Dir;

	for(unsigned int i=0; i<m_AllEntsList.size(); i++)
	{
		EvoEntity *pEntity = m_AllEntsList[i];
		if(!(pEntity->GetFlags() & ENTITY_CASTSHADOW)) 
			continue;

		// use sweep intersection
		if(1/*g_iVisibilityTest == VISTEST_ACCURATE*/)
		{
		  // test accurately
			if(!SweepIntersectionTest(pEntity->GetAABB(), frustum, vDir))
				continue;
		}
		/*else if(g_iVisibilityTest == VISTEST_CHEAP) {
		  // test only with AABB of frustum
			if(!SweepIntersectionTest(pObject->m_AABB, frustum.m_AABB, vDir))
				continue;
		}*/

		pEntityList->push_back(pEntity);
	}
}


// Helper function for AABB vs Frustum sweep test
//
//
bool ProjectedSweepIntersection(const EvoPoint3 &vHalfSize, const EvoPoint3 &vCenter, const EvoPoint3 *pFrustumPoints, const EvoPoint3 &vDir,
                                  float &fDistMin, float &fDistMax, const EvoPoint3 &vSweepDir)
{
  // project sweep direction
  float fSweepDir = vSweepDir.Dot(vDir);

  // project AABB center point to vector
  float fCenter = vCenter.Dot(vDir);
  // project AABB half-size to vector
  float fHalfSize = vHalfSize.m_fX * fabs(vDir.m_fX) +
                    vHalfSize.m_fY * fabs(vDir.m_fY) +
                    vHalfSize.m_fZ * fabs(vDir.m_fZ);

  float fMin1 = fCenter - fHalfSize;
  float fMax1 = fCenter + fHalfSize;

  // project frustum points
  float fProj2 = pFrustumPoints[0].Dot(vDir);
  float fMin2 = fProj2;
  float fMax2 = fProj2;
  for(int i=1;i<8;i++)
  {
    fProj2 = pFrustumPoints[i].Dot(vDir);
    fMin2 = MIN(fProj2, fMin2);
    fMax2 = MAX(fProj2, fMax2);
  }

  // sweep can affect intersection
  if(fSweepDir != 0)
  {
    // intersection starts when fMax1 + fSweepDir * t >= fMin2
    //
    float fIntersectionStart = (fMin2 - fMax1) / fSweepDir;

    // intersection ends when fMin1 + fSweepDir * t >= fMax2
    //
    float fIntersectionEnd = (fMax2 - fMin1) / fSweepDir;

    // ranges must be in right order
    if(fIntersectionStart > fIntersectionEnd) SWAP<float>(fIntersectionStart, fIntersectionEnd);

    // distance ranges don't overlap
    if(fDistMin > fIntersectionEnd || fIntersectionStart > fDistMax)
    {
      return false;
    }

    // otherwise merge ranges
    fDistMin = MAX(fDistMin, fIntersectionStart);
    fDistMax = MIN(fDistMax, fIntersectionEnd);
  }
  // sweep doesn't affect intersection
  else
  {
    // no intersectection ever
    if(fMin1 > fMax2 || fMin2 > fMax1)
    {
      return false;
    }
  }

  return true;
}

// AABB vs Frustum sweep test, returns true if intersection can occur if object is translated along given direction
//
//
bool SweepIntersectionTest(const EvoBoundingBox &objectBB, const EvoFrustum &frustum, const EvoPoint3 &vSweepDir)
{
  // Note that this code is very unoptimal
  //
  //

  EvoPoint3 vHalfSize = (objectBB.m_pt3Max - objectBB.m_pt3Min) * 0.5f;
  EvoPoint3 vCenter = (objectBB.m_pt3Min + objectBB.m_pt3Max) * 0.5f;

  float fDistMin = 0.0f;
  float fDistMax = FLT_MAX;

  // find potential intersection range in sweep direction
  if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, vSweepDir, fDistMin, fDistMax, vSweepDir)) return false;
  

  // AABB face normals
  if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, EvoPoint3(1,0,0), fDistMin, fDistMax, vSweepDir)) return false;
  if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, EvoPoint3(0,1,0), fDistMin, fDistMax, vSweepDir)) return false;
  if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, EvoPoint3(0,0,1), fDistMin, fDistMax, vSweepDir)) return false;

  // frustum face normals
  //

  // front and back faces:
  EvoPoint3 vNorm1 = (frustum.m_pPoints[1] - frustum.m_pPoints[0]).Cross(frustum.m_pPoints[3] - frustum.m_pPoints[0]);
  vNorm1.Unitize();
  if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, vNorm1, fDistMin, fDistMax, vSweepDir)) return false;

  // left face:
  EvoPoint3 vNorm2 = (frustum.m_pPoints[1] - frustum.m_pPoints[0]).Cross(frustum.m_pPoints[4] - frustum.m_pPoints[0]);
  vNorm2.Unitize();
  if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, vNorm2, fDistMin, fDistMax, vSweepDir)) return false;

  // right face:
  EvoPoint3 vNorm3 = (frustum.m_pPoints[2] - frustum.m_pPoints[3]).Cross(frustum.m_pPoints[7] - frustum.m_pPoints[3]);
  vNorm3.Unitize();
  if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, vNorm3, fDistMin, fDistMax, vSweepDir)) return false;

  // top face:
  EvoPoint3 vNorm4 = (frustum.m_pPoints[2] - frustum.m_pPoints[1]).Cross(frustum.m_pPoints[5] - frustum.m_pPoints[1]);
  vNorm4.Unitize();
  if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, vNorm4, fDistMin, fDistMax, vSweepDir)) return false;

  // bottom face:
  EvoPoint3 vNorm5 = (frustum.m_pPoints[3] - frustum.m_pPoints[0]).Cross(frustum.m_pPoints[4] - frustum.m_pPoints[0]);
  vNorm5.Unitize();
  if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, vNorm5, fDistMin, fDistMax, vSweepDir)) return false;


  // edge cross edge cases
  //
  EvoPoint3 pBoxEdges[3] = {EvoPoint3(1,0,0), EvoPoint3(0,1,0), EvoPoint3(0,0,1)};
  for(int i=0;i<3;i++)
  {
    // edge up-down
    EvoPoint3 vNorm1 =(frustum.m_pPoints[1] - frustum.m_pPoints[0]).Cross(pBoxEdges[i]);
	vNorm1.Unitize();
    if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, vNorm1, fDistMin, fDistMax, vSweepDir)) return false;

    // edge left-right
    EvoPoint3 vNorm2 = (frustum.m_pPoints[3] - frustum.m_pPoints[0]).Cross(pBoxEdges[i]);
	vNorm2.Unitize();
    if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, vNorm2, fDistMin, fDistMax, vSweepDir)) return false;

    // edge bottom left
    EvoPoint3 vNorm3 = (frustum.m_pPoints[4] - frustum.m_pPoints[0]).Cross(pBoxEdges[i]);
	vNorm3.Unitize();
    if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, vNorm3, fDistMin, fDistMax, vSweepDir)) return false;

    // edge top left
    EvoPoint3 vNorm4 = (frustum.m_pPoints[5] - frustum.m_pPoints[1]).Cross(pBoxEdges[i]);
	vNorm4.Unitize();
    if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, vNorm4, fDistMin, fDistMax, vSweepDir)) return false;

    // edge top right
    EvoPoint3 vNorm5 = (frustum.m_pPoints[6] - frustum.m_pPoints[2]).Cross(pBoxEdges[i]);
	vNorm5.Unitize();
    if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, vNorm5, fDistMin, fDistMax, vSweepDir)) return false;

    // edge bottom right
    EvoPoint3 vNorm6 = (frustum.m_pPoints[7] - frustum.m_pPoints[3]).Cross(pBoxEdges[i]);
	vNorm6.Unitize();
    if(!ProjectedSweepIntersection(vHalfSize, vCenter, frustum.m_pPoints, vNorm6, fDistMin, fDistMax, vSweepDir)) return false;
  }

  // all tests passed - intersection occurs
  return true;
}
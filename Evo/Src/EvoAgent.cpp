#include "EvoStdAfx.h"

EvoAgent::EvoAgent()
{
	m_pPath = NULL;
}

EvoAgent::~EvoAgent()
{

}

bool EvoAgent::MakePath(EvoPoint3 pt3Start, EvoPoint3 pt3End)
{
	SAFE_DELETE(m_pPath);

	m_pPath = new EvoPath();
	m_pPath->MakePath(m_pShape, pt3Start, pt3End);

	return m_pPath != NULL;
}

bool EvoAgent::MakeCurvedPath(EvoPoint3 pt3Start, EvoPoint3 pt3End, EvoPoint3 pt3StartFacing)
{
	SAFE_DELETE(m_pPath);

	m_pPath = new EvoPath();
	m_pPath->MakeCurvedPath(m_pShape, pt3Start, pt3End, pt3StartFacing);

	return m_pPath != NULL;
}

bool EvoAgent::AdvanceAlongPath(float fDistance, float &fX, float &fY)
{
	fDistance *= 10.0f;
	float fPrecisionX = 0.0f;
	float fPrecisionY = 0.0f;

	iCollisionInfo *pCI = m_pAgent->advanceAlongPathWithPrecision(m_pPath->GetPath(), fDistance, PATHING->GetContext(), fPrecisionX, fPrecisionY);

	cPosition Pos = m_pPath->GetPath()->position(0);

	fX = (Pos.x + fPrecisionX)/PATHING_SCALAR;
	fY = (Pos.y + fPrecisionY)/PATHING_SCALAR;

	return true;
}
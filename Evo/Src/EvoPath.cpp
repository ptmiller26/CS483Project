#include "EvoStdAfx.h"

EvoPath::EvoPath()
{
	m_pPath = NULL;
}

EvoPath::~EvoPath()
{
	delete m_pPath;
}

bool EvoPath::MakePath(iShape *pShape, EvoPoint3 pt3Start, EvoPoint3 pt3End)
{
	SAFE_DELETE(m_pPath);

	cPosition Start = PATHING->CreatePosition(pt3Start);
	cPosition Goal = PATHING->CreatePosition(pt3End);

	if (Start.cell != -1 && Goal.cell != -1)
	{
		Start = PATHING->GetMesh()->findClosestUnobstructedPosition(pShape, PATHING->GetContext(), Start, 300);
		Goal = PATHING->GetMesh()->findClosestUnobstructedPosition(pShape, PATHING->GetContext(), Goal, 300);

		m_pPath = PATHING->GetMesh()->findShortestPath(pShape, PATHING->GetContext(), Start, Goal);
	}

	return m_pPath != NULL;
}

bool EvoPath::MakeCurvedPath(iShape *pShape, EvoPoint3 pt3Start, EvoPoint3 pt3End, EvoPoint3 pt3StartFacing)
{
	iPath *pPath = NULL;
	MakePath(pShape, pt3Start, pt3End);

	pPath = PATHING->GetMesh()->generateCurvedPath(pShape, m_pPath, PATHING->GetContext(), (int)(pt3StartFacing.m_fX*1000), (int)(pt3StartFacing.m_fZ*1000), 2*10, 0.75f, 0.9f);
	delete m_pPath;
	m_pPath = pPath;

	return m_pPath != NULL;
}

bool EvoPath::AdvanceAlongPath(iShape *pShape, float fDistance, float &fX, float &fY)
{
	fDistance *= 10.0f;
	float fPrecisionX = 0.0f;
	float fPrecisionY = 0.0f;

	iCollisionInfo *pCI = m_pPath->advanceAlong(pShape, fDistance, PATHING->GetContext(), fPrecisionX, fPrecisionY);

	cPosition Pos = m_pPath->position(0);

	fX = (Pos.x + fPrecisionX)/PATHING_SCALAR;
	fY = (Pos.y + fPrecisionY)/PATHING_SCALAR;

	return true;
}
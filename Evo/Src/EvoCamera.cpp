#include "EvoStdAfx.h"

EvoCamera::EvoCamera():
m_bDirtyWorldMtx(true)
{
	m_WorldMatrix.SetIdentity();
	m_ViewMatrix.SetIdentity();
	SetParams((float)D3DXToRadian(60.0f), 16.0f/9.0f, 1.0f, 1000.0f);
}

EvoCamera::~EvoCamera()
{
}

void EvoCamera::SetParams(float fFOV, float fAspect, float fNear, float fFar)
{
	m_fFOV = fFOV;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	m_ProjMatrix.SetPerspectiveFovLH(fFOV, fAspect, fNear, fFar);
}

void EvoCamera::SetParamsOrtho(float fWidth, float fHeight, float fNear, float fFar)
{
	m_ProjMatrix.SetOrthoFovLH(fWidth, fHeight, fNear, fFar);
}

void EvoCamera::LookAt(EvoPoint3* lpEye, EvoPoint3* lpAt)
{
	EvoPoint3 Up(0.0F, 1.0F, 0.0F);
	m_WorldMatrix.SetLookAtLH(lpEye, lpAt, &Up); 
}
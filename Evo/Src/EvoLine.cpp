#include "EvoStdAfx.h"

EvoLine::EvoLine()
{
}

EvoLine::EvoLine(const EvoPoint3 &pt3Start, const EvoPoint3 &pt3End)
{
	m_pt3Start = pt3Start;
	m_pt3End = pt3End;
}

EvoLine::~EvoLine()
{
}

bool EvoLine::Intersect2D(const EvoLine &Line, EvoPoint3 *pIntersection, float *pfTime)
{
	float fT, fV;

	//generate the parametric equation for the first line
	float fCoefficient_tx = m_pt3Start.m_fX - m_pt3End.m_fX;
	float fCoefficient_tz = m_pt3Start.m_fZ - m_pt3End.m_fZ;

	//generate the parametric equations for the second line
	float fCoefficient_vx = -(Line.m_pt3Start.m_fX - Line.m_pt3End.m_fX);
	float fCoefficient_vz = -(Line.m_pt3Start.m_fZ - Line.m_pt3End.m_fZ);

	//solve the first two equations simultaneously for fT and fV
	//seems confusing but see p719 of Calculas book, actually very simple
	float fScalar;

	//if either one is a point
	if((!fCoefficient_vx && !fCoefficient_vz) || (!fCoefficient_tx && !fCoefficient_tz))
		return FALSE;

	//both parallel to z axis
	if(!fCoefficient_vx && !fCoefficient_tx)
	{
		//same line
		if(m_pt3Start.m_fX == Line.m_pt3Start.m_fX)
		{
			if((m_pt3Start.m_fZ <= Line.m_pt3Start.m_fZ && m_pt3Start.m_fX >= Line.m_pt3End.m_fZ) || (m_pt3Start.m_fZ <= Line.m_pt3End.m_fZ && m_pt3End.m_fZ >= Line.m_pt3End.m_fZ)
				|| (m_pt3End.m_fZ <= Line.m_pt3Start.m_fZ && m_pt3End.m_fZ >= Line.m_pt3End.m_fZ) || (m_pt3End.m_fZ <= Line.m_pt3End.m_fZ && m_pt3End.m_fZ >= Line.m_pt3Start.m_fZ))
			{
				pIntersection->m_fX = m_pt3Start.m_fX;
				pIntersection->m_fZ = (m_pt3Start.m_fZ + m_pt3End.m_fZ + Line.m_pt3Start.m_fZ + Line.m_pt3End.m_fZ) / 4;

				return true;
			}
		}
		else
			return false;
	}

	//both parallel to m_fX axis
	if(!fCoefficient_vz && !fCoefficient_tz)
	{
		//same line
		if(m_pt3Start.m_fZ == Line.m_pt3Start.m_fZ)
		{
			if((m_pt3Start.m_fX <= Line.m_pt3Start.m_fX && m_pt3Start.m_fX >= Line.m_pt3End.m_fX) || (m_pt3Start.m_fX <= Line.m_pt3End.m_fX && m_pt3Start.m_fX >= Line.m_pt3Start.m_fX)
				|| (m_pt3End.m_fX <= Line.m_pt3Start.m_fX && m_pt3End.m_fX >= Line.m_pt3End.m_fX) || (m_pt3End.m_fX <= Line.m_pt3End.m_fX && m_pt3End.m_fX >= Line.m_pt3Start.m_fX))
			{
				pIntersection->m_fX = (m_pt3Start.m_fX + m_pt3End.m_fX + Line.m_pt3Start.m_fX + Line.m_pt3End.m_fX) / 4;
				pIntersection->m_fZ = m_pt3Start.m_fZ;

				return TRUE;
			}
		}
		else
			return FALSE;
	}

	//parallel to one another
	if(fCoefficient_vx && fCoefficient_tx)
	{
		float fSlope1 = fCoefficient_tz / fCoefficient_tx;
		float fSlope2 = fCoefficient_vz / fCoefficient_vx;
		if(fSlope1 == fSlope2)
			return FALSE;
	}

	//solve for t
	if(fCoefficient_vz && fCoefficient_vx)
	{
		fScalar = -fCoefficient_vz / fCoefficient_vx;
		fT = ((Line.m_pt3Start.m_fX - m_pt3Start.m_fX) * fScalar + (Line.m_pt3Start.m_fZ - m_pt3Start.m_fZ)) / (fCoefficient_tx * fScalar + fCoefficient_tz);
	}
	else if(fCoefficient_vx)
		fT = (Line.m_pt3Start.m_fZ - m_pt3Start.m_fZ) / fCoefficient_tz;
	else if(fCoefficient_vz)
		fT = (Line.m_pt3Start.m_fX - m_pt3Start.m_fX) / fCoefficient_tx;
	
	//solve for v
	if(fCoefficient_tx && fCoefficient_tz)
	{
		fScalar = -fCoefficient_tz / fCoefficient_tx;
		fV = ((Line.m_pt3Start.m_fX - m_pt3Start.m_fX) * fScalar + (Line.m_pt3Start.m_fZ - m_pt3Start.m_fZ)) / (fCoefficient_vx * fScalar + fCoefficient_vz);
	}
	else if(fCoefficient_tz)
		fV = (Line.m_pt3Start.m_fX - m_pt3Start.m_fX) / fCoefficient_vx;
	else if(fCoefficient_tx)
		fV = (Line.m_pt3Start.m_fZ - m_pt3Start.m_fZ) / fCoefficient_vz;

	//return the intersection if t and v fall between 0 and -1
	if((fT <=0 && fT >= -1) && (fV <= 0 && fV >= -1))
	{
		pIntersection->m_fX = m_pt3Start.m_fX + fCoefficient_tx * fT;
//		pIntersection->m_fY = m_pt3Start.m_fY + fCoefficient_ty * fT;
		pIntersection->m_fZ = m_pt3Start.m_fZ + fCoefficient_tz * fT;

		if(pfTime)
			*pfTime = fT;

		return true;
	}
	
	return false;
}
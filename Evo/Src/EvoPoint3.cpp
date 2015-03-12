#include "EvoStdAfx.h"

EvoPoint3 EvoPoint3::operator+(const EvoPoint3 &rhs) const
{
    EvoPoint3 pt3Temp;
    pt3Temp.m_fX = m_fX + rhs.m_fX;
    pt3Temp.m_fY = m_fY + rhs.m_fY;
	pt3Temp.m_fZ = m_fZ + rhs.m_fZ;
    return pt3Temp;
}

EvoPoint3 EvoPoint3::operator-(const EvoPoint3 &rhs) const
{
    EvoPoint3 pt3Temp;
    pt3Temp.m_fX = m_fX - rhs.m_fX;
    pt3Temp.m_fY = m_fY - rhs.m_fY;
	pt3Temp.m_fZ = m_fZ - rhs.m_fZ;
    return pt3Temp;
}

EvoPoint3 EvoPoint3::operator*(const float &rhs) const
{
    EvoPoint3 pt3Temp;
    pt3Temp.m_fX = m_fX * rhs;
    pt3Temp.m_fY = m_fY * rhs;
	pt3Temp.m_fZ = m_fZ * rhs;
    return pt3Temp;
}

EvoPoint3 EvoPoint3::operator*(const EvoMatrix &rhs) const
{
	EvoPoint3 pt3Temp = *this;
    pt3Temp.Multiply(&rhs);
    return pt3Temp;
}
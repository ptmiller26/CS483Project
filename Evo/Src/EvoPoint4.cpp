#include "EvoStdAfx.h"

EvoPoint4 EvoPoint4::operator+(const EvoPoint4 &rhs)
{
    EvoPoint4 pt4Temp;
    pt4Temp.m_fX = m_fX + rhs.m_fX;
    pt4Temp.m_fY = m_fY + rhs.m_fY;
	pt4Temp.m_fZ = m_fZ + rhs.m_fZ;
	pt4Temp.m_fW = m_fW + rhs.m_fW;
    return pt4Temp;
}

EvoPoint4 EvoPoint4::operator-(const EvoPoint4 &rhs)
{
    EvoPoint4 pt4Temp;
    pt4Temp.m_fX = m_fX - rhs.m_fX;
    pt4Temp.m_fY = m_fY - rhs.m_fY;
	pt4Temp.m_fZ = m_fZ - rhs.m_fZ;
	pt4Temp.m_fW = m_fW - rhs.m_fW;
    return pt4Temp;
}

EvoPoint4 EvoPoint4::operator*(const float &rhs)
{
    EvoPoint4 pt4Temp;
    pt4Temp.m_fX = m_fX * rhs;
    pt4Temp.m_fY = m_fY * rhs;
	pt4Temp.m_fZ = m_fZ * rhs;
	pt4Temp.m_fW = m_fW * rhs;
    return pt4Temp;
}
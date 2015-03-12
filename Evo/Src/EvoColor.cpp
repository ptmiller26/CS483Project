#include "EvoStdAfx.h"

EvoColor::EvoColor()
{
	m_Color = D3DCOLOR_ARGB(255, 255, 255, 255);
}

EvoColor::EvoColor(uchar ucR, uchar ucG, uchar ucB, uchar ucA)
{
	m_Color = D3DCOLOR_ARGB(ucA, ucR, ucG, ucB);
}

EvoColor::~EvoColor()
{

}
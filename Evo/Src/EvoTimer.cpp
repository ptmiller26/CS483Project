#include "EvoStdAfx.h"

EvoTimer::EvoTimer()
{
	LARGE_INTEGER  liFrequency;

	QueryPerformanceFrequency(&liFrequency);
	m_llFrequency = liFrequency.QuadPart;
	QueryPerformanceCounter(&m_llCounter);
}

EvoTimer::~EvoTimer()
{

}
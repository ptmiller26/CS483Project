#include "EvoStdAfx.h"

EvoRenderable::EvoRenderable()
{
	m_pNode = NULL;
}

EvoRenderable::~EvoRenderable()
{
	if (m_pNode)
	{
		m_pNode->SetRenderable(NULL);
	}
}


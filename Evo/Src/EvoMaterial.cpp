#include "EvoStdAfx.h"

EvoMaterial::EvoMaterial()
{
	m_pShader = NULL;
	m_iTechnique = -1;
}

EvoMaterial::~EvoMaterial()
{
	RESMGR->DestroyShader(m_pShader);
}

void EvoMaterial::BindTextures()
{
	for (uint ui = 0; ui < m_apTextures.size(); ui++)
	{
		if (m_apTextures[ui])
		{
			EVO->BindToPS(ui, m_apTextures[ui]);
		}
	}
}

void EvoMaterial::UnbindTextures()
{
	for (uint ui = 0; ui < m_apTextures.size(); ui++)
	{
		if (m_apTextures[ui])
		{
			EVO->UnbindPS(ui);
		}
	}
}
#include "EvoStdAfx.h"


EvoNode::EvoNode()
{
	m_pParent		= NULL;
	m_szName		= NULL;
	m_pRenderable	= NULL;
	m_uiNumChildren	= 0;

	m_mtxLocal.SetIdentity();
	m_mtxWorld.SetIdentity();
}

EvoNode::~EvoNode()
{
	if (m_pParent) {
		m_pParent->RemoveChild(this);
		m_pParent = NULL;
	}

	while(m_apChildren.size())
	{
		EvoNode* pCurrent = m_apChildren.back();
		m_apChildren.pop_back();
        		
		delete pCurrent;
	}
}

void EvoNode::SetName(char *szName)
{
	if (m_szName)
		delete m_szName;

	m_szName = szName;
}

void EvoNode::RemoveChild(EvoNode *pNode)
{
	std::vector <EvoNode *>::iterator Iter = m_apChildren.begin();

	while (Iter != m_apChildren.end())
	{
		if (*Iter == pNode)
		{
			(*Iter)->m_pParent = NULL;
			m_apChildren.erase(Iter);
			m_uiNumChildren--;
			return;
		}
	}
}

void EvoNode::AddChild(EvoNode *pNode)
{
	m_uiNumChildren++;
	m_apChildren.push_back(pNode);
	
	if (pNode->m_pParent)
		pNode->m_pParent->RemoveChild(pNode);

	pNode->m_pParent = this;
}

void EvoNode::Render()
{
	if (m_pRenderable)
		m_pRenderable->Render();

	for (uint ui = 0; ui < m_uiNumChildren; ui++)
		m_apChildren[ui]->Render();
}

void EvoNode::Update(float fDeltaT)
{
	if (m_pParent)
	{
		m_mtxLocal.Multiply(m_pParent->GetWorldMatrix(), &m_mtxWorld);
	}
	else
	{
		m_mtxWorld = m_mtxLocal;
	}

	for (uint ui = 0; ui < m_uiNumChildren; ui++)
	{
		m_apChildren[ui]->Update(fDeltaT);
	}

	if (m_pRenderable)
	{
		m_pRenderable->Update(fDeltaT);
	}
}

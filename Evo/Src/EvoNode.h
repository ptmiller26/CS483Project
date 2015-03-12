#ifndef __EVONODE_H_
#define __EVONODE_H_

class EvoNode;

class EvoNode
{
   public:
	EvoNode();
	~EvoNode();

	uint GetNumChildren()								{ return m_uiNumChildren;			}
	void SetNumChildren(uint uiNumChildren)				{ m_uiNumChildren = uiNumChildren;	}
	void AddChild(EvoNode *pNode);
	void RemoveChild(EvoNode *pNode);

	EvoNode *GetChild(uint uiChild)						{ return m_apChildren[uiChild];		}

	EvoNode *GetParent()								{ return m_pParent;					}

	char *GetName()										{ return m_szName;					}

	void Render();

	void Update(float fDeltaT);

	void SetMatrix(EvoMatrix &Matrix)					{ m_mtxLocal = Matrix;	}
	EvoMatrix *GetMatrix()								{ return &m_mtxLocal;	}

	EvoMatrix *GetWorldMatrix()							{ return &m_mtxWorld;	}

	// The Node owns the pointer and will delete it
	void SetName(char *szName);

	void SetRenderable(EvoRenderable *pRenderable);
	EvoRenderable *GetRenderable()						{ return m_pRenderable;	}

   protected:

   protected:
	char *						m_szName;
	EvoNode *					m_pParent;

	uint						m_uiNumChildren;
	std::vector <EvoNode*>		m_apChildren;

	EvoRenderable *				m_pRenderable;

	EvoMatrix					m_mtxLocal;
	EvoMatrix					m_mtxWorld;

	friend class EvoResourceManager;
};

inline void EvoNode::SetRenderable(EvoRenderable *pRenderable)
{ 
	if (m_pRenderable)
	{
		m_pRenderable->SetOwner(NULL);
	}

	m_pRenderable = pRenderable;
	
	if (m_pRenderable)
	{
		pRenderable->SetOwner(this);
	}
}

#endif

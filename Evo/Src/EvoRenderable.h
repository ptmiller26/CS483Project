#ifndef _EVORENDERABLE_H_
#define _EVORENDERABLE_H_

class EvoNode;

namespace EvoRenderableType
{
	enum ERenderableType
	{
		GRANNY,
	};
};

class EvoRenderable
{
   public:
	EvoRenderable();
	virtual ~EvoRenderable();
   
	virtual void Update(float fDeltaT) = 0;
	virtual void Render() = 0;
 
	EvoNode *GetOwner();

	virtual int GetNumVerts()		{ return 0; }
	virtual int GetNumFaces()		{ return 0; }
	virtual int GetVertStride()		{ return 0; }
	virtual int GetFaceStride()		{ return 0;	}
	virtual void *GetVerts()		{ return NULL;	}
	virtual void *GetFaces()		{ return NULL;	}

   protected:
	virtual void SetOwner(EvoNode *pNode);

   protected:
	EvoNode *m_pNode;

	EvoRenderableType::ERenderableType m_eType;

	friend class EvoNode;
};

inline void EvoRenderable::SetOwner(EvoNode *pNode)
{
	m_pNode = pNode;
}

inline EvoNode *EvoRenderable::GetOwner()
{
	return m_pNode;
}

#endif
#ifndef _EVOCAMERA_H_
#define _EVOCAMERA_H_

class EvoCamera
{
   public:
	EvoCamera();
	~EvoCamera();

	void SetParams(float fFOV, float fAspect, float fNear, float fFar);
	void GetParams(float *fFOV, float *fAspect, float *fNear, float *fFar);
	void SetParamsOrtho(float fWidth, float fHeight, float fNear, float fFar);

	float GetAspect()	{ return m_fAspect;	}
	float GetFOV()		{ return m_fFOV;	}

	void LookAt(EvoPoint3* lpEye, EvoPoint3* lpAt);

	void  SetWorldMatrix(const EvoMatrix *pMat);				
	const EvoMatrix *GetWorldMatrix()					{ return &m_WorldMatrix;}
	const EvoMatrix *GetInvViewMatrix()					{ return &m_WorldMatrix;}
	const EvoMatrix *GetViewMatrix();					
	const EvoMatrix *GetProjMatrix()					{ return &m_ProjMatrix;	}

	EvoMatrix *GetProjMatrixPtr()						{ return &m_ProjMatrix;	}

	void SetRotateYPR(float fYaw, float fPitch, float fRoll); 
	void Translate(const EvoPoint3 &pt3);


	void GetFacingVector(EvoPoint3 *pPt3Facing);
	void GetUpVector(EvoPoint3 *pPt3Up);
	void GetRightVector(EvoPoint3 *pPt3Right);
	void GetTranslate(EvoPoint3 *pTranslate);
	
   protected:
    EvoMatrix   m_WorldMatrix;  //aka InvView
	EvoMatrix	m_ViewMatrix;
	EvoMatrix	m_ProjMatrix;

	bool  m_bDirtyWorldMtx;		// Any changes to the current worldMat.  Used to sync the ViewMtx
	float m_fFOV;
	float m_fAspect;
	float m_fNear;
	float m_fFar;

};



// Inlines
inline void EvoCamera::GetParams(float *fFOV, float *fAspect, float *fNear, float *fFar)
{
	*fFOV = m_fFOV;
	*fAspect = m_fAspect;
	*fNear = m_fNear;
	*fFar = m_fFar;
}

inline void EvoCamera::GetRightVector(EvoPoint3 *pPt3Right)
{
	m_WorldMatrix.GetRow(0, pPt3Right);
}

inline void EvoCamera::GetUpVector(EvoPoint3 *pPt3Up)
{
	m_WorldMatrix.GetRow(1, pPt3Up);
}

inline void EvoCamera::GetFacingVector(EvoPoint3 *pPt3Facing)
{
	m_WorldMatrix.GetRow(2, pPt3Facing);
}

inline void EvoCamera::GetTranslate(EvoPoint3 *pTranslate)
{
	m_WorldMatrix.GetRow(3, pTranslate);
}

inline void EvoCamera::SetRotateYPR(float fYaw, float fPitch, float fRoll)
{ 
	m_bDirtyWorldMtx = true; 
	m_WorldMatrix.SetRotateYPR(fYaw,fPitch,fRoll);
}

inline void EvoCamera::Translate(const EvoPoint3 &pt3) 
{	
	m_bDirtyWorldMtx = true; 
	m_WorldMatrix.Translate(pt3);
}
inline const EvoMatrix *EvoCamera::GetViewMatrix()					
{
	if(m_bDirtyWorldMtx)
	{
		m_ViewMatrix = m_WorldMatrix;
		m_ViewMatrix.Inverse();
		m_bDirtyWorldMtx = false;
	}
	return &m_ViewMatrix;	
}
inline void EvoCamera::SetWorldMatrix(const EvoMatrix *pMat)
{
	m_bDirtyWorldMtx = true;
	m_WorldMatrix = *pMat;
}

#endif

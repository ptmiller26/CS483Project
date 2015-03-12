#ifndef _EVOMATRIX_H_
#define _EVOMATRIX_H_

class EvoMatrix
{
   public:
	EvoMatrix() {};
	
	EvoMatrix( float f11, float f12, float f13, float f14,
               float f21, float f22, float f23, float f24,
               float f31, float f32, float f33, float f34,
               float f41, float f42, float f43, float f44 );

	~EvoMatrix() {};

	void SetIdentity();

	// Multiply Matrix by pMat(this*pMat)
	void Multiply(const EvoMatrix *pMat);
	void Multiply(const EvoMatrix *pMat, EvoMatrix *pResult);

	void SetRotateX(float fAngle);
	void SetRotateY(float fAngle);
	void SetRotateZ(float fAngle);
	void SetRotateYPR(float fYaw, float fPitch, float fRoll);

	void SetRotateAxis(float fAngle, const EvoPoint3 &pt3Axis);

	void SetScale(float fX, float fY, float fZ);
	void SetScale(const EvoPoint3 &pt3);

	void SetTranslate(float fX, float fY, float fZ);
	void SetTranslate(const EvoPoint3 &pt3);
    
	void Scale(const EvoPoint3 &pt3);
	void Scale(float fX, float fY, float fZ);

	void Translate(const EvoPoint3 &pt3);
	void Translate(float fX, float fY, float fZ);

	void GetTranslate(EvoPoint3 *pPt3Translate);

	void Transpose();

	void Inverse();

	void SetLookAtLH(EvoPoint3 *pEyePos, EvoPoint3 *pTargetPos, EvoPoint3 *pUpVector);

	void SetPerspectiveFovLH(float fFOV, float fAspectRatio, float fNear, float fFar);
	void SetOrthoFovLH(float fWidth, float fHeight, float fNear, float fFar);

	void GetCol(uint uiColumn, EvoPoint3 *pPt3V);
	void GetRow(uint uiRow, EvoPoint3 *pPt3V);

	void SetCol(uint uiColumn, EvoPoint3 *pPt3V);
	void SetRow(uint uiRow, EvoPoint3 *pPt3V);

	float Get33()			{ return m_Matrix._33;		}
	float Get43()			{ return m_Matrix._43;		}

	void Set33(float fVal)	{	m_Matrix._33 = fVal;	}
	void Set43(float fVal)	{	m_Matrix._43 = fVal;	}

	D3DXMATRIXA16 *GetD3DMatrix() { return &m_Matrix;	}

   protected:
	D3DXMATRIXA16 m_Matrix;
};


// Inlines 

inline EvoMatrix::EvoMatrix( float f11, float f12, float f13, float f14,
               float f21, float f22, float f23, float f24,
               float f31, float f32, float f33, float f34,
               float f41, float f42, float f43, float f44 )
{
    m_Matrix._11 = f11; m_Matrix._12 = f12; m_Matrix._13 = f13; m_Matrix._14 = f14;
    m_Matrix._21 = f21; m_Matrix._22 = f22; m_Matrix._23 = f23; m_Matrix._24 = f24;
    m_Matrix._31 = f31; m_Matrix._32 = f32; m_Matrix._33 = f33; m_Matrix._34 = f34;
    m_Matrix._41 = f41; m_Matrix._42 = f42; m_Matrix._43 = f43; m_Matrix._44 = f44;
}

inline void EvoMatrix::SetIdentity()
{
	D3DXMatrixIdentity(&m_Matrix);
}

inline void EvoMatrix::Multiply(const EvoMatrix *pMat)
{
	D3DXMatrixMultiply(&m_Matrix, &m_Matrix, const_cast<EvoMatrix *>(pMat)->GetD3DMatrix());
}

inline void EvoMatrix::Multiply(const EvoMatrix *pMat, EvoMatrix *pResult)
{
	D3DXMatrixMultiply(pResult->GetD3DMatrix(), &m_Matrix, const_cast<EvoMatrix *>(pMat)->GetD3DMatrix());
}

inline void EvoMatrix::SetRotateX(float fAngle)
{
	D3DXMatrixRotationX(&m_Matrix, fAngle);
}

inline void EvoMatrix::SetRotateY(float fAngle)
{
	D3DXMatrixRotationY(&m_Matrix, fAngle);
}

inline void EvoMatrix::SetRotateZ(float fAngle)
{
	D3DXMatrixRotationZ(&m_Matrix, fAngle);
}

inline void EvoMatrix::SetRotateYPR(float fYaw, float fPitch, float fRoll)
{
	D3DXMatrixRotationYawPitchRoll(&m_Matrix, fYaw, fPitch, fRoll);
}

inline void EvoMatrix::SetRotateAxis(float fAngle, const EvoPoint3 &pt3Axis)
{
	D3DXMatrixRotationAxis(&m_Matrix, (D3DXVECTOR3 *)&pt3Axis, fAngle);
}

inline void EvoMatrix::SetScale(float fX, float fY, float fZ)
{
	D3DXMatrixScaling(&m_Matrix, fX, fY, fZ);
}

inline void EvoMatrix::SetScale(const EvoPoint3 &pt3)
{
	SetScale(pt3.m_fX, pt3.m_fY, pt3.m_fZ);
}

inline void EvoMatrix::Scale(const EvoPoint3 &pt3)
{
	Scale(pt3.m_fX, pt3.m_fY, pt3.m_fZ);
}

inline void EvoMatrix::Scale(float fX, float fY, float fZ)
{
	D3DXMATRIX m;
	D3DXMatrixScaling(&m, fX, fY, fZ);
	D3DXMatrixMultiply(&m_Matrix, &m_Matrix, &m);
}

inline void EvoMatrix::SetTranslate(float fX, float fY, float fZ)
{
	D3DXMatrixTranslation(&m_Matrix, fX, fY, fZ);
}

inline void EvoMatrix::SetTranslate(const EvoPoint3 &pt3)
{
	SetTranslate(pt3.m_fX, pt3.m_fY, pt3.m_fZ);
}

inline void EvoMatrix::Translate(const EvoPoint3 &pt3)
{
	m_Matrix._41 += pt3.m_fX;
	m_Matrix._42 += pt3.m_fY;
	m_Matrix._43 += pt3.m_fZ;
}

inline void EvoMatrix::Translate(float fX, float fY, float fZ)
{
	m_Matrix._41 += fX;
	m_Matrix._42 += fY;
	m_Matrix._43 += fZ;
}

inline void EvoMatrix::SetLookAtLH(EvoPoint3 *pEyePos, EvoPoint3 *pTargetPos, EvoPoint3 *pUpVector)
{
	D3DXMatrixLookAtLH(&m_Matrix, (D3DXVECTOR3*)pEyePos, (D3DXVECTOR3*)pTargetPos, (D3DXVECTOR3*)pUpVector); 
}

inline void EvoMatrix::SetPerspectiveFovLH(float fFOV, float fAspectRatio, float fNear, float fFar)
{
	D3DXMatrixPerspectiveFovLH(&m_Matrix, fFOV, fAspectRatio, fNear, fFar);
}

inline void EvoMatrix::SetOrthoFovLH(float fWidth, float fHeight, float fNear, float fFar)
{
	D3DXMatrixOrthoLH(&m_Matrix, fWidth, fHeight, fNear, fFar);
}


inline void EvoMatrix::GetTranslate(EvoPoint3 *pPt3Translate)
{
	pPt3Translate->m_fX = m_Matrix._41;
	pPt3Translate->m_fY = m_Matrix._42;
	pPt3Translate->m_fZ = m_Matrix._43;
}

inline void EvoMatrix::Transpose()
{
	D3DXMatrixTranspose(&m_Matrix, &m_Matrix);
}

inline void EvoMatrix::Inverse()
{
	D3DXMatrixInverse(&m_Matrix, NULL, &m_Matrix);
}

inline void EvoMatrix::GetCol(uint uiColumn, EvoPoint3 *pPt3V)
{
	if (uiColumn == 0)
	{
		pPt3V->m_fX = m_Matrix._11;
		pPt3V->m_fY = m_Matrix._21;
		pPt3V->m_fZ = m_Matrix._31;
	}
	else if (uiColumn == 1)
	{
		pPt3V->m_fX = m_Matrix._12;
		pPt3V->m_fY = m_Matrix._22;
		pPt3V->m_fZ = m_Matrix._32;
	}
	else if (uiColumn == 2)
	{
		pPt3V->m_fX = m_Matrix._13;
		pPt3V->m_fY = m_Matrix._23;
		pPt3V->m_fZ = m_Matrix._33;
	}
	else if (uiColumn == 3)
	{
		pPt3V->m_fX = m_Matrix._14;
		pPt3V->m_fY = m_Matrix._24;
		pPt3V->m_fZ = m_Matrix._34;
	}
	else
		assert(0);
}

inline void EvoMatrix::GetRow(uint uiRow, EvoPoint3 *pPt3V)
{
	if (uiRow == 0)
	{
		pPt3V->m_fX = m_Matrix._11;
		pPt3V->m_fY = m_Matrix._12;
		pPt3V->m_fZ = m_Matrix._13;
	}
	else if (uiRow == 1)
	{
		pPt3V->m_fX = m_Matrix._21;
		pPt3V->m_fY = m_Matrix._22;
		pPt3V->m_fZ = m_Matrix._23;
	}
	else if (uiRow == 2)
	{
		pPt3V->m_fX = m_Matrix._31;
		pPt3V->m_fY = m_Matrix._32;
		pPt3V->m_fZ = m_Matrix._33;
	}
	else if (uiRow == 3)
	{
		pPt3V->m_fX = m_Matrix._41;
		pPt3V->m_fY = m_Matrix._42;
		pPt3V->m_fZ = m_Matrix._43;
	}
	else
		assert(0);
}

inline void EvoMatrix::SetCol(uint uiColumn, EvoPoint3 *pPt3V)
{
	if (uiColumn == 0)
	{
		m_Matrix._11 = pPt3V->m_fX;
		m_Matrix._21 = pPt3V->m_fY;
		m_Matrix._31 = pPt3V->m_fZ;
	}
	else if (uiColumn == 1)
	{
		m_Matrix._12 = pPt3V->m_fX;
		m_Matrix._22 = pPt3V->m_fY;
		m_Matrix._32 = pPt3V->m_fZ;
	}
	else if (uiColumn == 2)
	{
		m_Matrix._13 = pPt3V->m_fX;
		m_Matrix._23 = pPt3V->m_fY;
		m_Matrix._33 = pPt3V->m_fZ;
	}
}

inline void EvoMatrix::SetRow(uint uiRow, EvoPoint3 *pPt3V)
{
	if (uiRow == 0)
	{
		m_Matrix._11 = pPt3V->m_fX; 
		m_Matrix._12 = pPt3V->m_fY; 
		m_Matrix._13 = pPt3V->m_fZ; 
	}
	else if (uiRow == 1)
	{
		m_Matrix._21 = pPt3V->m_fX;
		m_Matrix._22 = pPt3V->m_fY;
		m_Matrix._23 = pPt3V->m_fZ;
	}
	else if (uiRow == 2)
	{
		m_Matrix._31 = pPt3V->m_fX;
		m_Matrix._32 = pPt3V->m_fY;
		m_Matrix._33 = pPt3V->m_fZ;
	}
}


#endif
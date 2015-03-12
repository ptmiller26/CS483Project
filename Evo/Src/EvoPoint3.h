#ifndef _EVOPOINT3_H_
#define _EVOPOINT3_H_

class EvoMatrix;

class EvoPoint3 {
   public:
	EvoPoint3() {};
	EvoPoint3(float fX, float fY, float fZ) { m_fX = fX, m_fY = fY, m_fZ = fZ; }

	~EvoPoint3() {};

	void Add(const EvoPoint3 &pt3);
	void Subtract(const EvoPoint3 &pt3);

	void AddScaled(const EvoPoint3 &pt3, float fScale);

	void Scale(float fScale);
	void Scale(const EvoPoint3 &pt3);

	void Multiply(const EvoMatrix *pMatrix);

	float Dot(const EvoPoint3 &pt3) const;

	EvoPoint3 Cross(const EvoPoint3 &pt3);

	float Unitize();

	float Length();
	float LengthSqrd();

	float Distance(const EvoPoint3 &pt3);
	float DistanceSqrd(const EvoPoint3 &pt3);

	// Operators
	EvoPoint3 operator+(const EvoPoint3 &rhs) const;
	EvoPoint3 operator-(const EvoPoint3 &rhs) const;
	EvoPoint3 operator*(const float &rhs) const;
	EvoPoint3 operator*(const EvoMatrix &rhs) const;

   public:
	float m_fX, m_fY, m_fZ;
};

inline void EvoPoint3::Add(const EvoPoint3 &pt3)
{
	m_fX += pt3.m_fX;
	m_fY += pt3.m_fY;
	m_fZ += pt3.m_fZ;
}

inline void EvoPoint3::Subtract(const EvoPoint3 &pt3)
{
	m_fX -= pt3.m_fX;
	m_fY -= pt3.m_fY;
	m_fZ -= pt3.m_fZ;
}

inline void EvoPoint3::AddScaled(const EvoPoint3 &pt3, float fScale)
{
	m_fX += pt3.m_fX*fScale;
	m_fY += pt3.m_fY*fScale;
	m_fZ += pt3.m_fZ*fScale;
}

inline void EvoPoint3::Scale(float fScale)
{
	m_fX *= fScale;
	m_fY *= fScale;
	m_fZ *= fScale;
}

inline void EvoPoint3::Scale(const EvoPoint3 &pt3)
{
	m_fX *= pt3.m_fX;
	m_fY *= pt3.m_fY;
	m_fZ *= pt3.m_fZ;
}

inline void EvoPoint3::Multiply(const EvoMatrix *pMatrix)
{
	D3DXVECTOR4 pTmp;

	D3DXVec3Transform(&pTmp, (CONST D3DXVECTOR3 *)this, (CONST D3DXMATRIX *)pMatrix);

	m_fX = pTmp.x;
	m_fY = pTmp.y;
	m_fZ = pTmp.z;
}

inline float EvoPoint3::Unitize()
{
	float fLength = Length();

	m_fX /= fLength;
	m_fY /= fLength;
	m_fZ /= fLength;

	return fLength;
}

inline float EvoPoint3::Length()
{
	return sqrtf(LengthSqrd());
}

inline float EvoPoint3::LengthSqrd()
{
	return m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ;
}

inline float EvoPoint3::Distance(const EvoPoint3 &pt3)
{
	return sqrtf(DistanceSqrd(pt3));
}

inline float EvoPoint3::DistanceSqrd(const EvoPoint3 &pt3)
{
	return SQR(m_fX - pt3.m_fX) + SQR(m_fY - pt3.m_fY) + SQR(m_fZ - pt3.m_fZ);
}

inline float EvoPoint3::Dot(const EvoPoint3 &pt3) const
{
	return (m_fX*pt3.m_fX + m_fY*pt3.m_fY + m_fZ*pt3.m_fZ);
}

inline EvoPoint3 EvoPoint3::Cross(const EvoPoint3 &pt3)
{
	EvoPoint3 pt3Result;

	D3DXVec3Cross((D3DXVECTOR3 *)&pt3Result, (CONST D3DXVECTOR3 *)this, (CONST D3DXVECTOR3 *)&pt3);

	return pt3Result;
}

#endif
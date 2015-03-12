#ifndef _EVOPOINT4_H_
#define _EVOPOINT4_H_

class EvoMatrix;

class EvoPoint4 {
   public:
	EvoPoint4() {};
	EvoPoint4(float fX, float fY, float fZ, float fW) { m_fX = fX; m_fY = fY; m_fZ = fZ; m_fW = fW; }
	EvoPoint4(EvoPoint3 pt3)	{ m_fX = pt3.m_fX; m_fY = pt3.m_fY; m_fZ = pt3.m_fZ; m_fW = 1.0f;	}

	~EvoPoint4() {};

	void Add(const EvoPoint4 &pt4);
	void Subtract(const EvoPoint4 &pt4);

	void AddScaled(const EvoPoint4 &pt4, float fScale);

	void Scale(float fScale);
	void Scale(const EvoPoint4 &pt4);

	void Multiply(const EvoMatrix *pMatrix);

	float Dot(const EvoPoint4 &pt4);

	float Unitize();

	float Length();
	float LengthSqrd();

	float Distance(const EvoPoint4 &pt4);
	float DistanceSqrd(const EvoPoint4 &pt4);

	// Operators
	EvoPoint4 operator+(const EvoPoint4 &rhs);
	EvoPoint4 operator-(const EvoPoint4 &rhs);
	EvoPoint4 operator*(const float &rhs);


   public:
	float m_fX, m_fY, m_fZ, m_fW;
};

inline void EvoPoint4::Add(const EvoPoint4 &pt4)
{
	m_fX += pt4.m_fX;
	m_fY += pt4.m_fY;
	m_fZ += pt4.m_fZ;
	m_fW += pt4.m_fW;
}

inline void EvoPoint4::Subtract(const EvoPoint4 &pt4)
{
	m_fX -= pt4.m_fX;
	m_fY -= pt4.m_fY;
	m_fZ -= pt4.m_fZ;
	m_fW += pt4.m_fW;
}

inline void EvoPoint4::AddScaled(const EvoPoint4 &pt4, float fScale)
{
	m_fX += pt4.m_fX*fScale;
	m_fY += pt4.m_fY*fScale;
	m_fZ += pt4.m_fZ*fScale;
	m_fW += pt4.m_fW*fScale;
}

inline void EvoPoint4::Scale(float fScale)
{
	m_fX *= fScale;
	m_fY *= fScale;
	m_fZ *= fScale;
	m_fW *= fScale;
}

inline void EvoPoint4::Scale(const EvoPoint4 &pt4)
{
	m_fX *= pt4.m_fX;
	m_fY *= pt4.m_fY;
	m_fZ *= pt4.m_fZ;
	m_fW *= pt4.m_fW;
}

inline void EvoPoint4::Multiply(const EvoMatrix *pMatrix)
{
	D3DXVec4Transform((D3DXVECTOR4 *)this, (CONST D3DXVECTOR4 *)this, (CONST D3DXMATRIX *)pMatrix);
}

inline float EvoPoint4::Unitize()
{
	float fLength = Length();

	m_fX /= fLength;
	m_fY /= fLength;
	m_fZ /= fLength;
	m_fW /= fLength;

	return fLength;
}

inline float EvoPoint4::Length()
{
	return sqrtf(LengthSqrd());
}

inline float EvoPoint4::LengthSqrd()
{
	return m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ + m_fW*m_fW;
}

inline float EvoPoint4::Distance(const EvoPoint4 &pt4)
{
	return sqrtf(DistanceSqrd(pt4));
}

inline float EvoPoint4::DistanceSqrd(const EvoPoint4 &pt4)
{
	return SQR(m_fX - pt4.m_fX) + SQR(m_fY - pt4.m_fY) + SQR(m_fZ - pt4.m_fZ) + SQR(m_fW = pt4.m_fW);
}

inline float EvoPoint4::Dot(const EvoPoint4 &pt4)
{
	return (m_fX*pt4.m_fX + m_fY*pt4.m_fY + m_fZ*pt4.m_fZ + m_fW*pt4.m_fW);
}

#endif
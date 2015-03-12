#ifndef _EVOBOUNDINGBOX_H_
#define _EVOBOUNDINGBOX_H_

class EvoBoundingBox
{
   public:
	EvoBoundingBox();

	// create from minimum and maximum vectors
	EvoBoundingBox(const EvoPoint3 &vMin, const EvoPoint3 &vMax);

	// create from set of points
	EvoBoundingBox(const void *pPoints, int iNumPoints, int iStride);

	// set from minimum and maximum vectors
	void Set(const EvoPoint3 &vMin, const EvoPoint3 &vMax);

	// set from set of points
	void Set(const void *pPoints, int iNumPoints, int iStride);

	// returns size of bounding box
	EvoPoint3 GetSize(void) const { return m_pt3Max - m_pt3Min; }

	// compute union
	void Union(const EvoBoundingBox &bb2);

	EvoPoint3 m_pPoints[8];
	EvoPoint3 m_pt3Min, m_pt3Max;
};

/////////////////////

inline EvoBoundingBox::EvoBoundingBox()
{
	m_pt3Min = EvoPoint3( FLT_MAX, FLT_MAX, FLT_MAX);
	m_pt3Max = EvoPoint3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
}

// compute union of two bounding boxes
inline EvoBoundingBox Union(const EvoBoundingBox &bb1, const EvoBoundingBox &bb2)
{
	EvoPoint3 vMin, vMax;
	vMin.m_fX = MIN(bb1.m_pt3Min.m_fX, bb2.m_pt3Min.m_fX);
	vMin.m_fY = MIN(bb1.m_pt3Min.m_fY, bb2.m_pt3Min.m_fY);
	vMin.m_fZ = MIN(bb1.m_pt3Min.m_fZ, bb2.m_pt3Min.m_fZ);
	vMax.m_fX = MAX(bb1.m_pt3Max.m_fX, bb2.m_pt3Max.m_fX);
	vMax.m_fY = MAX(bb1.m_pt3Max.m_fY, bb2.m_pt3Max.m_fY);
	vMax.m_fZ = MAX(bb1.m_pt3Max.m_fZ, bb2.m_pt3Max.m_fZ);
	return EvoBoundingBox(vMin, vMax);
}

// compute union
inline void EvoBoundingBox::Union(const EvoBoundingBox &bb2)
{
	(*this) = ::Union(*this, bb2);
}

// create from minimum and maximum vectors
inline EvoBoundingBox::EvoBoundingBox(const EvoPoint3 &vMin, const EvoPoint3 &vMax)
{
	Set(vMin, vMax);
}

// create from set of points
inline EvoBoundingBox::EvoBoundingBox(const void *pPoints, int iNumPoints, int iStride)
{
	Set(pPoints, iNumPoints, iStride);
}


// create from minimum and maximum vectors
inline void EvoBoundingBox::Set(const EvoPoint3 &vMin, const EvoPoint3 &vMax)
{
  // calculate points
  m_pPoints[0] = EvoPoint3(vMin.m_fX, vMin.m_fY, vMin.m_fZ);
  m_pPoints[1] = EvoPoint3(vMax.m_fX, vMin.m_fY, vMin.m_fZ);
  m_pPoints[2] = EvoPoint3(vMin.m_fX, vMin.m_fY, vMax.m_fZ);
  m_pPoints[3] = EvoPoint3(vMax.m_fX, vMin.m_fY, vMax.m_fZ);
  m_pPoints[4] = EvoPoint3(vMin.m_fX, vMax.m_fY, vMin.m_fZ);
  m_pPoints[5] = EvoPoint3(vMax.m_fX, vMax.m_fY, vMin.m_fZ);
  m_pPoints[6] = EvoPoint3(vMin.m_fX, vMax.m_fY, vMax.m_fZ);
  m_pPoints[7] = EvoPoint3(vMax.m_fX, vMax.m_fY, vMax.m_fZ);
  m_pt3Min = vMin;
  m_pt3Max = vMax;
}

// create from set of points
inline void EvoBoundingBox::Set(const void *pPoints, int iNumPoints, int iStride)
{
	// calculate min and max vectors
	m_pt3Min = EvoPoint3( FLT_MAX, FLT_MAX, FLT_MAX);
	m_pt3Max = EvoPoint3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	char *pData = (char *)pPoints;
	for(int i=0; i<iNumPoints; i++)
	{
		const EvoPoint3 &vPoint = *(EvoPoint3*)pData;
		if(vPoint.m_fX < m_pt3Min.m_fX) m_pt3Min.m_fX = vPoint.m_fX;
		if(vPoint.m_fY < m_pt3Min.m_fY) m_pt3Min.m_fY = vPoint.m_fY;
		if(vPoint.m_fZ < m_pt3Min.m_fZ) m_pt3Min.m_fZ = vPoint.m_fZ;

		if(vPoint.m_fX > m_pt3Max.m_fX) m_pt3Max.m_fX = vPoint.m_fX;
		if(vPoint.m_fY > m_pt3Max.m_fY) m_pt3Max.m_fY = vPoint.m_fY;
		if(vPoint.m_fZ > m_pt3Max.m_fZ) m_pt3Max.m_fZ = vPoint.m_fZ;

		// next position
		pData += iStride;
	}
	// create from vectors
	Set(m_pt3Min, m_pt3Max);
}



#endif
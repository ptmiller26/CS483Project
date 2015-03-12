#ifndef _EVOFRUSTUM_H_
#define _EVOFRUSTUM_H_

#include "EvoBoundingBox.h"

class EvoFrustum
{
   public:
	// 8 corner points of frustum
	EvoPoint3 m_pPoints[8];

	// computes AABB from corner points
	inline void CalculateAABB(void);

	// world space bounding box
	EvoBoundingBox m_AABB;
};

// computes AABB vectors from corner points
inline void EvoFrustum::CalculateAABB(void)
{
	m_AABB.Set(m_pPoints, 8, sizeof(EvoPoint3));
}

#endif
#ifndef _EVOQUAT_H_
#define _EVOQUAT_H_

class EvoQuat
{
   public:
	EvoQuat() {};
	~EvoQuat() {};
	
	void ConvertToMatrix(EvoMatrix *pMatrix);
   
   public:
	D3DXQUATERNION m_Quat;
};

inline void EvoQuat::ConvertToMatrix(EvoMatrix *pMatrix)
{
	D3DXMatrixRotationQuaternion(pMatrix->GetD3DMatrix(), &m_Quat);
}

#endif
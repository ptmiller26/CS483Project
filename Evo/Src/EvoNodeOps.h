#ifndef _EVONODEOPS_H_
#define _EVONODEOPS_H_

class EvoNodeOps
{
   public:
	EvoNodeOps()	{};
	~EvoNodeOps()	{};

	// Get the first Renderable you find
	static void SetMaterialTexture(EvoNode *pNode, uint uiSlot, EvoTexture *pTex);
};

inline void EvoNodeOps::SetMaterialTexture(EvoNode *pNode, uint uiSlot, EvoTexture *pTex)
{
	if (pNode)
	{
		EvoRenderable *pRenderable = pNode->GetRenderable();

		if (pRenderable)
		{
			((EvoGranny*)pRenderable)->SetMaterialTexture(uiSlot, pTex);
		}

		for (uint ui = 0; ui < pNode->GetNumChildren(); ui++)
		{
			 SetMaterialTexture(pNode->GetChild(ui), uiSlot, pTex);
		}
	}
}

#endif

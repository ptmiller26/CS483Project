#ifndef _EVOINPUTLAYOUTMANAGER_H_
#define _EVOINPUTLAYOUTMANAGER_H_

typedef int EvoInputLayoutHandle;

class EvoShader;

class EvoInputLayoutMappings
{
   public:
	bool AreEquivalent(EvoInputElements &IE);

	EvoInputElements m_InputElements;
	stdext::hash_map<std::string, EvoInputLayout *> m_ShaderTechniqueLayoutMap;
};

class EvoInputLayoutManager
{
   public:
	EvoInputLayoutManager();
	~EvoInputLayoutManager();

	EvoInputLayoutHandle RegisterInputElements(EvoInputElements InputElements);

	const EvoInputLayout *GetLayout(EvoInputLayoutHandle hInputLayout, EvoShader *pShader, char *szTechnique);

   protected:
	std::vector<EvoInputLayoutMappings>	m_aLayoutMappings;

};

#endif
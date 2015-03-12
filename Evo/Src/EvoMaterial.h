#ifndef _EVOMATERIAL_H_
#define _EVOMATERIAL_H_

class EvoShader;

class EvoMaterial
{
   public:
	EvoMaterial();
	~EvoMaterial();

	void SetTexture(uint Slot, EvoTexture *pTexture);

	uint GetTextureCnt();

	void SetShader(EvoShader *pShader);
	EvoShader *GetShader();

	void SetShaderTechnique(int iTechnique);
	int GetShaderTechnique();

	void BindTextures();
	void UnbindTextures();

   protected:
	std::vector<EvoTexture *>	m_apTextures;

	EvoShader *					m_pShader;
	int							m_iTechnique;
};


// Inlines

inline void EvoMaterial::SetTexture(uint uiSlot, EvoTexture *pTexture)
{
	if (pTexture != NULL)
	{
		if (uiSlot >= m_apTextures.size())
			m_apTextures.resize(uiSlot+1);

		m_apTextures[uiSlot] = pTexture;
	}
}

inline uint EvoMaterial::GetTextureCnt()
{
	return (uint)m_apTextures.size();
}

inline void EvoMaterial::SetShader(EvoShader *pShader)
{
	m_pShader = pShader;
}

inline EvoShader *EvoMaterial::GetShader()
{
	return m_pShader;
}

inline void EvoMaterial::SetShaderTechnique(int iTechnique)
{
	m_iTechnique = iTechnique;
}

inline int EvoMaterial::GetShaderTechnique()
{
	return m_iTechnique;
}

#endif
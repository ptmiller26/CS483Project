#ifndef _EVOINPUTELEMENTS_H_
#define _EVOINPUTELEMENTS_H_

namespace EvoInputElementUsage
{
	enum EUsage
	{
		POSITION,
		BLENDWEIGHT,
		BLENDINDICES,
		NORMAL,
		PSIZE,
		TEXCOORD,
		TANGENT,
		BINORMAL,
		POSITIONT,
		COLOR,
		FOG,
		DEPTH,
	};
};

class EvoInputElements
{
   public:
	EvoInputElements();
	~EvoInputElements();

	struct InputElement
	{
		EvoInputElementUsage::EUsage	m_eUsage;
		uint							m_uiInputNum;
		EvoFormat::EFormat				m_eFormat;
		uint							m_uiInputSlot;
		uint							m_uiByteOffset;
		bool							m_bPerInstance;
		uint							m_uiInstanceStepRate; 
	};

	void AddElement(EvoInputElementUsage::EUsage eUsage, uint uiInputNum, EvoFormat::EFormat eFormat, uint uiInputSlot, uint uiByteOffset, bool bPerInstance = false, uint uiInstanceStepRate = 0 );
	void AddElement(EvoInputElements::InputElement Element);

	uint GetElementCount()	{ return (uint)m_aInputElements.size();	}

#if defined(EVO_RENDERER_DX10)
	const D3D10_INPUT_ELEMENT_DESC *GetRendererSpecificFormat();

	const char *GetSemanticName(EvoInputElementUsage::EUsage eUsage);
#endif

#if defined(EVO_RENDERER_DX9)
	const D3DVERTEXELEMENT9 *GetRendererSpecificFormat();
#endif

   protected:
	std::vector<InputElement> m_aInputElements;

#if defined(EVO_RENDERER_DX9)
	D3DDECLUSAGE ToDX9DeclUsage(EvoInputElementUsage::EUsage eUsage);
	D3DDECLTYPE ToDX9DeclType(EvoFormat::EFormat eFormat);
#endif

	friend class EvoInputLayoutMappings;
};

#endif
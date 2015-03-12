#include "EvoStdAfx.h"

EvoInputElements::EvoInputElements()
{
}

EvoInputElements::~EvoInputElements()
{
}

void EvoInputElements::AddElement(EvoInputElementUsage::EUsage eUsage, uint uiInputNum, EvoFormat::EFormat eFormat, uint uiInputSlot, uint uiByteOffset, bool bPerInstance, uint uiInstanceStepRate)
{
	InputElement Element;
	Element.m_eUsage = eUsage;
	Element.m_uiInputNum = uiInputNum;
	Element.m_eFormat = eFormat;
	Element.m_uiInputSlot = uiInputSlot;
	Element.m_uiByteOffset = uiByteOffset;
	Element.m_bPerInstance = bPerInstance;
	Element.m_uiInstanceStepRate = uiInstanceStepRate;

	m_aInputElements.push_back(Element);
}

void EvoInputElements::AddElement(EvoInputElements::InputElement Element)
{
	m_aInputElements.push_back(Element);
}


#if defined(EVO_RENDERER_DX9)
const D3DVERTEXELEMENT9 *EvoInputElements::GetRendererSpecificFormat()
{
	D3DVERTEXELEMENT9 *pD3DVertexElement = new D3DVERTEXELEMENT9[GetElementCount()+1];
	
	for (uint ui = 0; ui < GetElementCount(); ui++)
	{
		pD3DVertexElement[ui].Stream = m_aInputElements[ui].m_uiInputSlot;
		pD3DVertexElement[ui].Offset = m_aInputElements[ui].m_uiByteOffset;
		pD3DVertexElement[ui].Type = ToDX9DeclType(m_aInputElements[ui].m_eFormat);
		pD3DVertexElement[ui].Method = D3DDECLMETHOD_DEFAULT;
		pD3DVertexElement[ui].UsageIndex = m_aInputElements[ui].m_uiInputNum;
		pD3DVertexElement[ui].Usage = ToDX9DeclUsage(m_aInputElements[ui].m_eUsage);
	}

	pD3DVertexElement[GetElementCount()].Stream = 0xff;
	pD3DVertexElement[GetElementCount()].Offset = 0;
	pD3DVertexElement[GetElementCount()].Type = D3DDECLTYPE_UNUSED;
	pD3DVertexElement[GetElementCount()].Method = 0;
	pD3DVertexElement[GetElementCount()].Usage = 0;
	pD3DVertexElement[GetElementCount()].UsageIndex = 0;

	return pD3DVertexElement;
}

D3DDECLTYPE EvoInputElements::ToDX9DeclType(EvoFormat::EFormat eFormat)
{
	switch (eFormat)
	{
		case EvoFormat::R32G32B32A32_FLOAT:
			return D3DDECLTYPE_FLOAT4;
		case EvoFormat::R32G32B32_FLOAT:
			return D3DDECLTYPE_FLOAT3;
		case EvoFormat::R32G32_FLOAT:
			return D3DDECLTYPE_FLOAT2;
		case EvoFormat::R8G8B8A8_UNORM:
			return D3DDECLTYPE_UBYTE4N;
		case EvoFormat::R8G8B8A8_UINT:
			return D3DDECLTYPE_UBYTE4;
			
	};

	assert(0);	// IMPLEMENT any unspecified formats!
	return D3DDECLTYPE_UNUSED ;
}

D3DDECLUSAGE EvoInputElements::ToDX9DeclUsage(EvoInputElementUsage::EUsage eUsage)
{
	switch (eUsage)
	{
		case EvoInputElementUsage::POSITION:
			return D3DDECLUSAGE_POSITION ;

		case EvoInputElementUsage::NORMAL:
			return D3DDECLUSAGE_NORMAL;

		case EvoInputElementUsage::BINORMAL:
			return D3DDECLUSAGE_BINORMAL;

		case EvoInputElementUsage::BLENDINDICES:
			return D3DDECLUSAGE_BLENDINDICES; 

		case EvoInputElementUsage::BLENDWEIGHT:
			return D3DDECLUSAGE_BLENDWEIGHT;

		case EvoInputElementUsage::COLOR:
			return D3DDECLUSAGE_COLOR;

		case EvoInputElementUsage::DEPTH:
			return D3DDECLUSAGE_DEPTH;

		case EvoInputElementUsage::FOG:
			return D3DDECLUSAGE_FOG;

		case EvoInputElementUsage::POSITIONT:
			return D3DDECLUSAGE_POSITIONT;

		case EvoInputElementUsage::PSIZE:
			return D3DDECLUSAGE_PSIZE;

		case EvoInputElementUsage::TANGENT:
			return D3DDECLUSAGE_TANGENT;

		case EvoInputElementUsage::TEXCOORD:
			return D3DDECLUSAGE_TEXCOORD;
	};

	assert(0);
	return D3DDECLUSAGE_POSITION;
}

#endif

#if defined(EVO_RENDERER_DX10)

const D3D10_INPUT_ELEMENT_DESC *EvoInputElements::GetRendererSpecificFormat()
{
	if (!m_pInputElementDesc)
	{
		m_pInputElementDesc = new D3D10_INPUT_ELEMENT_DESC[GetElementCount()];

		for (uint ui = 0; ui < GetElementCount(); ui++)
		{
			m_pInputElementDesc[ui].SemanticName = GetSemanticName(m_aInputElements[ui].m_eUsage);
			m_pInputElementDesc[ui].SemanticIndex = m_aInputElements[ui].m_uiInputNum;
			m_pInputElementDesc[ui].InputSlot = m_aInputElements[ui].m_uiInputSlot;
			m_pInputElementDesc[ui].AlignedByteOffset = m_aInputElements[ui].m_uiByteOffset;
			m_pInputElementDesc[ui].Format = (DXGI_FORMAT)m_aInputElements[ui].m_eFormat;
			
			if (!m_aInputElements[ui].m_bPerInstance)
				m_pInputElementDesc[ui].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA; 
			else m_pInputElementDesc[ui].InputSlotClass = D3D10_INPUT_PER_INSTANCE_DATA; 

			m_pInputElementDesc[ui].InstanceDataStepRate = m_aInputElements[ui].m_uiInstanceStepRate;
		}
	}

	return m_pInputElementDesc;
}

const char *EvoInputElements::GetSemanticName(EvoInputElementUsage::EUsage eUsage)
{
	switch (eUsage)
	{
		case EvoInputElementUsage::POSITION:
			return "POSITION";

		case EvoInputElementUsage::NORMAL:
			return "NORMAL";

		case EvoInputElementUsage::BINORMAL:
			return "BINORMAL";

		case EvoInputElementUsage::BLENDINDICES:
			return "BLENDINDICES";

		case EvoInputElementUsage::BLENDWEIGHT:
			return "BLENDWEIGHT";

		case EvoInputElementUsage::COLOR:
			return "COLOR";

		case EvoInputElementUsage::DEPTH:
			return "DEPTH";

		case EvoInputElementUsage::FOG:
			return "FOG";

		case EvoInputElementUsage::POSITIONT:
			return "POSITIONT";

		case EvoInputElementUsage::PSIZE:
			return "PSIZE";

		case EvoInputElementUsage::TANGENT:
			return "TANGENT";

		case EvoInputElementUsage::TEXCOORD:
			return "TEXCOORD";
	};

	return NULL;
}

#endif

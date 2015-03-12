#ifndef _EVOINPUTLAYOUT_H_
#define _EVOINPUTLAYOUT_H_

class EvoShader;

class EvoInputLayout
{
   public:
	#if defined(EVO_RENDERER_DX9)
	IDirect3DVertexDeclaration9 *GetRendererSpecific()	{ return m_pInputLayout;	}
#endif

#if defined(EVO_RENDERER_DX10)
	ID3D10InputLayout *GetRendererSpecific()			{ return m_pInputLayout;	}
#endif

   protected:
	EvoInputLayout(EvoShader *pShader, char *szTechnique, EvoInputElements *pInputElements);
	~EvoInputLayout();

   protected:
	
#if defined(EVO_RENDERER_DX9)
	IDirect3DVertexDeclaration9 * m_pInputLayout;
#endif

#if defined(EVO_RENDERER_DX10)
	ID3D10InputLayout * m_pInputLayout;
#endif

	friend class EvoInputLayoutManager;
};

#endif
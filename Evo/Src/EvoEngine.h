#ifndef _EVOENGINE_H_
#define _EVOENGINE_H_

enum EEvoTransform
{
	EVO_TRANSFORM_WORLD,
	EVO_TRANSFORM_VIEW,
	EVO_TRANSFORM_INVVIEW,
	EVO_TRANSFORM_PROJECTION,
};

class EvoEngine {
   public:
	~EvoEngine();

	bool Initialize(char *szIniFile, HWND hWindow, HINSTANCE hInst);
	void Shutdown();

	static EvoEngine *GetInstance();

	void Begin();
	void End();

	void Present();

	void Clear();

	EvoRenderer *GetRenderer();

	EvoBuffer *CreateBuffer(uint uiBytes, uint uiStride, void *pData, EvoBind::EBind eBind, EvoUsage::EUsage eUsage, EvoCpuAccess::ECpuAccess eCPUAccess, bool bGenMips, bool bCubeTexture);
	void DestroyBuffer(EvoBuffer *);

	void SetVB(EvoBuffer *pBuffer, uint uiSlot, uint uiStride, uint uiOffset);
	void SetVBs(EvoBuffer **ppBuffer, uint uiSlot, uint uiCnt, uint *puiStrides, uint *puiOffsets);

	// uiStride is 2 or 4
	void SetIB(EvoBuffer *pBuffer, uint uiStride, uint uiOffset);

	// Setup the Input Layout and specify which shader it will be used with
	void SetInputLayout(EvoInputLayoutHandle hInputLayout, EvoShader *pShader, char *szTechnique);

	void SetPrimitiveType(EvoPrimitive::EEvoPrimitive ePrimitive);
	EvoPrimitive::EEvoPrimitive GetPrimitiveType();

	void SetTransform(EEvoTransform eTransform, const EvoMatrix *);
	EvoMatrix *GetTransform(EEvoTransform eTransform);

	void DrawIndexed(uint uiVertCnt, uint uiIndexCnt, uint uiStartIndex, int iOffset);

	void BindToPS(uint uiSlot, EvoTexture *pTexture);
	void UnbindPS(uint uiSlot);

	void PushDepthStencilState(EvoDepthStencilState *pState);
	void PopDepthStencilState();
	void PushBlendState(EvoBlendState *pState);
	void PopBlendState();

	void PushClearColor(EvoColor Color);
	void PopClearColor();

	void PushCamera(EvoCamera *pCamera);
	void PopCamera();

	// uiIndex of 0, and pRT of NULL reverts to default backbuffer/DS targets
	void BeginRenderTarget(uint uiIndex, EvoRenderTarget *pRT);
	void EndRenderTarget(uint uiIndex, EvoRenderTarget *pRT);

	// This shader will override any shader applied to objects/materials being rendered.
	// Must specify technique to use for rigid objects and skinned objects
	// Set back to NULL to not use the override
	void SetShaderOverride(EvoShader *pShader, char *szRigidTechnique, char *szSkinnedTechnique);

	EvoShader *GetShaderOverride();
	char *GetShaderOverrideRigidTechnique();
	char *GetShaderOverrideSkinnedTechnique();

	EvoInputLayoutManager *GetInputLayoutMgr();

	HINSTANCE			GetInstanceHandle()		{ return m_hInstance;			}
	HWND				GetWindowHandle()		{ return m_hWnd;				}
	int					GetScreenWidth()		{ return m_InitData.m_uiScreenWidth;	}
	int					GetScreenHeight()		{ return m_InitData.m_uiScreenHeight;	}

   private:
    EvoEngine();

   protected:
	static EvoEngine *		ms_pInstance;

	HWND                	m_hWnd;
	HINSTANCE				m_hInstance;

	EvoInitData				m_InitData;

	EvoMatrix				m_mtxProjection;
	EvoMatrix				m_mtxWorld;
	EvoMatrix				m_mtxView;
	EvoMatrix				m_mtxInvView;

	EvoInputLayoutManager	m_InputLayoutMgr;

	EvoRenderer *			m_pRenderer;

	EvoPrimitive::EEvoPrimitive			m_eCurrentPrimitive;

	std::stack<EvoDepthStencilState *>	m_DepthStencilStateStack;
	std::stack<EvoBlendState *>			m_BlendStateStack;

	std::stack<EvoColor>				m_ClearColorStack;

	std::stack<EvoCamera *>				m_CameraStack;

	EvoShader *m_pShaderOverride;
	EvoString m_strRigidTechnique;
	EvoString m_strSkinnedTechnique;
};


// Inlines

inline void EvoEngine::SetTransform(EEvoTransform eTransform, const EvoMatrix *pM)
{
	switch (eTransform)
	{
		case EVO_TRANSFORM_WORLD:
			m_mtxWorld = *pM;
			break;

		case EVO_TRANSFORM_VIEW:
			m_mtxView = *pM;
			break;

		case EVO_TRANSFORM_INVVIEW:
			m_mtxInvView = *pM;
			break;

		case EVO_TRANSFORM_PROJECTION:
			m_mtxProjection = *pM;
			break;
	};
}

inline EvoMatrix *EvoEngine::GetTransform(EEvoTransform eTransform)
{
	switch (eTransform)
	{
		case EVO_TRANSFORM_WORLD:
			return &m_mtxWorld;
			break;

		case EVO_TRANSFORM_VIEW:
			return &m_mtxView;
			break;

		case EVO_TRANSFORM_INVVIEW:
			return &m_mtxInvView;
			break;

		case EVO_TRANSFORM_PROJECTION:
			return &m_mtxProjection;
			break;

		default:
			return NULL;
	};
}

inline void EvoEngine::Begin()
{
	m_pRenderer->Begin();
}

inline void EvoEngine::End()
{
	m_pRenderer->End();
}

inline EvoRenderer *EvoEngine::GetRenderer()
{
	return m_pRenderer;
}

inline void EvoEngine::SetVB(EvoBuffer *pBuffer, uint uiSlot, uint uiStride, uint uiOffset)
{
	m_pRenderer->SetVB(pBuffer, uiSlot, uiStride, uiOffset);
}


inline void EvoEngine::SetVBs(EvoBuffer **ppBuffer, uint uiSlot, uint uiCnt, uint *puiStrides, uint *puiOffsets)
{
	m_pRenderer->SetVBs(ppBuffer, uiSlot, uiCnt, puiStrides, puiOffsets);
}


inline void EvoEngine::SetIB(EvoBuffer *pBuffer, uint uiStride, uint uiOffset)
{
	m_pRenderer->SetIB(pBuffer, uiStride,uiOffset);
}

inline void EvoEngine::SetPrimitiveType(EvoPrimitive::EEvoPrimitive ePrimitive)
{
	m_eCurrentPrimitive = ePrimitive;

	m_pRenderer->SetPrimitiveType(ePrimitive);
}
	
inline EvoPrimitive::EEvoPrimitive EvoEngine::GetPrimitiveType()
{
	return m_eCurrentPrimitive;
}

inline void EvoEngine::DrawIndexed(uint uiVertCnt, uint uiIndexCnt, uint uiStartIndex, int iOffset)
{
	m_pRenderer->DrawIndexed(uiVertCnt, uiIndexCnt, uiStartIndex, iOffset);
}

inline void EvoEngine::BindToPS(uint uiSlot, EvoTexture *pTexture)
{
	m_pRenderer->BindToPS(uiSlot, pTexture);
}

inline void EvoEngine::UnbindPS(uint uiSlot)
{
	m_pRenderer->UnbindPS(uiSlot);
}

inline void EvoEngine::PushDepthStencilState(EvoDepthStencilState *pState)
{
	m_DepthStencilStateStack.push(pState);
	pState->Apply();
}

inline void EvoEngine::PushBlendState(EvoBlendState *pState)
{
	m_BlendStateStack.push(pState);
	pState->Apply();
}

inline void EvoEngine::PopDepthStencilState()
{
	m_DepthStencilStateStack.pop();
	m_DepthStencilStateStack.top()->Apply();
}

inline void EvoEngine::PopBlendState()
{
	m_BlendStateStack.pop();
	m_BlendStateStack.top()->Apply();
}

// Setup the Input Layout and specify which shader it will be used with
inline void EvoEngine::SetInputLayout(EvoInputLayoutHandle hInputLayout, EvoShader *pShader, char *szTechnique)
{
	const EvoInputLayout *pLayout = m_InputLayoutMgr.GetLayout(hInputLayout, pShader, szTechnique);

	m_pRenderer->SetInputLayout((EvoInputLayout *)pLayout);
}

inline void EvoEngine::SetShaderOverride(EvoShader *pShader, char *szRigidTechnique, char *szSkinnedTechnique)
{
	m_pShaderOverride = pShader;

	if (szRigidTechnique)
		m_strRigidTechnique.Copy(szRigidTechnique, false);

	if (szSkinnedTechnique)
		m_strSkinnedTechnique.Copy(szSkinnedTechnique, false);
}

inline EvoShader *EvoEngine::GetShaderOverride()
{
	return m_pShaderOverride;
}

inline char *EvoEngine::GetShaderOverrideRigidTechnique()
{
	return m_strRigidTechnique.GetBuffer();
}

inline char *EvoEngine::GetShaderOverrideSkinnedTechnique()
{
	return m_strSkinnedTechnique.GetBuffer();
}

inline EvoInputLayoutManager *EvoEngine::GetInputLayoutMgr()
{
	return &m_InputLayoutMgr;
}

inline void EvoEngine::BeginRenderTarget(uint uiIndex, EvoRenderTarget *pRT)
{
	m_pRenderer->BeginRenderTarget(uiIndex, pRT);
}

inline void EvoEngine::EndRenderTarget(uint uiIndex, EvoRenderTarget *pRT)
{
	m_pRenderer->EndRenderTarget(uiIndex, pRT);
}

inline void EvoEngine::PushClearColor(EvoColor Color)
{
	m_ClearColorStack.push(Color);
}

inline void EvoEngine::PopClearColor()
{
	m_ClearColorStack.pop();
}

inline void EvoEngine::PushCamera(EvoCamera *pCamera)
{
	m_CameraStack.push(pCamera);

	SetTransform(EVO_TRANSFORM_VIEW, m_CameraStack.top()->GetViewMatrix());
	SetTransform(EVO_TRANSFORM_PROJECTION, m_CameraStack.top()->GetProjMatrix());
	SetTransform(EVO_TRANSFORM_INVVIEW, m_CameraStack.top()->GetInvViewMatrix());
}

inline void EvoEngine::PopCamera()
{
	m_CameraStack.pop();

	SetTransform(EVO_TRANSFORM_VIEW, m_CameraStack.top()->GetViewMatrix());
	SetTransform(EVO_TRANSFORM_PROJECTION, m_CameraStack.top()->GetProjMatrix());
	SetTransform(EVO_TRANSFORM_INVVIEW, m_CameraStack.top()->GetInvViewMatrix());
}



#endif

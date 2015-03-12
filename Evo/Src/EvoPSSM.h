#ifndef _EVOPSSM_H_
#define _EVOPSSM_H_

class EvoTexture;
class EvoRenderTarget;
class EvoCamera;
class EvoShader;

#define PSSM_NUM_SPLITS	(3)

class EvoPSSM 
{
   public:
    EvoPSSM();
    ~EvoPSSM();

	void Initialize(int width=1024, int height=1024);
	void Shutdown();

    void Render(EvoCamera *pViewCam);

	EvoTexture *GetTexture(uint ui)		{ return m_pRenderedTexture[ui];	}

	void UpdateShaderConstants(EvoCamera *pCamera);

	EvoCamera *GetLightCamera()		{ return m_pLightCamera;	}

   protected:
	void AdjustViewCameraPlanes(const EntityList &aReceivingEnts);
	void CalculateSplitPositions();
	EvoFrustum CalculateFrustum(float fNear, float fFar);
	EvoMatrix BuildCropMatrix(const EvoPoint3 &vMin, const EvoPoint3 &vMax);
	EvoMatrix CalculateCropMatrix(const EvoFrustum &frustum, const EvoMatrix &mView, const EvoMatrix &mProj);
	EvoMatrix CalculateCropMatrix(EntityList casters, EntityList receivers, const EvoFrustum &frustum);

	void RenderShadowMap(uint uiIndex, const EntityList &aEntities, const EvoMatrix &mSplitViewProj);

   private:
	int m_iTextureWidth;
	int m_iTextureHeight;

	EvoTexture *		m_pRenderedTexture[PSSM_NUM_SPLITS];
	EvoTexture *		m_pDepthStencil[PSSM_NUM_SPLITS];
	EvoRenderTarget *	m_pRenderTarget[PSSM_NUM_SPLITS]; 

	float				m_fSplitSchemeWeight;
	float				m_fSplitDistances[PSSM_NUM_SPLITS+1];

	EvoShader *m_pShader;

	EvoCamera *m_pViewCamera;
	EvoCamera *m_pLightCamera;

	float m_fNear;
	float m_fFar;

	EvoMatrix mTexScaleBias;

};


#endif  //#ifndef _EVOSHADOWMAP_H_
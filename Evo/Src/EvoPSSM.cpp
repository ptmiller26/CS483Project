#include "EvoStdAfx.h"

EvoPSSM::EvoPSSM()
{
	m_pShader = NULL;
}

EvoPSSM::~EvoPSSM()
{

}

void EvoPSSM::Initialize(int iWidth, int iHeight)
{
	m_iTextureWidth = iWidth;
	m_iTextureHeight = iHeight;

	m_fSplitSchemeWeight = 0.96f;

	// Calculate a matrix to transform points to shadow map texture coordinates
	// (this should be exactly like in your standard shadow map implementation)
	//
	float fTexOffset = 0.5f + (0.5f / (float)m_iTextureWidth);

	mTexScaleBias = EvoMatrix(       0.5f,        0.0f,   0.0f,  0.0f,
									0.0f,       -0.5f,   0.0f,  0.0f,
									0.0f,        0.0f,   1.0f,  0.0f,
							  fTexOffset,  fTexOffset,   0.0f,  1.0f);

	for (uint ui = 0; ui < PSSM_NUM_SPLITS; ui++)
	{
		m_pRenderedTexture[ui] = RESMGR->CreateTexture(m_iTextureWidth, m_iTextureHeight, 0, 1, EvoFormat::R32_FLOAT,
										EvoUsage::IMMUTABLE, EvoCpuAccess::NONE, EvoBind::RENDER_TARGET, false, NULL); 
		assert(m_pRenderedTexture[ui]);	

		m_pDepthStencil[ui] = RESMGR->CreateTexture(m_iTextureWidth, m_iTextureHeight, 0, 1, EvoFormat::D24_UNORM_S8_UINT,
										EvoUsage::IMMUTABLE, EvoCpuAccess::NONE, EvoBind::DEPTH_STENCIL, false, NULL);

		m_pRenderTarget[ui] = new EvoRenderTarget();
		m_pRenderTarget[ui]->SetTextures(m_pRenderedTexture[ui], m_pDepthStencil[ui]);
	}

	float fFar = 1000;
	m_pLightCamera = new EvoCamera;
	m_pLightCamera->SetParamsOrtho(1, 1, 1, fFar);
	EvoPoint3 pt3Facing;
	m_pLightCamera->GetFacingVector(&pt3Facing);
	m_pLightCamera->Translate(pt3Facing*-100);

	m_pLightCamera->SetRotateYPR(DEGTORAD(-135), DEGTORAD(60.0f), 0);

	m_pShader = RESMGR->CreateShader("PSSM.fx");

}

void EvoPSSM::Render(EvoCamera *pViewCam)
{
	m_pViewCamera = pViewCam;

	EvoPoint3 pt3Dir;
	m_pLightCamera->GetFacingVector(&pt3Dir);

//	EntityList aReceivingEntities;
	WORLD->GetVisibleEntitiesWithFlagsSet(&aReceivingEntities, ENTITY_RECEIVESHADOW);

	AdjustViewCameraPlanes(aReceivingEntities);
	CalculateSplitPositions();

	EvoMatrix pTextureMatrices[PSSM_NUM_SPLITS];

	// for each split part
	for(int i = 0; i < PSSM_NUM_SPLITS; i++)
	{
		// calculate frustum
		EvoFrustum splitFrustum;
		splitFrustum = CalculateFrustum(m_fSplitDistances[i], m_fSplitDistances[i+1]);

		// find casters
		//casters = g_Light.FindCasters(splitFrustum);
		EntityList aCastingEntities;
		WORLD->GetEntitiesIntersectingFrustumWithCastedDir(&aCastingEntities, splitFrustum, pt3Dir);

		// calculate crop matrix
		EvoMatrix mCropMatrix = CalculateCropMatrix(aCastingEntities, aReceivingEntities, splitFrustum);
		// calculate view-proj matrix
		//EvoMatrix mSplitViewProj = g_Light.m_mView * g_Light.m_mProj * mCropMatrix;
		EvoMatrix mSplitViewProj = *(m_pLightCamera->GetViewMatrix());
		mSplitViewProj.Multiply(m_pLightCamera->GetProjMatrix());
		mSplitViewProj.Multiply(&mCropMatrix);
		// calculate texture matrix
		pTextureMatrices[i] = mSplitViewProj;
		pTextureMatrices[i].Multiply(&mTexScaleBias);

		// render shadow map
		RenderShadowMap(i, aCastingEntities, mSplitViewProj);
	}

	float fShadowMapSizeInverse = 1.0f/m_iTextureWidth;
	float fShadowSize = (float)m_iTextureWidth;
	EvoShader::SetSharedValue("fShadowMapSize",&fShadowSize,sizeof(float));
	EvoShader::SetSharedValue("fShadowMapSizeInverse",&fShadowMapSizeInverse, sizeof(float));
	EvoShader::SetSharedMatrixArray("mPSSMTextureMatrix", pTextureMatrices, PSSM_NUM_SPLITS);
	EvoShader::SetSharedValue("fSplitPlane", &m_fSplitDistances[1], PSSM_NUM_SPLITS*sizeof(float));

	for(int i=0;i<PSSM_NUM_SPLITS;i++)
	{
		EVO->GetRenderer()->GetDevice()->SetTexture(5+i, m_pRenderedTexture[i]->GetTexture());
		// set correct sampler states
		EVO->GetRenderer()->GetDevice()->SetSamplerState(5+i, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		EVO->GetRenderer()->GetDevice()->SetSamplerState(5+i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		EVO->GetRenderer()->GetDevice()->SetSamplerState(5+i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		EVO->GetRenderer()->GetDevice()->SetSamplerState(5+i, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		EVO->GetRenderer()->GetDevice()->SetSamplerState(5+i, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		EVO->GetRenderer()->GetDevice()->SetSamplerState(5+i, D3DSAMP_BORDERCOLOR, 0xFFFFFFFF);
	}
}

void EvoPSSM::AdjustViewCameraPlanes(const EntityList &aReceivingEnts)
{
	 // find the nearest and farthest points of given
	// scene objects in camera's view space
	//
	float fMaxZ = 0;
	float fMinZ = FLT_MAX;

	EvoPoint3 pt3Dir;
	EvoPoint3 pt3Pos;
	m_pViewCamera->GetFacingVector(&pt3Dir);
	m_pViewCamera->GetTranslate(&pt3Pos);

	// for each object
	for(unsigned int i = 0; i < aReceivingEnts.size(); i++)
	{
		EvoEntity *pEntity = aReceivingEnts[i];

		// for each point in AABB
		for(int j = 0; j < 8; j++)
		{
			// calculate m_fZ-coordinate distance to near plane of camera
			EvoPoint3 pt3PointToCam = pEntity->GetAABB().m_pPoints[j] - pt3Pos;
			float fZ = pt3PointToCam.Dot(pt3Dir);

			// find boundary values
			if(fZ > fMaxZ) fMaxZ = fZ;
			if(fZ < fMinZ) fMinZ = fZ;
		}
	}

	// use smallest distance as new near plane
	// and make sure it is not too small
	m_fNear = MAX(fMinZ, 1.0f);
	// use largest distance as new far plane
	// and make sure it is larger than nearPlane
	m_fFar = MAX(fMaxZ, m_fNear + 1.0f);
	
	float fFov, fAspect, fNear, fFar;
	m_pViewCamera->GetParams(&fFov, &fAspect, &fNear, &fFar);
	m_pViewCamera->SetParams(fFov, fAspect, m_fNear, m_fFar);
}

void EvoPSSM::CalculateSplitPositions()
{
	// Practical split scheme:
	//
	// CLi = n*(f/n)^(i/numsplits)
	// CUi = n + (f-n)*(i/numsplits)
	// Ci = CLi*(lambda) + CUi*(1-lambda)
	//
	// lambda scales between logarithmic and uniform
	//

	for(int i = 0; i < PSSM_NUM_SPLITS; i++)
	{
		float fIDM = i / (float)PSSM_NUM_SPLITS;
		float fLog = m_fNear * powf(m_fFar/m_fNear, fIDM);
		float fUniform = m_fNear + (m_fFar - m_fNear) * fIDM;
		m_fSplitDistances[i] = fLog * m_fSplitSchemeWeight + fUniform * (1 - m_fSplitSchemeWeight);
	}

	// make sure border values are accurate
	m_fSplitDistances[0] = m_fNear;
	m_fSplitDistances[PSSM_NUM_SPLITS] = m_fFar;
}

// computes a frustum with given far and near planes
EvoFrustum EvoPSSM::CalculateFrustum(float fNear, float fFar)
{
	EvoPoint3 vZ;
	m_pViewCamera->GetFacingVector(&vZ);//Normalize(m_vTarget - m_vSource);
	EvoPoint3 vX;
	m_pViewCamera->GetRightVector(&vX);//Normalize(Cross(m_vUpVector, vZ));
	EvoPoint3 vY;
	m_pViewCamera->GetUpVector(&vY);//Normalize(Cross(vZ, vX));

	float fAspect = m_pViewCamera->GetAspect();
	float fTanFOVDiv2 = tanf(m_pViewCamera->GetFOV()*0.5f);

	float fNearPlaneHalfHeight = fTanFOVDiv2 * fNear;
	float fNearPlaneHalfWidth = fNearPlaneHalfHeight * fAspect;

	float fFarPlaneHalfHeight = fTanFOVDiv2 * fFar;
	float fFarPlaneHalfWidth = fFarPlaneHalfHeight * fAspect;

	EvoPoint3 pt3CamPos;
	m_pViewCamera->GetTranslate(&pt3CamPos);

	EvoPoint3 vNearPlaneCenter = pt3CamPos + vZ * fNear;
	EvoPoint3 vFarPlaneCenter = pt3CamPos + vZ * fFar;

	EvoFrustum frustum;
	frustum.m_pPoints[0] = EvoPoint3(vNearPlaneCenter - vX*fNearPlaneHalfWidth - vY*fNearPlaneHalfHeight);
	frustum.m_pPoints[1] = EvoPoint3(vNearPlaneCenter - vX*fNearPlaneHalfWidth + vY*fNearPlaneHalfHeight);
	frustum.m_pPoints[2] = EvoPoint3(vNearPlaneCenter + vX*fNearPlaneHalfWidth + vY*fNearPlaneHalfHeight);
	frustum.m_pPoints[3] = EvoPoint3(vNearPlaneCenter + vX*fNearPlaneHalfWidth - vY*fNearPlaneHalfHeight);

	frustum.m_pPoints[4] = EvoPoint3(vFarPlaneCenter - vX*fFarPlaneHalfWidth - vY*fFarPlaneHalfHeight);
	frustum.m_pPoints[5] = EvoPoint3(vFarPlaneCenter - vX*fFarPlaneHalfWidth + vY*fFarPlaneHalfHeight);
	frustum.m_pPoints[6] = EvoPoint3(vFarPlaneCenter + vX*fFarPlaneHalfWidth + vY*fFarPlaneHalfHeight);
	frustum.m_pPoints[7] = EvoPoint3(vFarPlaneCenter + vX*fFarPlaneHalfWidth - vY*fFarPlaneHalfHeight);

	// update frustum AABB
	frustum.CalculateAABB();

	return frustum;
}

// build a matrix for cropping light's projection
// given vectors are in light's clip space
EvoMatrix EvoPSSM::BuildCropMatrix(const EvoPoint3 &vMin, const EvoPoint3 &vMax)
{
  float fScaleX, fScaleY, fScaleZ;
  float fOffsetX, fOffsetY, fOffsetZ;

  fScaleX = 2.0f / (vMax.m_fX - vMin.m_fX);
  fScaleY = 2.0f / (vMax.m_fY - vMin.m_fY);

  fOffsetX = -0.5f * (vMax.m_fX + vMin.m_fX) * fScaleX;
  fOffsetY = -0.5f * (vMax.m_fY + vMin.m_fY) * fScaleY;

  fScaleZ = 1.0f / (vMax.m_fZ - vMin.m_fZ);
  fOffsetZ = -vMin.m_fZ * fScaleZ;

  // crop volume matrix
  return EvoMatrix(   fScaleX,     0.0f,     0.0f,   0.0f,
                          0.0f,  fScaleY,     0.0f,   0.0f,
                          0.0f,     0.0f,  fScaleZ,   0.0f,
                      fOffsetX, fOffsetY, fOffsetZ,   1.0f  );
}

// helper function for computing AABB in clip space
EvoBoundingBox CreateClipSpaceAABB(const EvoBoundingBox &bb, const EvoMatrix &mViewProj)
{
	EvoPoint4 vTransformed[8];
	// for each point
	for(int i=0;i<8;i++)
	{
		// transform to projection space
		vTransformed[i] = bb.m_pPoints[i];
		vTransformed[i].Multiply(&mViewProj);

		// compute clip-space coordinates
		vTransformed[i].m_fX /= vTransformed[i].m_fW;
		vTransformed[i].m_fY /= vTransformed[i].m_fW;
		vTransformed[i].m_fZ /= vTransformed[i].m_fW;
	}

	return EvoBoundingBox(vTransformed, 8, sizeof(EvoPoint4));
}

// crops the light volume on given frustum (scene-independent projection)
EvoMatrix EvoPSSM::CalculateCropMatrix(const EvoFrustum &frustum, const EvoMatrix &mView, const EvoMatrix &mProj)
{
  EvoMatrix mViewProj = mView;
  mViewProj.Multiply(&mProj);

  EvoBoundingBox cropBB;

  // find boundaries in light’s clip space
  cropBB = CreateClipSpaceAABB(frustum.m_AABB, mViewProj);

  // use default near plane
  cropBB.m_pt3Min.m_fZ = 0.0f;

  // finally, create matrix
  return BuildCropMatrix(cropBB.m_pt3Min, cropBB.m_pt3Max);
}

// crops the light volume on given objects, constrained by given frustum
EvoMatrix EvoPSSM::CalculateCropMatrix(EntityList casters, EntityList receivers, const EvoFrustum &frustum)
{
  EvoMatrix mView = *m_pLightCamera->GetViewMatrix();
  EvoMatrix mProj = *m_pLightCamera->GetProjMatrix();
  //if(!g_bUseSceneDependentProjection) return CalculateCropMatrix(frustum, mView, mProj);

  EvoMatrix mViewProj = mView;
  mViewProj.Multiply(&mProj);

  // bounding box limits
  EvoBoundingBox receiversBB, splitBB, castersBB;

  // for each caster
  // find boundaries in light’s clip space
  for(unsigned int i = 0; i < casters.size(); i++)
	  castersBB.Union(CreateClipSpaceAABB(casters[i]->GetAABB(), mViewProj));

  // for each receiver
  // find boundaries in light’s clip space
  for(unsigned int i = 0; i < receivers.size(); i++)
  {
    receiversBB.Union(CreateClipSpaceAABB(receivers[i]->GetAABB(), mViewProj));
  }

  // find frustum boundaries in light’s clip space
  splitBB = CreateClipSpaceAABB(frustum.m_AABB, mViewProj);

  // next we will merge the bounding boxes
  //
  EvoBoundingBox cropBB;
  cropBB.m_pt3Min.m_fX = MAX(MAX(castersBB.m_pt3Min.m_fX, receiversBB.m_pt3Min.m_fX), splitBB.m_pt3Min.m_fX);
  cropBB.m_pt3Max.m_fX = MIN(MIN(castersBB.m_pt3Max.m_fX, receiversBB.m_pt3Max.m_fX), splitBB.m_pt3Max.m_fX);
  cropBB.m_pt3Min.m_fY = MAX(MAX(castersBB.m_pt3Min.m_fY, receiversBB.m_pt3Min.m_fY), splitBB.m_pt3Min.m_fY);
  cropBB.m_pt3Max.m_fY = MIN(MIN(castersBB.m_pt3Max.m_fY, receiversBB.m_pt3Max.m_fY), splitBB.m_pt3Max.m_fY);
  cropBB.m_pt3Min.m_fZ = castersBB.m_pt3Min.m_fZ;
  cropBB.m_pt3Max.m_fZ = MIN(receiversBB.m_pt3Max.m_fZ, splitBB.m_pt3Max.m_fZ);

  // when there are no casters, the merged
  // bounding box will be infinitely small
  if(casters.size() == 0)
  {
    // it will cause artifacts when rendering receivers,
    // so just use the frustum bounding box instead
    cropBB.m_pt3Min = splitBB.m_pt3Min;
    cropBB.m_pt3Max = splitBB.m_pt3Max;
  }

  // finally, create matrix
  return BuildCropMatrix(cropBB.m_pt3Min, cropBB.m_pt3Max);
}

void EvoPSSM::RenderShadowMap(uint uiIndex, const EntityList &aEntities, const EvoMatrix &mSplitViewProj)
{
	EvoColor ClearColor(255,255,255,255);
	EVO->BeginRenderTarget(0, m_pRenderTarget[uiIndex]); 
	EVO->PushClearColor(ClearColor);
	EVO->Clear();

	EVO->SetShaderOverride(m_pShader,"PSSMRender","PSSMRenderSkinned");

	m_pShader->SetMatrix("mViewProj", (EvoMatrix)mSplitViewProj);
	


	//Override shader to render depth only..??
	//Render Visible Entities
	//const EntityList &VisibleEnts = WORLD->GetVisibleEnts(m_pLightCamera);
	for(uint i=0;i<aEntities.size();i++)
		aEntities[i]->Render();

	//EVO->PopCamera();
	EVO->PopClearColor();
	EVO->SetShaderOverride(NULL,NULL,NULL);

	EVO->EndRenderTarget(0, m_pRenderTarget[uiIndex]); 
}
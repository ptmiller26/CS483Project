#include "EvoStdAfx.h"

granny_data_type_definition EffectFieldsType[] =
{
	{GrannyStringMember, "EffectFile"},
	{GrannyInt32Member, "Technique"},
	{GrannyEndMember}
};

struct EffectStruct
{
	char *	m_szEffectFile;
	int		m_iTechnique;
};

struct TextureStruct
{
	char *m_szTextureName;
};


EvoGranny::EvoGranny()
{

}

EvoGranny::~EvoGranny()
{

}

bool EvoGranny::Initialize(const char *szFilename)
{
	granny_file      *pGrannyFile;
	granny_file_info *pGrannyFileInfo;

	EvoString strPath;
	strPath.GetPath((TCHAR*)szFilename);

	m_fAccumTime = 0.0f;

	m_iFrame = 0;

	pGrannyFile = GrannyReadEntireFile(szFilename);

	if (pGrannyFile == NULL)
		return false;

	pGrannyFileInfo = GrannyGetFileInfo(pGrannyFile);

	if (pGrannyFileInfo == NULL)
		return false;

	// Put the file into a consistent coordinate system.
	TransformFile(pGrannyFileInfo);

	// And then the models.  Note that we'll track the maximum bone
	//  count here, since we need it for the shared local pose...
	m_iMaxBoneCnt = 0;
	for (int iModel = 0; iModel < pGrannyFileInfo->ModelCount; iModel++)
	{
		granny_model *pGrannyModel = pGrannyFileInfo->Models[iModel];
		int const iBoneCount = pGrannyModel->Skeleton->BoneCount;

		EvoGrannyModel* pNewModel = new EvoGrannyModel;
		m_apModels.push_back(pNewModel);

		// Create the model instance and it's world pose buffer
		pNewModel->m_pGrannyInstance = GrannyInstantiateModel(pGrannyModel);
		pNewModel->m_pWorldPose      = GrannyNewWorldPose(iBoneCount);

		GrannyBuildCompositeTransform4x4 ( &pGrannyModel->InitialPlacement, (float*)&pNewModel->m_InitialOrientation );

		// Track the max bone count
		m_iMaxBoneCnt = std::max(m_iMaxBoneCnt, iBoneCount);

		// Create the meshes for this model
		for (int iMesh = 0; iMesh < pGrannyModel->MeshBindingCount; iMesh++)
		{
			EvoGrannyMesh *pMesh = new EvoGrannyMesh();
			pMesh->Initialize(this, pGrannyModel, iMesh, strPath.GetBuffer());
			pNewModel->m_apMeshes.push_back(pMesh);
		}
	}

	// When Granny updates models, she needs an intermediate buffer
	// that holds the local transform for each bone in a model.  But,
	// I really don't want to keep around a separate buffer for each
	// model because I don't care about the results outside of the
	// update loop.  So I allocate a single local pose buffer, and use
	// it for all the models.
	m_pSharedLocalPose = GrannyNewLocalPose(m_iMaxBoneCnt);

	m_pCurrentControl = NULL;
	m_uiCurrentAnim = 0xffffffff;

	return true;
}

//-------------------------------------------------------------------------------------
// Transforms a file_info structure into a consistent coordinate system
//-------------------------------------------------------------------------------------
void EvoGranny::TransformFile(granny_file_info *pFileInfo)
{
	if ( pFileInfo->ArtToolInfo == NULL )
	{
		// File doesn't have any art tool info.
		// Might never have been exported, or might have been stripped by a preprocessor.
		return;
	}

	float Origin[]      = {0, 0, 0};
    float RightVector[] = {1, 0, 0};
    float UpVector[]    = {0, 1, 0};
    float BackVector[]  = {0, 0, -1};

    float Affine3[3];
    float Linear3x3[3][3];
    float InverseLinear3x3[3][3];
	GrannyComputeBasisConversion(pFileInfo, 1.0f,
                                 Origin, RightVector, UpVector,
                                 BackVector, Affine3, (float *)Linear3x3,
                                 (float *)InverseLinear3x3);

    GrannyTransformFile(pFileInfo, Affine3,
                        (float *)Linear3x3,
                        (float *)InverseLinear3x3,
						1e-5f, 1e-5f,
                        GrannyRenormalizeNormals | GrannyReorderTriangleIndices);
}


void EvoGranny::Render()
{
	for (size_t Idx = 0; Idx < m_apModels.size(); Idx++)
	{
		m_apModels[Idx]->Render();
	}
}

/*
void EvoGranny::AdjustBone(char *szBoneName)
{
	int iBoneIndex;
	bool bFound = false;

	for (size_t Idx = 0; Idx < m_apModels.size(); Idx++)
	{
		EvoGrannyModel* pModel = m_apModels[Idx];
		granny_skeleton *pSkeleton = GrannyGetSourceSkeleton(pModel->m_pGrannyInstance);
		if(GrannyFindBoneByName(pSkeleton, szBoneName, &iBoneIndex))
		{
			// The bone was found, it's index is iBoneIndex
			bFound = true;
			break;
		}
	}

	if (bFound)
	{
		EvoMatrix m;
		EvoMatrix m2;
		m.SetIdentity();
		float Quat[4];
		float Pos[3] = {0,0,0};
		float Scale[3][3] = {1,0,0,0,1,0,0,0,1};

		m2.SetRotateYPR(DEGTORAD(22.0f), DEGTORAD(0.0f), 0);

		granny_transform *pTransform = GrannyGetLocalPoseTransform(m_pSharedLocalPose, iBoneIndex);
		GrannyBuildCompositeTransform4x4(pTransform, (granny_real32 *)&m);

		m.Multiply(&m2);

		D3DXQuaternionRotationMatrix((D3DXQUATERNION *)Quat, (D3DXMATRIX *)&m);

		pTransform->Orientation[0] = Quat[0];
		pTransform->Orientation[1] = Quat[1];
		pTransform->Orientation[2] = Quat[2];
		pTransform->Orientation[3] = Quat[3];

		EvoPoint3 pt3T;
		m.GetTranslate(&pt3T);
		pTransform->Position[0] =pt3T.m_fX;
		pTransform->Position[1] =pt3T.m_fY;
		pTransform->Position[2] =pt3T.m_fZ;
	}
}*/

void EvoGranny::AddBoneModifier(EvoBoneModifier *pBoneModifier)
{
	m_apBoneModifiers.push_back(pBoneModifier);
}

void EvoGranny::ApplyBoneModifiers(uint uiModelIdx)
{
	for (uint ui = 0; ui < m_apBoneModifiers.size(); ui++)
	{
		if (m_apBoneModifiers[ui]->m_uiModelIndex == uiModelIdx)
		{
			m_apBoneModifiers[ui]->Apply();
		}
	}
}

void EvoGranny::Update(float fDeltaT)
{
	m_fAccumTime += fDeltaT;

	static EvoMatrix s_mtxResult;

	for (uint Idx = 0; Idx < m_apModels.size(); Idx++)
	{
		EvoGrannyModel* pModel = m_apModels[Idx];
		granny_skeleton *Skeleton = GrannyGetSourceSkeleton(pModel->m_pGrannyInstance);

		// Set the model clock
		GrannySetModelClock(pModel->m_pGrannyInstance, m_fAccumTime);

		// Sample the animation into the shared local pose...
		GrannySampleModelAnimations(pModel->m_pGrannyInstance,
									0, Skeleton->BoneCount,
									m_pSharedLocalPose);

		
		// Apply any bone modifiers here.
		if (m_apBoneModifiers.size())
			ApplyBoneModifiers(Idx);

		//static bool bB = false;
		//bB = !bB;
		//if (bB)
			//AdjustBone("Bip01 Spine1");
			//AdjustBone("Bip01 Spine2");

			//AdjustBone("Bip01 L UpperArm");
			//AdjustBone("Bip01 R UpperArm");

		// ...and build the world pose from it.  A null offset matrix
		// indicates that we want the model placed at the origin...
		pModel->m_InitialOrientation.Multiply(GetOwner()->GetWorldMatrix(), &s_mtxResult);
		GrannyBuildWorldPose(Skeleton, 0, Skeleton->BoneCount,  m_pSharedLocalPose, (granny_real32 *)&s_mtxResult, pModel->m_pWorldPose);
	}
}

// Set all materials uiSlot to pTex
void EvoGranny::SetMaterialTexture(uint uiSlot, EvoTexture *pTex)
{
	for (size_t Idx = 0; Idx < m_apModels.size(); Idx++)
	{
		m_apModels[Idx]->SetMaterialTexture(uiSlot, pTex);
	}
}

void EvoGranny::ComputeOBB(EvoBoundingBox *pBoundingBox)
{
	for (size_t Idx = 0; Idx < m_apModels.size(); Idx++)
	{
		EvoBoundingBox ModelOBB;
		m_apModels[Idx]->ComputeOBB(&ModelOBB);
		pBoundingBox->Union(ModelOBB);
	}
}

EvoGrannyMesh::EvoGrannyMesh()
{
	m_pVB = NULL;
	m_pIB = NULL;
}

EvoGrannyMesh::~EvoGrannyMesh()
{

}

void EvoGrannyMesh::Initialize(EvoGranny *pGranny, granny_model* Model, int const MeshIndex, const char *szPath)
{
	assert(MeshIndex >= 0 && MeshIndex < Model->MeshBindingCount);

	m_pGranny = pGranny;

	m_pGrannyMesh = Model->MeshBindings[MeshIndex].Mesh;

	// Note the mesh pointer and the binding
	m_pGrannyBinding = GrannyNewMeshBinding(m_pGrannyMesh, Model->Skeleton, Model->Skeleton);

	

	// Index Buffer Creation
	int const IndexCount = GrannyGetMeshIndexCount(m_pGrannyMesh);
	int const IndexBufferSize = sizeof(granny_uint32) * IndexCount;

	BYTE *pIndices = new BYTE[IndexBufferSize];
	GrannyCopyMeshIndices(m_pGrannyMesh, 4, pIndices);
	
	m_pIB = EVO->CreateBuffer(IndexBufferSize, sizeof(granny_uint32), pIndices, EvoBind::INDEX_BUFFER, EvoUsage::IMMUTABLE, EvoCpuAccess::NONE, false, false);
	delete pIndices;
		

	// Vertex Buffer Creation
	int VertexCount = GrannyGetMeshVertexCount(m_pGrannyMesh);
	uint uiNumComponents = (uint)GrannyGetVertexComponentCount(m_pGrannyMesh->PrimaryVertexData->VertexType);
	if (!GrannyMeshIsRigid(m_pGrannyMesh))
	{
		// Granny is retarded in how it stores bone weight and indices and only treats them as 1 component in the count,
		// even though there are 2 components in the data.
		uiNumComponents++;
	}
	uint uiStride = GrannyGetTotalObjectSize(m_pGrannyMesh->PrimaryVertexData->VertexType);
	
	EvoInputElements InputElements;
	InitInputElementsFromGrannyData(uiNumComponents, m_pGrannyMesh->PrimaryVertexData->VertexType, &InputElements);
	m_hInputLayout = EVO->GetInputLayoutMgr()->RegisterInputElements(InputElements);

	int const uiVBSize = VertexCount * uiStride;

	BYTE *pVertices = new BYTE[uiVBSize];
	GrannyCopyMeshVertices(m_pGrannyMesh, m_pGrannyMesh->PrimaryVertexData->VertexType, pVertices);
	m_pVB = EVO->CreateBuffer(uiVBSize, uiStride, pVertices, EvoBind::VERTEX_BUFFER, EvoUsage::IMMUTABLE, EvoCpuAccess::NONE, false, false);
	delete pVertices;

	// Create the texture references
	for (int i = 0; i < m_pGrannyMesh->MaterialBindingCount; i++)
	{
		EvoMaterial *pMat = new EvoMaterial();

		pMat = CreateEvoMaterial(m_pGrannyMesh->MaterialBindings[i].Material, szPath);
		m_apMaterialReferences.push_back(pMat);
	}
}

void EvoGrannyMesh::ComputeOBB(EvoBoundingBox *pBoundingBox, EvoGrannyModel *pModel)
{
/*	// This doesn't work right
	for(int BoneBindingIndex = 0; BoneBindingIndex < m_pGrannyMesh->BoneBindingCount; ++BoneBindingIndex)
	{

		granny_bone_binding &BoneBinding = m_pGrannyMesh->BoneBindings[BoneBindingIndex];
	 
		EvoBoundingBox BoneOBB;
		BoneOBB.Set(*((EvoPoint3*)BoneBinding.OBBMin), *((EvoPoint3*)BoneBinding.OBBMax));
		pBoundingBox->Union(BoneOBB);
	}
*/
	granny_data_type_definition *InputVertexLayout = m_pGrannyMesh->PrimaryVertexData->VertexType;
    granny_data_type_definition *OutputVertexLayout = m_pGrannyMesh->PrimaryVertexData->VertexType;//GrannyPN33VertexType;

	uint uiStride = sizeof(EvoPoint3)*2;
	EvoPoint3 *pSrcVerts = (EvoPoint3 *)new char[uiStride*m_pGrannyMesh->PrimaryVertexData->VertexCount];
	GrannyCopyMeshVertices(m_pGrannyMesh, GrannyPN33VertexType, pSrcVerts);
	granny_mesh_deformer *pDeformer = GrannyNewMeshDeformer(GrannyPN33VertexType, GrannyPN33VertexType, GrannyDeformPositionNormal);

   //granny_mesh_deformer *pDeformer = GrannyNewMeshDeformer(InputVertexLayout, OutputVertexLayout, GrannyDeformPositionNormalTangentBinormal);

	

	//uint uiStride = GrannyGetTotalObjectSize(m_pGrannyMesh->PrimaryVertexData->VertexType);
	EvoPoint3 *pDestVerts = (EvoPoint3 *)new char[uiStride*m_pGrannyMesh->PrimaryVertexData->VertexCount];
	//GrannyDeformVertices(pDeformer, GrannyGetMeshBindingFromBoneIndices(m_pGrannyBinding), ( granny_real32 const *)GrannyGetWorldPoseComposite4x4Array(pModel->m_pWorldPose), m_pGrannyMesh->PrimaryVertexData->VertexCount, m_pGrannyMesh->PrimaryVertexData->Vertices, (void *)pDestVerts);

	GrannyDeformVertices(pDeformer, GrannyGetMeshBindingFromBoneIndices(m_pGrannyBinding), ( granny_real32 const *)GrannyGetWorldPoseComposite4x4Array(pModel->m_pWorldPose), m_pGrannyMesh->PrimaryVertexData->VertexCount, pSrcVerts, (void *)pDestVerts);
	

	pBoundingBox->Set(pDestVerts, m_pGrannyMesh->PrimaryVertexData->VertexCount, uiStride);

	GrannyFreeMeshDeformer(pDeformer);

	delete pDestVerts;
}

EvoMaterial *EvoGrannyMesh::CreateEvoMaterial(granny_material *pGMat, const char *szPath)
{
	EffectStruct EffectData;
	granny_variant EffectFile;
	granny_variant TextureFile;
	if (GrannyFindMatchingMember(pGMat->ExtendedData.Type, pGMat->ExtendedData.Object, "EffectFile", &EffectFile))
	{
		GrannyConvertSingleObject(EffectFile.Type, EffectFile.Object, EffectFieldsType, &EffectData);
	}
	else
	{
		return NULL;
	}

	EvoMaterial *pMat = new EvoMaterial();

	EvoString szEffectFile;
	szEffectFile.StripPath(EffectData.m_szEffectFile);

	EvoShader *pShader = RESMGR->CreateShader(szEffectFile.GetBuffer());

	EvoString TexString;
	uint uiSamplerSlot;
	for (uint ui = 0; ui < 16; ui++)
	{
		if (pShader->GetTextureParam(ui, &TexString, &uiSamplerSlot))
		{
			TextureStruct TextureData;
			granny_data_type_definition TextureFieldType[] = { {GrannyStringMember, NULL}, {GrannyEndMember} };
			TextureFieldType[0].Name = TexString.GetBuffer();

			if (GrannyFindMatchingMember(pGMat->ExtendedData.Type, pGMat->ExtendedData.Object, TexString.GetBuffer(), &TextureFile))
			{
				GrannyConvertSingleObject(TextureFile.Type, TextureFile.Object, TextureFieldType, &TextureData);
			}
			
			if (strlen(TextureData.m_szTextureName) != 0)
			{

				EvoString StrippedPathTexString;
				StrippedPathTexString.StripPath(TextureData.m_szTextureName);

				EvoTexture *pTex = RESMGR->CreateTexture(StrippedPathTexString.GetBuffer(), szPath, false);
				
				if (pTex)
				{
					pMat->SetTexture(uiSamplerSlot, pTex);
				}
			}
		}
		else
		{
			break;
		}
	}

	pMat->SetShader(pShader);
	pMat->SetShaderTechnique(EffectData.m_iTechnique);

	return pMat;

}

void EvoGrannyMesh::Render(granny_matrix_4x4 *CompositeBuffer)
{
	// First I load the mesh's index buffer.
	EVO->SetIB(m_pIB, m_pIB->GetStride(), 0);
	EVO->SetVB(m_pVB, 0, m_pVB->GetStride(), 0);

	// Normally, you might use the BindingToBoneIndices
	// function here to map from Mesh verts to model verts.  Since
	// we've already reordered the mesh indices into absolute
	// model indices, we can ignore this for the purposes of this
	// sample.  If you were loading skinning matrices into the
	// constant registers, this is where you'd do the mapping.
	int const *ToBoneIndices = GrannyGetMeshBindingToBoneIndices(m_pGrannyBinding);

	int const NumMeshBones   = GrannyGetMeshBindingBoneCount(m_pGrannyBinding);
	// you'd want to handle this better in Real Life, but we'll
	// just place a restriction on the art for this sample.
//	assert(NumMeshBones <= MAX_CONSTANT_MATRICES);  

    // Next I load the mesh's vertex buffer, or deform
    // into a temporary buffer and load that, depending on
    // whether the mesh is rigid or not.
    int VertexCount = GrannyGetMeshVertexCount(m_pGrannyMesh);
	EvoMatrix *ppMatrices[100];
    if (GrannyMeshIsRigid(m_pGrannyMesh))
    {
        // It's a rigid mesh, so load the appropriate shader and
        // vertex decl.  Note that this is pretty slow, normally
        // you'd order your meshes to minimize the number of state
        // switches.
		
        // Now I look up the transform for this mesh, and load it.
		EVO->SetTransform(EVO_TRANSFORM_WORLD, (EvoMatrix *)&CompositeBuffer[*ToBoneIndices]);
    }
    else
    {
		// Load the matrices into the constant registers.  This is
		// about the simplest way to go about this, you might want
		// to marshall into a temp buffer if the function call
		// overhead becomes problematic.
		for (int Matrix = 0; Matrix < NumMeshBones; Matrix++)
		{
			ppMatrices[Matrix] = (EvoMatrix *)CompositeBuffer[ToBoneIndices[Matrix]];
		}

		EvoMatrix mtxIdentity;
		mtxIdentity.SetIdentity();
		EVO->SetTransform(EVO_TRANSFORM_WORLD, &mtxIdentity);
	}

    // Now both the indices and vertices are loaded, so I can
    // render.  I grab the material groups and spin over them,
    // changing to the appropriate texture and rendering each
    // batch.  A more savvy rendering loop might have instead
    // built a sorted list of material groups to minimize texture
    // changes, etc., but this is the most basic way to render.
    int GroupCount = GrannyGetMeshTriangleGroupCount(m_pGrannyMesh);
    granny_tri_material_group *Group = GrannyGetMeshTriangleGroups(m_pGrannyMesh);

	int iVertCount = GrannyGetMeshVertexCount(m_pGrannyMesh);	

	while(GroupCount--)
    {
		uint uiNumShaderPasses = 0;
		EvoMaterial *pMat = NULL;
		EvoShader *pShader = NULL;

		if(Group->MaterialIndex < int(m_apMaterialReferences.size()))
		{
			pMat = m_apMaterialReferences[Group->MaterialIndex];
			if(pMat)
			{
				if (EVO->GetShaderOverride())
				{
					pShader = EVO->GetShaderOverride();
					if (GrannyMeshIsRigid(m_pGrannyMesh))
					{
						pShader->SetTechnique(EVO->GetShaderOverrideRigidTechnique());
					}
					else
					{
						pShader->SetTechnique(EVO->GetShaderOverrideSkinnedTechnique());
					}
				}
				else
				{
					pShader = pMat->GetShader();
					pShader->SetTechnique(pMat->GetShaderTechnique());
				}

				pShader->Begin(&uiNumShaderPasses);

				EVO->SetInputLayout(m_hInputLayout, pShader, pShader->GetCurrentTechniqueName());

				if (!GrannyMeshIsRigid(m_pGrannyMesh))
				{
					pShader->SetMatrixPointerArray(EV0_SHADER_BONEMATRIXARRAY, ppMatrices, NumMeshBones);
				}
			}
			else
			{
				assert(0 && "Must have a material/shader!");
				return;
			}
		}

	    
		for (uint ui = 0; ui < uiNumShaderPasses; ui++)
		{
			pShader->BeginPass(ui);

			pMat->BindTextures();

			EVO->SetPrimitiveType(EvoPrimitive::TRIANGLELIST);
			EVO->DrawIndexed(iVertCount, Group->TriCount*3, Group->TriFirst*3, 0);

			pShader->EndPass();
			
			pMat->UnbindTextures();
		}

		pShader->End();
		++Group;
	}
}

void EvoGrannyMesh::SetMaterialTexture(uint uiSlot, EvoTexture *pTex)
{
	for (size_t Idx = 0; Idx < m_apMaterialReferences.size(); Idx++)
	{
		m_apMaterialReferences[Idx]->SetTexture(uiSlot, pTex);
	}
}

EvoGrannyModel::EvoGrannyModel()
{
}

EvoGrannyModel::~EvoGrannyModel()
{

}

void EvoGrannyModel::Render()
{
	 // Since I'm going to need it constantly, I dereference the composite
    // transform buffer for the model's current world-space pose.  This
    // buffer holds the transforms that move vertices from the position
    // in which they were modeled to their current position in world space.
    granny_matrix_4x4 *CompositeBuffer = GrannyGetWorldPoseComposite4x4Array(m_pWorldPose);

	for (uint ui = 0; ui < m_apMeshes.size(); ui++)
	{
		m_apMeshes[ui]->Render(CompositeBuffer);
	}
}

void EvoGrannyModel::SetMaterialTexture(uint uiSlot, EvoTexture *pTex)
{
	for (size_t Idx = 0; Idx < m_apMeshes.size(); Idx++)
	{
		m_apMeshes[Idx]->SetMaterialTexture(uiSlot, pTex);
	}
}

void EvoGrannyModel::ComputeOBB(EvoBoundingBox *pBoundingBox)
{
	for (size_t Idx = 0; Idx < m_apMeshes.size(); Idx++)
	{
		EvoBoundingBox MeshOBB;
		m_apMeshes[Idx]->ComputeOBB(&MeshOBB, this);
		pBoundingBox->Union(MeshOBB);
	}
}

int EvoGranny::GetNumVerts()
{
	 return GrannyGetMeshVertexCount(m_apModels[0]->m_apMeshes[0]->m_pGrannyMesh);
}

int EvoGranny::GetNumFaces()		
{ 
	 return GrannyGetMeshIndexCount(m_apModels[0]->m_apMeshes[0]->m_pGrannyMesh)/3;
}

int EvoGranny::GetVertStride()
{ 
	return (int)m_apModels[0]->m_apMeshes[0]->m_pVB->GetStride();
}

int EvoGranny::GetFaceStride()
{ 
	return 3*sizeof(uint);
}

void *EvoGranny::GetVerts()
{ 
	int iVertexBufferSize = GetNumVerts()*GetVertStride();
	BYTE *pLockedVertices, *pCopyVertices;
	pCopyVertices = new BYTE[iVertexBufferSize];
	pLockedVertices = (BYTE*)m_apModels[0]->m_apMeshes[0]->m_pVB->Lock(EvoLock::READ, true);
	memcpy(pCopyVertices, pLockedVertices, iVertexBufferSize);
	m_apModels[0]->m_apMeshes[0]->m_pVB->Unlock();
	return (void*)pCopyVertices;
}

void *EvoGranny::GetFaces()
{
	int iIndexBufferSize = GetNumFaces()*GetFaceStride();
	BYTE *pLockedIndices, *pCopyIndices;
	pCopyIndices = new BYTE[iIndexBufferSize];
	pLockedIndices = (BYTE*)m_apModels[0]->m_apMeshes[0]->m_pIB->Lock(EvoLock::READ, true);
	memcpy(pCopyIndices, pLockedIndices, iIndexBufferSize);
	m_apModels[0]->m_apMeshes[0]->m_pIB->Unlock();
	return (void*)pCopyIndices;
}

void EvoGranny::LoadAnimation(uint uiAnim, const char *szFilename)
{
	granny_file      *pGrannyFile;
	granny_file_info *pGrannyFileInfo;

	pGrannyFile = GrannyReadEntireFile(szFilename);

	if (pGrannyFile == NULL)
		return;

	pGrannyFileInfo = GrannyGetFileInfo(pGrannyFile);

	if (pGrannyFileInfo == NULL)
		return;

	// Put the file into a consistent coordinate system.
	TransformFile(pGrannyFileInfo);

	if (pGrannyFileInfo->AnimationCount)
	{
		granny_animation *pAnimation = pGrannyFileInfo->Animations[0];

		m_apAnimations.push_back(pAnimation);

		uint uiAnimIndex = m_apAnimations.size()-1;

		m_AnimMap.insert(std::pair <uint, uint>(uiAnim, uiAnimIndex));
	}

}

void EvoGranny::PlayAnimation(uint uiAnim)
{
	if (uiAnim == m_uiCurrentAnim)
		return;
	else m_uiCurrentAnim = uiAnim;

	uint uiAnimIndex = 0xffffffff;

	stdext::hash_map<uint, uint>::iterator iter = m_AnimMap.find(uiAnim);

	if (iter != m_AnimMap.end())
	{
		uiAnimIndex = iter->second;
	}
	else
	{
		return;
	}

	for (uint ui = 0; ui < m_apModels.size(); ui++)
	{
		// GrannyPlayControlledAnimation is the simplest way to
		// start an animation playing on a model instance.  There
		// are more complicated calls that allow for greater
		// control over how the animation is played, but the
		// purposes of this sample, this is all I need.
		granny_control *pNewControl = GrannyPlayControlledAnimation(0.0f, m_apAnimations[uiAnimIndex], m_apModels[ui]->m_pGrannyInstance);

		if(pNewControl)
		{
			// I want to play this animation ad infinitum, so I
			// set the loop count to the magic value of 0, which
			// means forever.  Any other loop value would play the
			// animation for that many loops and then clamp to the
			// final frame.
			GrannySetControlLoopCount(pNewControl, 0);

			// Since I don't plan to make any further adjustments
			// to this animation, I can just throw away the
			// control handle completely.  However, so I don't
			// leak memory, I have to tell Granny that, once the
			// model(s) that this control affects are freed (which
			// I will do during shutdown), free the control too.
			// Normally Granny won't ever free something you've
			// created unless you tell her too, so this call is
			// basically giving her permission.
			GrannyFreeControlOnceUnused(pNewControl);

			if (m_pCurrentControl)
			{
				GrannySetControlEaseIn(pNewControl, true);
				GrannyEaseControlIn(pNewControl, 0.5f, true);

				GrannySetControlEaseOut(m_pCurrentControl, true);
				GrannyCompleteControlAt(m_pCurrentControl, GrannyEaseControlOut(m_pCurrentControl, 0.5f ));
				m_pCurrentControl = pNewControl;
			}
			else
			{
				m_pCurrentControl = pNewControl;
			}
		}
	}
}

void InitInputElementsFromGrannyData(uint uiNumComponents, granny_data_type_definition *pVertexType, EvoInputElements *m_pInputElements)
{
	uint uiOffset = 0;
	for (uint ui = 0; ui < uiNumComponents; ui++)
	{
		if (stricmp(pVertexType[ui].Name, GrannyVertexPositionName) == 0)
		{
			m_pInputElements->AddElement(EvoInputElementUsage::POSITION, 0, EvoFormat::R32G32B32_FLOAT, 0, uiOffset, false, 0);
		}
		else if (stricmp(pVertexType[ui].Name, GrannyVertexNormalName) == 0)
		{
			m_pInputElements->AddElement(EvoInputElementUsage::NORMAL, 0, EvoFormat::R32G32B32_FLOAT, 0, uiOffset, false, 0);
		}
		else if (stricmp(pVertexType[ui].Name, GrannyVertexTangentName) == 0)
		{
			m_pInputElements->AddElement(EvoInputElementUsage::TANGENT, 0, EvoFormat::R32G32B32_FLOAT, 0, uiOffset, false, 0);
		}
		else if (stricmp(pVertexType[ui].Name, GrannyVertexBinormalName) == 0)
		{
			m_pInputElements->AddElement(EvoInputElementUsage::BINORMAL, 0, EvoFormat::R32G32B32_FLOAT, 0, uiOffset, false, 0);
		}
		else if (stricmp(pVertexType[ui].Name, GrannyVertexBoneWeightsName) == 0)
		{
			m_pInputElements->AddElement(EvoInputElementUsage::BLENDWEIGHT, 0, EvoFormat::R8G8B8A8_UNORM, 0, uiOffset, false, 0);
		}
		else if (stricmp(pVertexType[ui].Name, GrannyVertexBoneIndicesName) == 0)
		{
			m_pInputElements->AddElement(EvoInputElementUsage::BLENDINDICES, 0, EvoFormat::R8G8B8A8_UINT, 0, uiOffset, false, 0);
		}
		else if (stricmp(pVertexType[ui].Name, GrannyVertexTextureCoordinatesName "0") == 0)
		{
			m_pInputElements->AddElement(EvoInputElementUsage::TEXCOORD, 0, EvoFormat::R32G32_FLOAT, 0, uiOffset, false, 0);
		}
		else if (stricmp(pVertexType[ui].Name, GrannyVertexTextureCoordinatesName "1") == 0)
		{
			m_pInputElements->AddElement(EvoInputElementUsage::TEXCOORD, 1, EvoFormat::R32G32_FLOAT, 0, uiOffset, false, 0);
		}
		else if (stricmp(pVertexType[ui].Name, GrannyVertexTextureCoordinatesName "2") == 0)
		{
			m_pInputElements->AddElement(EvoInputElementUsage::TEXCOORD, 2, EvoFormat::R32G32_FLOAT, 0, uiOffset, false, 0);
		}
		else if (stricmp(pVertexType[ui].Name, GrannyVertexTextureCoordinatesName "3") == 0)
		{
			m_pInputElements->AddElement(EvoInputElementUsage::TEXCOORD, 3, EvoFormat::R32G32_FLOAT, 0, uiOffset, false, 0);
		}

		uiOffset += GrannyGetMemberTypeSize(&pVertexType[ui]);
	}
}
#ifndef _EVOGRANNY_H_
#define _EVOGRANNY_H_

class EvoMaterial;
class EvoGrannyModel;
class EvoGrannyMesh;

class EvoGranny : public EvoRenderable
{
   public:
	EvoGranny();
	~EvoGranny();

	bool Initialize(const char *szFilename);

	void LoadAnimation(uint uiAnim, const char *szFilename);

	void PlayAnimation(uint uiAnim);

	void Update(float fDeltaT);

	void Render();

	// Set all materials uiSlot to pTex
	void SetMaterialTexture(uint uiSlot, EvoTexture *pTex);

	virtual int GetNumVerts();
	virtual int GetNumFaces();		
	virtual int GetVertStride();	
	virtual int GetFaceStride();	
	virtual void *GetVerts();		
	virtual void *GetFaces();		

	void AddBoneModifier(EvoBoneModifier *pBoneModifier);

	void ComputeOBB(EvoBoundingBox *pBoundingBox);

   protected:
    // Granny helper function to convert to proper coord system
	void TransformFile(granny_file_info *pFileInfo);

	void ApplyBoneModifiers(uint uiModelIdx);

   protected:
	int			m_iFrame;
	int			m_iMaxBoneCnt;
	float		m_fAccumTime;

	granny_local_pose *				m_pSharedLocalPose;

	std::vector<EvoGrannyModel *>	m_apModels;
	std::vector<granny_animation *> m_apAnimations;
	std::vector<EvoBoneModifier *>	m_apBoneModifiers;

	// Translates anim code into index for m_apAnimations;
	stdext::hash_map<uint, uint>	m_AnimMap;

	uint							m_uiCurrentAnim;
	granny_control					*m_pCurrentControl;

	friend class EvoGrannyMesh;
	friend class EvoGrannyModel;
	friend class EvoBoneModifier;
	friend class EvoBoneQuery;
};


class EvoGrannyMesh
{
   public:
	EvoGrannyMesh();
	~EvoGrannyMesh();

	void Initialize(EvoGranny *pGranny, granny_model* pModel, int const iMeshIndex, const char *szPath);

	void Render(granny_matrix_4x4 *CompositeBuffer);

	// Set all materials uiSlot to pTex
	void SetMaterialTexture(uint uiSlot, EvoTexture *pTex);

	void ComputeOBB(EvoBoundingBox *pBoundingBox, EvoGrannyModel *pModel);

   protected:
	EvoMaterial *CreateEvoMaterial(granny_material *pGMat, const char *szPath);

   protected:
    EvoGranny *					m_pGranny;
	granny_mesh	*				m_pGrannyMesh;
    granny_mesh_binding	*		m_pGrannyBinding;
	std::vector<EvoMaterial*>	m_apMaterialReferences;
 
    EvoBuffer		*m_pIB;
    EvoBuffer		*m_pVB;

	EvoInputLayoutHandle m_hInputLayout;

	friend class EvoGranny;
};

class EvoGrannyModel
{
   public:
	EvoGrannyModel();
	~EvoGrannyModel();

	void Render();

	// Set all materials uiSlot to pTex
	void SetMaterialTexture(uint uiSlot, EvoTexture *pTex);

	void ComputeOBB(EvoBoundingBox *pBoundingBox);

   protected:
	granny_model_instance *			m_pGrannyInstance;
    granny_world_pose *				m_pWorldPose;

	EvoMatrix						m_InitialOrientation;

	std::vector<EvoGrannyMesh*>		m_apMeshes;

	friend class EvoGranny;
	friend class EvoBoneModifier;
	friend class EvoBoneQuery;
	friend class EvoGrannyMesh;
};

void InitInputElementsFromGrannyData(uint uiNumComponents, granny_data_type_definition *pVertexType, EvoInputElements *pInputElements);

#endif
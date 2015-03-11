// Evoshadowmap code
//

//#include "Common.fx"

float4x4 mtxViewProjection	: VIEWPROJECTION;
float4x4 mtxWorldView       : WORLDVIEW;
float4x4 mtxView            : VIEW;
float4x4 mtxProj            : PROJECTION;

//TODOTW: Move to common.fx
static const int MAX_MATRICES = 63;
float4x3    mtxBoneMatrixArray[MAX_MATRICES] : BONEMATRIXARRAY;

//---------------------------------------------------------------------------------------
texture  BaseTex<>;
texture  ShadowTex<>;

sampler2D BaseSampler =
sampler_state
{
    Texture = <BaseTex>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler2D ShadowSampler =
sampler_state
{
    Texture = <ShadowTex>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
    AddressU = Clamp;
    AddressV = Clamp;
};
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//TODOTW: Move into common.fx.  REPRO From Ubershader.fx
float4x3 ComputeWorldBoneTransform( float4 f4BlendIndices, float4 f4BlendWeights )
{
    int indices[4]   = (int[4])f4BlendIndices;
    float weights[4] = (float[4])f4BlendWeights;

    // Calculate bone transform
    float4x3 BoneTransform;
	BoneTransform = weights[0] * mtxBoneMatrixArray[indices[0]];
	BoneTransform += weights[1] * mtxBoneMatrixArray[indices[1]];
	BoneTransform += weights[2] * mtxBoneMatrixArray[indices[2]];
	BoneTransform += weights[3] * mtxBoneMatrixArray[indices[3]];
	return BoneTransform;
}
//---------------------------------------------------------------------------------------
// Vertex Shader: VertShadow
// Desc: Process vertex for the shadow map
//-----------------------------------------------------------------------------
void VertShadow( float4 Pos : POSITION, out float4 oPos : POSITION,
                 out float2 Depth : TEXCOORD0 )
{
    // Compute the projected coordinates
    oPos = mul( Pos, mtxWorldView );
    oPos = mul( oPos, mtxProj );

    // Store z and w in our spare texcoord
    Depth.xy = oPos.zw;
}
//---------------------------------------------------------------------------------------
// Vertex Shader: VertShadowSkinned
// Desc: Process vertex for the shadow map
//-----------------------------------------------------------------------------
void VertShadowSkinned( float4 iPos : POSITION,
                        float4 inBlendWeights : BLENDWEIGHT, 
                        float4 inBlendIndices : BLENDINDICES,
                        out float4 oPos : POSITION,
                        out float2 Depth : TEXCOORD0 )
{
    // Transform the position
	float4x3 mtxWorldBoneTransform = ComputeWorldBoneTransform(  inBlendIndices, inBlendWeights);
	float3 f3WorldPos = mul(iPos, mtxWorldBoneTransform);
	oPos = mul(float4(f3WorldPos, 1.0), mtxViewProjection );
    
    // Store z and w in our spare texcoord
    Depth.xy = oPos.zw;
}
//---------------------------------------------------------------------------------------
// Pixel Shader: PixShadow
// Desc: Process pixel for the shadow map
//-----------------------------------------------------------------------------
void PixShadow( float2 Depth : TEXCOORD0, out float4 Color : COLOR )
{
    Color = Depth.x / Depth.y;    // Depth is z / w
}
//-----------------------------------------------------------------------------
// Technique: RenderShadow
// Desc: Renders the shadow map 
//-----------------------------------------------------------------------------
technique ShadowMapRenderShadow<>
{
    pass p0
    {
        ALPHABLENDENABLE = FALSE;
        VertexShader = compile vs_1_1 VertShadow();
        PixelShader = compile ps_2_0 PixShadow();
    }
}

//-----------------------------------------------------------------------------
// Technique: RenderShadowSkinned
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique ShadowMapRenderShadowSkinned<>
{
    pass p0
    {
        ALPHABLENDENABLE = FALSE;
        VertexShader = compile vs_2_0 VertShadowSkinned();
        PixelShader = compile ps_2_0 PixShadow();
    }
}
//-----------------------------------------------------------------------------



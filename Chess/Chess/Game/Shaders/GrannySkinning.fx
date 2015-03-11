struct VS_IN
{
    float3  Position        : POSITION0;
    float4  BoneWeights     : BLENDWEIGHT;
    uint4  BoneIndices      : BLENDINDICES;
    float3  Normal          : NORMAL;
    float2  Tex0            : TEXCOORD0;
};

struct VS_IN_RIGID
{
    float3  Position        : POSITION0;
    float3  Normal          : NORMAL;
    float2  Tex0            : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position         : SV_POSITION;
    float2 Tex0             : TEXCOORD0;
    float3 Diffuse          : COLOR0;
};

// Must match the #defines in dx_sample.cpp
//float4x3 ObjToWorld  : register(c0);
//float4x3 WorldToView : register(c4);
//float4x4 ViewToClip  : register(c8);
float3 DirFromLight  : register(c12) = {0.5f, 1.0f, 0.5f};
float4 LightColour   : register(c13) = {1.0f, 0.8f, 0.8f, 1.0f};
float4 AmbientColour : register(c14) = {0.2f, 0.2f, 0.2f, 1.0f};

// transformations
float4x4 mtxWorld            : WORLD;
float4x4 mtxWorldViewProj    : WORLDVIEWPROJECTION;
float4x4 mtxViewProj         : VIEWPROJECTION;
static const int MAX_MATRICES = 63;
float4x3    mtxBoneMatrixArray[MAX_MATRICES] : BONEMATRIXARRAY;

// Note that 46 was chosen because of the art authored for
//  this demo, not for any deep reason.
//float4x3 BoneMatrices[46] : register(c15);


Texture2D txDiffuse;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

PS_INPUT SkinVSVertexFetch( VS_IN In )
{
    PS_INPUT Out;
    
    float BoneWeights[4] = (float[4])In.BoneWeights;
    int BoneIndices[4]   = (int[4])In.BoneIndices;
    
    float4 InPos     = float4( In.Position, 1 );
    float3 WorldPos    = 0;
    float3 WorldNormal = 0;
    
    for( int i = 0; i < 4; ++i )
    {
        float4x3 BoneMatrix = mtxBoneMatrixArray[BoneIndices[i]];
        WorldPos    += BoneWeights[i] * mul( InPos, BoneMatrix );
        WorldNormal += BoneWeights[i] * mul( In.Normal, BoneMatrix );
    }


    //float4 ClipPos = mul(float4(In.Position, 1), mtxWorldViewProj);
    float4 ClipPos = mul(float4(WorldPos, 1), mtxWorldViewProj);
    Out.Position   = ClipPos;

    Out.Tex0    = In.Tex0;
    Out.Diffuse = LightColour * max(0, dot(float4(normalize(WorldNormal),0), DirFromLight)) + AmbientColour;

    return Out;
}



PS_INPUT RigidVS( VS_IN_RIGID In )
{
    PS_INPUT Out;
    float4 ClipPos = mul(float4(In.Position, 1), mtxWorldViewProj);

    float4 InNormal  = float4( In.Normal, 0);
    float3 ObjNormal = mul(InNormal, (float4x3)mtxWorld);
    
    Out.Position = ClipPos;
    Out.Tex0     = In.Tex0;
    Out.Diffuse  = LightColour * max(0, dot(float4(ObjNormal,0), DirFromLight)) + AmbientColour;

    return Out;
}


float4 PS ( PS_INPUT In ) : SV_Target
{
	float3 DiffuseSamp =  txDiffuse.Sample( samLinear, In.Tex0 ) * In.Diffuse;
   
    return float4(DiffuseSamp, 1);
}


technique10 TSkinning
{
    pass P0
    {     
        SetVertexShader( CompileShader( vs_4_0, SkinVSVertexFetch() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }  
}

technique10 TRigid
{
    pass P0
    {   
        SetVertexShader( CompileShader( vs_4_0, RigidVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }  
}


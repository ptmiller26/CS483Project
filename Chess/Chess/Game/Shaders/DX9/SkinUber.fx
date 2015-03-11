
//#include "common.fx"

#define CUBEINTENSITY 1.0

float4x4	mtxWorldViewProjection		: WORLDVIEWPROJECTION;
float4x4	mtxViewProjection			: VIEWPROJECTION;
float4x4	mtxWorldInv					: INVWORLD;
float4x4	mtxWorld					: WORLD;
float4x4	mtxViewInv					: INVVIEW;
float4x4	mtxView						: VIEW;
float4x4    mtxWorldView				: WORLDVIEW;

float3		f3LightDir 			: GLOBAL = {0.577, -0.577, 0.577};		//{1, -1, 1} normalized
float3		f3LightDiffuse		: GLOBAL = { 1, .9, .5 };				// key light diffuse color
float		fSpecularExp	 	: GLOBAL = {32};						// key light specular exponent
float3		f3FogColor			: GLOBAL = {0.33, 0.25, 0.2};

static const int	iNumSplits = 3;
shared float4x4		mPSSMTextureMatrix[iNumSplits]	: GLOBAL;
shared float		fSplitPlane[iNumSplits]		: GLOBAL;

static const int MAX_MATRICES = 63;
float4x3    mtxBoneMatrixArray[MAX_MATRICES] : BONEMATRIXARRAY;


// Shadowmap constants
#define SHADOW_EPSILON 0.0005f
shared float    fShadowMapSize         : GLOBAL = 1024;
shared float    fShadowMapSizeInverse  : GLOBAL = 1.0f / 1024.0f;

float fTest = 512.0f;

shared float4x4 mtxViewToLightProj : GLOBAL;       // Transform from view space to light projection space
shared float3   f3LightVPos        : GLOBAL;       // Light position in view space
shared float3   f3LightVDir        : GLOBAL;       // Light direction in view space
shared float    fCosTheta          : GLOBAL = 0.0f; // Cosine of theta of the spot light
shared float3   f3ShadowColor	   : GLOBAL =  {27.0f/255.0f, 62.0f/255.0f, 140.0f/255.0f};


//------------------------------------------------------------------------------------------------
//							TEXTURES
//------------------------------------------------------------------------------------------------  

texture DiffuseTex <string UIName = "Diffuse Map"; int Slot = 0;>;
texture NormalTex <string UIName = "Normal Map"; int Slot = 1;>;
textureCUBE EnvTex <string UIName = "Env Cube Map"; int Slot = 2;>;
textureCUBE LightTex <string UIName = "Light Cube Map"; int Slot = 3;>;
texture ShadowTex<string UIName = "Shadow Map"; int Slot = 4;>;


sampler2D DiffuseMap : register(s0) < int Slot = 0;> = sampler_state
{
	Texture = <DiffuseTex>;
	MinFilter = Anisotropic;
	MagFilter = Linear;
	MipFilter = Linear;
    MaxAnisotropy = 8;
	AddressU = Wrap;
	AddressV = Wrap;
};

sampler2D NormalMap : register(s1) = sampler_state
{
	Texture = <NormalTex>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Wrap;
	AddressV = Wrap;
};

samplerCUBE EnvironmentMap : register(s2) = sampler_state
{
	Texture = <EnvTex>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};

samplerCUBE LightCubeMap : register(s3) = sampler_state
{
	Texture = <LightTex>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler2D ShadowMap : register(s4) = sampler_state
{
    Texture = <ShadowTex>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
    AddressU = Clamp;
    AddressV = Clamp;
};

sampler2D samShadowMap[iNumSplits] : register(s5);

//------------------------------------------------------------------------------------------------  
//------------------------------------------------------------------------------------------------  

struct VS_INPUT
{
	float4 f4Position		: POSITION0;
	float3 f3Normal			: NORMAL;
	float3 f3Tangent		: TANGENT;
	float3 f3Binormal		: BINORMAL;
	float2 f2TexCoord		: TEXCOORD0;
	float4 f4BlendWeights	: BLENDWEIGHT;
	float4 f4BlendIndices	: BLENDINDICES;
};


struct VS_OUTPUT
{
	float4	f4Position		: POSITION;
	float3	f3LightHalfAng	: COLOR0;
	float3	f3LightVec		: COLOR1;
	float2	f2TexCoord		: TEXCOORD0;
	float3	f3Normal		: TEXCOORD1;
	float3	f3Binormal		: TEXCOORD2;
	float3	f3Tangent		: TEXCOORD3;
	float3	f3EnvMapCoord	: TEXCOORD4;
	float2  f2Depth			: TEXCOORD5;
};

struct PSSM_OUTPUT
{
	float4 vPosOut : POSITION;
	float4 vTexCoord[iNumSplits+1] : TEXCOORD;
};

//------------------------------------------------------------------------------------------------  

float SamplePCF(sampler2D sam, float4 vTexCoord)
{
  // project texture coordinates
  vTexCoord.xy /= vTexCoord.w;

  // 2x2 PCF Filtering
  // 
  float fShadow[7];
  fShadow[0] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(                    0,                     0)));
  fShadow[1] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(fShadowMapSizeInverse,                     0)));
  fShadow[2] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(                    0, fShadowMapSizeInverse)));
  fShadow[3] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(fShadowMapSizeInverse, fShadowMapSizeInverse)));
  fShadow[4] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(-fShadowMapSizeInverse,                     0)));
  fShadow[5] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(                    0, -fShadowMapSizeInverse)));
  fShadow[6] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(-fShadowMapSizeInverse, -fShadowMapSizeInverse)));

  //float2 vLerpFactor = frac(fShadowMapSize * vTexCoord.xy);

  //return lerp( lerp(fShadow[0], fShadow[1], vLerpFactor.x),
  //             lerp(fShadow[2], fShadow[3], vLerpFactor.x),
  //                  vLerpFactor.y);
  
  float fVal = 0.14f;
  return fShadow[0]*fVal + fShadow[1]*fVal + fShadow[2]*fVal + fShadow[3]*fVal + fShadow[4]*fVal + fShadow[5]*fVal + fShadow[6]*fVal;
}

float SamplePCF13(sampler2D sam, float4 vTexCoord)
{
  // project texture coordinates
  vTexCoord.xy /= vTexCoord.w;

  // 2x2 PCF Filtering
  // 
  float fShadow[13];
  fShadow[0] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(                    0,                     0)));
  fShadow[1] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(fShadowMapSizeInverse,                     0)));
  fShadow[2] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(                    0, fShadowMapSizeInverse)));
  fShadow[3] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(fShadowMapSizeInverse, fShadowMapSizeInverse)));
  fShadow[4] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(2*fShadowMapSizeInverse,                     0)));
  fShadow[5] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(                    0, 2*fShadowMapSizeInverse)));
  fShadow[6] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(2*fShadowMapSizeInverse, 2*fShadowMapSizeInverse)));
  fShadow[7] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(3*fShadowMapSizeInverse,                     0)));
  fShadow[8] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(                    0, 3*fShadowMapSizeInverse)));
  fShadow[9] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(3*fShadowMapSizeInverse, 3*fShadowMapSizeInverse)));
  fShadow[10] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(4*fShadowMapSizeInverse,                     0)));
  fShadow[11] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(                    0, 4*fShadowMapSizeInverse)));
  fShadow[12] = (vTexCoord.z < tex2D(sam, vTexCoord + float2(4*fShadowMapSizeInverse, 4*fShadowMapSizeInverse)));

  //float2 vLerpFactor = frac(fShadowMapSize * vTexCoord.xy);

  //return lerp( lerp(fShadow[0], fShadow[1], vLerpFactor.x),
  //             lerp(fShadow[2], fShadow[3], vLerpFactor.x),
  //                  vLerpFactor.y);
  
  float fVal = 0.07f;
  return fShadow[0]*fVal + fShadow[1]*fVal + fShadow[2]*fVal + fShadow[3]*fVal + fShadow[4]*fVal + fShadow[5]*fVal + fShadow[6]*fVal+ fShadow[7]*fVal + fShadow[8]*fVal + fShadow[9]*fVal+ fShadow[10]*fVal + fShadow[11]*fVal + fShadow[12]*fVal;
}


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

VS_OUTPUT mainVS( VS_INPUT kInput, uniform bool bSkinned )
{
	VS_OUTPUT kOutput = (VS_OUTPUT)0;

	// Transform the position
	float3 f3WorldPos;
	if(bSkinned)
	{
		float4x3 mtxWorldBoneTransform = ComputeWorldBoneTransform( kInput.f4BlendIndices, kInput.f4BlendWeights );
		f3WorldPos = mul(kInput.f4Position, mtxWorldBoneTransform);
		
		//skin the normal and then back to object space
		kOutput.f3Normal  = mul(mul(kInput.f3Normal,    mtxWorldBoneTransform), mtxWorldInv);
		kOutput.f3Binormal= mul(mul(kInput.f3Binormal,  mtxWorldBoneTransform), mtxWorldInv);
		kOutput.f3Tangent = mul(mul(kInput.f3Tangent,   mtxWorldBoneTransform), mtxWorldInv);
	}
	else
	{
		f3WorldPos = mul(kInput.f4Position, mtxWorld);	
		
		kOutput.f3Normal  = kInput.f3Normal;
		kOutput.f3Binormal= kInput.f3Binormal;
		kOutput.f3Tangent = kInput.f3Tangent;
	}
	
	kOutput.f4Position = mul(float4(f3WorldPos, 1.0), mtxViewProjection );
	
	

	
	
	// Copy the texture coordinates
	kOutput.f2TexCoord = kInput.f2TexCoord;

	// Environment mapping - compute WS view vector and pass the tangent space matrix
	kOutput.f3EnvMapCoord = normalize( f3WorldPos - mtxViewInv[3].xyz );

	// Make the object-space to tangent-space matrix
	float3x3 mtxObjToTangentSpace;
	mtxObjToTangentSpace[0] = kOutput.f3Tangent;
	mtxObjToTangentSpace[1] = kOutput.f3Binormal;
	mtxObjToTangentSpace[2] = kOutput.f3Normal;
	
	// Compute the object-space camera direction
	float3 f3ObjectViewDir = mtxViewInv[3].xyz - f3WorldPos;
	f3ObjectViewDir = normalize(f3ObjectViewDir);
	
	// Compute the object-space light direction and half-angle for light #1.
	float3 f3LightVector = f3LightDir;
	float3 f3ObjectLightDir = mul(f3LightVector, mtxWorldInv);
	f3ObjectLightDir = normalize(f3ObjectLightDir);
	kOutput.f3LightHalfAng = 0.5f * f3ObjectViewDir + 0.5f;//0.5 * mul( mtxObjToTangentSpace, ( f4ObjectViewDir + f4VertNormLightVec.xyz ) / 2.0 ) + float3( 0.5, 0.5, 0.5 );
	kOutput.f3LightVec = 0.5f * mul(mtxObjToTangentSpace, f3ObjectLightDir) + 0.5f;
		
	kOutput.f2Depth.xy = kOutput.f4Position.zw;
	
	return kOutput;
}



//------------------------------------------------------------------------------------------------
// Pixel shaders
//------------------------------------------------------------------------------------------------

float4 mainPS( VS_OUTPUT kOutput, uniform bool bShadowmap) : COLOR
{
	float4 f4BaseColor = tex2D(DiffuseMap, kOutput.f2TexCoord);//tex2D( BaseSampler, kOutput.f2TexCoord );
	float4 f4NormalSample = tex2D(NormalMap, kOutput.f2TexCoord);//tex2D( NormalSampler, kOutput.f2TexCoord );
	float3 f3SpecularMask = float3(f4NormalSample.a, f4NormalSample.a, f4NormalSample.a );
	float3 f3EnvironmentMask = float3(f4NormalSample.a, f4NormalSample.a, f4NormalSample.a );

	// Make the tangent-space to object-space matrix
	float3x3 mtxTanToObjSpace; 
	mtxTanToObjSpace[0] = kOutput.f3Tangent;
	mtxTanToObjSpace[1] = kOutput.f3Binormal;
	mtxTanToObjSpace[2] = kOutput.f3Normal;
	
	// Compute the world-space normal
	f4NormalSample.rgb = ( f4NormalSample.rgb - 0.5 ) * 2.0;
	f4NormalSample.rgb = normalize( f4NormalSample.rgb );
	float3 f3OSNormal = mul( f4NormalSample.rgb, mtxTanToObjSpace );
	float3 f3WSNormal = normalize( mul( f3OSNormal, mtxWorld ) );

	// Now sample the environment map
	float4 f4EnvironmentMap = texCUBE(EnvironmentMap, reflect( kOutput.f3EnvMapCoord, f3WSNormal ));

	// Light coming from normal
	float3 f3ExpLightDir = f3WSNormal; 
	float3 f3ExpHalfAng = ((kOutput.f3LightHalfAng - 0.5) * 2.0 + f3WSNormal)/2.0f; // Compute half ang. kOutput.f3LightHalfAng is actually view vector to this vert
	
	// Diffuse (N * L) * C
	float3 f3Lighting = texCUBE(LightCubeMap, f3WSNormal + float3(0,.5,0))*CUBEINTENSITY;
	float3 f3Diffuse = f3LightDiffuse;
	f3Diffuse *= f3Lighting;
	
	// Find normal times the half angle, add in the environment map, and mask it
	float3 fNdH = dot( f3WSNormal, f3ExpHalfAng );
	fNdH =  pow( max( 0, fNdH ), fSpecularExp );
	float3 f3Specular = (saturate(fNdH * f3Diffuse)) * f3SpecularMask;

	float f3Env = f4EnvironmentMap.rgb;
	
	float3 ShadowDiffuse = float3(1,1,1);
	if(bShadowmap)
	{

	}
	
	// Final Lighting equation
	float3 FinalColor = lerp(( f3Diffuse ) * f4BaseColor, f3Env*f3Lighting, f3EnvironmentMask);
	FinalColor += f3Specular;
	FinalColor *= ShadowDiffuse;

			
	//TODOTW: Test
	//FinalColor = ShadowDiffuse;
	
	float fFogAmount = min(pow(kOutput.f2Depth.x/500, 1), 1);
	FinalColor = lerp(FinalColor, f3FogColor, fFogAmount);
		
	return float4(FinalColor, 1);
}


float4 pssmPS( PSSM_OUTPUT kOutput) : COLOR
{
	float fLightingFactor = 1;
	float fDistance = kOutput.vTexCoord[0].z;

	for(int i=0; i < iNumSplits; i++)
	{
		if(fDistance < fSplitPlane[i])
		{
			fLightingFactor = SamplePCF13(samShadowMap[i], kOutput.vTexCoord[i+1]);
			break;
		}
	}
	return float4(0, 0, .10, fLightingFactor*.6 + 0.4f);
/*	float3 vColorIn = 1;
	float3 g_vAmbient = 0.1;
	float3 vLighting = 0.5;
	float4 col = 1;
	col.rgb = vColorIn * saturate(g_vAmbient.xyz + vLighting.xyz * fLightingFactor);
	
	return col;//float4(fLightingFactor,fLightingFactor,fLightingFactor,1);*/
}

PSSM_OUTPUT pssmVS( VS_INPUT kInput, uniform bool bSkinned )
{
	PSSM_OUTPUT kOutput = (PSSM_OUTPUT)0;

	// Transform the position
	float3 f3WorldPos;
	if(bSkinned)
	{
		float4x3 mtxWorldBoneTransform = ComputeWorldBoneTransform( kInput.f4BlendIndices, kInput.f4BlendWeights );
		f3WorldPos = mul(kInput.f4Position, mtxWorldBoneTransform);
	}
	else
	{
		f3WorldPos = mul(kInput.f4Position, mtxWorld);	
	}
	
	kOutput.vPosOut = mul(float4(f3WorldPos, 1.0), mtxViewProjection );
	
	  // store view space position
	kOutput.vTexCoord[0] = mul(float4(f3WorldPos, 1.0), mtxView);

	// coordinates for shadow maps
	[unroll] for(int i=0;i<iNumSplits;i++)
	{
		kOutput.vTexCoord[i+1] = mul(float4(f3WorldPos, 1.0), mPSSMTextureMatrix[i]);
	}
	
	return kOutput;
}


technique TSkinUber
{
	pass P0
	{
		// Compile the shaders
		VertexShader = compile vs_3_0 mainVS(true);
        PixelShader  = compile ps_3_0 mainPS(true);
	}
	
	pass P1
	{
		AlphaBlendEnable = TRUE;
		SrcBlend         = INVSRCALPHA;
        DestBlend        = SRCALPHA;
        
		VertexShader = compile vs_3_0 pssmVS(true);
        PixelShader  = compile ps_3_0 pssmPS();
	}	
}

technique TUber
{

	pass P0
	{
		// Compile the shaders
		VertexShader = compile vs_3_0 mainVS(false);
        PixelShader  = compile ps_3_0 mainPS(true);
	}
	
	pass P1
	{
		AlphaBlendEnable = TRUE;
		SrcBlend         = INVSRCALPHA;
        DestBlend        = SRCALPHA;
        
		VertexShader = compile vs_3_0 pssmVS(false);
        PixelShader  = compile ps_3_0 pssmPS();
	}
}
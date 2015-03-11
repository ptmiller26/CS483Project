
#define CUBEINTENSITY 1.0

float4x4	mtxWorldViewProjection		: WORLDVIEWPROJECTION;
float4x4	mtxViewProjection			: VIEWPROJECTION;
float4x4	mtxWorldInv					: INVWORLD;
float4x4	mtxWorld					: WORLD;
float4x4	mtxViewInv					: INVVIEW;

float3		f3RRTLightDir 			: GLOBAL = {0.577, -0.577, 0.577};			//{1, -1, 1} normalized
float3		f3RRTLightDiffuse		: GLOBAL = { 1, 1, 1 };				// key light diffuse color

float		fRRTSpecularExp			: GLOBAL = {32};							// key light specular exponent

static const int MAX_MATRICES = 63;
float4x3    mtxBoneMatrixArray[MAX_MATRICES] : BONEMATRIXARRAY;

//------------------------------------------------------------------------------------------------
//							TEXTURES
//------------------------------------------------------------------------------------------------  
Texture2D DiffuseMap : register(t0) <string UIName = "Diffuse Map";>;
Texture2D NormalMap : register(t1) <string UIName = "Normal Map";>;
TextureCube EnvironmentMap : register(t2) <string UIName = "Env Map";>;
TextureCube LightCubeMap : register(t3) <string UIName = "LightCube Map";>;

struct VS_INPUT
{
	float4 f4Position		: POSITION0;
	float4 f4BlendWeights	: BLENDWEIGHT;
	uint4  f4BlendIndices	: BLENDINDICES;
	float3 f3Normal			: NORMAL;
	float3 f3Tangent		: TANGENT;
	float3 f3Binormal		: BINORMAL;
	float2 f2TexCoord		: TEXCOORD0;
};

struct VS_INPUT_RIGID
{
	float4 f4Position		: POSITION0;
	float3 f3Normal			: NORMAL;
	float3 f3Tangent		: TANGENT;
	float3 f3Binormal		: BINORMAL;
	float2 f2TexCoord		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4	f4Position		: SV_POSITION;
	float3	f3LightHalfAng	: COLOR0;
	float3	f3LightVec		: COLOR1;
	float2	f2TexCoord		: TEXCOORD0;
	float3	f3Normal		: TEXCOORD1;
	float3	f3Binormal		: TEXCOORD2;
	float3	f3Tangent		: TEXCOORD3;
	float3	f3EnvMapCoord	: TEXCOORD4;
};

SamplerState samLinearWrap
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samLinearClamp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

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

VS_OUTPUT VS( VS_INPUT_RIGID kInput)
{
	VS_OUTPUT kOutput = (VS_OUTPUT)0;

	float3 f3LightVector = f3RRTLightDir;

	// Transform the position, and copy the texture coordinates
	kOutput.f4Position = mul(kInput.f4Position, mtxWorldViewProjection);	
	kOutput.f2TexCoord = kInput.f2TexCoord;

	// Environment mapping - compute WS view vector and pass the tangent space matrix
	float3 f3WorldPos = mul(kInput.f4Position, mtxWorld);	
	kOutput.f3EnvMapCoord = normalize( f3WorldPos - mtxViewInv[3].xyz );
	kOutput.f3Normal = kInput.f3Normal;
	kOutput.f3Binormal = kInput.f3Binormal;
	kOutput.f3Tangent = kInput.f3Tangent;
		
	// Make the object-space to tangent-space matrix
	float3x3 mtxObjToTangentSpace;
	mtxObjToTangentSpace[0] = kInput.f3Tangent;
	mtxObjToTangentSpace[1] = kInput.f3Binormal;
	mtxObjToTangentSpace[2] = kInput.f3Normal;
	
	// Compute the object-space camera direction
	float3 f3ObjectViewDir = mtxViewInv[3].xyz - f3WorldPos;
	f3ObjectViewDir = normalize(f3ObjectViewDir);
	
	// Compute the object-space light direction and half-angle for light #1.
	float3 f3ObjectLightDir = mul(f3LightVector, mtxWorldInv);
	f3ObjectLightDir = normalize(f3ObjectLightDir);
	kOutput.f3LightHalfAng = 0.5f * f3ObjectViewDir + 0.5f;//0.5 * mul( mtxObjToTangentSpace, ( f4ObjectViewDir + f4VertNormLightVec.xyz ) / 2.0 ) + float3( 0.5, 0.5, 0.5 );
	kOutput.f3LightVec = 0.5f * mul(mtxObjToTangentSpace, f3ObjectLightDir) + 0.5f;
		
	return kOutput;
}

VS_OUTPUT VS_SKIN( VS_INPUT kInput)
{
	VS_OUTPUT kOutput = (VS_OUTPUT)0;

	float3 f3LightVector = f3RRTLightDir;
	
	// Transform the position, and copy the texture coordinates
	float4x3 mtxWorldBoneTransform = ComputeWorldBoneTransform( kInput.f4BlendIndices, kInput.f4BlendWeights );
	float3 f3WorldPos = mul(kInput.f4Position, mtxWorldBoneTransform);
	kOutput.f4Position = mul(float4(f3WorldPos, 1.0), mtxViewProjection );
	
	kOutput.f2TexCoord = kInput.f2TexCoord;

	// Environment mapping - compute WS view vector and pass the tangent space matrix
	kOutput.f3EnvMapCoord = normalize( f3WorldPos - mtxViewInv[3].xyz );
	
	//skin the normal and then back to object space
	kOutput.f3Normal = mul(mul(kInput.f3Normal, mtxWorldBoneTransform), mtxWorldInv);
	kOutput.f3Binormal = mul(mul(kInput.f3Binormal, mtxWorldBoneTransform), mtxWorldInv);
	kOutput.f3Tangent = mul(mul(kInput.f3Tangent, mtxWorldBoneTransform), mtxWorldInv);
		
	// Make the object-space to tangent-space matrix
	float3x3 mtxObjToTangentSpace;
	mtxObjToTangentSpace[0] = kOutput.f3Tangent;
	mtxObjToTangentSpace[1] = kOutput.f3Binormal;
	mtxObjToTangentSpace[2] = kOutput.f3Normal;
	
	// Compute the object-space camera direction
	float3 f3ObjectViewDir = mtxViewInv[3].xyz - f3WorldPos;
	f3ObjectViewDir = normalize(f3ObjectViewDir);
	
	// Compute the object-space light direction and half-angle for light #1.
	float3 f3ObjectLightDir = mul(f3LightVector, mtxWorldInv);
	f3ObjectLightDir = normalize(f3ObjectLightDir);
	kOutput.f3LightHalfAng = 0.5f * f3ObjectViewDir + 0.5f;//0.5 * mul( mtxObjToTangentSpace, ( f4ObjectViewDir + f4VertNormLightVec.xyz ) / 2.0 ) + float3( 0.5, 0.5, 0.5 );
	kOutput.f3LightVec = 0.5f * mul(mtxObjToTangentSpace, f3ObjectLightDir) + 0.5f;
		
	return kOutput;
}


//------------------------------------------------------------------------------------------------
// Pixel shaders
//------------------------------------------------------------------------------------------------

float4 PS( VS_OUTPUT kOutput) : SV_Target
{
	float4 f4BaseColor = DiffuseMap.Sample(samLinearWrap, kOutput.f2TexCoord);//tex2D( BaseSampler, kOutput.f2TexCoord );
	float4 f4NormalSample = NormalMap.Sample(samLinearWrap, kOutput.f2TexCoord);//tex2D( NormalSampler, kOutput.f2TexCoord );
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
	float4 f4EnvironmentMap = EnvironmentMap.Sample(samLinearClamp,reflect( kOutput.f3EnvMapCoord, f3WSNormal ));

	// Light coming from normal
	float3 f3ExpLightDir = f3WSNormal; 
	float3 f3ExpHalfAng = ((kOutput.f3LightHalfAng - 0.5) * 2.0 + f3WSNormal)/2.0f; // Compute half ang. kOutput.f3LightHalfAng is actually view vector to this vert
	
	// Diffuse (N * L) * C
	float3 f3Lighting = LightCubeMap.Sample(samLinearClamp, f3WSNormal + float3(0,.5,0))*CUBEINTENSITY;
	float3 f3Diffuse = f3RRTLightDiffuse;
	f3Diffuse *= f3Lighting;
	
	// Find normal times the half angle, add in the environment map, and mask it
	float3 fNdH = dot( f3WSNormal, f3ExpHalfAng );
	fNdH =  pow( max( 0, fNdH ), fRRTSpecularExp );
	float3 f3Specular = (saturate(fNdH * f3Diffuse)) * f3SpecularMask;
//	float3 f3Environment = 1*(f4EnvironmentMap.rgb * f3EnvironmentMask) * f4EnvironmentMap.a;
	
//	f3Specular *= 1;
//	float3 FinalColor = f3Environment + f3Specular + ( f3Diffuse ) * f4BaseColor;
	
//	return float4(FinalColor, 1);

	float f3Env = f4EnvironmentMap.rgb;
	
	float3 FinalColor = lerp(( f3Diffuse ) * f4BaseColor, f3Env, f3EnvironmentMask);
	FinalColor += f3Specular;
	
	return float4(FinalColor, 1);
	
}



technique10 TSkinUber
{
	pass P0
	{
		// Compile the shaders
  		SetVertexShader(CompileShader( vs_4_0, VS_SKIN() ));
  		SetGeometryShader( NULL );
		SetPixelShader(CompileShader( ps_4_0, PS() ));
	}	
}

technique10 TUber
{
	pass P0
	{
		// Compile the shaders
  		SetVertexShader(CompileShader( vs_4_0, VS() ));
  		SetGeometryShader( NULL );
		SetPixelShader(CompileShader( ps_4_0, PS() ));
	}	
}
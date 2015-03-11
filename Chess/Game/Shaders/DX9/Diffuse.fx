
float4x4	mtxWorldViewProjection		: WORLDVIEWPROJECTION;

//------------------------------------------------------------------------------------------------
//							TEXTURES
//------------------------------------------------------------------------------------------------  

texture DiffuseTex
<
	string UIName = "Diffuse Map";
	int Slot = 0;
>;

sampler2D DiffuseMap : register(s0) = sampler_state
{
	Texture = <DiffuseTex>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_INPUT_RIGID
{
	float4 f4Position		: POSITION0;
	float2 f2TexCoord		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4	f4Position		: POSITION;
	float2	f2TexCoord		: TEXCOORD0;
};


VS_OUTPUT VS( VS_INPUT_RIGID kInput)
{
	VS_OUTPUT kOutput = (VS_OUTPUT)0;

	// Transform the position, and copy the texture coordinates
	kOutput.f4Position = mul(kInput.f4Position, mtxWorldViewProjection);	
	kOutput.f2TexCoord = kInput.f2TexCoord;
	
	return kOutput;
}

//------------------------------------------------------------------------------------------------
// Pixel shaders
//------------------------------------------------------------------------------------------------

float4 PS( VS_OUTPUT kOutput) : COLOR
{
	float4 f4BaseColor = tex2D(DiffuseMap, kOutput.f2TexCoord);
	
	if (f4BaseColor.a <= 0.05)
		discard;
	
	return f4BaseColor;
}


technique TDiffuseNoLight
{
	pass P0
	{
		AlphaBlendEnable	= true;
        SrcBlend			= SrcAlpha;
        DestBlend			= InvSrcAlpha;
        
		// Compile the shaders
		VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
	}	
}

technique TDiffuseNoLightNoCull
{
	pass P0
	{
		AlphaBlendEnable	= true;
        SrcBlend			= SrcAlpha;
        DestBlend			= InvSrcAlpha;
        CullMode			= None;
        
		// Compile the shaders
		VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
	}	
}
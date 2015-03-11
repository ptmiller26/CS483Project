struct VSSceneIn
{
    float3 pos          : POSITION0;         //position of the particle
    float4 color        : COLOR0;           //velocity of the particle
    float2 tex          : TEXCOORD0;         //tex coords
};

struct PSSceneIn
{
    float4 pos		: POSITION;               //position
    float4 colorD	: COLOR0;                 //color for gouraud and flat shading
    float2 tex		: TEXCOORD0;                  //texture coordinate
};
 
//viewport params
float    g_ViewportHeight = 720;
float    g_ViewportWidth = 1280;

#define LINEAR_SAMPLER sampler_state { MipFilter = NONE; MinFilter = LINEAR; MagFilter = LINEAR; AddressU = CLAMP; AddressV = CLAMP; }
sampler2D  g_txDiffuse     : register(s0) = LINEAR_SAMPLER;


//
// VS for rendering in screen space
//
PSSceneIn VSScreenSpace(VSSceneIn input)
{
    PSSceneIn output = (PSSceneIn)0.0;

    //output our final position
    output.pos.x = (input.pos.x / (g_ViewportWidth/2.0)) -1;
    output.pos.y = -(input.pos.y / (g_ViewportHeight/2.0)) +1;
    output.pos.z = input.pos.z;
    output.pos.w = 1;
    
    //propogate texture coordinate
    output.tex = input.tex;
    output.colorD = input.color;
    
    return output;
}


//
// PS for rendering with alpha test
//
float4 PSScreenSpace(PSSceneIn input) : COLOR
{   
    float4 color =  tex2D( g_txDiffuse, input.tex ) * input.colorD.bgra;
    if( color.a < 0.05 )
        discard;
    return color;
    
}

//
// RenderScreneSpace - shows how to render something in screenspace
//
technique TScreenspace
{
    pass p0
    {
		ZEnable				= false;
		AlphaBlendEnable	= true;
        SrcBlend			= SrcAlpha;
        DestBlend			= InvSrcAlpha;
    
        VertexShader = compile vs_2_0 VSScreenSpace();
        PixelShader  = compile ps_2_0 PSScreenSpace();
    }  
}
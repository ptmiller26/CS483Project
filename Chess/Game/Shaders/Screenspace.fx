struct VSSceneIn
{
    float3 pos          : POSITION0;         //position of the particle
    float4 color        : COLOR0;           //velocity of the particle
    float2 tex          : TEXCOORD0;         //tex coords
};

struct PSSceneIn
{
    float4 pos		: SV_POSITION;               //position
    float4 colorD	: COLOR0;                 //color for gouraud and flat shading
    float2 tex		: TEXCOORD0;                  //texture coordinate
};
 
cbuffer cbPerViewChange
{
    //viewport params
    float    g_ViewportHeight = 720;
    float    g_ViewportWidth = 1280;
};


Texture2D g_txDiffuse : register(t0);

SamplerState g_samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

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
float4 PSScreenSpace(PSSceneIn input) : SV_Target
{   
    float4 color =  g_txDiffuse.Sample( g_samLinear, input.tex ) * input.colorD;
    if( color.a < 0.5 )
        discard;
    return color;
}

//
// RenderScreneSpace - shows how to render something in screenspace
//
technique10 TScreenspace
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VSScreenSpace() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScreenSpace() ) );
        
        SetDepthStencilState( DisableDepth, 0 );
    }  
}
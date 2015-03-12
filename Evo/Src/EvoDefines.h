#ifndef _EVODEFINES_H_
#define _EVODEFINES_H_

class EvoDX9Renderer;
class EvoDX10Renderer;
class Evo360Renderer;
class EvoPS3Renderer;
class EvoEngine;

#define     D3D_OVERLOADS

#define		EVO	EvoEngine::GetInstance()
#define		WORLD EvoWorld::GetInstance()

#if defined(EVO_RENDERER_DX9) || defined(EVO_RENDERER_DX10)
#define EVOD3DDEVICE EVO->GetRenderer()->GetDevice()
#endif

// define one of these in your preprocessor or pch.
#if defined (EVO_RENDERER_DX9)
typedef EvoDX9Renderer EvoRenderer;
#elif defined (EVO_RENDERER_DX10)
typedef EvoDX10Renderer EvoRenderer;
#elif defined (EVO_RENDERER_X360)
typedef Evo360Renderer EvoRenderer;
#elif defined (EVO_RENDERER_PS3)
typedef EvoPS3Renderer EvoRenderer;
#endif

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

#define RADTODEG(x)	((float)D3DXToDegree(x))
#define DEGTORAD(x)	((float)D3DXToRadian(x))

#define SQR(x)	((x)*(x))

#define SAFE_RELEASE(p)			if (p) p->Release(); p = NULL;
#define SAFE_DELETE(p)			if (p) delete p; p = NULL;
#define SAFE_DELETE_ARRAY(p)	if (p) delete []p; p = NULL;

#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))

template<class T> __forceinline void SWAP(T &x, T &y) 
{   
	T temp; 
	temp = x;   
	x = y;  
	y = temp;   
}

#ifndef D3DCOLOR_ARGB
#define D3DCOLOR_ARGB(a,r,g,b) ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#endif

namespace EvoPrimitive
{
	enum EEvoPrimitive
	{
		UNDEFINED = 0,
		POINTLIST = 1,
		LINELIST = 2,
		LINESTRIP = 3,
		TRIANGLELIST = 4,
		TRIANGLESTRIP = 5,
		LINELIST_ADJ = 10,
		LINESTRIP_ADJ = 11,
		TRIANGLELIST_ADJ = 12,
		TRIANGLESTRIP_ADJ = 13,
	};
}

namespace EvoLock
{
	enum ELock
	{
		READ			= 1,
		WRITE			= 2,
		READWRITE		= 3,
		DISCARD			= 4,
		NOOVERWRITE		= 5,
	};
};

namespace EvoUsage
{
	enum EUsage
	{
		READ_WRITE = 0,
		IMMUTABLE  = 1,
		DYNAMIC    = 2,
		STAGING    = 3,
	};
};

namespace EvoCpuAccess
{
	enum ECpuAccess
	{
		NONE	= 0,
		WRITE	= 0x10000L,
		READ	= 0x20000L,
	};
};

namespace EvoBind
{
	enum EBind
	{
		VERTEX_BUFFER	= 0x1L,
		INDEX_BUFFER	= 0x2L,
		CONSTANT_BUFFER = 0x4L,
		SHADER_RESOURCE = 0x8L,
		STREAM_OUTPUT	= 0x10L,
		RENDER_TARGET	= 0x20L,
		DEPTH_STENCIL	= 0x40L,
	};
};

namespace EvoFormat
{
	enum EFormat
	{
		UNKNOWN = 0,
		R32G32B32A32_TYPELESS = 1,
		R32G32B32A32_FLOAT = 2,
		R32G32B32A32_UINT = 3,
		R32G32B32A32_SINT = 4,
		R32G32B32_TYPELESS = 5,
		R32G32B32_FLOAT = 6,
		R32G32B32_UINT = 7,
		R32G32B32_SINT = 8,
		R16G16B16A16_TYPELESS = 9,
		R16G16B16A16_FLOAT = 10,
		R16G16B16A16_UNORM = 11,
		R16G16B16A16_UINT = 12,
		R16G16B16A16_SNORM = 13,
		R16G16B16A16_SINT = 14,
		R32G32_TYPELESS = 15,
		R32G32_FLOAT = 16,
		R32G32_UINT = 17,
		R32G32_SINT = 18,
		R32G8X24_TYPELESS = 19,
		D32_FLOAT_S8X24_UINT = 20,
		R32_FLOAT_X8X24_TYPELESS = 21,
		X32_TYPELESS_G8X24_UINT = 22,
		R10G10B10A2_TYPELESS = 23,
		R10G10B10A2_UNORM = 24,
		R10G10B10A2_UINT = 25,
		R11G11B10_FLOAT = 26,
		R8G8B8A8_TYPELESS = 27,
		R8G8B8A8_UNORM = 28,
		R8G8B8A8_UNORM_SRGB = 29,
		R8G8B8A8_UINT = 30,
		R8G8B8A8_SNORM = 31,
		R8G8B8A8_SINT = 32,
		R16G16_TYPELESS = 33,
		R16G16_FLOAT = 34,
		R16G16_UNORM = 35,
		R16G16_UINT = 36,
		R16G16_SNORM = 37,
		R16G16_SINT = 38,
		R32_TYPELESS = 39,
		D32_FLOAT = 40,
		R32_FLOAT = 41,
		R32_UINT = 42,
		R32_SINT = 43,
		R24G8_TYPELESS = 44,
		D24_UNORM_S8_UINT = 45,
		R24_UNORM_X8_TYPELESS = 46,
		X24_TYPELESS_G8_UINT = 47,
		R8G8_TYPELESS = 48,
		R8G8_UNORM = 49,
		R8G8_UINT = 50,
		R8G8_SNORM = 51,
		R8G8_SINT = 52,
		R16_TYPELESS = 53,
		R16_FLOAT = 54,
		D16_UNORM = 55,
		R16_UNORM = 56,
		R16_UINT = 57,
		R16_SNORM = 58,
		R16_SINT = 59,
		R8_TYPELESS = 60,
		R8_UNORM = 61,
		R8_UINT = 62,
		R8_SNORM = 63,
		R8_SINT = 64,
		A8_UNORM = 65,
		R1_UNORM = 66,
		R9G9B9E5_SHAREDEXP = 67,
		R8G8_B8G8_UNORM = 68,
		G8R8_G8B8_UNORM = 69,
		BC1_TYPELESS = 70,
		BC1_UNORM = 71,
		BC1_UNORM_SRGB = 72,
		BC2_TYPELESS = 73,
		BC2_UNORM = 74,
		BC2_UNORM_SRGB = 75,
		BC3_TYPELESS = 76,
		BC3_UNORM = 77,
		BC3_UNORM_SRGB = 78,
		BC4_TYPELESS = 79,
		BC4_UNORM = 80,
		BC4_SNORM = 81,
		BC5_TYPELESS = 82,
		BC5_UNORM = 83,
		BC5_SNORM = 84,
		B5G6R5_UNORM = 85,
		B5G5R5A1_UNORM = 86,
		B8G8R8A8_UNORM = 87,
		B8G8R8X8_UNORM = 88,
		FORCE_UINT = 0xffffffffUL,
	};
};

namespace EvoBlend
{
	enum EEvoBlend
	{
		ZERO = 1,
		ONE = 2,
		SRC_COLOR = 3,
		INV_SRC_COLOR = 4,
		SRC_ALPHA = 5,
		INV_SRC_ALPHA = 6,
		DEST_ALPHA = 7,
		INV_DEST_ALPHA = 8,
		DEST_COLOR = 9,
		INV_DEST_COLOR = 10,
		SRC_ALPHA_SAT = 11,
		BLEND_FACTOR = 14,
		INV_BLEND_FACTOR = 15,
		SRC1_COLOR = 16,
		INV_SRC1_COLOR = 17,
		SRC1_ALPHA = 18,
		INV_SRC1_ALPHA = 19,
	};
};

namespace EvoBlendOp
{
	enum EEvoBlendOp
	{
		ADD = 1,
		SUBTRACT = 2,
		REV_SUBTRACT = 3,
		MIN = 4,
		MAX = 5,
	};
};

namespace EvoComparison
{
	enum EComparison
	{
		NEVER = 1,
		LESS = 2,
		EQUAL = 3,
		LESS_EQUAL = 4,
		GREATER = 5,
		NOT_EQUAL = 6,
		GREATER_EQUAL = 7,
		ALWAYS = 8,
	};
};

namespace EvoStencilOp
{
	enum EStencilOp
	{
		KEEP = 1,
		ZERO = 2,
		REPLACE = 3,
		INCR_SAT = 4,
		DECR_SAT = 5,
		INVERT = 6,
		INCR = 7,
		DECR = 8,

	};
};

#endif
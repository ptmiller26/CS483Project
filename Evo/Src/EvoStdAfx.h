// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma warning( disable : 4996 )

//#define _WIN32_WINNT 0x0501

#include <iostream>
#include <tchar.h>

// TODO: reference additional headers your program requires here
#define _WIN32_DCOM
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <String.h>
#include <assert.h>

#include <vector>
#include <stack>
#include <string>
#include <hash_map>
#include <algorithm>
#include <xutility>

// DirectX 9
#if defined(EVO_RENDERER_DX9)
#include <d3d9.h>
#include <d3dx9.h>
#endif


// Misc Direct X
#ifndef DIRECTINPUT_VERSION
	#define DIRECTINPUT_VERSION 0x0800
#endif  
#include <dinput.h>


// Evo General
#include "EvoDefines.h"
#include "EvoString.h"
#include "EvoFile.h"
#include "EvoParser.h"
#include "EvoColor.h"
#include "EvoTask.h"
#include "EvoTaskManager.h"

#include "EvoInitData.h"

#include "EvoInputElements.h"
#include "EvoInputLayout.h"
#include "EvoInputLayoutManager.h"
#include "EvoBuffer.h"
#include "EvoTexture.h"
#include "EvoMaterial.h"
#include "EvoRenderTarget.h"

// Evo DX9
#include "EvoDX9BufferManager.h"
#include "EvoDX9Renderer.h"

#include "EvoDepthStencilState.h"
#include "EvoBlendState.h"
#include "EvoPoint3.h"
#include "EvoPoint4.h"
#include "EvoMatrix.h"
#include "EvoQuat.h"
#include "EvoLine.h"
#include "EvoBoundingBox.h"
#include "EvoFrustum.h"
#include "EvoCamera.h"
#include "EvoRenderable.h"

#include "EvoNode.h"
#include "EvoResourceManager.h"
#include "EvoFont.h"
#include "EvoEngine.h"
#include "EvoShader.h"
#include "EvoInput.h"
#include "EvoTimer.h"

// Evo GUI
#include "EvoGUIWidget.h"
#include "EvoGUITextWidget.h"
#include "EvoGUIRectWidget.h"
#include "EvoGUI.h"

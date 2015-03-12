#include "EvoStdAfx.h"

char *szEvoShaderBindingName[NUM_EVO_SHADER_BINDINGS] = 
{
	"WORLD", 
	"VIEW",
	"PROJECTION",
	"WORLDVIEW",
	"VIEWPROJECTION",
	"WORLDVIEWPROJECTION",
	"BONEMATRIXARRAY"
};

EvoShader::EvoShader()
{
	m_pEffect = NULL;
}

EvoShader::~EvoShader()
{

}



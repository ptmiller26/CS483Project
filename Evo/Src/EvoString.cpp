#include "EvoStdAfx.h"

EvoString::EvoString()
{
	m_pString			= NULL;
	m_uiFlags			= EVOSTRING_UNDEFINED;
	m_uiBufferLength	= 0;
}

EvoString::EvoString(uint uiLen)
{
	AllocateBuffer(uiLen);
}

EvoString::EvoString(TCHAR *pString, uint uiFlags)
{
	m_pString = NULL;
	m_uiFlags = uiFlags;
	m_uiBufferLength = 0;

	if (uiFlags & EVOSTRING_COPY)
	{
		Copy(pString, false);
	}
	else if (uiFlags & EVOSTRING_OWNED)
	{
		m_pString = pString;
		m_uiBufferLength = Length()+1;
	}
	else if (uiFlags & EVOSTRING_BORROWED)
	{
		m_pString = pString;
		m_uiBufferLength = Length()+1;
	}
}

EvoString::~EvoString()
{
	FreeBuffer();
}

// Creates new buffer containing only the path of pString
void EvoString::GetPath(TCHAR *pString)
{
	uint ui;
	uint uiLen = (uint)_tcslen(pString);

	for (ui = uiLen-1; ui > 0; ui--)
	{
		if (pString[ui] == '\\' || pString[ui] == '/')
		{
			// Found first path marker
			break;
		}
	}

	if (ui == 0)
	{
		Copy("", false);
	}
	else
	{
		Copy(pString, ui+1, false);
		m_pString[ui+1] = NULL;
	}
}

// Creates new buffer without any path from pString
void EvoString::StripPath(TCHAR *pString)
{
	uint ui;
	uint uiLen = (uint)_tcslen(pString);

	for (ui = uiLen-1; ui > 0; ui--)
	{
		if (pString[ui] == '\\' || pString[ui] == '/')
		{
			// Found first path marker
			break;
		}
	}

	if (ui == 0)
	{
		Copy(pString, false);
	}
	else
	{
		if (ui+1 <= uiLen)
		{
			Copy(&pString[ui+1], false);
		}
	}
}

void EvoString::Concat(TCHAR *pString, bool bCompact)
{
	uint uiLen = _tcslen(pString);
	uint uiNewBufferLen = Length() + uiLen + 1;

	if (m_uiBufferLength >= uiNewBufferLen)
	{
		_tcscat(m_pString, pString);
	}
	else
	{
		TCHAR *pNewBuffer = new TCHAR[uiNewBufferLen];
		
		if (m_pString)
			_tcscpy(pNewBuffer, m_pString);
		else pNewBuffer[0] = NULL;

		_tcscat(pNewBuffer, pString);
		FreeBuffer();
		m_pString = pNewBuffer;
		m_uiFlags = EVOSTRING_OWNED;
		m_uiBufferLength = uiNewBufferLen;
	}
}
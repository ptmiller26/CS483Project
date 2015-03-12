#ifndef _EVOSTRING_H_
#define _EVOSTRING_H_

// Platform independent string class

#define EVOSTRING_UNDEFINED (0<<0)
#define EVOSTRING_BORROWED	(1<<0)						// The string class does not own this string
#define EVOSTRING_OWNED		(2<<0)						// The string class owns the string and will delete it upon destruction
#define EVOSTRING_COPY		(3<<1 | EVOSTRING_OWNED)	// The string is copied, and therefore also owned by the class

class EvoString
{	
   public:
	EvoString();
	EvoString(uint uiLen);
	EvoString(TCHAR *, uint uiFlags);
	~EvoString();

	// Length of the string
	uint Length();

	// Length of the buffer holding the string.  Can be longer than the string.
	uint BufferLength()	{ return m_uiBufferLength;	}

	TCHAR *GetBuffer()	{ return m_pString;			}

	void FreeBuffer();

	// bCompact specifies whether to reallocate the string if the new string is smaller
	void Copy(TCHAR *pString, bool bCompact);

	// uiLenToCopy specifies length to copy
	// bCompact specifies whether to reallocate the string if the new string is smaller
	void Copy(TCHAR *pString, uint uiLenToCopy, bool bCompact);

	// Concatenate pString onto buffer
	void Concat(TCHAR *pString, bool bCompact);

	// If the buffer length is greater than the string length, this function will reallocate the buffer to the string length.
	void Compact();

	// Creates new buffer containing only the path of pString
	void GetPath(TCHAR *pString);

	// Creates new buffer without any path from pString
	void StripPath(TCHAR *pString);

	// Replace extension on the string (Assuming its a filename string)
	void ReplaceExt(TCHAR *pExt);

   protected:
	void AllocateBuffer(uint uiLen);

   protected:
	uint	m_uiFlags;
	uint	m_uiBufferLength;
	TCHAR *	m_pString;
};


// Inlines

inline uint EvoString::Length()
{
	if (m_pString)
		return (uint)_tcslen(m_pString);
	else return 0;
}

inline void EvoString::AllocateBuffer(uint uiLen)
{
	m_uiBufferLength = uiLen;
	m_pString = new TCHAR[m_uiBufferLength];
	m_pString[m_uiBufferLength-1] = NULL;
	m_uiFlags = EVOSTRING_OWNED;
}

inline void EvoString::FreeBuffer()
{
	if (m_uiFlags & EVOSTRING_OWNED)
	{
		SAFE_DELETE_ARRAY(m_pString);
	}
	else
	{
		m_pString = NULL;
	}

	m_uiFlags			= EVOSTRING_UNDEFINED;
	m_uiBufferLength	= 0;
}

inline void EvoString::Copy(TCHAR *pString, bool bCompact)
{
	uint uiLen = (uint)_tcslen(pString);

	if (uiLen >= m_uiBufferLength || (uiLen != m_uiBufferLength-1 && bCompact))
	{
		FreeBuffer();
		AllocateBuffer(uiLen+1);
	}

	_tcscpy(m_pString, pString);
}

// iLen specifies length to copy
// bCompact specifies whether to reallocate the string if the new string is smaller
inline void EvoString::Copy(TCHAR *pString, uint uiLenToCopy, bool bCompact)
{
	uint uiLen = (uint)_tcslen(pString);

	if (uiLenToCopy > uiLen)
	{
		uiLenToCopy = uiLen;
	}

	if (uiLenToCopy >= m_uiBufferLength || (uiLenToCopy != m_uiBufferLength-1 && bCompact))
	{
		FreeBuffer();
		AllocateBuffer(uiLenToCopy+1);
	}

	_tcsncpy(m_pString, pString, uiLenToCopy);
}

// Replace extension on the string (Assuming its a filename string)
inline void EvoString::ReplaceExt(TCHAR *pExt)
{
	uint uiLen = (uint)_tcslen(m_pString);

	_tcscpy(&m_pString[uiLen-3], pExt);
}

#endif

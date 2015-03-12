#include "EvoStdAfx.h"
#include <IO.h>


EvoFile::EvoFile()
{
	m_szFullname	= NULL;
	m_szFilename	= NULL;
	m_szPath		= NULL;
	m_szExt			= NULL;

	m_pFile			= NULL;
}

EvoFile::~EvoFile()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

EvoFile *EvoFile::OpenFile(const char *szFilename, EEvoFileFlag eFlags)
{
	EvoFile *pEvoFile = new EvoFile();

	if (eFlags == READ)
		pEvoFile->m_pFile = fopen(szFilename, "rb");
	else if (eFlags == WRITE)
		pEvoFile->m_pFile = fopen(szFilename, "wb");

	if (!pEvoFile->m_pFile)
	{
		delete pEvoFile;
		return NULL;
	}

	// Copy the filename and split it into path/file/ext too.
	int iLen = strlen(szFilename);

	pEvoFile->m_szFullname = new char[iLen+1];
	strcpy(pEvoFile->m_szFullname, szFilename);

	bool bExt = false;
	int iExtLen = 0;
	for (int i = iLen-1; i > 0; i--)
	{
		if (!bExt && szFilename[i] == '.')
		{
			//Ext
			iExtLen = iLen - i;
			pEvoFile->m_szExt = new char[iExtLen + 1];
			strcpy(pEvoFile->m_szExt, &szFilename[i]);
			bExt = true;
		}
		else if (bExt && (szFilename[i] == '\\' || szFilename[i] == '/'))
		{
			// Filename
			int iFilenameLen = iLen - i - iExtLen - 1;
			pEvoFile->m_szFilename = new char[iFilenameLen + 1];
			strncpy(pEvoFile->m_szFilename, &szFilename[i+1], iFilenameLen);
			pEvoFile->m_szFilename[iFilenameLen] = NULL;
	
			// Path
			int iPathLen = iLen - iFilenameLen - iExtLen;
			if (iPathLen > 0)
			{
				pEvoFile->m_szPath = new char[iPathLen+1];
				strncpy(pEvoFile->m_szPath, &szFilename[0], iPathLen);
				pEvoFile->m_szPath[iPathLen] = NULL;
			}
			else
			{
				pEvoFile->m_szPath = new char[2+1];
				strcpy(pEvoFile->m_szPath, ".\\");

			}
		}
	}

	return pEvoFile;
}

void EvoFile::CloseFile(EvoFile *&pEvoFile)
{
	delete pEvoFile;
	pEvoFile = NULL;
}

// Read bytes from file
uint EvoFile::ReadBytes(void *pData, uint uiSize)
{
	return fread(pData, sizeof(char), uiSize, m_pFile);
}

// Write bytes to file
uint EvoFile::WriteBytes(const void *pData, uint uiSize)
{
	return fwrite(pData, sizeof(char), uiSize, m_pFile);
}

uint EvoFile::GetSize()
{
	uint uiFileSize;
	_finddata_t findData;

	long hFile = _findfirst(m_szFullname, &findData);

	if(hFile == -1L)
		uiFileSize = 0;
	else
	{
		uiFileSize = findData.size;
		_findclose(hFile);
	}

	return uiFileSize;
}


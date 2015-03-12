#ifndef _EVOFILE_H_
#define _EVOFILE_H_

class EvoFile
{
   public:

	enum EEvoFileFlag
	{
		READ,
		WRITE,
	};

	// Opening/Closing files
	static EvoFile *OpenFile(const char *szFilename, EEvoFileFlag eFlags);
	static void CloseFile(EvoFile *&pEvoFile);
	
	uint GetSize();

	template<class T>
	uint Read(T &type);

	// Read bytes from file
	uint ReadBytes(void *pData, uint uiSize);

	// Write bytes to file
	uint WriteBytes(const void *pData, uint uiSize);

	// Filename/Path/Ext accesors
	const char *GetFullname()	{ return m_szFullname;	}
	const char *GetFilename()	{ return m_szFilename;	}
	const char *GetPath()		{ return m_szPath;		}
	const char *GetExt()		{ return m_szExt;		}

   protected:
	EvoFile();
	~EvoFile();

   protected:
	char *m_szFullname;
	char *m_szFilename;
	char *m_szExt;
	char *m_szPath;

	FILE *m_pFile;
};

template<class T>
uint EvoFile::Read(T &type)
{
	return ReadBytes((void *)&type, sizeof(T));
}

#endif

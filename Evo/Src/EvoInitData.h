#ifndef _EVOINITDATA_H_
#define _EVOINITDATA_H_

enum EEvoRenderer;

class EvoInitData
{
   public:
	   EvoInitData(): m_uiScreenX(0),m_uiScreenY(0)	{ m_bFullscreen = false; m_uiScreenWidth = 1280; m_uiScreenHeight = 720; m_uiBufferCount = 2;	}
	~EvoInitData() {};

	bool Load(char *szFilename);

	bool			m_bFullscreen;
	uint			m_uiScreenX;
	uint			m_uiScreenY;
	uint			m_uiScreenWidth;
	uint			m_uiScreenHeight;
	uint			m_uiBufferCount;
};

// Inlines

inline bool EvoInitData::Load(char *szFilename)
{
	EvoParser cParser;

	if (!cParser.Initialize(szFilename))
		return false;

	cParser.SetGroupKey("EVO");
	cParser.GetKeyValue("FULLSCREEN", (int *)&m_bFullscreen);
	cParser.GetKeyValue("SCREENX", &m_uiScreenX);
	cParser.GetKeyValue("SCREENY", &m_uiScreenY);
	cParser.GetKeyValue("SCREENWIDTH",  &m_uiScreenWidth);
	cParser.GetKeyValue("SCREENHEIGHT", &m_uiScreenHeight);
	cParser.GetKeyValue("BUFFERCOUNT", &m_uiBufferCount);

	return true;
}

#endif
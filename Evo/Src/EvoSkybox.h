#ifndef _EVOSKYBOX_H_
#define _EVOSKYBOX_H_

class EvoSkybox
{
   public:
	EvoSkybox();
	~EvoSkybox();

	// Pass in filename of unit cube model
	void Initialize(char *szFilename);

	void Render(EvoCamera *pCamera);

   protected:
	EvoNode *m_pNode;

	EvoDepthStencilState m_DSState;
};

// Inlines


#endif
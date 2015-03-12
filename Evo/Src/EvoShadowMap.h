#ifndef _EVOSHADOWMAP_H_
#define _EVOSHADOWMAP_H_

class EvoTexture;
class EvoRenderTarget;
class EvoGUI;
class EvoGUIRectWidget;
class EvoCamera;
class EvoShader;

class EvoShadowMap 
{
public:
    EvoShadowMap();
    ~EvoShadowMap();

	void Init(int width=1024, int height=1024);
	void Shutdown();

	void Update(){};
    void Render();

	EvoTexture *GetTexture() { return m_pRenderedTexture;}
	void SetShadowCamera(const EvoCamera *pCamera);

	void UpdateShaderConstants(EvoCamera *pCamera);
private:
	int m_TextureWidth;
	int m_TextureHeight;
	EvoTexture *m_pRenderedTexture;
	EvoTexture *m_pDepthStencil;
	EvoRenderTarget *m_pRenderTarget; 

	EvoShader *m_pShader;

	EvoCamera *m_pLightCamera;

};


#endif  //#ifndef _EVOSHADOWMAP_H_

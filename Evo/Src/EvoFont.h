#ifndef _EVOFONT_H_
#define _EVOFONT_H_

#define MAX_FONT_LENGTH	256
#define MAX_FONT_VERTS	512
#define MAX_FONT_INDICES MAX_FONT_VERTS*4

// Flags to pass to Drawing Routine
#define BITMAPFONT_CENTER_X			1
#define BITMAPFONT_CENTER_Y			2
#define BITMAPFONT_CENTER_XY		(BITMAPFONT_CENTER_X | BITMAPFONT_CENTER_Y)
#define BITMAPFONT_ADDITIVEBLEND	4

struct FONTCHAR {
	int x;
	int y;
	int Width;
	float tu, tv;
	float tWidth;
};

struct FONTVERT {
	float sx, sy, sz;
	D3DCOLOR color;
	float tu, tv;
};

class EvoFont
{
   public:
	EvoFont();
	~EvoFont();

	// Initialize the BitmapFont
	// Ex. Initialize("Fonts\\Font1.bmp", "ABCD", lpRoadWarLib) if the font only had ABCD in it, in that order in the BMP File.
	// Spaces should not be a part of the Font.bmp file, Spaces will be the width of the first letter specified in Chars.
	void Initialize(char *Filename, char *Chars);

	// Draw Text onto lpSurf at x,y,  Note: x,y is the Upperleft corner of where the text is drawn
	void DrawText(float x, float y, char *Text, DWORD dwFlags = NULL);

	// All Letters will now have nPixels between them, Default = 0
	void SetSpacing(int nPixels);

	// Useful for effects such as ITALICS, or wavy letters
	void SetHorizontalShift(float Top, float Bottom);
	void SetVerticalShift(float Left, float Right);

	// Scale the Font, All calls to GetStringWidth and GetFontHeight will reflect new Scale
	void SetScale(float UniformScale);
	float GetScale()	{ return Scale; }

	void SetTint(EvoColor Top, EvoColor Bottom);

	// returns the Width in Pixels of the String using current Spacing
	float GetStringWidth(char *Text);

	// returns the Height in Pixels of the Font
	int GetFontHeight();

private:
	void PreProcess(char *Chars);

	uint m_uiFontSurfWidth, m_uiFontSurfHeight;
	EvoTexture *m_pTex;

	int FontHeight;
	float tFontHeight;

	FONTCHAR FontCharacter[MAX_FONT_LENGTH];

	int Spacing;
	float Scale;

	EvoColor TopTint, BottomTint;
	float HorizTopShift, HorizBottomShift, VertLeftShift, VertRightShift;

	EvoBuffer *m_pIB;
	EvoBuffer *m_pVB;
	EvoInputLayoutHandle m_hInputLayout;

	EvoShader *m_pShader;
};

#endif

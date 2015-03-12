#include "EvoStdAfx.h"

class EvoRect
{
   public:
	float top, left, right, bottom;
};

// This Function Traverses the Column and returns True if a solid column of Black
// Pixels is present
BOOL bEndCharacter(int Col, int CurrentY, DWORD *lpDDMemory, int PitchDiv4, int FontHeight);

int FindNextRow(int CurrentY, DWORD *lpDDMemory, int PitchDiv4, int FontSurfHeight, int FontSurfWidth);

EvoFont::EvoFont()
{
	m_uiFontSurfWidth = m_uiFontSurfHeight = 0;
	m_pTex = NULL;
	m_pIB = NULL;
	m_pVB = NULL;
	FontHeight = 0;
	Scale = 1.0F;
}

EvoFont::~EvoFont()
{
	RESMGR->DestroyTexture(m_pTex);
	m_pTex = NULL;
}

// Initialize the BitmapFont
void EvoFont::Initialize(char *lpFilename, char *Chars)
{
	HorizTopShift = HorizBottomShift = VertLeftShift = VertRightShift = 0.0F;
	
	// Create a Staging texture that we can preprocess on the cpu.
	m_pTex = RESMGR->CreateTexture(lpFilename, NULL, true);

	// No Space between characters
	Spacing = 0;

	PreProcess(Chars);

	RESMGR->DestroyTexture(m_pTex);

	// Create a normal texture that we can bind to the pipeline
	m_pTex = RESMGR->CreateTexture(lpFilename, NULL, false);

	uint uiIBSize = MAX_FONT_INDICES*2;
	char *pIBMem = new char[uiIBSize];

	m_pIB = EVO->CreateBuffer(uiIBSize, 2, pIBMem, EvoBind::INDEX_BUFFER, EvoUsage::DYNAMIC, EvoCpuAccess::WRITE, false, false);

	uint uiVBSize = MAX_FONT_VERTS*sizeof(FONTVERT);
	char *pVBMem = new char[uiVBSize];

	m_pVB = EVO->CreateBuffer(uiVBSize, sizeof(FONTVERT), pVBMem, EvoBind::VERTEX_BUFFER, EvoUsage::DYNAMIC, EvoCpuAccess::WRITE, false, false);

	m_pShader = RESMGR->CreateShader("Screenspace.fx");

	EvoInputElements InputElements;
	InputElements.AddElement(EvoInputElementUsage::POSITION, 0, EvoFormat::R32G32B32_FLOAT, 0, 0, false, 0);
	InputElements.AddElement(EvoInputElementUsage::COLOR, 0, EvoFormat::R8G8B8A8_UNORM, 0, 12, false, 0);
	InputElements.AddElement(EvoInputElementUsage::TEXCOORD, 0, EvoFormat::R32G32_FLOAT, 0, 16, false, 0);

	m_hInputLayout = EVO->GetInputLayoutMgr()->RegisterInputElements(InputElements);

}

// Draw Text onto lpSurf at x,y
void EvoFont::DrawText(float x, float y, char *Text, DWORD dwFlags)
{
//	return;  // Until we've implemented shader

		// Get the length of the string to be Drawn
	int CharCnt = strlen(Text);

	if (!CharCnt)
		return;
	
	// If the Flags specify to Center on X, adjust x accordingly
	if (dwFlags & BITMAPFONT_CENTER_X) {
		float StringWidth = GetStringWidth(Text);
		x -= (StringWidth/2);
	}

	// If the Flags specify to Center on Y, adjust y accordingly
	if (dwFlags & BITMAPFONT_CENTER_Y) {
		y -= ((FontHeight*Scale)/2.0F);
	}

	EvoRect DstR;
	
	DstR.top = y;
	DstR.bottom = DstR.top + FontHeight*Scale;

	DstR.top = DstR.top;
	DstR.bottom = DstR.bottom;
	
	float Modifier = 0.5f;

	unsigned short	*lpIndices = NULL;
	FONTVERT		*lpVerts = NULL;

	lpIndices = (unsigned short *)m_pIB->Lock(EvoLock::DISCARD, true);
	lpVerts = (FONTVERT *)m_pVB->Lock(EvoLock::DISCARD, true);

	int VertIndex = 0, FaceIndex = 0;
	for (int cnt = 0; cnt < CharCnt; cnt++) {
		if (Text[cnt] != ' ') {
			float top = FontCharacter[Text[cnt]].tv;
			float bottom = top + (tFontHeight);
			float left = FontCharacter[Text[cnt]].tu;
			float right = left + FontCharacter[Text[cnt]].tWidth;

			DstR.left = x;
			DstR.right = x + FontCharacter[Text[cnt]].Width*Scale;

			//DstR.left = DstR.left;
			//DstR.right = DstR.right;
				
			lpVerts[VertIndex].sx = (float)DstR.left + HorizTopShift - Modifier;
			lpVerts[VertIndex].sy = (float)DstR.top + VertLeftShift - Modifier;
			lpVerts[VertIndex].sz = 0.0F;
			lpVerts[VertIndex].color = TopTint.GetD3DColor();
			lpVerts[VertIndex].tu = left;
			lpVerts[VertIndex].tv = top;
			
			VertIndex++;

			lpVerts[VertIndex].sx = (float)DstR.right + HorizTopShift - Modifier;
			lpVerts[VertIndex].sy = (float)DstR.top + VertRightShift - Modifier;
			lpVerts[VertIndex].sz = 0.0F;
			lpVerts[VertIndex].color = TopTint.GetD3DColor();
			lpVerts[VertIndex].tu = right;
			lpVerts[VertIndex].tv = top;
			VertIndex++;

			lpVerts[VertIndex].sx = (float)DstR.right + HorizBottomShift - Modifier;
			lpVerts[VertIndex].sy = (float)DstR.bottom + VertRightShift - Modifier;
			lpVerts[VertIndex].sz = 0.0F;
			lpVerts[VertIndex].color = BottomTint.GetD3DColor();
			lpVerts[VertIndex].tu = right;
			lpVerts[VertIndex].tv = bottom;
			VertIndex++;

			lpVerts[VertIndex].sx = (float)DstR.left + HorizBottomShift - Modifier;
			lpVerts[VertIndex].sy = (float)DstR.bottom + VertLeftShift - Modifier;
			lpVerts[VertIndex].sz = 0.0F;
			lpVerts[VertIndex].color = BottomTint.GetD3DColor();
			lpVerts[VertIndex].tu = left;
			lpVerts[VertIndex].tv = bottom;
			VertIndex++;

			x += ((FontCharacter[Text[cnt]].Width + Spacing)*Scale);

			int CurrentIndex = VertIndex-4;

			lpIndices[FaceIndex] = lpIndices[FaceIndex+5] = CurrentIndex;
			lpIndices[FaceIndex+1] = CurrentIndex+1;
			lpIndices[FaceIndex+2] = lpIndices[FaceIndex+3] = CurrentIndex+2;
			lpIndices[FaceIndex+4] = CurrentIndex+3;
			
			FaceIndex += 6;
		}
		else {
			x += (FontCharacter[Text[cnt]].Width*Scale);
		}
	}

	m_pIB->Unlock();
	m_pVB->Unlock();

	
	m_pShader->SetTechnique("TScreenspace");

	uint uiNumShaderPasses = 0;
	m_pShader->Begin(&uiNumShaderPasses);

	EVO->SetInputLayout(m_hInputLayout, m_pShader, m_pShader->GetCurrentTechniqueName());

	EVO->SetIB(m_pIB, m_pIB->GetStride(), 0);
	EVO->SetVB(m_pVB, 0, m_pVB->GetStride(), 0);
	EVO->SetPrimitiveType(EvoPrimitive::TRIANGLELIST);

	for (uint ui = 0; ui < uiNumShaderPasses; ui++)
	{
		m_pShader->BeginPass(ui);

		EVO->BindToPS(0, m_pTex);
		EVO->DrawIndexed(VertIndex, FaceIndex, 0, 0);

		m_pShader->EndPass();
	}
	m_pShader->End();
}

void EvoFont::SetHorizontalShift(float Top, float Bottom)
{
	HorizTopShift = Top;
	HorizBottomShift = Bottom;
}

void EvoFont::SetVerticalShift(float Left, float Right)
{
	VertLeftShift = Left;
	VertRightShift = Right;
}

// All Letters will now have nPixels between them, Default = 0
void EvoFont::SetSpacing(int nPixels)
{
	Spacing = nPixels;
}

void EvoFont::SetScale(float UniformScale)
{
	Scale = UniformScale;
}

void EvoFont::SetTint(EvoColor Top, EvoColor Bottom)
{
	TopTint = Top;
	BottomTint = Bottom;
}

int EvoFont::GetFontHeight()
{
	return (int)(FontHeight*Scale);
}

// returns the Width in Pixels of the String using current Spacing
float EvoFont::GetStringWidth(char *Text)
{
	int CharCnt = strlen(Text);

	int StringWidth = 0;

	for (int cnt = 0; cnt < CharCnt; cnt++) {
		StringWidth += FontCharacter[Text[cnt]].Width + Spacing;	
	}
	StringWidth -= Spacing;

	return StringWidth*Scale;
}

void EvoFont::PreProcess(char *Chars)
{
	int CurrentX = 0;	// Current Column of the Surface we are Processing
	int CurrentY = 0;
	int CharCnt = strlen(Chars);	// Get the # of Characters in the Font

	if (!CharCnt)
		return;

	// Lock the surface to gain access to surface contents
	m_pTex->GetInfo(&m_uiFontSurfWidth, &m_uiFontSurfHeight);

	uint uiRowPitch;
	uint uiPitchDiv4;
	DWORD *lpDDMemory = (DWORD *)m_pTex->Lock(EvoLock::READ, true, &uiRowPitch);
	uiPitchDiv4 = uiRowPitch/4;

	FontHeight = FindNextRow(CurrentY, lpDDMemory, uiPitchDiv4, m_uiFontSurfHeight, m_uiFontSurfWidth);
	tFontHeight = ((float)FontHeight)/m_uiFontSurfHeight;

	BOOL bFinished = FALSE;

	// For every character in the font Increment CurrentX until a solid column of black is found
	for (int cnt = 0; cnt < CharCnt && bFinished == FALSE; cnt++) {
		FontCharacter[Chars[cnt]].Width = 0;		// Initialize Width to 0
		FontCharacter[Chars[cnt]].x = CurrentX;		// Initialize Characters Starting Column
		FontCharacter[Chars[cnt]].y = CurrentY;
		FontCharacter[Chars[cnt]].tu = ((float)CurrentX)/m_uiFontSurfWidth;		// Initialize Characters Starting Column
		FontCharacter[Chars[cnt]].tv = ((float)CurrentY)/m_uiFontSurfHeight;
			
		// Increase Width and CurrentX until a solid column of Black is found
		while (!bEndCharacter(CurrentX, CurrentY, lpDDMemory, uiPitchDiv4, FontHeight)) {
			FontCharacter[Chars[cnt]].Width++;
			CurrentX++;
		}
		FontCharacter[Chars[cnt]].tWidth = ((float)FontCharacter[Chars[cnt]].Width)/m_uiFontSurfWidth;
		CurrentX++;	// Increment to next characters starting point
		if (bEndCharacter(CurrentX, CurrentY, lpDDMemory, uiPitchDiv4, FontHeight)) {
			CurrentX = 0;
			CurrentY = FindNextRow(CurrentY, lpDDMemory, uiPitchDiv4, m_uiFontSurfHeight, m_uiFontSurfWidth);
			if (CurrentY == -1)
				bFinished = TRUE;
		}
	}

	// Make spaces the width of the first character in the font
	char c = ' ';
	FontCharacter[c].Width = FontCharacter[Chars[0]].Width;
	FontCharacter[c].x = 0;

	// Unlock the surface
	m_pTex->Unlock();
}

// This Function Traverses the Column and returns True if a solid column of Black
// Pixels is present
BOOL bEndCharacter(int Col, int CurrentY, DWORD *lpDDMemory, int PitchDiv4, int FontHeight)
{

	// Move down the current column of the surface and return false if a non zero pixel is found
	for (int cnt = CurrentY; cnt < FontHeight+CurrentY; cnt++) {
		if (((lpDDMemory[cnt*PitchDiv4 + Col] & 0x000000FF) >= 0x00000002))
			return FALSE;
	}

	// No Non-Black pixels found, Must be end of character
	return TRUE;
}

int FindNextRow(int CurrentY, DWORD *lpDDMemory, int PitchDiv4, int FontSurfHeight, int FontSurfWidth)
{
	BOOL bNextRow = FALSE;

	while (!bNextRow && CurrentY < FontSurfHeight) {
		bNextRow = TRUE;
		for (int cnt = 0; cnt < FontSurfWidth; cnt++) {
			if ((lpDDMemory[CurrentY*PitchDiv4 + cnt]  & 0x000000FF) >= 0x00000002)
				bNextRow = FALSE;
		}
		CurrentY++;
	}

	BOOL bEndFont = TRUE;
	if (CurrentY >= FontSurfHeight)
		return -1;
	else {
		for (int cnt = 0; cnt < FontSurfWidth; cnt++) {
			if (lpDDMemory[CurrentY*PitchDiv4 + cnt] != 0)
				bEndFont = FALSE;
		}
	}

	if (bEndFont)
		return -1;

	return CurrentY;
}
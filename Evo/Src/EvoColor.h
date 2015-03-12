#ifndef _EVOCOLOR_H_
#define _EVOCOLOR_H_

class EvoColor
{
   public:
	EvoColor();
	EvoColor(uchar ucR, uchar ucG, uchar ucB, uchar ucA);

	~EvoColor();

	D3DCOLOR GetD3DColor()		{ return m_Color;	}

   public:
	D3DCOLOR m_Color;
};


#endif

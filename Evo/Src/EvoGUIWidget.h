#ifndef _EVOGUIWIDGET_H_
#define _EVOGUIWIDGET_H_

class EvoGUIWidget
{
   public:
	EvoGUIWidget():m_bEnabled(true) {};
	virtual ~EvoGUIWidget() {};
	
	// Pure virtual Render function
	virtual void Render() = 0;

	bool IsEnabled() { return m_bEnabled;}
	void Enable(bool bVal) { m_bEnabled = bVal;}

   protected:
	bool m_bEnabled;

};

#endif
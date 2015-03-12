#include "EvoStdAfx.h"

// Direct Input Constructor
EvoInput::EvoInput()
{
	m_pDI = NULL;
	m_pDIKeyboard = NULL;
	m_pDIJoystick = NULL;
	m_pDIMouse = NULL;

	bForceFeedback = FALSE;

	memset(&m_abPrevJoyStates[0],0,sizeof(m_abPrevJoyStates));
	memset(&m_abJoyStates[0],0,sizeof(m_abJoyStates));
}

// Destructor
EvoInput::~EvoInput(void)
{

	if (bMouseAcquired) {
		m_pDIMouse->Unacquire();
		bMouseAcquired = FALSE;
	}

    if(bKeyboardAcquired)
    {
        m_pDIKeyboard->Unacquire();
        bKeyboardAcquired = FALSE;
    }

	if(bJoystickAcquired)
    {
        m_pDIJoystick->Unacquire();
        bJoystickAcquired = FALSE;
    }

	if (m_pDIMouse != NULL)
		m_pDIMouse->Release();

    if(m_pDIKeyboard != NULL)
        m_pDIKeyboard->Release();

	if(m_pDIJoystick != NULL)
        m_pDIJoystick->Release();

    if(m_pDI != NULL)
        m_pDI->Release();   
}

// Direct Input Constructor
void EvoInput::Initialize()
{
	HRESULT hRes;

	m_pDI = NULL;
	m_pDIKeyboard = NULL;
	m_pDIJoystick = NULL;
	bJoystickPresent = FALSE;
	m_pDIMouse = NULL;

	memset(&Keys[0], 0, sizeof(BYTE)*KEYCNT);
	memset(&JoyState, 0, sizeof(DIJOYSTATE2));
	memset(&MouseState, 0, sizeof(DIMOUSESTATE));

    // Create DI Object
	DirectInput8Create(EVO->GetInstanceHandle(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL);
    
	// try to create keyboard device
    m_pDI->CreateDevice(GUID_SysKeyboard, &m_pDIKeyboard, NULL);
    
    // Tell CDirectInput that we want to receive data in keyboard format
    m_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    
	// Set Cooperative Level
	m_pDIKeyboard->SetCooperativeLevel(EVO->GetWindowHandle(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	
    // Try to acquire the keyboard
	hRes = m_pDIKeyboard->Acquire();
	if(hRes == DI_OK)
		bKeyboardAcquired = TRUE;
	else bKeyboardAcquired = FALSE;


	m_pDI->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL);
	m_pDIMouse->SetDataFormat(&c_dfDIMouse);

#ifdef _DEBUG
	m_pDIMouse->SetCooperativeLevel(EVO->GetWindowHandle(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
#else 
	m_pDIMouse->SetCooperativeLevel(EVO->GetWindowHandle(), DISCL_EXCLUSIVE | DISCL_BACKGROUND);
#endif
	
	hRes = m_pDIMouse -> Acquire();
	if (hRes == DI_OK)
		bMouseAcquired = TRUE;
	else bMouseAcquired = FALSE;

	// Enumerate Direct Joystick Input Devices and Create if Found
	m_pDI -> EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDirectInputDevicesCallback, this, DIEDFL_ATTACHEDONLY);

	if (m_pDIJoystick != NULL) {		// Joystick has been Created
		bJoystickPresent = TRUE;
		m_pDIJoystick->SetDataFormat(&c_dfDIJoystick2);
		m_pDIJoystick->SetCooperativeLevel(EVO->GetWindowHandle(), DISCL_EXCLUSIVE | DISCL_FOREGROUND/* DISCL_NONEXCLUSIVE | DISCL_BACKGROUND*/);

		// Try to acquire the Joy
		hRes = m_pDIJoystick->Acquire();
		if(hRes == DI_OK)
			bJoystickAcquired = TRUE;
		else bJoystickAcquired = FALSE;

		DIPROPRANGE diprange;  
		diprange.diph.dwSize       = sizeof(DIPROPRANGE); 
		diprange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
		diprange.diph.dwObj        = DIJOFS_X; // device property 
		diprange.diph.dwHow        = DIPH_BYOFFSET; 
 
		m_pDIJoystick->GetProperty(DIPROP_RANGE, &diprange.diph); 

		XLength = diprange.lMax - diprange.lMin;

		diprange.diph.dwObj        = DIJOFS_Y; // device property 
		m_pDIJoystick->GetProperty(DIPROP_RANGE, &diprange.diph);

		YLength = diprange.lMax - diprange.lMin;

		diprange.diph.dwObj        = DIJOFS_Z; // device property 
		m_pDIJoystick->GetProperty(DIPROP_RANGE, &diprange.diph);

		ZLength = diprange.lMax - diprange.lMin;

//		SetJoystickDeadzone(lpcEngine->IniStruct.JoystickDeadzone);

		DIDEVCAPS DIDevCaps;
		memset(&DIDevCaps, 0, sizeof(DIDevCaps));
		DIDevCaps.dwSize = sizeof(DIDevCaps);
		m_pDIJoystick->GetCapabilities(&DIDevCaps);

		if (DIDevCaps.dwFlags & DIDC_FORCEFEEDBACK)
			bForceFeedback = TRUE;
		else bForceFeedback = FALSE;
	}
	else {
		bJoystickPresent = FALSE;
		bJoystickAcquired = FALSE;
	}

	MousePosition.x = MousePosition.y = 0;
	SetCursorPos(MousePosition.x, MousePosition.y);

	SetAxisRange(200, 200, 200);
	SetJoystickDeadzone(25);
}

void EvoInput::SetJoystickDeadzone(int Deadzone)
{
	if(bJoystickPresent)
	{
		DIPROPDWORD dipdword;  
		dipdword.diph.dwSize       = sizeof(DIPROPDWORD); 
		dipdword.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdword.dwData			   = Deadzone * 100;	// % of the Joystick is Dead Zone
		dipdword.diph.dwObj        = 0; // device property 
		dipdword.diph.dwHow        = DIPH_DEVICE; 
		m_pDIJoystick->SetProperty(DIPROP_DEADZONE, &dipdword.diph); 
	}
}

// Read Input from All Input Devices
void EvoInput::ReadInput(void)
{
	ReadKeyboard();
	ReadJoystick();
	ReadMouse();
}

void EvoInput::ReadMouse()
{
	HRESULT hRes;

	hRes = m_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &MouseState);

    if (hRes == DIERR_INPUTLOST) {
		// we lost control of the Mouse, reacquire
		bMouseAcquired = FALSE;
		if (m_pDIMouse->Acquire() == DI_OK)
			bMouseAcquired = TRUE;
	}

	GetCursorPos(&MousePosition);
}

void EvoInput::ReadKeyboard()
{
	HRESULT hRes;

	hRes = m_pDIKeyboard->GetDeviceState(256, Keys);

	if(hRes != DI_OK)
	{
		if(hRes == DIERR_INPUTLOST)
		{
			// we lost control of the keyboard, reacquire
			bKeyboardAcquired = FALSE;
			if (m_pDIKeyboard->Acquire() == DI_OK)
				bKeyboardAcquired = TRUE;
			
		}
    }
}

void EvoInput::ReadJoystick()
{
	HRESULT hRes;

	if (bJoystickPresent) {
		m_pDIJoystick -> Poll();
		if (m_pDIJoystick != NULL) {
			hRes = m_pDIJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &JoyState);

			if(hRes != DI_OK)
			{
				if(hRes == DIERR_INPUTLOST)
				{
					// we lost control of the keyboard, reacquire
					bJoystickAcquired = FALSE;
					if (m_pDIJoystick->Acquire() == DI_OK)
						bJoystickAcquired = TRUE;
					
				}
			}
		}

		JoyState.lX = (int)(XRangeDivXLength*JoyState.lX - XRangeDiv2);
		JoyState.lY = (int)(YRangeDivYLength*JoyState.lY - YRangeDiv2);
		JoyState.lZ = (int)(ZRangeDivZLength*JoyState.lZ - ZRangeDiv2);

		JoyState.lRx = (int)(XRangeDivXLength*JoyState.lRx - XRangeDiv2);
		JoyState.lRy = (int)(YRangeDivYLength*JoyState.lRy - YRangeDiv2);
		JoyState.lRz = (int)(ZRangeDivZLength*JoyState.lRz - ZRangeDiv2);

		memcpy(&m_abPrevJoyStates[0], &m_abJoyStates[0], sizeof(m_abJoyStates));
		memset(&m_abJoyStates[0], 0, sizeof(m_abJoyStates));
		memset(&m_afJoyValues[0], 0, sizeof(m_afJoyValues));

		if(JoyState.lX > 0)
		{
			m_abJoyStates[DIJ_XPOS] = true;
			m_afJoyValues[DIJ_XPOS] = (float)JoyState.lX/XRangeDiv2;
		}
		if(JoyState.lY  >0)
		{
			m_abJoyStates[DIJ_YPOS] = true;
			m_afJoyValues[DIJ_YPOS] = (float)JoyState.lY/YRangeDiv2;
		}
		if(JoyState.lZ>0)
		{
			m_abJoyStates[DIJ_ZPOS] = true;
			m_afJoyValues[DIJ_ZPOS] = (float)JoyState.lZ/ZRangeDiv2;
		}
		if(JoyState.lX<0)
		{
			m_abJoyStates[DIJ_XNEG] = true;
			m_afJoyValues[DIJ_XNEG] = -(float)JoyState.lX/XRangeDiv2;
		}
		if(JoyState.lY<0)
		{
			m_abJoyStates[DIJ_YNEG] = true;
			m_afJoyValues[DIJ_YNEG] = -(float)JoyState.lY/YRangeDiv2;
		}
		if(JoyState.lZ<0)
		{
			m_abJoyStates[DIJ_ZNEG] = true;
			m_afJoyValues[DIJ_ZNEG] = -(float)JoyState.lZ/ZRangeDiv2;
		}

		if(JoyState.lRx > 0)
		{
			m_abJoyStates[DIJ_X2POS] = true;
			m_afJoyValues[DIJ_X2POS] = (float)JoyState.lRx/XRangeDiv2;
		}
		if(JoyState.lRy > 0)
		{
			m_abJoyStates[DIJ_Y2POS] = true;
			m_afJoyValues[DIJ_Y2POS] = (float)JoyState.lRy/YRangeDiv2;
		}
		if(JoyState.lRz>0)
		{
			m_abJoyStates[DIJ_Z2POS] = true;
			m_afJoyValues[DIJ_Z2POS] = (float)JoyState.lRz/ZRangeDiv2;
		}
		if(JoyState.lRx<0)
		{
			m_abJoyStates[DIJ_X2NEG] = true;
			m_afJoyValues[DIJ_X2NEG] = -(float)JoyState.lRx/XRangeDiv2;
		}
		if(JoyState.lRy<0)
		{
			m_abJoyStates[DIJ_Y2NEG] = true;
			m_afJoyValues[DIJ_Y2NEG] = -(float)JoyState.lRy/YRangeDiv2;
		}
		if(JoyState.lRz<0)
		{
			m_abJoyStates[DIJ_Z2NEG] = true;
			m_afJoyValues[DIJ_Z2NEG] = -(float)JoyState.lRz/ZRangeDiv2;
		}

		for(int i = 0;i < MAX_JOY_BUTTONS; i++)
		{
			if(JoyState.rgbButtons[i])
			{
				m_abJoyStates[DIJ_BUTTON1 + i] = true;
			}
		}

		BOOL POVCentered = (LOWORD(JoyState.rgdwPOV[0]) == 0xFFFF);
		if(!POVCentered)
		{
			if(JoyState.rgdwPOV[0] > 31500 || JoyState.rgdwPOV[0] < 4500)
				m_abJoyStates[DIJ_POV_FWD] = true;
			else if(JoyState.rgdwPOV[0] > 4500 && JoyState.rgdwPOV[0] < 13500)
				m_abJoyStates[DIJ_POV_RIGHT] = true;
			else if(JoyState.rgdwPOV[0] > 13500 && JoyState.rgdwPOV[0] < 22500)
				m_abJoyStates[DIJ_POV_BACK] = true;
			else if(JoyState.rgdwPOV[0] > 22500 && JoyState.rgdwPOV[0] < 31500)
				m_abJoyStates[DIJ_POV_LEFT] = true;
		}
	}
}

//Range = 1000 Means  -500 to 500
void EvoInput::SetAxisRange(int Range_X, int Range_Y, int Range_Z)
{
	XRange = Range_X;
	YRange = Range_Y;
	ZRange = Range_Z;

	XRangeDiv2 = XRange/2.0F;
	YRangeDiv2 = YRange/2.0F;
	ZRangeDiv2 = ZRange/2.0F;
	
	XRangeDivXLength = XRange/((float)XLength);
	YRangeDivYLength = YRange/((float)YLength);
	ZRangeDivZLength = ZRange/((float)ZLength);
}

void EvoInput::ReacquireDevices()
{
	 // Try to acquire the keyboard
	HRESULT hRes = m_pDIKeyboard->Acquire();
	if(hRes == DI_OK)
		bKeyboardAcquired = TRUE;
	else bKeyboardAcquired = FALSE;

	// Try to acquire the Joy
		hRes = m_pDIJoystick->Acquire();
		if(hRes == DI_OK)
			bJoystickAcquired = TRUE;
		else bJoystickAcquired = FALSE;
}

// Enumerate Direct Input Devices, Looking For Joystick
BOOL CALLBACK EnumDirectInputDevicesCallback(LPCDIDEVICEINSTANCE lpDIDI, LPVOID lpRef)
{
	if (!(lpDIDI -> dwDevType & DI8DEVCLASS_GAMECTRL))
		return DIENUM_CONTINUE;
	else {
		LPDIRECTINPUTDEVICE8 lpTempDev = NULL;
		EvoInput *pInput = (EvoInput *)lpRef;

		if (pInput->m_pDI->CreateDevice(lpDIDI->guidInstance, &(pInput->m_pDIJoystick), NULL) != DI_OK)
			return DIENUM_CONTINUE;

		if (lpTempDev != NULL)
			lpTempDev->Release();
		return DIENUM_STOP;
	}
}




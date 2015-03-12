#ifndef _EVOINPUT_H_
#define _EVOINPUT_H_

#define KEYCNT	256
#define MAX_JOY_BUTTONS 16

enum
{
	DIJ_XPOS,
	DIJ_YPOS,
	DIJ_ZPOS,
	DIJ_XNEG,
	DIJ_YNEG,
	DIJ_ZNEG,
	DIJ_X2POS,
	DIJ_Y2POS,
	DIJ_Z2POS,
	DIJ_X2NEG,
	DIJ_Y2NEG,
	DIJ_Z2NEG,
	DIJ_BUTTON1,
	DIJ_BUTTON2,
	DIJ_BUTTON3,
	DIJ_BUTTON4,
	DIJ_BUTTON5,
	DIJ_BUTTON6,
	DIJ_BUTTON7,
	DIJ_BUTTON8,
	DIJ_BUTTON9,
	DIJ_BUTTON10,
	DIJ_BUTTON11,
	DIJ_BUTTON12,
	DIJ_BUTTON13,
	DIJ_BUTTON14,
	DIJ_BUTTON15,
	DIJ_BUTTON16,
	DIJ_POV_FWD,
	DIJ_POV_RIGHT,
	DIJ_POV_BACK,
	DIJ_POV_LEFT,
	NUM_JOY_DEV_CONSTANTS
};

#define X360_A				(DIJ_BUTTON1)
#define X360_B				(DIJ_BUTTON2)
#define X360_X				(DIJ_BUTTON3)
#define X360_Y				(DIJ_BUTTON4)
#define X360_LB				(DIJ_BUTTON5)	// Left Bumper
#define X360_RB				(DIJ_BUTTON6)	// Right Bumper
#define X360_LT				(DIJ_ZPOS)		// Left Trigger
#define X360_RT				(DIJ_ZNEG)		// Right Trigger
#define X360_DPAD_FWD		(DIJ_POV_FWD)
#define X360_DPAD_RIGHT		(DIJ_POV_RIGHT)
#define X360_DPAD_BACK		(DIJ_POV_BACK)
#define X360_DPAD_LEFT		(DIJ_POV_LEFT)
#define X360_LSTICK_FWD		(DIJ_YNEG)
#define X360_LSTICK_RIGHT	(DIJ_XPOS)
#define X360_LSTICK_BACK	(DIJ_YPOS)
#define X360_LSTICK_LEFT	(DIJ_XNEG)
#define X360_RSTICK_FWD		(DIJ_Y2NEG)
#define X360_RSTICK_RIGHT	(DIJ_X2POS)
#define X360_RSTICK_BACK	(DIJ_Y2POS)
#define X360_RSTICK_LEFT	(DIJ_X2NEG)

class EvoInput
{
	// Functions
   public:
	EvoInput();
	~EvoInput();

	void Initialize();

	void SetJoystickDeadzone(int Deadzone);

	// Reads Input from ALL input Devices
	// Joystick Input Will be Normalized to Range(set using SetAxisRange)
	void ReadInput(void);

	// Individual Functions in case you dont need to read all devices
	// Mouse Gets Read Automatically as its State Changes
	void ReadKeyboard();
	void ReadJoystick();
	void ReadMouse();

	BYTE GetKey(uint uiKey)				{ return Keys[uiKey];		}

	// Gets Immediate State of Joystick
	bool GetJoy(uint uiJoy)						{ return m_abJoyStates[uiJoy];	}

	float GetNormalizedJoyValue(uint uiJoy)		{ return m_afJoyValues[uiJoy];	}

	// Gets only new state, so if the Button A was pressed last frame, this would return false this frame even if its still pressed.
	bool GetJoyPressed(uint uiJoy)		{ return m_abJoyStates[uiJoy] && !m_abPrevJoyStates[uiJoy];	}

	// Range = 1000 Means  -500 to 500
	void SetAxisRange(int Range_X, int Range_Y, int Range_Z);

	float GetXRangeDiv2() { return XRangeDiv2; };
	float GetYRangeDiv2() { return YRangeDiv2; };
	float GetZRangeDiv2() { return ZRangeDiv2; };

	LPDIRECTINPUT8	GetDirectInput()				{ return m_pDI;	}
	LPDIRECTINPUTDEVICE8 GetDirectInputJoystick()	{ return m_pDIJoystick;	}

	void ReacquireDevices();

   protected:
	int XLength;  // XMax - XMin, Used to Normalize Joystick Axis Range
	int YLength;
	int ZLength;
	int XRange, YRange, ZRange;

	float XRangeDivXLength, YRangeDivYLength, ZRangeDivZLength;
	float XRangeDiv2, YRangeDiv2, ZRangeDiv2;

	// Data
	LPDIRECTINPUT8			m_pDI;
	LPDIRECTINPUTDEVICE8	m_pDIKeyboard;
	LPDIRECTINPUTDEVICE8	m_pDIMouse;
	LPDIRECTINPUTDEVICE8	m_pDIJoystick;		// Can Support Force Feedback

	BOOL bKeyboardAcquired;		// has the keyboard been acquired
	BOOL bJoystickAcquired;     // Is Joystick Acquired
	BOOL bMouseAcquired;		// Is Mouse Acquired

	BOOL bJoystickPresent;
	BOOL bForceFeedback;		// Is Joystick ForceFeedback Capable

	// The Read Input Function Fills these 2 Structures, You know a keys has been pressed by
	// doing a 'if (Keys[DIK_SPACE] != 0)', See Dinput.h for a complete list of DIK Codes

	POINT			MousePosition;	// Absolute Mouse Position(Screen Coordinates)
	DIMOUSESTATE	MouseState;		// Mouse Button Info and Relative Mouse Movements
	DIJOYSTATE2		JoyState;
	BYTE			Keys[KEYCNT];

	bool			m_abJoyStates[NUM_JOY_DEV_CONSTANTS];
	bool			m_abPrevJoyStates[NUM_JOY_DEV_CONSTANTS];
	float			m_afJoyValues[NUM_JOY_DEV_CONSTANTS];
	
	friend BOOL CALLBACK EnumDirectInputDevicesCallback(LPCDIDEVICEINSTANCE lpDIDI, LPVOID lpRef);
};

// Enumerate Direct Input Devices, Looking For Joystick
BOOL CALLBACK EnumDirectInputDevicesCallback(LPCDIDEVICEINSTANCE lpDDI, LPVOID pvRef);

#endif




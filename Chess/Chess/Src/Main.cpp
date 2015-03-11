#include "stdafx.h"

#define MAX_ORDER (3)
#define APP_NAME "Example"

static BOOL AppInit(HINSTANCE hInstance, LPSTR lpCmdLine);

long FAR PASCAL WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL bQuit = FALSE;  // Global Boolean to Set When Quiting

HWND MainHwnd;       // Handle to the Window

DWORD Prev = 0;

BOOL bAppActivated = FALSE;

float CurrentTime;

EvoCamera cCamera;
EvoFont cFont;
EvoTimer Timer;
EvoInput Input;
EvoGUI gGUI;		// CREATING THE GUI HERE!!!!!!
Game* chessMatch;			 

/***************************************************************************
*                            WinMain                                       *
****************************************************************************
*																		   *		
* Initializes the application then enters a message loop which calls	   *
* Render until a quit message is received.								   *
***************************************************************************/
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) // THIS IS THE ENTRANCE TO MAIN
{
	int iSeed = timeGetTime();
	srand((unsigned int) iSeed);
	
    MSG msg;
	float fSecondsPassed;
	float totalSeconds;

	char FPSString[MAX_PATH];
	char SecPassString[MAX_PATH];
	
	FPSString[0] = NULL;
	SecPassString[0] = NULL;

    if(!AppInit(hInstance, lpCmdLine))
        return FALSE;

	Timer.Start();
	totalSeconds = 0.0;

    while(!bQuit) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		fSecondsPassed = Timer.GetSecondsPassed();
		totalSeconds = totalSeconds + fSecondsPassed;

		//calculate the FPS
		sprintf(FPSString, "FPS: %.1f", 1.0f/fSecondsPassed);

		//calculate the Seconds Passed
		sprintf(SecPassString, "Seconds Passed: %.1f", totalSeconds);

		if(!bQuit) {
			Input.ReadInput();

			EVO->Begin();

			EVO->Clear();

			chessMatch->Tick(fSecondsPassed);

			gGUI.Render();

			// cFont.DrawText(20, 10, FPSString);
			// cFont.DrawText(200, 10, SecPassString);

			EVO->End();

			EVO->Present();
		}

	}

	return 0; 
}

BOOL AppAlreadyRunning(HWND CurrentHwnd)
{
	char Class[MAX_PATH];

	HWND NextHwnd = GetNextWindow(CurrentHwnd, GW_HWNDNEXT);

	while(NextHwnd)
	{
		GetClassName(NextHwnd, Class, MAX_PATH);
		if(!stricmp(Class, APP_NAME))
			return TRUE;

		NextHwnd =  GetNextWindow(NextHwnd, GW_HWNDNEXT);
	}

	NextHwnd =  GetNextWindow(CurrentHwnd, GW_HWNDPREV);
	while(NextHwnd)
	{
		GetClassName(NextHwnd, Class, MAX_PATH);
		if(!stricmp(Class, APP_NAME))
			return TRUE;

		NextHwnd =  GetNextWindow(NextHwnd, GW_HWNDPREV);
	}

	return FALSE;
}

static BOOL AppInit(HINSTANCE hInstance, LPSTR lpCmdLine)
{
    WNDCLASS wc;

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, "AppIcon");
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = APP_NAME;
    
	RegisterClass(&wc);
     
	int ScreenX, ScreenY;
	BOOL bFull;

	EvoParser cP("Ini\\Engine.ini");

	cP.SetGroupKey("ENGINE");
	cP.GetKeyValue("SCREENX", &ScreenX);
	cP.GetKeyValue("SCREENY", &ScreenY);
	cP.GetKeyValue("FULLSCREEN", &bFull);

	DWORD WStyle = 0;//WS_EX_TOPMOST;
	// if(!bFull)
	//	WStyle = 0;

    MainHwnd = CreateWindowEx(WStyle, APP_NAME, APP_NAME, WS_POPUP, 0, 0, ScreenX, ScreenY, NULL, NULL, hInstance, NULL);                            

	if(AppAlreadyRunning(MainHwnd)) {
		DestroyWindow(MainHwnd);
		return FALSE;
	}

	ShowCursor(TRUE);
	ShowCursor(TRUE);

    ShowWindow(MainHwnd, SW_SHOWNORMAL);
    UpdateWindow(MainHwnd);
	SetFocus(MainHwnd);

	if (!EVO->Initialize("Ini\\Engine.ini", MainHwnd, hInstance))
		return FALSE;	

	EvoPoint3 EyeV(0.0f, 0.0f, -30.0f);
	EvoPoint3 AtV(0.0f, 0.5f, 0.0f);

	cCamera.LookAt(&EyeV, &AtV);
		
	EVO->SetTransform(EVO_TRANSFORM_VIEW, cCamera.GetViewMatrix());
	

	cFont.Initialize("Fonts\\16FontImp12.dds", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.,/%$!@#[]+=?:;'0123456789<>~");
	cFont.SetSpacing(1);
	cFont.SetScale(2.0);

	Input.Initialize();

	// Create big boys burger shop here
	chessMatch = new Game(); // newing the pointer right before we initialize the chessMatch
	chessMatch->Initialize();

	return TRUE;
}

/*
 * WindowProc
 * Main window message handler.
 */
long FAR PASCAL WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MSG Msg;
	Msg.message = message;
	Msg.wParam = wParam;
	Msg.lParam = lParam;
	switch(message) 
	{
		case WM_CREATE:
			break;

		case WM_LBUTTONDBLCLK:
			chessMatch->KeyPressed(wParam, lParam);
			break;

		case WM_RBUTTONDBLCLK:
			break;

		case WM_LBUTTONDOWN:
			chessMatch->KeyPressed(wParam, lParam);
			break;

        case WM_LBUTTONUP:
			chessMatch->KeyPressed(wParam, lParam);
			break;

		case WM_RBUTTONDOWN:
			break;

        case WM_RBUTTONUP:
			break;

        case WM_MOUSEMOVE: 
           	break;

		case WM_COMMAND:
			break;

		case WM_CHAR:
			break;
			

		case WM_KEYDOWN:
			{
 				if(wParam == VK_ESCAPE) 
				{
					bQuit = TRUE;
					SendMessage(hwnd, WM_DESTROY, 0, 0);
				}
 				/*else if (wParam == VK_UP)
				{
					chessMatch.RaiseWager();
				}
				else if (wParam == VK_DOWN)
				{
					chessMatch.LowerWager();
				}*/
				else
				{
					chessMatch->KeyPressed(wParam, lParam);
				}
			}
			break;

		case WM_ACTIVATEAPP: {
				BOOL fActive = (BOOL) wParam;
				DWORD dwThreadID = (DWORD) lParam;   

				if(fActive && bAppActivated)
				{
					
				}
				else if(bAppActivated)
				{

				}

				if(fActive)
					bAppActivated = TRUE;
			}
			return 0L;

        case WM_DESTROY:
			bQuit = TRUE;
            PostQuitMessage(0);
            break;
   
    }

    return (long)DefWindowProc(hwnd, message, wParam, lParam);
}


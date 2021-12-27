#include <windows.h>

#include <idc.h>
#include <brain.h>



HINSTANCE	g_hInst;
int g_test;
char g_chTest[1024*40];

/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		
		/* Upon destruction, tell the main thread to stop */
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		case WM_COMMAND: {
			switch(LOWORD(wParam)){
				case IDC_START: {
					wsprintf(g_chTest, "Click count = %d", g_test++);
					//SetWindowText(hwnd, g_chTest);
					test(g_chTest);
					SetWindowText(hwnd, g_chTest);
					UpdateWindow(hwnd);
					break;
				}
			}	
		}
		case WM_CREATE: {
			CreateWindow("button", "SendButton", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						 10, 10, 100, 30, hwnd, (HMENU)IDC_START, g_hInst, NULL);
			break;
		}
		case WM_PAINT: {
			HDC hdc;
			PAINTSTRUCT ps;
			HFONT font, oldfont;
			char str[] = "ÆùÆ® test !0123456789abcdefghijklmnopqrstuvwxyz";
			LOGFONT lf;
			int i, j;
			
			hdc = BeginPaint(hwnd, &ps);
			strcpy(lf.lfFaceName, "±Ã¼­");
			//oldfont = (HFONT)SelectObject(hdc, font);
			SetTextColor(hdc, RGB(255,0,0));
			SetBkColor(hdc, RGB(255,255,0));
			//TextOut(hdc, 10, 10, str, strlen(str));
			TextOut(hdc, 10, 10, g_chTest, strlen(g_chTest));
			
			EndPaint(hwnd, &ps);
			break;
		}
		
		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG msg; /* A temporary location for all messages */

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
#if 0	// Window size - normal
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","Caption",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		640, /* width */
		480, /* height */
		NULL,NULL,hInstance,NULL);
#else	// Window size - allways top
	/*
	hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		"WindowClass", "Caption", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);*/
	hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		"WindowClass", "Caption", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 140, 80,
		NULL, (HMENU)NULL, hInstance, NULL);
#endif
	ShowWindow(hwnd, nCmdShow);
	
	#if 1
	// Transparent window - start
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, 0, 127, LWA_ALPHA);
	// Transparent window - end
	#endif

	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	/*
		This is the heart of our program where all input is processed and 
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	while(GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}
	return msg.wParam;
}

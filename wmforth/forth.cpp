// forth.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "forth.h"
#include <commctrl.h>

#include <stdio.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE			hInst;			// The current instance
HWND				hwndCB;			// The command bar handle

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About			(HWND, UINT, WPARAM, LPARAM);


extern "C" int __fastcall forth_main (	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow);

extern "C" void createfile(void);

extern "C" HINSTANCE ghInstance, ghPrevInstance;
extern "C" LPTSTR glpCmdLine;
extern "C" int gnCmdShow;
extern "C" HWND ghWnd, hEdit1, hButton1;
extern "C" LRESULT (*gWndProc) (HWND, UINT, WPARAM, LPARAM);
extern "C" int (*gPaint) (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

extern "C" int gotinput;
#define INSIZE 1000
extern "C" wchar_t inbuf[INSIZE];

extern "C" 	dump_asm (FILE *, unsigned int *, unsigned int *);

extern "C" int initext(void);


#define DX(xxx) strncpy(labels[nl].name, #xxx, LEN); labels[nl].value = (int)xxx; nl++;

#define DA(xxx) tabadr[nl].adr = (int)xxx; strcpy(tabadr[nl].name,#xxx); nl++; 


#define NLABELS 0x2000
#define LEN 0x20
extern "C" struct
{
	// unsigned int value;
	char name[LEN];
	unsigned int value;
} labels[NLABELS];
extern "C" int nl;

extern "C" struct
{
	// unsigned int adr;
	char name[LEN];
	unsigned int adr;
} *tabadr;

extern "C" void dumpcode (FILE *fdasm, char *s, void *a, void *b);
extern "C" void dumpvar (FILE *fdasm, char *s, int size);
extern "C" void dumpstring (FILE *fdasm, char *s, void *a, void *b);
void endfunctions();

wchar_t edit[] = L"EDIT";
wchar_t emptystring[] = L"";
wchar_t button[] = L"BUTTON";
wchar_t ok[] = L"OK";
wchar_t endstrings[] = L"ENDSTRINGS";

#define WIN
// #define SIMPLE
// #define GRAPH

#ifndef WIN
int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	wchar_t buf[1000];
	/*
	swprintf (buf, L"hInstance=%X", hInstance);
	MessageBox(0,buf,L"trace",MB_OK);
	*/
	// MessageBox (0, L"1", L"1", 0);
	// createfile();

	ghInstance = hInstance;
	ghPrevInstance = hPrevInstance;
	glpCmdLine = lpCmdLine;
	gnCmdShow = nCmdShow;

	forth_main (hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	// WinMain1 (hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	return 0;
}

#else

#ifdef SIMPLE

int WinMain1(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow);

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	/*
	HACCEL hAccelTable;
    FILE *f;
	union
	{
		unsigned int *pui;
		LRESULT (*f) (HWND, UINT, WPARAM, LPARAM);
	} u1, u2;

	u1.f = WndProc;
	u2.f = About;

	gPaint = NULL;

	f = fopen ("dump.txt", "w");
	dump_asm (f, u1.pui, u2.pui);
	fclose (f);
	*/

	FILE *fd;

	initext();
	fd = fopen ("dump.txt","w");

	DA(&hInst)
	DA(&hwndCB)
	DA(&ghInstance)
	DA(&ghPrevInstance)
	DA(&glpCmdLine)
	DA(&gnCmdShow)
	DA(&ghWnd)
	DA(&hEdit1)
	DA(&hButton1)
	DA(&gotinput)
	DA(inbuf)
	
	DA(edit)
	DA(emptystring)
	DA(button)
	DA(ok)

	dumpcode (fd, "WinMain1", WinMain1, MyRegisterClass);
	dumpcode (fd, "MyRegisterClass", MyRegisterClass, InitInstance);
	dumpcode (fd, "InitInstance", InitInstance, WndProc);
	dumpcode (fd, "WndProc", WndProc, endfunctions);

	dumpvar (fd, "&hInst", 4);
	dumpvar (fd, "&hwndCB", 4);
	dumpvar (fd, "&ghInstance", 4);
	dumpvar (fd, "&ghPrevInstance", 4);
	dumpvar (fd, "&glpCmdLine", 4);
	dumpvar (fd, "&gnCmdShow", 4);
	dumpvar (fd, "&ghWnd", 4);
	dumpvar (fd, "&hEdit1", 4);
	dumpvar (fd, "&hButton1", 4);
	dumpvar (fd, "&gotinput", 4);
	dumpvar (fd, "inbuf", 1000);

	dumpstring (fd, "edit", edit, emptystring);
	dumpstring (fd, "emptystring", emptystring, button);
	dumpstring (fd, "button", button, ok);
	dumpstring (fd, "ok", ok, endstrings);
	
	fclose(fd);

	// return WinMain1(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	wchar_t buf[1000];
	/*
	swprintf (buf, L"hInstance=%X", hInstance);
	MessageBox(0,buf,L"trace",MB_OK);
	*/
	createfile();

	ghInstance = hInstance;
	ghPrevInstance = hPrevInstance;
	glpCmdLine = lpCmdLine;
	gnCmdShow = nCmdShow;

	DX(WinMain1)

	forth_main (hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	return 0;
}

int  WinMain1(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;

	ghInstance = hInstance;
	ghPrevInstance = hPrevInstance;
	glpCmdLine = lpCmdLine;
	gnCmdShow = nCmdShow;

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	// hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_FORTH);

	// forth_main (hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		// if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		// {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		// }
	}

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    It is important to call this function so that the application 
//    will get 'well formed' small icons associated with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS	wc;

    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FORTH));
    wc.hCursor			= 0;
    wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);
}

//
//  FUNCTION: InitInstance(HANDLE, int)
//
//  PURPOSE: Saves instance handle and creates main window
//
//  COMMENTS:
//
//    In this function, we save the instance handle in a global variable and
//    create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND	hWnd;
	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

	hInst = hInstance;		// Store instance handle in our global variable
	// Initialize global strings
	LoadString(hInstance, IDC_FORTH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance, szWindowClass);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	ghWnd = hWnd;

	if (!hWnd)
	{	
		return FALSE;
	}
/*
	hEdit1 = CreateWindow (L"EDIT", L"Test", 
		WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|WS_HSCROLL|
		ES_MULTILINE|ES_WANTRETURN|ES_AUTOHSCROLL|ES_AUTOVSCROLL,
		20, 40, 200, 60,
		hWnd, NULL, hInstance, NULL);

	hButton1 = CreateWindow (
		L"BUTTON", L"OK",
		WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
		20, 140, 50, 20,
		hWnd, NULL, hInstance, NULL);
*/
#ifndef GRAPH
	hEdit1 = CreateWindow (edit, emptystring, 

		WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|WS_HSCROLL|
		ES_MULTILINE|ES_WANTRETURN|ES_AUTOHSCROLL|ES_AUTOVSCROLL,
		5, 50, 230, 130,
		
		hWnd, NULL, hInstance, NULL);
#else
	hEdit1 = CreateWindow (edit, emptystring, 
		
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_WANTRETURN, 
		60, 25, 175, 20,

		hWnd, NULL, hInstance, NULL);
#endif
	hButton1 = CreateWindow (
		button, ok,
		WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
		5, 25, 50, 20,
		hWnd, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	if (hwndCB)
		CommandBar_Show(hwndCB, TRUE);

	return TRUE;
}



// extern "C" void paint (HWND hWnd);
/*
void paint (HWND hWnd)
{
			RECT rt;
			HBRUSH brush;
			HPEN pen;
			PAINTSTRUCT ps;
			HDC hdc;

			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);

			brush = CreateSolidBrush (0x40C080);
			SelectObject (hdc, brush);
			pen = CreatePen (PS_SOLID,5,0x302010);
			SelectObject (hdc, pen);
			Rectangle (hdc, 230, 40, 300, 90);

			EndPaint(hWnd, &ps);
			
}
*/

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	TCHAR szHello[MAX_LOADSTRING];
	wchar_t bufinbuf[100];
	int i;

	switch (message) 
	{
		case WM_COMMAND:
			// swprintf (bufinbuf, L"COMMAND %X %X %X", wParam, hWnd, hButton1);
			// MessageBox (ghWnd, bufinbuf, L"message", MB_OK);
			// if (hWnd == hButton1)
			if (wParam == 0)
			{
				// MessageBox (ghWnd, L"Button1 pressed", L"message", MB_OK);
				SendMessage (hEdit1, WM_GETTEXT, INSIZE, (long)inbuf);
				for (i=0; inbuf[i]; i++)
					inbuf[i]++;
				SendMessage (hEdit1, WM_SETTEXT, 0, (long)inbuf);
				gotinput = 1;
				// swprintf (bufinbuf, L"%X", inbuf);
				// MessageBox (ghWnd, bufinbuf, L"adr inbuf", MB_OK);
				// MessageBox (ghWnd, inbuf, L"cont inbuf", MB_OK);
				return 0;
			}
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			/*switch (wmId)
			{
				case IDM_HELP_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_FILE_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:*/
				   return DefWindowProc(hWnd, message, wParam, lParam);
			/*}*/
			break;
#if 0
		case WM_CREATE:
			hwndCB = CommandBar_Create(hInst, hWnd, 1);			
			CommandBar_InsertMenubar(hwndCB, hInst, IDM_MENU, 0);
			CommandBar_AddAdornments(hwndCB, 0, 0);
			break;
		
		case WM_PAINT:
			RECT rt;
			/*
			wchar_t buf[100];
			// swprintf (buf, L"gPaint = %X", gPaint);
			swprintf (buf, L"hWnd = %X", hWnd);
			MessageBox (0, buf, L"Message", MB_OK);
			*/
			if (gPaint != NULL)
			{
				(*gPaint) (hWnd, message, wParam, lParam);
				// MessageBox (0, L"Hello !", L"Message", MB_OK);
			} 
			else
			{	
				// paint (hWnd, message, wParam, lParam);
				/*
				hdc = BeginPaint(hWnd, &ps);
				GetClientRect(hWnd, &rt);
				LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
				DrawText(hdc, szHello, _tcslen(szHello), &rt, 
					DT_SINGLELINE | DT_VCENTER | DT_CENTER);
				EndPaint(hWnd, &ps);
				*/
			}
			break;
#endif
		case WM_DESTROY:
			// CommandBar_Destroy(hwndCB);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void endfunctions (void) { }

/* 
void f1 (HWND hWnd) { }
void f2 (UINT message) { }
void f3 (WPARAM wParam) { }
void f4 (LPARAM lParam) { }


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	f1 (hWnd);
	f2 (message);
	f3 (wParam);
	f4 (lParam);
	if (gWndProc == NULL)
		return WndProc1 (hWnd, message, wParam, lParam);
	else
		return (*(gWndProc)) (hWnd, message, wParam, lParam);
}


// Mesage handler for the About box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rt, rt1;
	int DlgWidth, DlgHeight;	// dialog width and height in pixel units
	int NewPosX, NewPosY;

	switch (message)
	{
		case WM_INITDIALOG:
			// trying to center the About dialog
			if (GetWindowRect(hDlg, &rt1)) {
				GetClientRect(GetParent(hDlg), &rt);
				DlgWidth	= rt1.right - rt1.left;
				DlgHeight	= rt1.bottom - rt1.top ;
				NewPosX		= (rt.right - rt.left - DlgWidth)/2;
				NewPosY		= (rt.bottom - rt.top - DlgHeight)/2;
				
				// if the About box is larger than the physical screen 
				if (NewPosX < 0) NewPosX = 0;
				if (NewPosY < 0) NewPosY = 0;
				SetWindowPos(hDlg, 0, NewPosX, NewPosY,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			return TRUE;

		case WM_COMMAND:
			if ((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDCANCEL))
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
*/

#else

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;
    FILE *f;
	union
	{
		unsigned int *pui;
		LRESULT (*f) (HWND, UINT, WPARAM, LPARAM);
	} u1, u2;

	u1.f = WndProc;
	u2.f = About;

	gPaint = NULL;

	f = fopen ("dump.txt", "w");
	dump_asm (f, u1.pui, u2.pui);
	fclose (f);

	ghInstance = hInstance;
	ghPrevInstance = hPrevInstance;
	glpCmdLine = lpCmdLine;
	gnCmdShow = nCmdShow;

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_FORTH);

	forth_main (hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    It is important to call this function so that the application 
//    will get 'well formed' small icons associated with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS	wc;

    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FORTH));
    wc.hCursor			= 0;
    wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);
}

//
//  FUNCTION: InitInstance(HANDLE, int)
//
//  PURPOSE: Saves instance handle and creates main window
//
//  COMMENTS:
//
//    In this function, we save the instance handle in a global variable and
//    create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND	hWnd;
	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

	hInst = hInstance;		// Store instance handle in our global variable
	// Initialize global strings
	LoadString(hInstance, IDC_FORTH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance, szWindowClass);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	ghWnd = hWnd;

	if (!hWnd)
	{	
		return FALSE;
	}

	/*
	hEdit1 = CreateWindow (L"EDIT", L"Test", 
		WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|WS_HSCROLL|
		ES_MULTILINE|ES_WANTRETURN|ES_AUTOHSCROLL|ES_AUTOVSCROLL,
		20, 40, 200, 60,
		hWnd, NULL, hInstance, NULL);

	hButton1 = CreateWindow (
		L"BUTTON", L"OK",
		WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
		20, 140, 50, 20,
		hWnd, NULL, hInstance, NULL);
	*/
#ifndef GRAPH
	hEdit1 = CreateWindow (L"EDIT", L"", 
		WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|WS_HSCROLL|
		ES_MULTILINE|ES_WANTRETURN|ES_AUTOHSCROLL|ES_AUTOVSCROLL,
		5, 50, 230, /*130,*/ 72,
		hWnd, NULL, hInstance, NULL);
#else
	
	hEdit1 = CreateWindow (L"EDIT", L"", 
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_WANTRETURN, 
		60, 25, 175, 20,
		hWnd, NULL, hInstance, NULL);
	/*
	hEdit1 = CreateWindow (L"EDIT", L"",
		WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_HSCROLL |
		ES_MULTILINE | ES_WANTRETURN | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
		5, 50, 230, 72,
		hWnd, NULL, hInstance, NULL);
		*/
#endif


	hButton1 = CreateWindow (
		L"BUTTON", L"OK",
		WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
		5, 25, 50, 20,
		hWnd, NULL, hInstance, NULL);
// #endif
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	if (hwndCB)
		CommandBar_Show(hwndCB, TRUE);

	return TRUE;
}

extern "C" void paint (HWND hWnd);
/*
void paint (HWND hWnd)
{
			RECT rt;
			HBRUSH brush;
			HPEN pen;
			PAINTSTRUCT ps;
			HDC hdc;

			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);

			brush = CreateSolidBrush (0x40C080);
			SelectObject (hdc, brush);
			pen = CreatePen (PS_SOLID,5,0x302010);
			SelectObject (hdc, pen);
			Rectangle (hdc, 230, 40, 300, 90);

			EndPaint(hWnd, &ps);
			
}
*/

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

LRESULT CALLBACK WndProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	TCHAR szHello[MAX_LOADSTRING];
	wchar_t bufinbuf[100];

	switch (message) 
	{
		case WM_COMMAND:
			// swprintf (bufinbuf, L"COMMAND %X %X %X", wParam, hWnd, hButton1);
			// MessageBox (ghWnd, bufinbuf, L"message", MB_OK);
			// if (hWnd == hButton1)
			if (wParam == 0)
			{
				// MessageBox (ghWnd, L"Button1 pressed", L"message", MB_OK);
				SendMessage (hEdit1, WM_GETTEXT, INSIZE, (long)inbuf);
				gotinput = 1;
				// swprintf (bufinbuf, L"%X", inbuf);
				// MessageBox (ghWnd, bufinbuf, L"adr inbuf", MB_OK);
				// MessageBox (ghWnd, inbuf, L"cont inbuf", MB_OK);
				return 0;
			}
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_HELP_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_FILE_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_CREATE:
			hwndCB = CommandBar_Create(hInst, hWnd, 1);			
			CommandBar_InsertMenubar(hwndCB, hInst, IDM_MENU, 0);
			CommandBar_AddAdornments(hwndCB, 0, 0);
			break;
		case WM_PAINT:
			RECT rt;
			/*
			wchar_t buf[100];
			// swprintf (buf, L"gPaint = %X", gPaint);
			swprintf (buf, L"hWnd = %X", hWnd);
			MessageBox (0, buf, L"Message", MB_OK);
			*/
			if (gPaint != NULL)
			{
				(*gPaint) (hWnd, message, wParam, lParam);
				// MessageBox (0, L"Hello !", L"Message", MB_OK);
			} 
			else
			{	
				// paint (hWnd, message, wParam, lParam);
				/*
				hdc = BeginPaint(hWnd, &ps);
				GetClientRect(hWnd, &rt);
				LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
				DrawText(hdc, szHello, _tcslen(szHello), &rt, 
					DT_SINGLELINE | DT_VCENTER | DT_CENTER);
				EndPaint(hWnd, &ps);
				*/
			}
			break;
		case WM_DESTROY:
			CommandBar_Destroy(hwndCB);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void f1 (HWND hWnd) { }
void f2 (UINT message) { }
void f3 (WPARAM wParam) { }
void f4 (LPARAM lParam) { }


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	f1 (hWnd);
	f2 (message);
	f3 (wParam);
	f4 (lParam);
	if (gWndProc == NULL)
		return WndProc1 (hWnd, message, wParam, lParam);
	else
		return (*(gWndProc)) (hWnd, message, wParam, lParam);
}


// Mesage handler for the About box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rt, rt1;
	int DlgWidth, DlgHeight;	// dialog width and height in pixel units
	int NewPosX, NewPosY;

	switch (message)
	{
		case WM_INITDIALOG:
			// trying to center the About dialog
			if (GetWindowRect(hDlg, &rt1)) {
				GetClientRect(GetParent(hDlg), &rt);
				DlgWidth	= rt1.right - rt1.left;
				DlgHeight	= rt1.bottom - rt1.top ;
				NewPosX		= (rt.right - rt.left - DlgWidth)/2;
				NewPosY		= (rt.bottom - rt.top - DlgHeight)/2;
				
				// if the About box is larger than the physical screen 
				if (NewPosX < 0) NewPosX = 0;
				if (NewPosY < 0) NewPosY = 0;
				SetWindowPos(hDlg, 0, NewPosX, NewPosY,
					0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			return TRUE;

		case WM_COMMAND:
			if ((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDCANCEL))
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

#endif
#endif
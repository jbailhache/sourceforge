
#include <windows.h>
#include <windowsx.h>
#ifdef WM
#include <aygshell.h>
#endif
#include <time.h>

/* #include <unistd.h> */
#include <fcntl.h>

#include <winsock.h>

#include "serial.h"

#include "ext.h"

/* #define WIN32 */

extern HANDLE ghInstance;  
extern HWND ghwnd;
extern int gnCmdShow;

struct msgparam
{
	HWND hwnd;
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	int flag;
};

extern struct msgparam msgparam;

void trace (char *s);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND CreateWindow1 (
#ifdef CHAR8
	char *lpClassName,
	char *lpWindowName,
#else
	LPCWSTR lpClassName,
	LPCWSTR lpWindowName,
#endif
	DWORD dwStyle,
	int x,
	int y,
	int nWidth,
	int nHeight,
	HWND hwndParent,
	HMENU hMenu,
	HINSTANCE hInstance,
	LPVOID lpParam)
{
	HWND hwnd;
	hwnd = CreateWindow (lpClassName,
		lpWindowName,
		dwStyle,
		x,
		y,
		nWidth,
		nHeight,
		hwndParent,
		hMenu,
		hInstance,
		lpParam);
	return hwnd;
}

/*
ATOM RegisterClassEx1 (CONST WNDCLASSEXW* wndclass)
{
	ATOM r;
	r = RegisterClassEx (wndclass);
	return r;
}
*/

extern char obuf[];

struct test_struct1 xxx1;
struct test_struct2 xxx2;

long GetNthCharWstring (wchar_t *s, long i)
{
	return s[i];
} 

long MessageBox1 (HWND hwnd, 
#ifdef CHAR8
	char *s, char *c,
#else
	wchar_t *s, wchar_t *c, 
#endif
	long t)
{
	char buf[1000];
	int i;
	sprintf (obuf, "MessageBox %ld %ld %ld %ld",
			hwnd, s, c, t);
	trace(obuf);
	sprintf (obuf,"ghwnd=%ld", ghwnd);
	trace(obuf);

	for (i=0; s[i]; i++)
		buf[i] = s[i];
	buf[i] = 0;
	sprintf (obuf, "s=<%s>", buf);
	trace(obuf);

	for (i=0; s[i]; i++)
		buf[i] = c[i];
	buf[i] = 0;
	sprintf (obuf, "c=<%s>", buf);
	trace(obuf);

	MessageBox (hwnd, s, c, t);
}

SOCKET socket1 (int a, int b, int c)
{
	long s;
	sprintf (obuf, "socket %ld %ld %ld", a, b, c); trace(obuf);
	s = socket (a, b, c);
	trace("test");
	sprintf (obuf, "socket -> %ld", s); trace(obuf);
	return s;
}

long GetCommState1 (HANDLE hFile, LPDCB lpDCB)
{
	long r, a, b;
	a = (long)hFile;
	b = (long)lpDCB;
	sprintf (obuf, "GetCommState %ld %ld",a,b); trace(obuf);
	r = GetCommState (hFile, lpDCB);
	sprintf (obuf, "GetCommState -> %ld", r);
	return r;
}

#define DX(xname) if (!strcmp(name,#xname)) val=(long)xname; else

long val_ext1 (char *name, long *pval)
{
		long val;

		DX(time)
		DX(srand)
		DX(rand) 

		DX(exit)

		DX(GetSystemMetrics)
		DX(SM_CXFULLSCREEN)
		DX(atoi)
		DX(GetNthCharWstring)
		DX(MessageBox1)
		DX(GetCommState1)
		DX(socket1)
		DX(&xxx1)
		DX(&xxx2)

		DX (CreateFile)
		DX (CloseHandle)
		DX (ReadFile)
		DX (WriteFile)
		DX(GetCommState)
		DX(SetCommState)
		
		DX(SetCommMask)
		DX(EV_RXCHAR)

		DX(WaitCommEvent)
		
		DX(GetCommTimeouts)
		DX(SetCommTimeouts)
		DX(EscapeCommFunction)
		DX(GetLastError)
		DX(GetCommModemStatus)

		DX(NULL)
		DX(TRUE)
		DX(FALSE)

		DX(FILE_SHARE_DELETE)
		DX(FILE_SHARE_READ)
		DX(FILE_SHARE_WRITE)
		DX(CREATE_ALWAYS)
		DX(CREATE_NEW)
		DX(OPEN_ALWAYS)
		/* DX(OPEN_NEW) */
		DX(OPEN_EXISTING)
		DX(TRUNCATE_EXISTING)
		DX(FILE_ATTRIBUTE_ARCHIVE)
		DX(FILE_ATTRIBUTE_ENCRYPTED)
		DX(FILE_ATTRIBUTE_HIDDEN)
		DX(FILE_ATTRIBUTE_NORMAL)
		DX(FILE_ATTRIBUTE_OFFLINE)
		DX(FILE_ATTRIBUTE_READONLY)
		DX(FILE_ATTRIBUTE_SYSTEM)
		DX(FILE_ATTRIBUTE_TEMPORARY)
		DX(FILE_FLAG_BACKUP_SEMANTICS)
		DX(FILE_FLAG_DELETE_ON_CLOSE)
		DX(FILE_FLAG_NO_BUFFERING)
		// DX(FILE_FLAG_OPEN_NO_RECALL)
		// DX(FILE_FLAG_OPEN_REPARSE_POINT)
		DX(FILE_FLAG_OVERLAPPED)
		DX(FILE_FLAG_POSIX_SEMANTICS)
		DX(FILE_FLAG_RANDOM_ACCESS)
		DX(FILE_FLAG_SEQUENTIAL_SCAN)
		DX(FILE_FLAG_WRITE_THROUGH)

		DX(GENERIC_READ)
		DX(GENERIC_WRITE)
		
		DX(INVALID_HANDLE_VALUE)

		DX(DTR_CONTROL_ENABLE)
		DX(RTS_CONTROL_ENABLE)
		DX(NOPARITY)
		DX(ONESTOPBIT)

		DX(CLRBREAK)
		DX(CLRDTR)
		DX(CLRRTS)
		DX(SETBREAK)
		DX(SETDTR)
		DX(SETRTS)
		DX(SETXOFF)
		DX(SETXON)

		DX(EV_RXCHAR)

		DX(MS_CTS_ON)
		DX(MS_DSR_ON)
		DX(MS_RING_ON)
		DX(MS_RLSD_ON)

		DX(hPort)
		DX(hReadThread)
		DX(PortInitialize)
		DX(PortWrite)
		DX(PortReadThread)
		DX(PortClose)
		DX(LireSerie)

		DX(WSAStartup)
		DX(WSACleanup)
		DX(socket)
		DX(gethostbyname)
		DX(htons)
		DX(connect)
		DX(bind)
		DX(listen)
		DX(accept)
		DX(send)
		DX(recv)
		DX(closesocket)
		DX(memcpy)
		/* DX(WSAGetLastError) */

		DX(inet_addr)

		DX(PF_INET)
		DX(SOCK_STREAM)
		DX(IPPROTO_TCP)
		DX(AF_INET)

		DX(INVALID_SOCKET)

		DX(select)
		/* DX(ioctl) */
		DX(ioctlsocket)
		/* DX(fcntl) */
		/*DX(sleep)*/
		DX(FIONBIO)
		/*
		DX(F_SETFL)
		DX(O_NONBLOCK)
		*/

		DX (GetModuleHandle)
		DX(GetProcAddress)
		DX(LoadLibrary)

		{
			*pval = 0;
			return 0;
		}

		*pval = val; 
	
		return 1;
}

long val_ext2 (char *name, long *pval)
{
		long val;

		DX(ghwnd)
		DX(ghInstance)
		DX(&msgparam)
		DX(WndProc)

		DX(DefWindowProc)		

		DX(FormatMessage)

		DX(PeekMessage)
		DX(PM_NOREMOVE)
		DX(PM_REMOVE)

		DX(GetMessage)
		DX(TranslateMessage)
		DX(DispatchMessage)
		DX(SendMessage)
		DX(FindWindow)
		DX(SetForegroundWindow)
		DX(RegisterClass) 
		/* DX(RegisterClassEx)  */
		DX(CreateWindow1)
		DX(CreateWindowEx)
		DX(DestroyWindow)
		DX(GetWindowRect)
		DX(MoveWindow)
		DX(ShowWindow)
		DX(UpdateWindow)
		DX(GetDC)
		DX(ReleaseDC)
		DX(BeginPaint)
		DX(EndPaint)
		DX(GetClientRect)
		DX(CreatePen)
		DX(CreateSolidBrush)
		DX(SelectObject)
		DX(DeleteObject)
		DX(Rectangle)
		DX(MoveToEx)
		DX(LineTo)
		DX(DrawText)
#ifdef WM
		DX(SHCreateMenuBar)
		DX(SHHandleWMActivate)
		DX(SHHandleWMSettingChange)
#endif

		DX(GetStockObject)
             DX(gnCmdShow)

		DX(MessageBox)
		DX(MB_OK)

		DX(WM_GETTEXT)
		DX(WM_SETTEXT)

		DX(WM_CREATE)
		DX(WM_ACTIVATE)
		DX(WM_SETTINGCHANGE)
		DX(WM_PAINT)
		DX(WM_COMMAND)
		DX(WM_DESTROY)

		DX(EM_GETLINECOUNT)
		DX(EM_LINESCROLL)

		DX(CW_USEDEFAULT)

		DX(WS_SYSMENU)
		DX(WS_VISIBLE)
		DX(WS_CHILD)
		DX(WS_BORDER)
		DX(WS_VSCROLL)

		DX(WS_HSCROLL)
		DX(ES_MULTILINE)
		DX(ES_WANTRETURN)
		DX(ES_AUTOHSCROLL)
		DX(ES_AUTOVSCROLL)
		DX(ES_LEFT)

		/*
		DX(sizeof(PAINTSTRUCT))
		DX(sizeof(WNDCLASS))
		*/

		/* DX(gnCmdShow)  */
		DX(CS_VREDRAW)
		DX(CS_HREDRAW)
		/* DX(GetStockObject) */
		DX(WHITE_BRUSH)

		DX(TRUE)
		DX(FALSE)
		DX(LB_ERRSPACE)
		DX(CB_ERRSPACE)
		DX(CB_ERR)

		/*
		sprintf (obuf, "Undefined external <%s>", name);
		trace(obuf);
		*/
		/*
		val = 0;
		return val;
		*/

		{
			*pval = 0;
			return 0;
		}

		*pval = val;
		return 1;
}

long val_ext (char *name)
{
	int r;
	long val;
	r = val_ext1 (name, &val);
	if (r) return val;
	r = val_ext2 (name, &val);
	if (r) return val;

	sprintf (obuf, "Undefined external <%s>", name);
	trace(obuf);

	return 0;

}

#define DS(_tname) { _tname str; if (!strcmp(tname, #_tname)) { *sizestr = sizeof (_tname); 

#define DF(_fname) if (!strcmp(fname, #_fname)) \
	{ *offset = (char *)&str._fname - (char *)&str; *size = sizeof(&str._fname); return 1; } else

#define ES if (*fname) { sprintf(obuf, "Undefined field <%s> in <%s>", fname, tname); trace(obuf); } return 0; }}

/* long field_offset (char *tname, char *fname) */

int field (char *tname, char *fname, long *offset, long *size, long *sizestr)
{
	/*
	sprintf (obuf, "field_offset tname=<%s> fname=<%s>", tname, fname);
	trace(obuf);
	*/

	DS(struct test_struct1)
		DF(ccc)
		DF(sss)
		DF(lll)
	ES
	DS(struct test_struct2)
		DF(ppp)
		DF(ttt)
	ES

	DS(DCB)
		DF(DCBlength)
		DF(BaudRate)
		/*
		DF(fBinary)
		DF(fAbortOnError)
		DF(fDummy2)
		*/
		DF(XonLim)
		DF(XoffLim)
		DF(ByteSize)
		DF(Parity)
		DF(StopBits)
		DF(XonChar)
		DF(XoffChar)
		DF(ErrorChar)
		DF(EofChar)
		DF(EvtChar)
		ES

	DS(COMMTIMEOUTS)
		DF(ReadIntervalTimeout)
		DF(ReadTotalTimeoutMultiplier)
		DF(ReadTotalTimeoutConstant)
		DF(WriteTotalTimeoutMultiplier)
		DF(WriteTotalTimeoutConstant)
		ES

	DS(WSADATA)
		DF(wVersion)
		DF(wHighVersion)
		DF(szDescription)
		DF(szSystemStatus)
		DF(iMaxSockets)
		DF(iMaxUdpDg)
		DF(lpVendorInfo)
		ES

	DS(struct hostent)
		DF(h_name)
		DF(h_aliases)
		DF(h_addrtype)
		DF(h_length)
		DF(h_addr_list)
		ES

	DS(struct sockaddr)
		DF(sa_family)
		DF(sa_data)
		ES

	DS(struct sockaddr_in)
		DF(sin_family)
		DF(sin_port)
		DF(sin_addr)
		DF(sin_zero)
		ES

	DS(SOCKADDR_IN)
		DF(sin_family)
		DF(sin_port)
		DF(sin_addr)
		DF(sin_zero)
		ES

	DS(MSG)
		DF(hwnd)
		DF(message)
		DF(wParam)
		DF(lParam)
		DF(time)
		DF(pt)
		ES

	DS(struct msgparam)
		DF(hwnd)
		DF(message)
		DF(wParam)
		DF(lParam)
		DF(flag)
		ES

	DS(WNDCLASS)
		DF(style)
		DF(lpfnWndProc)
		DF(cbClsExtra)
		DF(cbWndExtra)
		DF(hInstance)
		DF(hIcon)
		DF(hCursor)
		DF(hbrBackground)
		DF(lpszMenuName)
		DF(lpszClassName)
		ES
/*
	DS(WNDCLASSEX)
		DF(cbSize)
		DF(style)
		DF(lpfnWndProc)
		DF(cbClsExtra)
		DF(cbWndExtra)
		DF(hInstance)
		DF(hIcon)
		DF(hCursor)
		DF(hbrBackground)
		DF(lpszMenuName)
		DF(lpszClassName)
		DF(hIconSm)
		ES
*/

	DS(RECT)
		DF(left)
		DF(top)
		DF(right)
		DF(bottom)
		ES

#ifdef WM
	DS(SHMENUBARINFO)
		DF(cbSize)
		DF(hwndParent)
		DF(dwFlags)
		DF(nToolBarId)
		DF(hInstRes)
		DF(nBmpId)
		DF(cBmpImages)
		DF(hwndMB)
		/* DF(clrBk) */
		ES
#endif

	DS(PAINTSTRUCT)
		DF(hdc)
		DF(fErase)
		DF(rcPaint)
		DF(fRestore)
		DF(fIncUpdate)
		DF(fIncUpdate)
		DF(rgbReserved)
		ES

	
	{
	*offset=-1;
	*size=-1;
	*sizestr = -1;

	sprintf (obuf, "Undefined type <%s> for field <%s>", tname, fname);
	trace(obuf);

	return 0;
	}

}

// int WSAGetLastError (void);

/*
tester ()
{
	WSAGetLastError ();
}
*/


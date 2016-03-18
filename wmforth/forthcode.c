
#include <windows.h>
#include <windowsx.h>
// #include <aygshell.h>

#include <stdio.h>
#include <math.h>
#include <winsock.h>
#include <commctrl.h>

FILE *ftrace;

FILE *infd;
FILE *outfd;

	HINSTANCE ghInstance;
	HINSTANCE ghPrevInstance;
	LPTSTR    glpCmdLine;
	int       gnCmdShow;
	HWND	  ghWnd, hEdit1, hButton1;
	LRESULT  (*gWndProc) (HWND, UINT, WPARAM, LPARAM) = NULL;
	int (*gPaint) (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = NULL;
	int gotinput = 0;
#define INSIZE 1000
	wchar_t inbuf[INSIZE];

	wchar_t bufm[1000];

	typedef unsigned int instr;

	extern unsigned int instrcode (char *opcode, int nargs, 
		unsigned int args[], char *com, int adr);	
	extern int asm (char *line, int adr);
	extern int codeasm (char *s, int adr);
	extern int substlabels (char *line, char *line1);
	extern instr *loadasm1 (instr *adr1, char *filename);
	extern char *getline (char *ptr, char *line);
	extern instr *asmem (instr *adr1, char *asmcode);

	void paint (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	extern HINSTANCE hInst;
	extern HWND hwndCB;

	ATOM MyRegisterClass (HINSTANCE,LPTSTR);
	BOOL InitInstance (HINSTANCE,int);
	LRESULT CALLBACK WndProc (HWND,UINT,WPARAM,LPARAM);
	void endfunctions (void);

	int __fastcall forth_main (	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow);

	int testweb(void);

	instr * compile_call (instr *ptrcode, int (*f)(), int regs, int res);

	void dump1 (void);
	void handle_message(void);

	int aaa, bbb;

	int flag_locals = 0;

#define TRACES

#ifdef TRACES
#define TRACE fprintf (ftrace, "lastword=%X freedic=%X freespace=%X ptrstack=%X ", lastword,freedic.i,freespace.i,ptrstack); fprintf (ftrace,
#define ENDTRACE ); fflush(ftrace); 

#define TRACE2 fprintf (ftrace, "RB=%X ", rb.f()); fprintf (ftrace,
#define ENDTRACE2 ); fflush(ftrace); 

#endif

/*
#define TRACE2 fprintf (ftrace, "", 
#define ENDTRACE2 );
*/
/*
char buftrace[1000];

#define TRACE sprintf (buftrace, "exe=<%s> lastword=%X freespace=%X ", exenamea, lastword,freespace.i); fprintf (ftrace,
#define ENDTRACE ); 
*/

void notrace (int x, ...)
{
	int a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16;
	return;
}

#ifndef TRACES
// #define TRACE notrace (
// la definition precedente provoque un bug dans VARGS, a etudier
#define TRACE fprintf (ftrace, "", 
#define ENDTRACE );
#define TRACE2 fprintf (ftrace, "", 
#define ENDTRACE2 );
#endif


char buftrace[1000];
wchar_t wbuftrace[1000];
int itrace;

/*
#define TRACE sprintf (buftrace, 
#define ENDTRACE ); \
	for (itrace=0; buftrace[itrace]; itrace++) \
		wbuftrace[itrace]=buftrace[itrace]; \
	wbuftrace[itrace] = 0; \
	MessageBox (ghwnd, wbuftrace, L"Trace", MB_OK); 
*/

typedef wchar_t carac;

// extern HWND ghwnd;

HWND ghwnd = (HWND)0;

void printasm (char *s, unsigned int x, unsigned int adr);
int codeasm (char *s, int adr);
void dump (void);
void mainloop(void);

int wstrcmp (wchar_t *s1, wchar_t *s2)
{
	int i;
	i = 0;
	for (;;)
	{
		if (s1[i] == 0 && s2[i] == 0)
			return 0;
		if (s1[i] != s2[i])
			return 1;
		i++;
	}
}

int testcall (void)
{
	MessageBox (ghwnd, L"Test call", L"Message", MB_OK);
}

int divide (int a, int b)
{
	return a / b;
}

int modulo (int a, int b)
{
	return a % b;
}

int match (unsigned int x, unsigned int pattern, unsigned int mask)
{
	return ((x & mask) == (pattern & mask));
}

double doubleofint  (int i)
{
	return (double)i;
}

int intofdouble (double x)
{
	return (int)x;
}

double dadd (double a, double b)
{
	return a + b;
}

double dsub (double a, double b)
{
	return a - b;
}

double dmult (double a, double b)
{
	return a * b;
}

double ddiv (double a, double b)
{
	return a / b;
}

int plus (int a, int b)
{
	return a + b;
}

int fputc1 (char c, FILE *f)
{
	fputc (c, f);
	// TRACE "**************** fputc1 %X %X\n", c, f ENDTRACE
}

int forthtest (carac *in, carac *out)
{
	for (;;)
	{
		if (!*in)
		{
			*out = 0;
			break;
		}
		*out = *in + 1;
		in++;
		out++;
	}
}

typedef wchar_t carac;
#define sprf swprintf
#define sscf swscanf
void scpy (carac *, carac *);

#define CODESIZE 8000
#define NLEVELS 16
// typedef unsigned int instr;

typedef instr *pinstr;

typedef union
{
	int i;
	int *pi;
	struct header *ph;
	int (*fi) ();
	instr * (*f) ();
	instr *pcode;
	char *pchar;
	carac *pcarac;
	FILE *fd;
	instr instruc;
} any;

typedef struct header
{
	struct header *link;
	any value;
	any param;
	int wmode;
	struct header *ph;
	any imvalue;
	int length;
	carac name[2];
} *pheader;


instr codes[NLEVELS][CODESIZE];

#define MODECP 0
#define MODEIM 1
#define MODEIM1 2

instr *imcodeptr;
instr imcodebuf[0x10000]; /* immediate code */
any imcode;

pheader getreadword (void);

#define NINLEVELS 16
// carac *inptrs[NINLEVELS];
instr *ptrcodes[NLEVELS];

#define STACKSIZE 0x20000
int stack[STACKSIZE+0x10000];

/*
#define PSTACKSIZE 8000
int pstack[PSTACKSIZE];
*/

carac cr;
#define WORDSIZE 200
carac wr[WORDSIZE];
char awr[WORDSIZE];

typedef struct fp
{
	int (*f) ();
	any p;
} fp;

#define INSTACKSIZE 256
fp instack[INSTACKSIZE];

fp out;

any startcodes[NLEVELS];

#define DICSIZE 30000
int dic[DICSIZE];

#define DICCODESIZE 60000
int diccode[DICCODESIZE];

wchar_t exename[120];
char exenamea[120];

struct
{
	int mode;
	carac curchar;
	carac curword[WORDSIZE];
	carac *inptr, *outptr;
	int inlevel;
	// FILE *out;
	// instr *ptrcode;
	int level;
	// any startcode;
	pheader lastword;
	any freespace;
	any freedic;
	int *ptrstack;
	int *ptrpstack;
	int iflevel;
	int wlevel;

	int *dic;
	int *diccode;
	instr *codes;
	instr **ptrcodes;
	carac *wr;
	char *awr;
	fp *instack;
	any *startcodes;
	pheader phcalled;
} globals;


#define mode (globals.mode)
#define inptr (globals.inptr)
#define outptr (globals.outptr)
#define inlevel (globals.inlevel)
// #define out (globals.out)
// #define ptrcode (globals.ptrcode)
#define level (globals.level)
// #define startcode (globals.startcode)
#define lastword (globals.lastword)
#define freespace (globals.freespace)
#define freedic (globals.freedic)
#define ptrstack (globals.ptrstack)
#define ptrpstack (globals.ptrpstack)
#define iflevel (globals.iflevel)
#define wlevel (globals.wlevel)
#define phcalled (globals.phcalled)

// #define ptrcode (ptrcodes[level])

// instr *ptrcode;
#define startcode (startcodes[level])

instr *gptrcode;
any gparam;
pheader gph;

pheader findword (carac *name);
pheader createword (carac *name);
pheader getword (carac *name);

	
int bufcode_rb[10];
any rb;


void savecore (char *name)
{
	FILE *f;
	f = fopen (name, "wb");
	fwrite (&globals, 1, sizeof(globals), f);
	fwrite (ptrcodes, 1, sizeof(ptrcodes), f);
	fwrite (startcodes, 1, sizeof(startcodes), f);
	fwrite (dic, 1, sizeof(dic), f);
	fwrite (diccode, 1, sizeof(diccode), f);
	fclose (f);
}

void restorecore (char *name)
{
	FILE *f;
	f = fopen (name, "rb");
	fread (&globals, 1, sizeof(globals), f);
	fread (ptrcodes, 1, sizeof(ptrcodes), f);
	fread (startcodes, 1,  sizeof(startcodes), f);
	fread (dic, 1, sizeof(dic), f);
	fread (diccode, 1, sizeof(diccode), f);
	fclose(f);
}

void savedic (char *name)
{
	FILE *f;
	f = fopen (name, "wb");
	fwrite (&lastword, 1, sizeof(lastword), f);
	fwrite (&freespace, 1, sizeof(freespace), f);
	fwrite (&freedic, 1, sizeof(freedic), f);
	fwrite (dic, 1, sizeof(dic), f);
	fwrite (diccode, 1, sizeof(diccode), f);
	fclose (f);
}

void restoredic (char *name)
{
	FILE *f;
	f = fopen (name, "rb");
	fread (&lastword, 1, sizeof(lastword), f);
	fread (&freespace, 1, sizeof(freespace), f);
	fread (&freedic, 1, sizeof(freedic), f);
	fread (dic, 1, sizeof(dic), f);
	fread (diccode, 1, sizeof(diccode), f);
	fclose(f);
}

int writecharmem (any *p, carac c)
{
	*(p->pchar)++ = c;
}


int writecaracmem (any *p, carac c)
{
	*(p->pcarac)++ = c;
}

int writecaracfile (any *p, carac c)
{
	fputc (c, p->fd);
	fflush (p->fd);
}

int writecarac (carac c)
{
	(*(out.f)) (&(out.p), c);
}

int writestring (carac *s)
{
	TRACE "writestring <%s>\n", s ENDTRACE
	while (*s)
	{
		TRACE "writecarac '%c' %X\n", *s, *s ENDTRACE
		writecarac (*s++);
	}
}

int readcharmem (any *p)
{
	cr = *(p->pchar)++;
}

int readcaracmem (any *p)
{
	cr = *(p->pcarac)++;
	// swprintf (bufm, L"readcaracmem -> '%c' %X", cr, cr);
	// MessageBox (0, bufm, L"message", MB_OK);
}

int readcaracfile (any *p)
{
	cr = fgetc (p->fd);
	if (feof(p->fd))
		cr = 0;
}

carac readcarac (void)
{
loop:
	(*(instack[inlevel].f)) (&(instack[inlevel].p));
	// TRACE "read carac %X <%c>\n", cr, (char)cr ENDTRACE
	if (cr)
		return cr;	
	else if (inlevel > 0)
	{
		inlevel--;
		TRACE "inlevel %d\n", inlevel ENDTRACE
		goto loop;
	}
	else
		return 0;
}

int isblnk (carac c)
{
	if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
		return 1;
	else
		return 0;
}

int iswordcarac (carac c)
{
	return (!isblnk(c) && c);
}

void readto (char delim, char *ptr)
{
 for (;;)
 {
  readcarac();
  if (cr==delim)
   break; 
  *ptr++ = cr;
 }
 *ptr++ = 0;
}
 
void readword (void)
{
	int i;
	readcarac();
	// TRACE "readword: 1st cr = '%c' %X\n", cr, cr ENDTRACE
	while (isblnk (cr))
	{
		readcarac();
		// TRACE "skip blanks: cr = '%c' %X\n", cr, cr ENDTRACE
	}
	i = 0;
	while (iswordcarac(cr))
	{
		// TRACE "read cr = '%c' %X\n", cr, cr ENDTRACE
		wr[i] = cr;
		awr[i] = cr;
		// TRACE "awr = <%s>\n", awr ENDTRACE
		i++;
		readcarac();
	}
	wr[i] = 0;
	awr[i] = 0;
	TRACE "Word read: <%s>\n", awr ENDTRACE
}


/*
carac word[WORDSIZE];
char cword[WORDSIZE];
*/

pheader getword (carac *name);
int forth (carac *inptr1, carac *outptr1);

int scmp (carac *a, carac *b);
void scpy (carac *dst, carac *src);
int slen (carac *s);

int sscanf1 (char *buf, char *format, double *p)
{
	int status;
	TRACE "sscanf <%s> <%s> %X\n", buf, format, p ENDTRACE
	status = sscanf (buf, format, p);
	TRACE "sscanf -> %X, %lf\n", status, *p ENDTRACE
}

/*
#define DL(xxx) if (!strcmp (name, #xxx)) return (int)xxx; else
#define DX(xxx) if (!strcmp (name, #xxx)) return (int)xxx; else
*/


#define NLABELS 0x2000
#define LEN 0x20
struct
{
	// unsigned int value;
	char name[LEN];
	unsigned int value;
} labels[NLABELS];
int nl;


struct
{
	// unsigned int adr;
	char name[LEN];
	unsigned int adr;
} *tabadr = labels;

/*
extern struct
{
	int adr;
	char *name;
} tabadr[];
extern int nadr;
*/

#define DL(xxx) strncpy(labels[nl].name, #xxx, LEN); labels[nl].value = (int)xxx; nl++;
#define DX(xxx) strncpy(labels[nl].name, #xxx, LEN); labels[nl].value = (int)xxx; nl++;
// #define DA(xxx) tabadr[nl].adr = (int)xxx; tabadr[nl].name = #xxx; nl++; 
#define DA(xxx) tabadr[nl].adr = (int)xxx; strcpy(tabadr[nl].name,#xxx); nl++; 

int valext (char *name)
{
	int i;
	for (i=0; i<nl; i++)
	{
		if (!strncmp (labels[i].name, name, LEN))
			return labels[i].value;
	}
	return 0;
}

int valext1 (char *name)
{

		DL(fopen)
		DL(fclose)
		DL(fgetc)
		DL(fputc)
		DL(fprintf)
		DL(divide)
		DL(modulo)
		DL(match)
		DL(fscanf)
		DL(fgets)
		DL(feof)
		DL(sprintf)
		DL(swprintf)
		DL(sscanf)
		DL(sscanf1)
		DL(strcmp)
		DL(strncmp)
		DL(strcpy)
		DL(strncpy)
		DL(strlen)

		DX(scpy)
		DX(scmp)
		DX(slen)

		DL(plus)
		DX(testcall)
		DX(writecarac)
		DX(readcarac)
		DX(getreadword)
		DX(readword)
		DX(&wr)
		DX(&awr)
		DX(findword)
		DX(createword)
		DX(getword)
		DX(fputc1)
		DX(printasm)
		DX(codeasm)
		DX(savecore)
		DX(restorecore)
		DX(savedic)
		DX(restoredic)
		DX(dump)
		DX(malloc)

		DL(doubleofint)
		DL(intofdouble)
		DL(dadd)
		DL(dsub)
		DL(dmult)
		DL(ddiv)
		DL(exp)
		DL(pow)
		DL(log)
		DL(sin)
		DL(cos)
		DL(tan)
		DL(atan)
		DL(asin)
		DL(acos)
		DL(fabs)

		DL(instrcode)
		DL(loadasm1)
		DL(asm)
		DL(codeasm)
		DL(substlabels)
		DL(getline)
		DL(asmem)

		DL(paint)

		DL(forth_main)

		DL(FindWindow)
		DL(SetForegroundWindow)
		DL(GetStockObject)
		DL(RegisterClass)
		DL(CreateWindowEx)
		// DA(CreateWindow)
		DL(GetWindowRect)
		DL(InvalidateRect)
		DL(MoveWindow)
		DL(ShowWindow)
		DL(UpdateWindow)
		DL(GetMessage)
		DL(TranslateMessage)
		DL(DispatchMessage)
		DL(SendMessage)
		DL(memset)
		// DL(SHCreateMenuBar)
		// DL(SHHandleWMActivate)
		// DL(SHHandleWMSettingChange)
		// DL(SHInitDialog)
		DL(EndDialog)
		DL(BeginPaint)
		DL(GetClientRect)
		DL(DrawText)
		DL(EndPaint)
		DL(GetPixel)
		DL(SetPixel)
		// DA(DialogBox)
		DL(DestroyWindow)
		DL(PostQuitMessage)

		DL(LoadIcon)
		DL(LoadString)
		DL(CommandBar_Show)

		DL(testweb)

		DL(wstrcmp)
		DL(exit)


		// DX(time)
		DX(srand)
		DX(rand) 

		// DX(exit)

		DX(GetSystemMetrics)
		DX(SM_CXFULLSCREEN)
		DX(atoi)
		//DX(GetNthCharWstring)
		// DX(MessageBox1)
		// DX(GetCommState1)
		// DX(socket1)
		// DX(&xxx1)
		// DX(&xxx2)

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

		DX(GetModuleHandle)
		// DX(GetModuleInformation)
		// DX(GetModuleFilename)
		DX(LoadLibrary)
		DX(LoadLibraryEx)
		DX(FreeLibrary)
		DX(FreeLibraryAndExitThread)
		DX(GetProcAddress)
		DX(DllMain)
		DX(DisableThreadLibraryCalls)

		DX(GetTickCount)

		return 0;
}

int valext2 (char *name)
{


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

		return 0;
}

int valext3 (char *name)
{

		/*
		DX(hPort)
		DX(hReadThread)
		DX(PortInitialize)
		DX(PortWrite)
		DX(PortReadThread)
		DX(PortClose)
		DX(LireSerie)
		*/

	
	

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
		// DX(WSAGetLastError) 

		DX(inet_addr)

		DX(PF_INET)
		DX(SOCK_STREAM)
		DX(IPPROTO_TCP)
		DX(AF_INET)

		DX(INVALID_SOCKET)

		DX(select)
		/* DX(ioctl) */
// #if 0
		DX(ioctlsocket)
		/* DX(fcntl) */
		/*DX(sleep)*/
		DX(FIONBIO)
		/*
		DX(F_SETFL)
		DX(O_NONBLOCK)
		*/

	

		DX(&ghwnd)
		DX(&ghInstance)
		DX(&ghPrevInstance)
		DX(&glpCmdLine)
		DX(&gnCmdShow)

		// DX(&msgparam)
		// DX(WndProc)


		DX(DefWindowProc)		

		DX(FormatMessage)

		DX(PeekMessage)
		DX(PM_NOREMOVE)
		DX(PM_REMOVE)

		/*
		DX(GetMessage)
		DX(TranslateMessage)
		DX(DispatchMessage)
		DX(SendMessage)
		DX(FindWindow)
		DX(SetForegroundWindow)
		DX(RegisterClass) 
		//  DX(RegisterClassEx)  
		DX(CreateWindow1)
		DX(CreateWindowEx)
		DX(DestroyWindow)
		DX(GetWindowRect)
		DX(MoveWindow)
		DX(ShowWindow)
		DX(UpdateWindow)
		*/
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
// #endif
		DX(DrawText)
#ifdef WM
		DX(SHCreateMenuBar)
		DX(SHHandleWMActivate)
		DX(SHHandleWMSettingChange)
#endif

		// DX(GetStockObject)
             // DX(gnCmdShow)

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

		


	
		return 0;


}

int initext (void)
{
	int x;
	char *name;
	x = valext1 (name);
	if (x)
		return x;
	x = valext2 (name);
	if (x)
		return x;
	x = valext3 (name);
	return x;
}

void trace1 (void)
{
	TRACE "trace1\n" ENDTRACE
}


void trace2 (void)
{
	TRACE "trace2\n" ENDTRACE
}

void trace3 (void)
{
	TRACE "trace3\n" ENDTRACE
}

void trace4 (void)
{
	TRACE "trace4\n" ENDTRACE
}

void trace5 (void)
{
	TRACE "trace5\n" ENDTRACE
}

instr * instr_begin (instr *ptrcode)
{
	*ptrcode++ = 0xE92D4000; // PUSH RD 4000

	ptrcode = compile_call (ptrcode, trace1, 0, 0);
	
	*ptrcode++ = 0xE59F0000; // R0 = ptrstack
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&ptrstack;
	*ptrcode++ = 0xE590B000; // RB = [R0]

	*ptrcode++ = 0xE59F0000; // R0 = ptrpstack
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&ptrpstack;
	*ptrcode++ = 0xE590A000; // RA = [R0]

	ptrcode = compile_call (ptrcode, trace2, 0, 0);

	return ptrcode;
}

void saveregs (int *rb, int *ra)
{
	ptrstack = rb;
	ptrpstack = ra;
}

int codeclearregs[] = { 
	0xE92D4000,
	0xE3A01000,
	0xE3A02000,
	0xE3A03000,
	0xE3A04000,
	0xE3A05000,
	0xE3A06000,
	0xE3A07000,
	0xE3A08000,
	0xE3A09000,
	0xE3A0A000,
	0xE3A0B000,
	0xE3A0C000,
	0xE8BD8000 };

any clearregs;

instr *instr_end (instr *ptrcode)
{
	// int i;
	// for (i=0; i<1000000; i++);
	// dump1 ();
	// handle_message();
	// fflush (outfd);

	FILE *fd1;
	fd1 = fopen ("\\aaa\\bbb\\ccc", "r");

	// clearregs.pi = codeclearregs;
	// (*(clearregs.f))();

	ptrcode = compile_call (ptrcode, trace3, 0, 0);
/*
	*ptrcode++ = 0xE59F0000; // R0 = ptrstack
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&ptrstack;
	// *ptrcode++ = (instr)aps;
	*ptrcode++ = 0xE580B000; // [R0] = RB

	ptrcode = compile_call (ptrcode, trace4, 0, 0);

	*ptrcode++ = 0xE59F0000; // R0 = ptrpstack
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&ptrpstack;
	// *ptrcode++ = (instr)apps;
	*ptrcode++ = 0xE580A000; // [R0] = RA
*/	

	// R0 = RB
	*ptrcode++ = 0xE1A0000B;
	// R1 = RA
	*ptrcode++ = 0xE1A0100A;
	ptrcode = compile_call (ptrcode, saveregs, 0, 0);

	ptrcode = compile_call (ptrcode, trace5, 0, 0);

	*ptrcode++ = 0xE8BD8000; // POP RD 8000
	return ptrcode;
}

instr * code_restorestack (instr *ptrcode)
{
	// *ptrcode++ = 0xE92D4000; // PUSH RD 4000
	
	*ptrcode++ = 0xE59F8000; // R8 = ptrstack
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&ptrstack;
	*ptrcode++ = 0xE598B000; // RB = [R8]

	*ptrcode++ = 0xE59F8000; // R8 = ptrpstack
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&ptrpstack;
	*ptrcode++ = 0xE598A000; // RA = [R8]
	return ptrcode;
}

instr *code_savestack (instr *ptrcode)
{
	*ptrcode++ = 0xE59F8000; // R8 = ptrstack
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&ptrstack;
	*ptrcode++ = 0xE588B000; // [R8] = RB

	*ptrcode++ = 0xE59F8000; // R8 = ptrpstack
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&ptrpstack;
	*ptrcode++ = 0xE598A000; // [R8] = RA

	// *ptrcode++ = 0xE8BD8000; // POP RD 8000
	return ptrcode;
}

instr *compile_init (instr *ptrcode)
{
	*ptrcode++ = 0xE92D4000; // PUSH RD 4000
	
	*ptrcode++ = 0xE59F0000; // R0 = ptrstack
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&ptrstack;
	*ptrcode++ = 0xE590B000; // RB = [R0]

	*ptrcode++ = 0xE59F0000; // R0 = ptrpstack
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&ptrpstack;
	*ptrcode++ = 0xE590A000; // RA = [R0]

	*ptrcode++ = 0xE8BD8000; // POP RD 8000
	return ptrcode;
}

instr *compile_begindef (instr *ptrcode)
{
	*ptrcode++ = 0xE92D4000;
	return ptrcode;
}

instr *compile_enddef (instr *ptrcode)
{
	TRACE "enddef ptrcode=%X\n", ptrcode ENDTRACE
	*ptrcode++ = 0xE8BD8000;
	TRACE "ptrcode=%X\n", ptrcode ENDTRACE
	return ptrcode;
}

instr *compile_pushint (instr *ptrcode, int x)
{
	// TRACE2 "compile_pushint %X\n", x ENDTRACE2
	*ptrcode++ = 0xE59F0000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)x;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *code_pushint (instr *ptrcode, any param, pheader ph)
{
	int x;
	TRACE "code_pushint ptrcode=%X\n", ptrcode ENDTRACE
	x = 0;
	readword();
	sscf (wr, L"%X", &x);
	TRACE "x=%X\n", x ENDTRACE
	ptrcode = compile_pushint (ptrcode, x);
	TRACE "code_pushint return %X\n", ptrcode ENDTRACE
	return ptrcode;
}

instr * code_phcalled (instr *ptrcode)
{
	*ptrcode++ = 0xE59F0000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)phcalled;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *compile_movr0 (instr *ptrcode, int x)
{
	*ptrcode++ = 0xE59F0000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)x;
	return ptrcode;
}

instr *compile_movr1 (instr *ptrcode, int x) 
{
	*ptrcode++ = 0xE59F1000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)x;
	return ptrcode;
}

instr *compile_movr2 (instr *ptrcode, int x)
{
	*ptrcode++ = 0xE59F2000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)x;
	return ptrcode;
}

instr *compile_movr (instr *ptrcode, int n, int x)
{
	*ptrcode++ = 0xE59F0000 + n * 0x1000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)x;
	return ptrcode;
}

instr * code_pushint_param (instr *ptrcode,any p)
{
	*ptrcode++ = 0xE59F0000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)p.i;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr * code_pushintdec (instr *ptrcode)
{
	int x;
	x = 0;
	readword();
	sscf (wr, L"%d", &x);
	*ptrcode++ = 0xE59F0000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)x;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}


instr * code_globals (instr *ptrcode)
{
	*ptrcode++ = 0xE59F0000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&globals;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr * code_putcode (instr *ptrcode)
{
	int x;
	x = 0;
	readword();
	sscf (wr, L"%X", &x);
	*ptrcode++ = (instr)x;
	return ptrcode;
}

instr * code_add (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0003; // POP RB 3
	*ptrcode++ = 0xE0800001; // R0 = R0 + R1
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	return ptrcode;
}

instr * code_overn (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = 0xE79B0100;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *code_stop (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0100;
	*ptrcode++ = 0xE8AA0100;
	return ptrcode;
}

instr *code_nstop (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0200;
	*ptrcode++ = 0xE2299001;
	*ptrcode++ = 0xE1B09009;
	*ptrcode++ = 0x4A000003;
	*ptrcode++ = 0xE8BB0100;
	*ptrcode++ = 0xE8AA0100;
	*ptrcode++ = 0xE2299001;
	*ptrcode++ = 0xEAFFFFF9;
	return ptrcode;
}

instr *code_npdrop (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0200;
	*ptrcode++ = 0xE04AA109;
	return ptrcode;
}

instr *code_np (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = 0xE04A0109;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *code_rp (instr *ptrcode)
{
	*ptrcode++ = 0xE92B0400;
	return ptrcode;
}

/*
int code_get (void)
{
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = 0xE5900000;
	*ptrcode++ = 0xE92B0001;
}

int code_put (void)
{
	*ptrcode++ = 0xE8BB0003;
	*ptrcode++ = 0xE5801000;
}
*/

int print (int x)
{
	// outptr += sprf (outptr, L"%X", x);
	carac buf[100];
	TRACE "print %X\n", x ENDTRACE
	sprf (buf, L" %X ", x);
	TRACE "before writestring\n" ENDTRACE
	writestring (buf);
	TRACE "after writestring\n" ENDTRACE
}

int printdec (int x)
{
	carac buf[100];
	sprf (buf, L" %d ", x);
	TRACE "printdec before writestring\n" ENDTRACE
	writestring (buf);
	TRACE "printdec after writestring\n" ENDTRACE
}

int printastring (char *s)
{
	while(*s)
		writecarac(*s++);
}

int printustring (carac *s)
{
	/* while (*s)
		writecarac(*s++);
	*/
	 writestring(s);
}

/*
int printchar (int x)
{
	// writecarac (x);
	carac buf[100];
	sprf (buf, L"%c", x);
	writestring(buf);
	
}
*/

int printchar (int x)
{
	writecarac (x);
}

instr instr_call (instr *ptrcode, int (*f)())
{
	union
	{
		int (*f)();
		int i;
	} uf;
	union
	{
		instr *pc;
		int i;
	} upc;
	unsigned int ofs;
	uf.f = f;
	upc.pc = ptrcode;
	ofs = (uf.i-(upc.i+8))/4;
	ofs &= 0xFFFFFF;
	TRACE "instr_call: ptrcode=%X f=%X ofs=%X\n", ptrcode, f, ofs ENDTRACE
	return 0xEB000000 | ofs;
}

#if 0
instr * code_print (instr *ptrcode)
{
	TRACE "code_print ptrcode=%X\n", ptrcode ENDTRACE
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = instr_call (ptrcode, print);
	return ptrcode;
}

instr * code_printdec (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = instr_call (ptrcode, printdec);
	return ptrcode;
}

instr *code_printastring (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = instr_call (ptrcode, printastring);
	return ptrcode;
}

instr *code_printustring (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = instr_call (ptrcode, printustring);
	return ptrcode;
}

instr *code_printchar (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = instr_call (ptrcode, printchar);
	return ptrcode;
}

instr *code_readchar (instr *ptrcode)
{
	*ptrcode++ = instr_call (ptrcode, readcarac);
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}
#endif

instr * compile_call (instr *ptrcode, int (*f)(), int regs, int res)
{
	if (regs)
		*ptrcode++ = 0xE8BB0000 | regs;
	*ptrcode = instr_call (ptrcode, f);
	ptrcode++;
	if (res)
		*ptrcode++ = 0xE92B0000 | res;
	return ptrcode;
}

instr * compile_farcall (instr *ptrcode, int (*f)(), int regs, int res)
{
	if (regs)
		*ptrcode++ = 0xE8BB0000 | regs;
	*ptrcode++ = 0xE28FE004; // RE = RF + 4
	*ptrcode++ = 0xE51FF004; // RF = [RF+4]
	*ptrcode++ = (int)f;
	if (res)
		*ptrcode++ = 0xE92B0000 | res;
	return ptrcode;
}

instr * compile_call_param (instr *ptrcode, int (*f)(), int p, int regs, int res)
{
	if (regs)
		*ptrcode++ = 0xE8BB0000 | regs;
	*ptrcode++ = 0xE59F0000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)p;
	*ptrcode = instr_call (ptrcode, f);
	ptrcode++;
	if (res)
		*ptrcode++ = 0xE92B0000 | res;
	return ptrcode;
}

instr *code_print (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, print, 1, 0);
	return ptrcode;
}

instr *code_printdec (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, printdec, 1, 0);
	return ptrcode;
}

instr *code_printustring (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, printustring, 1, 0);
	return ptrcode;
}

instr *code_printastring (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, printastring, 1, 0);
	return ptrcode;
}

instr *code_printchar (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, printchar, 1, 0);
	return ptrcode;
}

instr *code_readchar (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, readcarac, 0, 1);
	return ptrcode;
}

instr * code_nextword(instr *ptrcode)
{
	pheader ph;
	readword(); 
	ph=getword(wr); 
	ptrcode = compile_pushint (ptrcode, (int)ph);
	return ptrcode;
}

instr * code_compile_pushint (instr *ptrcode) 
{
	pheader ph;
	int n;
	readword();
	sscanf(awr,"%X",&n);
	ptrcode = compile_call_param(ptrcode,compile_pushint,n,0,0);
	return ptrcode;
}

instr * code_pushint_compile(instr *ptrcode) 
{
	// pc = compile_movr0 (pc, pc.i); // a verifier
	ptrcode = compile_call(ptrcode,compile_pushint,0,0);
	return ptrcode;
}
	
/*
instr *code_extval (instr *ptrcode)
{
	int x;
	readword();
	x = valext (awr);
	ptrcode = compile_pushint (ptrcode, x);
	return ptrcode;
}
*/

instr * code_call (instr *ptrcode)
{
	int (*f)();
	int i;
	any u;
	int regs;
	int x;
	readword ();
	u.i = valext (awr);
	readword();
	sscf(wr,L"%X",&x);
	regs = (1 << x) - 1; 
	ptrcode = compile_call (ptrcode, u.f, regs, 1);
	return ptrcode;
}

instr *code_callmodule (instr *ptrcode)
{
	int (*f)();
	int i;
	any u;
	int regs;
	int x;
	carac modulename[100], functionname[100];
	HINSTANCE module;
	wchar_t buf[1000];

	readword ();
	// MessageBox (NULL, wr, L"wr", MB_OK);
	scpy (modulename, wr);
	// MessageBox (NULL, modulename, L"modulename", MB_OK);

	readword ();
	scpy (functionname, wr);
	// MessageBox (NULL, functionname, L"functionname", MB_OK);

	readword();
	// MessageBox (NULL, wr, "x", MB_OK);
	sscf(wr,L"%X",&x);
	regs = (1 << x) - 1; 

	module = GetModuleHandle (modulename);
	// swprintf (buf, L"GetModuleHandle -> %X", module);
	// MessageBox (NULL, buf, L"message", MB_OK);

	if (module != NULL)
	{
		f = GetProcAddress (module, functionname);
		// swprintf (buf, L"GetModuleHandle -> f=%X", f);
		// MessageBox (NULL, buf, L"message", MB_OK);
	}
	else
	{
		module = LoadLibrary (modulename);
		// swprintf (buf, L"LoadLibrary -> %X", module);
		// MessageBox (NULL, buf, L"message", MB_OK);

		if (module != NULL)
		{
			f = GetProcAddress (module, functionname);
			// swprintf (buf, L"LoadLibrary -> f = %X", f);
			// MessageBox (NULL, buf, L"message", MB_OK);
		}
		else
		{
			f = NULL;
			// MessageBox (NULL, L"f = NULL", L"message", MB_OK);
		}
	}
	if (f != NULL)
	{
		// MessageBox (NULL, L"f != NULL", L"message", MB_OK);
		ptrcode = compile_farcall (ptrcode, f, regs, 1);
	}
	return ptrcode;
}

instr * code_dcall (instr *ptrcode)
{
	int (*f)();
	int i;
	any u;
	int regs;
	int x;
	readword ();
	u.i = valext (awr);
	readword();
	sscf(wr,L"%X",&x);
	regs = (1 << x) - 1; 
	ptrcode = compile_call (ptrcode,u.f, regs, 3);
	return ptrcode;
}

instr *code_calladr (instr *ptrcode)
{
	/* POP RB 80 */	*ptrcode++ = 0xE8BB0080;
	/* RE = RF 	 */	*ptrcode++ = 0xE1A0E00F;
	/* RF = R7	 */ *ptrcode++ = 0xE1A0F007; 	
	return ptrcode;
}

instr *code_extval (instr *ptrcode)
{
	int x;
	readword();
	x = valext(awr);
	/* push x */
	*ptrcode++ = 0xE59F0000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)x;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *code_add (instr *ptrcode);
/*
int traceregs (int R0, int R1)
{
	carac buf[1000];
	sprf (buf, L"R0=%X R1=%X code_add=%X", R0, R1, code_add);
	MessageBox (ghwnd, buf, L"traceregs", MB_OK);
}

int code_add (void)
{
	*ptrcode++ = 0xE8BB0003; // POP RB 3
	// *ptrcode++ = instr_call (traceregs);
	*ptrcode++ = 0xE0800001; // R0 = R0 + R1
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
}
*/
instr * code_doit (instr *ptrcode)
{
	ptrcode = compile_enddef(ptrcode);
	TRACE "call code at %X\n", startcode.f ENDTRACE
	(*(startcode.f)) ();
	TRACE "returned\n" ENDTRACE
	ptrcode = startcode.pcode;
	ptrcode = compile_begindef(ptrcode);
	return ptrcode;
}

instr * code_levelup (instr *ptrcode)
{
	// ptrcodes[level] = ptrcode;
	level++;
	startcode.pcode = codes[level];
	ptrcode = codes[level];
	// instr_begin();
	ptrcode = compile_begindef(ptrcode);
	return ptrcode;
}

instr * code_leveldown (instr *ptrcode)
{
	// instr_end();
	ptrcode = compile_enddef(ptrcode);
	(*(startcode.f)) ();
	level--;
	// ptrcode = ptrcodes[level];
	// startcode.pcode = codes[level];
	return ptrcode;
}

instr *code_modedown1(instr *ptrcode)
{
	level--;
	mode--;
	return ptrcode;
}

instr * code_modeup1 (instr *ptrcode)
{
	level++;
	startcode.pcode = codes[level];
	ptrcode = codes[level];
	ptrcode = compile_begindef(ptrcode);
	mode++;
	return ptrcode;
}

instr *code_beginim (instr *ptrcode)
{
	mode = MODEIM;
	return ptrcode;
}

instr *code_endim (instr *ptrcode)
{
	mode = MODECP;
	return ptrcode;
}

instr *code_modeup (instr *ptrcode)
{
	mode++;
	return ptrcode;
}

instr *code_modedown (instr *ptrcode)
{
	mode--;
	return ptrcode;
}

instr *code_setmode (instr *ptrcode)
{
	pheader ph;
	int n;
	readword(); 
	ph=getword (wr);
	sscanf(awr,"%X",&n);
	mode=n;
	return ptrcode;
}

instr * code_getmode(instr *ptrcode)
{
	ptrcode = compile_pushint (ptrcode, mode);
	return ptrcode;
}

/*
int code_def (void)
{
	carac name[WORDSIZE];
	int i;
	pheader ph;
	instr *pcsrc, *pcdst;
	readword();
	ph = getword (wr);
	pcsrc = startcode.pcode;
	pcdst = freespace.pcode;
	for (;;)
	{
		if (pcsrc >= ptrcode)
			break;
		*pcdst++ = *pcsrc++;
	}
	freespace.pcode = pcdst;
	ph->value.pcode = startcode.pcode;
	ptrcode = startcode.pcode;
	instr_begin();
}
*/

pheader phdef;

/*
int calldef (any param, pheader ph)
{
	if (ph)
	{
		*ptrcode++ = 0xE59F0000;
		*ptrcode++ = 0xEA000000;
		*ptrcode++ = (instr)ph;
	}
	compile_call (param.f, 0, 0);
}
*/

// a verifier
instr *calldef (instr *ptrcode, any param, pheader ph)
{
	/*
	*ptrcode++ = 0xE59F0000; // R0 = ptrcode
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr) ptrcode;
	*ptrcode++ = 0xE59F1000; // R1 = param
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = param.instruc;
	*ptrcode++ = 0xE59F2000; // R2 = ph
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr) ph;
	*/
	/*
	if (ph)
	{
		*ptrcode++ = 0xE59F0000;
		*ptrcode++ = 0xEA000000;
		*ptrcode++ = (instr)ph;
	}
	*/
	ptrcode = compile_call (ptrcode, param.f, 0, 0);
	return ptrcode;
}

instr * code_def (instr *ptrcode) 
{
	readword(); 
	phdef = getword(wr);
	phdef->value.f = calldef;
	// phdef->imvalue.f = callim;
	phdef->param = freespace;
	mode = MODECP;
	// level++;
	// mode++;
	// startcode = freespace; 
	// ptrcode = freespace.pcode;
	// instr_begin ();
	freespace.pcode = compile_begindef(freespace.pcode);
	return ptrcode;
}

instr * code_defrawcode (instr *ptrcode) 
{
	readword(); 
	phdef = getword(wr);
	phdef->value.f = calldef;
	// phdef->imvalue.f = callim;
	phdef->param = freespace;
	mode = MODECP;
	// level++;
	// mode++;
	// startcode = freespace; 
	// ptrcode = freespace.pcode;
	// instr_begin ();
	// freespace.pcode = compile_begindef(freespace.pcode);
	return ptrcode;
}

instr * code_defmacro (instr *ptrcode)
{
	readword();
	phdef = getword (wr);
	phdef->value = freespace;
	// phdef->imvalue = freespace; // a verifier
	mode = MODECP;
	// level++;
	// mode++;
	// startcode = freespace;
	// ptrcode = freespace.pcode;
	freespace.pcode = compile_begindef(freespace.pcode);
	return ptrcode;
}

instr * code_defrawcodemacro (instr *ptrcode)
{
	readword();
	phdef = getword (wr);
	phdef->value = freespace;
	// phdef->imvalue = freespace; // a verifier
	mode = MODECP;
	// level++;
	// mode++;
	// startcode = freespace;
	// ptrcode = freespace.pcode;
	// freespace.pcode = compile_begindef(freespace.pcode);
	return ptrcode;
}

instr * code_enddef2 (instr *ptrcode)
{
	// instr_end ();
	/**/ // MessageBox (NULL, L"code_enddef", L"message", MB_OK);
	if (flag_locals)
	{
		
		*ptrcode++ = 0xE8BB0001; // POP RB 0001
		*ptrcode++ = 0xE1A0B009; // RB = R9
		*ptrcode++ = 0xE92B0001; // PUSH RB 0001
		/*
		*ptrcode++ = 0xE28BB000 + 4 * flag_locals; // RB = RB + 4*flag_locals
		*ptrcode++ = 0xE2499004;				   // R9 = R9 - 4
		*ptrcode++ = 0xE049800B;				   // R8 = R9 - RB
		*ptrcode++ = 0xE1B08008;				   // R8 ?= R8
		*ptrcode++ = 0x4A000002;				   // <0 GOTO +2
		*ptrcode++ = 0xE5198000 + 4 * flag_locals; // R8 = [R9-4*flag_locals]
		*ptrcode++ = 0xE5898000;				   // [R9] = R8
		*ptrcode++ = 0xEAFFFFF8;				   // GOTO -8
		*/
	}
	flag_locals = 0;
	ptrcode = compile_enddef(ptrcode);
	mode = MODEIM;
	// phdef->value.f = calldef;
	// phdef->param = startcode;
	// freespace.pcode = ptrcode;
	// level--;
	// mode--;
	return ptrcode;
}


instr * code_enddef (instr *ptrcode)
{
	// instr_end ();
	/**/ // MessageBox (NULL, L"code_enddef", L"message", MB_OK);
	if (flag_locals)
	{
		/*
		*ptrcode++ = 0xE8BB0001; // POP RB 0001
		*ptrcode++ = 0xE1A0B009; // RB = R9
		*ptrcode++ = 0xE92B0001; // PUSH RB 0001
		*/
		*ptrcode++ = 0xE28BB000 + 4 * flag_locals; // RB = RB + 4*flag_locals
		*ptrcode++ = 0xE2499004;				   // R9 = R9 - 4
		*ptrcode++ = 0xE049000B;				   // R0 = R9 - RB
		*ptrcode++ = 0xE1B00000;				   // R0 ?= R0
		*ptrcode++ = 0x4A000002;				   // <0 GOTO +2
		*ptrcode++ = 0xE5190000 + 4 * flag_locals; // R0 = [R9-4*flag_locals]
		*ptrcode++ = 0xE5890000;				   // [R9] = R0
		*ptrcode++ = 0xEAFFFFF8;				   // GOTO -8
		
	}
	flag_locals = 0;
	ptrcode = compile_enddef(ptrcode);
	mode = MODEIM;
	// phdef->value.f = calldef;
	// phdef->param = startcode;
	// freespace.pcode = ptrcode;
	// level--;
	// mode--;
	return ptrcode;
}
instr * code_endrawcode (instr *ptrcode)
{
	// instr_end ();
	// ptrcode = compile_enddef(ptrcode);
	mode = MODEIM;
	// phdef->value.f = calldef;
	// phdef->param = startcode;
	// freespace.pcode = ptrcode;
	// level--;
	// mode--;
	return ptrcode;
}


instr *callim (instr *ptrcode, any param, pheader ph)
{
	gptrcode = ptrcode;
	gparam = param;
	gph = ph;
	ptrcode = (*(param.f)) (ptrcode, param, ph);
	return gptrcode;
}

int levellambda = -1;
instr *adrblambda[0x100];

instr *code_lambda (instr *ptrcode)
{
	levellambda++;
	*ptrcode++ = 0xE28F0004; // R0 = RF + 4
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	adrblambda[levellambda] = ptrcode;
	*ptrcode++ = 0xEA000000;
	ptrcode = compile_begindef (ptrcode);
	return ptrcode;
}

instr code_endlambda (instr *ptrcode)
{
	int ofs;
	ptrcode = compile_enddef (ptrcode);
	ofs = ptrcode - (adrblambda[levellambda]+2);
	*(adrblambda[levellambda]) |= ofs;
	levellambda--;
	return ptrcode;
}

/*
int code_lambda (void)
{
	level++;
	mode++;
	startcode = freespace;
	ptrcode = freespace.pcode;
	compile_begindef();
}

int code_endlambda (void)
{
	any start;
	compile_enddef();
	freespace.pcode = ptrcode;
	start = startcode;
	level--;
	mode--;

	*ptrcode++ = 0xE59F0000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)(start.i);
	*ptrcode++ = 0xE92B0001;
}
*/


instr * code_endlambdacompile (instr *ptrcode)
{
	any start;
	ptrcode = compile_enddef(ptrcode);
	freespace.pcode = ptrcode;
	start = startcode;
	level--;
	ptrcode = compile_call (ptrcode,start.f,0,0);
	return ptrcode;
}

int nargs; 
int nlocals;

#define MAXARGLEVELS 256
int nargstack[MAXARGLEVELS];
pheader argstack[MAXARGLEVELS];
int arglevel = 0;
int argstackptr = 0;

instr * compile_getcargp (instr *ptrcode, any p)
{
	*ptrcode++ = 0xE51A0000 + 4 * p.i;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *compile_getvargp (instr *ptrcode, any p)
{
	*ptrcode++ = 0xE24A0000 + 4 * p.i;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *compile_getvarg (instr *ptrcode, any p)
{
	*ptrcode++ = 0xE2490000 + 4 * p.i;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *compile_getcarg (instr *ptrcode, any p)
{
	*ptrcode++ = 0xE5190000 + 4 * p.i;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr * compile_getargstack (instr *ptrcode, any p)
{
	int i;
	for (i=1; argstackptr-i>=0; i++)
	{
		if (argstack[argstackptr-i] == p.ph)
		{
			*ptrcode++ = 0xE24A0000 + 4 * i;
			*ptrcode++ = 0xE92B0001;
			return ptrcode;
		}
	}
	ptrcode = compile_pushint (ptrcode, 0);
	return ptrcode;
}

instr * code_define (instr *ptrcode)
{
	pheader ph;
	int i;
	
	readword(); 
	phdef = getword(wr);
	phdef->value.f = calldef;
	// phdef->imvalue.f = callim;
	phdef->param = freespace;
	level++;
	mode--;
	startcode = freespace; 
	ptrcode = freespace.pcode;
	// instr_begin ();
	ptrcode = compile_begindef(ptrcode);

	for (nargs=0; ; nargs++)
	{
		readword();
		if (!strcmp(awr,":"))
			break;
		ph = getword(wr);
		ph->value.f = compile_getcargp;
		// ph->imvalue.f = compile_getcargp; // a verifier
		ph->param.i = nargs+1;
		// compile_call (code_stop, 0, 0);
		ptrcode = code_stop(ptrcode);
	}
	return ptrcode;
}

instr * code_cargsp (instr *ptrcode)
{
	pheader ph;
	int i;
	
	for (nargs=0; ; nargs++)
	{
		readword();
		if (!strcmp(awr,":"))
			break;
		ph = getword(wr);
		ph->value.f = compile_getcargp;
		// ph->imvalue.f = compile_getcargp; // a verifier
		ph->param.i = nargs+1;
		// compile_call (code_stop, 0, 0);
	}

	readword(); 
	phdef = getword(wr);
	phdef->value.f = calldef;
	phdef->param = freespace;
	// level++;
	// mode++;
	// startcode = freespace; 
	// ptrcode = freespace.pcode;
	// ptrcode = compile_begindef(ptrcode);
	mode = MODECP;
	freespace.pcode = compile_begindef (freespace.pcode);

	for (i=0; i<nargs; i++)
		// compile_call (code_stop, 0, 0);
		// ptrcode = code_stop(ptrcode);
		freespace.pcode = code_stop(freespace.pcode);
	return ptrcode;
}

instr * code_vargsp (instr *ptrcode)
{
	pheader ph;
	int i;
	
	for (nargs=0; ; nargs++)
	{
		readword();
		if (!strcmp(awr,":"))
			break;
		ph = getword(wr);
		ph->value.f = compile_getvargp;
		ph->param.i = nargs+1;
		// compile_call (code_stop, 0, 0);
	}

	readword(); 
	phdef = getword(wr);
	phdef->value.f = calldef;
	phdef->param = freespace;
	// level++;
	// mode++;
	// startcode = freespace; 
	// ptrcode = freespace.pcode;
	mode = MODECP;
	freespace.pcode = compile_begindef(freespace.pcode);

	for (i=0; i<nargs; i++)
		// compile_call (code_stop, 0, 0);
		freespace.pcode = code_stop(freespace.pcode);
	return ptrcode;
}

instr *code_vargs (instr *ptrcode)
{
	pheader ph;
	int i;

	nlocals = 0;
	for (nargs=0; ; nargs++)
	{
		readword();
		if (!strcmp(awr,":") || !strcmp (awr, "LOCAL")) break;
		ph = getword(wr);
		ph->value.f = compile_getvarg;
		ph->param.i = nargs+1;
	}

	if (!strcmp (awr, "LOCAL"))
	{
		for (nlocals=0; ; nlocals++)
		{
			readword();
			if (!strcmp(awr,":")) break;
			ph = getword(wr);
			ph->value.f = compile_getvarg;
			ph->param.i = nargs + nlocals + 1;
		}
	}

	readword();
	phdef = getword(wr);
	phdef->value.f = calldef;
	phdef->param = freespace;
	mode = MODECP;
	freespace.pcode = compile_begindef (freespace.pcode);
	*freespace.pcode++ = 0xE28B9000 + 4 * nargs; // R9 = RB + 4*nargs
	if (nlocals > 0)
		*freespace.pcode++ = 0xE24BB000 + 4 * nlocals; // RB = RB - 4 * nlocals
	return ptrcode;
}

instr *code_cargs (instr *ptrcode)
{
	pheader ph;
	int i;

	nlocals = 0;
	for (nargs=0; ; nargs++)
	{
		readword();
		if (!strcmp(awr,":") || !strcmp (awr, "LOCAL")) break;
		ph = getword(wr);
		ph->value.f = compile_getcarg;
		ph->param.i = nargs+1;
	}

	if (!strcmp (awr, "LOCAL"))
	{
		for (nlocals=0; ; nlocals++)
		{
			readword();
			if (!strcmp(awr,":")) break;
			ph = getword(wr);
			ph->value.f = compile_getvarg;
			ph->param.i = nargs + nlocals + 1;
		}
	}

	readword();
	phdef = getword(wr);
	phdef->value.f = calldef;
	phdef->param = freespace;
	mode = MODECP;
	freespace.pcode = compile_begindef (freespace.pcode);
	*freespace.pcode++ = 0xE28B9000 + 4 * nargs; // R9 = RB + 4*nargs
	if (nlocals > 0)
		*freespace.pcode++ = 0xE24BB000 + 4 * nlocals; // RB = RB - 4 * nlocals
	return ptrcode;
}

/*
instr *code_vargs (instr *ptrcode)
{
	pheader ph[0x100];
	int i;

	for (nargs=0; ; nargs++)
	{
		readword();
		if (!strcmp(awr,":")) break;
		ph[nargs] = getword(wr);
		ph->value.f = compile_getvarg;
	}

	for (i=0; i<nargs; i++)
		ph[i]->param.i = nargs-i;

	readword();
	phdef = getword(wr);
	phdef->value.f = calldef;
	phdef->param = freespace;
	freespace.pcode = compile_begindef (freespace.pcode);
	return ptrcode;
}

instr *code_cargs (instr *ptrcode)
{
	pheader ph[0x100];
	int i;

	for (nargs=0; ; nargs++)
	{
		readword();
		if (!strcmp(awr,":")) break;
		ph[nargs] = getword(wr);
		ph->value.f = compile_getcarg;
	}

	for (i=0; i<nargs; i++)
		ph[i]->param.i = nargs-i;

	readword();
	phdef = getword(wr);
	phdef->value.f = calldef;
	phdef->param = freespace;
	freespace.ptrcode = compile_begindef (freespace.ptrcode);
	return ptrcode;
}
*/

/*
int code_local (void)
{
	pheader ph;

	for (nlocals=0; ; nlocals++)
	{
		readword();
		if (!strcmp(awr,":"))
			break;
		ph = getword(wr);
		ph->value.f = compile_getvarg;
		ph->param.i = nlocals+1;

	}

}
*/

/*
int code_beginargs (void)
{
	pheader ph;
	for (nargs=0; ; nargs++)
	{
		readword();
		if (!strcmp(awr,"))"))
			break;
		ph = getword(wr);
		ph->value.f = compile_getarg;
		ph->param.i = nargs+1;
		compile_call (code_stop, 0, 0);
	}
}
*/

instr * code_beginvargs1 (instr *ptrcode)
{
	pheader ph;
	arglevel++;
	for (nargstack[arglevel]=0; ; nargstack[arglevel]++)
	{
		readword();
		if (!strcmp(awr,"->"))
			break;
		ph = getword(wr);
		argstack[argstackptr] = ph;
		argstackptr++;
		ph->value.f = compile_getargstack;
		ph->param.ph = ph;
		// compile_call (code_stop, 0, 0);
		ptrcode = code_stop (ptrcode);
	}
	return ptrcode;
}

instr * code_endvargs1 (instr *ptrcode)
{
	*ptrcode++ = 0xE24AA000 + 4 * nargstack[arglevel];
	argstackptr -= nargstack[arglevel];
	arglevel--;
	return ptrcode;
}

/*
instr *code_beginvargs (instr *ptrcode)
{
	pheader ph;
	arglevel++;
	for (nargstack[arglevel]=0; ; nargstack[arglevel]++)
	{
		readword();
		if (!strcmp (awr, "->")) break;
		ph = getword (wr);
		ph->value.f = compile_getvarg;
		ph->param.i = nargs+1;
	}

	return ptrcode;
}

instr *code_endvargs (instr *ptrcode)
{

	return ptrcode;
}
*/

instr *code_endp (instr *ptrcode)
{
	*ptrcode++ = 0xE24AA000 + 4 * nargs;
	return ptrcode;
}

instr *code_end (instr *ptrcode)
{
	*ptrcode++ = 0xE1A0B009; // RB = R9
	return ptrcode;
}

instr *code_sap (instr *ptrcode)
{
	*ptrcode++ = 0xE1A0900B;
	nargs = 0;
	return ptrcode;
}

instr *code_beginargs (instr *ptrcode)
{
	nargs = 0;
	return ptrcode;
}

instr *code_endargs (instr *ptrcode)
{
	// R9 = RB + nargs 
	*ptrcode++ = 0xE28B9000 + 4 * nargs;
	return ptrcode;
}

instr *code_varg (instr *ptrcode)
{
	pheader ph;
	nargs++;
	readword();
	ph = getword(wr);
	ph->value.f = compile_getvarg;
	ph->param.i = nargs;
	return ptrcode;
}

instr *code_carg (instr *ptrcode)
{
	pheader ph;
	nargs++;
	readword();
	ph = getword(wr);
	ph->value.f = compile_getcarg;
	ph->param.i = nargs;
	return ptrcode;
}

instr *code_begin_locals (instr *ptrcode)
{
	pheader ph;
	int i;

	nlocals = 0;
	for (nargs=0; ; nargs++)
	{
		readword();
		/* if (!strcmp(awr,":") || !strcmp (awr, "LOCAL")) break; */
		if (!strcmp (awr, "\\") 
		 || !strcmp (awr, "--") 
		 || !strcmp (awr, "}")) break;
		ph = getword(wr);
		ph->value.f = compile_getcarg;
		ph->param.i = nargs+1;
	}

	/* if (!strcmp (awr, "LOCAL")) */
	if (!strcmp (awr, "\\"))
	{
		for (nlocals=0; ; nlocals++)
		{
			readword();
			/* if (!strcmp(awr,":")) break; */
			if (!strcmp (awr, "--") || !strcmp (awr, "}")) break; 
			ph = getword(wr);
			ph->value.f = compile_getcarg;
			ph->param.i = nargs + nlocals + 1;
		}
	}

	if (!strcmp (awr, "--"))
	{
		for (;;)
		{
			readword();
			if (!strcmp(awr,"}")) break;
		}
	}

	// readword();

	/*
	phdef = getword(wr);
	phdef->value.f = calldef;
	phdef->param = freespace;
	mode = MODECP;
	freespace.pcode = compile_begindef (freespace.pcode);
    *freespace.pcode++ = 0xE28B9000 + 4 * nargs; // R9 = RB + 4*nargs
    if (nlocals > 0)
		*freespace.pcode++ = 0xE24BB000 + 4 * nlocals; // RB = RB - 4 * nlocals
	*/

	*ptrcode++ = 0xE28B9000 + 4 * nargs; // R9 = RB + 4*nargs
    if (nlocals > 0)
		*ptrcode++ = 0xE24BB000 + 4 * nlocals; // RB = RB - 4 * nlocals

	flag_locals = nargs + nlocals; 

	return ptrcode;
}

/*
instr *code_end_locals (instr *ptrcode)
{

	return ptrcode;
}

instr *code_begin_uninit (instr *ptrcode)
{

	return ptrcode;
}
*/

instr *code_adr_local (instr *ptrcode)
{
	pheader ph;
	readword();
	ph = getword(wr);
	*ptrcode++ = 0xE2490000 + 4 * ph->param.i; // R0 = R9 - 4 * ph->param.i
    *ptrcode++ = 0xE92B0001; // PUSH RB 1
	return ptrcode;
}

instr *code_store_local (instr *ptrcode)
{
    pheader ph;
	readword();
	ph = getword(wr);
	*ptrcode++ = 0xE2490000 + 4 * ph->param.i; // RO = R9 - 4 * ph->param.i
	*ptrcode++ = 0xE8BB0002; // POP RB 2
	*ptrcode++ = 0xE5801000; // [R0] = R1 
	return ptrcode;
}

instr *code_wmodeim (instr *ptrcode)
{
	pheader ph;
	ph = lastword;
	ph->wmode = MODEIM;
	return ptrcode;
}

instr *code_immediate (instr *ptrcode)
{
	pheader ph;
	ph = lastword;
	// ph->value = ph->param;
	// ph->value = callim;
	ph->wmode = MODEIM | MODEIM1;
	return ptrcode;
}

instr *code_wmode (instr *ptrcode)
{
	readword();
	lastword->wmode = atoi (awr);
	return ptrcode;
}

instr *code_execomp (instr *ptrcode)
{
	/* POP RB 2 	*/	*ptrcode++ = 0xE8BB0002;
	/* R2=[R1+4] 	*/	*ptrcode++ = 0xE5912004;
	/* R0=[R1+8]	*/	*ptrcode++ = 0xE5910008;
	/* RE=RF 	*/	*ptrcode++ = 0xE1A0E00F;
	/* RF=R2 	*/	*ptrcode++ = 0xE1A0F002;
	return ptrcode;
}

instr *code_execute(instr *ptrcode)
{
	/* POP RB 1 	*/	*ptrcode++ = 0xE8BB0001;
	/* R1=[R0+8] 	*/	*ptrcode++ = 0xE5901008;
	/* RE=RF 	*/	*ptrcode++ = 0xE1A0E00F;
	/* RF=R1 	*/	*ptrcode++ = 0xE1A0F001;
	return ptrcode;
}

/*
int code_enddef (void) 
{
	any startcode1;
	instr_end();
	startcode1.pcode = ptrcode;
	instr_begin();
	compile_call(startcode.f,0);
	instr_end();
	phdef->value.f=startcode.f;
	freespace.pcode = ptrcode;
	level--;
}
*/


instr * code_endmacro (instr *ptrcode)
{
	// instr_end();
	ptrcode = compile_enddef(ptrcode);
	phdef->value.f=startcode.f;
	freespace.pcode=ptrcode;
	level--;
	return ptrcode;
}


instr *code_loadrw (instr *ptrcode)
{
	FILE *f;
	readword();
	f = fopen (awr, "r");
	if (f == NULL)
		return 0;
	inlevel++;
	instack[inlevel].f = readcaracfile;
	instack[inlevel].p.fd = f;
	return ptrcode;
}

int load (char *filename)
{
 FILE *f;
 f=fopen(filename,"r");
 if(f==NULL) return 0;
 inlevel++;
 instack[inlevel].f = readcaracfile;
 instack[inlevel].p.fd = f;
}

int loaddesc (FILE *f)
{
 if(f==NULL) return 0;
 inlevel++;
 instack[inlevel].f = readcaracfile;
 instack[inlevel].p.fd = f;
}

instr *code_load (instr *ptrcode)
{
 ptrcode=compile_call(ptrcode,load,1,0);
 return ptrcode;
}

instr *code_loaddesc (instr *ptrcode)
{
 ptrcode=compile_call(ptrcode,loaddesc,1,0);
 return ptrcode;
}

int doastring (char *s)
{
	inlevel++;
	instack[inlevel].f = readcharmem;
	instack[inlevel].p.pchar = s;
}

int doustring (carac *s)
{
	// MessageBox (0, s, L"doustring", MB_OK);
	inlevel++;
	instack[inlevel].f = readcaracmem;
	instack[inlevel].p.pcarac = s;
}

instr *code_doastring (instr *ptrcode)
{
	TRACE "code_doastring ptrcode=%X\n", ptrcode ENDTRACE
	// *ptrcode++ = 0xE8BB0001; // POP RB 1
	TRACE "compile_call ptrcode=%X\n", ptrcode ENDTRACE
	ptrcode = compile_call (ptrcode, doastring, 1, 0);
	TRACE "return %X\n", ptrcode ENDTRACE
	return ptrcode;
}

instr *code_doustring (instr *ptrcode)
{
	// *ptrcode++ = 0xE8BB0001; // POP RB 1
	ptrcode = compile_call (ptrcode, doustring, 1, 0);
	return ptrcode;
}

instr *code_mainloop (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, mainloop, 3, 0);
	return ptrcode;
}

int outstd(void)
{
 	out.f = writecaracfile;
	out.p.fd = outfd;
}

int outfile(char *filename)
{
 FILE *f;
 f=fopen (filename,"wb");
 out.f=writecaracfile;
 out.p.fd=f;
}

int outfiledesc (FILE *fd)
{
	out.f = writecaracfile;
	out.p.fd = fd;
}

int closeout()
{
 fclose(out.p.fd);
}

instr *code_outstd(instr *ptrcode)
{
 ptrcode = compile_call(ptrcode,outstd,0,0);
 return ptrcode;
}

int *code_outfile(instr *ptrcode)
{ 
 ptrcode = compile_call(ptrcode,outfile,1,0);
 return ptrcode;
}

int *code_outfiledesc (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, outfiledesc, 1, 0);
	return ptrcode;
}

int *code_closeout(instr *ptrcode)
{
 ptrcode=compile_call(ptrcode,closeout,0,0);
 return ptrcode;
}

int outastring (char *s)
{
 out.f=writecharmem;
 out.p.pchar=s;
}

int outustring(carac *s)
{ 
 out.f=writecaracmem;
 out.p.pcarac=s;
}

instr *code_outastring (instr *ptrcode)
{
 ptrcode=compile_call(ptrcode,outastring,1,0);
 return ptrcode;
}

instr *code_outustring (instr *ptrcode)
{
 ptrcode=compile_call(ptrcode,outustring,1,0);
 return ptrcode;
}

/*
: interpa ( instr outstr -- )
 OUTASTRING 
 DOASTRING
 OUTSTD ;
*/


instr *code_loadtick (instr *ptrcode)
{
	FILE *f;
	readword();
	f = fopen (awr, "r");
	if (f == NULL)
		return 0;
	inlevel++;
	instack[inlevel].f = readcaracfile;
	instack[inlevel].p.fd = f;
	return ptrcode;
}

int loadfile (char *filename)
{
 FILE *f;
 f=fopen(filename,"r");
 if(f==NULL) return 0;
 inlevel++;
 instack[inlevel].f = readcaracfile;
 instack[inlevel].p.fd = f;
}

instr *code_loadfile (instr *ptrcode)
{
 ptrcode=compile_call(ptrcode,loadfile,1,0);
 return ptrcode;
}

/*
int code_load (void)
{
	FILE *f;
	char cname[WORDSIZE];
	carac name[WORDSIZE];
	char cline[1000];
	carac line[1000];
	int i;
	readword();
	f = fopen (wr, "r");
	for (;;)
	{
		fgets (cline, sizeof(cline), f);
		if (feof(f))
			break;
		for (i=0; cline[i]; i++)
			line[i] = cline[i];
		line[i] = 0;
		forth (line, outptr);
	}

}
*/

instr *code_tick (instr *ptrcode)
{
	pheader ph;
	readword(); 
	ph = getword(wr);
	*ptrcode++ = 0xE59F0000; // R0=[RF]
	*ptrcode++ = 0xEA000000; // GOTO RF
	*ptrcode++ = (instr)ph;     
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	return ptrcode;
}

instr * code_compiletick (instr *ptrcode)
{
	ptrcode = compile_call(ptrcode,getreadword,0,0);
	*ptrcode++ = 0xE92B0001; 
	return ptrcode;
}

/*
int code_imcompile (void) 
{
	pheader ph;
	readword();
	ph=getword(wr); 
	instr_call(ph->value.f,0);
}
*/

instr *code_mode (instr *ptrcode)
{
	*ptrcode++ = 0xE59F0000; // RO=[RF]
	*ptrcode++ = 0xEA000000; // GOTO RF
	*ptrcode++ = (instr)&mode; 
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	return ptrcode;
}

instr *code_dp (instr *ptrcode)
{
	*ptrcode++ = 0xE59F0000; // RO=[RF]
	*ptrcode++ = 0xEA000000; // GOTO RF
	*ptrcode++ = (instr)&freespace; // ptrcode
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	return ptrcode;
}

instr *code_freedic (instr *ptrcode)
{
	*ptrcode++ = 0xE59F0000; // RO=[RF]
	*ptrcode++ = 0xEA000000; // GOTO RF
	*ptrcode++ = (instr)&freedic;
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	return ptrcode;
}

instr *code_freespace (instr *ptrcode)
{
	*ptrcode++ = 0xE59F0000; // RO=[RF]
	*ptrcode++ = 0xEA000000; // GOTO RF
	*ptrcode++ = (instr)&freespace;
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	return ptrcode;
}

instr *code_lastword (instr *ptrcode)
{
	*ptrcode++ = 0xE59F0000; // RO = [RF]
	*ptrcode++ = 0xEA000000; // GOTO RF
	*ptrcode++ = (instr)&lastword;
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	return ptrcode;
}

instr *code_compile1(instr *ptrcode) // a verifier
{
	pheader ph;
	readword(); 
	ph=getword(wr); 
	// compile_movr0 (ph->param.i);
	// compile_movr1 ((int)ph->ph);
	ptrcode = compile_movr1 (ptrcode, ph->param.i); 
	ptrcode = compile_movr2 (ptrcode, (int)ph); // ph->ph
	ptrcode = compile_call(ptrcode,ph->value.f,0,0);
	return ptrcode;
}

instr *code_compile (instr *ptrcode)
{
	pheader ph;
	readword();
	ph = getword (wr);
	ptrcode = compile_movr (ptrcode, 3, &freespace); // ou gptrcode ?
	*ptrcode++ = 0xE5930000; // R0 = [R3]
	ptrcode = compile_movr (ptrcode, 1, ph->param.i);
	ptrcode = compile_movr (ptrcode, 2, (int)ph);
	ptrcode = compile_call (ptrcode, ph->value.f, 0, 0);
	ptrcode = compile_movr (ptrcode, 3, (int)&freespace);
	*ptrcode++ = 0xE5830000; // [R3] = R0;
	return ptrcode;
}

instr *code_compileim (instr *ptrcode)
{
	pheader ph;
	readword(); 
	ph=getword(wr); 
	ptrcode = (*(ph->value.f)) (ptrcode, ph->param, ph);
	return ptrcode;
}

instr *code_postpone (instr *ptrcode)
{
	pheader ph;
	readword();
	ph = getword(wr);
	if (ph->wmode == MODEIM)
		ptrcode = (*(ph->value.f)) (ptrcode, ph->param, ph);
	else
	{
		ptrcode = compile_movr (ptrcode, 3, &freespace); // ou gptrcode ?
		*ptrcode++ = 0xE5930000; // R0 = [R3]
		ptrcode = compile_movr (ptrcode, 1, ph->param.i);
		ptrcode = compile_movr (ptrcode, 2, (int)ph);
		ptrcode = compile_call (ptrcode, ph->value.f, 0, 0);
		ptrcode = compile_movr (ptrcode, 3, (int)&freespace);
		*ptrcode++ = 0xE5830000; // [R3] = R0;
	}
	return ptrcode;
}

instr *code_compileadr (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0000; // POP RB 1
	*ptrcode++ = 0xE1A0E00F; // RE = RF
	*ptrcode++ = 0xE1A0F001; // RF = R0
	return ptrcode;
}

instr *code_get (instr *ptrcode)
{
	TRACE "code_get: ptrcode=%X\n", ptrcode ENDTRACE
	*ptrcode++ = 0xE8BB0001; // POP RB 1
	*ptrcode++ = 0xE5900000; // R0 = [R0]
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	return ptrcode;
}

instr *code_put (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0003; // POP RB 3
	*ptrcode++ = 0xE5801000; // [R0] = R1
	return ptrcode;
}

instr *code_cget (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0001; // POP RB 1
	*ptrcode++ = 0xE5500000; // R0 = $[R0]
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	return ptrcode;
}

instr *code_cput (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0003; // POP RB 3
	*ptrcode++ = 0xE5401000; // $[R0] = R1
	return ptrcode;
}

instr *code_hget (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0001; // POP RB 1
	*ptrcode++ = 0xE15000B0; // R0 = $$[R0]
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	return ptrcode;
}

instr *code_hput (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0003; // POP RB 3
	*ptrcode++ = 0xE14010B0; // $$[R0] = R1
	return ptrcode;
}

instr *code_sub (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0003;
	*ptrcode++ = 0xE0410000;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *code_bitand (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0003;
	*ptrcode++ = 0xE0010000;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *code_biteor (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0003;
	*ptrcode++ = 0xE0110000;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *code_mul (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0003;
	*ptrcode++ = 0xE0000091;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

int divi (int b, int a)
{
	return a/b;
}

instr *code_divi (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, divi, 3, 1);
	return ptrcode;
}

int modu (int b, int a)
{
	return a%b;
}

instr *code_modu (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, modu, 3, 1);
	return ptrcode;
}

int logand (int a, int b)
{
	return a && b;
}

instr *code_logand (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, logand, 3, 1);
	return ptrcode;
}

int logor (int a, int b)
{
	return a || b;
}

instr *code_logor (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, logor, 3, 1);
	return ptrcode;
}

int bitor (int a, int b)
{
	return a | b;
}

instr *code_bitor(instr *ptrcode)
{
	ptrcode = compile_call(ptrcode, bitor, 3, 1);
	return ptrcode;
}


int lognot (int a)
{
	return !a;
}

instr *code_lognot (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, lognot, 1, 1);
	return ptrcode;
}

#define NIFLEVELS 256
instr *adrif[NIFLEVELS];
instr *adrelse[NIFLEVELS];
// instr *adrendif[NIFLEVELS];
#define NWLEVELS 256
instr *adrwhile[NWLEVELS];
instr *adrdo[NWLEVELS];
int ofs;

instr *code_ifge0 (instr *ptrcode)
{
	iflevel++;
	*ptrcode++ = 0xE8BB0001;	
	*ptrcode++ = 0xE1B00000;
	adrif[iflevel] = ptrcode;
	adrelse[iflevel] = 0;
	*ptrcode++ = 0x4A000000;
	return ptrcode;
}

instr *code_ifne0 (instr *ptrcode)
{
	iflevel++;
	*ptrcode++ = 0xE8BB0001;	// POP RB 1
	*ptrcode++ = 0xE1B00000;	// R0 ?= R0
	adrif[iflevel] = ptrcode;
	adrelse[iflevel] = 0;
	*ptrcode++ = 0x0A000000;	// == GOTO ...
	return ptrcode;
}

/*
int dummy2 (void)
{
	int cond;
	readword();
	sscf (wr, L"%X", &cond);
	iflevel++;
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = 0xE1B00000;
	adrif[iflevel] = ptrcode;
	*ptrcode++ = 0x0A000000 | (cond * 0x10000000);
}
*/

instr *code_ifncond (instr *ptrcode)
{
	int cond;
	readword();
	sscf (wr, L"%X", &cond);
	iflevel++;
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = 0xE1B00000;
	adrif[iflevel] = ptrcode;
	adrelse[iflevel] = 0;
	*ptrcode++ = 0x0A000000 | (cond * 0x10000000);
	return ptrcode;
}

instr *code_else (instr *ptrcode)
{
	adrelse[iflevel] = ptrcode;
	*ptrcode++ = 0xEA000000;	// GOTO ...
	ofs = ptrcode - (adrif[iflevel] + 2);
	*(adrif[iflevel]) |= ofs;
	return ptrcode;
}

instr *code_endif (instr *ptrcode)
{
	if (adrelse[iflevel] == 0)
	{
		ofs = ptrcode - (adrif[iflevel] + 2);
		*(adrif[iflevel]) |= ofs;
		iflevel--;
		return ptrcode;
	}
	ofs = ptrcode - (adrelse[iflevel] + 2);
	*(adrelse[iflevel]) |= ofs;
	iflevel--;
/*
	jumpto[iflevel] = ptrcode;
	ofs = jumpto[iflevel] - (jumpfrom[iflevel] + 2);
	*(jumpfrom[iflevel]) |= ofs;
	iflevel--;
*/
	return ptrcode;
}

instr *code_begin (instr *ptrcode)
{
	wlevel++;
	adrwhile[wlevel] = ptrcode;
	return ptrcode;
}


instr *code_ge0do (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = 0xE1B00000;
	adrdo[wlevel] = ptrcode;
	*ptrcode++ = 0x4A000000;
	return ptrcode;
}

instr *code_ne0do (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = 0xE1B00000;
	adrdo[wlevel] = ptrcode;
	*ptrcode++ = 0x0A000000;
	return ptrcode;
}

instr *code_doncond (instr *ptrcode)
{
	int cond;
	readword();
	sscf (wr, L"%X", &cond);
	*ptrcode++ = 0xE8BB0001;
	*ptrcode++ = 0xE1B00000;
	adrdo[wlevel] = ptrcode;
	*ptrcode++ = 0x0A000000 | (0x10000000 * cond);
	return ptrcode;
}

instr *code_endwhile (instr *ptrcode)
{
	int ofs;
	ofs = adrwhile[wlevel] - (ptrcode+2);
	*ptrcode++ = 0xEA000000 | (ofs & 0xFFFFFF);
	ofs = ptrcode - (adrdo[wlevel] + 2);
	*(adrdo[wlevel]) |= (ofs & 0xFFFFFF);
	wlevel--;
	return ptrcode;
}

#define NCLEVELS 256
instr *cstack[NCLEVELS];
int clevel = 0;

instr *code_test (instr *ptrcode)
{
	*ptrcode++ = 0xE8BB0001; // POP RB 1
	*ptrcode++ = 0xE1B00000; // TEST R0
	return ptrcode;
}

instr *code_ge0 (instr *ptrcode)
{
	*ptrcode++ = 0xE3A01000; // R1 = 0
	*ptrcode++ = 0xE8BB0001; // POP RB 1
	*ptrcode++ = 0xE1B00000; // TEST R0
	*ptrcode++ = 0x53A01001; // >=0 R1=1
	*ptrcode++ = 0xE92B0002; // PUSH RB 2
	return ptrcode;
}

int lt (int b, int a)
{
	return a < b;
}

instr *code_lt (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, lt, 3, 1);
	return ptrcode;
}

int gt (int b, int a)
{
	return a > b;
}

instr *code_gt (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, gt, 3, 1);
	return ptrcode;
}

int le (int b, int a)
{
	return a <= b;
}

instr *code_le (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, le, 3, 1);
	return ptrcode;
}

int ge (int b, int a)
{
	return a >= b;
}

instr *code_ge (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, ge, 3, 1);
	return ptrcode;
}

int dif (int b, int a)
{
	return a != b;
}

instr *code_dif (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, dif, 3, 1);
	return ptrcode;
}

instr *code_equal (instr *ptrcode)
{
	*ptrcode++ = 0xE3A02000; // R2 = 0
	*ptrcode++ = 0xE8BB0003; // POP RB 3
	*ptrcode++ = 0xE0500001; // R0 ?= R0 - R1
	*ptrcode++ = 0x03A02001; // =0 R2=1
	*ptrcode++ = 0xE92B0004; // PUSH RB 4
	return ptrcode;
}

instr * code_here (instr *ptrcode)
{
	any ptrcode1;
	ptrcode1.pcode = ptrcode;
	level++; // a verifier
	startcode.pcode = codes[level];
	ptrcode = codes[level];
	ptrcode = compile_begindef(ptrcode);

	/* R0 = ptrcode1 */
	*ptrcode++ = 0xE59F0000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)(ptrcode1.i);

	/* PUSH RB 1 */
	*ptrcode++ = 0xE92B0001;

	ptrcode = compile_enddef(ptrcode);
	(*(startcode.f)) ();
	level--;
	return ptrcode;
}

instr * code_brofs (instr *ptrcode)
{
	// a verifier
	level++;
	startcode.pcode = codes[level];
	ptrcode = codes[level];
	ptrcode = compile_begindef(ptrcode);

	/* POP RB 3 	*/	*ptrcode++ = 0xE8BB0003;
	/* R2 = R0 - R1 	*/	*ptrcode++ = 0xE0402001;	
	/* R2 = R2 - 8 	*/ 	*ptrcode++ = 0xE2422008;
	/* R2 = R2 >> 2 	*/	*ptrcode++ = 0xE1A02142;
	/* R3 = [R1] 	*/	*ptrcode++ = 0xE5913000;
	/* R3 = R3 | R2 	*/	*ptrcode++ = 0xE1833002;
	/* [R1] = R3 	*/	*ptrcode++ = 0xE5813000;

	ptrcode = compile_enddef(ptrcode);
	(*(startcode.f)) ();
	level--;
	return ptrcode;
}

instr *code_cbranch (instr *ptrcode)
{
	int cond;
	readword();
	sscf (wr, L"%X", &cond);
	*ptrcode++ = 0x0A000000 | (cond * 0x10000000);
	return ptrcode;
}

instr *code_push (instr *ptrcode)
{
	int regs;
	readword();
	sscf(wr,L"%X",&regs);
	*ptrcode++ = 0xE92B0000 | regs;
	return ptrcode;
}

instr *code_pop (instr *ptrcode)
{
	int regs;
	readword();
	sscf(wr,L"%X",&regs);
	*ptrcode++ = 0xE8BB0000 | regs;
	return ptrcode;
}

instr *code_pushr (instr *ptrcode)
{
	int reg, regs;
	readword();
	sscf (wr, L"%X", &reg);
	readword();
	sscf(wr,L"%X",&regs);
	*ptrcode++ = 0xE9200000 | (reg * 0x10000) | regs;
	return ptrcode;
}

instr *code_popr (instr *ptrcode)
{
	int reg, regs;
	readword();
	sscf (wr, L"%X", &reg);
	readword();
	sscf(wr,L"%X",&regs);
	*ptrcode++ = 0xE8B00000 | (reg * 0x10000) | regs;
	return ptrcode;
}

instr *code_reg (instr *ptrcode)
{
	unsigned int reg;
	readword();
	sscf (wr, L"%X", &reg);
	*ptrcode++ = 0xE3A00000 | (reg & 0xF);
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *code_synonym (instr *ptrcode)
{
	pheader phnew, phold;
	readword();
	phnew = getword (wr);
	readword();
	phold = getword (wr);
	phnew->value = phold->value;
	phnew->param = phold->param;
	return ptrcode;
}

instr *code_comment (instr *ptrcode)
{
	carac endcom[100];
	pheader phend, ph;
	readword ();
	phend = getword(wr);
	scpy (endcom, wr);
	for (;;)
	{
		readword();
		ph = getword (wr);
		if (ph == phend)
			break;
		if (!scmp (wr, endcom))
			break;
		if (!*wr)
			break;	
	}
	return ptrcode;
}


instr *code_lpar (instr *ptrcode)
{
	carac endcom[100];
	pheader phend, ph;
	
	for (;;)
	{
		readcarac();
		if (cr == ')' || cr == 0)
			break;
	}
	return ptrcode;
}


instr *code_backslash (instr *ptrcode)
{
	carac endcom[100];
	pheader phend, ph;
	
	for (;;)
	{
		readcarac();
		if (cr == 10 || cr == 13 || cr == 0)
			break;
	}
	return ptrcode;
}

instr *code_allotw (instr *ptrcode)
{
	/* POP R0 */
	*ptrcode++ = 0xE8BB0001;

	/* R1 = &freespace */
	*ptrcode++ = 0xE59F1000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&freespace;
	
	/* R2 = [R1] */
	*ptrcode++ = 0xE5912000;

	/* R0 = R0 + R2 */
	// *ptrcode++ = 0xE0800002;

	// R0 = R2 + R0 << 2
	*ptrcode++ = 0xE0820100; 

	/* [R1] = R0 */
	*ptrcode++ = 0xE5810000;
	return ptrcode;
}

/*
instr * code_ustring (instr *ptrcode)
{
	any begin;
	begin = freespace;
	for (;;)
	{
		readcarac();
		if (cr == '"')
			break;
		*freespace.pcarac++ = cr;
	}
	*freespace.pcarac++ = 0;
	freespace.i = ((freespace.i + 3) / 4) * 4;
	ptrcode = compile_pushint (ptrcode, begin.i);
	return ptrcode;
}

instr * code_astring (instr *ptrcode)
{
	any begin;
	begin = freespace;
	for (;;)
	{
		readcarac();
		if (cr == '"') 
			break;
		*freespace.pchar++ = cr;
	}
	*freespace.pchar++ = 0;
	freespace.i = ((freespace.i + 3) / 4) * 4;
	ptrcode = compile_pushint (ptrcode, begin.i);
	return ptrcode;
}
*/

instr * code_ustring1 (instr *ptrcode)
{
	any begin;
	any ptr;
	instr *adrgoto;
	*ptrcode++ = 0xE28F0004; // R0 = RF+4
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	adrgoto = ptrcode;
	*ptrcode++ = 0xEA000000; // GOTO
	begin.pcode = ptrcode;
	ptr.pcode = ptrcode;
	for (;;)
	{
		readcarac();
		if (cr == '"') 
			break;
		*ptr.pcarac++ = cr;
	}
	*ptr.pchar++ = 0;
	ptr.i = ((ptr.i + 3) / 4) * 4;
	*adrgoto += ptr.pcode - adrgoto - 2;
	return ptr.pcode;
}

instr * code_astring1 (instr *ptrcode)
{
	any begin;
	any ptr;
	instr *adrgoto;
	*ptrcode++ = 0xE28F0004; // R0 = RF+4
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	adrgoto = ptrcode;
	*ptrcode++ = 0xEA000000; // GOTO
	begin.pcode = ptrcode;
	ptr.pcode = ptrcode;
	for (;;)
	{
		readcarac();
		if (cr == '"') 
			break;
		*ptr.pchar++ = cr;
	}
	*ptr.pchar++ = 0;
	ptr.i = ((ptr.i + 3) / 4) * 4;
	*adrgoto += ptr.pcode - adrgoto - 2;
	return ptr.pcode;
}


instr * code_ustring (instr *ptrcode)
{
	any begin;
	any ptr;
	instr *adrgoto;
	*ptrcode++ = 0xE28F0004; // R0 = RF+4
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	adrgoto = ptrcode;
	*ptrcode++ = 0xEA000000; // GOTO
	begin.pcode = ptrcode;
	ptr.pcode = ptrcode;
	for (;;)
	{
		readcarac();
		if (cr == '"') 
		{
			readcarac();
			if(cr!='"') 
			 break;
		}
		*ptr.pcarac++ = cr;
	}
	*ptr.pchar++ = 0;
	ptr.i = ((ptr.i + 3) / 4) * 4;
	*adrgoto += ptr.pcode - adrgoto - 2;
	return ptr.pcode;
}

instr * code_astring (instr *ptrcode)
{
	any begin;
	any ptr;
	instr *adrgoto;
	*ptrcode++ = 0xE28F0004; // R0 = RF+4
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	adrgoto = ptrcode;
	*ptrcode++ = 0xEA000000; // GOTO
	begin.pcode = ptrcode;
	ptr.pcode = ptrcode;
	for (;;)
	{
		readcarac();
		if (cr == '"') 
			break;
		*ptr.pchar++ = cr;
	}
	*ptr.pchar++ = 0;
	ptr.i = ((ptr.i + 3) / 4) * 4;
	*adrgoto += ptr.pcode - adrgoto - 2;
	return ptr.pcode;
}



instr *code_comma (instr *ptrcode)
{
	/* POP R0 */
	*ptrcode++ = 0xE8BB0001;

	/* R1 = &freespace */
	*ptrcode++ = 0xE59F1000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&freespace;

	/* R2 = [R1] */
	*ptrcode++ = 0xE5912000;

	/* [R2]  = R0 */
	*ptrcode++ = 0xE5820000;

	/* R2 = R2 + 4 */
	*ptrcode++ = 0xE2822004;

	/* [R1] = R2 */
	*ptrcode++ = 0xE5812000;
	return ptrcode;
}

instr * getparam (instr *ptrcode, any param)
{
	TRACE "getparam ptrcode=%X param=%X\n", ptrcode, param ENDTRACE
	/* PUSH RB param */
	*ptrcode++ = 0xE59F0000; 
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)param.i;
	*ptrcode++ = 0xE92B0001;
	return ptrcode;
}

instr *code_icreate (instr *ptrcode)
{
	pheader ph;
	readword();
	ph = getword(wr);
	ph->value.f = getparam;
	TRACE "code_icreate: freespace=%X ptrcode=%X\n", freespace, ptrcode ENDTRACE
	ph->param = freespace;
	return ptrcode;
}

instr * code_create (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, code_icreate, 0, 0);
	return ptrcode;
}

instr *code_createdef (instr *ptrcode)
{
	pheader ph;
	readword();
	ph = getword(wr);
	ph->value.f = getparam;
	ph->param = freespace;
	mode = MODECP;
	return ptrcode;
}

instr *code_endcreatedef (instr *ptrcode)
{
	mode = MODEIM;
	return ptrcode;
}

#if 0
int code_does (void)
{
	any startdoes;

	compile_enddef ();
	// lastword->value.pcode = ptrcode;
	startdoes.pcode = ptrcode;
	compile_begindef ();
	*ptrcode++ = 0xE92B0001; // PUSH RB 1
	
	/* R1 = &lastword */
	*ptrcode++ = 0xE59F1000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&lastword;

	/* R2 = startdoes */
	*ptrcode++ = 0xE59F2000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)(startdoes.i);

	/* R1 = [R1] */
	*ptrcode++ = 0xE5911000;

	/* [R1+4] = R2 */
	*ptrcode++ = 0xE5812004;
}

int code_does (void)
{
	/* R1 = &lastword */
	*ptrcode++ = 0xE59F1000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&lastword;

	/* R2 = ptrcode + 4 */
	*ptrcode++ = 0xE59F2000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)(ptrcode+4);

	/* R1 = [R1] */
	*ptrcode++ = 0xE5911000;

	/* [R1+4] = R2 */
	*ptrcode++ = 0xE5812004;

	compile_enddef();
	compile_begindef();

	*ptrcode++ = 0xE92B0001; // PUSH RB 1
}
#endif

instr * code_idoes (instr *ptrcode)
{
	/* R1 = &lastword */
	*ptrcode++ = 0xE59F1000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&lastword;

	/* R2 = ptrcode + 4 */
	*ptrcode++ = 0xE59F2000;
	*ptrcode++ = 0xEA000000;
	*ptrcode = (instr)(ptrcode+4);
	ptrcode++;

	/* R1 = [R1] */
	*ptrcode++ = 0xE5911000;

	/* [R1+4] = R2 */
	*ptrcode++ = 0xE5812004;

	ptrcode = compile_enddef(ptrcode);
	ptrcode = compile_begindef(ptrcode);

	// *ptrcode++ = 0xE92B0001; // PUSH RB 1

	// a verifier : compile R0=ptrcode ?
	ptrcode = compile_call (ptrcode,getparam,0,0);
	return ptrcode;
}

instr * code_does (instr *ptrcode)
{
	any u;
	/* R1 = &lastword */
	*ptrcode++ = 0xE59F1000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&lastword;

	/* R2 = ptrcode + 4 */
	*ptrcode++ = 0xE59F2000;
	*ptrcode++ = 0xEA000000;
	*ptrcode = (instr)(ptrcode+4);
	ptrcode++;

	/* R1 = [R1] */
	*ptrcode++ = 0xE5911000;

	/* [R1+4] = R2 */
	*ptrcode++ = 0xE5812004;

	ptrcode = compile_enddef(ptrcode);
	ptrcode = compile_begindef(ptrcode);

	// *ptrcode++ = 0xE92B0001; // PUSH RB 1
	// a verifier : compile R0=ptrcode ?
	ptrcode = compile_call (ptrcode, getparam,0,0);

	u.pcode = ptrcode+5;
	ptrcode = compile_movr1 (ptrcode, u.i);
	ptrcode = compile_call (ptrcode,calldef,0,0);

	ptrcode = compile_enddef(ptrcode);
	ptrcode = compile_begindef(ptrcode);
}

/*
int code_does (void)
{
	compile_enddef();
	lastword->value.pcode = ptrcode;
	compile_begindef();
	compile_call (getparam,0,0);
}
*/


instr *doesf (instr *ptrcode, any param)
{
	compile_call (ptrcode, param.f, 0, 0);
	return ptrcode;
}

instr * code_doest (instr *ptrcode)
{
	/* R1 = &lastword */
	*ptrcode++ = 0xE59F1000;
	*ptrcode++ = 0xEA000000;
	*ptrcode++ = (instr)&lastword;

	/* R2 = ptrcode + 4 */
	*ptrcode++ = 0xE59F2000;
	*ptrcode++ = 0xEA000000;
	*ptrcode = (instr)(ptrcode+4);
	ptrcode++;

	/* R1 = [R1] */
	*ptrcode++ = 0xE5911000;

	/* [R1+8] = R2 */
	*ptrcode++ = 0xE5812008;

	/* R2 = doesf */
	*ptrcode++ = 0xE59F2000;
	*ptrcode++ = 0xEA000000;
	*ptrcode = (instr)doesf;
	
	/* [R1+4] = R2 */
	*ptrcode++ = 0xE5812004;

	ptrcode = compile_enddef(ptrcode);
	ptrcode = compile_begindef(ptrcode);

	// *ptrcode++ = 0xE92B0001; // PUSH RB 1
	// a verifier : compile R0=ptrcode ?
	ptrcode = compile_call (ptrcode,getparam,0,0);
	return ptrcode;
}

instr *code_savecorerw (instr *ptrcode)
{
	readword ();
	savecore (awr);
	return ptrcode;
}

instr *code_restorecorerw (instr *ptrcode)
{
	readword();
	restorecore (awr);
	return ptrcode;
}

instr *code_savecore (instr *ptrcode)
{
 ptrcode=compile_call(ptrcode,savecore,1,0);
 return ptrcode;
}

instr *code_restorecore(instr *ptrcode)
{
 ptrcode=compile_call(ptrcode,restorecore,1,0);
 return ptrcode;
}


instr *code_savedicrw (instr *ptrcode)
{
	readword ();
	savedic (awr);
	return ptrcode;
}

instr *code_restoredicrw (instr *ptrcode)
{
	readword();
	restoredic (awr);
	return ptrcode;
}

instr *code_savedic (instr *ptrcode)
{
 ptrcode=compile_call(ptrcode,savedic,1,0);
 return ptrcode;
}

instr *code_restoredic(instr *ptrcode)
{
 ptrcode=compile_call(ptrcode,restoredic,1,0);
 return ptrcode;
}

instr *code_dump (instr *ptrcode)
{
	dump();
	return ptrcode;
}

instr *code_instance (instr *ptrcode)
{
	ptrcode = compile_pushint (ptrcode, (int)ghInstance);
	return ptrcode;
}

instr *code_previnstance (instr *ptrcode)
{
	ptrcode = compile_pushint (ptrcode, (int)ghPrevInstance);
	return ptrcode;
}

instr *code_cmdline (instr *ptrcode)
{
	ptrcode = compile_pushint (ptrcode, (int)glpCmdLine);
	return ptrcode;
}

instr *code_cmdshow (instr *ptrcode)
{
	ptrcode = compile_pushint (ptrcode, gnCmdShow);
	return ptrcode;
}

instr *code_mainwindow (instr *ptrcode)
{
	ptrcode = compile_pushint (ptrcode, (int)ghWnd);
	return ptrcode;
}

int setmainwindowproc (LRESULT (*f)())
{
	gWndProc = f;
}

instr *code_setmainwindowproc (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, setmainwindowproc, 1, 0);
	return ptrcode;
}

int setpaintproc (int (*f)())
{
	gPaint = f;
}

instr *code_setpaintproc (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, setpaintproc, 1, 0);
	return ptrcode;
}

instr *code_hedit1 (instr *ptrcode)
{
	ptrcode = compile_pushint (ptrcode, (int)hEdit1);
	return ptrcode;
}

instr *code_hbutton1 (instr *ptrcode)
{
	ptrcode = compile_pushint (ptrcode, (int)hButton1);
	return ptrcode;
}

instr *code_inbuf (instr *ptrcode)
{
	ptrcode = compile_pushint (ptrcode, (int)inbuf);
	return ptrcode;
}

wchar_t *input (wchar_t *s)
{
	MSG msg;
	int status;
	if (s != NULL)
		SendMessage (hEdit1, WM_SETTEXT, 0, (long)s);
	gotinput = 0;
	while (!gotinput)
	{
		status = GetMessage (&msg, NULL, 0, 0);
		if (!status)
			return inbuf;
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}
	return inbuf;
}

instr *code_input (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, input, 1, 1);
	return ptrcode;
}

struct ParamCreateWindow 
{
	LPCTSTR lpClassName;
    LPCTSTR lpWindowName;
    DWORD dwStyle;
    int x;
    int y;
    int nWidth;
    int nHeight;
    HWND hWndParent;
    HMENU hMenu;
    HINSTANCE hInstance;
    LPVOID lpParam;
};

HWND CreateWindowStruct (struct ParamCreateWindow *p)
{
	HWND hWnd;
	char aclass[1000];
	char aname[1000];
	int i;
	for (i=0; p->lpClassName[i]; i++)
		aclass[i] = p->lpClassName[i];
	aclass[i] = 0;
	for (i=0; p->lpWindowName[i]; i++)
		aname[i] = p->lpWindowName[i];
	aname[i] = 0;

	TRACE "CreateWindow\nclass=%X <%s>\nname=%X <%s>\nstyle=%X\nx=%X\ny=%X\nw=%X\nh=%X\nparent=%X\nmenu=%X\ninstance=%X\nparam=%X\n",
		p->lpClassName, aclass, p->lpWindowName, aname, p->dwStyle, p->x, p->y, 
		p->nWidth, p->nHeight, p->hWndParent, p->hMenu, p->hInstance, p->lpParam
	ENDTRACE

	hWnd = CreateWindow (
		p->lpClassName, 
		p->lpWindowName, 
		p->dwStyle,
		p->x,
		p->y,
		p->nWidth,
		p->nHeight,
		p->hWndParent,
		p->hMenu,
		p->hInstance,
		p->lpParam);


	TRACE "CreateWindow -> %X\n", hWnd ENDTRACE
		
	return hWnd;
}


instr *code_createwindow (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, CreateWindowStruct, 1, 1);
	return ptrcode;
}

int rectangle1 (HDC hdc, int *r)
{
	/*
	wchar_t buf[1000];
	swprintf (buf, L"hdc=%X x1=%X y1=%X x2=%X y2=%X",
		hdc, r[0], r[1], r[2], r[3]);
	MessageBox (0, buf, L"trace", MB_OK);
	*/
	return Rectangle (hdc, r[0], r[1], r[2], r[3]);
}

instr code_rectangle (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, rectangle1, 3, 1);
	return ptrcode;
}

int rgb (int b, int g, int r)
{
	return RGB (r, g, b);
}

instr *code_rgb (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, rgb, 7, 1);
	return ptrcode;
}

int coord (int j, int i)
{
	return i<<16 | (j & 0xFFFF);
}

instr *code_coord (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, coord, 3, 1);
	return ptrcode;
}

int setpixel (HDC hdc, int ij, int c)
{
	return SetPixel (hdc, ij>>16, ij&0xFFFF, c);
}

instr *code_setpixel (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, setpixel, 7, 1);
	return ptrcode;
}

int getpixel (HDC hdc, int ij)
{
	return GetPixel (hdc, ij>>16, ij&0xFFFF);
}

instr *code_getpixel (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, getpixel, 3, 1);
	return ptrcode;
}

char asmcode[30000];

instr *code_asm (instr *ptrcode)
{
 TRACE "code_asm\n" ENDTRACE 
 readto ('}',asmcode);
 TRACE "ptrcode=%X out.f=%X out.p=%X code: <%s>\n", ptrcode, out.f, out.p, asmcode ENDTRACE
 ptrcode = asmem(ptrcode,asmcode);
 TRACE "ptrcode=%X out.f=%X out.p=%X\n", ptrcode, out.f, out.p ENDTRACE 
 return ptrcode;
}

instr *code_includeasm (instr *ptrcode)
{
 char filename[100];
 readto ('"', filename);
 ptrcode = loadasm1 (ptrcode, filename);
 return ptrcode;
}

int trace (char *s)
{
	TRACE "trace: %s.\n", s ENDTRACE
}

instr *code_trace (instr *ptrcode)
{
	compile_call (ptrcode, trace, 1, 1);
	return ptrcode;
}

instr *code_resw (instr *ptrcode)
{
	int n;
	readword();
	sscanf (awr, "%x", &n);
	ptrcode += n;
	return ptrcode;
}


/*
int lognot (int a)
{
	return !a;
}

instr *code_lognot (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, lognot, 1, 1);
	return ptrcode;
}
*/

int gtc (void)
{
	return GetTickCount();
}

instr *code_gtc (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, gtc, 0, 1);
	return ptrcode;
}

int gtcm (void)
{
	DWORD (*pgtc) (void);
	pgtc = GetProcAddress (GetModuleHandle (L"corelib.dll"), L"GetTickCount");
	return (*pgtc)();
}

instr *code_gtcm (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, gtcm, 0, 1);
	return ptrcode;
}

int wsagetlasterror1 (void)
{
	return WSAGetLastError ();
}

instr code_wsagetlasterror (instr *ptrcode)
{
	ptrcode = compile_call (ptrcode, wsagetlasterror1, 0, 1);
	return ptrcode;
}

instr *code_quit (instr *ptrcode)
{
	exit(0);
	return ptrcode;
}

pheader word_phcalled, word_def;

#define DEFP(name,fnc) ph=createword(name); ph->value.f=fnc;

void init_prim (void)
{
	pheader ph;
	instr *pc;

	TRACE "begin init_prim\n" ENDTRACE

		/*
	ph = getword (L"#");
	TRACE "assign value %X\n", &(ph->value.f) ENDTRACE
	ph->value.f = code_pushint;

	TRACE "DEFP +" ENDTRACE
		*/

	DEFP (L"#", code_pushint);
	DEFP (L"0x", code_pushint);
	DEFP (L"LIT", code_pushint);
	DEFP (L"PHCALLED", code_phcalled);
	word_phcalled = ph;
	DEFP (L"#D", code_pushintdec);
	DEFP (L"#P", code_pushint);
	DEFP (L"+", code_add);
	DEFP (L"-", code_sub);
	DEFP (L"*", code_mul);
	DEFP (L"/", code_divi);
	DEFP (L"%", code_modu);
	DEFP (L"&", code_bitand);
	DEFP (L"|",code_bitor);
	DEFP (L"EOR",code_biteor);
	DEFP (L"&&",code_logand);
	DEFP (L"||",code_logor);
	DEFP (L"~",code_lognot);

	DEFP (L"OVERN", code_overn);
	DEFP (L"S>P", code_stop);
	DEFP (L"NS>P", code_nstop);
	DEFP (L"NPDROP", code_npdrop);
	DEFP (L"NP", code_np);
	DEFP (L"RP", code_rp);

	DEFP (L"PRINT", code_print);
	DEFP (L".", code_printdec);
	DEFP (L"PRINTDEC", code_printdec);
	DEFP (L"PRINTASTRING", code_printastring);
	DEFP (L"PRINTUSTRING", code_printustring);
	DEFP (L"DOIT", code_doit);

	DEFP (L"@", code_get);
	DEFP (L"!", code_put);
	DEFP (L"C@", code_cget);
	DEFP (L"C!", code_cput);
	DEFP (L"H@", code_hget);
	DEFP (L"H!", code_hput);

	DEFP (L"[[", code_levelup);
	DEFP (L"]]", code_leveldown);
	DEFP (L"[", code_beginim);
	DEFP (L"]", code_endim);
	
	/*
	DEFP (L"[-", code_modedown); ph->wmode = 0;
	DEFP (L"-]", code_modeup); ph->wmode = 0;
	DEFP (L"[--", code_modedown1);
	DEFP (L"--]", code_modeup1);
	*/
	
	DEFP (L"MODEDOWN", code_modedown); ph->wmode = MODEIM;
	DEFP (L"MODEUP", code_modeup); ph->wmode = MODEIM;
	DEFP (L"MODEDOWN1", code_modedown1);
	DEFP (L"MODEUP1", code_modeup1);

	DEFP (L"GETMODE",code_getmode);
	DEFP (L"SETMODE",code_setmode);

	DEFP (L":", code_def);
	word_def = ph;
	DEFP (L";", code_enddef);
	DEFP (L";;", code_enddef2);
    DEFP (L";RAWCODE", code_endrawcode);

	DEFP (L"INITDEF", compile_begindef);
	DEFP (L"TERMDEF", compile_enddef);

	DEFP (L"DEFINE", code_define);
	DEFP (L"CARGS", code_cargs); ph->wmode = MODEIM1;
	DEFP (L"VARGS", code_vargs); ph->wmode = MODEIM1;
	DEFP (L"END", code_end);
	DEFP (L"CARGSP", code_cargsp); ph->wmode = MODEIM1;
	DEFP (L"VARGSP", code_vargsp); ph->wmode = MODEIM1;
	DEFP (L"ENDP", code_endp);
	DEFP (L"((", code_beginargs);
	DEFP (L"))", code_endargs);
	DEFP (L"((1", code_beginvargs1);
	DEFP (L"1))", code_endvargs1);
	DEFP (L"SAP", code_sap);
	DEFP (L"VARG", code_varg);
	DEFP (L"CARG", code_carg);

	DEFP (L"{", code_begin_locals);
	/*
	DEFP (L"}", code_end_locals);
	DEFP (L"\\", code_begin_uninit); 
	*/
	DEFP (L"->", code_store_local);
	DEFP (L"^", code_adr_local);

	DEFP (L"{{", code_lambda);
	DEFP (L"}}", code_endlambda);
	DEFP (L"LAMBDA", code_lambda);
	DEFP (L";LAMBDA", code_endlambda);
	DEFP (L";LCOMPILE", code_endlambdacompile);

	DEFP (L"WMODEIM", code_wmodeim);
	DEFP (L"IMMEDIATE",code_immediate);

	DEFP (L"EXECUTE", code_execute);
	DEFP (L"EXECOMP", code_execomp);

	DEFP (L"MACRO:", code_defmacro);
	DEFP (L"RAWCODE:", code_defrawcode);
	DEFP (L"RAWCODEMACRO:", code_defrawcodemacro);
	DEFP (L";MACRO", code_endmacro);
	DEFP (L"LOAD", code_load);
	DEFP (L"LOADDESC", code_loaddesc);
	DEFP (L"LOAD>", code_loadrw);
	DEFP (L"DOASTRING", code_doastring);
	DEFP (L"DOUSTRING", code_doustring);
	DEFP (L"MAINLOOP", code_mainloop);
	DEFP (L"OUTASTRING", code_outastring);
	DEFP (L"OUTUSTRING", code_outustring);
	DEFP (L"OUTSTD", code_outstd);
	DEFP (L"OUTFILE", code_outfile);
	DEFP (L"OUTFILEDESC", code_outfiledesc);
	DEFP (L"CLOSEOUT", code_closeout);
	DEFP (L"PUTCODE", code_putcode);
	// DEFP (L"EXTVAL", code_extval);
	DEFP (L"CALL", code_call);
	DEFP (L"CALLMODULE", code_callmodule);
	DEFP (L"CM", code_callmodule);
	DEFP (L"DCALL", code_dcall);
	DEFP (L"CALLADR", code_calladr);
	DEFP (L"EXTVAL", code_extval);
	DEFP (L"GLOBALS", code_globals);
	DEFP (L"'", code_tick);
	DEFP (L"[COMPILE]'", code_compiletick);
	DEFP (L"MODE", code_mode);
	DEFP (L"DP", code_dp);
	DEFP (L"FREEDIC", code_freedic);
	DEFP (L"FREESPACE", code_freespace);
	DEFP (L"LASTWORD", code_lastword);
	DEFP (L"POSTPONE", code_postpone);
	DEFP (L"COMPILE", code_compile);
	DEFP (L"COMPILE1", code_compile1);
	DEFP(L"[COMPILE]",code_compileim);
	DEFP (L"COMPILEADR", code_compileadr);
	DEFP (L"COMPILE#",code_compile_pushint);
	DEFP (L"#COMPILE",code_pushint_compile);
	DEFP (L"IF>=0", code_ifge0);
	DEFP (L"IF", code_ifne0);
	DEFP (L"IFNCOND", code_ifncond);
	DEFP (L"ELSE", code_else);
	DEFP (L"THEN", code_endif);

	DEFP (L"BEGIN", code_begin);
	DEFP (L">=0DO", code_ge0do);
	DEFP (L"WHILE", code_ne0do);
	DEFP (L"DONCOND", code_doncond);
	DEFP (L"REPEAT", code_endwhile);

	DEFP (L"TEST", code_test);
	DEFP (L">=0", code_ge0);
	DEFP (L"<", code_lt);
	DEFP (L">", code_gt);
	DEFP (L"<=", code_le);
	DEFP (L">=", code_ge);
	DEFP (L"<>", code_dif);
	DEFP (L"=", code_equal);
	DEFP (L"HERE", code_here);
	DEFP (L"CBRANCH", code_cbranch);
	DEFP (L"BROFS", code_brofs);

	DEFP (L"PUSH", code_push);
	DEFP (L"POP", code_pop);

	DEFP (L"PUSHR", code_pushr);
	DEFP (L"POPR", code_popr);
	DEFP (L"REG", code_reg);

	DEFP (L"PRINTCHAR", code_printchar);
	DEFP (L"READCHAR", code_readchar);

	DEFP (L"SYNONYM", code_synonym);

	DEFP (L"REM", code_comment);
	DEFP (L"(", code_lpar);
	DEFP (L"\\", code_backslash);

	DEFP (L"L\"", code_ustring);
	DEFP (L"\"", code_astring);
	DEFP (L"ALLOTW", code_allotw);
	DEFP (L",", code_comma);
	DEFP (L"ICREATE", code_icreate);
	DEFP (L"CREATE", code_create); ph->wmode = MODEIM1;
	DEFP (L"CREATE:", code_createdef);
	DEFP (L";CREATE", code_endcreatedef);

	// DEFP (L"CDOES>", code_cdoes);
	DEFP (L"IDOES>", code_idoes);
	DEFP (L"DOES>", code_does);
	DEFP (L"DOEST>", code_doest);

	DEFP (L"SAVECORE", code_savecore);
	DEFP (L"RESTORECORE", code_restorecore);
	DEFP (L"SAVECORE>", code_savecorerw);
	DEFP (L"RESTORECORE>", code_restorecorerw);

	DEFP (L"SAVEDIC", code_savedic);
	DEFP (L"RESTOREDIC", code_restoredic);
	DEFP (L"SAVEDIC>", code_savedicrw);
	DEFP (L"RESTOREDIC>", code_restoredicrw);

	DEFP (L"DUMP", code_dump);

	DEFP (L"INSTANCE", code_instance);
	DEFP (L"PREVINSTANCE", code_previnstance);
	DEFP (L"CMDLINE", code_cmdline);
	DEFP (L"CMDSHOW", code_cmdshow);
	DEFP (L"MAINWINDOW", code_mainwindow);
	DEFP (L"SETMAINWINDOWPROC", code_setmainwindowproc);
	DEFP (L"SETPAINTPROC", code_setpaintproc);
	DEFP (L"HEDIT1", code_hedit1);
	DEFP (L"HBUTTON1", code_hbutton1);
	DEFP (L"INBUF", code_inbuf);
	DEFP (L"INPUT", code_input);

	DEFP (L"CREATEWINDOW", code_createwindow);
	DEFP (L"RECTANGLE", code_rectangle);

	DEFP (L"RGB",code_rgb);
	DEFP (L"COORD",code_coord);
	DEFP (L"SETPIXEL",code_setpixel);
	DEFP (L"GETPIXEL",code_getpixel);

	DEFP (L"ASM{",code_asm);
	DEFP (L"INCLUDEASM\"", code_includeasm);

	DEFP (L"TRACE", code_trace);

	DEFP (L"RESW", code_resw);
	DEFP (L"RESTORESTACK", code_restorestack);
	DEFP (L"SAVESTACK", code_savestack);

	DEFP (L"GTC", code_gtc);
	DEFP (L"GTCM", code_gtcm);

	DEFP (L"WSAGETLASTERROR", code_wsagetlasterror);

	DEFP (L"QUIT", code_quit);

/*
	DEFP (L"#", code_pushint);
	DEFP (L"+", code_add);
	DEFP (L"-", code_sub);
	DEFP (L"*", code_mul);

	DEFP (L"PRINT", code_print);
	DEFP (L".", code_dot);

	DEFP (L"@", code_get);
	DEFP (L"!", code_put);
	DEFP (L"C@", code_cget);
	DEFP (L"C!", code_cput);
	DEFP (L"H@", code_hget);
	DEFP (L"H!", code_hput);

	DEFP (L"{", code_beginim);
	DEFP (L"}", code_endim);
	DEFP (L"DEF", code_def);
	DEFP (L"END", code_enddef);

	DEFP (L"ENDMACRO", code_endmacro);
	DEFP (L"LOAD", code_load);
	DEFP (L"PUTCODE", code_putcode);
	DEFP (L"CALL", code_call);
	DEFP (L"GLOBALS", code_globals);
	DEFP (L"'", code_tick);
	DEFP (L"DP", code_dp);
	DEFP (L"FREESPACE", code_freespace);
	DEFP (L"LASTWORD", code_lastword);
	DEFP (L"COMPILE", code_compile);
	DEFP (L"IF>=0", code_ifge0);
	DEFP (L"IF<>0", code_ifne0);
	DEFP (L"IFNCOND", code_ifncond);
	DEFP (L"ELSE", code_else);
	DEFP (L"ENDIF", code_endif);

	DEFP (L"WHILE", code_begin);
	DEFP (L">=0DO", code_ge0do);
	DEFP (L"<>0DO", code_ne0do);
	DEFP (L"DONCOND", code_doncond);
	DEFP (L"ENDWHILE", code_endwhile);

	DEFP (L"PUSH", code_push);
	DEFP (L"POP", code_pop);

	DEFP (L"PRINTCHAR", code_printchar);
	DEFP (L"READCHAR", code_readchar);

	DEFP (L"SYNONYM", code_synonym);

	DEFP (L"REM", code_comment);

	DEFP (L"ALLOT", code_allot);
	DEFP (L",", code_comma);
	DEFP (L"CREATE", code_create);

	DEFP (L"DOES>", code_does);

	DEFP (L"QUIT", code_quit);
*/

	TRACE "end init_prim\n" ENDTRACE
/*
	ph = getword (L"#");
	ph->value.f = code_pushint;

	ph = getword (L"+");
	ph->value.f = code_add;

	ph = getword (L"PRINT");
	ph->value.f = code_print;

	ph = getword (L".");
	ph->value.f = code_dot;

	ph = getword (L"{");
	ph->value.f = code_begin;

	ph = getword (L"}");
	ph->value.f = code_end;

	ph = getword (L"DEF");
	ph->value.f = code_def;

	ph = getword (L"END");
	ph->value.f = code_enddef;

	ph = getword (L"LOAD");
	ph->value.f = code_load;

	ph = getword (L"PUTCODE");
	ph->value.f = code_putcode;

	ph = getword (L"CALL");
	ph->value.f = code_call;

	ph = getword (L"GLOBALS");
	ph->value.f = code_globals;

	ph = getword (L"'");
	ph->value.f = code_tick;

	ph = getword (L"[COMPILE]");
	ph->value.f = code_imcompile;

	ph = getword (L"DP");
	ph->value.f = code_dp;

	ph = getword (L"COMPILE");
	ph->value.f = code_compile;
*/
}

void init_forth (void)
{
	// out = fopen ("\\Carte de stockage\\pellesc\\forth\\out.txt", "w");
	// fprintf (out, "init_forth\n");
	// fflush (out);
	TRACE "init_forth\n" ENDTRACE
	initext();
	lastword = NULL;
	freedic.ph = (pheader)dic;
	freespace.ph = (pheader)diccode;
	level = 0;
	// ptrcode = codes[0];
	startcode.pcode = codes[0];
	TRACE "instr_begin\n" ENDTRACE
	// ptrcode = instr_begin(ptrcode);
	inlevel = 0;
	init_prim ();
	ptrstack = stack + STACKSIZE;
	// ptrpstack = pstack;
	ptrpstack = stack;
	iflevel = 0;

	mode=MODEIM;

	globals.dic = dic;
	globals.diccode = diccode;
	globals.codes = codes[0];
	globals.ptrcodes = ptrcodes;
	globals.wr = wr;
	globals.awr = awr;
	globals.instack = instack;
	globals.startcodes = startcodes;

	imcode.pcode = imcodebuf;

	imcodeptr = imcode.pcode;
	imcodeptr = compile_init (imcodeptr);
	// (*(imcode.f)) ();
	aaa = 0;
	bbb = 0;
}

instr *defaultop (instr *ptrcode, any param, pheader ph)
{
	carac mes[100];
	int i;
	int status, x;
	/*
	for (i=0; ph->name[i]; i++)
		mes[i] = ph->name[i];
	mes[i]=0;
	MessageBox (NULL, mes, L"defaultop", MB_OK);
	*/
	status = sscf (ph->name, L"%d", &x);
	if (status == 1)
	{
		ptrcode = compile_pushint (ptrcode, x);
	}
	else
	{
		sprf (mes, L"Undefined word: <%s> %X", ph->name, ph);
	/* 
	sprf (mes, L"Undefined word: <");
	for (i=0; ph->name[i]; i++)
		mes[slen(mes)] = ph->name[i];
		// mes[17+i] = ph->name[i];
	sprf (mes+slen(mes), "> %X", ph);
	// sprf (mes+17+i, "> %X", ph);
	*/
		MessageBox (ghwnd, mes, L"Error", MB_OK);
	}
	return ptrcode;
}

pheader findword (carac *name)
{
	pheader ph;
	char buf[100];
	int i;
	TRACE "begin findword\n" ENDTRACE
	for (i=0; name[i]; i++)
		buf[i]=name[i];
	buf[i]=0;
	TRACE "findword <%s>\n", buf ENDTRACE
	ph = lastword;
	TRACE "loop\n" ENDTRACE
	for (;;)
	{
		// TRACE "ph=%X\n", ph ENDTRACE
		if (ph == NULL)
		{
			TRACE "findword: not found\n" ENDTRACE
			return NULL;
		}
		// TRACE "not null\n" ENDTRACE
		if (!scmp (name, ph->name))
		{
			TRACE "findword: found\n" ENDTRACE
			return ph;
		}
		// TRACE "next\n" ENDTRACE
		ph = ph->link;
	}
	return NULL;
} 

pheader createword (carac *name)
{
	int (*defaultop1) ();
	pheader ph;

	TRACE "createword\n" ENDTRACE
	freedic.ph->link = lastword;

	// freespace.ph->value.f = defaultop;

	TRACE "search UNDEFINED\n" ENDTRACE
	ph = findword (L"UNDEFINED");
	if (ph == NULL)
	{
		freedic.ph->value.f = defaultop;
		freedic.ph->param.ph = freespace.ph;
		freedic.ph->ph = 0;
	}
	else
	{
		freedic.ph->value.f = ph->value.f;
		freedic.ph->param = ph->param;
		freedic.ph->ph = freespace.ph;
	}

	freedic.ph->wmode=MODECP;
	freedic.ph->length = slen(name);
	TRACE "copy name\n" ENDTRACE
	scpy (freedic.ph->name, name);
	TRACE "assign lastword\n" ENDTRACE
	lastword=freedic.ph;
	TRACE "update freedic\n" ENDTRACE
	freedic.pchar += sizeof(struct header) + sizeof(carac) * (slen(name)+1);
	freedic.i = ((freedic.i + 3) / 4) * 4;
	TRACE "end createword\n" ENDTRACE
	return lastword;
}

pheader getword (carac *name)
{
	pheader ph;
	char aname[WORDSIZE];
	int i;
	for (i=0; name[i]; i++)
		aname[i]=name[i];
	aname[i]=0;
	TRACE "getword <%s>\n", aname ENDTRACE 
	ph = findword (name);
	TRACE "ph = %X\n", ph ENDTRACE
	if (ph != NULL)
	{
		TRACE "getword: found\n" ENDTRACE
		return ph;
	}
	TRACE "getword: not found\n" ENDTRACE
	ph = createword (name);
	TRACE "ph=%X\n", ph ENDTRACE
	// TRACE "end getword\n" ENDTRACE
	return ph;
}

pheader getreadword (void)
{
	pheader ph;
	readword();
	ph = getword(wr);
	return ph;
}

void dump (void)
{
	FILE *fdump;
	any ptr;
	int i;
	pheader ph;
	char dumpname[300];
	char asm[300];

	TRACE "begin dump\n" ENDTRACE

	sprintf (dumpname, "%s.dump.txt", exenamea);

	TRACE "dump 2 dumpname=<%s>\n",dumpname ENDTRACE

	// fdump = fopen ("\\Carte de stockage\\pellesc\\forth\\dump.txt", "w");
	fdump = fopen (dumpname, "w");

	TRACE "dump 3 fdump=%X\n", fdump ENDTRACE

	fprintf (fdump, "lastword = %X\n\nDictionary:\n", lastword);

	TRACE "dump 4\n" ENDTRACE

	for (ph=lastword; ph!=NULL; ph=ph->link)
	{
		fprintf (fdump, "%08X: L=%08X V=%08X P=%08X l=%d N=<",
			ph, ph->link, ph->value, ph->param, ph->length);
		
		for (i=0; ph->name[i]; i++)
			fprintf (fdump, "%c", (char)(ph->name[i]));
		fprintf (fdump, ">\n");
		
	}

	TRACE "dump 6\n" ENDTRACE

	fprintf (fdump, "\nDump of dictionary:\n");

	TRACE "dump 7\n" ENDTRACE
	ptr.pi = dic;
	TRACE "dump 8\n" ENDTRACE
	for (;;)
	{
		if (ptr.pcode > freedic.pcode)
			break;
		printasm (asm, *(ptr.pcode), (unsigned int)ptr.pcode);
		fprintf (fdump, "%X: %08X %s\n", ptr.pcode, *(ptr.pcode), asm);
		ptr.pcode++;
	}
	
	fprintf (fdump, "\nDump of dictionary code :\n");

	TRACE "dump 7\n" ENDTRACE
	ptr.pi = diccode;
	TRACE "dump 8\n" ENDTRACE
	for (;;)
	{
		if (ptr.pcode > freespace.pcode)
			break;
		printasm (asm, *(ptr.pcode), (unsigned int)ptr.pcode);
		fprintf (fdump, "%X: %08X %s\n", ptr.pcode, *(ptr.pcode), asm);
		ptr.pcode++;
	}
	TRACE "dump 9\n" ENDTRACE
	fprintf (fdump, "\nCode:\n");
	TRACE "dump 10\n" ENDTRACE
	for (i=0; i<300; i++)
	{
		printasm (asm, codes[0][i], (unsigned int)&(codes[0][i]));
		fprintf (fdump, "%X: %08X %s\n", &(codes[0][i]), codes[0][i], asm);
	}
	TRACE "dump 11\n" ENDTRACE
	/*
	fprintf (fdump, "\n");
	any a;
	for (a.i = 0x1403C; a.i < 0x1405C; a.i++)
	{
		fprintf (fdump, "%X: %08X\n", a.i, *(a.pi));
	}
	*/
	fclose(fdump);
	TRACE "fin dump\n" ENDTRACE
}

void dump1 (void)
{
	FILE *fdump;
	any ptr;
	int i;
	pheader ph;
	char dumpname[300];

	// return;
	// if (aaa == bbb) return;

	sprintf (dumpname, "%s.dump1.txt", exenamea);

	// for (i=0; i<10000000; i++);

	// fdump = NULL;

	// fdump = fopen ("\\Carte de stockage\\pellesc\\forth\\dump1.txt", "w");
	fdump = fopen (dumpname, "w");
/*
	fprintf (fdump, "lastword = %X\n\nDictionary:\n", lastword);
	for (ph=lastword; ph!=NULL; ph=ph->link)
	{
		fprintf (fdump, "%08X: L=%08X V=%08X P=%08X l=%d N=<",
			ph, ph->link, ph->value, ph->param, ph->length);
		
		for (i=0; ph->name[i]; i++)
			fprintf (fdump, "%c", (char)(ph->name[i]));
		fprintf (fdump, ">\n");
		
	}
	fprintf (fdump, "\nDump of dictionary:\n");
	ptr.pi = dic;
	for (;;)
	{
		if (ptr.pcode > freespace.pcode)
			break;
		fprintf (fdump, "%X: %08X\n", ptr.pcode, *(ptr.pcode));
		ptr.pcode++;
	}
	fprintf (fdump, "\nCode:\n");
	for (i=0; i<300; i++)
	{
		fprintf (fdump, "%X: %08X\n", &(codes[0][i]), codes[0][i]);
	}
*/
	fclose(fdump);

}

void dummy (void)
{
	FILE *fdump;
	any ptr;
	int i;
	pheader ph;
	return;
}

void handle_message (void)
{
	int status;
	MSG msg;

	status = PeekMessage (&msg, NULL, 0, 0, PM_REMOVE);
	if (status)
	{
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}
}


dump_asm (FILE *f, instr *begin, instr *end)
{
	char asm[300];
	instr *p;
	fprintf (f, "\n");
	for (p=begin; p<=end; p++)
	{
		printasm (asm, *p, (unsigned int)p);
		fprintf (f, "%X: %08X %s\n", p, *p, asm);
	}
}

void paint (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

char buf[100];

HWND testcreatewindow (void)
{
	return CreateWindow ((LPSTR)0x1111, (LPSTR)0x2222, 0x3333, 
		0x4444, 0x5555, 0x6666, 0x7777, 
		(HWND)0x8888, (HMENU)0x9999, (HANDLE)0xAAAA, (LPVOID)0xBBBB);
}

double scandouble (void)
{
	double x;
	sscanf ("123.456", "%lf", &x);
	return x;
}

void printdouble (double x)
{
	sprintf (buf, "%lf", x);
}

double testdouble (double x, double y)
{
	return x + (2.0 * y);
}

instr * testcompil (instr *imcodeptr, pheader ph)
{
	imcodeptr = (*(ph->value.f)) (imcodeptr, ph->param, ph);
	return imcodeptr;
}

int testasm (int x, int y, int z)
{
	return x+y;
}

int accu = 0;

int endvar;

int incr (int x)
{
	accu += x;
	return accu;
}

void endincr (void) { }

int testweb (void)
{
	WORD version;
	WSADATA wsadata;
	int status;
	SOCKET s;
	int port;
	struct hostent *host;
	struct in_addr addr;
	struct sockaddr_in sa;
	int i;
	char bufs[1000];
	char bufr[1000];
	wchar_t bufm[1000];

	version = 0x101;
	status = WSAStartup (version, &wsadata);
	s = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
	port = htons (80);
	host = gethostbyname ("www.yahoo.fr");
	i = 0;
	while (host->h_addr_list[i] != 0) 
	{
            addr.s_addr = *(u_long *) host->h_addr_list[i++];
	}
	memset (&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = port;
	sa.sin_addr = addr;
	status = connect (s, &sa, sizeof(sa));

	sprintf (bufs, "GET / HTTP/1.0\n\n");
	status = send (s, bufs, strlen(bufs), 0);
	status = recv (s, bufr, sizeof(bufr), 0);
	for (i=0; i<1000; i++)
		bufm[i] = bufr[i];
	MessageBox (0, bufm, L"received", MB_OK);

	closesocket(s);

}

void endtestweb (void) { }


/*
extern LRESULT WndProc();
extern LRESULT About();
*/


	pheader ph;
	//carac buf[1000];
	int i, k;
	char inname[300], outname[300], tracename[300];

#define FLAG_IM1 2
#define FLAG_DUMP 4

int __fastcall forth_main (	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	// FILE *infd, *outfd;
	
	/*
	ghInstance = hInstance;
	ghPrevInstance = hPrevInstance;
	glpCmdLine = lpCmdLine;
	gnCmdShow = nCmdShow;
	*/

	// gPaint = paint;
/*
	{
		wchar_t buf[1000];
		swprintf (buf, L"sizeof(DWORD)=%d", sizeof(DWORD));
		MessageBox (NULL, buf, L"message", MB_OK);
	}
*/	
	nl = 0;
	flag_locals = 0;

	rb.pi = bufcode_rb;
	bufcode_rb[0] = 0xE1A0000B;
	bufcode_rb[1] = 0xE1A0F00E;

	exename[0] = 0;
	GetModuleFileName (NULL, exename, sizeof(exename));
	for (i=0; exename[i]; i++)
		exenamea[i] = exename[i];
	exenamea[i] = 0;

	sprintf (inname, "%s.4th.txt", exenamea);
	sprintf (outname, "%s.out.txt", exenamea);
	sprintf (tracename, "%s.trace.txt", exenamea);

	// ftrace = fopen ("\\Carte de stockage\\pellesc\\forth\\trace.txt", "w");
	ftrace = fopen (tracename, "w");
	TRACE "begin forth\n" ENDTRACE

	init_forth();

	DA(&accu)
	dumpcode (ftrace, "incr", incr, endincr);
	// dumpvar (ftrace, "&accu", sizeof(accu));
	dumpdata (ftrace, "&accu", &accu, &endvar);

	
	dump_asm (ftrace, (instr *)incr, (instr *)endincr);

	dumpcode (ftrace, "testweb", testweb, endtestweb);
	dump_asm (ftrace, (instr *)testweb, (instr *)endtestweb);
	

	/*
	DA(&hInst)
	DA(&hwndCB)
	dumpcode (ftrace, "WinMain", WinMain, MyRegisterClass);
	dumpcode (ftrace, "MyRegisterClass", MyRegisterClass, InitInstance);
	dumpcode (ftrace, "InitInstance", InitInstance, endfunctions);
	*/

	// dump_asm (ftrace, (instr *)WndProc, (instr *)About);
	dump_asm (ftrace, (instr *)paint, (instr *)testcreatewindow);
	dump_asm (ftrace, (instr *)testcreatewindow, (instr *)scandouble);
	dump_asm (ftrace, (instr *)scandouble, (int *)printdouble);
	dump_asm (ftrace, (instr *)printdouble, (instr *)testdouble);
	dump_asm (ftrace, (instr *)testdouble, (instr *)testcompil);
	dump_asm (ftrace, (instr *)testasm, (instr *)forth_main);
	dump_asm (ftrace, (instr *)code_pushint, (instr *)code_phcalled);
	dump_asm (ftrace, (instr *)testcompil, (instr *)testasm);

	TRACE "Open files\n" ENDTRACE
	// infd = fopen ("\\Carte de stockage\\pellesc\\forth\\in.txt", "r");
	infd = fopen (inname, "r");
	instack[0].f = readcaracfile;
	instack[0].p.fd = infd;
	// outfd = fopen ("\\Carte de stockage\\pellesc\\forth\\out.txt", "w");
	outfd = fopen (outname, "w");
	out.f = writecaracfile;
	out.p.fd = outfd;
	
	// init_forth();

	// writestring (L"Test output\n");
	
	TRACE "\n*************************************** Main loop ***********************************\n\n" ENDTRACE 

	dump();
	mainloop (FLAG_DUMP | FLAG_IM1, L"");
}

void mainloop (int flags, wchar_t *ex)
{
	int modeprec;
	// MessageBox (0, ex, L"ex", MB_OK);
	swprintf (bufm, L"ex=<%s>", ex);
	// MessageBox (0, bufm, L"mainloop", MB_OK);
	TRACE2 "mainloop ex=%X\n", ex ENDTRACE2
	modeprec = MODECP;
	for (i=1; ;i++)
	{
		TRACE "Begin main loop\n" ENDTRACE
		readword();
		/**/ // MessageBox (NULL, wr, L"wr", MB_OK);
		TRACE2 "after readword wr=%X <%s>\n", wr, awr ENDTRACE2
		if (!*wr)
			break;
		if (ex)
		{
			if (!scmp (wr, ex))
			{
				// MessageBox (0, L"break", L"mainloop", MB_OK);
				break;
			}
		}
		TRACE "before getword\n" ENDTRACE
		ph = getword(wr);
		TRACE "after getword <%s> ph=%X\n", awr, ph ENDTRACE
		// sprf (buf, L"Mot lu: <%s>\n", wr);
		// writestring (buf);
		
		// if ((i % 4) == 0) 
		
	 	// if (flags & FLAG_DUMP) dump1();
		
		// dump();
		// dummy();
		TRACE "handle message\n" ENDTRACE
		handle_message();

		TRACE "test mode\n" ENDTRACE
		if ((mode & MODEIM) || (ph->wmode & MODEIM)) // immediate <=0
		{
			TRACE "immediate\n" ENDTRACE
			if ((modeprec != MODEIM) || (flags & FLAG_IM1))
			{
				imcodeptr = imcode.pcode;
				TRACE "1 imcode=%X imcodeptr=%X\n", imcode.pcode, imcodeptr ENDTRACE
				imcodeptr = instr_begin (imcodeptr);
			}
			TRACE "2 imcode=%X imcodeptr=%X\n", imcode.pcode, imcodeptr ENDTRACE
			TRACE "compile <%s> f=%X imcodeptr=%X param=%X ph=%X\n", 
				awr, ph->value.f, imcodeptr, ph->param.i, ph ENDTRACE
			imcodeptr = (*(ph->value.f)) (imcodeptr, ph->param, ph);
			TRACE "3 imcode=%X imcodeptr=%X\n", imcode.pcode, imcodeptr ENDTRACE
			if ((flags & FLAG_IM1) || (ph->wmode & MODEIM1))
			{
				imcodeptr = instr_end (imcodeptr);
				TRACE "4 imcode=%X imcodeptr=%X\n", imcode.pcode, imcodeptr ENDTRACE
				dump_asm (ftrace, imcode.pcode, imcodeptr+1);
				TRACE "call code of <%s>\n", awr ENDTRACE
				// if (mode <= 0)
				//	(*(imcode.f)) ();
				// else
				//	freespace.pcode = (*(imcode.f)) (freespace.pcode, ph->param, ph);
				TRACE2 "before call code of <%s> immediate mode\n", awr ENDTRACE2
				(*(imcode.f)) ();
				TRACE2 " after call code of <%s> immediate mode\n", awr ENDTRACE2
			}
			TRACE "returned from code of <%s>\n", awr ENDTRACE
			modeprec = MODEIM;
		}
		else // compilation
		{
			if ((modeprec == MODEIM) && !((flags & FLAG_IM1) || 
				(ph->wmode & MODEIM1)))
			{
				imcodeptr = instr_end (imcodeptr);
				TRACE "4 imcode=%X imcodeptr=%X\n", imcode.pcode, imcodeptr ENDTRACE
				// dump_asm (ftrace, imcode.pcode, imcodeptr+1);
				TRACE "call code of <%s>\n", awr ENDTRACE
				// if (mode <= 0)
				//	(*(imcode.f)) ();
				// else
				//	freespace.pcode = (*(imcode.f)) (freespace.pcode, ph->param, ph);
				TRACE2 "before call code of <%s> compile mode\n", awr ENDTRACE2
				(*(imcode.f)) ();
				TRACE2 " after call code of <%s> compile mode\n", awr ENDTRACE2
				
			}
			TRACE "compile <%s> ph=%X f=%X param=%X freespace=%X\n", awr, ph, ph->value.f, ph->param, freespace ENDTRACE
			freespace.pcode = (*(ph->value.f)) (freespace.pcode, ph->param, ph);
			TRACE "returned from code call\n" ENDTRACE
			modeprec = MODECP;
		}
		TRACE "end loop\n" ENDTRACE

		
#if 0
		if (mode <= 0 || ph->wmode <= 0)
			code_levelup ();
		/*
		if (ph != word_phcalled && ph != word_def)
			phcalled = ph;
		*/
		TRACE "Call code of <%s>\n", awr ENDTRACE
		 (*(ph->value.f)) (ph->param, ph->ph);
		TRACE "returned\n" ENDTRACE
		if (mode<=0 || ph->wmode <= 0)
		{
			TRACE "leveldown\n" ENDTRACE
			code_leveldown();
			// code_doit();
		}
#endif
		TRACE "End main loop\n" ENDTRACE
	}
	TRACE "exited\n" ENDTRACE
			if ((modeprec == MODEIM) && !((flags & FLAG_IM1) || 
				(ph->wmode & MODEIM1)))
			{
				imcodeptr = instr_end (imcodeptr);
				TRACE "4 imcode=%X imcodeptr=%X\n", imcode.pcode, imcodeptr ENDTRACE
				// dump_asm (ftrace, imcode.pcode, imcodeptr+1);
				TRACE "call code of <%s>\n", awr ENDTRACE
				// if (mode <= 0)
				//	(*(imcode.f)) ();
				// else
				//	freespace.pcode = (*(imcode.f)) (freespace.pcode, ph->param, ph);
				(*(imcode.f)) ();
			}
	dump ();
}

/*
int forth (carac *inptr1, carac *outptr1)
{
	int i;
	pheader ph;
	inptrs[inlevel++] = inptr;
	inptr = inptr1;
	outptr = outptr1;
	for (;;)
	{
		while (*inptr == ' ' && *inptr)
			inptr++;
		if (*inptr == 0)
		{
			*outptr = 0;
			break;
		}
		i = 0;
		for (;;)
		{
			if (*inptr == ' ' || *inptr == 0)
			{
				word[i] = 0;
				cword[i] = 0;
				while (*inptr == ' ' && *inptr)
					inptr++;
				break;
			}
			word[i] = *inptr;
			cword[i] = *inptr;
			i++;
			inptr++;
		}
		outptr += sprf (outptr, L"Treating <%s>\r\n", word);
		ph = getword (word);
		(*(ph->value.f)) (ph->param);
	}
	if (inlevel > 0)
	{
		inlevel--;
		inptr = inptrs[inlevel];
	}
}
*/

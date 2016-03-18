
/* utilitaires */

#include <windows.h>
#include <windowsx.h>
// #include <aygshell.h>
// #include "main.h"

extern HWND ghwnd;

char buftrace[1000];
wchar_t wbuftrace[1000];
int itrace;

#define TRACE sprintf (buftrace, 
#define ENDTRACE ); \
	for (itrace=0; buftrace[itrace]; itrace++) \
		wbuftrace[itrace]=buftrace[itrace]; \
	wbuftrace[itrace] = 0; \
	MessageBox (ghwnd, wbuftrace, L"Trace", MB_OK); 

typedef wchar_t carac;

void scpy (carac *dst, carac *src)
{
	while (*src)
		*dst++ = *src++;
	*dst = 0;
}

int scmp (carac *a, carac *b)
{
	int i;
	i = 0;
	for (;;)
	{
		// TRACE "scmp <%s> <%s> %d %X %X\n", a, b, i, a[i], b[i]  ENDTRACE
		if (a[i] == 0)
		{
			if (b[i] == 0)
				return 0;
			return 1;
		}
		else if (a[i] == b[i])
			i++;
		else return 1;
	}
}

int slen (carac *s)
{
	int i;
	for (i=0; s[i]; i++);
	return i;
}

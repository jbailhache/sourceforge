
#define GBLVARS
#include "cvm.h"

#if 0
/* #define PPC */
#define INTERP
/* #define COMPARM */
/* #define COMPX86 */

#ifndef PPC
#include <stdio.h>
#endif

#include <stdlib.h>
#include <string.h>

void printasm (char *s, unsigned int x, unsigned int adr);
void debug (void);

#define NLABELS 0x100
#define LEN 0x20

struct labels
{
	char name[LEN];
	unsigned int value;
} labels[NLABELS];


typedef unsigned char chr;
typedef char byte;

#ifdef INTERP
typedef long instr;
#endif

#ifdef COMPARM
typedef int instr;
#endif

#ifdef COMPX86
typedef unsigned char instr;
#endif

typedef instr *codeptr;

#ifdef CPLUSPLUS
typedef codeptr (*codefn) (codeptr, union any);
#else
typedef codeptr (*codefn)();
#endif

typedef union any
{
 long i;
 instr ins;
 long *p;
 codeptr cp;
 codefn f;
 void (*vf) (void);
 int (*intf)();
 struct word *pw;
 char *pchar;
} any;

instr cod[4000];

codeptr lab[256];

int nlab = 0;
struct label
{
	char name[LEN];
	any value;
} label[NLABELS];

struct word
{
 struct word *link;
 any code;
 any param;
 chr *pname;
 chr name[4];
};

typedef struct word *pword;


pword lastword = NULL;

#define DICSIZE 1000
int dic[DICSIZE];

any freedic;
any freespace;

pword gpw;
int gwri;

codeptr c_mov_ra_im (codeptr cp, int im, char *l);
codeptr c_mov_r0_cont_ra (codeptr cp);
codeptr c_mov_ra_cont_ra (codeptr cp);

#endif

void init_dic (void)
{
 freedic.pw = (pword)dic;
}

chr getchr1 ()
{
 chr c;
 c = getchar();
 /* lastcp = instbl+c; */
 return c;
}

/*
chr (*pgetchr) (void) = getchr1;
#define getchr (*pgetchr)
*/

void putchr (chr c)
{
 putchar(c);
}

void printdec (int i)
{
 printf (" %d ", i);
}

void printhex (int i)
{
 printf (" %X ", i);
}

/*
#define WORDSIZE 32

chr cr;
chr wr[WORDSIZE];
char awr[WORDSIZE];
*/

chr readcarac (void)
{
 cr = getchr();
 return cr;
}

int isblnk (chr c)
{
	if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
		return 1;
	else
		return 0;
}

int iswordcarac (chr c)
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
	/* TRACE "readword: 1st cr = '%c' %X\n", cr, cr ENDTRACE */
	while (isblnk (cr))
	{
		readcarac();
		/* TRACE "skip blanks: cr = '%c' %X\n", cr, cr ENDTRACE */
	}
	i = 0;
	while (iswordcarac(cr))
	{
		/* TRACE "read cr = '%c' %X\n", cr, cr ENDTRACE */
		wr[i] = cr;
		awr[i] = cr;
		/* TRACE "awr = <%s>\n", awr ENDTRACE */
		i++;
		readcarac();
	}
	wr[i] = 0;
	awr[i] = 0;
	sscanf (awr, "%x", &gwri);
	/* TRACE "Word read: <%s>\n", awr ENDTRACE */
}

void testreadword (void)
{
 for (;;)
 {
  readword();
  printf (" <%s> ", wr);
  if (!strcmp((char*)wr,"BYE"))
   break;
 }
}

int slen (chr *s)
{
 return strlen((char *)s);
}

int scmp (chr *s1, chr *s2)
{
 return strcmp ((char*)s1, (char*)s2);
}

chr * scpy (chr *s1, chr *s2)
{
 return (chr*) strcpy ((char*)s1, (char*)s2);
}

codeptr compile_pushint (codeptr cp, int x);

/*
instr *code_undef (instr *ptrcode, any p)
{
int i;
 i = 0;
 sscanf ((char *)p.s, "%d", &i);
 if (i == 0)
 {
  i = 1;
  sscanf ((char *)p.s, "%d", i);
  if (i == 1)
  {
   printf ("Undefined word: <%s>\n", (char *)p.s);
   return ptrcode;
  }
  else
  {
   ptrcode = compile_pushint (ptrcode, i);
   return ptrcode;
  }
 }
 else
 {
  ptrcode = compile_pushint (ptrcode, i);
  return ptrcode;
 }
}
*/

pword findword (chr *name)
{
	pword pw;
	char buf[100];
	int i;
	/* TRACE "begin findword\n" ENDTRACE */
	for (i=0; name[i]; i++)
		buf[i]=name[i];
	buf[i]=0;
	/* TRACE "findword <%s>\n", buf ENDTRACE */
	pw = lastword;
	/* TRACE "loop\n" ENDTRACE */
	for (;;)
	{
		/* TRACE "pw=%X\n", pw ENDTRACE */
		if (pw == NULL)
		{
			/* TRACE "findword: not found\n" ENDTRACE */
			return NULL;
		}
		/* TRACE "not null\n" ENDTRACE */
		if (!scmp (name, pw->pname))
		{
			/* TRACE "findword: found\n" ENDTRACE */
			return pw;
		}
		/* TRACE "next\n" ENDTRACE */
		pw = pw->link;
	}
	return NULL;
}

pword createword (chr *name)
{
	int (*defaultop1) ();
	pword pw;

	freedic.pw->link = lastword;

	pw = findword ((chr *)"UNDEFINED");
	if (pw == NULL)
	{
		freedic.pw->code.f = NULL;
		freedic.pw->param.pw = freespace.pw;
		/* freedic.pw->pw = 0; */
	}
	else
	{
		freedic.pw->code.f = pw->code.f;
		freedic.pw->param = pw->param;
		/* freedic.pw->pw = freespace.pw; */
	}

	/* freedic.ph->wmode=MODECP; */
	/* freedic.ph->length = slen(name); */
	freedic.pw->pname = freedic.pw->name;
	scpy (freedic.pw->name, name);
	lastword=freedic.pw;
	freedic.pchar += sizeof(struct word) + sizeof(chr) * (slen(name)+1);
	freedic.i = ((freedic.i + 3) / 4) * 4;
	return lastword;
}


pword getwordstr (chr *name)
{
	pword pw;
	char aname[WORDSIZE];
	int i;
	for (i=0; name[i]; i++)
		aname[i]=name[i];
	aname[i]=0;
	/* TRACE "getword <%s>\n", aname ENDTRACE */
	pw = findword (name);
	/* TRACE "pw = %X\n", pw ENDTRACE */
	if (pw != NULL)
	{
		/* TRACE "getword: found\n" ENDTRACE */
		return pw;
	}
	/* TRACE "getword: not found\n" ENDTRACE */
	pw = createword (name);
	/* TRACE "pw=%X\n", pw ENDTRACE */
	/* TRACE "end getword\n" ENDTRACE */
        gpw = pw;
	return pw;
}

/*
pheader getreadword (void)
{
	pheader pw;
	readword();
	pw = getword(wr);
	return pw;
}
*/

pword getword (void)
{
 readword();
 return getwordstr(wr);
}


#define code_mov_r0_im 0x1000F
#define code_q_mov_r0_imr0 0x101000F
#define code_mov_r1_im 0x1010F
#define code_q_mov_r1_imr0 0x101010F
#define code_mov_ra_im 0x10A0F
#define code_q_mov_ra_imr0 0x1010A0F
#define code_mov_r_r 0x10000
#define code_mov_r1_r0 0x10100
#define code_q_mov_r1_r0 0x1010100
#define code_mov_r2_r0 0x10200
#define code_mov_r0_r2 0x10002
#define code_mov_r1_r2 0x10102
#define code_mov_ra_r0 0x10A00
#define code_mov_rb_r0 0x10B00
#define code_mov_rc_r0 0x10C00
#define code_mov_r0_ra 0x1000A
#define code_mov_r0_rb 0x1000B
#define code_mov_r0_rc 0x1000C
#define code_mov_ra_r1 0x10A01
#define code_mov_rc_r1 0x10C01
#define code_mov_rc_ra 0x10C0A
#define code_mov_r1_rc 0x1010C
#define code_mov_r0_cont_ra 0x1001A
#define code_mov_ra_cont_ra 0x10A1A
#define code_mov_cont_ra_r0 0x11A00
#define code_mov_r0_cont_ra_plus_im 0x1009A
#define code_mov_cont_ra_plus_im_r0 0x19A00
#define code_mov_r0_cont_rb_inc 0x1005B
#define code_mov_cont_dec_rb_r0 0x13B00
#define code_mov_r1_cont_rb_inc 0x1015B
#define code_mov_ra_cont_rb_inc 0x10A5B
#define code_mov_cont_dec_rb_r1 0x13B01
#define code_mov_cont_rc_inc_r0 0x15C00
#define code_add_r0_r1 0x20001
#define code_sub_r0_r1 0x30001
#define code_bp_depl 0xC2FFF
#define code_bz_depl 0xC3FFF
#define code_bp_ra 0xC0AFF
#define code_call_depl 0xD2FFF
#define code_call_ra 0xD0AFF
#define code_beginf 0xE0001
#define code_endf 0xE0002
#define code_ret 0xEFFFF
#define code_ror_r0 0x600FF
#define code_asr_r0 0x700FF
#define code_and_r0_r1 0x80001
#define code_xor_r0_r1 0x90001
#define code_fillmem 0xE0000
#define code_callext 0xFFF0F
#define code_getchar 0x1000FF
#define code_putchar 0x11FF00
#define code_scanhex 0x1200FF
#define code_getword 0x1300FF
#define code_setbradr 0x140000
#define code_exit 0xFFFFFF

/*
FILE *f_tasm = NULL;
FILE *f_asmi = NULL;
FILE *f_asml = NULL;
FILE *f_x86 = NULL;
*/

/* char last_label; */
/* char last_label[200]; */

codeptr c_push_r0 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " push ax\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " push %%ax\n");
 if (f_asml != NULL)
  fprintf (f_asml, " push %%ax\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x50\n");
#ifdef COMPARM

#endif
#ifdef COMPX86
 *cp++ = 0x50;
#endif
 return cp;
}

codeptr c_pop_r0 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " pop ax\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " pop %%ax\n");
 if (f_asml != NULL)
  fprintf (f_asml, " pop %%ax\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x58\n");
#ifdef COMPARM

#endif
#ifdef COMPX86
 *cp++ = 0x58;
#endif
 return cp;
}

codeptr c_push_r1 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " push bx\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " push %%bx\n");
 if (f_asml != NULL)
  fprintf (f_asml, " push %%bx\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x53\n");
#ifdef COMPARM

#endif
#ifdef COMPX86
 *cp++ = 0x53;
#endif
 return cp;
}

codeptr c_pop_r1 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " pop bx\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " pop %%bx\n");
 if (f_asml != NULL)
  fprintf (f_asml, " pop %%bx\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x5B\n");
#ifdef COMPARM

#endif
#ifdef COMPX86
 *cp++ = 0x5B;
#endif
 return cp;
}

codeptr c_pop_r2 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " pop cx\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " pop %%cx\n");
 if (f_asml != NULL)
  fprintf (f_asml, " pop %%cx\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x59\n");
#ifdef COMPARM

#endif
#ifdef COMPX86
 *cp++ = 0x59;
#endif
 return cp;
}

#ifdef INTERP

void setbradr (instr *from, instr *to)
{
 int depl;
 depl = to - (from+2);
 from[1] = depl;
}

codeptr compile_call0 (codeptr cp, int (*f)())
{
 *cp++ = code_callext;
 *cp++ = (instr)f;
 return cp;
}

codeptr compile_call (codeptr cp, int (*f)(), int regs, int res)
{
 compile_call0 (cp, f);
}

#endif

#ifdef COMPARM

void setbradr (instr *from, instr *to)
{
 int ofs;
 ofs = to - (from + 3);
 printf("from=%X to=%X ofs=%X from[1]=%X\n",
  from,to,ofs,from[1]);
 /* *from |= ofs; */
 /* *from = (*from & 0xFF000000) | (ofs & 0xFFFFFF); */
 from[1] = (from[1] & 0xFF000000) | (ofs & 0xFFFFFF);
 printf("from[1]=%X\n",from[1]);
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
	return 0xEB000000 | ofs;
}

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

instr *compile_call0 (instr *ptrcode, int (*f)())
{
 return compile_call (ptrcode, f, 0, 0);
}


#endif

#ifdef COMPX86

void setbradr (instr *from, instr *to)
{
 int ofs;
/*
 ofs = to - (from + 4);
 from[2] = ofs & 0xFF;;
 from[3] = (ofs & 0xFF00) >> 8;
*/
 ofs = to - (from + 5);
 printf ("from=%X to=%X ofs=%X\n", from, to, ofs);
 from[4] = ofs;
}

codeptr compile_call0 (codeptr cp, int (*f)())
{
 any a;
 int d;
 a.intf = f;
 d=a.cp-(cp+3);
 *cp++ = 0xE8;
 *cp++ = d & 0xFF;
 *cp++ = (d & 0xFF00) >> 8;
 return cp;
}

codeptr compile_call (codeptr ptrcode, int (*f)(), int regs, int res)
{
 return compile_call0 (ptrcode, f);
}

#endif

int f_getchar (void)
{
 return getchar();
}

void f_putchar (int c)
{
 putchar (c);
 /* printf(" %c-%X ",c,c); */
}

void f_print (int x)
{
 printf(" %X ",x);
}

int f_scanhex (void)
{
 int x;
 x = 0;
 scanf ("%X", &x);
 return x;
}

#ifdef INTERP

codeptr c_getchar (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov ah,1\n int 21h\n mov ah,0\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " call getchar\n mov %%ah, 0\n");
 if (f_asml != NULL)
  fprintf (f_asml, " call getchar\n mov $0,%%ah\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " call getchar\n .byte 0xB4, 0\n");
 *cp++ = code_getchar;
 return cp;
}

codeptr c_putchar (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov dx,ax\n mov ah,2\n int 21h\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " mov DWORD PTR [%%esp], %%eax\n call putchar\n");
 if (f_asml != NULL)
  fprintf (f_asml, " movl %%eax,(%%esp)\n call putchar\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " movl %%eax,(%%esp)\n call putchar\n");
 *cp++ = code_putchar;
 return cp;
}

#else
codeptr c_getchar (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov ah,1\n int 21h\n mov ah,0\n");
 if (f_asml != NULL)
  fprintf (f_asml, " call getchar\n mov $0,%%ah\n");
 cp = compile_call (cp, (int(*)())f_getchar, 0, 0);
 return cp;
}

codeptr c_putchar (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov dx,ax\n mov ah,2\n int 21h\n");
 if (f_asml != NULL)
  fprintf (f_asml, " movl %%eax,(%%esp)\n call putchar\n");
#ifdef COMPX86
 cp = c_push_r0 (cp);
#endif
 cp = compile_call (cp, (int(*)())f_putchar, 0, 0);
#ifdef COMPX86
 cp = c_pop_r2 (cp);
#endif
 return cp;
}

codeptr c_print (codeptr cp)
{
 cp = compile_call (cp, (int(*)())f_print,0,0);
 return cp;
}

#endif

codeptr c_getword (codeptr cp)
{
 if (f_tasm != NULL)
  /* fprintf (f_tasm, " call getword\n mov si,[gpw]\n"); */
  fprintf (f_tasm, " call _getword\n");
#ifdef INTERP
 *cp++ = code_getword;
#else
 cp = compile_call (cp, (int(*)())getword, 0, 0);
 cp = c_mov_ra_im (cp, (int)&gwri, "");
 cp = c_mov_r0_cont_ra (cp);
 cp = c_mov_ra_im (cp, (int)&gpw, "");
 cp = c_mov_ra_cont_ra (cp);
#endif
 return cp;
}

codeptr c_scanhex (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " call _scanhex\n");
#ifdef INTERP
 *cp++ = code_scanhex;
#else
 cp = compile_call0 (cp, (int(*)())f_scanhex);
#endif
 return cp;
}

codeptr c_mov_r0_im (codeptr cp, int im, char *l)
{
 if (f_tasm != NULL)
 {
  if (l == 0 || *l == 0)
   fprintf (f_tasm, " mov ax,%Xh\n", im);
  else
   fprintf (f_tasm, " lea ax,[%s]\n", l);
 }
 if (f_asmi != NULL)
 {
  if (*l == 0)
   fprintf (f_asmi, " mov %%ax,0x%X\n", im);
  else
   fprintf (f_asmi, " lea %%ax,[%s]\n", l);
 }
 if (f_asml != NULL)
 {
  if (*l == 0)
   fprintf (f_asml, " mov $0x%X,%%ax\n", im);
  else
   fprintf (f_asml, " lea (%s),%%ax\n", l);
 }
 if (f_x86 != NULL)
 {
  if (*l == 0)
   fprintf (f_x86, " .byte 0xB8, 0x%X, 0x%X\n", im & 0xFF, (im & 0xFF00) >> 8);
  else
  {
   int d;
   /* d = lab[l] - cod; */
   /* fprintf (f_x86, " .byte 0xB8, 0x%X, 0x%X\n", d & 0xFF, (d & 0xFF00) >> 8); */
   fprintf (f_x86, " .byte 0xB8\n .word %s-main\n", l);
   /* fprintf (f_x86, " .byte 0xB8, %c, 0\n", l); */
  }
 }
#ifdef INTERP
 *cp++ = code_mov_r0_im;
 *cp++ = im;
#endif
#ifdef COMPARM
 *cp++ = 0xE59F0000;
 *cp++ = 0xEA000000;
 *cp++ = im;
#endif
#ifdef COMPX86
 *cp++ = 0xB8;
 *cp++ = im & 0xFF;
 *cp++ = (im & 0xFF00) >> 8;
#endif
 return cp;
}

codeptr c_mov_r1_im (codeptr cp, int im, char *l)
{
 if (f_tasm != NULL)
 {
  if (*l == 0)
   fprintf (f_tasm, " mov dx,%Xh\n", im);
  else
   fprintf (f_tasm, " lea dx,[%s]\n", l);
 }
 if (f_asmi != NULL)
 {
  if (*l == 0)
   fprintf (f_asmi, " mov %%dx,0x%X\n", im);
  else
   fprintf (f_asmi, " lea %%dx,[%s]\n", l);
 }
 if (f_asml != NULL)
 {
  if (*l == 0)
   fprintf (f_asml, " mov $0x%X,%%dx\n", im);
  else
   fprintf (f_asml, " lea (%s),%%dx\n", l);
 }
 if (f_x86 != NULL)
 {
  if (*l == 0)
   fprintf (f_x86, " .byte 0xBA, 0x%X, 0x%X\n", im & 0xFF, (im & 0xFF00) >> 8);
  else
  {
   int d;
   /* d = lab[l] - cod; */
   /* fprintf (f_x86, " .byte 0xBB, 0x%X, 0x%X\n", d & 0xFF, (d & 0xFF00) >> 8); */
   fprintf (f_x86, " .byte 0xBA\n .word %s-main\n", l);
   /* fprintf (f_x86, " .byte 0xBB, %c, 0\n", l); */
  }
 }
#ifdef INTERP
 *cp++ = code_mov_r1_im;
 *cp++ = im;
#endif
#ifdef COMPARM
 *cp++ = 0xE59F1000;
 *cp++ = 0xEA000000;
 *cp++ = im;
#endif
#ifdef COMPX86
 *cp++ = 0xBA;
 *cp++ = im & 0xFF;
 *cp++ = (im & 0xFF00) >> 8;
#endif
 return cp;
}

codeptr c_mov_ra_im (codeptr cp, int im, char *l)
{
 printf("c_mov_ra_im\n");
 if (f_tasm != NULL)
 {
  if (*l == 0)
   fprintf (f_tasm, " mov si,%Xh\n", im);
  else
   fprintf (f_tasm, " lea si,[%s]\n", l);
 }
 if (f_asmi != NULL)
 {
  if (*l == 0)
   fprintf (f_asmi, " mov %%si,0x%X\n", im);
  else
   fprintf (f_asmi, " lea %%si,[%s]\n", l);
 }
 if (f_asml != NULL)
 {
  if (*l == 0)
   fprintf (f_asml, " mov $0x%X,%%si\n", im);
  else
   fprintf (f_asml, " lea (%s),%%dx\n", l);
 }
 if (f_x86 != NULL)
 {
  if (*l == 0)
   fprintf (f_x86, " .byte 0xBE, 0x%X, 0x%X\n", im & 0xFF, (im & 0xFF00) >> 8);
  else
  {
   int d;
   /* d = lab[l] - cod; */
   /* fprintf (f_x86, " .byte 0xBB, 0x%X, 0x%X\n", d & 0xFF, (d & 0xFF00) >> 8); */
   fprintf (f_x86, " .byte 0xBE\n .word %s-main\n", l);
   /* fprintf (f_x86, " .byte 0xBB, %c, 0\n", l); */
  }
 }
 printf("end write files\n");
#ifdef INTERP
 printf("interp 1 cp=%X\n",cp);
 *cp++ = code_mov_ra_im;
 printf("interp 2\n");
 *cp++ = im;
 printf("interp 3\n");
#endif
#ifdef COMPARM
 printf("comparm 1 cp=%X\n",cp);
 *cp++ = 0xE59FA000;
 printf("comparm 2\n");
 *cp++ = 0xEA000000;
 printf("comparm 3\n");
 *cp++ = im;
 printf("comparm 4\n");
#endif
#ifdef COMPX86
 *cp++ = 0xBE;
 *cp++ = im & 0xFF;
 *cp++ = (im & 0xFF00) >> 8;
#endif
 printf("end c_mov_ra_im\n");
 return cp;
}

codeptr c_q_mov_r0_imr0 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov cl,0B8h\n mov [di],cl\n inc di\n mov [di],al\n inc di\n mov[di],ah\n inc di\n");
#ifdef INTERP
 *cp++ = code_q_mov_r0_imr0;
#endif
#ifdef COMPARM
 *cp++ = 0xE59F9000; /* R9=[RF] */
 *cp++ = 0xEA000000; /* GOTO RF */
 *cp++ = 0xE59F0000; /* R0=[RF] */
 *cp++ = 0xE58C9000; /* [RC]=R9 */
 *cp++ = 0xE28CC004; /* RC=RC+4 */
 *cp++ = 0xE59F9000; /* R9=[RF] */
 *cp++ = 0xEA000000; /* GOTO RF */
 *cp++ = 0xEA000000; /* GOTO RF */
 *cp++ = 0xE58C9000; /* [RC]=R9 */
 *cp++ = 0xE28CC004; /* RC=RC+4 */
 *cp++ = 0xE58C0000; /* [RC]=R0 */
 *cp++ = 0xE28CC004; /* RC=RC+4 */
#endif
#ifdef COMPX86
 *cp++ = 0xB1; *cp++ = 0xB8; /* MOV CL,B8 */
 *cp++ = 0x88; *cp++ = 0x0D; /* MOV [DI],CL */
 *cp++ = 0x47; /* INC DI */
 *cp++ = 0x88; *cp++ = 0x05; /* MOV [DI],AL */
 *cp++ = 0x47; /* INC DI */
 *cp++ = 0x88; *cp++ = 0x25; /* MOV [DI],AH */
 *cp++ = 0x47; /* INC DI */
#endif
 return cp;
}

codeptr c_q_mov_r1_imr0 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov cl,0BAh\n mov [di],cl\n inc di\n mov [di],al\n inc di\n mov[di],ah\n inc di\n");
#ifdef INTERP
 *cp++ = code_q_mov_r1_imr0;
#endif
#ifdef COMPARM
 *cp++ = 0xE59F9000; /* R9=[RF] */
 *cp++ = 0xEA000000; /* GOTO RF */
 *cp++ = 0xE59F1000; /* R1=[RF] */
 *cp++ = 0xE58C9000; /* [RC]=R9 */
 *cp++ = 0xE28CC004; /* RC=RC+4 */
 *cp++ = 0xE59F9000; /* R9=[RF] */
 *cp++ = 0xEA000000; /* GOTO RF */
 *cp++ = 0xEA000000; /* GOTO RF */
 *cp++ = 0xE58C9000; /* [RC]=R9 */
 *cp++ = 0xE28CC004; /* RC=RC+4 */
 *cp++ = 0xE58C0000; /* [RC]=R0 */
 *cp++ = 0xE28CC004; /* RC=RC+4 */
#endif
#ifdef COMPX86
 *cp++ = 0xB1; *cp++ = 0xBA; /* MOV CL,BA */
 *cp++ = 0x88; *cp++ = 0x0D; /* MOV [DI],CL */
 *cp++ = 0x47; /* INC DI */
 *cp++ = 0x88; *cp++ = 0x05; /* MOV [DI],AL */
 *cp++ = 0x47; /* INC DI */
 *cp++ = 0x88; *cp++ = 0x25; /* MOV [DI],AH */
 *cp++ = 0x47; /* INC DI */
#endif
 return cp;
}

codeptr c_q_mov_ra_imr0 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov cl,0BEh\n mov [di],cl\n inc di\n mov [di],al\n inc di\n mov[di],ah\n inc di\n");
#ifdef INTERP
 *cp++ = code_q_mov_ra_imr0;
#endif
#ifdef COMPARM
 *cp++ = 0xE59F9000; /* R9=[RF] */
 *cp++ = 0xEA000000; /* GOTO RF */
 *cp++ = 0xE59FA000; /* RA=[RF] */
 *cp++ = 0xE58C9000; /* [RC]=R9 */
 *cp++ = 0xE28CC004; /* RC=RC+4 */
 *cp++ = 0xE59F9000; /* R9=[RF] */
 *cp++ = 0xEA000000; /* GOTO RF */
 *cp++ = 0xEA000000; /* GOTO RF */
 *cp++ = 0xE58C9000; /* [RC]=R9 */
 *cp++ = 0xE28CC004; /* RC=RC+4 */
 *cp++ = 0xE58C0000; /* [RC]=R0 */
 *cp++ = 0xE28CC004; /* RC=RC+4 */
#endif
#ifdef COMPX86
 *cp++ = 0xB1; *cp++ = 0xBE; /* MOV CL,BE */
 *cp++ = 0x88; *cp++ = 0x0D; /* MOV [DI],CL */
 *cp++ = 0x47; /* INC DI */
 *cp++ = 0x88; *cp++ = 0x05; /* MOV [DI],AL */
 *cp++ = 0x47; /* INC DI */
 *cp++ = 0x88; *cp++ = 0x25; /* MOV [DI],AH */
 *cp++ = 0x47; /* INC DI */
#endif
 return cp;
}


int reg_num_x86[] = { 0, 2, 1, -1, -1, -1, -1, -1, -1, -1, 6, 3, 7, 0, 0, 0 };

const char * reg_name_x86 [] = { "ax", "dx", "cx", "", "", "", "", "", "", "", "si", "bx", "di" };

codeptr c_mov_r_r (codeptr cp, int rd, int rs)
{
 if (f_tasm != NULL)
  fprintf (f_tasm," mov %s,%s\n", reg_name_x86[rd], reg_name_x86[rs]);
 if (f_asmi != NULL)
  fprintf (f_asmi, " mov %%%s,%%%s\n", reg_name_x86[rd], reg_name_x86[rs]);
 if (f_asml != NULL)
  fprintf (f_asml, " mov %%%s,%%%s\n", reg_name_x86[rs], reg_name_x86[rd]);
 if (f_x86 != NULL)
  /* fprintf (f_x86, " .byte 0x89, 0xCA\n"); */
  fprintf (f_x86, " .byte 0x8B, 0x%X\n", 0xC0 | (rd<<3) | rs);
#ifdef INTERP
 *cp++ = code_mov_r_r | (rd<<8) | rs;
#endif
#ifdef COMPARM
  /* *cp++ = 0xE1A01002; */
  *cp++ = 0xEAA00000 | (rd<<12) | rs;
#endif
#ifdef COMPX86
 /*
 *cp++ = 0x89;
 *cp++ = 0xCA;
 */
 *cp++ = 0x8B;
 *cp++ = 0xC0 | (rd<<3) | rs;
#endif
 return cp;
}
codeptr c_mov_r1_r0 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm," mov dx,ax\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " mov %%dx,%%ax\n");
 if (f_asml != NULL)
  fprintf (f_asml, " mov %%ax,%%dx\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x89, 0xC2\n");
#ifdef INTERP
 *cp++ = code_mov_r1_r0;
#endif
#ifdef COMPARM
  *cp++ = 0xE1A01000;
#endif
#ifdef COMPX86
 *cp++ = 0x89;
 *cp++ = 0xC2;
#endif
 return cp;
}

/* [rc+=n] = code(r1=r0) */
codeptr c_q_mov_r1_r0 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov cl,89h\n mov [di],cl\n inc di\n mov cl,C3h\n mov [di],cl\n inc di\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " mov %%cl,0x89\n mov [%%di],%%cl\n inc %%di\n mov %%cl,0xC3\n mov [%%di],%%cl\n inc %%di\n");
 if (f_asml != NULL)
  fprintf (f_asml, " mov $0x89,%%cl\n mov %%cl,(%%di)\n inc %%di\n mov $0xC3,%%cl\n mov %%cl,(%%di)\n inc %%di\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0xB1, 0x89, 0x88, 0x0D, 0x47, 0xB1, 0xC2, 0x88, 0x0D, 0x47\n");
#ifdef INTERP
 *cp++ = code_q_mov_r1_r0;
#endif
#ifdef COMPARM
 *cp++ = 0xE59F9000; /* R9 = [RF] */
 *cp++ = 0xEA000000; /* GOTO RF */
 *cp++ = 0xE1A01000; /* code of R1=R0 to load in R9 */
 *cp++ = 0xE58C9000; /* [RC]=R9 */
 *cp++ = 0xE28CC004; /* RC=RC+4 */
#endif
#ifdef COMPX86
 *cp++ = 0xB1;
 *cp++ = 0x89;
 *cp++ = 0x88;
 *cp++ = 0x0D;
 *cp++ = 0x47;
 *cp++ = 0xB1;
 *cp++ = 0xC2;
 *cp++ = 0x88;
 *cp++ = 0x0D;
 *cp++ = 0x47;
#endif
 return cp;
}

codeptr c_mov_r0_cont_ra (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov ax,[si]\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " mov %%ax,[%%si]\n");
 if (f_asml != NULL)
  fprintf (f_asml, " mov (%%si),%%ax\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x8B, 0x04\n");
#ifdef INTERP
 *cp++ = code_mov_r0_cont_ra;
#endif
#ifdef COMPARM
 *cp++ = 0xE59A0000;
#endif
#ifdef COMPX86
 *cp++ = 0x8B;
 *cp++ = 0x04;
#endif
 return cp;
}

codeptr c_mov_ra_cont_ra (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov si,[si]\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " mov %%si,[%%si]\n");
 if (f_asml != NULL)
  fprintf (f_asml, " mov (%%si),%%si\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x8B, 0x34\n");
#ifdef INTERP
 *cp++ = code_mov_ra_cont_ra;
#endif
#ifdef COMPARM
 *cp++ = 0xE59AA000;
#endif
#ifdef COMPX86
 *cp++ = 0x8B;
 *cp++ = 0x34;
#endif
 return cp;
}

codeptr c_mov_cont_ra_r0 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov [si],ax\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " mov [%%si],%%ax\n");
 if (f_asml != NULL)
  fprintf (f_asml, " mov %%ax,(%%si)\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x89, 0x04\n");
#ifdef INTERP
 *cp++ = code_mov_cont_ra_r0;
#endif
#ifdef COMPARM
 *cp++ = 0xE58A0000;
#endif
#ifdef COMPX86
 *cp++ = 0x89;
 *cp++ = 0x04;
#endif
 return cp;
}

codeptr c_mov_r0_cont_ra_plus_im (codeptr cp, int im)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov ax,[si+%Xh]\n", im);
 if (f_asmi != NULL)
  fprintf (f_asmi, " mov %%ax,[%%si+0x%X]\n", im);
 if (f_asml != NULL)
  fprintf (f_asml, " mov (%%si+0x%X),%%ax\n", im);
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x8B, 0x44, 0x%X\n", im);
#ifdef INTERP
 *cp++ = code_mov_r0_cont_ra_plus_im;
 *cp++ = (instr)im;
#endif
#ifdef COMPARM
 *cp++ = 0xE59A0000 | im;
#endif
#ifdef COMPX86
 *cp++ = 0x8B;
 *cp++ = 0x44;
 *cp++ = im;
#endif
 return cp;
}

codeptr c_mov_cont_ra_plus_im_r0 (codeptr cp, int im)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov [si+%Xh],ax\n", im);
 if (f_asmi != NULL)
  fprintf (f_asmi, " mov [%%si+0x%X],%%ax\n", im);
 if (f_asml != NULL)
  fprintf (f_asml, " mov %%ax,(%%si+0x%X)\n", im);
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x89, 0x44; 0x%X\n", im);
#ifdef INTERP
 *cp++ = code_mov_cont_ra_plus_im_r0;
 *cp++ = (instr)im;
#endif
#ifdef COMPARM
 *cp++ = 0xE58A0000 | im;
#endif
#ifdef COMPX86
 *cp++ = 0x89;
 *cp++ = 0x44;
 *cp++ = im;
#endif
 return cp;
}

codeptr c_mov_r0_cont_rb_inc (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov ax,[bx]\n add bx,2\n");
#ifdef INTERP
 *cp++ = code_mov_r0_cont_rb_inc;
#endif
#ifdef COMPARM
 *cp++ = 0xE8BB0001;
#endif
#ifdef COMPX86
 *cp++ = 0x8B; /* mov ax,[bx] */
 *cp++ = 0x07;
 *cp++ = 0x83; /* add bx,2 */
 *cp++ = 0xC3;
 *cp++ = 0x02;
#endif
 return cp;
}

codeptr c_mov_cont_dec_rb_r0 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " sub bx,2\n mov [bx],ax\n");
#ifdef INTERP
 *cp++ = code_mov_cont_dec_rb_r0;
#endif
#ifdef COMPARM
 *cp++ = 0xE92B0001;
#endif
#ifdef COMPX86
 *cp++ = 0x83; /* sub bx,2 */
 *cp++ = 0xEB;
 *cp++ = 0x02;
 *cp++ = 0x89; /* mov [bx],ax */
 *cp++ = 0x07;
#endif
 return cp;
}

codeptr c_mov_r1_cont_rb_inc (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov dx,[bx]\n add bx,2\n");
#ifdef INTERP
 *cp++ = code_mov_r1_cont_rb_inc;
#endif
#ifdef COMPARM
 *cp++ = 0xE8BB0002;
#endif
#ifdef COMPX86
 *cp++ = 0x8B; /* mov dx,[bx] */
 *cp++ = 0x17;
 *cp++ = 0x83; /* add bx,2 */
 *cp++ = 0xC3;
 *cp++ = 0x02;
#endif
 return cp;
}

codeptr c_mov_ra_cont_rb_inc (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov si,[bx]\n add bx,2\n");
#ifdef INTERP
 *cp++ = code_mov_ra_cont_rb_inc;
#endif
#ifdef COMPARM
 *cp++ = 0xE8BB0400;
#endif
#ifdef COMPX86
 *cp++ = 0x8B; /* mov si,[bx] */
 *cp++ = 0x37;
 *cp++ = 0x83; /* add bx,2 */
 *cp++ = 0xC3;
 *cp++ = 0x02;
#endif
 return cp;
}

codeptr c_mov_cont_dec_rb_r1 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " sub bx,2\n mov [bx],dx\n");
#ifdef INTERP
 *cp++ = code_mov_cont_dec_rb_r1;
#endif
#ifdef COMPARM
 *cp++ = 0xE92B0002;
#endif
#ifdef COMPX86
 *cp++ = 0x83; /* sub bx,2 */
 *cp++ = 0xEB;
 *cp++ = 0x02;
 *cp++ = 0x89; /* mov [bx],dx */
 *cp++ = 0x17;
#endif
 return cp;
}

codeptr c_mov_cont_rc_inc_r0 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov [di],ax\n inc di\n");
#ifdef INTERP
 *cp++ = code_mov_cont_rc_inc_r0;
#endif
#ifdef COMPARM
 *cp++ = 0xE8AC0001;
#endif
#ifdef COMPX86
 *cp++ = 0x89; *cp++ = 0x05; /* mov [di],ax */
 *cp++ = 0x47; /* inc di */
#endif
 return cp;
}

codeptr c_add_r0_r1 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " add ax,dx\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " add %%ax,%%dx\n");
 if (f_asml != NULL)
  fprintf (f_asml, " add %%dx,%%ax\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x01, 0xD0\n");
#ifdef INTERP
 *cp++ = code_add_r0_r1;
#endif
#ifdef COMPARM
 *cp++ = 0xE0800001;
#endif
#ifdef COMPX86
 *cp++ = 0x01;
 *cp++ = 0xD0;
#endif
 return cp;
}

codeptr c_sub_r0_r1 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " sub ax,dx\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " sub %%ax,%%dx\n");
 if (f_asml != NULL)
  fprintf (f_asml, " sub %%dx,%%ax\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x29, 0xD0\n");
#ifdef INTERP
 *cp++ = code_sub_r0_r1;
#endif
#ifdef COMPARM
 *cp++ = 0xE0400001;
#endif
#ifdef COMPX86
 *cp++ = 0x29;
 *cp++ = 0xD0;
#endif
 return cp;
}

codeptr c_bp_depl (codeptr cp, int depl)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " cmp ax,0\n jge %s\n", last_label);
 if (f_asmi != NULL)
  fprintf (f_asmi, " cmp %%ax,0\n jge %s\n", last_label);
 if (f_asml != NULL)
  fprintf (f_asml, " cmp %%ax,0\n jge %s\n", last_label);
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x3D, 0, 0\n jge %s\n", last_label);
#ifdef INTERP
 *cp++ = code_bp_depl;
 *cp++ = depl;
#endif
#ifdef COMPARM
 *cp++ = 0xE1B00000;
 *cp++ = 0x5A000000 + depl;
#endif
#ifdef COMPX86
 *cp++ = 0x3D;
 *cp++ = 0x00;
 *cp++ = 0x00;
 /* *cp++ = 0x2E; */

 *cp++ = 0x7D;
 *cp++ = depl;
/*
 *cp++ = 0x0F;
 *cp++ = 0x8D;
 *cp++ = depl & 0xFF;
 *cp++ = (depl & 0xFF00) >> 8;
*/
#endif
 return cp;
}

codeptr c_bz_depl (codeptr cp, int depl)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " cmp ax,0\n je %s\n", last_label);
 if (f_asmi != NULL)
  fprintf (f_asmi, " cmp %%ax,0\n je %s\n", last_label);
 if (f_asml != NULL)
  fprintf (f_asml, " cmp %%ax,0\n je %s\n", last_label);
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x3D, 0, 0\n je %s\n", last_label);
#ifdef INTERP
 *cp++ = code_bz_depl;
 *cp++ = (instr)depl;
#endif
#ifdef COMPARM
 *cp++ = 0xE1B00000;
 *cp++ = 0x0A000000 + depl;
#endif
#ifdef COMPX86
 *cp++ = 0x3D; /* cmp ax,0 */
 *cp++ = 0x00;
 *cp++ = 0x00;

 *cp++ = 0x74; /* je adr */
 *cp++ = (instr)depl;
#endif
 return cp;
}

codeptr c_setbradr (codeptr cp)
{
/* compile code to modify the instruction bp located at rA in such a way it
   branches to r0 */
#ifdef INTERP
 *cp++ = code_setbradr;
#endif
#ifdef COMPARM
 *cp++ = 0xE040400A; /* R4=R0-RA */
 *cp++ = 0xE244400C; /* R4=R4-C */
 *cp++ = 0xE1A04144;/* R4=R4>>2 */
 *cp++ = 0xE59A5004; /* R5=[RA+4] */
 *cp++ = 0xE0855004; /* R5=R5+R4 */
 *cp++ = 0xE58A5004; /* [RA+4]=R5 */
#endif
#ifdef COMPX86
 *cp++ = 0x89; *cp++ = 0xC1; /* MOV CX,AX */
 *cp++ = 0x29; *cp++ = 0xF1; /* SUB CX,SI */
 *cp++ = 0x83; *cp++ = 0xE9; *cp++ = 0x05; /* SUB CX,5 */
 *cp++ = 0x89; *cp++=0x4C; *cp++ = 0x04; /* MOV [SI+4],CX */
#endif
 return cp;
}

int label_tasm = 0;
int label_asmi = 0;
int label_asml = 0;

codeptr c_bp_ra (codeptr cp)
{
 if (f_tasm != NULL)
 {
  label_tasm++;
  fprintf (f_tasm, " cmp ax,0\n jl l%d\n jmp si\nl%d:\n",
   label_tasm, label_tasm);
 }
 if (f_asmi != NULL)
 {
  label_asmi++;
  fprintf (f_asmi, " cmp %%ax,0\n jl l%d\n jmp %%si\nl%d:\n",
   label_asmi, label_asmi);
 }
 if (f_asml != NULL)
 {
  label_asml++;
  fprintf (f_asml, " cmp %%ax,0\n jl l%d\n jmp *%%si\nl%d:\n",
   label_asml, label_asml);
 }
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x3D, 0, 0, 0x7C, 2, 0xFF, 0xE6\n");

#ifdef INTERP
 *cp++ = code_bp_ra;
#endif
#ifdef COMPARM
 *cp++ = 0xE1B00000;
 *cp++ = 0x51A0F00A;
#endif
#ifdef COMPX86
 *cp++ = 0x3D;      	/* cmp ax,0 */
 *cp++ = 0x00;
 *cp++ = 0x00;
 /* *cp++ = 0x2E; */
 *cp++ = 0x7C;      	/* jl $+2 */
 *cp++ = 0x02;
 *cp++ = 0xFF;        	/* jmp si */
 *cp++ = 0xE6;
#endif
 return cp;
}

codeptr c_bz_r1 (codeptr cp)
{
#ifdef COMPARM
 *cp++ = 0xE1B00000;
 *cp++ = 0x01A0F001;
#endif
#ifdef COMPX86

#endif
 return cp;
}

codeptr c_call_depl (codeptr cp, int depl)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " call %s\n", last_label);
 if (f_asmi != NULL)
  fprintf (f_asmi, " call %s\n", last_label);
 if (f_asml != NULL)
  fprintf (f_asml, " call %s\n", last_label);
 if (f_x86 != NULL)
  fprintf (f_x86, " call %s\n", last_label);
#ifdef INTERP
 *cp++ = code_call_depl;
 *cp++ = depl;
#endif
#ifdef COMPARM
 *cp++ = 0xE1A0E00F;
 *cp++ = 0xEB000000 + depl;
#endif
#ifdef COMPX86
 *cp++ = 0x2E;
 *cp++ = 0xE8;
 *cp++ = depl;
 *cp++ = 0x00;
#endif
 return cp;
}

codeptr c_q_call_imr0 (codeptr cp)
{

}

codeptr c_call_ra (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " call si\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " call %%si\n");
 if (f_asml != NULL)
  fprintf (f_asml, " call *%%si\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0xFF, 0xD6\n");
#ifdef INTERP
 *cp++ = code_call_ra;
#endif
#ifdef COMPARM
 *cp++ = 0xE1A0E00F;
 *cp++ = 0xE1A0F00A;
#endif
#ifdef COMPX86
 *cp++ = 0xFF;
 *cp++ = 0xD6;
#endif
 return cp;
}

codeptr c_beginf (codeptr cp)
{
#ifdef INTERP
 *cp++ = code_beginf;
#endif
#ifdef COMPARM
 *cp++ = 0xE92D4000; /* PUSH RD RE */
#endif
 return cp;
}

codeptr c_endf (codeptr cp)
{
 if (f_tasm!=NULL)
  fprintf(f_tasm," ret\n");
#ifdef INTERP
 *cp++ = code_endf;
#endif
#ifdef COMPARM
 *cp++ = 0xE8BD8000; /* POP RS RF */
#endif
#ifdef COMPX86
 *cp++ = 0xC3;
#endif
 return cp;
}

codeptr c_ret (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " ret\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " ret\n");
 if (f_asml != NULL)
  fprintf (f_asml, " ret\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0xC3\n");
#ifdef INTERP
 *cp++ = code_ret;
#endif
#ifdef COMPARM
 *cp++ = 0xE1A0F00E;
#endif
#ifdef COMPX86
 *cp++ = 0xC3;
#endif
 return cp;
}

codeptr c_ror_r0 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " ror ax,1\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " ror %%ax,1\n");
 if (f_asml != NULL)
  fprintf (f_asml, " ror %%ax,$1\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0xD1, 0xC8\n");
#ifdef INTERP
 *cp++ = code_ror_r0;
#endif
#ifdef COMPARM
 *cp++ = 0xE1A000A0;
#endif
#ifdef COMPX86
 *cp++ = 0xD1;
 *cp++ = 0xC8;
#endif
 return cp;
}

codeptr c_asr_r0 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " sar ax,1\n");
#ifdef INTERP
 *cp++ = code_asr_r0;
#endif
#ifdef COMPARM
 *cp++ = 0xE1A000C0;
#endif
#ifdef COMPX86
 *cp++ = 0xC1;
 *cp++ = 0xF8;
 *cp++ = 0x01;
#endif
 return cp;
}


codeptr c_and_r0_r1 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " and ax,dx\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " and %%ax,%%dx\n");
 if (f_asml != NULL)
  fprintf (f_asml, " and %%ax,%%dx\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x21, 0xD0\n");
#ifdef INTERP
 *cp++ = code_and_r0_r1;
#endif
#ifdef COMPARM
 *cp++ = 0xE0000001;
#endif
#ifdef COMPX86
 *cp++ = 0x21;
 *cp++ = 0xD0;
#endif
 return cp;
}

codeptr c_xor_r0_r1 (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " xor ax,dx\n");
 if (f_asmi != NULL)
  fprintf (f_asmi, " xor %%ax,%%dx\n");
 if (f_asml != NULL)
  fprintf (f_asml, " xor %%ax,%%dx\n");
 if (f_x86 != NULL)
  fprintf (f_x86, " .byte 0x31, 0xD0\n");
#ifdef INTERP
 *cp++ = code_xor_r0_r1;
#endif
#ifdef COMPARM
 *cp++ = 0xE0200001;
#endif
#ifdef COMPX86
 *cp++ = 0x31;
 *cp++ = 0xD0;
#endif
 return cp;
}

int f_exit (void)
{
 exit(0);
}

codeptr c_exit (codeptr cp)
{
 if (f_tasm != NULL)
  fprintf (f_tasm, " mov ax,0\n int 21h\n");
#ifdef INTERP
 *cp++ = code_exit;
#endif
#ifdef COMPARM
 cp = compile_call0 (cp, f_exit);
#endif
#ifdef COMPX86
 *cp++ = 0xB8; *cp++ = 0; *cp++ = 0; /* mov ax,0 */
 *cp++ = 0xCD; *cp++ = 0x21; /* int 21h */
#endif
 return cp;
}

any fillmem;
instr fillmem_code[1000];

codeptr lengthadr;

int ncode=0;

codeptr c_code (codeptr cp)
{
 /*
  to do : produce code for f_tasm
  problem : length unknown
  possible solution : write code to buffer until length known,
  then write length and buffer to file
 */
 ncode++;
 if (f_tasm!=NULL)
  fprintf(f_tasm," call fillmem\n db b%d-a%d\na%d:\n",
   ncode, ncode, ncode);
#ifdef INTERP
 *cp++ = code_fillmem;
 lengthadr = cp;
 *cp++ = 0;
#else
 cp = compile_call0 (cp, fillmem.intf);
 lengthadr = cp;
 *cp++ = 0;
#endif
 return cp;
}

codeptr c_endcode (codeptr cp)
{
int n;
 if (f_tasm!=NULL)
  fprintf (f_tasm, "b%d:\n", ncode);
 n = cp - lengthadr - 1;
 *lengthadr = (instr)n;

 return cp;
}

/* any r0, r1, r2, ra, rb, rc; */
/*
any reg[16];
#define r0 reg[0]
#define r1 reg[1]
#define r2 reg[2]
#define ra reg[10]
#define rb reg[11]
#define rc reg[12]

#define STACKSIZE 1000
codeptr stack[STACKSIZE];
int sp;
*/

void interp (void)
{
codeptr cp;
instr ins;
int depl;
any a;
char c;
 cp = cod;
 sp = 0;
 for (;;)
 {
  ins = *cp++;
  /* printf ("%08X %08X\n", ins, *cp); */
  if (ins == code_mov_r0_im)
    r0.ins = *cp++;
  else if (ins == code_mov_r1_im)
    r1.ins = *cp++;
  else if (ins == code_mov_ra_im)
    ra.ins = *cp++;
  else if (ins == code_q_mov_r0_imr0)
  {
    *(rc.p++) = code_mov_r0_im;
    *(rc.p++) = r0.ins;
  }
  else if (ins == code_q_mov_r1_imr0)
  {
    *(rc.p++) = code_mov_r1_im;
    *(rc.p++) = r0.ins;
  }
  else if (ins == code_q_mov_ra_imr0)
  {
    *(rc.p++) = code_mov_ra_im;
    *(rc.p++) = r0.ins;
  }
  else if ((ins & 0xFFFF0000) == code_mov_r_r)
  {
    int rd, rs;
    rd = (ins & 0xFF00) >> 8;
    rs = (ins & 0xFF);
    reg[rd] = reg[rs];
  }
  else if (ins == code_mov_r1_r0)
    r1 = r0;
  else if (ins == code_mov_r2_r0)
    r2 = r0;
  else if (ins == code_mov_r0_r2)
    r0 = r2;
  else if (ins == code_mov_r1_r2)
    r1 = r2;
  else if (ins == code_q_mov_r1_r0)
    *(rc.p++) = code_mov_r1_r0;
  else if (ins == code_mov_ra_r0)
    ra = r0;
  else if (ins == code_mov_rb_r0)
    rb = r0;
  else if (ins == code_mov_rc_r0)
    rc = r0;
  else if (ins == code_mov_r0_ra)
    r0 = ra;
  else if (ins == code_mov_r0_rb)
    r0 = rb;
  else if (ins == code_mov_r0_rc)
    r0 = rc;
  else if (ins == code_mov_ra_r1)
    ra = r1;
  else if (ins == code_mov_rc_r1)
    rc = r1;
  else if (ins == code_mov_rc_ra)
    rc = ra;
  else if (ins == code_mov_r1_rc)
    r1 = rc;
  else if (ins == code_mov_r0_cont_ra)
    r0.i = *(ra.p);
  else if (ins == code_mov_ra_cont_ra)
    ra.i = *(ra.p);
  else if (ins == code_mov_cont_ra_r0)
    *(ra.p) = r0.i;
  else if (ins == code_mov_r0_cont_ra_plus_im)
    r0.i = ra.p[((int)(*cp))/4];
  else if (ins == code_mov_cont_ra_plus_im_r0)
    ra.p[((int)(*cp))/4] = r0.i;
  else if (ins == code_mov_r0_cont_rb_inc)
   r0.i = *(rb.p++);
  else if (ins == code_mov_cont_dec_rb_r0)
   *(--rb.p) = r0.i;
  else if (ins == code_mov_r1_cont_rb_inc)
   r1.i = *(rb.p++);
  else if (ins == code_mov_ra_cont_rb_inc)
   ra.i = *(rb.p++);
  else if (ins == code_mov_cont_dec_rb_r1)
   *(--rb.p) = r1.i;
  else if (ins == code_mov_cont_rc_inc_r0)
   *(rc.p++) = r0.i;
  else if (ins == code_add_r0_r1)
    r0.i = r0.i + r1.i;
  else if (ins == code_sub_r0_r1)
    r0.i = r0.i - r1.i;
  else if (ins == code_bp_depl)
  {
    depl = (int) *cp++;
    if (r0.i >= 0)
     cp += depl;
  }
  else if (ins == code_bz_depl)
  {
    depl = (int) *cp++;
    if (r0.i == 0)
     cp += depl;
  }
  else if (ins == code_bp_ra)
  {
    if (r0.i >= 0)
     cp = ra.cp;
  }
  else if (ins == code_setbradr)
  {
   /* rA = adresse de l'instruction de saut bp
      r0 = adresse du saut */
   int depl;
   depl = r0.cp - ra.cp + 1;
   ra.cp[1] = depl;
  }
  else if (ins == code_call_depl)
  {
    if (sp>=STACKSIZE)
    {
     printf("\nstack overflow\n");
     break;
    }
    depl = (int) *cp++;
    stack[sp++] = cp;
    cp += depl;
  }
  else if (ins == code_call_ra)
  {
    if (sp >= STACKSIZE)
    {
     printf ("\nstack overflow\n");
     /* exit(0); */
     return;
    }
    stack[sp++] = cp;
    cp = ra.cp;
  }
  else if (ins==code_beginf);
  else if (ins == code_ret || ins==code_endf)
  {
    if (sp <= 0)
    {
     printf ("\nstack underflow\n");
     /* exit(0); */
     return;
    }
    cp = stack[--sp];
  }
  else if (ins == code_asr_r0)
    r0.i = r0.i >> 1;
  else if (ins == code_and_r0_r1)
    r0.i = r0.i & r1.i;
  else if (ins == code_xor_r0_r1)
    r0.i = r0.i ^r1.i;
  else if (ins == code_callext)
  {
    a.ins = *cp++;
    a.vf();
  }
  else if (ins == code_getchar)
    r0.i = getchar();
  else if (ins == code_putchar)
    putchar (r0.i);
  else if (ins == code_getword)
  {
    ra.pw = getword();
    r0.i = 0;
    sscanf (awr, "%x", &r0.i);
  }
  else if (ins == code_scanhex)
    r0.i = f_scanhex ();
  else if (ins == code_fillmem)
  {
   int n, i;
   n = (int)(*cp++);
   for (i=0; i<n; i++)
    *(rc.cp++) = *cp++;
  }
  else if (ins == code_exit)
   exit(r0.i);
  else
    printf ("Instruction %X not implemented\n", ins);

 }

}

codeptr compile_prog1 (codeptr cp)
{
 cp = c_getchar (cp);
 cp = c_mov_r1_r0 (cp);
 cp = c_mov_r0_im (cp, 1, 0);
 cp = c_add_r0_r1 (cp);
 cp = c_putchar (cp);
 cp = c_ret (cp);
 return cp;
}

codeptr compile_prog (codeptr cp)
{
 codeptr from, to, adr1, adr2, adr3, adr4;
 /* cp = c_mov_r0_im (cp, 1); */
 /* cp = c_mov_r1_r0 (cp); */
 cp = c_getchar(cp);
 adr1 = cp;
 cp = c_mov_ra_im (cp, 0, 0);
 cp = c_mov_cont_ra_r0 (cp);
 to = cp;
 printf ("to=%X\n",to);
 adr2 = cp;
 cp = c_mov_ra_im (cp, 0, 0);
 cp = c_mov_r0_cont_ra (cp);
 cp = c_mov_r1_im (cp, 1, 0);
 cp = c_sub_r0_r1 (cp);
 adr3 = cp;
 cp = c_mov_ra_im (cp, 0, 0);
 cp = c_mov_cont_ra_r0 (cp);
 cp = c_putchar (cp);
 /* cp = c_print (cp); */
 /* cp = c_mov_r1_im (cp, 0xff); */
 /* cp = c_and_r0_r1 (cp); */
 /* cp = c_mov_r0_im (cp, -1); */
 adr4 = cp;
 cp = c_mov_ra_im (cp, 0, 0);
 cp = c_mov_r0_cont_ra (cp);
 /* from = cp+1; */
 from = cp;
 printf("from=%X\n",from);
 cp = c_bp_depl (cp, 0);
 printf("from=%X to=%X\n",from,to);
 setbradr (from, to);
 cp = c_ret (cp);
 c_mov_r1_im (adr1, (int)cp, 0);
 c_mov_r1_im (adr2, (int)cp, 0);
 c_mov_r1_im (adr3, (int)cp, 0);
 c_mov_r1_im (adr4, (int)cp, 0);
 return cp;
}

int isdgt (char c)
{
 if (c>='0' && c<='9')
  return 1;
 if (c>='A' && c<='F')
  return 1;
 return 0;
}

codeptr compile_op (codeptr cp, char *op, int nargs, int *iargs, char largs[8][200], char *s)
{
int i;
codeptr from, to;
any a;
 printf ("cp=0x%X op=<%s>" , cp, op);
 for (i=0; i<nargs; i++)
  printf (" arg%d=%X", i, iargs[i]);
 printf ("\n");
 if (op[0] == ':')
 {
  char name[200];
  lab[op[1]] = cp;
  printf ("label %c = 0x%X, cod=0x%X, depl=0x%X\n", op[1], cp, cod, cp-cod);
  sscanf (op+1, "%s", name);
  strcpy (label[nlab].name, name);
  label[nlab].value.cp = cp;
  nlab++;
  if (f_tasm != NULL)
   fprintf (f_tasm, "%s:\n", name);
  if (f_asmi != NULL)
   fprintf (f_asmi, "%s:\n", name);
  if (f_asml != NULL)
   fprintf (f_asml, "%s:\n", name);
  if (f_x86 != NULL)
   fprintf (f_x86, "%s:\n", name);
  /*
  if (f_tasm != NULL)
   fprintf (f_tasm, "%c:\n", op[1]);
  if (f_asmi != NULL)
   fprintf (f_asmi, "%c:\n", op[1]);
  if (f_asml != NULL)
   fprintf (f_asml, "%c:\n", op[1]);
  if (f_x86 != NULL)
   fprintf (f_x86, "%c:\n", op[1]);
  */
 }
 else if (!strcmp (op, "align"))
 {
  if (f_tasm != NULL)
   fprintf (f_tasm, " align %d\n", iargs[0]);
  if (f_asmi != NULL)
   fprintf (f_asmi, " .align %d\n", iargs[0]);
  if (f_asml != NULL)
   fprintf (f_asml, " .align %d\n", iargs[0]);
  if (f_x86 != NULL)
   fprintf (f_x86, " .align %d\n", iargs[0]);
#ifndef INTERP
  a.cp = cp;
  a.i = (a.i + iargs[0] - 1) / iargs[0] * iargs[0];
  a.i = (a.i + sizeof(instr) - 1) / sizeof(instr) * sizeof(instr);
  return a.cp;
#endif
 }
 else if (!strcmp (op, "res"))
 {
  if (f_tasm != NULL)
   fprintf (f_tasm, " db %Xh dup (0)\n", iargs[0]);
  if (f_asmi != NULL)
   fprintf (f_asmi, " .space 0x%X, 0\n", iargs[0]);
  if (f_asml != NULL)
   fprintf (f_asml, " .space 0x%X, 0\n", iargs[0]);
  if (f_x86 != NULL)
   fprintf (f_x86, " .space 0x%X, 0\n", iargs[0]);
  a.cp = cp;
  a.i += iargs[0];
  a.i = (a.i + sizeof(instr) - 1) / sizeof(instr) * sizeof(instr);
  return a.cp;
 }
 else if (!strncmp (op, "defword", 7))
 {
  /* a verifier */

  union
  {
   codeptr cp;
   void **p;
   pword pw;
   char *pchar;
   chr *pchr;
  } u;

  pword pw;
  int nc, nw;
  char name[100];
  sscanf (s+8,"%s",name);
  pw = (pword)cp;
  /*
  readword();
  nc = strlen(awr);
  */
  /* getwordstr(s+8); */
  nc = strlen(name);
  nw = nc/4 + 1;
  u.cp = cp;
  *(u.p) = (void *)lastword;
  lastword = u.pw;
  u.p++;
  *(u.p)++ = (void *)(u.pw+4+nw);
  *(u.p)++ = NULL;
  *(u.p)++ = (void *)(u.pw+4);
  strcpy (u.pchar, name);
  return (codeptr)(pw+4+nw);
 }
 else if (!strcmp (op, "r="))
 {
  printf("op r=\n");
  if (iargs[0]==0)
   cp = c_mov_r0_im (cp, iargs[1], largs[1]);
  else if (iargs[0]==1)
   cp = c_mov_r1_im (cp, iargs[1], largs[1]);
  else if (iargs[0]==10)
  {
   printf("op rA=\n");
   cp = c_mov_ra_im (cp, iargs[1], largs[1]);
   printf("end rA=\n");
  }
  else
   printf ("Invalid instruction <%s>\n", s);
  printf("end r=\n");
 }
 else if (!strcmp (op, "coder={r}"))
 {
  if (iargs[0]==0 && iargs[1]==0)
   cp = c_q_mov_r0_imr0 (cp);
  else if (iargs[0]==1 && iargs[1]==0)
   cp = c_q_mov_r1_imr0 (cp);
  else if (iargs[0]==10 && iargs[1]==0)
   cp = c_q_mov_ra_imr0 (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "r=r"))
 {
  cp = c_mov_r_r (cp, iargs[0], iargs[1]);
  /*
  if (iargs[0]==1 && iargs[1]==0)
   cp = c_mov_r1_r0 (cp);
 
  else
   printf ("Invalid instruction <%s>\n", s);
  */
 }
 else if (!strcmp (op, "'r=r"))
 {
  if (iargs[0]==1 && iargs[1]==0)
   cp = c_q_mov_r1_r0 (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "r=[r]"))
 {
  if (iargs[0]==0 && iargs[1]==10)
   cp = c_mov_r0_cont_ra (cp);
  else if (iargs[0]==10 && iargs[1]==10)
   cp = c_mov_ra_cont_ra (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "[r]=r"))
 {
  if (iargs[0]==10 && iargs[1]==0)
   cp = c_mov_cont_ra_r0 (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "r=[r+]"))
 {
  if (iargs[0]==0 && iargs[1]==10)
   cp = c_mov_r0_cont_ra_plus_im (cp, iargs[2]);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "[r+]=r"))
 {
  if (iargs[0]==10 && iargs[2]==0)
   cp = c_mov_cont_ra_plus_im_r0 (cp, iargs[1]);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "r=[r++]"))
 {
  if (iargs[0]==0 && iargs[1]==11)
   cp = c_mov_r0_cont_rb_inc (cp);
  else if (iargs[0]==1 && iargs[1]==11)
   cp = c_mov_r1_cont_rb_inc (cp);
  else if (iargs[0]==10 && iargs[1]==11)
   cp = c_mov_ra_cont_rb_inc (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "[--r]=r"))
 {
  if (iargs[0]==11 && iargs[1]==0)
   cp = c_mov_cont_dec_rb_r0 (cp);
  else if (iargs[0]==11 && iargs[1]==1)
   cp = c_mov_cont_dec_rb_r1 (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "[r++]=r"))
 {
  if (iargs[0]==12 && iargs[1]==0)
   cp = c_mov_cont_rc_inc_r0 (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "r=r+r"))
 {
  if (iargs[0]==0 && iargs[1]==0 && iargs[2]==1)
   cp = c_add_r0_r1 (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "r=r-r"))
 {
  if (iargs[0]==0 && iargs[1]==0 && iargs[2]==1)
   cp = c_sub_r0_r1 (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "asrr"))
 {
  if (iargs[0]==0)
   cp = c_asr_r0 (cp);
  else
   printf ("Invalid instruction <%s>\n");
 }
 else if (!strcmp (op, "r=r&r"))
 {
  if (iargs[0]==0 && iargs[1]==0 && iargs[2]==1)
   cp = c_and_r0_r1 (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "r=r^r"))
 {
  if (iargs[0]==0 && iargs[1]==0 && iargs[2]==1)
   cp = c_xor_r0_r1 (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "bp"))
 {
  from = cp;
  cp = c_bp_depl (cp, 0);
  setbradr (from, (codeptr)(iargs[0]));
 }
 else if (!strcmp (op, "bz"))
 {
  from = cp;
  cp = c_bz_depl (cp, 0);
  setbradr (from, (codeptr)(iargs[0]));
 }
 else if (!strcmp (op, "bpr"))
 {
  if (iargs[0]==10)
   cp = c_bp_ra (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp (op, "call"))
 {
  from = cp;
  cp = c_call_depl (cp, 0);
  setbradr (from, (codeptr)(iargs[0]));
 }
 else if (!strcmp (op, "call{r}"))
 {
  cp = c_q_call_imr0 (cp);
 }
 else if (!strcmp (op, "callr"))
 {
  if (iargs[0]==10)
   cp = c_call_ra (cp);
  else
   printf ("Invalid instruction <%s>\n", s);
 }
 else if (!strcmp(op,"beginf"))
  cp=c_beginf(cp);
 else if (!strcmp(op,"endf"))
  cp=c_endf(cp);
 else if (!strcmp (op, "ret"))
  cp = c_ret (cp);
 else if (!strcmp (op, "getchar"))
 {
  printf("before getchar cp=%X\n",cp);
  cp = c_getchar (cp);
  printf("after getchar cp=%X\n",cp);
 }
 else if (!strcmp (op, "putchar"))
  cp = c_putchar (cp);
 else if (!strcmp (op, "scanhex"))
  cp = c_scanhex (cp);
 else if (!strcmp (op, "getword"))
  cp = c_getword (cp);
 else if (!strcmp (op, "code"))
  cp = c_code (cp);
 else if (!strcmp (op, "endcode"))
  cp = c_endcode (cp);
 else if (!strcmp (op, "setbradr"))
  cp = c_setbradr (cp);
 else if (!strcmp (op, "exit"))
  cp = c_exit (cp);
 else if (!strcmp (op, ""));
 else
  printf ("Invalid instruction <%s>\n", s);
 printf("end compile_op\n");
 return cp;
}

int value_of_label (char *name)
{
 int i;
 for (i=0; i<nlab; i++)
  if (!strcmp (label[i].name, name))
   return label[i].value.i;
 return 0;
}

codeptr compile_string (codeptr cp, char *s)
{
int i, k, nargs, lop;
char op[100], args[8][10];
int iargs[8];
/* char largs[8]; */
char largs[8][200];
 if (!strncmp (s, "defword ", 8))
 {
   /* a verifier */

  union
  {
   codeptr cp;
   void **p;
   pword pw;
   char *pchar;
   chr *pchr;
  } u;

  pword pw;
  int nc, nw;
  char name[100];
  sscanf (s+8,"%s",name);
  printf ("defword <%s>\n", name);
  pw = (pword)cp;
  /*
  readword();
  nc = strlen(awr);
  */
  /* getwordstr(s+8); */
  nc = strlen(name);
  nw = nc/4 + 1;
  u.cp = cp;
  *(u.p) = (void *)lastword;
  lastword = u.pw;
  u.p++;
  *(u.p)++ = (void *)(u.pw+4+nw);
  *(u.p)++ = NULL;
  *(u.p)++ = (void *)(u.pw+4);
  strcpy (u.pchar, name);
  return (codeptr)(pw+4+nw);

 }
 for (i=0; i<8; i++)
  largs[i][0] = 0;
 printf ("%s", s);
 nargs = 0;
 lop = 0;
 for (i=0; s[i]!=0 && s[i]!='\n' && s[i]!='\r' && s[i]!='%'; i++)
 {
  if (s[i]!=' ' && s[i]!='\t')
  {
   if (isdgt(s[i]))
   {
    for (k=0; isdgt(s[i+k]); k++)
     args[nargs][k] = s[i+k];
    args[nargs][k] = 0;
    nargs++;
    i += k-1;
   }
   else if (s[i] == '@')
   {
    char name[200];
    i++;
    /*
    largs[nargs] = s[i];
    sprintf (args[nargs++], "%X", lab[s[i]]);
    last_label = s[i];
    */
    sscanf (s+i, "%s", name);
    i += strlen(name);
    strcpy (largs[nargs], name);
    sprintf (args[nargs++], "%X", value_of_label (name));
    strcpy (last_label, name);
   }
   else
   {
    op[lop++] = s[i];
   }
  }
 }
 op[lop]=0;
 /*
 printf ("op=<%s>" , op);
 for (i=0; i<nargs; i++)
  printf (" arg %d = <%s>", i, args[i]);
 printf ("\n");
 */
 for (i=0; i<nargs; i++)
  sscanf (args[i], "%X", &(iargs[i]));
 cp = compile_op (cp, op, nargs, iargs, largs, s);
 return cp;
}

codeptr compile_file (codeptr cp, const char *fn,
 const char *fn_tasm, const char *fn_asmi, const char *fn_asml, const char *fn_x86)
{
FILE *f;
char line[1000];
 printf ("compile file %s\n", fn);
 f = fopen (fn, "r");
 if (f == NULL)
 {
  printf ("Cannot open file <%s>\n", fn);
  exit(0);
 }

 if (fn_tasm == NULL)
  f_tasm = NULL;
 else
  f_tasm = fopen (fn_tasm, "w");

 if (fn_asmi == NULL)
  f_asmi = NULL;
 else
  f_asmi = fopen (fn_asmi, "w");

 if (fn_asml == NULL)
  f_asml = NULL;
 else
  f_asml = fopen (fn_asml, "w");

 if (fn_x86 == NULL)
  f_x86 = NULL;
 else
  f_x86 = fopen (fn_x86, "w");

 if (f_tasm != NULL)
 {
  fprintf (f_tasm, ".model tiny\n");
  fprintf (f_tasm, ".stack 100h\n");
  fprintf (f_tasm, ".code\n");
  fprintf (f_tasm, "start proc\n");
  fprintf (f_tasm, " mov ax,cs\n");
  fprintf (f_tasm, " mov ds,ax\n");
  fprintf (f_tasm, " call main\n");
  fprintf (f_tasm, " mov ah,0\n");
  fprintf (f_tasm, " int 21h\n");
  fprintf (f_tasm, "start endp\n");
  fprintf (f_tasm, "main proc\n");
 }

 if (f_asmi != NULL)
 {
  fprintf (f_asmi, " .intel_syntax\n");
  fprintf (f_asmi, " .text\n");
/*
  fprintf (f_asml, ".globl start\n");
  fprintf (f_asml, " .type start, @function\n");
  fprintf (f_asml, "start:\n");
  fprintf (f_asml, " mov %%cs,%%ax\n");
  fprintf (f_asml, " mov %%ax,%%ds\n");
  fprintf (f_asml, " call main\n");
  fprintf (f_asml, " ret\n");
*/
  fprintf (f_asmi, ".globl main\n");
  fprintf (f_asmi, " .type main, @function\n");
  fprintf (f_asmi, "main:\n");
  fprintf (f_asmi, " mov %%ax,%%cs\n");
  fprintf (f_asmi, " mov %%ds,%%ax\n");

 }

 if (f_asml != NULL)
 {
  fprintf (f_asml, " .text\n");
  fprintf (f_asml, ".globl start\n");
  fprintf (f_asml, " .type start, @function\n");
  fprintf (f_asml, "start:\n");
  fprintf (f_asml, " mov %%cs,%%ax\n");
  fprintf (f_asml, " mov %%ax,%%ds\n");
  fprintf (f_asml, " call main\n");
  fprintf (f_asml, " ret\n");
  fprintf (f_asml, ".globl main\n");
  fprintf (f_asml, " .type main, @function\n");
  fprintf (f_asml, "main:\n");
 }

 if (f_x86 != NULL)
 {
  fprintf (f_x86, " .text\n");
/*
  fprintf (f_x86, ".globl start\n");
  fprintf (f_x86, " .type start, @function\n");
  fprintf (f_x86, "start:\n");
  fprintf (f_x86, " .byte 0x8C, 0xC8, 0x8E, 0xD8\n");
  fprintf (f_x86, " call main\n");
  fprintf (f_x86, " .byte 0xC3\n");
*/
  fprintf (f_x86, ".globl main\n");
  fprintf (f_x86, " .type main, @function\n");
  fprintf (f_x86, "main:\n");
  fprintf (f_x86, " .byte 0x8C, 0xC8, 0x8E, 0xD8\n");
 }

 for (;;)
 {
  fgets (line, sizeof(line), f);
  if (feof(f))
   break;
  cp = compile_string (cp, line);
 }

 if (f_tasm != NULL)
 {
  /*
  fprintf (f_tasm, " mov ah,0\n");
  fprintf (f_tasm, " int 21h\n");
  */
  fprintf (f_tasm, "main endp\n");
  fprintf (f_tasm, "end start\n");
  fclose (f_tasm);
  f_tasm = NULL;
 }

 if (f_asml != NULL)
 {
  fprintf (f_asml, "\n");
 }

 return cp;
}

void init_code (void)
{
 codeptr cp;
 fillmem.cp = fillmem_code;
 cp = fillmem_code;
#ifdef COMPARM
 *cp++ = 0xE92D0003; /* PUSH RD 3 */
 *cp++ = 0xE59E0000; /* R0=[RE] */
 *cp++ = 0xE28EE004; /* RE=RE+4 */
 *cp++ = 0xE2400001; /* loop: R0=R0-1 */
 *cp++ = 0xE1B00000; /* R0?=R0 */
 *cp++ = 0xDA000003; /* <0 GOTO fin (+3) */
 *cp++ = 0xE59E1000; /* R1=[RE] */
 *cp++ = 0xE28EE004; /* RE=RE+4 */
 *cp++ = 0xE58C1004; /* [RC++]=R1 */
 *cp++ = 0xEAFFFFF8; /* GOTO loop (-8) */
 *cp++ = 0xE8BD0003; /* POP RD 3 */
 *cp++ = 0xE1A0F00E; /* RF=RE */
#endif
#ifdef COMPX86
 *cp++ = 0x53; /* PUSH BX */
 *cp++ = 0x51; /* PUSH CX */
 *cp++ = 0x56; /* PUSH SI */
 *cp++ = 0x89; *cp++ = 0xE3; /* MOV BX,SP */
 *cp++ = 0x8B; *cp++ = 0x77; *cp++ = 0x06; /* MOV SI,[BX+6] */
 *cp++ = 0x8A; *cp++ = 0x0C; /* MOV CL,[SI] */
 *cp++ = 0xB5; *cp++ = 0x00; /* MOV CH,0 */
 *cp++ = 0x46; /* INC SI */
 *cp++ = 0xFC; /* CLD */
 *cp++ = 0xF3; *cp++ = 0xA4; /* REP MOVSB */
 *cp++ = 0x89; *cp++ = 0x77; *cp++ = 0x06; /* MOV [BX+6],SI */
 *cp++ = 0x5E; /* POP SI */
 *cp++ = 0x59; /* POP CX */
 *cp++ = 0x5B; /* POP BX */
 *cp++ = 0xC3; /* RET */
#endif
}

void init (void)
{
int i;
 for (i=0; i<256; i++)
  lab[i] = 0;
 init_code ();
}

int main (int argc, char *argv[])
{
any ucode;
int i;
codeptr cp;
char bufasm[300];
char buf[100];
 printf ("\n#0\n");
 init ();
 ucode.cp = cod;
/*
#ifdef PPC
 printf ("First pass\n");
 cp = compile_file (cod, "\\pgcc\\samp\\cvm\\test3.txt", NULL, NULL, NULL, NULL);
 printf ("Second pass\n");
 cp = compile_file (cod, "\\pgcc\\samp\\cvm\\test3.txt",
  "\\pgcc\\samp\\cvm\\test3.asm",
  "\\pgcc\\samp\\cvm\\test3i.s",
  "\\pgcc\\samp\\cvm\\test3.s",
  "\\pgcc\\samp\\cvm\\test3x.s");
 printf ("Done\n");
#else
 printf ("First pass\n");
 cp = compile_file (cod, "test3.txt", NULL, NULL, NULL, NULL);
 printf ("Second pass\n");
 cp = compile_file (cod,
  "test3.txt",
  "test3.asm",
  "test3i.s",
  "test3.s",
  "test3x.s");
 printf ("Done\n");
#endif
*/

#ifdef PPC
 printf ("First pass\n");
 cp = compile_file (cod, "cvm.txt", NULL, NULL, NULL, NULL);
 printf ("Program size: %u = 0x%X instructions\n", cp-cod, cp-cod);
 printf ("Second pass\n");
 cp = compile_file (cod, "cvm.txt", "cvm.asm", NULL, NULL, NULL);
 printf ("Done\n");
#else
 printf("argc=%X\n",argc);

 printf ("file %s\n",argv[2]);

 printf ("First pass\n");
 cp = compile_file (cod, argv[2], NULL, NULL, NULL, NULL);
 printf ("Program size: %u = 0x%X instructions\n", cp-cod, cp-cod);
 printf ("Second pass\n");
 if (argc >= 3)
  cp = compile_file (cod, argv[2], argv[3], NULL, NULL, NULL);
 else
  cp = compile_file (cod, argv[2], NULL, NULL, NULL, NULL);
 printf ("Done\n");
#endif

 /* cp = compile_prog (cod); */

 printf ("Machine code:\n");
#ifdef INTERP
 for (i=0; i<cp-cod; i++) printf ("%3d: %08lX\n", i, cod[i]);
#else

 for (i=0; i<cp-cod; i++)
 {
#ifdef COMPARM
  printasm (bufasm, (unsigned int)(ucode.cp[i]), (unsigned int)(ucode.cp+i));
  printf ("%8X: %08lX %s\n", ucode.cp+i, ucode.cp[i], bufasm);
  if (i%16==15) gets(buf);
#else
  if (i%8 == 0)
   printf ("\n%3X: ", i);
  printf ("%02X ", cod[i]);
#endif
 }
 printf ("\n");
#endif

#ifndef PPC
 if (argv[1][0] == 'r' || argv[1][0] == 'd')
#endif
 {
#ifdef INTERP
#ifdef PPC
 char buf[100];
 printf("debug?");
 gets(buf);
 if(*buf=='y')
  debug();
 else
  interp();
#else
  if (argv[1][0] == 'r')
   interp ();
  else
   debug ();
#endif
#else
  (*(ucode.f))();
#endif
 }
 return 0;

 gets(buf);
 printf("\n#1\n");
 ucode.cp = cod;
 printf("\n#2\n");
 cp = compile_prog (cod);
 printf("\n#3\n");
 for (i=0; i<=cp-cod; i++)
 {
#ifdef COMPARM
  printasm (bufasm, (unsigned int)(ucode.cp[i]), (unsigned int)(ucode.cp+i));
#else
  bufasm[0] = 0;
#endif
  printf ("%X: %08lX %s\n", ucode.cp+i, ucode.cp[i], bufasm);
  if (i%16==15) gets(buf);
 }
 printf("\n#4\n");

#ifdef INTERP
 interp();
#else
 (*(ucode.f))();
#endif
 printf("\n#5\n");
}

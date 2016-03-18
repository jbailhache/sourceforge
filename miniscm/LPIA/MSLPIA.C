/*
 *      ---------- Mini-Scheme Interpreter Version 0.85 ----------
 *
 *                coded by Atsushi Moriwaki (11/5/1989)
 *
 *            E-MAIL :  moriwaki@kurims.kurims.kyoto-u.ac.jp
 *
 *               THIS SOFTWARE IS IN THE PUBLIC DOMAIN
 *               ------------------------------------
 * This software is completely free to copy, modify and/or re-distribute.
 * But I would appreciate it if you left my name on the code as the author.
 *
 */
/*--
 *
 *  This version has been modified by R.C. Secrist and Jacques Bailhache.
 *
 *  Mini-Scheme is now maintained by Akira KIDA.
 *
 *  This is a revised and modified version by Akira KIDA.
 *	current version is 0.85k4 (15 May 1994)
 *
 *  Please send suggestions, bug reports and/or requests to:
 *		<SDI00379@niftyserve.or.jp>
 *--
 */

/* #define TRACE */

#define LPIA

/* int step = 0; */

/*
 * Here is System declaration.
 * Please define exactly one symbol in the following section.
 */
/* #define LSC		*/	/* LightSpeed C for Macintosh */
/* #define LSC4		*/	/* THINK C version 4.0 for Macintosh */
/* #define MPW2		*/	/* Macintosh Programmer's Workshop v2.0x */
/* #define BSD		*/	/* 4.x BSD */
/* #define MSC		*/	/* Microsoft C Compiler v.4.00 - 7.00 */
/* #define TURBOC	*/	/* Turbo C compiler v.2.0, or TC++ 1.0  */
/* #define SYSV		*/	/* System-V, or POSIX */
/* #define VAXC		*/	/* VAX/VMS VAXC 2.x or later */ /* (automatic) */

#ifdef __BORLANDC__	/* Borland C++ - MS-DOS */
#define TURBOC
#endif

#ifdef __TURBOC__	/* Turbo C V1.5 - MS-DOS */
#define TURBOC
#endif

#ifdef mips		/* DECstation running OSF/1 */
#define BSD
#endif

#ifdef __osf__		/* Alpha AXP running OSF/1 */
#define BSD
#endif

#ifdef __DECC		/* Alpha AXP running VMS */
#define VAXC
#endif

#ifdef _AIX		/* RS/6000 running AIX */
#define BSD
#endif

/*
 * Define or undefine following symbols as you need.
 */
/* #define VERBOSE */	/* define this if you want verbose GC */
#define	AVOID_HACK_LOOP	/* define this if your compiler is poor
			 * enougth to complain "do { } while (0)"
			 * construction.
			 */
#define USE_SETJMP	/* undef this if you do not want to use setjmp() */
#define USE_QQUOTE	/* undef this if you do not need quasiquote */
#define USE_MACRO	/* undef this if you do not need macro */

	
#ifdef USE_QQUOTE
/*--
 *  If your machine can't support "forward single quotation character"
 *  i.e., '`',  you may have trouble to use backquote.
 *  So use '^' in place of '`'.
 */
# define BACKQUOTE '`'
#endif

/*
 *  Basic memory allocation units
 */

#ifdef TURBOC             	/* rcs */
#define CELL_SEGSIZE  2048
#define CELL_NSEGMENT  100
#define STR_SEGSIZE   2048
#define STR_NSEGMENT   100
#else
#define CELL_SEGSIZE    50000	/* # of cells in one segment */
#define CELL_NSEGMENT   1000	/* # of segments for cells */
#define STR_SEGSIZE     2500	/* bytes of one string segment */
#define STR_NSEGMENT    100	/* # of segments for strings */
#endif



#define banner "Hello, This is Mini-Scheme Interpreter Version 0.85k4-a.\n"


#include <stdio.h>
#include <ctype.h>
#ifdef USE_SETJMP
#include <setjmp.h>
#endif

#include <errno.h>

/* System dependency */
#ifdef LSC
#include <strings.h>
#include <unix.h>
#define malloc(x)	NewPtr((long)(x))
#define prompt "> "
#define InitFile "init.scm"
#define FIRST_CELLSEGS 5
#endif

#ifdef LSC4
#include <string.h>
#include <stdlib.h>
#define malloc(x)	NewPtr((long)(x))
#define prompt "> "
#define InitFile "init.scm"
#define FIRST_CELLSEGS 5
#endif

#ifdef MPW2
#include <strings.h>
#include <memory.h>
#define malloc(x)	NewPtr((long)(x))
#define prompt "> [enter at next line]\n"
#define InitFile "init.scm"
#define FIRST_CELLSEGS 5
#endif

#ifdef BSD
#include <strings.h>
#include <signal.h>
#define prompt "> "
#define InitFile "init.scm"
#define FIRST_CELLSEGS 10
#endif

#ifdef MSC
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <process.h>
#define prompt "> "
#define InitFile "init.scm"
#define FIRST_CELLSEGS 3
#endif

#ifdef TURBOC
#include <string.h>
#include <stdlib.h>
#define prompt "> "
#define InitFile "init.scm"
#define FIRST_CELLSEGS 3
#endif

#ifdef SYSV
#include <string.h>
#include <malloc.h>
#if __STDC__
# include <stdlib.h>
#endif
#define prompt "> "
#define InitFile "init.scm"
#define FIRST_CELLSEGS 10
#endif

#ifdef	VAXC
#include <string.h>
#include <stdlib.h>
#define prompt "> "
#define InitFile "init.scm"
#define FIRST_CELLSEGS 10
#endif

#ifdef __GNUC__
/*
 * If we use gcc, AVOID_HACK_LOOP is unnecessary
 */
#undef AVOID_HACK_LOOP
#endif

#ifndef	FIRST_CELLSEGS
#error Please define your system type.
/*
 * We refrain this to raise an error anyway even if on pre-ANSI system.
 */
error Please define your system type.
#endif

/* cell structure */
struct cell {
	unsigned short _flag;
	union {
		struct {
			char   *_svalue;
			short   _keynum;
		} _string;
		struct {
			long    _ivalue;
		} _number;
		struct {
			struct cell *_car;
			struct cell *_cdr;
		} _cons;
	} _object;
};

typedef struct cell *pointer;

#define T_STRING         1	/* 0000000000000001 */
#define T_NUMBER         2	/* 0000000000000010 */
#define T_SYMBOL         4	/* 0000000000000100 */
#define T_SYNTAX         8	/* 0000000000001000 */
#define T_PROC          16	/* 0000000000010000 */
#define T_PAIR          32	/* 0000000000100000 */
#define T_CLOSURE       64	/* 0000000001000000 */
#define T_CONTINUATION 128	/* 0000000010000000 */
#ifdef USE_MACRO
# define T_MACRO        256	/* 0000000100000000 */
#endif
#define T_PROMISE      512	/* 0000001000000000 */
#define T_FCLOSURE    1024
#define T_MCLOSURE    2048

#define T_ATOM       16384	/* 0100000000000000 */	/* only for gc */
#define CLRATOM      49151	/* 1011111111111111 */	/* only for gc */
#define MARK         32768	/* 1000000000000000 */
#define UNMARK       32767	/* 0111111111111111 */

/* macros for cell operations */
#define type(p)         ((p)->_flag)

#define isstring(p)     (type(p)&T_STRING)
#define strvalue(p)     ((p)->_object._string._svalue)
#define keynum(p)       ((p)->_object._string._keynum)

#define isnumber(p)     (type(p)&T_NUMBER)
#define ivalue(p)       ((p)->_object._number._ivalue)

#define ispair(p)       (type(p)&T_PAIR)
#define car(p)          ((p)->_object._cons._car)
#define cdr(p)          ((p)->_object._cons._cdr)

#define CAR(p)          ((p)->_object._cons._car)
#define CDR(p)          ((p)->_object._cons._cdr)


#define issymbol(p)     (type(p)&T_SYMBOL)
#define symname(p)      strvalue(car(p))
#define hasprop(p)      (type(p)&T_SYMBOL)
#ifndef LPIA
#define symprop(p)      cdr(p)
#else
#define symdef(p)	(car(cdr(p)))
#define symprop(p)	(CAR(CDR(CDR(p))))
#endif

#define issyntax(p)     (type(p)&T_SYNTAX)
#define isproc(p)       (type(p)&T_PROC)
#define syntaxname(p)   strvalue(car(p))
#define syntaxnum(p)    keynum(car(p))
#define procnum(p)      ivalue(p)

#define isclosure(p)    (type(p)&T_CLOSURE)
#define isfclosure(p)	(type(p)&T_FCLOSURE)
#define ismclosure(p)	(type(p)&T_MCLOSURE)
#ifdef USE_MACRO
# define ismacro(p)      (type(p)&T_MACRO)
#endif
#define closure_code(p) car(p)
#define closure_env(p)  cdr(p)

#define iscontinuation(p) (type(p)&T_CONTINUATION)
#define cont_dump(p)    cdr(p)

#define ispromise(p)    (type(p)&T_PROMISE)
#define setpromise(p)   type(p) |= T_PROMISE

#define isatom(p)       (type(p)&T_ATOM)
#define setatom(p)      type(p) |= T_ATOM
#define clratom(p)      type(p) &= CLRATOM

#define ismark(p)       (type(p)&MARK)
#define setmark(p)      type(p) |= MARK
#define clrmark(p)      type(p) &= UNMARK

#define caar(p)         car(car(p))
#define cadr(p)         car(cdr(p))
#define cdar(p)         cdr(car(p))
#define cddr(p)         cdr(cdr(p))
#define cadar(p)        car(cdr(car(p)))
#define caddr(p)        car(cdr(cdr(p)))
#define cadaar(p)       car(cdr(car(car(p))))
#define cadddr(p)       car(cdr(cdr(cdr(p))))
#define cdddr(p)	cdr(cdr(cdr(p)))
#define cddddr(p)       cdr(cdr(cdr(cdr(p))))

/* arrays for segments */
pointer cell_seg[CELL_NSEGMENT];
int     last_cell_seg = -1;
char   *str_seg[STR_NSEGMENT];
int     str_seglast = -1;

#ifndef LPIA
/* We use 4 registers. */
pointer args;			/* register for arguments of function */
pointer envir;			/* stack register for current environment */
pointer code;			/* register for current code */
pointer dump;			/* stack register for next evaluation */

static pointer value;

#else

#define NREG 0x100
#define NREG1 0xF0

pointer reg[NREG];

#define champ_rac 1
#define champ_symb 2
#define champ_ctx 3
#define champ_strat 4
#define champ_prog 5
#define champ_bas 6
#define champ_haut 7
#define champ_flags 8
#define champ_acc 9
#define champ_env 10
#define champ_limnse 15

#define champ_trace 17
#define champ_nivafse 2

#define champ_newexpr 20
#define champ_newexpr_stack 21

#define champ_code 5
#define champ_args 6
#define champ_value 0x53
#define champ_dump 0x54
#define champ_envir 0x55

#define champ_step 0x59
#define champ_n_next 0x5A
#define champ_n_break 0x5B

#define champ_prog_lpia champ_prog
#define champ_bas_lpia champ_bas
#define champ_envir_lpia champ_env

#define champ_lctxs 3

#define champ_llctxs 0x14

#define args	(reg[champ_args])
#define envir	(reg[champ_envir])
#define code	(reg[champ_code])
#define dump	(reg[champ_dump])
#define value   (reg[champ_value])

#define newexpr	    (reg[champ_newexpr])
#define newexpr_stack (reg[champ_newexpr_stack])

#define code_lpia   (reg[champ_prog_lpia])
#define args_lpia   (reg[champ_bas_lpia])
#define reg_prog    (reg[champ_prog_lpia])
#define reg_bas     (reg[champ_bas_lpia])
#define envir_lpia  (reg[champ_envir_lpia])

/* #define env envir_lpia */

#define haut (reg[champ_haut])

#define lctxs (reg[champ_lctxs])
#define strat (reg[champ_strat])

#define llctxs (reg[champ_llctxs])

#define step (ivalue(reg[champ_step]))
#define n_next (ivalue(reg[champ_n_next]))
#define n_break (ivalue(reg[champ_n_break]))

#define MIN_CTX 4
#define MAX_CTX 15

#endif

struct cell _NIL;
pointer NIL = &_NIL;		/* special cell representing empty cell */
struct cell _T;
pointer T = &_T;		/* special cell representing #t */
struct cell _F;
pointer F = &_F;		/* special cell representing #f */

struct cell _VAR, _NOM_VAN, _VAN;
pointer VAR = &_VAR, 
        NOM_VAN = &_NOM_VAN,
        VAN = &_VAN;

pointer UNDEF, LPIA_QUOTE, GETVSQ, UNDEFINED, S_EOF, _UDF, _BRK;

pointer oblist = &_NIL;		/* pointer to symbol table */
pointer global_env;		/* pointer to global environment */

/* global pointers to special symbols */
pointer LAMBDA;			/* pointer to syntax lambda */
pointer QUOTE;			/* pointer to syntax quote */

#ifdef USE_QQUOTE
pointer QQUOTE;			/* pointer to symbol quasiquote */
pointer UNQUOTE;		/* pointer to symbol unquote */
pointer UNQUOTESP;		/* pointer to symbol unquote-splicing */

#endif

pointer free_cell = &_NIL;	/* pointer to top of free cells */
long    fcells = 0;		/* # of free cells */

FILE   *infp;			/* input file */
FILE   *outfp;			/* output file */

#ifdef USE_SETJMP
jmp_buf error_jmp;

#endif
char    gc_verbose;		/* if gc_verbose is not zero, print gc status */


pointer fcar (pointer x)
{
    if (ispair(x) || x == NIL)
	return CAR(x);
    break_debug();
    fprintf (outfp, "\nCAR of non cons : ");
    print (x);
    fprintf (outfp, "\n");
    return x;
}

pointer fcdr (pointer x)
{
    if (ispair(x) || x == NIL)
	return CDR(x);
    break_debug();
    fprintf (outfp, "\nCDR of non cons : ");
    print (x);
    fprintf (outfp, "\n");
    return x;
}

/* allocate new cell segment */
alloc_cellseg(n)
int     n;
{
	register pointer p;
	register long i;
	register int k;

	for (k = 0; k < n; k++) {
		if (last_cell_seg >= CELL_NSEGMENT - 1)
			return k;
		p = (pointer) malloc(CELL_SEGSIZE * sizeof(struct cell));
		if (p == (pointer) 0)
			return k;
		cell_seg[++last_cell_seg] = p;
		fcells += CELL_SEGSIZE;
		for (i = 0; i < CELL_SEGSIZE - 1; i++, p++) {
			type(p) = 0;
			car(p) = NIL;
			cdr(p) = p + 1;
		}
		type(p) = 0;
		car(p) = NIL;
		cdr(p) = free_cell;
		free_cell = cell_seg[last_cell_seg];
	}
	return n;
}

/* allocate new string segment */
alloc_strseg(n)
int     n;
{
	register char *p;
	register long i;
	register int k;

	for (k = 0; k < n; k++) {
		if (str_seglast >= STR_NSEGMENT)
			return k;
		p = (char *) malloc(STR_SEGSIZE * sizeof(char));
		if (p == (char *) 0)
			return k;
		str_seg[++str_seglast] = p;
		for (i = 0; i < STR_SEGSIZE; i++)
			*p++ = (char) (-1);
	}
	return n;
}

#ifdef LPIA
init_lpia ()
{
int i;
    for (i=0; i < NREG1; i++)
	reg[i] = NIL;
    init_varsys ();
    envir_lpia = T;
}

#endif

/* initialization of Mini-Scheme */
init_scheme()
{
	register pointer i;

	if (alloc_cellseg(FIRST_CELLSEGS) != FIRST_CELLSEGS)
		FatalError("Unable to allocate initial cell segments");
	if (!alloc_strseg(1))
		FatalError("Unable to allocate initial string segments");
#ifdef VERBOSE
	gc_verbose = 1;
#else
	gc_verbose = 0;
#endif
	init_globals();
#ifdef LPIA
	init_lpia ();
#endif
}

/* get new cell.  parameter a, b is marked by gc. */
pointer get_cell(a, b)
register pointer a, b;
{
	register pointer x;

	if (free_cell == NIL) {
		gc(a, b);
		if (free_cell == NIL)
#ifdef USE_SETJMP
			if (!alloc_cellseg(1)) {
				args = envir = code = dump = NIL;
#ifdef LPIA
				{
				int i;
				    for (i=0; /*i < sizeof(reg)/sizeof(reg[0]);
					       */ 
					 i < NREG1; i++)
					reg[i] = NIL;

				}
#endif
				gc(NIL, NIL);
				if (free_cell != NIL)
					Error("run out of cells --- rerurn to top level");
				else
					FatalError("run out of cells --- unable to recover cells");
			}
#else
			if (!alloc_cellseg(1))
				FatalError("run out of cells  --- unable to recover cells");
#endif
	}
	x = free_cell;
	free_cell = cdr(x);
	--fcells;
	return (x);
}

reset_newexpr ()
{
    newexpr = NIL;
}

/* get new cons cell */
pointer cons1(a, b)
register pointer a, b;
{
	register pointer x = get_cell(a, b);

	if (a->_flag == 0 || b->_flag == 0)
	    break_debug();

	type(x) = T_PAIR;
	car(x) = a;
	cdr(x) = b;
	return (x);
}

add_newexpr (pointer x)
{
    newexpr = cons1 (x, newexpr);
}

push_newexpr (pointer x)
{
    newexpr_stack = cons1 (x, newexpr_stack);
}

pop_newexpr ()
{
    newexpr_stack = fcdr (newexpr_stack);
}

pointer cons (pointer a, pointer b)
{
pointer c;
    c = cons1 (a, b);
    add_newexpr (c);
    return c;
}

/* get number atom */
pointer mk_number(num)
register long num;
{
	register pointer x = get_cell(NIL, NIL);

	type(x) = (T_NUMBER | T_ATOM);
	ivalue(x) = num;
	return (x);
}

pointer mk_operator (long op)
{
pointer y;
	y = get_cell(NIL, NIL);
	type(y) = (T_PROC | T_ATOM);
	ivalue(y) = (long) op;
	return y;
}

/* allocate name to string area */
char   *store_string(name)
char   *name;
{
	register char *q;
	register short i;
	long    len, remain;

	/* first check name has already listed */
	for (i = 0; i <= str_seglast; i++)
		for (q = str_seg[i]; *q != (char) (-1); ) {
			if (!strcmp(q, name))
				goto FOUND;
			while (*q++)
				;	/* get next string */
		}
	len = strlen(name) + 2;
	remain = (long) STR_SEGSIZE - ((long) q - (long) str_seg[str_seglast]);
	if (remain < len) {
		if (!alloc_strseg(1))
			FatalError("run out of string area");
		q = str_seg[str_seglast];
	}
	strcpy(q, name);
FOUND:
	return (q);
}

/* get new string */
pointer mk_string(str)
char   *str;
{
	register pointer x = get_cell(NIL, NIL);

	strvalue(x) = store_string(str);
	type(x) = (T_STRING | T_ATOM);
	keynum(x) = (short) (-1);
	return (x);
}

/* get new symbol */
pointer mk_symbol(name)
char   *name;
{
	register pointer x;

	/* fisrt check oblist */
	for (x = oblist; x != NIL; x = cdr(x))
		if (!strcmp(name, symname(car(x))))
			break;

	if (x != NIL)
		return (car(x));
	else {
#ifndef LPIA
		x = cons(mk_string(name), NIL);
#else
		x = cons (mk_string(name),
		     cons (NIL,
		      cons (NIL, NIL) ) );
#endif
		type(x) = T_SYMBOL;
		oblist = cons(x, oblist);
		return (x);
	}
}

/* make symbol or number atom from string */
pointer mk_atom(q)
char   *q;
{
	char    c, *p;

	p = q;
	if (!isdigit(c = *p++)) {
		if ((c != '+' && c != '-') || !isdigit(*p))
			return (mk_symbol(q));
	}
	for ( ; (c = *p) != 0; ++p)
		if (!isdigit(c))
			return (mk_symbol(q));
	return (mk_number(atol(q)));
}

/* make constant */
pointer mk_const(name)
char   *name;
{
	long    x;
	char    tmp[256];

	if (!strcmp(name, "t"))
		return (T);
	else if (!strcmp(name, "f"))
		return (F);
	else if (*name == 'o') {/* #o (octal) */
		sprintf(tmp, "0%s", &name[1]);
		sscanf(tmp, "%lo", &x);
		return (mk_number(x));
	} else if (*name == 'd') {	/* #d (decimal) */
		sscanf(&name[1], "%ld", &x);
		return (mk_number(x));
	} else if (*name == 'x') {	/* #x (hex) */
		sprintf(tmp, "0x%s", &name[1]);
		sscanf(tmp, "%lx", &x);
		return (mk_number(x));
	} else if (!strcmp (name, "\\space")) {
		return (mk_number(' '));
	} else if (!strcmp (name, "\\tab")) {
		return (mk_number('\t'));
	} else
		return (NIL);
}


/* ========== garbage collector ========== */

/*--
 *  We use algorithm E (Kunuth, The Art of Computer Programming Vol.1,
 *  sec.3.5) for marking.
 */
mark(a)
pointer a;
{
	register pointer t, q, p;

#ifdef LPIA
	if (a == NIL)
	    return;
#endif
	
E1:	t = (pointer) 0;
	p = a;
E2:	setmark(p);
E3:	if (isatom(p))
		goto E6;
E4:	q = car(p);
	if (q && !ismark(q)) {
		setatom(p);
		car(p) = t;
		t = p;
		p = q;
		goto E2;
	}
E5:	q = cdr(p);
	if (q && !ismark(q)) {
		cdr(p) = t;
		t = p;
		p = q;
		goto E2;
	}
E6:	if (!t)
		return;
	q = t;
	if (isatom(q)) {
		clratom(q);
		t = car(q);
		car(q) = p;
		p = q;
		goto E5;
	} else {
		t = cdr(q);
		cdr(q) = p;
		p = q;
		goto E6;
	}
}


/* garbage collection. parameter a, b is marked. */
gc(a, b)
register pointer a, b;
{
	register pointer p;
	register short i;
	register long j;

	if (gc_verbose)
		printf("gc...");

	/* mark system globals */
	mark(oblist);
	mark(global_env);

	/* mark current registers */
#ifndef LPIATEST
#ifdef LPIA
	{
	int i;
	    for (i=0; i < NREG1; i++)
		mark(reg[i]);
	}
#else
	mark(args);
	mark(envir);
	mark(code);
	mark(dump);
#endif
#else
	{
	int i;
	    for (i=0; i < NREG1; i++)
		mark(reg[i]);
	}
	/* verify if there is no problem to mark value */
#endif

	/* mark variables a, b */
	mark(a);
	mark(b);

	/* garbage collect */
	clrmark(NIL);
	fcells = 0;
	free_cell = NIL;
	for (i = 0; i <= last_cell_seg; i++) {
		for (j = 0, p = cell_seg[i]; j < CELL_SEGSIZE; j++, p++) {
			if (ismark(p))
				clrmark(p);
			else {
				type(p) = 0;
				cdr(p) = free_cell;
				car(p) = NIL;
				free_cell = p;
				++fcells;
			}
		}
	}

	if (gc_verbose)
		printf(" done %ld cells are recovered.\n", fcells);
}


/* ========== Rootines for Reading ========== */

#define TOK_LPAREN  0
#define TOK_RPAREN  1
#define TOK_DOT     2
#define TOK_ATOM    3
#define TOK_QUOTE   4
#define TOK_COMMENT 5
#define TOK_DQUOTE  6
#ifdef USE_QQUOTE
# define TOK_BQUOTE  7
# define TOK_COMMA   8
# define TOK_ATMARK  9
#endif
#define TOK_SHARP   10
#define TOK_2P	    11

#define LINESIZE 1024
char    linebuff[LINESIZE];
char    strbuff[256];
char   *currentline = linebuff;
char   *endline = linebuff;

/* get new character from input file */
int     inchar()
{
	if (currentline >= endline) {	/* input buffer is empty */
		if (feof(infp)) {
			fclose(infp);
			infp = stdin;
			printf(prompt);
		}
		strcpy(linebuff, "\n");
		if (fgets(currentline = linebuff, LINESIZE, infp) == NULL)
			if (infp == stdin) {
				fprintf(stderr, "Good-bye\n");
				exit(0);
			}
		endline = linebuff + strlen(linebuff);
	}
#ifdef TRACE
	putchar (*currentline);
#endif
	return (*currentline++);
}

/* clear input buffer */
clearinput()
{
	currentline = endline = linebuff;
}

/* back to standard input */
flushinput()
{
	if (infp != stdin) {
		fclose(infp);
		infp = stdin;
	}
	clearinput();
}

/* back character to input buffer */
backchar()
{
	currentline--;
}

/* read chacters to delimiter */
char   *readstr(delim)
char   *delim;
{
	char   *p = strbuff;

	while (isdelim(delim, (*p++ = inchar())))
		;
	backchar();
	*--p = '\0';
	return (strbuff);
}

/* read string expression "xxx...xxx" */
char   *readstrexp()
{
	char    c, *p = strbuff;

	for (;;) {
		if ((c = inchar()) != '"')
			*p++ = c;
		else if (p > strbuff && *(p - 1) == '\\')
			*(p - 1) = '"';
		else {
			*p = '\0';
			return (strbuff);
		}
	}
}

/* check c is delimiter */
isdelim(s, c)
char   *s;
char    c;
{
	while (*s)
		if (*s++ == c)
			return (0);
	return (1);
}

/* skip white characters */
skipspace()
{
	while (isspace(inchar()))
		;
	backchar();
}

/* get token */
token()
{
	skipspace();
	switch (inchar()) {
	case '(':
		return (TOK_LPAREN);
	case ')':
		return (TOK_RPAREN);
	case '.':
		return (TOK_DOT);
	case '\'':
		return (TOK_QUOTE);
	case ';':
		return (TOK_COMMENT);
	case '"':
		return (TOK_DQUOTE);
#ifdef USE_QQUOTE
	case BACKQUOTE:
		return (TOK_BQUOTE);
	case ',':
		if (inchar() == '@')
			return (TOK_ATMARK);
		else {
			backchar();
			return (TOK_COMMA);
		}
#endif
	case '#':
		return (TOK_SHARP);
	/* case ':':
		return (TOK_2P); */
	default:
		backchar();
		return (TOK_ATOM);
	}
}

/* ========== Rootines for Printing ========== */
#define	ok_abbrev(x)	(ispair(x) && cdr(x) == NIL)

char *(keyword[1000]);

strunquote(p, s)
char *p;
char *s;
{
	*p++ = '"';
	for ( ; *s; ++s) {
		if (*s == '"') {
			*p++ = '\\';
			*p++ = '"';
		} else if (*s == '\n') {
			*p++ = '\\';
			*p++ = 'n';
		} else
			*p++ = *s;
	}
	*p++ = '"';
	*p = '\0';
}


#define N_VARSYS 0x100

#define VARSYS_AUTOFLUSH    0x11
#define VARSYS_ECHO	0x12
#define VARSYS_GCALARM	    0x13
#define VARSYS_PROMPT	    0x14
#define VARSYS_PRINTLEVEL   0x15
#define VARSYS_ECRIRESEXPR  0x16
#define VARSYS_INTERRUPT    0x17
#define VARSYS_NSEXPRS	    0x18
#define VARSYS_MAXNSEXPRS   0x19
#define VARSYS_MASQUE_EQ    0x1A
#define VARSYS_UNITE_ALLOC  0x1B

#define VARSYS_AUTOID	    0x21
#define VARSYS_AUTOSCHEDULE 0x22
#define VARSYS_SCHEDULEALARM	0x23
#define VARSYS_ERRFAT	    0x24
#define VARSYS_NDF	0x25
#define VARSYS_MARKPROG	    0x26

#define VARSYS_DEBUG	    0x31
#define VARSYS_NIVEAU_IMP_DEBUG 0x32
#define VARSYS_DEBUG_CONT   0x33
#define VARSYS_DEBUG_IMP    0x34
#define VARSYS_DEBUG_EXEC   0x35
#define VARSYS_DEBUG_CHMD   0x36

#define VARSYS_GARDER_VARS  0x41
#define VARSYS_OCCUR_CHECK  0x42
#define VARSYS_ADDV	0x43
#define VARSYS_UNBOUND	    0x44
#define VARSYS_UNDEF	    0x45

#define MAXVARSYS 0x50

#define ECRIRESEXPR_DPM	    1
#define ECRIRESEXPR_DPD	    2
#define ECRIRESEXPR_MARQUE  4
#define ECRIRESEXPR_MARQUELISTE 8
#define ECRIRESEXPR_DEC	    0x10
#define ECRIRESEXPR_CHAINE  0x20

int tab_varsys[N_VARSYS];

#define flag_autoflush	    (tab_varsys[VARSYS_AUTOFLUSH])
#define niveau_ecrire_sexpr	(tab_varsys[VARSYS_PRINTLEVEL])

#define sexpr pointer

#define integer int

#define ent_sexpr ivalue
#define conv_ent_sexpr ivalue

#define REGVARSYS 0x80

#define VARSYS(v) (ivalue(reg[REGVARSYS+(v)]))

#define PRINTLEVEL VARSYS(VARSYS_PRINTLEVEL)

init_varsys ()
{
sexpr x;
int i;
    tab_varsys[VARSYS_AUTOFLUSH] = 1;
    tab_varsys[VARSYS_GCALARM] = 1;
    tab_varsys[VARSYS_PROMPT] = 0;
    tab_varsys[VARSYS_PRINTLEVEL] = 3 /*0x100*/;
    tab_varsys[VARSYS_ECRIRESEXPR] =
	ECRIRESEXPR_DPM |
	ECRIRESEXPR_MARQUE |
	ECRIRESEXPR_MARQUELISTE |
	ECRIRESEXPR_CHAINE;
    tab_varsys[VARSYS_INTERRUPT] = 0;
    tab_varsys[VARSYS_NSEXPRS] = 0;
    /* tab_varsys[VARSYS_MASQUE_EQ] = 
	conv_ent_sexpr (MK_SEXPR (0, 0, 1, 1, 0xF, 0xFFFFFF)) >> 24; */ /* a verifier */
    tab_varsys[VARSYS_UNITE_ALLOC] = 2 * sizeof (integer); /* a verifier */
    tab_varsys[VARSYS_DEBUG] = 0;
    tab_varsys[VARSYS_AUTOID] = 0;
    tab_varsys[VARSYS_AUTOSCHEDULE] = 14;
    /* bit 1 : toujours -> 0
       bit 2 : pour SETCTX -> 1
       bit 4 : pour SETPRIO -> 1
       bit 8 : pour SETLX et ESI -> 1
    */
    tab_varsys[VARSYS_SCHEDULEALARM] = 0; /* bit 1 : avant boucle
			 bit 2 : apres */
    tab_varsys[VARSYS_ECHO] = 0;
    tab_varsys[VARSYS_ERRFAT] = 1; /* 1(eof)+2(lctxs vide)+4(prog vide) */
    tab_varsys[VARSYS_NDF] = 0;
    tab_varsys[VARSYS_MARKPROG] = 0;
    tab_varsys[VARSYS_NIVEAU_IMP_DEBUG] = 20;
#ifdef BIG_ENDIAN
    tab_varsys[VARSYS_DEBUG_CONT] = 'c' << 16 /*'c\0\0'*/; /* a verifier */
    tab_varsys[VARSYS_DEBUG_IMP] = 'i' << 16 /*'i\0\0'*/;
    tab_varsys[VARSYS_DEBUG_EXEC] = 'e' << 16 /*'e\0\0'*/;
    tab_varsys[VARSYS_DEBUG_CHMD] = 'd' << 16 /*'d\0\0'*/;
#else
    tab_varsys[VARSYS_DEBUG_CONT] = 'c';
    tab_varsys[VARSYS_DEBUG_IMP] = 'i';
    tab_varsys[VARSYS_DEBUG_EXEC] = 'e';
    tab_varsys[VARSYS_DEBUG_CHMD] = 'd';
#endif
    tab_varsys[VARSYS_GARDER_VARS] = 0;
    tab_varsys[VARSYS_OCCUR_CHECK] = 1;
    tab_varsys[VARSYS_ADDV] = 1;
    tab_varsys[VARSYS_UNBOUND] = 0x8000; /* a verifier */
    tab_varsys[VARSYS_UNDEF] = 0x80AE00;
    
    for (i=0; i<MAXVARSYS; i++)
    {
	reg[REGVARSYS+i] = mk_number (tab_varsys[i]);
    }

    reg[champ_step] = mk_number(0);
    reg[champ_n_next] = mk_number(0);
    reg[champ_n_break] = mk_number(0);
}


char special_chars[] = "().-{}'%&@:\"`,;# \t\n\r\014";

int is_special (int c)
{
int i;
    for (i=0; i<sizeof(special_chars); i++)
	if (c == special_chars[i])
	    return 1;
    return 0;
}

int is_digit (char c)
{
    if (c >= '0' && c <= '9')
	return 1;
    return 0;
}

int is_letter (char c)
{
    if (is_digit (c))
	return 0;
    if (is_special (c))
	return 0;
    return 1;
/*
    if (c >= 'A' && c <= 'Z')
	return 1;
    if (c >= 'a' && c <= 'z')	
	return 1;
    if (c == '_' || c == '$' || c == '%')
	return 1;
    return 0;
*/
}

int is_alphanum (char c)
{
    if (is_letter(c))
	return 1;
    if (is_digit(c))
	return 1;
    if (c == '-')
	return 1;
    return 0;
}

int is_blank (char c)
{
    if (c == ' ' || c == '\t' || c == '\n' || c == '\014' || c == '#')
	return 1;
    return 0;
}

#define TYPE_IDENT 1
#define TYPE_NUM 2
#define TYPE_KEYWORD 3
#define TYPE_SEP 4
#define TYPE_STRING 5
#define TYPE_EOF 8

struct unit
{
    int type;
    int ltext;
    char text[300];
};

struct param_read
{
    int cur_char;
    int (*get_char) (struct param_read *p);
    FILE *in;
    struct unit unit;
    int in_comment;
    char buf[120];
    int ptr;
};

int f_get_char (struct param_read *param)
{
    return fgetc (param->in);
}

set_cur_char (struct param_read *param)
{

}

unget_char (struct param_read *param, char c)
{
    param->buf [param->ptr++] = param->cur_char;
    param->cur_char = c;
}

get_char_1 (struct param_read *param)
{
    if (param->ptr > 0)
	param->cur_char = param->buf [--(param->ptr)];
    else
	param->cur_char = param->get_char (param);
/*
    if (options & OPT_ECHO_SRC)
        putchar (param->cur_char);
    if (param->cur_char == '\n')
        param->line++;
*/

}

get_char (struct param_read *param)
{
char buf[4];
int c;
loop:
    get_char_1 (param);
/*
    if (param->cur_char >= 'A' && param->cur_char <= 'Z')
        param->cur_char += 'a' - 'A';
*/
/*
    if (is_blank(param->cur_char))
        goto loop;
*/
    if (param->cur_char == '|')
	get_char_1 (param);
    if (param->cur_char == '\\')
    {
	get_char_1 (param);
	buf[0] = param->cur_char;
	get_char_1 (param);
        buf[1] = param->cur_char;
	buf[2] = 0;
	sscanf (buf, "%x", &c);
	param->cur_char = c;
    }	
    if (!param->in_comment)
        param->unit.text[param->unit.ltext++] = param->cur_char;
}

int cur_char (struct param_read *param)
{
    return param->cur_char;
}

begin_unit (struct param_read *param)
{
    set_cur_char (param);
    param->unit.ltext = 0;
    param->unit.text[param->unit.ltext++] = param->cur_char;
    param->in_comment = 0;
}

end_unit (struct param_read *param)
{
    param->unit.ltext--;
    param->unit.text[param->unit.ltext] = 0;
}

#define tprintf(f,x) 

get_comment (struct param_read *param)
{
int level;
    param->in_comment = 1;
    level = 1;
    for (;;)
    {
	if (level <= 0)
	    break;
	if (param->cur_char == '{')
	{
	    level++;
	    tprintf ("[level %d]", level);
	    get_char (param);
        }
	else if (param->cur_char == '}')
	{
	    level--;
	    tprintf ("[level %d]", level);
	    get_char (param);
        }
        else if (param->cur_char == '(')
	{
	    get_char (param);
	    if (param->cur_char == '*')
	    {
		level++;
		tprintf ("[level %d]", level);
		get_char (param);
	    }
	}
	else if (param->cur_char == '*')
	{
	    get_char (param);
	    if (param->cur_char == ')')
	    {
	    	level--;
		tprintf ("[level %d]", level);
		get_char (param);
	    }
        }
	else
	{
	    get_char (param);
	    if (param->cur_char == EOF)
	    {
		fprintf (outfp, "\nEOF in comment\n");
		level = 0;
	    }
	}
    }
    param->in_comment = 0;
}

get_unit (struct param_read *param)
{
int i;
loop:
    begin_unit (param);
    if (param->cur_char == EOF)
    {
	param->unit.type = TYPE_EOF;
	return;
    }
    if (is_blank (param->cur_char))
    {
	get_char (param);
	goto loop;
    }
    if (param->cur_char == '(')
    {
	get_char (param);
	if (param->cur_char == '*')
        {
	    get_char (param);
	    get_comment (param);
	    goto loop;
        }
	else
	{
	    param->unit.type = TYPE_SEP;
	    /*
	    param->unit.text[0] = '(';
	    param->unit.text[1] = 0;
	    */
	    end_unit (param);
	    return;	    
	}   
    }
    if (param->cur_char == '{')
    {
	get_char (param);
	get_comment (param);
	goto loop;
    }
    if (is_letter(param->cur_char))
    {
	param->unit.type = TYPE_IDENT;
	for (i=0; ; i++)
	{
	    /* param->unit.text[i] = param->cur_char; */
	    get_char (param);
	    if (!is_alphanum(param->cur_char))
		break;
	}
	/* param->unit.text[i] = 0; */
	end_unit (param);
/*
	for (i=0; i<sizeof(keywords)/sizeof(keywords[0]); i++)
	{
	    if (!strcmp (param->unit.text, keywords[i]))
	    {
	    	param->unit.type = TYPE_KEYWORD;
		break;
	    }
	}
*/
	return;
    }
    else if (is_digit (param->cur_char))
    {
    number:
	param->unit.type = TYPE_NUM;
	for (i=0; ; i++)
	{
	    /* param->unit.text[i] = param->cur_char; */
	    get_char (param);
	    if (!is_digit(param->cur_char))
		break;
	}
	/* param->unit.text[i] = 0; */
	end_unit (param);
	return;
    }	 

    else if (param->cur_char == '-')
    {
	get_char (param);
	if (!is_digit(param->cur_char))
	{
	    param->unit.type = TYPE_SEP;
	    end_unit (param);
	}
	else
	    goto number;
    }    

    else if (param->cur_char == '"')
    {
	param->unit.type = TYPE_STRING;
	get_char (param);
	for (;;)
	{
	    if (param->cur_char == EOF)
	    {
		fprintf (outfp, "\nEOF in string\n");
		break;
	    }
	    if (param->cur_char == '"')
	    {
		get_char (param);
		if (param->cur_char == '"')
		    ;
		else
		    break;
	    }
	    else
		get_char (param);
	}
	end_unit (param);
    }    
#if 0
    else if (param->cur_char == ':')
    {
	param->unit.type = TYPE_SEP;
	get_char (param);
	if (param->cur_char == '=')
	{
	    get_char (param);
	    end_unit (param);
	}
	else if (param->cur_char == ':')
        {
            get_char (param);
            end_unit (param);
        }
	else
	    end_unit (param);
    }
    else if (param->cur_char == '[')
    {
	param->unit.type = TYPE_SEP;
	get_char (param);
	if (param->cur_char == ']')
	    get_char (param);
	end_unit (param);
    }
    else if (param->cur_char == '<')
    {
	param->unit.type = TYPE_SEP;
	get_char (param);
	if (param->cur_char == '=' || param->cur_char == '>')
	{
	    get_char (param);
	    end_unit (param);
	}
	else
	    end_unit (param);
    }
    else if (param->cur_char == '>')
    {
	param->unit.type = TYPE_SEP;
	get_char (param);
	if (param->cur_char == '=')
	{
	    get_char (param);
	    end_unit (param);
	}
	else
	    end_unit (param);
    }
    else if (param->cur_char == '.')
    {
        param->unit.type = TYPE_SEP;
        get_char (param);
        if (param->cur_char == '.')
            get_char (param);
        end_unit (param);
    }
    else if (param->cur_char == '*')
    {
        param->unit.type = TYPE_SEP;
        get_char (param);
        if (param->cur_char == '*')
            get_char (param);
        end_unit (param);
    }
#endif
    else
    {
	param->unit.type = TYPE_SEP;
	get_char (param);
	end_unit (param);
    }

}

int test_unit (struct param_read *param, int type, char *text)
{
    if (param->unit.type != type)
        return 0;
    if (text == NULL)
        return 1;
    if (strcmp (param->unit.text, text))
        return 0;
    return 1;
}
      
error1 (char *s)
{
    printf ("Error : %s\n", s);
}

verif_unit (struct param_read *param, int type, char *text)
{
    if (!test_unit (param, type, text))
    {
        printf ("Error : %d[%s] found, %d[%s] expected\n",
            /*param->line,*/ param->unit.type, param->unit.text, type, text);
        exit (0);
    }
    if (text != NULL)
        get_unit (param);
}

pointer read_sexpr (struct param_read *param);
pointer reverse(); 
pointer append();

pointer read_sexpr_file (FILE *in)
{
struct param_read param[1];
pointer x;
    param->in = in;
    param->get_char = f_get_char;
    param->ptr = 0;
    begin_unit (param);
    get_char (param);
    x = read_sexpr (param);
    return x;
}

#define text(param) ((param)->unit.text)

pointer read_sexpr1 (struct param_read *param, int flag_list);

pointer read_sexpr (struct param_read *param)
{
    get_unit (param);
    return read_sexpr1 (param, 0);    
}

pointer read_sexpr1 (struct param_read *param, int flag_list)
{
pointer x, l;
int after_dot;
char buf[1000];

loop:

    if (flag_list)
	goto read_list;

    if (test_unit (param, TYPE_EOF, NULL))
    {
	unget_char (param, ')');
	return S_EOF;
    }

    if (test_unit (param, TYPE_IDENT, NULL))
	return mk_symbol (text(param));

    if (test_unit (param, TYPE_STRING, NULL))
    {
	strcpy (buf, text(param)+1);
	buf[strlen(buf)-1] = 0;
	return mk_string (buf);
    }

    if (test_unit (param, TYPE_NUM, NULL))
    {
	return mk_number (atoi(text(param)));
    }

    if (test_unit (param, TYPE_SEP, "'"))
    {
	x = read_sexpr (param);
	return cons (LPIA_QUOTE, cons (x, NIL));
    }

    if (test_unit (param, TYPE_SEP, "%"))
    {
	x = read_sexpr (param);
	return cons (GETVSQ, cons (x, NIL));
    }

    if (test_unit (param, TYPE_SEP, "&"))
    {
	x = read_sexpr (param);
	return cons (VAR, cons (x, NIL));
    }
	
    if (test_unit (param, TYPE_SEP, "@"))
    {
        x = read_sexpr (param);
        return reverse(x);
    }

    if (test_unit (param, TYPE_SEP, ":"))
    {
	x = read_sexpr1 (param, 1);
	unget_char (param, ')');
	return x;
    }

    if (test_unit (param, TYPE_SEP, "("))
    {
    read_list:
	after_dot = 0;
	l = NIL;
	for (;;)
	{
	    get_unit (param);
	    
	    if (test_unit (param, TYPE_SEP, ")"))
	    {
		if (after_dot > 0)
		    return l;
		else
		    return reverse (l);
	    }

	    if (test_unit (param, TYPE_SEP, "."))
	    {
		x = read_sexpr (param);
		if (x == S_EOF)
		{
		    fprintf (outfp, "\nEOF before end of list\n\t");
		    if (after_dot > 0)
			print (l);
		    else
			print (reverse(l));
		    fprintf (outfp, "\n");
		}
		l = append (reverse (l), x);
		after_dot = 1 - after_dot;
	    }				
	    else if (test_unit (param, TYPE_SEP, ":"))
	    {
		x = read_sexpr1 (param, 1);
		if (x == S_EOF)
		{
		    fprintf (outfp, "\nEOF before end of list\n\t");
		    if (after_dot > 0)
			print (l);
		    else
			print (reverse(l));
		    fprintf (outfp, "\n");
		}
                if (after_dot > 0)
		{
                    l = cons (l, x);
		    return l;
		}
                else
		{
                    l = cons (x, l);
		    return reverse(l);
		}
	    }	
	    else
	    {
		x = read_sexpr1 (param, 0);
		if (x == S_EOF)
		{
		    fprintf (outfp, "\nEOF before end of list\n\t");
		    if (after_dot > 0)
			print (l);
		    else
			print (reverse(l));
		    fprintf (outfp, "\n");
		}
		if (after_dot > 0)
		    l = cons (l, x);
		else
		    l = cons (x, l); 
	    }
	}
    }
    else
    {
	fprintf (outfp, "Bad begin of expr \"%s\"\n", text(param));
	get_unit(param);
	goto loop;	
    }
}

/* print atoms */
int printatom(l, f)
pointer l;
int     f;
{
	char	*p;
	p = "???";

	if (l == NIL)
		p = "()";
	else if (l == T)
		p = "#t";
	else if (l == F)
		p = "#f";
	else if (isnumber(l)) {
		p = strbuff;
		sprintf(p, "%ld", ivalue(l));
	} else if (isstring(l)) {
		if (!f)
			p = strvalue(l);
		else {
			p = strbuff;
			if ((int)(strvalue(l)) == 8192) p = "???"; else
			strunquote(p, strvalue(l));
		}
	} else if (issymbol(l))
		p = symname(l);
	else if (isproc(l)) {
		p = strbuff;
		sprintf(p, "#<PROCEDURE %ld %s>", procnum(l), keyword[procnum(l)]);
#ifdef USE_MACRO
	} else if (ismacro(l)) {
		p = "#<MACRO>";
#endif
	} else if (isclosure(l))
		p = "#<CLOSURE>";
	else if (isfclosure(l))
		p = "#<FCLOSURE>";
	else if (ismclosure(l))
		p = "#<MCLOSURE>";
	else if (iscontinuation(l))
		p = "#<CONTINUATION>";
	if (f < 0)
		return strlen(p);
	fputs(p, outfp);
	return 0;
}


protoprint1 (pointer x, int l)
{
pointer p;
int i;
	if (l < 0)
	{
	    fprintf (outfp, " ... ");
	    return;
	}
	if (isatom(x))
	{
		/* fprintf (outfp, "<"); */
		printatom (x, 1);
		/* fprintf (outfp, ">"); */
	}
	else if (isnumber(x))
	{
	    fprintf (outfp, "%d", ivalue(x));
	}
	else if (ispair(x))
	{

		fprintf (outfp, "(");
		protoprint1 (car(x), l-1);
		for (p=cdr(x), i=0; ispair(p); p=cdr(p), i++)
		{
                	fprintf (outfp, " ");
			protoprint1 (car(p), l-1);
			if (i >= l)
			{
			    fprintf (outfp, " .... ");
			    goto endlist;
			}
		}
		if (!eqv (p, NIL))
		{
			fprintf (outfp, " . ");
			protoprint (p);
                }
	endlist:
                fprintf (outfp, ")");
        /*
		fprintf (outfp, ".");
		protoprint (cdr(x));
		fprintf (outfp, ")");
	*/
        	
	}
	else
        {
		/* fprintf (outfp, "["); */ 
		printatom(x);
		/* fprintf (outfp, "]"); */ 
	}

}

protoprint (pointer x)
{
    protoprint1 (x, PRINTLEVEL);
}

print (pointer x)
{
	protoprint (x);
}

print_nl (pointer x)
{
    print (x);
    fprintf (outfp, "\n");
}

#define code reg_prog
#define args reg_bas

printstate1 ()
{
pointer p, q;
int i, j;
	fprintf (outfp, "\nState:", n_next);

	fprintf (outfp, "\n\tcode = ");
	print (code);

	fprintf (outfp, "\n\tvalue = ");
	print (value);

	for (p=args, i=1; ispair(p); p=cdr(p), i++)
	{
		fprintf (outfp, "\n\targ %d = ", i);
		print (car(p));
	}

	for (p=envir, i=1; ispair(p); p=cdr(p), i++)
        for (q=car(p), j=1; /*ispair(q)*/ j<=1; q=cdr(q), j++)
	{
		fprintf (outfp, "\n\tvar %d %d ", i, j);
		print (caar(q));
		fprintf (outfp, " = ");
		print (cdar(q));
                
	}

	for (p=dump, i=1; ispair(p); p=cdr(cdr(cdr(p))), i++)
	{
		fprintf (outfp, "\n\tdump %d:", i);

		fprintf (outfp, "\n\t\top = ");
		print (car(dump));
		fprintf (outfp, " [%s]", keyword[ivalue(car(dump))]);

		fprintf (outfp, "\n\t\tcode = ");
		print (cadddr(dump));

		for (p=cadr(dump), i=1; ispair(p); p=cdr(p), i++)
		{
			fprintf (outfp, "\n\t\targ %d = ", i);
			print (car(p));
		}

		for (p=caddr(dump), i=1; ispair(p); p=cdr(p), i++)
		for (q=car(p), j=1; /*ispair(q)*/ j<=1; q=cdr(q), j++)
		{
			fprintf (outfp, "\n\t\tvar %d %d ", i, j);
			print (caar(q));
			
			fprintf (outfp, " = ");
			print (cdar(q));
			
		}

		for (p=lctxs, i=1; ispair(p); p=cdr(p), i++)
		{
			fprintf (outfp, "\n\tctx %d = ", i);
			protoprint1 (car(p), 3);
		}
		
	}
}

printstate ()
{
pointer p, q;
int i, j;
int level;
	level = PRINTLEVEL;
	PRINTLEVEL = 3;
	fprintf (outfp, "\nState at n_next=%d :", n_next);

	fprintf (outfp, "\n\tcode = ");
	print (code);

	fprintf (outfp, "\n\tvalue = ");
	print (value);

	for (p=args, i=1; ispair(p) && i<=3 ; p=cdr(p), i++)
	{
		fprintf (outfp, "\n\targ %d = ", i);
		print (car(p));
	}

/*
	fprintf (outfp, "\n\targs = ");
	print (p);
*/

	for (p=envir, i=1; ispair(p); p=cdr(p), i++)
        for (q=car(p), j=1; /*ispair(q)*/ j<=1; q=cdr(q), j++)
	{
		fprintf (outfp, "\n\tvar %d %d ", i, j);
		print (caar(q));
		fprintf (outfp, " = ");
		print (cdar(q));
                
	}

	for (p=dump, i=1; ispair(p) && i<=1; p=cdr(cdr(cdr(p))), i++)
	{
		fprintf (outfp, "\n\tdump %d:", i);

		fprintf (outfp, "\n\t\top = ");
		print (car(dump));
		fprintf (outfp, " [%s]", keyword[ivalue(car(dump))]);

		fprintf (outfp, "\n\t\tcode = ");
		print (cadddr(dump));

		for (p=cadr(dump), i=1; ispair(p); p=cdr(p), i++)
		{
			fprintf (outfp, "\n\t\targ %d = ", i);
			print (car(p));
		}

		for (p=caddr(dump), i=1; ispair(p); p=cdr(p), i++)
		for (q=car(p), j=1; /*ispair(q)*/ j<=1; q=cdr(q), j++)
		{
			fprintf (outfp, "\n\t\tvar %d %d ", i, j);
			print (caar(q));
			
			fprintf (outfp, " = ");
			print (cdar(q));
			
		}

	}

	for (p=lctxs, i=1; ispair(p) && i<=1; p=cdr(p), i++)
	{
		fprintf (outfp, "\n\tctx %d = ", i);
		protoprint1 (car(p), 3);
	}

	fprintf (outfp, "\n");    
	PRINTLEVEL = level;
}

#define args	(reg[champ_args])
#define code	(reg[champ_code])

/* ========== Rootines for Evaluation Cycle ========== */

/* make closure. c is code. e is environment */
pointer mk_closure(c, e)
register pointer c, e;
{
	register pointer x = get_cell(c, e);

	type(x) = T_CLOSURE;
	car(x) = c;
	cdr(x) = e;
	return (x);
}

/* make closure. c is code. e is environment */
pointer mk_fclosure(c, e)
register pointer c, e;
{
	register pointer x = get_cell(c, e);

	type(x) = T_FCLOSURE;
	car(x) = c;
	cdr(x) = e;
	return (x);
}

/* make closure. c is code. e is environment */
pointer mk_mclosure(c, e)
register pointer c, e;
{
	register pointer x = get_cell(c, e);

	type(x) = T_MCLOSURE;
	car(x) = c;
	cdr(x) = e;
	return (x);
}

/* make continuation. */
pointer mk_continuation(d)
register pointer d;
{
	register pointer x = get_cell(NIL, d);

	type(x) = T_CONTINUATION;
	cont_dump(x) = d;
	return (x);
}

/* reverse list -- make new cells */
pointer reverse(a)
register pointer a;		/* a must be checked by gc */
{
	register pointer p = NIL;

	for ( ; ispair(a); a = cdr(a))
		p = cons(car(a), p);
	return (p);
}

/* reverse list --- no make new cells */
pointer non_alloc_rev(term, list)
pointer term, list;
{
	register pointer p = list, result = term, q;

	while (p != NIL) {
		q = cdr(p);
		cdr(p) = result;
		result = p;
		p = q;
	}
	return (result);
}

/* append list -- make new cells */
pointer append(a, b)
register pointer a, b;
{
	register pointer p = b, q;

	if (a != NIL) {
		a = reverse(a);
		while (a != NIL) {
			q = cdr(a);
			cdr(a) = p;
			p = a;
			a = q;
		}
	}
	return (p);
}

/* equivalence of atoms */
eqv(a, b)
register pointer a, b;
{
	if (isstring(a)) {
		if (isstring(b))
			return (strvalue(a) == strvalue(b));
		else
			return (0);
	} else if (isnumber(a)) {
		if (isnumber(b))
			return (ivalue(a) == ivalue(b));
		else
			return (0);
	} else
		return (a == b);
}

pointer last_of_list (pointer l)
{
    if (!ispair(l))
	return l;
    if (!ispair(cdr(l)))
	return l;
    return last_of_list(cdr(l));
}

pointer getval (pointer cod)
{
pointer x, y;
		if (issymbol(cod)) {	/* symbol */
			for (x = envir; x != NIL; x = cdr(x)) {
				for (y = car(x); y != NIL; y = cdr(y))
					if (caar(y) == cod)
						break;
				if (y != NIL)
					break;
			}
			if (x != NIL) {
				return(cdar(y));
			} else {
				return NIL;
			}
		} else return NIL;
}

setval (pointer cod, pointer val)
{
pointer x, y;
		for (x = envir; x != NIL; x = cdr(x)) {
			for (y = car(x); y != NIL; y = cdr(y))
				if (caar(y) == cod)
					break;
			if (y != NIL)
				break;
		}
		if (x != NIL) {
			cdar(y) = val;
		} 
}

define (pointer cod, pointer val)
{
/*pointer x, y;
    z = getval (cod);
    if (z == nil) */

	car(envir) = cons(cons(cod, val), car(envir));
}

declsym (pointer cod)
{
pointer v;
    v = getval (cod);
    if (v == NIL)
	define (cod, NIL);
}

#if 0
	case OP_DEF0:	/* define */
		if (ispair(car(code))) {
			x = caar(code);
			code = cons(LAMBDA, cons(cdar(code), cdr(code)));
		} else {
			x = car(code);
			code = cadr(code);
		}
		if (!issymbol(x)) {
			Error_0("Variable is not symbol");
		}
		s_save(OP_DEF1, NIL, x);
		s_goto(OP_EVAL);

	case OP_DEF1:	/* define */
		for (x = car(envir); x != NIL; x = cdr(x))
			if (caar(x) == code)
				break;
		if (x != NIL)
			cdar(x) = value;
		else
			car(envir) = cons(cons(code, value), car(envir));
		s_return(code);
#endif

/* true or false value macro */
#define istrue(p)       ((p) != NIL && (p) != F)
#define isfalse(p)      ((p) == NIL || (p) == F)

/* Error macro */
#ifdef	AVOID_HACK_LOOP
# define	BEGIN	{
# define	END	}
#else
/*
 * I believe this is better, but some compiler complains....
 */
# define	BEGIN	do {
# define	END	} while (0)
#endif

#define Error_0(s) BEGIN                       \
    args = cons(mk_string((s)), NIL);          \
    operator = (short)OP_ERR0;                 \
    return T; END

#define Error_1(s, a) BEGIN                    \
    args = cons((a), NIL);                     \
    args = cons(mk_string((s)), args);         \
    operator = (short)OP_ERR0;                 \
    return T; END

/* control macros for Eval_Cycle */
#define s_goto(a) BEGIN                        \
    operator = (short)(a);                     \
    return T; END

#define s_save(a, b, c)  (                     \
    dump = cons(envir, cons((c), dump)),       \
    dump = cons((b), dump),                    \
    dump = cons(mk_number((long)(a)), dump))   \


#define s_return(a) BEGIN                      \
    value = (a);                               \
    operator = ivalue(car(dump));              \
    args = cadr(dump);                         \
    envir = caddr(dump);                       \
    code = cadddr(dump);                       \
    dump = cddddr(dump);                       \
    return T; END

#define s_retbool(tf)	s_return((tf) ? T : F)

#ifdef LPIA

#define s_next BEGIN \
	reset_newexpr(); \
	operator = next_op (); \
        return T; END

#define OP_STEPON 301

top_debug ()
{
char buf[200];
    printstate ();
    gets (buf);
    if (*buf == 'g')
	step = 0;
}

break_debug () {}

int next_op ()
{
pointer x, v;
char buf[200];
/* static int n_next = 0; */

	n_next++;
 
	if (n_next == n_break)
	    code = cons (_BRK, code);

	if (step > 4)
	{
	    printf ("step = 0x%X, n_next = %d\n", step, n_next);
	}

	if (n_next == 52509)
	    break_debug();

	if (n_next == 149999)
            break_debug();

	if (args->_flag == 0 ||
	    last_of_list(envir_lpia)->_flag == 0)
	{
	    printf ("n_next = %d\n", n_next);
	    gets (buf);
	    break_debug();
	}

	if (step & 1)
	{
		/* 
		fprintf (outfp, "\nop = %d [%s]",
			operator, keyword[operator]);
		*/
		/*
		printstate ();
		gets (buf);
		*/
		top_debug ();
	}

	x = car(code);
        code = cdr(code);
	for (;;)
	{
		if (code == NIL)
		{
		    fprintf (outfp, "\nNo code\n");
		    	    
		}
		if (isproc(x))
                	break;
		if (issymbol(x))
		{
			v = getval(x);
			if (v == NIL)
			{
			    /*
			    fprintf (outfp, "\nUndefined function ");
			    print (x);
			    fprintf (outfp, "\n");
			    */
			    v = cons (_UDF, cons (x, NIL));
			}    
			if (step && ispair(v))
			{
			    fprintf (outfp, "(%X) Step into ", step);
			    print (x);
			    fprintf (outfp, " ? ");
			    gets (buf);
			    if (!(buf[0] == 'y' || buf[0] == 'Y'))
			    {
				step = 0; 
				code = cons (mk_operator(OP_STEPON), code);
				/*
				fprintf (outfp, "code = ");
				print (code);
				fprintf (outfp, "\n");
				*/
			    }
			}
			x = v;
		}
		else if (ispair(x))
		{
			code = append (cdr(x), code);
			x = car(x); 
		}
		else if (x == NIL)
		{
			x = car(code);
			code = cdr(code);	
		}
		else
		{
			args = cons (x, args);
			x = car(code);
                        code = cdr(code);
                }
	}
	return ivalue(x);
}

#endif


/* ========== Evaluation Cycle ========== */

/* operator code */
#define	OP_LOAD			0
#define	OP_T0LVL		1
#define	OP_T1LVL		2
#define	OP_READ			3
#define	OP_VALUEPRINT		4
#define	OP_EVAL			5
#define	OP_E0ARGS		6
#define	OP_E1ARGS		7
#define	OP_APPLY		8
#define	OP_DOMACRO		9

#define	OP_LAMBDA		10
#define	OP_QUOTE		11
#define	OP_DEF0			12
#define	OP_DEF1			13
#define	OP_BEGIN		14
#define	OP_IF0			15
#define	OP_IF1			16
#define	OP_SET0			17
#define	OP_SET1			18
#define	OP_LET0			19
#define	OP_LET1			20
#define	OP_LET2			21
#define	OP_LET0AST		22
#define	OP_LET1AST		23
#define	OP_LET2AST		24
#define	OP_LET0REC		25
#define	OP_LET1REC		26
#define	OP_LET2REC		27
#define	OP_COND0		28
#define	OP_COND1		29
#define	OP_DELAY		30
#define	OP_AND0			31
#define	OP_AND1			32
#define	OP_OR0			33
#define	OP_OR1			34
#define	OP_C0STREAM		35
#define	OP_C1STREAM		36
#define	OP_0MACRO		37
#define	OP_1MACRO		38
#define	OP_CASE0		39
#define	OP_CASE1		40
#define	OP_CASE2		41

#define	OP_PEVAL		42
#define	OP_PAPPLY		43
#define	OP_CONTINUATION		44
#define	OP_ADD			45
#define	OP_SUB			46
#define	OP_MUL			47
#define	OP_DIV			48
#define	OP_REM			49
#define	OP_CAR			50
#define	OP_CDR			51
#define	OP_CONS			52
#define	OP_SETCAR		53
#define	OP_SETCDR		54
#define	OP_NOT			55
#define	OP_BOOL			56
#define	OP_NULL			57
#define	OP_ZEROP		58
#define	OP_POSP			59
#define	OP_NEGP			60
#define	OP_NEQ			61
#define	OP_LESS			62
#define	OP_GRE			63
#define	OP_LEQ			64
#define	OP_GEQ			65
#define	OP_SYMBOL		66
#define	OP_NUMBER		67
#define	OP_STRING		68
#define	OP_PROC			69
#define	OP_PAIR			70
#define	OP_EQ			71
#define	OP_EQV			72
#define	OP_FORCE		73
#define	OP_WRITE		74
#define	OP_DISPLAY		75
#define	OP_NEWLINE		76
#define	OP_ERR0			77
#define	OP_ERR1			78
#define	OP_REVERSE		79
#define	OP_APPEND		80
#define	OP_PUT			81
#define	OP_GET			82
#define	OP_QUIT			83
#define	OP_GC			84
#define	OP_GCVERB		85
#define	OP_NEWSEGMENT		86

#define	OP_RDSEXPR		87
#define	OP_RDLIST		88
#define	OP_RDDOT		89
#define	OP_RDQUOTE		90
#define	OP_RDQQUOTE		91
#define	OP_RDUNQUOTE		92
#define	OP_RDUQTSP		93

#define	OP_P0LIST		94
#define	OP_P1LIST		95

#define	OP_LIST_LENGTH		96
#define	OP_ASSQ			97
#define	OP_PRINT_WIDTH		98
#define	OP_P0_WIDTH		99
#define	OP_P1_WIDTH		100
#define	OP_GET_CLOSURE		101
#define	OP_CLOSUREP		102
#define	OP_MACROP		103

#define OP_FLAMBDA	104
#define OP_MLAMBDA	105
#define OP_DOMLAMBDA	106

#ifdef LPIA
#define OP_LPIA1 	107
#define OP_LPIA		116
#define OP_SCM		108
#define OP_RLPIA	109
#define OP_LPIA_QUOTE	110
#define OP_LPIA_CAR	111
#define OP_LPIA_PRINTATOM 112
#define OP_PROTOPRINT 	113
#define OP_PRINTSTATE	114
#define OP_STEP		115
#define OP_ELPIA	117
#define OP_I		118
#define OP_K		119
#define OP_S		120
#define OP_C		121
#define OP_W		122
#define OP_Y		123
#define OP_DEP		124
#define OP_REP		125
#define OP_ECH		126
#define OP_THEN		127
#define OP_EXEC		128
#define OP_GETCODE	129
#define OP_SETCODE	130
#define OP_GETARGS	131
#define OP_SETARGS	132
#define OP_GETENVIR	133
#define OP_SETENVIR	134
#define OP_GETDUMP	135
#define OP_SETDUMP	136
#define OP_GETVALUE	137
#define OP_SETVALUE	138
#define OP_LPIA_CDR	139
#define OP_LPIA_CONS	140
#define OP_LPIA_EQ	141
#define OP_NCONSPTHEN 142
#define OP_LPIA_RPLACA	143
#define OP_LPIA_RPLACD	144
#define OP_LPIA_PRINT	145
#define OP_LPIA_TYI	146
#define OP_LPIA_TYO	147

#define OP_LPIA_EDEF	151
#define OP_LPIA_DEF	152

#define OP_MON		161
#define OP_DES		162
#define OP_MONDEP	163
#define OP_GETH0	164
#define OP_GETH1	165
#define OP_GETH2	166
#define OP_GETH3	167

#define OP_LPIA_NOT		169

#define OP_B		171
#define OP_P		172
#define OP_J		173
#define OP_APPLYTO	174

#define OP_GETLCTXS	181
#define OP_SETLCTXS	182
#define OP_GETCTX	183
#define OP_GETCTX_ENLINSTR 184
#define OP_SETCTX	185
#define OP_INSCTX	186
#define OP_GETSTRAT	187
#define OP_SETSTRAT	188 
#define OP_END		189
#define OP_ALT1		190
#define OP_INSTR_SEND	191
#define OP_INSTR_RECEIVE   192
#define OP_SETCTXS	193
#define OP_STOP		194
#define OP_REVOL1	195
#define OP_GETLLCTXS	196
#define OP_SETLLCTXS	197
#define OP_GETENV	198
#define OP_SETENV	199

#define OP_LENGTH	201
#define OP_MEMQ		202
#define OP_MEMBER	203
#define OP_EQUAL	204
#define OP_SEPARER	205
#define OP_REMQ		206
#define OP_REMOVE	207
#define OP_INCLQ	208
#define OP_GETVENV	209
#define OP_SETVENV	210
#define OP_ADDVENV	211
#define OP_REMVENV	212
#define OP_GETVSENV	213
#define OP_BINDVENV	214
#define OP_UNBINDVENV	215
#define OP_GENSYM	216
#define OP_BOUNDVENV	217
#define OP_GETRECVENV	218
#define OP_ENSEMBLE	219
#define OP_VARIABLES	220
#define OP_SUBST	221
#define OP_RENOMMER	222
#define OP_CONSTRUIRE	223
#define OP_UNIFIER	224

#define OP_ARG		226
#define OP_GETVQ	227
#define OP_GETVSQ	228
#define OP_SETVQ	229
#define OP_BINDVQ	230
#define OP_UNBINDVQ	231

#define OP_INCL		233

#define OP_DECLSYM	235
#define OP_GETDEF	236
#define OP_SETDEF	237

#define OP_LPIA_READ	239
#define OP_LPIA_READFILE 240

#define OP_PRINTSTRING  241
#define OP_LPIA_PRIN	242
 
#define OP_LPIA_PLUS	244
#define OP_LPIA_MOINS	245
#define OP_LPIA_FOIS	246
#define OP_LPIA_DIV	247
#define OP_LPIA_MOD	248
#define OP_LPIA_ETL	249
#define OP_LPIA_OUL	250
#define OP_LPIA_OXL	251
#define OP_LPIA_ZEROP	252
#define OP_LPIA_POSP	253

#define OP_LPIA_GETPROPS 261
#define OP_LPIA_SETPROPS 262
#define OP_LPIA_GETPROP 263
#define OP_LPIA_PUTPROP 264
#define OP_LPIA_ADDPROP 265
#define OP_LPIA_REMPROP 266

#define OP_LPIA_GETTYPE 268

#define OP_LPIA_MAKESTRING 271
#define OP_LPIA_CREF	272
#define OP_LPIA_CSET	273
#define OP_LPIA_CATENATE 274

#define OP_GETREG 276
#define OP_SETREG 277

#define OP_GETPROG 278
#define OP_SETPROG 279
#define OP_GETSTACK 280
#define OP_SETSTACK 281

#define OP_PEEK 283
#define OP_POKE 284
#define OP_USR 285
#define OP_EXECMACH 286

#define OP_FOPEN 291
#define OP_FCLOSE 292
#define OP_FGETC 293
#define OP_FPUTC 294
#define OP_FEOF 295
#define OP_GETERRNO 296
#define OP_FSEEK 297

#define OP_GETH 298

/* #define OP_STEPON 301 */

#define OP_READSTRING 302

#define OP_LPIA_APPEND 303
#define OP_LPIA_REVERSE 304

#define OP_ALT 305

#define OP_LPIA_QUIT 299

#endif

static FILE *tmpfp;
static int tok;
static int print_flag;
/* static pointer value; */
static short operator;

pointer bind (pointer vars, pointer ars, pointer env)
{
	if (isfalse(vars))
		return env;
	if (!ispair(vars))
		return cons (cons(vars,ars), env);
	if (ispair(ars))
		return bind (car(vars), car(ars),
			bind (cdr(vars), cdr(ars), env));
	return bind (car(vars), ars,
		bind (cdr(vars), ars, env));
}

void set1 (pointer var, pointer val)
{
pointer x, y;
		for (x = envir; x != NIL; x = cdr(x)) {
			for (y = car(x); y != NIL; y = cdr(y))
				if (caar(y) == var)
					break;
			if (y != NIL)
				break;
		}
		if (x != NIL) {
			cdar(y) = val;
		} else {
		    car(envir) = bind (var, val, car(envir));
		}
}

pointer opexe_0(op)
register short op;
{
	register pointer x, y;

	switch (op) {
	case OP_LOAD:		/* load */
		if (!isstring(car(args))) {
			Error_0("load -- argument is not string");
		}
		if ((infp = fopen(strvalue(car(args)), "r")) == NULL) {
			infp = stdin;
			Error_1("Unable to open", car(args));
		}
		fprintf(outfp, "loading %s", strvalue(car(args)));
		s_goto(OP_T0LVL);

	case OP_T0LVL:	/* top level */
		fprintf(outfp, "\n");
		dump = NIL;
		envir = global_env;
		s_save(OP_VALUEPRINT, NIL, NIL);
		s_save(OP_T1LVL, NIL, NIL);
		if (infp == stdin)
			printf(prompt);
		s_goto(OP_READ);

	case OP_T1LVL:	/* top level */
		code = value;
		s_goto(OP_EVAL);

	case OP_READ:		/* read */
		tok = token();
		s_goto(OP_RDSEXPR);

	case OP_VALUEPRINT:	/* print evalution result */
		print_flag = 1;
		args = value;
		s_save(OP_T0LVL, NIL, NIL);
		s_goto(OP_P0LIST);

	case OP_EVAL:		/* main part of evalution */
		if (issymbol(code)) {	/* symbol */
			for (x = envir; x != NIL; x = cdr(x)) {
				for (y = car(x); y != NIL; y = cdr(y))
					if (caar(y) == code)
						break;
				if (y != NIL)
					break;
			}
			if (x != NIL) {
				s_return(cdar(y));
			} else {
				Error_1("Unbounded variable", code);
			}
		} else if (ispair(code)) {
			if (issyntax(x = car(code))) {	/* SYNTAX */
				code = cdr(code);
				s_goto(syntaxnum(x));
			} else {/* first, eval top element and eval arguments */
#ifdef USE_MACRO
				s_save(OP_E0ARGS, NIL, code);
#else
				s_save(OP_E1ARGS, NIL, cdr(code));
#endif
				code = car(code);
				s_goto(OP_EVAL);
			}
		} else {
			s_return(code);
		}

#ifdef USE_MACRO
	case OP_E0ARGS:	/* eval arguments */
		if (ismacro(value)) {	/* macro expansion */
			s_save(OP_DOMACRO, NIL, NIL);
			args = cons(code, NIL);
			code = value;
			s_goto(OP_APPLY);
		} else if (isfclosure(value)) { /* do not evalue */
			args = cdr(code);
			code = value;
			s_goto(OP_APPLY);
		} else if (ismclosure(value)) {
		/*	s_save(OP_DOMLAMBDA, NIL, NIL);
			args = cons (code, NIL);
			code = value;
			s_goto (OP_APPLY);
		*/
			args = code;
			code = value;
			/* s_goto (OP_DOMLAMBDA); */

			s_goto (OP_APPLY);
		} else {
			code = cdr(code);
			s_goto(OP_E1ARGS);
		}
#endif

	case OP_E1ARGS:	/* eval arguments */
		args = cons(value, args);
		if (ispair(code)) {	/* continue */
			s_save(OP_E1ARGS, args, cdr(code));
			code = car(code);
			args = NIL;
			s_goto(OP_EVAL);
		} else {	/* end */
			args = reverse(args);
			code = car(args);
			args = cdr(args);
			s_goto(OP_APPLY);
		}

	case OP_APPLY:		/* apply 'code' to 'args' */
		if (isproc(code)) {
			s_goto(procnum(code));	/* PROCEDURE */
		} else if (isclosure(code)) {	/* CLOSURE */
			/* make environment */
			envir = cons(NIL, closure_env(code));
#ifdef OLD
			for (x = car(closure_code(code)), y = args;
			     ispair(x); x = cdr(x), y = cdr(y)) {
				if (y == NIL) {
					Error_0("Few arguments");
				} else {
					car(envir) = cons(cons(car(x), car(y)), car(envir));
				}
			}
			if (x == NIL) {
				/*--
				 * if (y != NIL) {
				 * 	Error_0("Many arguments");
				 * }
				 */
			} else if (issymbol(x))
				car(envir) = cons(cons(x, y), car(envir));
			else {
				Error_0("Syntax error in closure");
			}
#else
			car(envir) = bind (car(closure_code(code)),
				args, car(envir));
#endif
			code = cdr(closure_code(code));
			args = NIL;
			s_goto(OP_BEGIN);
		} else if (isfclosure(code)) {	/* CLOSURE */
			/* make environment */
			envir = cons(NIL, closure_env(code));
			car(envir) = bind (car(closure_code(code)),
				args, car(envir));
			code = cdr(closure_code(code));
			args = NIL;
			s_goto(OP_BEGIN);
		} else if (ismclosure(code)) {	/* CLOSURE */
			/* make environment */
			envir = cons(NIL, closure_env(code));
			car(envir) = bind (car(closure_code(code)),
				args, car(envir));
			code = cdr(closure_code(code));
			args = NIL;
			s_goto(OP_BEGIN);

		} else if (iscontinuation(code)) {	/* CONTINUATION */
			dump = cont_dump(code);
			s_return(args != NIL ? car(args) : NIL);
		} else {
			/* Error_0("Illegal function"); */
			Error_1 ("Illegal function ", code);
		}

#ifdef USE_MACRO
	case OP_DOMACRO:	/* do macro */
		code = value;
		s_goto(OP_EVAL);
#endif

	case OP_LAMBDA:	/* lambda */
		s_return(mk_closure(code, envir));

	case OP_QUOTE:		/* quote */
		s_return(car(code));

	case OP_DEF0:	/* define */
		if (ispair(car(code))) {
			x = caar(code);
			code = cons(LAMBDA, cons(cdar(code), cdr(code)));
		} else {
			x = car(code);
			code = cadr(code);
		}
		if (!issymbol(x)) {
			Error_0("Variable is not symbol");
		}
		s_save(OP_DEF1, NIL, x);
		s_goto(OP_EVAL);

	case OP_DEF1:	/* define */
		for (x = car(envir); x != NIL; x = cdr(x))
			if (caar(x) == code)
				break;
		if (x != NIL)
			cdar(x) = value;
		else
			car(envir) = cons(cons(code, value), car(envir));
		s_return(code);

	case OP_SET0:		/* set! */
		s_save(OP_SET1, NIL, car(code));
		code = cadr(code);
		s_goto(OP_EVAL);

	case OP_SET1:		/* set! */
		for (x = envir; x != NIL; x = cdr(x)) {
			for (y = car(x); y != NIL; y = cdr(y))
				if (caar(y) == code)
					break;
			if (y != NIL)
				break;
		}
		if (x != NIL) {
			cdar(y) = value;
			s_return(value);
		} else {
			Error_1("Unbounded variable", code);
		}

	case OP_BEGIN:		/* begin */
		if (!ispair(code)) {
			s_return(code);
		}
		if (cdr(code) != NIL) {
			s_save(OP_BEGIN, NIL, cdr(code));
		}
		code = car(code);
		s_goto(OP_EVAL);

	case OP_IF0:		/* if */
		s_save(OP_IF1, NIL, cdr(code));
		code = car(code);
		s_goto(OP_EVAL);

	case OP_IF1:		/* if */
		if (istrue(value))
			code = car(code);
		else
			code = cadr(code);	/* (if #f 1) ==> () because
						 * car(NIL) = NIL */
		s_goto(OP_EVAL);

	case OP_LET0:		/* let */
		args = NIL;
		value = code;
		code = issymbol(car(code)) ? cadr(code) : car(code);
		s_goto(OP_LET1);

	case OP_LET1:		/* let (caluculate parameters) */
		args = cons(value, args);
		if (ispair(code)) {	/* continue */
			s_save(OP_LET1, args, cdr(code));
			code = cadar(code);
			args = NIL;
			s_goto(OP_EVAL);
		} else {	/* end */
			args = reverse(args);
			code = car(args);
			args = cdr(args);
			s_goto(OP_LET2);
		}

	case OP_LET2:		/* let */
		envir = cons(NIL, envir);
		for (x = issymbol(car(code)) ? cadr(code) : car(code), y = args;
		     y != NIL; x = cdr(x), y = cdr(y))
			car(envir) = cons(cons(caar(x), car(y)), car(envir));
		if (issymbol(car(code))) {	/* named let */
			for (x = cadr(code), args = NIL; x != NIL; x = cdr(x))
				args = cons(caar(x), args);
			x = mk_closure(cons(reverse(args), cddr(code)), envir);
			car(envir) = cons(cons(car(code), x), car(envir));
			code = cddr(code);
			args = NIL;
		} else {
			code = cdr(code);
			args = NIL;
		}
		s_goto(OP_BEGIN);

	case OP_LET0AST:	/* let* */
		if (car(code) == NIL) {
			envir = cons(NIL, envir);
			code = cdr(code);
			s_goto(OP_BEGIN);
		}
		s_save(OP_LET1AST, cdr(code), car(code));
		code = cadaar(code);
		s_goto(OP_EVAL);

	case OP_LET1AST:	/* let* (make new frame) */
		envir = cons(NIL, envir);
		s_goto(OP_LET2AST);

	case OP_LET2AST:	/* let* (caluculate parameters) */
		car(envir) = cons(cons(caar(code), value), car(envir));
		code = cdr(code);
		if (ispair(code)) {	/* continue */
			s_save(OP_LET2AST, args, code);
			code = cadar(code);
			args = NIL;
			s_goto(OP_EVAL);
		} else {	/* end */
			code = args;
			args = NIL;
			s_goto(OP_BEGIN);
		}
	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;
}


pointer opexe_1(op)
register short op;
{
	register pointer x, y;

	switch (op) {
	case OP_LET0REC:	/* letrec */
		envir = cons(NIL, envir);
		args = NIL;
		value = code;
		code = car(code);
		s_goto(OP_LET1REC);

	case OP_LET1REC:	/* letrec (caluculate parameters) */
		args = cons(value, args);
		if (ispair(code)) {	/* continue */
			s_save(OP_LET1REC, args, cdr(code));
			code = cadar(code);
			args = NIL;
			s_goto(OP_EVAL);
		} else {	/* end */
			args = reverse(args);
			code = car(args);
			args = cdr(args);
			s_goto(OP_LET2REC);
		}

	case OP_LET2REC:	/* letrec */
		for (x = car(code), y = args; y != NIL; x = cdr(x), y = cdr(y))
			car(envir) = cons(cons(caar(x), car(y)), car(envir));
		code = cdr(code);
		args = NIL;
		s_goto(OP_BEGIN);

	case OP_COND0:		/* cond */
		if (!ispair(code)) {
			Error_0("Syntax error in cond");
		}
		s_save(OP_COND1, NIL, code);
		code = caar(code);
		s_goto(OP_EVAL);

	case OP_COND1:		/* cond */
		if (istrue(value)) {
			if ((code = cdar(code)) == NIL) {
				s_return(value);
			}
			s_goto(OP_BEGIN);
		} else {
			if ((code = cdr(code)) == NIL) {
				s_return(NIL);
			} else {
				s_save(OP_COND1, NIL, code);
				code = caar(code);
				s_goto(OP_EVAL);
			}
		}

	case OP_DELAY:		/* delay */
		x = mk_closure(cons(NIL, code), envir);
		setpromise(x);
		s_return(x);

	case OP_AND0:		/* and */
		if (code == NIL) {
			s_return(T);
		}
		s_save(OP_AND1, NIL, cdr(code));
		code = car(code);
		s_goto(OP_EVAL);

	case OP_AND1:		/* and */
		if (isfalse(value)) {
			s_return(value);
		} else if (code == NIL) {
			s_return(value);
		} else {
			s_save(OP_AND1, NIL, cdr(code));
			code = car(code);
			s_goto(OP_EVAL);
		}

	case OP_OR0:		/* or */
		if (code == NIL) {
			s_return(F);
		}
		s_save(OP_OR1, NIL, cdr(code));
		code = car(code);
		s_goto(OP_EVAL);

	case OP_OR1:		/* or */
		if (istrue(value)) {
			s_return(value);
		} else if (code == NIL) {
			s_return(value);
		} else {
			s_save(OP_OR1, NIL, cdr(code));
			code = car(code);
			s_goto(OP_EVAL);
		}

	case OP_C0STREAM:	/* cons-stream */
		s_save(OP_C1STREAM, NIL, cdr(code));
		code = car(code);
		s_goto(OP_EVAL);

	case OP_C1STREAM:	/* cons-stream */
		args = value;	/* save value to register args for gc */
		x = mk_closure(cons(NIL, code), envir);
		setpromise(x);
		s_return(cons(args, x));

#ifdef USE_MACRO
	case OP_0MACRO:	/* macro */
		x = car(code);
		code = cadr(code);
#ifdef OLD
		if (!issymbol(x)) {
			Error_0("Variable is not symbol");
		}
#endif
		s_save(OP_1MACRO, NIL, x);
		s_goto(OP_EVAL);

	case OP_1MACRO:	/* macro */
		type(value) |= T_MACRO;

		for (x = car(envir); x != NIL; x = cdr(x))
			if (caar(x) == code)
				break;
		if (x != NIL)
			cdar(x) = value;
		else
			car(envir) = cons(cons(code, value), car(envir));
		s_return(code);
#endif

	case OP_CASE0:		/* case */
		s_save(OP_CASE1, NIL, cdr(code));
		code = car(code);
		s_goto(OP_EVAL);

	case OP_CASE1:		/* case */
		for (x = code; x != NIL; x = cdr(x)) {
			if (!ispair(y = caar(x)))
				break;
			for ( ; y != NIL; y = cdr(y))
				if (eqv(car(y), value))
					break;
			if (y != NIL)
				break;
		}
		if (x != NIL) {
			if (ispair(caar(x))) {
				code = cdar(x);
				s_goto(OP_BEGIN);
			} else {/* else */
				s_save(OP_CASE2, NIL, cdar(x));
				code = caar(x);
				s_goto(OP_EVAL);
			}
		} else {
			s_return(NIL);
		}

	case OP_CASE2:		/* case */
		if (istrue(value)) {
			s_goto(OP_BEGIN);
		} else {
			s_return(NIL);
		}
	case OP_PAPPLY:	/* apply */
		code = car(args);
		args = cadr(args);
		s_goto(OP_APPLY);

	case OP_PEVAL:	/* eval */
		code = car(args);
		args = NIL;
		s_goto(OP_EVAL);

	case OP_CONTINUATION:	/* call-with-current-continuation */
		code = car(args);
		args = cons(mk_continuation(dump), NIL);
		s_goto(OP_APPLY);

	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;
}


pointer opexe_2(op)
register short op;
{
	register pointer x, y;
	register long v;

	switch (op) {
	case OP_ADD:		/* + */
		for (x = args, v = 0; x != NIL; x = cdr(x))
			v += ivalue(car(x));
		s_return(mk_number(v));

	case OP_SUB:		/* - */
		for (x = cdr(args), v = ivalue(car(args)); x != NIL; x = cdr(x))
			v -= ivalue(car(x));
		s_return(mk_number(v));

	case OP_MUL:		/* * */
		for (x = args, v = 1; x != NIL; x = cdr(x))
			v *= ivalue(car(x));
		s_return(mk_number(v));

	case OP_DIV:		/* / */
		for (x = cdr(args), v = ivalue(car(args)); x != NIL; x = cdr(x)) {
			if (ivalue(car(x)) != 0)
				v /= ivalue(car(x));
			else {
				Error_0("Divided by zero");
			}
		}
		s_return(mk_number(v));

	case OP_REM:		/* remainder */
		for (x = cdr(args), v = ivalue(car(args)); x != NIL; x = cdr(x)) {
			if (ivalue(car(x)) != 0)
				v %= ivalue(car(x));
			else {
				Error_0("Divided by zero");
			}
		}
		s_return(mk_number(v));

	case OP_CAR:		/* car */
		if (ispair(car(args))) {
			s_return(caar(args));
		} else {
			Error_0("Unable to car for non-cons cell");
		}

	case OP_CDR:		/* cdr */
		if (ispair(car(args))) {
			s_return(cdar(args));
		} else {
			Error_0("Unable to cdr for non-cons cell");
		}

	case OP_CONS:		/* cons */
		cdr(args) = cadr(args);
		s_return(args);

	case OP_SETCAR:	/* set-car! */
		if (ispair(car(args))) {
			caar(args) = cadr(args);
			s_return(car(args));
		} else {
			Error_0("Unable to set-car! for non-cons cell");
		}

	case OP_SETCDR:	/* set-cdr! */
		if (ispair(car(args))) {
			cdar(args) = cadr(args);
			s_return(car(args));
		} else {
			Error_0("Unable to set-cdr! for non-cons cell");
		}

	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;
}


pointer opexe_3(op)
register short op;
{
	register pointer x, y;

	switch (op) {
	case OP_NOT:		/* not */
		s_retbool(isfalse(car(args)));
	case OP_BOOL:		/* boolean? */
		s_retbool(car(args) == F || car(args) == T);
	case OP_NULL:		/* null? */
		s_retbool(car(args) == NIL);
	case OP_ZEROP:		/* zero? */
		s_retbool(ivalue(car(args)) == 0);
	case OP_POSP:		/* positive? */
		s_retbool(ivalue(car(args)) > 0);
	case OP_NEGP:		/* negative? */
		s_retbool(ivalue(car(args)) < 0);
	case OP_NEQ:		/* = */
		s_retbool(ivalue(car(args)) == ivalue(cadr(args)));
	case OP_LESS:		/* < */
		s_retbool(ivalue(car(args)) < ivalue(cadr(args)));
	case OP_GRE:		/* > */
		s_retbool(ivalue(car(args)) > ivalue(cadr(args)));
	case OP_LEQ:		/* <= */
		s_retbool(ivalue(car(args)) <= ivalue(cadr(args)));
	case OP_GEQ:		/* >= */
		s_retbool(ivalue(car(args)) >= ivalue(cadr(args)));
	case OP_SYMBOL:	/* symbol? */
		s_retbool(issymbol(car(args)));
	case OP_NUMBER:	/* number? */
		s_retbool(isnumber(car(args)));
	case OP_STRING:	/* string? */
		s_retbool(isstring(car(args)));
	case OP_PROC:		/* procedure? */
		/*--
	         * continuation should be procedure by the example
	         * (call-with-current-continuation procedure?) ==> #t
                 * in R^3 report sec. 6.9
	         */
		s_retbool(isproc(car(args)) || isclosure(car(args)) ||
			isfclosure(car(args)) || ismclosure(car(args))
			  || iscontinuation(car(args)));
	case OP_PAIR:		/* pair? */
		s_retbool(ispair(car(args)));
	case OP_EQ:		/* eq? */
		s_retbool(car(args) == cadr(args));
	case OP_EQV:		/* eqv? */
		s_retbool(eqv(car(args), cadr(args)));
	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;
}


pointer opexe_4(op)
register short op;
{
	register pointer x, y;

	switch (op) {
	case OP_FORCE:		/* force */
		code = car(args);
		if (ispromise(code)) {
			args = NIL;
			s_goto(OP_APPLY);
		} else {
			s_return(code);
		}

	case OP_WRITE:		/* write */
		print_flag = 1;
		args = car(args);
		s_goto(OP_P0LIST);

	case OP_DISPLAY:	/* display */
		print_flag = 0;
		args = car(args);
		s_goto(OP_P0LIST);

	case OP_NEWLINE:	/* newline */
		fprintf(outfp, "\n");
		s_return(T);

	case OP_ERR0:	/* error */
		if (!isstring(car(args))) {
			Error_0("error -- first argument must be string");
		}
		tmpfp = outfp;
		outfp = stderr;
		fprintf(outfp, "Error: ");
		fprintf(outfp, "%s", strvalue(car(args)));
		args = cdr(args);
		s_goto(OP_ERR1);

	case OP_ERR1:	/* error */
		fprintf(outfp, " ");
		if (args != NIL) {
			s_save(OP_ERR1, cdr(args), NIL);
			args = car(args);
			print_flag = 1;
			s_goto(OP_P0LIST);
		} else {
			fprintf(outfp, "\n");
			flushinput();
			outfp = tmpfp;
			s_goto(OP_T0LVL);
		}

	case OP_REVERSE:	/* reverse */
		s_return(reverse(car(args)));

	case OP_APPEND:	/* append */
		s_return(append(car(args), cadr(args)));

	case OP_PUT:		/* put */
		if (!hasprop(car(args)) || !hasprop(cadr(args))) {
			Error_0("Illegal use of put");
		}
		for (x = symprop(car(args)), y = cadr(args); x != NIL; x = cdr(x))
			if (caar(x) == y)
				break;
		if (x != NIL)
			cdar(x) = caddr(args);
		else
			symprop(car(args)) = cons(cons(y, caddr(args)),
						  symprop(car(args)));
		s_return(T);

	case OP_GET:		/* get */
		if (!hasprop(car(args)) || !hasprop(cadr(args))) {
			Error_0("Illegal use of get");
		}
		for (x = symprop(car(args)), y = cadr(args); x != NIL; x = cdr(x))
			if (caar(x) == y)
				break;
		if (x != NIL) {
			s_return(cdar(x));
		} else {
			s_return(NIL);
		}

	case OP_QUIT:		/* quit */
		return (NIL);

	case OP_GC:		/* gc */
		gc(NIL, NIL);
		s_return(T);

	case OP_GCVERB:		/* gc-verbose */
	{	int	was = gc_verbose;

		gc_verbose = (car(args) != F);
		s_retbool(was);
	}

	case OP_NEWSEGMENT:	/* new-segment */
		if (!isnumber(car(args))) {
			Error_0("new-segment -- argument must be number");
		}
		fprintf(outfp, "allocate %d new segments\n",
			alloc_cellseg((int) ivalue(car(args))));
		s_return(T);
	}
}


pointer opexe_5(op)
register short op;
{
char message[200];
	register pointer x, y;

	switch (op) {
	/* ========== reading part ========== */
	case OP_RDSEXPR:
		switch (tok) {
		case TOK_COMMENT:
			while (inchar() != '\n')
				;
			tok = token();
			s_goto(OP_RDSEXPR);
		case TOK_LPAREN:
			tok = token();
			if (tok == TOK_RPAREN) {
				s_return(NIL);
			} else if (tok == TOK_DOT) {
				Error_0("syntax error -- illegal dot expression");
			} else {
				s_save(OP_RDLIST, NIL, NIL);
				s_goto(OP_RDSEXPR);
			}			
		case TOK_QUOTE:
			s_save(OP_RDQUOTE, NIL, NIL);
			tok = token();
			s_goto(OP_RDSEXPR);
#ifdef USE_QQUOTE
		case TOK_BQUOTE:
			s_save(OP_RDQQUOTE, NIL, NIL);
			tok = token();
			s_goto(OP_RDSEXPR);
		case TOK_COMMA:
			s_save(OP_RDUNQUOTE, NIL, NIL);
			tok = token();
			s_goto(OP_RDSEXPR);
		case TOK_ATMARK:
			s_save(OP_RDUQTSP, NIL, NIL);
			tok = token();
			s_goto(OP_RDSEXPR);
#endif
		case TOK_ATOM:
			s_return(mk_atom(readstr("();\t\n ")));
		case TOK_DQUOTE:
			s_return(mk_string(readstrexp()));
		case TOK_SHARP:
			if ((x = mk_const(readstr("();\t\n "))) == NIL) {
				Error_0("Undefined sharp expression");
			} else {
				s_return(x);
			}
		default:
#ifdef VO
			Error_0("syntax error -- illegal token");
#else
			sprintf (message, "syntax error -- illegal token %d",
				tok);
			Error_0 (message);
#endif
		}
		break;

	case OP_RDLIST:
		args = cons(value, args);
		tok = token();
		if (tok == TOK_COMMENT) {
			while (inchar() != '\n')
				;
			tok = token();
		}
		if (tok == TOK_RPAREN) {
			s_return(non_alloc_rev(NIL, args));
		} else if (tok == TOK_DOT) {
			s_save(OP_RDDOT, args, NIL);
			tok = token();
			s_goto(OP_RDSEXPR);
		} else {
			s_save(OP_RDLIST, args, NIL);;
			s_goto(OP_RDSEXPR);
		}

	case OP_RDDOT:
		if (token() != TOK_RPAREN) {
			Error_0("syntax error -- illegal dot expression");
		} else {
			s_return(non_alloc_rev(value, args));
		}

	case OP_RDQUOTE:
		s_return(cons(QUOTE, cons(value, NIL)));

#ifdef USE_QQUOTE
	case OP_RDQQUOTE:
		s_return(cons(QQUOTE, cons(value, NIL)));

	case OP_RDUNQUOTE:
		s_return(cons(UNQUOTE, cons(value, NIL)));

	case OP_RDUQTSP:
		s_return(cons(UNQUOTESP, cons(value, NIL)));
#endif

	/* ========== printing part ========== */
	case OP_P0LIST:
		if (!ispair(args)) {
			printatom(args, print_flag);
			s_return(T);
		} else if (car(args) == QUOTE && ok_abbrev(cdr(args))) {
			fprintf(outfp, "'");
			args = cadr(args);
			s_goto(OP_P0LIST);
		} else if (car(args) == QQUOTE && ok_abbrev(cdr(args))) {
			fprintf(outfp, "`");
			args = cadr(args);
			s_goto(OP_P0LIST);
		} else if (car(args) == UNQUOTE && ok_abbrev(cdr(args))) {
			fprintf(outfp, ",");
			args = cadr(args);
			s_goto(OP_P0LIST);
		} else if (car(args) == UNQUOTESP && ok_abbrev(cdr(args))) {
			fprintf(outfp, ",@");
			args = cadr(args);
			s_goto(OP_P0LIST);
		} else {
			fprintf(outfp, "(");
			s_save(OP_P1LIST, cdr(args), NIL);
			args = car(args);
			s_goto(OP_P0LIST);
		}

	case OP_P1LIST:
		if (ispair(args)) {
			s_save(OP_P1LIST, cdr(args), NIL);
			fprintf(outfp, " ");
			args = car(args);
			s_goto(OP_P0LIST);
		} else {
			if (args != NIL) {
				fprintf(outfp, " . ");
				printatom(args, print_flag);
			}
			fprintf(outfp, ")");
			s_return(T);
		}

	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);

	}
	return T;
}

pointer opexe_6(op)
register short op;
{
	register pointer x, y;
	register long v;
	static long	w;
	char	buffer[32];

	switch (op) {
	case OP_LIST_LENGTH:	/* list-length */	/* a.k */
		for (x = car(args), v = 0; ispair(x); x = cdr(x))
			++v;
		s_return(mk_number(v));

	case OP_ASSQ:		/* assq */	/* a.k */
		x = car(args);
		for (y = cadr(args); ispair(y); y = cdr(y)) {
			if (!ispair(car(y))) {
				Error_0("Unable to handle non pair element");
			}
			if (x == caar(y))
				break;
		}
		if (ispair(y)) {
			s_return(car(y));
		} else {
			s_return(F);
		}

	case OP_PRINT_WIDTH:	/* print-width */	/* a.k */
		w = 0;
		args = car(args);
		print_flag = -1;
		s_goto(OP_P0_WIDTH);

	case OP_P0_WIDTH:
		if (!ispair(args)) {
			w += printatom(args, print_flag);
			s_return(mk_number(w));
		} else if (car(args) == QUOTE
			   && ok_abbrev(cdr(args))) {
			++w;
			args = cadr(args);
			s_goto(OP_P0_WIDTH);
		} else if (car(args) == QQUOTE
			   && ok_abbrev(cdr(args))) {
			++w;
			args = cadr(args);
			s_goto(OP_P0_WIDTH);
		} else if (car(args) == UNQUOTE
			   && ok_abbrev(cdr(args))) {
			++w;
			args = cadr(args);
			s_goto(OP_P0_WIDTH);
		} else if (car(args) == UNQUOTESP
			   && ok_abbrev(cdr(args))) {
			w += 2;
			args = cadr(args);
			s_goto(OP_P0_WIDTH);
		} else {
			++w;
			s_save(OP_P1_WIDTH, cdr(args), NIL);
			args = car(args);
			s_goto(OP_P0_WIDTH);
		}

	case OP_P1_WIDTH:
		if (ispair(args)) {
			s_save(OP_P1_WIDTH, cdr(args), NIL);
			++w;
			args = car(args);
			s_goto(OP_P0_WIDTH);
		} else {
			if (args != NIL)
				w += 3 + printatom(args, print_flag);
			++w;
			s_return(mk_number(w));
		}

	case OP_GET_CLOSURE:	/* get-closure-code */	/* a.k */
		args = car(args);
		if (args == NIL) {
			s_return(F);
		} else if (isclosure(args) || isfclosure(args) ||
				ismclosure(args)) {
			s_return(cons(LAMBDA, closure_code(value)));
#ifdef USE_MACRO
		} else if (ismacro(args)) {
			s_return(cons(LAMBDA, closure_code(value)));
#endif
		} else {
			s_return(F);
		}
	case OP_CLOSUREP:		/* closure? */
		/*
		 * Note, macro object is also a closure.
		 * Therefore, (closure? <#MACRO>) ==> #t
		 */
		if (car(args) == NIL) {
		    s_return(F);
		}
		s_retbool(isclosure(car(args)) || isfclosure(car(args)) ||
			ismclosure(car(args)));
#ifdef USE_MACRO
	case OP_MACROP:		/* macro? */
		if (car(args) == NIL) {
		    s_return(F);
		}
		s_retbool(ismacro(car(args)));
#endif
	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;	/* NOTREACHED */
}


pointer inslctxs (pointer l1, pointer l2);

pointer getctx ()
{
pointer ctx;
int i;
    ctx = NIL;
    for (i=MAX_CTX; i>=MIN_CTX; i--)
	ctx = cons (reg[i], ctx);
    return ctx;
}

setctx (pointer ctx)
{
pointer p;
int i;
    for (p=ctx, i=MIN_CTX; i<=MAX_CTX; p=cdr(p), i++)
	reg[i] = car(p);
}

pointer getctx_enlinstr ()
{
pointer ctx, x;
    x = car(code);
    code = cdr(code);
    ctx = getctx();
    code = cons(x,code);
    return ctx;
}

#define PRIO(ctx) (ivalue(car(car(ctx)))) 

pointer insctx (pointer ctx, pointer ctxs)
{
    if (!ispair(ctxs))
	return cons (ctx, ctxs);
    if (PRIO(ctx) >= PRIO(car(ctxs)))
	return cons (ctx, ctxs);
    return cons (car(ctxs), insctx (ctx, cdr(ctxs)));
}

end ()
{
pointer ctx;
    if (!ispair(lctxs))
    {
	fprintf (outfp, "No context\n");
    }
    else
    {
	ctx = car(lctxs);
	lctxs = cdr(lctxs);
	setctx (ctx);    
    }
}

alt1 ()
{
    lctxs = insctx (getctx(), lctxs);
    code = cdr(code);
}

setctxs (pointer ctxs)
{
/*
    lctxs = inslctxs (ctxs, lctxs);
    end ();
*/
    if (!ispair(ctxs))
	end ();
    else
    {
	lctxs = inslctxs (cdr(ctxs), lctxs);
	setctx (car(ctxs));
    }
}

#define RPLACA(x,y) (CAR(x)=(y))
#define RPLACD(x,y) (CDR(x)=(y))
/*
#define CAR car
#define CDR cdr
*/

#define car fcar
#define cdr fcdr

#define STRAT(ctx) (car(ctx))
#define PROG(ctx) (cadr(ctx))
#define BAS(ctx) (caddr(ctx))

#define LIST1(x) (cons((x),NIL))
#define VERITE istrue
#define verite istrue
#define vrai T
#define faux NIL
#define nil NIL

/*
#define EQ(x,y) ((x)==(y))
#define eq(x,y) ((x)==(y))
*/

int eq (pointer x, pointer y)
{
    if (isnumber(x) && isnumber(y))
	return (ivalue(x) == ivalue(y));
    return x == y;
}

#define EQ(x,y) eq(x,y)

#define consp ispair
#define econsp ispair

#define symbole mk_symbol
#define creer_symbole mk_symbol

#define sexpr_ent mk_number
#define ents_sexpr ivalue

pointer inslctxs (pointer l1, pointer l2)
{
    if (!ispair(l1))
	return l2;
    return insctx (car(l1), inslctxs (cdr(l1), l2));
}

pointer last (pointer l)
{
    if (!ispair(l))
	return NIL;
    if (!ispair(cdr(l)))
	return l;
    return last(cdr(l));
}

#define EXIT_CYCLE_NORMAL 0
#define EXIT_CYCLE_STOP 1
#define EXIT_CYCLE_END 2

int Eval_Cycle1 ();

pointer revol_ctx1 (pointer ctx)
{
pointer svreg[MAX_CTX+1];
int i, status;
    for (i=MIN_CTX; i<=MAX_CTX; i++)
    {
	svreg[i] = reg[i];
	push_newexpr (svreg[i]);
    }
    setctx (ctx);
    /* boucle interp */
    status = Eval_Cycle1 ();
    if (status == EXIT_CYCLE_END)
	ctx = NIL;
    else
	ctx = getctx();
    for (i=MIN_CTX; i<=MAX_CTX; i++)
    {
	reg[i] = svreg[i];
	pop_newexpr ();
    }
    return ctx;
}

enfiler (sexpr prio, sexpr ctx, sexpr file)
{
sexpr x;
    if (!VERITE(prio))
    {
	x = last (file);
	RPLACD (x, LIST1(ctx));
	return;
    }
    if (PRIO(CAR(file)) < PRIO(ctx))
    {
	RPLACD (file, cons(CAR(file),CDR(file)));
	RPLACA (file, ctx);
	return;
    }
    if (!VERITE(CDR(file)))
    {
	RPLACD (file, LIST1(ctx));
	return;
    }
    enfiler (prio, ctx, CDR(file));
}

#define RS_RCV 1
#define RS_SND 2

sexpr instr_exchange (sexpr ctx, int rs)
{
sexpr canal, x, xc, flag, file, prio, ctxc;
    canal = CAR(BAS(ctx));
    if (rs & RS_SND)
	x = CAR(CDR(BAS(ctx)));
    if (!VERITE(CAR(CDR(CDR(canal)))))
    {
	RPLACD (canal, cons(((rs&RS_SND)?vrai:faux),
	    cons(LIST1(ctx),
	     cons(CAR(CDR(CDR(CDR(canal)))),
	      nil))));
	return vrai;
    }
    flag = CAR(CDR(canal));
/*
    printf ("flag:%d %x NIL=%x F=%x canal:\n", istrue(flag), flag, NIL, F);
    print (canal);
*/
    file = CAR(CDR(CDR(canal)));
    prio = CAR(CDR(CDR(CDR(canal))));
    if (((rs&RS_SND) && VERITE(flag)) ||
        ((rs&RS_RCV) && !VERITE(flag)))
    {
	enfiler (prio, ctx, file);
	return vrai;
    }
    RPLACA (CDR(CDR(canal)), CDR(file));
    ctxc = CAR(file);
    return cons(
	    cons(STRAT(ctxc),
	     cons(PROG(ctxc),
	       cons((rs&RS_SND)?cons(x,CDR(BAS(ctxc))):
		    CDR(CDR(BAS(ctxc))),
	        CDR(CDR(CDR(ctxc))) ))),
	cons(
	     cons(STRAT(ctx),
	      cons(PROG(ctx),
	       cons((rs&RS_SND)?CDR(CDR(BAS(ctx))):
		     cons(xc,CDR(BAS(ctx))),
		CDR(CDR(CDR(ctx))) ))),
	 nil));
}

sexpr instr_send (sexpr ctx)
{
    return instr_exchange (ctx, RS_SND);
}

sexpr instr_receive (sexpr ctx)
{
    return instr_exchange (ctx, RS_RCV);
}


int gettype (pointer x)
{
    if (isatom(x)) 
	return 0;
    if (isproc(x))
	return 1;
    if (isnumber(x))
	return 2;
    if (ispair(x))
	return 8;
    if (issymbol(x))
	return 14;
    return 16;
}

int length (sexpr l)
{
    if (!consp(l))
	return 0;
    return 1 + length (CDR(l));
}

sexpr member_e (int (*e) (sexpr, sexpr), sexpr x, sexpr l)
{
sexpr r;
    if (!consp(l))
	return nil;
    if ((*e)(x,CAR(l)))
    {
	r = l;
	/* SET_VERITE (r, 1); */
	return r;
    }
    return member_e (e, x, CDR(l));
}

int f_eq (sexpr x, sexpr y)
{
    return EQ(x,y);
}

int equal (sexpr x, sexpr y)
{
    if (EQ(x,y))
	return 1;
    if (consp(x) != consp(y))
	return 0;
    if (!consp(x))
	return 0;    
    return equal(CAR(x),CAR(y)) && 
	   equal(CDR(x),CDR(y));
}

sexpr memq (sexpr x, sexpr y)
{
    return member_e (f_eq, x, y);
}

sexpr member (sexpr x, sexpr y)
{
    return member_e (equal, x, y);
}

sexpr separer (int n, sexpr l)
{
sexpr r;
    if (n == 0)
	return cons (nil, l);
    r = separer (n-1, cdr(l));
    return cons (cons (car(l), car(r)), cdr(r));
}

sexpr remove_e (int (*e) (sexpr, sexpr), sexpr x, sexpr l)
{
    if (!consp(l))
	return l;
    if ((*e)(CAR(l),x))
	return CDR(l);
    return cons (CAR(l), remove_e (e, x, CDR(l)));
}

sexpr remq (sexpr x, sexpr l)
{
    return remove_e (f_eq, x, l);
}

sexpr remov (sexpr x, sexpr l)
{
    return remove_e (equal, x, l);
}

int incl_e (int (*e) (sexpr, sexpr), sexpr l1, sexpr l2)
{
sexpr x;
    if (!consp(l1))
	return 1;
    x = member_e (e, CAR(l1), l2);
    return VERITE(x) && incl_e (e, CDR(l1), l2);
}

int inclq (sexpr l1, sexpr l2)
{
    return incl_e (f_eq, l1, l2);
}

int incl (sexpr l1, sexpr l2)
{
    return incl_e (equal, l1, l2);
}


pointer getvenv (pointer env, pointer var)
{
    if (!ispair(env))
	return NIL;
    if (EQ(car(CAR(env)),var))
	return car(cdr(CAR(env)));
    return getvenv (CDR(env), var);
}

pointer setvenv (pointer env, pointer var, pointer val)
{
    if (!ispair(env))
	return cons(cons(var,cons(val,nil)),env);
    if (EQ(car(CAR(env)),var))
	return cons(cons(var,cons(val,
		cdr(cdr(CAR(env))))),CDR(env));
    return cons(CAR(env),setvenv(CDR(env),var,val));
}

pointer addvenv (pointer env, pointer var, pointer val)
{
    if (!ispair(env))
	return cons(cons(var,cons(val,NIL)),env);
    if (EQ(car(CAR(env)),var))
	return cons(cons(var,cons(val,
	    cdr(CAR(env)))),CDR(env));
    return cons(CAR(env),addvenv(CDR(env),var,val));
}

pointer remvenv (pointer env, pointer var)
{
pointer x, t[10];
    if (!ispair(env))
	return env;
    if (EQ(car(CAR(env)),var))
    {
	x = cdr(cdr(CAR(env)));
	if (!tab_varsys[VARSYS_GARDER_VARS] && !consp(x))
	    return CDR(env);
	/* return cons(cons(var,x),CDR(env)); */
	t[1] = cons (var, x);
	t[2] = CDR(env);
	t[3] = cons (t[1], t[2]);
	return t[3];
    }
    /* return cons(CAR(env),remvenv(CDR(env),var)); */
    t[4] = CAR(env);
    t[5] = CDR(env);
    t[6] = remvenv (t[5], var);
    t[7] = cons (t[4], t[6]);
    return t[7];
}

sexpr getvsenv (sexpr env, sexpr vars)
{
    if (!econsp(vars))
	return getvenv(env,vars);
    if (EQ(CAR(vars),LPIA_QUOTE) && consp(CDR(vars)))
	return CAR(CDR(vars));
    return cons(getvsenv(env,CAR(vars)),
	    getvsenv(env,CDR(vars)));
}

sexpr bindvenv (sexpr env, sexpr vars, sexpr vals)
{
sexpr a, b;
    if (!VERITE(vars))
	return env;
    if (!econsp(vars))
	return addvenv(env,vars,vals);
    a = (consp(vals)) ? car(vals) : vals;
    b = (consp(vals)) ? cdr(vals) : vals;
    return bindvenv(bindvenv(env,car(vars),a),cdr(vars),b);
}

sexpr unbindvenv (sexpr env, sexpr vars)
{
sexpr a, b, t[10];
    if (!VERITE(vars))
    {
	if (env->_flag == 0)
	    break_debug();
	return env; 
    }	
    if (!econsp(vars))
    {
	/* return remvenv(env,vars); */
	t[1] = remvenv(env,vars);
	if (t[1]->_flag == 0)
	    break_debug();
        return t[1];
    }
    /* return unbindvenv(unbindvenv(env,car(vars)),cdr(vars)); */
    t[1] = car(vars);
    if ((int)env == 5612684 && (int)(t[1]) == 7828344)
	break_debug();
    t[2] = unbindvenv (env, t[1]);
    if (t[2]->_flag == 0)
	break_debug();
    if ((int)vars == 7828416 && (int)(t[2]) == 5613488)
	break_debug();
    t[3] = cdr(vars);
    if (t[2]->_flag == 0)
	break_debug();
    t[4] = unbindvenv (t[2], t[3]);
    if (t[4]->_flag == 0)
	break_debug();
    return t[4];
}

#define CREER_VAR(x) (cons(VAR,cons(x,nil)))
#define NOM_VAR(x) (CAR(CDR(x)))
#define EST_VAR(x) (consp(x) && EQ(VAR,CAR(x)))
#define EST_VAN(x) (EST_VAR(x) && EQ(nil,NOM_VAR(x)))

/* char carac_gensym = '0'; */

int n_gensym = 0;

sexpr gensym ()
{
char s[7];
    n_gensym++;
    sprintf (s, "GEN-%d", n_gensym);
    /*
    s[4] = carac_gensym++;
    s[5] = 0;
    */
    return creer_symbole(s);
}

int boundvenv (sexpr env, sexpr var)
{
    if (!consp(env))
	return 0;
    if (EQ(car(CAR(env)),var)
	&& consp(CDR(CAR(env)))
	&& !EQ(CAR(CDR(CAR(env))),UNDEFINED))
	return 1;
    return boundvenv(CDR(env),var);
}

sexpr getrecvenv (sexpr env, sexpr var)
{
sexpr val;
    if (!boundvenv(env,var))
	return CREER_VAR(var);
    val = getvenv(env,var);
    if (!EST_VAR(val))
	return val;
    return getrecvenv(env,NOM_VAR(val));
}

sexpr ensemble (sexpr l)
{
    if (!consp(l))
	return l;
    if (verite(memq(CAR(l),CDR(l))))
	return ensemble(CDR(l));
    return cons(CAR(l),ensemble(CDR(l)));
}

sexpr variables1 (sexpr x)
{
    if (EST_VAR(x))
	return cons(NOM_VAR(x),nil);
    if (!econsp(x))
	return nil;
    return append (variables1(CAR(x)), variables1(CDR(x)));
}

sexpr variables (sexpr x)
{
    return ensemble (variables1 (x));
}

sexpr subst (sexpr x, sexpr y, sexpr z)
{
    if (EQ(y,z))
	return x;
    if (!econsp(z))
	return z;
    return cons (subst(x,y,CAR(z)), subst(x,y,CDR(z)));
}

sexpr renommer1 (sexpr x, sexpr vars)
{
    if (!econsp(vars))
	return x;
    return renommer1 (subst(gensym(),CAR(vars),x),CDR(vars));
}

sexpr renommer (sexpr x)
{
    return renommer1 (x, variables(x));
}

sexpr construire (sexpr env, sexpr x)
{
    if (EST_VAR(x))
	return getrecvenv(env,NOM_VAR(x));
    if (!econsp(x))
	return x;
    return cons (construire(env,CAR(x)),
	         construire(env,CDR(x)));
}

#define LIER(env,var,val) \
    ((tab_varsys[VARSYS_ADDV]?addvenv:setvenv)(env,var,val))

/* #define LIER(env,var,val) addvenv(env,var,val) */

sexpr unifier1 (sexpr env, sexpr x, sexpr y);

sexpr unifier (sexpr env, sexpr x, sexpr y)
{
    if (!VERITE(env))
	return env;
    return unifier1(env,construire(env,x),construire(env,y));
}

sexpr unifier1 (sexpr env, sexpr x, sexpr y)
{
    if (EST_VAN(x) || EST_VAN(y) || equal(x,y))
	return env;
    if (EST_VAR(x) && (!tab_varsys[VARSYS_OCCUR_CHECK]
	|| !verite(memq(NOM_VAR(x),variables(y)))))
	return LIER(env,NOM_VAR(x),y);
    if (EST_VAR(y) && (!tab_varsys[VARSYS_OCCUR_CHECK]
	|| !verite(memq(NOM_VAR(y),variables(x)))))
	return LIER(env,NOM_VAR(y),x);
    if (!econsp(x) || !econsp(y))
	return nil;
    return unifier(unifier1(env,CAR(x),CAR(y)),CDR(x),CDR(y));
}

#ifdef V98 
int call (int (*f)(int), int x)
{
    return (*f) (x);
}
#else
long call (long f, long x)
{
union { long l; long (*f) (long); } u;
long r;
	u.l = f;
	r = (*(u.f)) (x);
	return r;
}
#endif


#define INSTR0(op,f) \
	case op: \
		args = cons (f(), args); \
		s_next; 

#define INSTR1(op,f) \
	case op: \
		args = cons (f(car(args)), cdr(args)); \
		s_next; 

#define INSTR2(op,f) \
	case op: \
		args = cons (f(car(args),cadr(args)), cddr(args)); \
		s_next;

#define INSTR2T(op,f) \
	case op: \
		args = cons (f(car(args),cadr(args))?T:nil, cddr(args)); \
		s_next;

#define INSTR3(op,f) \
	case op: \
		args = cons (f(car(args),cadr(args),caddr(args)), \
				cdddr(args)); \
		s_next;

pointer opexe_7 (op) register short op;
{
sexpr x, y, z;
FILE *f;
char buf[1000];
int i, j, n;
pointer p;

	switch (op)
	{
	case OP_FLAMBDA:
		s_return(mk_fclosure(code, envir));
		break;
	case OP_MLAMBDA:
		s_return(mk_mclosure(code, envir));
		break;
	case OP_DOMLAMBDA:
		/*
		code = value;
		s_goto(OP_EVAL);
		*/
			/* make environment */
			envir = cons(NIL, closure_env(code));
			CAR(envir) = bind (car(closure_code(code)),
				args, car(envir));
			code = cdr(closure_code(code));
			args = NIL;
			s_goto(OP_BEGIN);

		break;
#ifdef LPIA
	case OP_LPIA1:
	case OP_LPIA:
        	/*
		code = cdar (args);
		s_goto (ivalue(caar(args)));
		*/
		code = car(args);
                s_next;

	case OP_SCM:
		s_save (OP_RLPIA, NIL, NIL);

	case OP_RLPIA:
		args = cons (value, args);
		s_next;

/* #define args args_lpia */

	case OP_LPIA_QUOTE:
		args = cons (car(code), args);
		code = cdr(code);
		s_next;

	case OP_LPIA_CAR:
		args = cons (car(car(args)), cdr(args));
		s_next;

	case OP_LPIA_PRINTATOM:
		printatom (car(args), 1);
		args = cdr(args);
		s_next;

	case OP_PROTOPRINT:
		protoprint (car(args));
		s_return (car(args));

	case OP_PRINTSTATE:
		printstate();
		s_return (NIL);

	case OP_STEP:
		step = ivalue(car(args));
		s_return (car(args));

	case OP_ELPIA:
		s_return (car(args));

	case OP_LPIA_EDEF:
		/* symdef(car(args)) = cadr(args); */
		set1 (car(args), cadr(args));
		args = cddr(args);
		s_next;		

	case OP_LPIA_DEF:
		set1 (car(code), cadr(code));
		code = cddr(code);
		s_next;

	case OP_I:
		s_next;

	case OP_K:
		code = cons (car(code), cddr(code));
		s_next;

	case OP_S:
		code = cons (car(code), cons (caddr(code),
			cons (cons(cadr(code),cons(caddr(code),NIL)),
				cdr(cdr(cdr(code))) ) ));
		s_next;

	case OP_C:
		code = cons (car(code), cons (caddr(code), cons (cadr(code),
			cdr(cdr(cdr(code))) )));
		s_next;

	case OP_W:
		code = cons (car(code), cons (cadr(code), cons (cadr(code),
			cddr(code) )));
		s_next;

	case OP_Y:
		code = cons (car(code),
			cons (cons (mk_operator(OP_Y), cons (car(code), NIL)),
				cdr(code) ) );
		s_next;

	case OP_B:
		code = cons (car(code), 
			cons (cons (cadr(code), cons(caddr(code), NIL)), 
			    cdddr(code) ) );
		s_next;

	case OP_P:
		code = cons (caddr(code), cons (car(code), cons (cadr(code), cdddr(code))));
		s_next;

	case OP_J:
		code = cdr(code);
		s_next;
	
        case OP_APPLYTO:
	        code = cons (cadr(code), cons (car(code), cddr(code)));
		s_next;
		
	case OP_DEP:
		args = cdr(args);
		s_next;

	case OP_REP:
		args = cons (car(args), args);
		s_next;

	case OP_ECH:
		args = cons (cadr(args), cons (car(args), cddr(args)));
		s_next;

	case OP_DES:
		haut = cons (car(args), haut);
		args = cdr(args);
		s_next;

	case OP_MON:
		args = cons (car(haut), args);
		haut = cdr(haut);
		s_next;

	case OP_MONDEP:
	    	haut = cdr(haut);
		s_next;

	case OP_GETH:
		j = ivalue(car(code));
		for (i=0, p=haut; 
		     i<j && ispair(p); 
		     i++, p=cdr(p));
		args = cons (car(p), args);
		code = cdr(code);
		s_next; 		

	case OP_GETH0:
		args = cons (car(haut), args);
		s_next;

	case OP_GETH1:
		args = cons (cadr(haut), args);
		s_next;

	case OP_GETH2:
		args = cons (caddr(haut), args);
		s_next;

	case OP_GETH3:
		args = cons (cadddr(haut), args);
		s_next;

	case OP_THEN:
		if (istrue(car(args)))
			code = cons (car(code), cddr(code));
		else
			code = cdr(code);
		args = cdr(args);
		s_next;

	case OP_NCONSPTHEN:
		if (!ispair(car(args)))
                    	code = cons (car(code), cddr(code));
                else
			code = cdr(code);
		args = cdr(args);
		s_next;

	case OP_LPIA_NOT:
		if (istrue(car(args)))
		    args = cons(NIL,cdr(args));
		else
		    args = cons(T,cdr(args));
		s_next;

	case OP_EXEC:
		code = cons (car(args), code);
		args = cdr(args);
		s_next;

	case OP_GETREG:
		args = cons (reg[ivalue(car(args))], cdr(args));
		s_next;

	case OP_SETREG:
		reg[ivalue(car(args))] = cadr(args);
		args = cddr(args);
		s_next;

	case OP_GETPROG:
		args = cons (code, args);
		s_next;

	case OP_SETPROG:
		code = car(args);
		args = cdr(args);
		s_next;

	case OP_GETSTACK:
		args = cons (args, args);
		s_next;

	case OP_SETSTACK:
		args = car(args);
		s_next;

	case OP_GETCODE:
		args = cons (code, args);
		s_next;

	case OP_SETCODE:
		code = car(args);
		args = cdr(args);
		s_next;

	case OP_GETARGS:
		args = cons (args, args);
		s_next;

	case OP_SETARGS:
		args = car(args);
		s_next;

	case OP_GETENV:
		args = cons (envir_lpia, args);
		s_next;

	case OP_SETENV:
		envir_lpia = car(args);
		args = cdr(args);
		s_next;

	case OP_GETENVIR:
		args = cons (envir, args);
		s_next;

	case OP_SETENVIR:
		envir = car(args);
		args = cdr(args);
		s_next;

	case OP_GETDUMP:
		args = cons (dump, args);
		s_next;

	case OP_SETDUMP:
		dump = car(args);
		args = cdr(args);
		s_next;

	case OP_GETVALUE:
		args = cons (value, args);
		s_next;

	case OP_SETVALUE:
		value = car(args);
		args = cdr(args);
		s_next;

	case OP_GETLCTXS:
		args = cons (lctxs, args);
		s_next;

	case OP_SETLCTXS:
		lctxs = car(args);
		args = cdr(args);
		s_next;

	case OP_GETLLCTXS:
		args = cons (llctxs, args);
		s_next;

        case OP_SETLLCTXS:
                llctxs = car(args);
                args = cdr(args);
                s_next;

	case OP_GETSTRAT:
		args = cons (strat, args);
		s_next;

        case OP_SETSTRAT:
		strat = car(args);
		args = cdr(args);
		s_next;

	case OP_GETCTX:
	        args = cons (getctx(), args);
		s_next;

	case OP_GETCTX_ENLINSTR:
		args = cons (getctx_enlinstr(), args);
		s_next;

	case OP_SETCTX:
		setctx (car(args));
		s_next;

	case OP_SETCTXS:
		setctxs (car(args));
		s_next;

	case OP_END:
		end ();
		s_next;

	case OP_INSCTX:
		args = cons (insctx(car(args),cadr(args)), cddr(args));
		s_next;

	case OP_ALT1:
		alt1 ();
		s_next;
		
	case OP_ALT:
		code = cons (mk_operator(OP_ALT1),
			cons (cons (mk_operator(OP_K), 
			       cons (cadr(code), NIL) ),
			 cons (cons (mk_operator(OP_ALT1),
			        cons (cons (mk_operator(OP_K),
                                       cons (car(code), NIL) ),
			         cons (mk_operator(OP_END), NIL) ) ),
			  cddr(code) ) ) );
		s_next;

	case OP_INSTR_SEND:
		args = cons (instr_send(car(args)), cdr(args));
		s_next;

	case OP_INSTR_RECEIVE:
		args = cons (instr_receive(car(args)), cdr(args));
		s_next;

	case OP_REVOL1:
		args = cons (revol_ctx1(car(args)), cdr(args));
		s_next;
 
	case OP_STOP:
		fprintf (outfp, "STOP\n");
		s_next;

	/* INSTR1 (OP_LPIA_CDR, cdr) */

	case OP_LPIA_CDR:
		args = cons (cdar(args), cdr(args));
		s_next;

	case OP_LPIA_CONS:
		args = cons (cons(car(args),cadr(args)), cddr(args));
		s_next;

	case OP_LPIA_EQ:
		args = cons (eq(car(args),cadr(args))?T:NIL, cddr(args));
		s_next;

	case OP_LPIA_RPLACA:
		CAR(car(args)) = cadr(args);
		args = cddr(args);
		s_next;

	case OP_LPIA_RPLACD:
		CDR(car(args)) = cadr(args);
		args = cddr(args);
		s_next;

	case OP_LPIA_PRINT:
		print_nl(car(args));
		args = cdr(args);
		s_next;

	case OP_LPIA_PRIN:
                print(car(args));
                args = cdr(args);
                s_next;

	case OP_PRINTSTRING:
		fprintf (outfp, "%s", strvalue(car(args)));
		args = cdr(args);
		s_next;		

	case OP_READSTRING:
		gets (buf);
		args = cons (mk_string(buf), args);
		s_next;

	case OP_LPIA_TYI:
		args = cons (mk_number((long)(getchar())), args);
		s_next;

	case OP_LPIA_TYO:
		putchar (ivalue(car(args)));
		args = cdr(args);
		s_next;

	case OP_LENGTH:
		args = cons (sexpr_ent(length(car(args))), cdr(args));
		s_next;

/*
	case OP_EQUAL:
		if (equal (car(args), cadr(args)))
		    args = cons (T, cddr(args));
		else
		    args = cons (nil, cddr(args));
		s_next;
*/
	INSTR2T (OP_EQUAL, equal)
	/* INSTR2 (OP_MEMQ, memq) */
	case OP_MEMQ:
	    args = cons (memq(cadr(args),car(args)), cddr(args));
	    s_next;

	/* INSTR2 (OP_MEMBER, member) */
	case OP_MEMBER:
	    args = cons (member(cadr(args),car(args)), cddr(args));
	    s_next;

	case OP_SEPARER:
		args = cons (separer (ent_sexpr(car(args)), cadr(args)),
				cddr(args));
		s_next;

	INSTR2 (OP_REMQ, remq)
	INSTR2 (OP_REMOVE, remov)

	INSTR2T (OP_INCLQ, inclq)
	INSTR2T (OP_INCL, incl)

	/* INSTR1 (OP_DECLSYM, declsym) */
	case OP_DECLSYM:
	    declsym (car(args));
	    s_next;    

	INSTR1 (OP_GETDEF, getval)
	/* INSTR2 (OP_SETDEF, setval) */
	case OP_SETDEF:
	    setval (car(args), cadr(args));
	    args = cddr(args);
	    s_next;

	case OP_ARG:
	    x = car(code);
	    y = cadr(code);
	    z = car(args);
	    envir_lpia = bindvenv (envir_lpia, x, z);
	    code = cons (y, 
		    cons ( cons (mk_operator(OP_UNBINDVQ), cons(x,nil)) ,
			    cddr(code) ));
	    args = cdr(args);
	    s_next;

	case OP_GETVQ:
	    x = car(code);
	    code = cdr(code);
	    args = cons (getvenv(envir_lpia,x), args);	   
	    s_next;

	case OP_GETVSQ:
	    x = car(code);
	    code = cdr(code);
	    args = cons (getvsenv(envir_lpia,x), args);	   
	    s_next;

	case OP_SETVQ:
	    x = car(code);
	    code = cdr(code);
	    y = car(args);
	    args = cdr(args);
	    envir_lpia = setvenv (envir_lpia, x, y);
	    s_next;

	case OP_BINDVQ:
	    x = car(code);
	    code = cdr(code);
	    y = car(args);
	    args = cdr(args);
	    envir_lpia = bindvenv (envir_lpia, x, y);
	    s_next;

	case OP_UNBINDVQ:
	    x = car(code);
	    code = cdr(code);
	    envir_lpia = unbindvenv (envir_lpia, x);
	    s_next;

	INSTR2 (OP_GETVENV, getvenv)
	INSTR3 (OP_SETVENV, setvenv)
	INSTR3 (OP_ADDVENV, addvenv)
	INSTR2 (OP_REMVENV, remvenv)

	INSTR2 (OP_GETVSENV, getvsenv)
	INSTR3 (OP_BINDVENV, bindvenv)
	INSTR2 (OP_UNBINDVENV, unbindvenv)

	INSTR0 (OP_GENSYM, gensym)
	INSTR2T (OP_BOUNDVENV, boundvenv)
	INSTR2 (OP_GETRECVENV, getrecvenv)
	INSTR1 (OP_ENSEMBLE, ensemble)
	INSTR1 (OP_VARIABLES, variables)
	INSTR3 (OP_SUBST, subst)
	INSTR1 (OP_RENOMMER, renommer)
	INSTR2 (OP_CONSTRUIRE, construire)	
	INSTR3 (OP_UNIFIER, unifier)

	INSTR2 (OP_LPIA_APPEND, append)
	INSTR1 (OP_LPIA_REVERSE, reverse)
    
	case OP_LPIA_READ:
	    args = cons (read_sexpr_file(stdin), args);
	    s_next;
	
	case OP_LPIA_READFILE:
	    f = fopen (strvalue(car(args)), "r");
	    if (f == NULL)
		args = cons (NIL, cdr(args));
	    else
	    {
		args = cons (read_sexpr_file(f), cdr(args));
		fclose (f);
	    }
	    s_next;

	case OP_LPIA_PLUS:
	    args = cons (mk_number (ivalue(car(args)) + ivalue(cadr(args))),
			    cddr(args));
	    s_next;

	case OP_LPIA_MOINS:
	    args = cons (mk_number (ivalue(cadr(args)) - ivalue(car(args))),
			    cddr(args));
	    s_next;

	case OP_LPIA_FOIS:
	    args = cons (mk_number (ivalue(car(args)) * ivalue(cadr(args))),
			    cddr(args));
	    s_next;

	case OP_LPIA_DIV:
	    args = cons (mk_number (ivalue(cadr(args)) / ivalue(car(args))),
			    cddr(args));
	    s_next;

	case OP_LPIA_MOD:
	    args = cons (mk_number (ivalue(cadr(args)) % ivalue(car(args))),
			    cddr(args));
	    s_next;

	case OP_LPIA_ETL:
	    args = cons (mk_number (ivalue(car(args)) & ivalue(cadr(args))),
			    cddr(args));
	    s_next;

	case OP_LPIA_OUL:
	    args = cons (mk_number (ivalue(car(args)) | ivalue(cadr(args))),
			    cddr(args));
	    s_next;

	case OP_LPIA_OXL:
	    args = cons (mk_number (ivalue(car(args)) ^ ivalue(cadr(args))),
			    cddr(args));
	    s_next;

	case OP_LPIA_ZEROP:
	    if (ivalue(car(args)) == 0)
		args = cons (T, cdr(args));
	    else
		args = cons (NIL, cdr(args));
	    s_next;

	case OP_LPIA_POSP:
	    if (ivalue(car(args)) >= 0)
		args = cons (T, cdr(args));
	    else
		args = cons (NIL, cdr(args));
	    s_next;

	case OP_LPIA_GETPROPS:
	    args = cons (symprop(car(args)), cdr(args));
	    s_next;

	case OP_LPIA_SETPROPS:
	    symprop(car(args)) = cadr(args);
	    args = cddr(args);
	    s_next;

	case OP_LPIA_GETPROP:
	    args = cons (getvenv (symprop(car(args)), cadr(args)), cddr(args));
	    s_next;

	case OP_LPIA_PUTPROP:
	    symprop(car(args)) = setvenv (symprop(car(args)), cadr(args),
					    caddr(args));
	    args = cdddr(args);
	    s_next;

	case OP_LPIA_ADDPROP:
	    symprop(car(args)) = addvenv (symprop(car(args)), cadr(args),
					    caddr(args));
	    args = cdddr(args);
	    s_next;

	case OP_LPIA_REMPROP:
	    symprop(car(args)) = remvenv (symprop(car(args)), cadr(args));
	    args = cddr(args);
	    s_next;

	case OP_LPIA_GETTYPE:
	    args = cons (mk_number(gettype(car(args))), cdr(args));
	    s_next;

	case OP_LPIA_MAKESTRING:
	    n = ivalue(car(args));
	    for (i=0; i<n; i++)
		buf[i] = ' ';
	    buf[n] = 0;
	    args = cons (mk_string(buf), cdr(args));
	    s_next;

	case OP_LPIA_CREF:
	    args = cons (mk_number (strvalue(car(args))[ivalue(cadr(args))]),
			    cddr(args));
	    s_next;

	case OP_LPIA_CSET:
	    strvalue(car(args))[ivalue(cadr(args))] = ivalue(caddr(args));
	    args = cdddr(args);
	    s_next;

	case OP_LPIA_CATENATE:
	    sprintf (buf, "%s%s", strvalue(cadr(args)), strvalue(car(args)));
	    args = cons (mk_string (buf), cddr(args));
	    s_next;

	case OP_PEEK:
	    args = cons (mk_number (*(char *)(ivalue(car(args)))), cdr(args));
	    s_next;

	case OP_POKE:
	    *(char *)(ivalue(car(args))) = ivalue(cadr(args));
	    s_next;

	case OP_USR:
	    args = cons (mk_number (call (ivalue(car(args)), 
					  ivalue(cadr(args)))),
		cddr(args));
	    s_next;

	case OP_EXECMACH:
	    {
	    char machcode[1000];
	    int i;
	    pointer p;
		for (i=0, p=car(args); 
		     i<sizeof(machcode) && ispair(p);
		     i++, p=cdr(p))
		    machcode[i] = ivalue(car(p));
		args = cons (mk_number(call(
#ifndef V98
					    (long)
#endif
					    machcode,ivalue(cadr(args)))),
				cddr(args));
		s_next; 
	    }

	case OP_GETERRNO:
	    args = cons (mk_number(errno), cdr(args));
	    s_next;

	case OP_FOPEN:
	    {
	    char buf[16];
	    int i;
	    pointer x;
#ifdef VMS /* V98 */
		if (car(args) == NIL)
#endif
		    f = fopen (strvalue(caddr(args)), strvalue(cadr(args)));
#ifdef VMS
		else
		    f = fopen (strvalue(caddr(args)), strvalue(cadr(args)),
				strvalue(car(args)));
#endif
		if (f == NULL)
		    args = cons (NIL, cdddr(args));
		else
		{
		    for (i=0; i<sizeof(f); i++)
			buf[i] = ' ';
		    buf[sizeof(f)] = 0;
		    x = mk_string (buf);
		    memcpy (strvalue(x), &f, sizeof(f));
		    args = cons (x, cdddr(args));
		}
		s_next;
	    }

	case OP_FCLOSE:
	    memcpy (&f, strvalue(car(args)), sizeof(f));
	    args = cons (mk_number (fclose(f)), cdr(args));
	    s_next;

	case OP_FEOF:
	    memcpy (&f, strvalue(car(args)), sizeof(f));
	    args = cons (mk_number (feof(f)), cdr(args));
	    s_next;

	case OP_FGETC:
	    memcpy (&f, strvalue(car(args)), sizeof(f));
	    args = cons (mk_number (fgetc(f)), cdr(args));
	    s_next;

	case OP_FPUTC:
	    memcpy (&f, strvalue(car(args)), sizeof(f));
	    args = cons (mk_number (fputc(ivalue(cadr(args)),f)), cddr(args));
	    s_next;

	case OP_FSEEK:
	    memcpy (&f, strvalue(car(args)), sizeof(f));
	    args = cons (mk_number (fseek (f, ivalue(caddr(args)),
					    ivalue(cadr(args)) )), cdddr(args));
	    s_next;

	case OP_STEPON:
	    step = 1;
	    s_next;

	case OP_LPIA_QUIT:
	    exit (ivalue(car(args)));

#endif
	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;
}

#define car(p)          ((p)->_object._cons._car)
#define cdr(p)          ((p)->_object._cons._cdr)

pointer	(*dispatch_table[])() = {
	opexe_0,	/* OP_LOAD = 0, */
	opexe_0,	/* OP_T0LVL, */
	opexe_0,	/* OP_T1LVL, */
	opexe_0,	/* OP_READ, */
	opexe_0,	/* OP_VALUEPRINT, */
	opexe_0,	/* OP_EVAL, */
	opexe_0,	/* OP_E0ARGS, */
	opexe_0,	/* OP_E1ARGS, */
	opexe_0,	/* OP_APPLY, */
	opexe_0,	/* OP_DOMACRO, */

	opexe_0,	/* OP_LAMBDA, */
	opexe_0,	/* OP_QUOTE, */
	opexe_0,	/* OP_DEF0, */
	opexe_0,	/* OP_DEF1, */
	opexe_0,	/* OP_BEGIN, */
	opexe_0,	/* OP_IF0, */
	opexe_0,	/* OP_IF1, */
	opexe_0,	/* OP_SET0, */
	opexe_0,	/* OP_SET1, */
	opexe_0,	/* OP_LET0, */
	opexe_0,	/* OP_LET1, */
	opexe_0,	/* OP_LET2, */
	opexe_0,	/* OP_LET0AST, */
	opexe_0,	/* OP_LET1AST, */
	opexe_0,	/* OP_LET2AST, */

	opexe_1,	/* OP_LET0REC, */
	opexe_1,	/* OP_LET1REC, */
	opexe_1,	/* OP_LETREC2, */
	opexe_1,	/* OP_COND0, */
	opexe_1,	/* OP_COND1, */
	opexe_1,	/* OP_DELAY, */
	opexe_1,	/* OP_AND0, */
	opexe_1,	/* OP_AND1, */
	opexe_1,	/* OP_OR0, */
	opexe_1,	/* OP_OR1, */
	opexe_1,	/* OP_C0STREAM, */
	opexe_1,	/* OP_C1STREAM, */
	opexe_1,	/* OP_0MACRO, */
	opexe_1,	/* OP_1MACRO, */
	opexe_1,	/* OP_CASE0, */
	opexe_1,	/* OP_CASE1, */
	opexe_1,	/* OP_CASE2, */

	opexe_1,	/* OP_PEVAL, */
	opexe_1,	/* OP_PAPPLY, */
	opexe_1,	/* OP_CONTINUATION, */

	opexe_2,	/* OP_ADD, */
	opexe_2,	/* OP_SUB, */
	opexe_2,	/* OP_MUL, */
	opexe_2,	/* OP_DIV, */
	opexe_2,	/* OP_REM, */
	opexe_2,	/* OP_CAR, */
	opexe_2,	/* OP_CDR, */
	opexe_2,	/* OP_CONS, */
	opexe_2,	/* OP_SETCAR, */
	opexe_2,	/* OP_SETCDR, */

	opexe_3,	/* OP_NOT, */
	opexe_3,	/* OP_BOOL, */
	opexe_3,	/* OP_NULL, */
	opexe_3,	/* OP_ZEROP, */
	opexe_3,	/* OP_POSP, */
	opexe_3,	/* OP_NEGP, */
	opexe_3,	/* OP_NEQ, */
	opexe_3,	/* OP_LESS, */
	opexe_3,	/* OP_GRE, */
	opexe_3,	/* OP_LEQ, */
	opexe_3,	/* OP_GEQ, */
	opexe_3,	/* OP_SYMBOL, */
	opexe_3,	/* OP_NUMBER, */
	opexe_3,	/* OP_STRING, */
	opexe_3,	/* OP_PROC, */
	opexe_3,	/* OP_PAIR, */
	opexe_3,	/* OP_EQ, */
	opexe_3,	/* OP_EQV, */

	opexe_4,	/* OP_FORCE, */
	opexe_4,	/* OP_WRITE, */
	opexe_4,	/* OP_DISPLAY, */
	opexe_4,	/* OP_NEWLINE, */
	opexe_4,	/* OP_ERR0, */
	opexe_4,	/* OP_ERR1, */
	opexe_4,	/* OP_REVERSE, */
	opexe_4,	/* OP_APPEND, */
	opexe_4,	/* OP_PUT, */
	opexe_4,	/* OP_GET, */
	opexe_4,	/* OP_QUIT, */
	opexe_4,	/* OP_GC, */
	opexe_4,	/* OP_GCVERB, */
	opexe_4,	/* OP_NEWSEGMENT, */
	
	opexe_5,	/* OP_RDSEXPR, */
	opexe_5,	/* OP_RDLIST, */
	opexe_5,	/* OP_RDDOT, */
	opexe_5,	/* OP_RDQUOTE, */
	opexe_5,	/* OP_RDQQUOTE, */
	opexe_5,	/* OP_RDUNQUOTE, */
	opexe_5,	/* OP_RDUQTSP, */
	opexe_5,	/* OP_P0LIST, */
	opexe_5,	/* OP_P1LIST, */

	opexe_6,	/* OP_LIST_LENGTH, */
	opexe_6,	/* OP_ASSQ, */
	opexe_6,	/* OP_PRINT_WIDTH, */
	opexe_6,	/* OP_P0_WIDTH, */
	opexe_6,	/* OP_P1_WIDTH, */
	opexe_6,	/* OP_GET_CLOSURE, */
	opexe_6,	/* OP_CLOSUREP, */
#ifdef USE_MACRO
	opexe_6,	/* OP_MACROP, */
#endif
	opexe_7,
	opexe_7,
	opexe_7,

#ifdef LPIA
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,

	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	opexe_7,
	
	opexe_7
#endif
};

char buf[200];

/* kernel of this intepreter */
pointer Eval_Cycle(op)
register short op;
{

	operator = op;
	for (;;)
	{
		reset_newexpr ();
		if (step & 2)
		{
			fprintf (outfp, "\nop = %d [%s]",
				operator, keyword[operator]);
			/*
			printstate ();
			gets (buf);
			*/
			top_debug();
                }
		if (operator >= 104)
		{
		    if (opexe_7(operator) == NIL)
			return NIL;
		}
		else if ((*dispatch_table[operator])(operator) == NIL)
			return NIL;
        }
}

int Eval_Cycle1 ()
{
char buf[200];
	operator = next_op ();
	for (;;)
	{
		reset_newexpr();
		if (step & 2)
		{
			fprintf (outfp, "\nop = %d [%s]",
				operator, keyword[operator]);
			/*
			printstate ();
			gets (buf);
			*/
			top_debug();
                }
		if (operator == OP_STOP)
			return /*T*/ EXIT_CYCLE_STOP;
		if (operator == OP_END)
			return EXIT_CYCLE_END;
		if (operator >= 104)
		{
		    if (opexe_7(operator) == NIL)
			return /*NIL*/ EXIT_CYCLE_NORMAL;
		}
		else if ((*dispatch_table[operator])(operator) == NIL)
			return /*NIL*/ EXIT_CYCLE_NORMAL;
        }
}

/* ========== Initialization of internal keywords ========== */

mk_syntax(op, name)
unsigned short op;
char   *name;
{
	pointer x;

	x = cons(mk_string(name), NIL);
	type(x) = (T_SYNTAX | T_SYMBOL);
	syntaxnum(x) = op;
	oblist = cons(x, oblist);

	keyword[op] = name;
}

mk_proc(op, name)
unsigned short op;
char   *name;
{
	pointer x, y;

	x = mk_symbol(name);
	y = get_cell(NIL, NIL);
	type(y) = (T_PROC | T_ATOM);
	ivalue(y) = (long) op;
	car(global_env) = cons(cons(x, y), car(global_env));

        keyword[op] = name;
}


init_vars_global()
{
	pointer x;

	/* init input/output file */
	infp = stdin;
	outfp = stdout;
	/* init NIL */
	type(NIL) = (T_ATOM | MARK);
	car(NIL) = cdr(NIL) = NIL;
	newexpr = NIL;
	newexpr_stack = NIL;
	/* init T */
	type(T) = (T_ATOM | MARK);
	car(T) = cdr(T) = T;
	/* init F */
	type(F) = (T_ATOM | MARK);
	car(F) = cdr(F) = F;

	VAR = mk_symbol ("VAR");
	VAN = CREER_VAR (nil);
	UNDEF = mk_symbol ("_UNDEF");
	LPIA_QUOTE = mk_symbol ("Q");
	GETVSQ = mk_symbol ("GETVSQ");
	UNDEFINED = mk_symbol ("UNDEFINED");	
	S_EOF = mk_symbol ("EOF");
	_UDF = mk_symbol ("_UDF");
	_BRK = mk_symbol ("_BRK");

	/* init global_env */
	global_env = cons(NIL, NIL);
	/* init else */
	x = mk_symbol("else");
	car(global_env) = cons(cons(x, T), car(global_env));
}


init_syntax()
{
	/* init syntax */
	mk_syntax(OP_LAMBDA, "lambda");
	mk_syntax(OP_FLAMBDA, "flambda");
	mk_syntax(OP_MLAMBDA, "mlambda");
#ifdef LPIA
	/*
	mk_syntax(OP_LPIA, "LPIA");
	mk_syntax(OP_SCM, "SCM");
	mk_syntax(OP_RLPIA, "RLPIA");
	*/
#endif
	/* mk_syntax(OP_DOMLAMBDA, "OP_DOMLAMBDA"); */
	mk_syntax(OP_QUOTE, "quote");
	mk_syntax(OP_DEF0, "define");
	mk_syntax(OP_IF0, "if");
	mk_syntax(OP_BEGIN, "begin");
	mk_syntax(OP_SET0, "set!");
	mk_syntax(OP_LET0, "let");
	mk_syntax(OP_LET0AST, "let*");
	mk_syntax(OP_LET0REC, "letrec");
	mk_syntax(OP_COND0, "cond");
	mk_syntax(OP_DELAY, "delay");
	mk_syntax(OP_AND0, "and");
	mk_syntax(OP_OR0, "or");
	mk_syntax(OP_C0STREAM, "cons-stream");
#ifdef USE_MACRO
	mk_syntax(OP_0MACRO, "macro");
#endif
	mk_syntax(OP_CASE0, "case");
}


init_procs()
{
	/* init procedure */
	mk_proc(OP_PEVAL, "eval");
	mk_proc(OP_PAPPLY, "apply");
	mk_proc(OP_CONTINUATION, "call-with-current-continuation");
	mk_proc(OP_FORCE, "force");
	mk_proc(OP_CAR, "car");
	mk_proc(OP_CDR, "cdr");
	mk_proc(OP_CONS, "cons");
	mk_proc(OP_SETCAR, "set-car!");
	mk_proc(OP_SETCDR, "set-cdr!");
	mk_proc(OP_ADD, "+");
	mk_proc(OP_SUB, "-");
	mk_proc(OP_MUL, "*");
	mk_proc(OP_DIV, "/");
	mk_proc(OP_REM, "remainder");
	mk_proc(OP_NOT, "not");
	mk_proc(OP_BOOL, "boolean?");
	mk_proc(OP_SYMBOL, "symbol?");
	mk_proc(OP_NUMBER, "number?");
	mk_proc(OP_STRING, "string?");
	mk_proc(OP_PROC, "procedure?");
	mk_proc(OP_PAIR, "pair?");
	mk_proc(OP_EQV, "eqv?");
	mk_proc(OP_EQ, "eq?");
	mk_proc(OP_NULL, "null?");
	mk_proc(OP_ZEROP, "zero?");
	mk_proc(OP_POSP, "positive?");
	mk_proc(OP_NEGP, "negative?");
	mk_proc(OP_NEQ, "=");
	mk_proc(OP_LESS, "<");
	mk_proc(OP_GRE, ">");
	mk_proc(OP_LEQ, "<=");
	mk_proc(OP_GEQ, ">=");
	mk_proc(OP_READ, "read");
	mk_proc(OP_WRITE, "write");
	mk_proc(OP_DISPLAY, "display");
	mk_proc(OP_NEWLINE, "newline");
	mk_proc(OP_LOAD, "load");
	mk_proc(OP_ERR0, "error");
	mk_proc(OP_REVERSE, "reverse");
	mk_proc(OP_APPEND, "append");
	mk_proc(OP_PUT, "put");
	mk_proc(OP_GET, "get");
	mk_proc(OP_GC, "gc");
	mk_proc(OP_GCVERB, "gc-verbose");
	mk_proc(OP_NEWSEGMENT, "new-segment");
	mk_proc(OP_LIST_LENGTH, "list-length");	/* a.k */
	mk_proc(OP_ASSQ, "assq");	/* a.k */
	mk_proc(OP_PRINT_WIDTH, "print-width");	/* a.k */	
	mk_proc(OP_GET_CLOSURE, "get-closure-code");	/* a.k */
	mk_proc(OP_CLOSUREP, "closure?");	/* a.k */
#ifdef USE_MACRO
	mk_proc(OP_MACROP, "macro?");	/* a.k */
#endif
	mk_proc(OP_QUIT, "quit");

#ifdef LPIA
	mk_proc(OP_LPIA1, "LPIA1");
	mk_proc(OP_LPIA, "LPIA");
	mk_proc(OP_SCM, "SCM");
	mk_proc(OP_RLPIA, "RLPIA");
	mk_proc(OP_LPIA_QUOTE, "Q");
	mk_proc(OP_LPIA_QUOTE, "QUOTE");
	mk_proc(OP_LPIA_CAR, "CAR");
	mk_proc(OP_LPIA_PRINTATOM, "PRINTATOM");
	mk_proc(OP_PROTOPRINT, "protoprint");
	mk_proc(OP_PRINTSTATE, "printstate");
	mk_proc(OP_STEP, "step");
	mk_proc(OP_ELPIA, "ELPIA");

	mk_proc(OP_I, "I");
	mk_proc (OP_K, "K");
	mk_proc (OP_S, "S");
	mk_proc (OP_C, "C");
	mk_proc (OP_W, "W");
	mk_proc (OP_Y, "Y");

        mk_proc (OP_B, "B");
	mk_proc (OP_P, "P");
	mk_proc (OP_J, "KI");
	mk_proc (OP_J, "J");
	mk_proc (OP_APPLYTO, "CI");
	mk_proc (OP_APPLYTO, "APPLYTO");

	mk_proc (OP_DEP, "DEP");
	mk_proc (OP_REP, "REP");
	mk_proc (OP_ECH, "ECH");
	mk_proc (OP_MON, "MON");
	mk_proc (OP_DES, "DES");
	mk_proc (OP_MONDEP, "MONDEP");
	mk_proc (OP_GETH, "GETH");
	mk_proc (OP_GETH0, "GETH0");
	mk_proc (OP_GETH1, "GETH1");
	mk_proc (OP_GETH2, "GETH2");
	mk_proc (OP_GETH3, "GETH3");

	mk_proc (OP_THEN, "THEN");
	mk_proc (OP_LPIA_NOT, "NOT");

	mk_proc (OP_EXEC, "EXEC");

	mk_proc (OP_GETREG, "GETREG");
	mk_proc (OP_SETREG, "SETREG");

	mk_proc (OP_GETPROG, "GETPROG");
	mk_proc (OP_SETPROG, "SETPROG");
	mk_proc (OP_GETSTACK, "GETSTACK");
	mk_proc (OP_SETSTACK, "SETSTACK");

	mk_proc (OP_GETCODE, "GETCODE");
	mk_proc (OP_SETCODE, "SETCODE");
	mk_proc (OP_GETARGS, "GETARGS");
	mk_proc (OP_SETARGS, "SETARGS");
	mk_proc (OP_GETENV, "GETENV");
	mk_proc (OP_SETENV, "SETENV");

	mk_proc (OP_GETENVIR, "GETENVIR");
	mk_proc (OP_SETENVIR, "SETENVIR");
	mk_proc (OP_GETDUMP, "GETDUMP");
	mk_proc (OP_SETDUMP, "SETDUMP");
	mk_proc (OP_GETVALUE, "GETVALUE");
	mk_proc (OP_SETVALUE, "SETVALUE");

	mk_proc (OP_GETLCTXS, "GETLCTXS");
	mk_proc (OP_SETLCTXS, "SETLCTXS");
	mk_proc (OP_GETLLCTXS, "GETLLCTXS");
        mk_proc (OP_SETLLCTXS, "SETLLCTXS");
                                                      
	mk_proc (OP_GETSTRAT, "GETSTRAT");
	mk_proc (OP_SETSTRAT, "SETSTRAT");
	mk_proc (OP_GETCTX, "GETCTX");
	mk_proc (OP_GETCTX_ENLINSTR, "GETCTX-ENLINSTR");
	mk_proc (OP_SETCTX, "SETCTX");
	mk_proc (OP_SETCTXS, "SETCTXS");
	mk_proc (OP_END, "END");
	mk_proc (OP_INSCTX, "INSCTX");
	mk_proc (OP_ALT1, "ALT1");
	mk_proc (OP_ALT, "ALT");
	mk_proc (OP_ALT, "ALT2E");
	mk_proc (OP_ALT, "ALTNP");

	mk_proc (OP_INSTR_SEND, "INSTR-SEND");
	mk_proc (OP_INSTR_RECEIVE, "INSTR-RECEIVE");
	mk_proc (OP_REVOL1, "REVOL1");
	mk_proc (OP_STOP, "STOP1");

	mk_proc (OP_LPIA_CDR, "CDR");
	mk_proc (OP_LPIA_CONS, "CONS");
	mk_proc (OP_LPIA_EQ, "EQ");
	mk_proc (OP_NCONSPTHEN, "NCONSPTHEN");
	mk_proc (OP_LPIA_RPLACA, "RPLACA");
	mk_proc (OP_LPIA_RPLACD, "RPLACD");
	mk_proc (OP_LPIA_PRINT, "PRINT");
	mk_proc (OP_LPIA_PRIN, "PRIN");
	mk_proc (OP_PRINTSTRING, "PRINTSTRING");
	mk_proc (OP_READSTRING, "READSTRING");
	mk_proc (OP_LPIA_TYI, "TYI");
	mk_proc (OP_LPIA_TYO, "TYO");

	mk_proc (OP_LPIA_EDEF, "EDEF");
	mk_proc (OP_LPIA_DEF, "DEF");

	mk_proc (OP_LENGTH, "LENGTH");
	mk_proc (OP_MEMQ, "MEMQ");
	mk_proc (OP_MEMBER, "MEMBER");
	mk_proc (OP_EQUAL, "EQUAL");
	mk_proc (OP_SEPARER, "SEPARER");
	mk_proc (OP_REMQ, "REMQ");
	mk_proc (OP_REMOVE, "REMOVE");
	mk_proc (OP_INCLQ, "INCLQ");
	mk_proc (OP_INCL, "INCL");

	mk_proc (OP_ARG, "ARG");
	mk_proc (OP_GETVQ, "GETVQ");
	mk_proc (OP_GETVSQ, "GETVSQ");
	mk_proc (OP_SETVQ, "SETVQ");
	mk_proc (OP_BINDVQ, "BINDVQ");
	mk_proc (OP_UNBINDVQ, "UNBINDVQ");

	mk_proc (OP_GETVENV, "GETVENV");
	mk_proc (OP_SETVENV, "SETVENV");
	mk_proc (OP_ADDVENV, "ADDVENV");
	mk_proc (OP_REMVENV, "REMVENV");
	mk_proc (OP_GETVSENV, "GETVSENV");
	mk_proc (OP_BINDVENV, "BINDVENV");
	mk_proc (OP_UNBINDVENV, "UNBINDVENV");

	mk_proc (OP_GENSYM, "GENSYM");
	mk_proc (OP_BOUNDVENV, "BOUNDVENV");
	mk_proc (OP_GETRECVENV, "GETRECVENV");
	mk_proc (OP_ENSEMBLE, "ENSEMBLE");
	mk_proc (OP_VARIABLES, "VARIABLES");
	mk_proc (OP_SUBST, "SUBST");
	mk_proc (OP_RENOMMER, "RENOMMER");
	mk_proc (OP_RENOMMER, "RENAME");
	mk_proc (OP_CONSTRUIRE, "CONSTRUIRE");
	mk_proc (OP_CONSTRUIRE, "CONSTR");
	mk_proc (OP_UNIFIER, "UNIFIER");	
	mk_proc (OP_UNIFIER, "UNIF");

	mk_proc (OP_LPIA_APPEND, "APPEND");
	mk_proc (OP_LPIA_REVERSE, "REVERSE"); 

	mk_proc (OP_DECLSYM, "DECLSYM");
	mk_proc (OP_GETDEF, "GETDEF");
	mk_proc (OP_SETDEF, "SETDEF");

	mk_proc (OP_LPIA_READ, "READ");
	mk_proc (OP_LPIA_READFILE, "READFILE"); 

	mk_proc (OP_LPIA_PLUS, "PLUS");
	mk_proc (OP_LPIA_MOINS, "MOINS");
	mk_proc (OP_LPIA_FOIS, "FOIS");
	mk_proc (OP_LPIA_DIV, "DIV");
	mk_proc (OP_LPIA_MOD, "MOD");
	mk_proc (OP_LPIA_ETL, "ETL");
	mk_proc (OP_LPIA_OUL, "OUL");
	mk_proc (OP_LPIA_OXL, "OXL");
	mk_proc (OP_LPIA_ZEROP, "ZEROP");
	mk_proc (OP_LPIA_POSP, "POSP");

	mk_proc (OP_LPIA_GETPROPS, "GETPROPS");
	mk_proc (OP_LPIA_SETPROPS, "SETPROPS");
	mk_proc (OP_LPIA_GETPROP, "GETPROP");
	mk_proc (OP_LPIA_PUTPROP, "PUTPROP");
	mk_proc (OP_LPIA_PUTPROP, "SETPROP");
	mk_proc (OP_LPIA_ADDPROP, "ADDPROP");
	mk_proc (OP_LPIA_REMPROP, "REMPROP");
    
	mk_proc (OP_LPIA_GETTYPE, "GETTYPE");

	mk_proc (OP_LPIA_MAKESTRING, "MAKESTRING");
	mk_proc (OP_LPIA_CREF, "CREF");
	mk_proc (OP_LPIA_CSET, "CSET");
	mk_proc (OP_LPIA_CATENATE, "CATENATE");

	mk_proc (OP_PEEK, "PEEK");
	mk_proc (OP_POKE, "POKE");
	mk_proc (OP_USR, "USR");
	mk_proc (OP_EXECMACH, "EXECMACH");

	mk_proc (OP_FOPEN, "FOPEN");
	mk_proc (OP_FCLOSE, "FCLOSE");
	mk_proc (OP_FGETC, "FGETC");
	mk_proc (OP_FPUTC, "FPUTC");
	mk_proc (OP_FEOF, "FEOF");
	mk_proc (OP_GETERRNO, "GETERRNO");
	mk_proc (OP_FSEEK, "FSEEK");

	mk_proc (OP_STEPON, "STEPON");

	mk_proc (OP_LPIA_QUIT, "QUIT");

#endif

}


/* initialize several globals */
init_globals()
{
	init_vars_global();
	init_syntax();
	init_procs();
	/* intialization of global pointers to special symbols */
	LAMBDA = mk_symbol("lambda");
	QUOTE = mk_symbol("quote");
#ifdef USE_QQUOTE
	QQUOTE = mk_symbol("quasiquote");
	UNQUOTE = mk_symbol("unquote");
	UNQUOTESP = mk_symbol("unquote-splicing");
#endif

}

/* ========== Error ==========  */

FatalError(fmt, a, b, c)
char   *fmt, *a, *b, *c;
{
	fprintf(stderr, "Fatal error: ");
	fprintf(stderr, fmt, a, b, c);
	fprintf(stderr, "\n");
	exit(1);
}

#ifdef USE_SETJMP
Error(fmt, a, b, c)
char   *fmt, *a, *b, *c;
{
	fprintf(stderr, "Error: ");
	fprintf(stderr, fmt, a, b, c);
	fprintf(stderr, "\n");
	flushinput();
	longjmp(error_jmp, OP_T0LVL);
}

#endif

/* ========== Main ========== */

main()
{
	short   op = (short) OP_LOAD;

	printf(banner);
	init_scheme();
	args = cons(mk_string(InitFile), NIL);
#ifdef USE_SETJMP
	op = setjmp(error_jmp);
#endif
	Eval_Cycle(op);
}


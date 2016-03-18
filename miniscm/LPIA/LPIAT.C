
/* Expressions symboliques - J 5-2-87 */
 
#include <stdio.h>
#include <math.h>

#define integer long

#define TAILLE_SEXPR 32

#define TAILLE_TNSEXPRS 0x400 /*0x4000*/

#ifdef DEBUG
#define TAILLE_ZCONS 50
#define TAILLE_ZBLOCS 100
#else
#define TAILLE_ZCONS 0x400 /*0x1000*/ /*0x10000*/
#define TAILLE_ZBLOCS 0x100 /*0x800*/ /*0x10000*/
#endif

#define UNITE_ALLOC (tab_varsys[VARSYS_UNITE_ALLOC])

#define TYPE_AS		0
#define TYPE_INSTR	1
#define TYPE_ENTIER	2
#define TYPE_CHAINE	4
#define TYPE_GENTIER	5
#define TYPE_REEL	6
#define TYPE_COMPLEXE	7
#define TYPE_CONS	8
#define TYPE_VECTEUR	12
#define TYPE_SYMBOLE	14

#define AS_NIL 0

#define N_VARSYS 0x100

#define VARSYS_AUTOFLUSH	0x11
#define VARSYS_ECHO		0x12
#define VARSYS_GCALARM		0x13
#define VARSYS_PROMPT		0x14
#define VARSYS_PRINTLEVEL	0x15
#define VARSYS_ECRIRESEXPR	0x16
#define VARSYS_INTERRUPT	0x17
#define VARSYS_NSEXPRS		0x18
#define VARSYS_MAXNSEXPRS	0x19
#define VARSYS_MASQUE_EQ	0x1A
#define VARSYS_UNITE_ALLOC	0x1B

#define VARSYS_AUTOID		0x21
#define VARSYS_AUTOSCHEDULE	0x22
#define VARSYS_SCHEDULEALARM	0x23
#define VARSYS_ERRFAT		0x24
#define VARSYS_NDF		0x25
#define VARSYS_MARKPROG		0x26

#define VARSYS_DEBUG		0x31
#define VARSYS_NIVEAU_IMP_DEBUG 0x32
#define VARSYS_DEBUG_CONT	0x33
#define VARSYS_DEBUG_IMP	0x34
#define VARSYS_DEBUG_EXEC	0x35
#define VARSYS_DEBUG_CHMD	0x36

#define VARSYS_GARDER_VARS	0x41
#define VARSYS_OCCUR_CHECK	0x42
#define VARSYS_ADDV		0x43
#define VARSYS_UNBOUND		0x44
#define VARSYS_UNDEF		0x45

#define ECRIRESEXPR_DPM		1
#define ECRIRESEXPR_DPD		2
#define ECRIRESEXPR_MARQUE	4
#define ECRIRESEXPR_MARQUELISTE 8
#define ECRIRESEXPR_DEC		0x10
#define ECRIRESEXPR_CHAINE	0x20

int tab_varsys[N_VARSYS];

#define flag_autoflush 		(tab_varsys[VARSYS_AUTOFLUSH])
#define niveau_ecrire_sexpr 	(tab_varsys[VARSYS_PRINTLEVEL])

/*
struct sexpr_s
{
	unsigned int occup: 1;
	unsigned int iad: 1;
	unsigned int verite: 1;
	unsigned int marque: 1;
	unsigned int type: 4;
	int val: 24;
};

typedef struct sexpr_s sexpr;
*/

typedef long sexpr;

#define POS_OCCUP 31
#define POS_IAD 30
#define POS_VERITE 29
#define POS_MARQUE 28
#define POS_TYPE 24
#define POS_VAL 0

#define OCCUP(x) 	(((x)>>POS_OCCUP) & 1)
#define IAD(x)          (((x)>>POS_IAD) & 1)
#define VERITE(x)	(((x)>>POS_VERITE) & 1)
#define MARQUE(x)	(((x)>>POS_MARQUE) & 1)
#define TYPE(x)		(((x)>>POS_TYPE) & 0xF)
#define VAL(x)		(((x)>>POS_VAL) & 0xFFFFFF)

#define SET_OCCUP(x,y) 	((x)=(((x) & ~(1L<<POS_OCCUP)) | (((long)((y)&1))<<POS_OCCUP)))
#define SET_IAD(x,y)	((x)=(((x) & ~(1L<<POS_IAD)) | (((long)((y)&1))<<POS_IAD)))
#define SET_VERITE(x,y) ((x)=(((x) & ~(1L<<POS_VERITE)) | (((long)((y)&1))<<POS_VERITE)))
#define SET_MARQUE(x,y) ((x)=(((x) & ~(1L<<POS_MARQUE)) | (((long)((y)&1))<<POS_MARQUE)))
#define SET_TYPE(x,y)	((x)=(((x) & ~(0xFL<<POS_TYPE)) | (((long)((y)&0xF))<<POS_TYPE)))
#define SET_VAL(x,y)	((x)=(((x) & ~(0xFFFFFFL<<POS_VAL)) | (((long)(((long)(y))&0xFFFFFF))<<POS_VAL)))

#define MK_SEXPR(o,i,v,m,t,val) \
	((((long)o)<<POS_OCCUP) | \
	 (((long)i)<<POS_IAD) | \
	 (((long)v)<<POS_VERITE) | \
	 (((long)m)<<POS_MARQUE) | \
	 (((long)t)<<POS_TYPE) | \
	 (((long)val)<<POS_VAL))

integer conv_ent_sexpr();

init_varsys ()
{
sexpr x;
	tab_varsys[VARSYS_AUTOFLUSH] = 1;
	tab_varsys[VARSYS_GCALARM] = 1;
	tab_varsys[VARSYS_PROMPT] = 0;
	tab_varsys[VARSYS_PRINTLEVEL] = 0x100;
	tab_varsys[VARSYS_ECRIRESEXPR] =
		ECRIRESEXPR_DPM |
		ECRIRESEXPR_MARQUE |
		ECRIRESEXPR_MARQUELISTE |
		ECRIRESEXPR_CHAINE;
	tab_varsys[VARSYS_INTERRUPT] = 0;
	tab_varsys[VARSYS_NSEXPRS] = 0;
	tab_varsys[VARSYS_MASQUE_EQ] = conv_ent_sexpr (MK_SEXPR (0, 0, 1, 1, 0xF, 0xFFFFFF)) >> 24; /* a verifier */
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
}

int debug = 0;

autoflush (FILE *f)
{
	if (flag_autoflush)
		fflush (f);
}

union u_sexpr
{
	sexpr s;
	integer i;
};

typedef sexpr fonction ();

struct paire
{
	sexpr a;
	sexpr d;
};

struct zone_cons
{
	int ptr;
	struct paire tab[TAILLE_ZCONS];
};

/*
typedef struct
{
	unsigned int occup: 1;
	unsigned int symb: 1;
	int adrbloc: 30;
} bloc_item;
*/

typedef long bloc_item;

#define POS_OCCUP_BLOC 31
#define POS_SYMB_BLOC 30
#define POS_ADR_BLOC 0

#define OCCUP_BLOC(x) (((x) >> POS_OCCUP_BLOC) & 1)
#define SYMB_BLOC(x) (((x) >> POS_SYMB_BLOC) & 1)
#define ADR_BLOC(x) (((x) >> POS_ADR_BLOC) & 0x3FFFFFFF)

#define SET_OCCUP_BLOC(x,y) 	((x)=(((x) & ~(1L<<POS_OCCUP_BLOC)) | (((long)((y)&1))<<POS_OCCUP_BLOC)))
#define SET_SYMB_BLOC(x,y)	((x)=(((x) & ~(1L<<POS_SYMB_BLOC)) | (((long)((y)&1))<<POS_SYMB_BLOC)))
#define SET_ADR_BLOC(x,y)	((x)=(((x) & ~(0x3FFFFFFFL<<POS_ADR_BLOC)) | (((long)(((long)(y))&0x3FFFFFFF))<<POS_ADR_BLOC)))

#define MK_BLOC_ITEM(o,s,a) \
	((((long)o) << POS_OCCUP_BLOC) | \
	 (((long)s) << POS_SYMB_BLOC) | \
	 (((long)a) << POS_ADR_BLOC))

struct zone_blocs
{
	int ptrtab;
	int ptrbloc;
	int lim;
	union
	{
		bloc_item s[TAILLE_ZBLOCS];
		integer i[TAILLE_ZBLOCS];
		char c[TAILLE_ZBLOCS*sizeof(integer)];
	} tab;
};

struct nouvelles_sexprs
{
	int ptr;
	sexpr tab[TAILLE_TNSEXPRS];
};

struct etat
{
	sexpr racine;
	struct nouvelles_sexprs nsexprs;
	struct zone_cons zcons;
	struct zone_blocs zblocs;
};

struct etat *e;

struct s_options
{
	int gcalarm;
	int debug;
	int autoid;
};

struct s_options *options;

int option_debug;

sexpr nil, faux, vrai, as_u, as_v;

init_atoms ()
{
	nil = MK_SEXPR (1, 0, 0, 0, 0, 0);
	faux = nil;
	vrai = MK_SEXPR (1, 0, 1, 0, 0, 0);
	as_u = MK_SEXPR (1, 0, 0, 0, 0, 1);
	as_v = MK_SEXPR (1, 0, 1, 0, 0, 1);

}

int symb = 0;

FILE *entree, *sortie, *sorerr;

int getc_entree ()
{
	return getc (entree);
}

int feof_entree ()
{
	return feof (entree);
}

aff_sortie (char *s)
{
	fprintf (sortie, "%s", s);
	autoflush (sortie);
}

aff_sorerr (char *s)
{
	fprintf (sorerr, "%s", s);
	autoflush (sorerr);
}

char buf[1000];

terminer (int x)
{
	exit (x);
}

init_sexpr ()
{
int i;
	e = malloc (sizeof (*e));
	if (e == NULL)
	{
		sprintf (buf, "Error allocating state\n");
		aff_sorerr (buf);
		terminer (0);
	}

	e->racine = nil;
	e->nsexprs.ptr = 0;
	e->zcons.ptr = 0;
	e->zblocs.ptrtab = 0;
	e->zblocs.ptrbloc = 0;
	e->zblocs.lim = 0;

	for (i=0; i<TAILLE_ZCONS; i++)
		SET_OCCUP (e->zcons.tab[i].a, 0);

	options = malloc (sizeof (*options));
	if (options == NULL)
	{
		sprintf (buf, "Cannot allocate options\n");
		terminer (0);
	}

	options->gcalarm = 1;
	options->debug = 0;
	options->autoid = 0;
	option_debug = 0;
	symb = 0;

	init_es ();
}

#define PLACE_ZBLOCS (4*(TAILLE_ZBLOCS-1-e->zblocs.lim)-e->zblocs.ptrbloc-1)
/* a verifier */

valid ()
{
	e->nsexprs.ptr = 0;
	if (e->zcons.ptr >= TAILLE_ZCONS * 3/4 ||
		PLACE_ZBLOCS <= TAILLE_ZBLOCS * 1/4)
		gc();
}

int verif_debord_blocs ()
{
	return 0;
}

int verif_debord_blocs1 ()
{
	if (e->zblocs.ptrtab > e->zblocs.lim)
		e->zblocs.lim = e->zblocs.ptrtab;
	if (PLACE_ZBLOCS <= 0)
	{
		gc();
		if (PLACE_ZBLOCS <= 0)
		{
			erreur ("verif_debord_blocs: debordement de la zone des blocs.\n");
			return 1;
		}
		return 0;
	}
	return 0;
}

gc ()
{
int i;
	if (tab_varsys[VARSYS_GCALARM])
		aff_sorerr ("{GC");

	for (i=0; i<TAILLE_ZCONS; i++)
		SET_OCCUP (e->zcons.tab[i].a, 0);

	if (e->zblocs.ptrtab > e->zblocs.lim)
		e->zblocs.lim = e->zblocs.ptrtab;

	for (i=0; i<e->zblocs.lim; i++)
		SET_OCCUP_BLOC (e->zblocs.tab.s[TAILLE_ZBLOCS-1-i], 1);

	recup (e->racine);

	for (i=0; i<e->nsexprs.ptr; i++)
		recup (e->nsexprs.tab[i]);

	i = 0;
	bouclegc:
		if (i >= TAILLE_ZCONS)
			erreur_fatale ("Debordement de la zone des cons");
		if (OCCUP(e->zcons.tab[i].a) == 0)
		{
			e->zcons.ptr = i;
			SET_OCCUP (e->zcons.tab[i].a, 1);
			goto fingc;
		}
		i++;
		goto bouclegc;
	fingc:

	compacter ();

	if (tab_varsys[VARSYS_GCALARM])
		aff_sorerr ("}");

}

recup (sexpr x)
{
int i, j, adr, lgr;
sexpr y;
	if (debug)
	{
		sprintf (buf, "recup: occup=%d iad=%d verite=%d marque=%d type=%X val=%X\n",
			OCCUP(x), IAD(x), MARQUE(x), TYPE(x), VAL(x));
		aff_sorerr (buf);
	}

	if ((TYPE(x) & 12) == 4) /* bloc atomique */
		SET_OCCUP (e->zblocs.tab.s[TAILLE_ZBLOCS-1-VAL(x)], 1);
	else if ((TYPE(x) == TYPE_CONS) && (OCCUP(e->zcons.tab[VAL(x)].a) == 0))
	{
		SET_OCCUP (e->zcons.tab[VAL(x)].a, 1);
		recup (e->zcons.tab[VAL(x)].a);
		recup (e->zcons.tab[VAL(x)].d);
	}
	else if ((TYPE(x) == TYPE_VECTEUR) &&
		 (OCCUP(e->zblocs.tab.s[TAILLE_ZBLOCS-1-VAL(x)]) == 0))
	{
		SET_OCCUP (e->zblocs.tab.s[TAILLE_ZBLOCS-1-VAL(x)], 1);
		adr = ADR_BLOC(e->zblocs.tab.s[TAILLE_ZBLOCS-1-VAL(x)]);
		/* lgr = *(int *) */
		memcpy (&lgr, e->zblocs.tab.c + adr - sizeof(integer),
			sizeof(integer));
		for (j=0; j<lgr/sizeof(sexpr); j++)
		{
			memcpy (&y, e->zblocs.tab.c+adr+j*sizeof(sexpr),
				sizeof(sexpr));
			recup (y);
		}
	}
	else if ((TYPE(x) == TYPE_SYMBOLE) &&
		 (OCCUP(e->zblocs.tab.s[TAILLE_ZBLOCS-1-VAL(x)]) == 0))
	{
		SET_OCCUP (e->zblocs.tab.s[TAILLE_ZBLOCS-1-VAL(x)], 1);
		adr = ADR_BLOC(e->zblocs.tab.s[TAILLE_ZBLOCS-1-VAL(x)]);
		/* lgr = *(int *) */
		memcpy (&lgr, e->zblocs.tab.c + adr - sizeof(integer),
			sizeof(integer));
		for (j=0; j<2; j++)
		{
			memcpy (&y, e->zblocs.tab.c+adr+j*sizeof(sexpr),
				sizeof(sexpr));
			recup (y);
		}
	}
}

compacter ()
{
integer reg1, 	/* 1e ptr blocs, nouvelle adr */
	reg2,   /* 2e ptr blocs, ancienne adr */
	reg3,	/* numero bloc */
	reg4,	/* taille bloc */
	nreg1;	/* nouvelle valeur de reg1 */
integer x, i;

	reg1 = 0;
	reg2 = 0;
	while (reg2 < e->zblocs.ptrbloc)
	{
		nreg1 = reg1;
		memcpy (&reg3, e->zblocs.tab.c+reg2, sizeof(reg3));
		memcpy (&x, e->zblocs.tab.c+reg2+sizeof(integer), sizeof(x));
		reg4 = 2*sizeof(integer) + x;
		while (reg4 % sizeof(integer) != 0)
			reg4++;
		if (OCCUP(e->zblocs.tab.s[TAILLE_ZBLOCS-1-reg3]) == 1)
		{
			if (reg1 != reg2)
				for (i=0; i<reg4; i++)
					e->zblocs.tab.c[reg1+i] = e->zblocs.tab.c[reg2+i];
			nreg1 += reg4;
		}
		SET_ADR_BLOC (e->zblocs.tab.s[TAILLE_ZBLOCS-1-reg3], reg1+2*sizeof(integer));
		reg1 = nreg1;
		reg2 += reg4;
	}
	e->zblocs.ptrbloc = reg1;
	e->zblocs.ptrtab = 0;
}

aj_nsexpr (sexpr x)
{
	if (e->nsexprs.ptr >= TAILLE_TNSEXPRS-1)
		erreur_fatale ("aj_nsexpr: debordement du tableau des nouvelles sexprs");
	e->nsexprs.tab[e->nsexprs.ptr++] = x;
	tab_varsys[VARSYS_NSEXPRS] = e->nsexprs.ptr;
}

sexpr alloc_cons ()
{
sexpr x;
	if (debug)
	{
		sprintf (buf, "alloc_cons avant ptr=%d\n", e->zcons.ptr);
		aff_sortie (buf);
	}

	if (e->zcons.ptr >= TAILLE_ZCONS-1)
		gc();

	e->zcons.ptr++;

	if (debug)
	{
		sprintf (buf, "alloc_cons apres ptr=%d\n", e->zcons.ptr);
		aff_sortie (buf);
	}

	while ((e->zcons.ptr < TAILLE_ZCONS-1) &&
	       (OCCUP(e->zcons.tab[e->zcons.ptr].a) == 1))
		e->zcons.ptr++;

	if (OCCUP(e->zcons.tab[e->zcons.ptr].a) == 1)
		gc();
l_alloc_cons:
	SET_OCCUP (e->zcons.tab[e->zcons.ptr].a, 1);
	x = MK_SEXPR (1, 0, 1, 0, TYPE_CONS, e->zcons.ptr);
	e->zcons.tab[VAL(x)].a = nil;
	e->zcons.tab[VAL(x)].d = nil;
	aj_nsexpr (x);
	return x;
}

char *adr_bloc;

#define BLOC (e->zblocs.tab.s[TAILLE_ZBLOCS-1-e->zblocs.ptrtab])

sexpr alloc_bloc1 (integer n, integer p)
{
sexpr x;
		x = MK_SEXPR (1, 0, 1, 0, TYPE_CHAINE, e->zblocs.ptrtab);
		if (debug)
		{
			sprintf (buf, "alloc_bloc: x.val=%d\n", VAL(x));
			aff_sortie (buf);
		}

		adr_bloc = e->zblocs.tab.c + e->zblocs.ptrbloc + 2 * sizeof(integer);

		SET_OCCUP_BLOC (BLOC, 1);
		SET_SYMB_BLOC (BLOC, symb);
		SET_ADR_BLOC (BLOC, e->zblocs.ptrbloc + 2 * sizeof(integer));

		memcpy (e->zblocs.tab.c + e->zblocs.ptrbloc, e->zblocs.ptrtab,
			sizeof(integer));
		memcpy (e->zblocs.tab.c + e->zblocs.ptrbloc + sizeof(integer),
			&p, sizeof(integer));

		e->zblocs.ptrbloc += n + 2 * sizeof(integer);

		while (e->zblocs.ptrbloc % sizeof(integer))
			e->zblocs.ptrbloc++;

		aj_nsexpr (x);
		e->zblocs.ptrtab++;

		if (e->zblocs.ptrtab > e->zblocs.lim)
			e->zblocs.lim = e->zblocs.ptrtab;

		if (verif_debord_blocs());

		return x;
}


sexpr alloc_bloc (integer n)
{
sexpr x;
integer p;

	p = n;
	while (n % UNITE_ALLOC != 0)
		n++;

	if (verif_debord_blocs())
		goto l_alloc_bloc;
	while ((e->zblocs.ptrtab < e->zblocs.lim) &&
	       (OCCUP (BLOC) == 1))
		e->zblocs.ptrtab++;

	if (e->zblocs.ptrtab > e->zblocs.lim)
		e->zblocs.lim = e->zblocs.ptrtab;

	if (PLACE_ZBLOCS > n)
		return alloc_bloc1 (n, p);

	gc();
	if (verif_debord_blocs())
		goto debord_blocs;
l_alloc_bloc:
	while ((e->zblocs.ptrtab < e->zblocs.lim) && (OCCUP(BLOC) == 1))
		e->zblocs.ptrtab++;

	if (e->zblocs.ptrtab > e->zblocs.lim)
		e->zblocs.lim = e->zblocs.ptrtab;

	if (PLACE_ZBLOCS > n)
		return alloc_bloc1 (n, p);

debord_blocs:
	erreur_fatale ("Debordement de la zone des blocs");
}

sexpr alloc_vect (integer n)
{
sexpr x;
int i;

	x = alloc_bloc (n * sizeof(sexpr));
	for (i=0; i<n; i++)
	{
		memcpy (e->zblocs.tab.c
			+ ADR_BLOC(e->zblocs.tab.s[TAILLE_ZBLOCS-1-VAL(x)])
			+ i * sizeof(sexpr), &nil, sizeof(sexpr));
	}
	SET_TYPE (x, TYPE_VECTEUR);
	return x;
}

#define CAR(x) (e->zcons.tab[VAL(x)].a)
#define CDR(x) (e->zcons.tab[VAL(x)].d)

#define EQ(x,y) (eq(x,y))

#define RPLACA(x,y) { CAR(x)=y; SET_OCCUP(CAR(x),1); }
#define RPLACD(x,y) CDR(x)=y

int type (sexpr x) 	{ return TYPE(x); }
int verite (sexpr x)    { return VERITE(x); }
int marque (sexpr x)	{ return MARQUE(x); }
int val (sexpr x)	{ return VAL(x); }

sexpr non (sexpr x)
{
	SET_VERITE (x, VERITE(x)?0:1);
	return x;
}

#define ADRESSE_BLOC(x) (e->zblocs.tab.c \
	+ ADR_BLOC(e->zblocs.tab.s[TAILLE_ZBLOCS-1-VAL(x)]))

char *adresse_bloc (sexpr x)
{
char *debut;
integer adr;
	debut = e->zblocs.tab.c;
	adr = ADR_BLOC (e->zblocs.tab.s[TAILLE_ZBLOCS-1-VAL(x)]);
	return debut + adr;
}

integer taille_bloc (sexpr x)
{
integer t;
	memcpy (&t, ADRESSE_BLOC(x)-sizeof(integer), sizeof(integer));
	return t;
}

#define TAILLE_BLOC(x) (taille_bloc(x))
#define TAILLE_VECTEUR(x) (TAILLE_BLOC(x) / sizeof(sexpr))

#define EST_VRAI(x) VERITE(x)

#define CREF(x,n) (ADRESSE_BLOC(x)[n])
#define CSET(x,n,y) CREF(x,n)=y

char cref (sexpr x, int n)
{
	if ((TYPE(x) & 4) != 4)
	{
		erreur ("cref: l'argument n'est pas un bloc : ");
		ecrire_sexpr_err (x);
		aff_sorerr ("\n");
		return ((char) 0);
	}
	if (TAILLE_BLOC(x) <= n || n < 0)
	{
		erreur ("cref: indice hors limite");
		ecrire_sexpr_err (n);
		aff_sorerr ("\n");
		return ((char) 0);
	}
	return CREF(x,n);
}

#define VREF(x,n) (vref1(x,n))
/* #define VSET(x,n,y) VREF(x,n)=y */
#define VSET(x,n,y) (vset1(x,n,y))

sexpr vref1 (sexpr x, int n)
{
sexpr y;
	memcpy (&y, ADRESSE_BLOC(x) + n * sizeof(sexpr), sizeof(sexpr));
	return y;
}

vset1 (sexpr x, int n, sexpr y)
{
	memcpy (ADRESSE_BLOC(x) + n * sizeof(sexpr), &y, sizeof(sexpr));
}

sexpr vref (sexpr x, int n)
{
	if (TYPE(x) != TYPE_VECTEUR && TYPE(x) != TYPE_SYMBOLE)
	{
		erreur ("vref : l'argument n'est pas un vecteur : ");
		ecrire_sexpr_err (x);
		aff_sorerr ("\n");
		return nil;
	}
	if (TAILLE_VECTEUR(x) <= n || n < 0)
	{
		erreur ("vref : indice hors limites : ");
		sprintf (buf, "%d\n", n);
		aff_sorerr (buf);
		return nil;
	}
	return VREF(x,n);
}

#define DEF(x) VREF(x,0)
#define PROPS(x) VREF(x,1)
#define INSTR(x) instr1(x)

#define SET_DEF(x,y) VSET(x,0,y)
#define SET_PROPS(x,y) VSET(x,1,y)

integer instr1 (sexpr x)
{
integer i;
	memcpy (&i, ADRESSE_BLOC(x) + 2 * sizeof(sexpr), sizeof(i));
	return i;
}

#define RPLAC_DEF(x,y) VSET(x,0,y)
#define RPLAC_PROPS(x,y) VSET(x,1,y)
#define RPLAC_INSTR(x,y) rplac_instr1(x,y)

rplac_instr1 (sexpr x, integer y)
{
	memcpy (ADRESSE_BLOC(x) + 2 * sizeof(sexpr), &y, sizeof(integer));
}

sexpr car (sexpr x)
{
	if (TYPE(x) == TYPE_CONS)
		return CAR(x);
	if (TYPE(x) == TYPE_AS)
		return x;
	erreur ("car : l'argument n'est pas une liste : ");
	ecrire_sexpr_err (x);
	aff_sorerr ("\n");
	return nil;
}

sexpr cdr (sexpr x)
{
	if (TYPE(x) == TYPE_CONS)
		return CDR(x);
	if (TYPE(x) == TYPE_AS)
		return x;
	erreur ("car : l'argument n'est pas une liste : ");
	ecrire_sexpr_err (x);
	aff_sorerr ("\n");
	return nil;
}

sexpr cons (sexpr x, sexpr y)
{
sexpr r;
	r = alloc_cons ();
	RPLACA (r, x);
	RPLACD (r, y);
	return r;
}

int eq (sexpr x, sexpr y)
{
int masque;
	masque = ((tab_varsys[VARSYS_MASQUE_EQ] << 24) | 0xFFFFFF); /* a verifier */
	return ((conv_ent_sexpr(x) & masque) ==
		(conv_ent_sexpr(y) & masque));
}

integer ent_sexpr (sexpr x)
{
	return VAL(x);
}

int ents_sexpr (sexpr x)	 /* entier avec signe */
{
	if ((VAL(x) & 0x800000) == 0)
		return VAL(x);
	else
		return VAL(x) | 0xFF000000;
}

sexpr sexpr_ent (integer i)
{
sexpr x;
	x = vrai;
	SET_TYPE (x, TYPE_ENTIER);
	SET_VAL (x, i);
	return x;
}

int equal_string (char *s1, char *s2)
{
	return !strcmp (s1, s2);
}

copy_string (char *s1, char *s2)
{
	strcpy (s2, s1);
}

int length_string (char *s)
{
	return strlen (s);
}

int in_string (char *s, char c)
{
int i;
	for (i=0; s[i]; i++)
		if (s[i] == c)
			return 1;
	return 0;
}

#define N_CHAR 0x1000

char *read_string (char *t)
{
static char s[N_CHAR+1];
int i;
char c;
	for (i=0;
		i < N_CHAR && !in_string (t, c = getc_entree());
		s[i++] = c);
	if (i < N_CHAR)
		s[i++] = c;
	s[i] = 0;
	return s;
}

sexpr sexpr_chaine (char *s)
{
sexpr x;
int i, l;
	l = length_string(s) + 1;
	x = alloc_bloc(l);
	for (i=0; i<l; i++)
		CSET(x,i,s[i]);
	return x;
}

/* typedef double reel; */
#define reel double

#define REEL_SEXPR(x) reel_sexpr(x)

reel reel_sexpr (sexpr x)
{
reel r;
	memcpy (&r, ADRESSE_BLOC(x), sizeof(r));
	return r;
}

sexpr sexpr_reel (reel r)
{
sexpr x;
	x = alloc_bloc (sizeof(reel));
	SET_TYPE (x, TYPE_REEL);
	memcpy (ADRESSE_BLOC(x), &r, sizeof(r));
	return x;
}

#define DEPL_PNAME (2*sizeof(sexpr)+sizeof(integer))

sexpr creer_symbole (char *s)
{
sexpr x;
	symb = 1;
	x = alloc_bloc (DEPL_PNAME + length_string(s) + 1);
	symb = 0;
	RPLAC_DEF (x, nil);
	RPLAC_PROPS (x, nil);
	RPLAC_INSTR (x, 0);
	copy_string (s, adr_bloc+DEPL_PNAME);
	SET_TYPE (x, TYPE_SYMBOLE);
	SET_SYMB_BLOC(e->zblocs.tab.s[TAILLE_ZBLOCS-1-VAL(x)], 1);
	return x;
}

sexpr symbole (char *s)
{
sexpr x;
int i;
	for (i=0; i<e->zblocs.lim; i++)
	{
		if (OCCUP (e->zblocs.tab.s[TAILLE_ZBLOCS-1-i]) == 1
		 && SYMB_BLOC (e->zblocs.tab.s[TAILLE_ZBLOCS-1-i]) == 1
		 && equal_string (s, (e->zblocs.tab.c
			+ ADR_BLOC(e->zblocs.tab.s[TAILLE_ZBLOCS-1-i])
			+ DEPL_PNAME)))
		{
			x = MK_SEXPR (1, 0, 1, 0, TYPE_SYMBOLE, i);
			return x;
		}
	}
	return creer_symbole (s);
}

#define PNAME(x) (ADRESSE_BLOC(x) + DEPL_PNAME)

char *pname (sexpr x)
{
	if (TYPE(x) != TYPE_SYMBOLE)
	{
		erreur ("pname : l'argument n'est pas un symbole : ");
		ecrire_sexpr_err (x);
		aff_sorerr ("\n");
		return "";
	}
	return PNAME(x);
}

char lire_carac1 ()
{
char c;
	if (feof_entree())
	{
		if (tab_varsys[VARSYS_ERRFAT] & 1)
			erreur_fatale ("fin de fichier entree");
		else
			term ();
	}
	c = getc_entree ();
	if (tab_varsys[VARSYS_ECHO] & 1)
	{
		sprintf (buf, "%c", c);
		aff_sortie (buf);
	}
	if (tab_varsys[VARSYS_ECHO] & 2)
	{
		sprintf (buf, "%c", c);
		aff_sorerr (buf);
	}
	return c;
}

ecrire_carac1 (char c)
{
	sprintf (buf, "%c", c);
	aff_sortie (buf);
}

char tab_cs[0x100];

#define CS_ESPACE 	0x20
#define CS_DIESE 	0x21
#define CS_DCOM 	0x22
#define CS_FCOM 	0x23
#define CS_CARHEX 	0x24
#define CS_CAR0 	0x26
#define CS_CAR1 	0x27
#define CS_QUOTE 	0x28
#define CS_VAR 		0x29
#define CS_GET 		0x2A

#define CS_REPHEX 	0x18
#define CS_QUIT 	0x19
#define CS_CTRL		0x1A
#define CS_REV		0x1B
#define CS_ECHCOM	0x1C
#define CS_MARQUE	0x1D
#define CS_MOINS	0x1E
#define CS_NEG		0x1F

#define CS_OCT		8
#define CS_DEC		10
#define CS_HEX		16

#define CS_AS		0x30
#define CS_INSTR	0x31
#define CS_CARAC	0x32
#define CS_CHAINE	0x34
#define CS_GENTIER	0x35
#define CS_REEL		0x36
#define CS_COMPLEXE	0x37

#define CS_DLISTE	0x38
#define CS_FLISTE	0x39
#define CS_PP		0x3A
#define CS_DP		0x3B
#define CS_DVECT	0x3C
#define CS_FVECT	0x3D

#define CS_NIL		0x40
#define CS_VRAI		0x41
#define CS_U		0x42
#define CS_V		0x43

int niveau_com;

char carac_lu;

init_tab_cs ()
{
	tab_cs[CS_ESPACE] = ' ';
	tab_cs[CS_DIESE] = '#';
	tab_cs[CS_DCOM] = '{';
	tab_cs[CS_FCOM] = '}';
	tab_cs[CS_CARHEX] = '\\';
	tab_cs[CS_CAR0] = '/';
	tab_cs[CS_CAR1] = '\\';

	tab_cs[CS_REPHEX] = '=';
	tab_cs[CS_QUIT] = 'Q';
	tab_cs[CS_CTRL] = '^';
	tab_cs[CS_REV] = '@';
	tab_cs[CS_ECHCOM] = '|';
	tab_cs[CS_QUOTE] = '\'';
	tab_cs[CS_VAR] = '&';
	tab_cs[CS_GET] = '%';

	tab_cs[CS_MARQUE] = '*';
	tab_cs[CS_MOINS] = '-';
	tab_cs[CS_NEG] = '~';

	tab_cs[CS_OCT] = '8';
	tab_cs[CS_DEC] = 'A';
	tab_cs[CS_HEX] = 'G';

	tab_cs[CS_AS] = 'S';
	tab_cs[CS_INSTR] = 'I';
	tab_cs[CS_CARAC] = '/';
	tab_cs[CS_CHAINE] = '"';
	tab_cs[CS_GENTIER] = 'M';
	tab_cs[CS_REEL] = 'R';
	tab_cs[CS_COMPLEXE] = 'Z';

	tab_cs[CS_DLISTE] = '(';
	tab_cs[CS_FLISTE] = ')';
	tab_cs[CS_PP] = '.';
	tab_cs[CS_DP] = ':';
	tab_cs[CS_DVECT] = '[';
	tab_cs[CS_FVECT] = ']';

	tab_cs[CS_NIL] = 'N';
	tab_cs[CS_VRAI] = 'T';
	tab_cs[CS_U] = 'U';
	tab_cs[CS_V] = 'V';

}

init_es ()
{
	init_tab_cs ();
	niveau_com = 0;
	niveau_ecrire_sexpr = 0x100;

	entree = stdin;
	sortie = stdout;
	sorerr = stderr;

#ifdef BANNIERE
	aff_sorerr ("Bonjour !\n");
#endif
	carac_lu = ' ';
}

int est_chiffre_hex (char c)
{
	return ((c>='0' && c<='9') || (c>='A' && c<='F'));
}

int val_hex (char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	aff_sorerr ("val_hex : l'argument n'est pas un chiffre hexadecimal : ");
	sprintf (buf, "%c\n", c);
	aff_sorerr (buf);
	return 0;
}

char chiffre_hex (int i)
{
	if (i < 0 || i > 15)
	{
		erreur ("chiffre_hex : l'argument n'est pas un chiffre hexadecimal : ");
		sprintf (buf, "%x\n", i);
		return '0';
	}
	if (i <= 9)
		return '0' + i;
	return 'A' + i - 10;
}

char lire_carac2 ()
{
char c;
	c = lire_carac1 ();
	if (c != tab_cs[CS_CARHEX])
		return c;
	c = lire_carac1 ();
	if (c == tab_cs[CS_CTRL])
		return lire_carac2() - 64;
	if (c == tab_cs[CS_QUIT])
		term ();
	if (!est_chiffre_hex(c))
		return c;
	return 0x10 * val_hex(c) + val_hex (lire_carac1());
}

ecrire_carac2 (char c)
{
	if (c >= ' ' && c != 0x7F && c != tab_cs[CS_CARHEX])
		ecrire_carac1 (c);
	else
	{
		ecrire_carac1 (tab_cs[CS_CARHEX]);
		ecrire_carac1 (chiffre_hex ((c & 0xFF) >> 4));
		ecrire_carac1 (chiffre_hex (c & 0xF));
	}
}

ecrire_carac3 (char c)
{
	if (c == tab_cs[CS_DCOM] || c == tab_cs[CS_FCOM] || c == tab_cs[CS_ECHCOM]
		|| (est_blanc(c) && !(c==tab_cs[CS_ESPACE])))
		ecrire_carac2 (tab_cs[CS_ECHCOM]);
	ecrire_carac2 (c);
}

int est_carac_nul (char c)
{
	return c==0;
}

char lire_carac3 ()
{
char c;
	while (1)
	{
		c = lire_carac2 ();
		if (est_carac_nul (c))
			continue;
		if (c == tab_cs[CS_DCOM])
		{
			niveau_com++;
			if (niveau_com > 0)
				lire_com ();
			continue;
		}
		if (c == tab_cs[CS_FCOM])
		{
			niveau_com--;
			continue;
		}
		if (est_blanc (c))
			return tab_cs[CS_ESPACE];
		if (c == tab_cs[CS_ECHCOM])
		{
			c = lire_carac2 ();
			return c;
		}
		return c;
	}
}

lire_com ()
{
char c;
	while (1)
	{
		c = lire_carac2 ();
		if (c == tab_cs[CS_DCOM])
		{
			niveau_com++;
			continue;
		}
		if (c == tab_cs[CS_FCOM])
		{
			niveau_com--;
			if (niveau_com <= 0)
				return;
			continue;
		}
	}
}

int est_blanc (char c)
{
	return (c == tab_cs[CS_ESPACE] || c == 9 || c == 10 || c == 13);
}

char lire_carac ()
{
	return lire_carac3 ();
}

ecrire_carac (char c)
{
	ecrire_carac3 (c);
}

ecrire_hex (int i)
{
	sprintf (buf, "%X", i);
	aff_sortie (buf);
}

ecrire_dec (int i)
{
	if (i < 0)
		sprintf (buf, "#%d", i);
	else
		sprintf (buf, "%d", i);
	aff_sortie (buf);
}

ecrire_sexpr (sexpr x)
{
	ecrire_sexpr_niveau (x, niveau_ecrire_sexpr);
}

#define ECS(x) ecrire_carac (tab_cs[x])

ecrire_sexpr_niveau (sexpr x, int niveau)
{
char *adr;
	if (TYPE(x) == TYPE_AS
	 || TYPE(x) == TYPE_INSTR
	 || TYPE(x) == TYPE_ENTIER
	 || TYPE(x) == TYPE_CHAINE
	 || TYPE(x) == TYPE_REEL
	 || TYPE(x) == TYPE_SYMBOLE
	 || TYPE(x) == TYPE_VECTEUR
	 || TYPE(x) == TYPE_CONS)
	{
		if (!VERITE(x) && !EQ(x,as_u))
			/* ecrire_carac (tab_cs[CS_NEG]); */
			ECS (CS_NEG);
		if ((tab_varsys[VARSYS_ECRIRESEXPR] & ECRIRESEXPR_MARQUE)
			&& MARQUE(x))
		{
			ECS (CS_DIESE);
			ECS (CS_MARQUE);
		}
	}
	switch (TYPE(x))
	{
		case TYPE_AS:
			if (EQ(x,nil))
			{
				ECS (CS_DLISTE);
				ECS (CS_FLISTE);
			}
			else if (EQ(x,vrai))
			{
				ECS (CS_DIESE);
				ECS (CS_VRAI);
			}
			else if (EQ(x,as_u))
			{
				ECS (CS_DIESE);
				ECS (CS_U);
			}
			else if (EQ(x,as_v))
			{
				ECS (CS_DIESE);
				ECS (CS_V);
			}
			else
			{
				ECS (CS_DIESE);
				ECS (CS_AS);
				ecrire_hex (VAL(x));
			}
		case TYPE_INSTR:
			ECS (CS_DIESE);
			ECS (CS_INSTR);
			ecrire_carac (VAL(x) >> 16);
			ecrire_carac ((VAL(x) >> 8) & 0xFF);
			ecrire_carac (VAL(x) & 0xFF);
			break;
		case TYPE_ENTIER:
			if (!(tab_varsys[VARSYS_ECRIRESEXPR] & ECRIRESEXPR_DEC))
			{
				ECS (CS_DIESE);
				ECS (CS_HEX);
				ecrire_hex (VAL(x));
			}
			else
				ecrire_dec (ents_sexpr (x));
			break;
		case TYPE_CHAINE:
			ECS (CS_CHAINE);
			adr = ADRESSE_BLOC (x);
			if (!(tab_varsys[VARSYS_ECRIRESEXPR] & ECRIRESEXPR_CHAINE))
			while (*adr)
			{
				ecrire_carac (*adr);
				if (*adr == tab_cs[CS_CHAINE])
					ecrire_carac (*adr);
				adr++;
			}
			else
			{
			int i;
				for (i=0; i<TAILLE_BLOC(x)-1; i++)
				{
					ecrire_carac (*adr);
					if (*adr == tab_cs[CS_CHAINE])
						ecrire_carac (*adr);
					adr++;
				}
				if (*adr)
					ecrire_carac (*adr);
			}
			ECS (CS_CHAINE);
			break;
		case TYPE_GENTIER:
			ecrire_gentier (x);
			break;
		case TYPE_REEL:
			ecrire_reel (x);
			break;
		case TYPE_COMPLEXE:
			ecrire_complexe (x);
			break;
		case TYPE_SYMBOLE:
			adr = ADRESSE_BLOC(x) + DEPL_PNAME;
			while (*adr)
				ecrire_carac (*adr++);
			break;
		case TYPE_VECTEUR:
			ecrire_vecteur (x, niveau);
			break;
		case TYPE_CONS:
			ECS (CS_DLISTE);
			if (niveau <= 0)
			{
				ecrire_carac2 (tab_cs[CS_DCOM]);
				ecrire_carac2 (tab_cs[CS_FCOM]);
				ECS (CS_FLISTE);
				return;
			}
			if ((tab_varsys[VARSYS_ECRIRESEXPR] & ECRIRESEXPR_DPD)
				&& EQ (CDR(x), nil)
				&& TYPE(CAR(x)) == TYPE_CONS)
			{
				ECS (CS_DP);
				ecrire_liste (CAR(x), niveau-1);
			}
			else
			{
				ecrire_sexpr_niveau (CAR(x), niveau-1);
				ecrire_liste (CDR(x), niveau-1);
			}
			break;
		default:
			ecrire_rephex (x);
	}
	/* autoflush (sortie); */
}

ecrire_vecteur (sexpr x, int niveau)
{
int i;
int aaa;
sexpr xxx;
	ECS (CS_DVECT);
	if (niveau <= 0)
	{
		ecrire_carac2 (tab_cs[CS_DCOM]);
		ecrire_carac2 (tab_cs[CS_FCOM]);
		ECS (CS_FVECT);
		return;
	}
	for (i=0; i<TAILLE_VECTEUR(x)-1; i++)
	{
		ecrire_sexpr_niveau (vref(x,i), niveau-1);
		ECS (CS_ESPACE);
	}
#ifdef DEBUG
	aaa = TAILLE_VECTEUR(x);
	sprintf (buf, "ecrire_vecteur : taille=%d'n", aaa);
	aff_sortie (buf);
	brkpt ();
#endif
	if (TAILLE_VECTEUR(x) > 0)
		ecrire_sexpr_niveau (vref(x,TAILLE_VECTEUR(x)-1), niveau-1);
	ECS (CS_FVECT);
}

brkpt ()
{
}

ecrire_liste (sexpr x, int niveau)
{
	if (EQ(x,nil))
		ECS (CS_FLISTE);
	else if (TYPE(x) != TYPE_CONS || !VERITE(x)
		|| ((tab_varsys[VARSYS_ECRIRESEXPR] & ECRIRESEXPR_MARQUELISTE)
			&& MARQUE(x)))
	{
		ECS (CS_ESPACE);
		ECS (CS_PP);
		ECS (CS_ESPACE);
		if (niveau <= 0)
		{
			ecrire_carac2 (tab_cs[CS_DCOM]);
			ecrire_carac2 (tab_cs[CS_FCOM]);
			ECS (CS_FLISTE);
			return;
		}
		ecrire_sexpr_niveau (x, niveau-1);
		ECS (CS_FLISTE);
	}
	else if ((tab_varsys[VARSYS_ECRIRESEXPR] & ECRIRESEXPR_DPM)
		&& EQ(CDR(x),nil)
		&& TYPE(CAR(x)) == TYPE_CONS)
	{
		ECS (CS_ESPACE);
		ECS (CS_DP);
		if (niveau <= 0)
		{
			ECS (CS_ESPACE);
			ecrire_carac2 (tab_cs[CS_DCOM]);
			ecrire_carac2 (tab_cs[CS_FCOM]);
			return;
		}
		ecrire_liste (CAR(x), niveau-1);
	}
	else
	{
		ECS (CS_ESPACE);
		if (niveau <= 0)
		{
			ecrire_carac2 (tab_cs[CS_DCOM]);
			ecrire_carac2 (tab_cs[CS_FCOM]);
			return;
		}
		ecrire_sexpr_niveau (CAR(x), niveau-1);
		ecrire_liste (CDR(x), niveau-1);
	}
}

integer conv_ent_sexpr (sexpr x)
{
union { integer i; sexpr s; } u;
	u.s = x;
	return u.i;
}

sexpr conv_sexpr_ent (integer n)
{
union { integer i; sexpr s; } u;
	u.i = n;
	return u.s;
}

ecrire_rephex (sexpr x)
{
	ECS (CS_DIESE);
	ECS (CS_REPHEX);
	ecrire_hex (conv_ent_sexpr(x));
}

ecrire_gentier (sexpr x)
{
	ecrire_rephex (x);
}

ecrire_reel (sexpr x)
{
	ECS (CS_DIESE);
	ECS (CS_REEL);
	sprintf (buf, "%1.15e", REEL_SEXPR(x));
	aff_sortie (buf);
}

ecrire_complexe (sexpr x)
{
	ecrire_rephex (x);
}

carac_suiv ()
{
	carac_lu = lire_carac ();
}

lire_blancs ()
{
	while (est_blanc (carac_lu))
		carac_suiv();
}

#define TAILLE_BUFFER_ES 0x800

struct buffer_es
{
	int ptr;
	union
	{
		sexpr s[TAILLE_BUFFER_ES];
		char c[TAILLE_BUFFER_ES*sizeof(sexpr)];
	} tab;
};

struct buffer_es buffer;

tbc ()
{
	if (buffer.ptr >= TAILLE_BUFFER_ES * sizeof(sexpr) - 1)
		erreur_fatale ("aj_carac : debordement du buffer");
}

tbs ()
{
	if (buffer.ptr >= TAILLE_BUFFER_ES-1)
		erreur_fatale ("aj_sexpr : debordement du buffer");
}

#define aj_carac(__buffer,__c) { tbc(); __buffer.tab.c[__buffer.ptr++]=__c;}
#define aj_sexpr(__buffer,__x) { tbs(); __buffer.tab.s[__buffer.ptr++]=__x;}

int lire_hex ()
{
int r;
	fscanf (entree, "%X", &r);
	return r;
}

#define lire_sexpr() (lire_rec(1))
#define lire_liste() (lire_rec(2))

sexpr append (sexpr x, sexpr y)
{
	if (TYPE(x) != TYPE_CONS)
		return y;
	return cons (CAR(x), append (CDR(x), y));
}

sexpr reverse (sexpr l)
{
	if (TYPE(l) != TYPE_CONS)
		return l;
	return append (reverse (CDR(l)), cons (CAR(l), nil));
}

#define LCS(x) (carac_lu == tab_cs[x])

sexpr lire_rec (int n)
{
sexpr x;
char c;
int v;
int i;
	switch (n)
	{
		case 1:
			lire_blancs ();
			if (LCS (CS_DIESE))
			{
				carac_suiv ();
				x = vrai;
				if (LCS (CS_AS))
					SET_TYPE (x, TYPE_AS);
				else if (LCS (CS_INSTR))
				{
					carac_suiv();
					SET_TYPE (x, TYPE_INSTR);
					SET_VAL (x, 0x10000*carac_lu);
					carac_suiv();
					SET_VAL (x, VAL(x)+0x100*carac_lu);
					carac_suiv();
					SET_VAL (x, VAL(x)+carac_lu);
					carac_suiv();
					return x;
				}
				else if (LCS (CS_HEX))
					SET_TYPE (x, TYPE_ENTIER);
				else if (LCS (CS_NIL))
				{
					carac_suiv();
					return nil;
				}
				else if (LCS (CS_VRAI))
				{
					carac_suiv();
					return vrai;
				}
				else if (LCS (CS_U))
				{
					carac_suiv();
					return as_u;
				}
				else if (LCS (CS_V))
				{
					carac_suiv();
					return as_v;
				}
				else if (LCS (CS_MOINS))
				{
					carac_suiv();
					return sexpr_ent (-ent_sexpr(lire_sexpr()));
				}
				else if (LCS (CS_MARQUE))
				{
					carac_suiv();
					x = lire_sexpr();
					SET_MARQUE (x, 1);
					return x;
				}
				else if (LCS (CS_CAR0))
				{
					carac_suiv();
					x = sexpr_ent (carac_lu);
					carac_suiv();
					return x;
				}
				else if (LCS (CS_CAR1))
				{
					carac_suiv();
					x = sexpr_ent (carac_lu-128);
					carac_suiv();
					return x;
				}
				else if (LCS (CS_REPHEX))
				{
					fscanf (entree, "%X", &v);
					x = conv_sexpr_ent (v);
					if (tab_varsys[VARSYS_ECHO] & 1)
					{
						sprintf (buf, "%X", v);
						aff_sortie (buf);
					}
					if (tab_varsys[VARSYS_ECHO] & 2)
					{
						sprintf (buf, "%X", v);
						aff_sorerr (buf);
					}
					carac_suiv();
					return x;
				}
				else if (LCS (CS_REEL))
				{
				float f;
					fscanf (entree, "%f", &f);
					if (tab_varsys[VARSYS_ECHO] & 1)
					{
						sprintf (buf, "%f", f);
						aff_sortie (buf);
					}
					if (tab_varsys[VARSYS_ECHO] & 2)
					{
						sprintf (buf, "%f", f);
						aff_sorerr (buf);
					}
					carac_suiv();
					return sexpr_reel ((reel)x);
				}
				else
					SET_TYPE (x, TYPE_ENTIER);

				fscanf (entree, "%X", &v);
				SET_VAL (x, v);
				if (tab_varsys[VARSYS_ECHO] & 1)
				{
					sprintf (buf, "%X", v);
					aff_sortie (buf);
				}
				if (tab_varsys[VARSYS_ECHO] & 2)
				{
					sprintf (buf, "%X", v);
					aff_sorerr (buf);
				}
				carac_suiv();
				return x;
			}
			else if (LCS (CS_CHAINE))
			{
			struct buffer_es buffer;
				carac_suiv();
				buffer.ptr = 0;
				while (1)
				{
					if (LCS (CS_CHAINE))
					{
						carac_suiv();
						if (LCS (CS_CHAINE))
						{
							aj_carac (buffer, carac_lu);
							carac_suiv();
							continue;
						}
						else
						{
							aj_carac (buffer, 0);
							x = alloc_bloc (buffer.ptr);
							SET_TYPE (x, TYPE_CHAINE);
							copy_string (buffer.tab.c, adr_bloc);
							return x;
						}
					}
					else
					{
						aj_carac (buffer, carac_lu);
						carac_suiv();
					}
				}
			}
			else if (LCS (CS_DLISTE))
			{
				carac_suiv();
				x = lire_liste();
				lire_blancs();
				if (LCS (CS_FLISTE))
					carac_suiv();
				else
					erreur ("lire_sexpr : ) manquant");
				return x;
			}
			else if (LCS (CS_DP))
			{
				carac_suiv();
				return lire_liste();
			}
			else if (LCS (CS_REV))
			{
				carac_suiv();
				return reverse (lire_sexpr());
			}
			else if (LCS (CS_FLISTE) || LCS (CS_PP) || LCS (CS_FVECT))
			{
				erreur ("lire_sexpr : mauvais debut d'expression : ");
				sprintf (buf, "%c\n", carac_lu);
				aff_sorerr (buf);
				carac_suiv();
				return lire_sexpr();
			}
			else if (LCS (CS_DVECT))
			{
			struct buffer_es buffer;
				carac_suiv();
				buffer.ptr = 0;
				lire_blancs();
				while (carac_lu != tab_cs[CS_FVECT])
				{
					aj_sexpr (buffer, lire_sexpr());
					lire_blancs();
				}
				carac_suiv();
				x = alloc_bloc (buffer.ptr * sizeof(sexpr));
				SET_TYPE (x, TYPE_VECTEUR);
				for (i=0; i<buffer.ptr; i++)
					VSET (x, i, buffer.tab.s[i]);
				return x;
			}
			else if (LCS (CS_QUOTE))
			{
				carac_suiv();
				return cons (symbole("QUOTE"), cons (lire_sexpr(), nil));
			}
			else if (LCS (CS_VAR))
			{
				carac_suiv();
				return cons (symbole("VAR"), cons (lire_sexpr(), nil));
			}
			else if (LCS (CS_GET))
			{
				carac_suiv();
				return cons (symbole("GET"), cons (lire_sexpr(), nil));
			}
			else if (LCS (CS_NEG))
			{
				carac_suiv();
				return non (lire_sexpr());
			}
			else if (carac_lu >= '0' && carac_lu <= '9')
			{
			int i;
				i = 0;
				while (carac_lu >= '0' && carac_lu <= '9')
				{
					i = 10 * i + carac_lu - '0';
					carac_suiv();
				}
				return sexpr_ent(i);
			}
			else /* symbole */
			{
			struct buffer_es buffer;
				buffer.ptr = 0;
				aj_carac (buffer, carac_lu);
				carac_suiv();
				while (!LCS (CS_ESPACE)
					&& !LCS (CS_DIESE)
					&& !LCS (CS_CHAINE)
					&& !LCS (CS_DLISTE)
					&& !LCS (CS_FLISTE)
					&& !LCS (CS_PP)
					&& !LCS (CS_DP)
					&& !LCS (CS_DVECT)
					&& !LCS (CS_FVECT))
				{
					aj_carac (buffer, carac_lu);
					carac_suiv();
				}
				aj_carac (buffer, 0);
				return symbole (buffer.tab.c);
			}
			break;
		case 2:
			lire_blancs();
			if (LCS (CS_FLISTE) || LCS (CS_FVECT))
				return nil;
			else if (LCS (CS_PP))
			{
				carac_suiv();
				x = lire_sexpr ();
				lire_blancs ();
				return x;
			}
			/*
			else if (LCS (CS_DP))
			{
				carac_suiv();
				return cons (lire_liste(), nil);
			}
			*/
			else
			{
				x = lire_sexpr ();
				return cons (x, lire_liste());
			}
			break;
	}
}

sexpr x;

top_test ()
{
int i;
long x;
	i = sizeof(sexpr);
	printf ("sizeof(sexpr)=%d\n", i);

	printf ("UNITE_ALLOC = %d\n", UNITE_ALLOC);

	x = 1L << 30;
	printf ("%lX\n", x);

	for (x=lire_sexpr(); !EQ(x,nil); x=lire_sexpr())
	{
		if (verif_debord_blocs());

		printf ("ptr=%d\n", e->zcons.ptr);
		printf (" = \n");
		fflush (sortie);
		ecrire_sexpr (x);

		if (EQ (x, symbole("gc"))) gc();
		if (EQ (x, symbole("valid"))) valid();
		if (EQ (x, symbole("srac"))) e->racine = lire_sexpr();
		if (EQ (x, symbole("prac"))) ecrire_sexpr (e->racine);
		if (EQ (x, symbole("niveau"))) niveau_ecrire_sexpr = ent_sexpr (lire_sexpr());

		printf ("\n* \n");
		fflush (sortie);
		printf ("ptr=%d\n", e->zcons.ptr);

	}
fin: ;
}

erreur_fatale (char *s)
{
	sprintf (buf, "\n--- Erreur fatale : %s.\n", s);
	aff_sorerr (buf);
	exit (0);
}

erreur (char *s)
{
	sprintf (buf, "\n--- Erreur : %s.\n", s);
	aff_sorerr (buf);
}

ecrire_sexpr_err (sexpr x)
{
FILE *f;
	f = sortie;
	sortie = sorerr;
	ecrire_sexpr (x);
	sortie = f;
}

/* Interprete */

/*
	Representation de l'etat de l'interprete :

	racine = (((SYMBOLES . ext)
		   (ctxg ctxc ctx ctx ...)
		   (ctxg ctxc ctx ctx ...) ...) . ext)

	Representation d'un contexte :

	ctx = (strat prog piles acc env . ext)

	strat = (prio incr id hist coefs . ext)
	prog = (instr instr ...)
	piles = ((flags . haut) . bas)
	env = ((var val val ...) (var val val ...) ...)

*/

#define SYMBOLES(r) (CAR (CAR (CAR (r))))
#define SET_SYMBOLES(r,s) RPLACA (CAR(CAR(r)), s)

#define LCTXS(r) (CAR (CDR (CAR (r))))
#define SET_LCTXS(r,x) RPLACA (CDR(CAR(r)), x)

#define PREMIERE_INSTR(ctx) (CAR(CAR(CDR(ctx))))
#define CTX_AJOUTER_INSTR(ctx,x) (cons(CAR(ctx),cons(cons(x,CAR(CDR(ctx))),CDR(CDR(ctx)))))
#define CTX_ENLEVER_INSTR(ctx) (cons(CAR(ctx),cons(CDR(CAR(CDR(ctx))),CDR(CDR(ctx)))))

#define SOMMET(ctx) (CAR(CDR(CAR(CDR(CDR(ctx))))))
#define CTX_EMPILER(ctx,x) (cons(CAR(ctx),cons(CAR(CDR(ctx)), \
	cons(cons(CAR(CAR(CDR(CDR(ctx)))),cons(x,CDR(CAR(CDR(CDR(ctx)))))), \
	CDR(CDR(CDR(ctx)))))))
#define CTX_DEPILER(ctx) (cons(CAR(ctx),cons(CAR(CDR(ctx)), \
	cons(cons(CAR(CAR(CDR(CDR(ctx)))), CDR(CDR(CAR(CDR(CDR(ctx)))))), \
	CDR(CDR(CDR(ctx)))))))

sexpr premiere_instr (sexpr ctx)
{
	return PREMIERE_INSTR (ctx);
}

sexpr ctx_ajouter_instr (sexpr ctx, sexpr x)
{
	return CTX_AJOUTER_INSTR (ctx, x);
}

sexpr ctx_enlever_instr (sexpr ctx)
{
	return CTX_ENLEVER_INSTR (ctx);
}

sexpr sommet (sexpr ctx)
{
	return SOMMET (ctx);
}

sexpr ctx_empiler (sexpr ctx, sexpr x)
{
	return CTX_EMPILER (ctx, x);
}

sexpr ctx_depiler (sexpr ctx)
{
	return CTX_DEPILER (ctx);
}

#define AJOUTER_INSTR(ctx,x) { ctx = ctx_ajouter_instr (ctx, x); }
#define ENLEVER_INSTR(ctx,x) { x = premiere_instr (ctx); ctx = ctx_enlever_instr (ctx); }

#define EMPILER(ctx,x) { ctx = ctx_empiler (ctx, x); }
#define DEPILER(ctx,x) { x = sommet (ctx); ctx = ctx_depiler (ctx); }

#define STRAT(ctx) (CAR(ctx))
#define LX(ctx) (CAR(CDR(CDR(CDR(CDR(CDR(STRAT(ctx))))))))
#define MODIF_LX(ctx,x) (modif_lx(ctx,x))

#define PROG(ctx) (CAR(CDR(ctx)))

#define PILES(ctx) (CAR(CDR(CDR(ctx))))
#define FLAGS(ctx) (CAR(CAR(PILES(ctx))))
#define HAUT(ctx) (CDR(CAR(PILES(ctx))))
#define BAS(ctx) (CDR(PILES(ctx)))

#define ACC(ctx) (CAR(CDR(CDR(CDR(ctx)))))

#define ENV(ctx) (CAR(CDR(CDR(CDR(CDR(ctx))))))

#define MAJ_CTX(ctx,prog,bas) (cons(car(ctx),cons(prog,\
	cons(cons(cons(car(car(car(cdr(cdr(ctx))))),\
	cdr(car(car(cdr(cdr(ctx)))))),bas),\
	cdr(cdr(cdr(ctx)))))))

modif_lx (sexpr ctx, sexpr x)
{
	return cons(cons(CAR(STRAT(ctx)),
		cons(CAR(CDR(STRAT(ctx))),
		 cons(CAR(CDR(CDR(STRAT(ctx)))),
		  cons(CAR(CDR(CDR(CDR(STRAT(ctx))))),
		   cons(CAR(CDR(CDR(CDR(CDR(STRAT(ctx)))))),
		    cons(x,CDR(CDR(CDR(CDR(CDR(CDR(ctx)))))))
		     ))))),
		CDR(ctx));
}

sexpr X, Y;

#if 1
#define DEF_INSTR(nom,ni)
#else
#define DEF_INSTR(nom,ni) \
{ \
	X = symbole (nom); \
	SET_SYMBOLES (e->racine, cons (X, SYMBOLES(e->racine))); \
}
#endif

#define LIST1(x) (cons(x,nil))
#define LIST2(x,y) (cons(x,cons(y,nil)))
#define LIST3(x,y,z) (cons(x,cons(y,cons(z,nil))))

/* START -> _START */
#define START_PROG (cons(symbole("Y"), \
			cons(cons(symbole("READ"),cons(symbole("EXEC"),nil)), \
				nil)))

#define CTX_INIT (cons(cons(sexpr_ent(0),cons(sexpr_ent(0),cons(nil,cons(nil,nil)))), \
			cons(START_PROG,\
			 cons(cons(cons(nil,nil),nil), \
			  cons(nil, \
			   cons(vrai,nil) )))))

#define RACINE_INIT (cons(cons(cons(nil,nil), \
			       cons(cons(CTX_INIT,cons(CTX_INIT,nil)),nil) ), \
                          nil))   
       
#define MAJ_STRAT(ctx) (cons(cons(sexpr_ent(ent_sexpr(CAR(STRAT(ctx))) \
					   +ent_sexpr(CAR(CDR(STRAT(ctx)))) ),\
				  CDR(STRAT(ctx))), \
			     CDR(ctx)))

sexpr maj_strat (sexpr ctx)
{
	return MAJ_STRAT (ctx);
}

/*
#define UNBOUND ((tab_varsys[VARSYS_UNBOUND]&800000)?conv_sexpr_ent((tab_varsys[VARSYS_
#define UNDEF ((tab_varsys[VARSYS_UNDEF]&0x800000)?conv_sexpr_ent((tab_varsys[VARSYS_
*/

/* #define UNBOUND symbole("UNBOUND") */
#define UNBOUND nil

#define UNDEF symbole("_UNDEF")

/* #define QUOTE symbole("QUOTE") */
sexpr QUOTE;

int length (sexpr l)
{
	if (TYPE(l) != TYPE_CONS)
		return 0;
	return 1 + length (CDR(l));
}

sexpr catenate (sexpr x, sexpr y)
{
sexpr z;
int i, j;
	z = alloc_bloc (TAILLE_BLOC(x)+TAILLE_BLOC(y));
	for (i=0, j=0; (i<TAILLE_BLOC(x) && CREF(x,i)); i++, j++)
		CSET (z, j, CREF(x,i));
	for (i=0; i<TAILLE_BLOC(y) && CREF(y,i); i++, j++)
		CSET (z, j, CREF(y,i));
	if (j < TAILLE_BLOC(z))
		CSET (z, j, 0);
	return z;
}

sexpr last (sexpr x)
{
	if (TYPE(x) != TYPE_CONS)
		return nil;
	else if (TYPE(CDR(x)) != TYPE_CONS)
		return x;
	else
		return last (CDR(x));
}

sexpr memq (sexpr x, sexpr l)
{
sexpr r;
	if (TYPE(l) != TYPE_CONS)
		return nil;
	if (EQ(x,CAR(l)))
	{
		r = l;
		SET_VERITE (r, 1);
		return r;
	}
	return memq (x, CDR(l));
}

int equal (sexpr x, sexpr y)
{
	if (EQ(x,y))
		return 1;
	if (TYPE(x) != TYPE(y))
		return 0;
	switch (TYPE(x))
	{
		case TYPE_CHAINE:
			return equal_string (ADRESSE_BLOC(x), ADRESSE_BLOC(y));
		case TYPE_REEL:
			return REEL_SEXPR(x) == REEL_SEXPR(y);
		case TYPE_CONS:
			return equal(CAR(x),CAR(y)) && equal(CDR(x),CDR(y));
		case TYPE_VECTEUR:
		{
		int i;
			if (TAILLE_VECTEUR(x) != TAILLE_VECTEUR(y))
				return 0;
			for (i=0; i<TAILLE_VECTEUR(x); i++)
				if (!equal(VREF(x,i),VREF(y,i)))
					return 0;
			return 1;
		}
		default:
			return EQ(x,y);
	}
}

sexpr member (sexpr x, sexpr l)
{
sexpr r;
	if (TYPE(l) != TYPE_CONS)
		return nil;
	if (equal(x,CAR(l)))
	{
		r = l;
		SET_VERITE (r, 1);
		return r;
	}
	return member (x, CDR(l));
}

sexpr separer (int n, sexpr l)
{
sexpr r;
	if (n == 0)
		return cons (nil, l);
	r = separer (n-1, cdr(l));
	return cons (cons (car(l), car(r)), cdr(r));
}

sexpr remq (sexpr x, sexpr l)
{
	if (TYPE(l) != TYPE_CONS)
		return l;
	if (EQ(CAR(l),x))
		return CDR(l);
	return cons (CAR(l), remq (x, CDR(l)));
}

#undef remove
#define remove _lpia_remove

sexpr remove (sexpr x, sexpr l)
{
	if (TYPE(l) != TYPE_CONS)
		return l;
	if (equal(CAR(l),x))
		return CDR(l);
	return cons (CAR(l), remove (x, CDR(l)));
}

int inclq (sexpr l1, sexpr l2)
/* teste si l1 est inclus dans l2 avec EQ */
{
sexpr x;
	if (TYPE(l1) != TYPE_CONS)
		return 1;
	x = memq (CAR(l1), l2);
	return VERITE(x) && inclq (CDR(l1), l2);
}

int incl (sexpr l1, sexpr l2)
/* teste si l1 est inclus dans l2 avec equal */
{
sexpr x;
	if (TYPE(l1) != TYPE_CONS)
		return 1;
	x = member (CAR(l1), l2);
	return VERITE(x) && incl (CDR(l1), l2);
}


#define ENV_VIDE vrai

sexpr getvenv (sexpr env, sexpr var)
{
	if (TYPE(env) != TYPE_CONS)
		return UNBOUND;
	if (EQ(car(CAR(env)),var))
		return car(cdr(CAR(env)));
	return getvenv (CDR(env), var);
}

sexpr setvenv (sexpr env, sexpr var, sexpr val)
{
	if (TYPE(env) != TYPE_CONS)
		return cons(cons(var,cons(val,nil)),env/*ENV_VIDE*/);
	if (EQ(car(CAR(env)),var))
		return cons(cons(var,cons(val,
				cdr(cdr(CAR(env))))),
			CDR(env));
	return cons(CAR(env),setvenv(CDR(env),var,val));
}

sexpr addvenv (sexpr env, sexpr var, sexpr val)
{
	if (TYPE(env) != TYPE_CONS)
		return cons(cons(var,cons(val,
				cdr(CAR(env)))),
			CDR(env));
	return cons(CAR(env),addvenv(CDR(env),var,val));
}

sexpr remvenv (sexpr env, sexpr var)
{
sexpr x;
	if (TYPE(env) != TYPE_CONS)
		return env;
	if (EQ(car(CAR(env)),var))
	{
		x = cdr(cdr(CAR(env)));
		if (!tab_varsys[VARSYS_GARDER_VARS] && TYPE(x)!=TYPE_CONS)
			return CDR(env);
		else
			return cons(cons(var,x),CDR(env));
	}
	return cons(CAR(env),remvenv(CDR(env),var));
}

sexpr getvsenv (sexpr env, sexpr vars)
{
	if (TYPE(vars) != TYPE_CONS)
		return getvenv(env,vars);
	if (EQ(CAR(vars),QUOTE) && TYPE(CDR(vars))==TYPE_CONS)
		return CAR(CDR(vars));
	return cons(getvsenv(env,CAR(vars)),
		    getvsenv(env,CDR(vars)));
}

sexpr bindvenv (sexpr env, sexpr vars, sexpr vals)
{
sexpr a, b;
	if (!VERITE(vars))
		return env;
	if (TYPE(vars) != TYPE_CONS)
		return addvenv(env,vars,vals);
	a = (TYPE(vals)==TYPE_CONS) ? car(vals) : vals;
	b = (TYPE(vals)==TYPE_CONS) ? cdr(vals) : vals;
	return bindvenv(bindvenv(env,car(vars),a),cdr(vars),b);
}

sexpr unbindvenv (sexpr env, sexpr vars)
{
sexpr a, b;
	if (!VERITE(vars))
		return env;
	if (TYPE(vars) != TYPE_CONS)
		return remvenv(env,vars);
	return unbindvenv(unbindvenv(env,car(vars)),cdr(vars));
}

sexpr VAR, NOM_VAN, VAN;

#define CREER_VAR(x) (cons(VAR,cons(x,nil)))
#define NOM_VAR(x) (CAR(CDR(x)))
#define EST_VAR(x) (TYPE(x)==TYPE_CONS && EQ(VAR,CAR(x)))
#define EST_VAN(x) (EST_VAR(x) && EQ(nil,NOM_VAR(x)))

char carac_gensym1 = '0';
char carac_gensym2 = '0';

sexpr gensym ()
{
char s[7];
	s[0] = 'G';
	s[1] = 'E';
	s[2] = 'N';
	s[3] = '-';
	if (carac_gensym2 == '9')
		carac_gensym2 = 'A';
	else if (carac_gensym2 == 'Z')
		carac_gensym2 = 'a';
	else if (carac_gensym2 == 'z')
	{
		carac_gensym2 = '0';
		if (carac_gensym1 == '9')
			carac_gensym1 = 'A';
		else if (carac_gensym1 == 'Z')
			carac_gensym1 = 'a';
		else if (carac_gensym1 == 'z')
			carac_gensym1 = '0';
		else
			carac_gensym1++;
	}
	else
		carac_gensym2++;
	s[4] = carac_gensym1;
	s[5] = carac_gensym2;
	s[6] = 0;
	return creer_symbole(s);
}

int boundvenv (sexpr env, sexpr var)
{
	if (TYPE(env) != TYPE_CONS)
		return 0;
	if (EQ(car(CAR(env)),var)
		&& TYPE(CDR(CAR(env)))==TYPE_CONS
		&& !EQ(CAR(CDR(CAR(env))),UNDEF))
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
	if (TYPE(l) != TYPE_CONS)
		return l;
	if (verite(memq(CAR(l),CDR(l))))
		return ensemble(CDR(l));
	return cons(CAR(l),ensemble(CDR(l)));
}

sexpr variables1 (sexpr x)
{
	if (EST_VAR(x))
		return cons(NOM_VAR(x),nil);
	if (TYPE(x) != TYPE_CONS)
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
	if (TYPE(z) != TYPE_CONS)
		return z;
	return cons (subst(x,y,CAR(z)), subst(x,y,CDR(z)));
}

sexpr renommer1 (sexpr x, sexpr vars)
{
	if (TYPE(vars) != TYPE_CONS)
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
	if (TYPE(x) != TYPE_CONS)
		return x;
	return cons (construire(env,CAR(x)),
		     construire(env,CDR(x)));
}

#define LIER(env,var,val) ((tab_varsys[VARSYS_ADDV]?addvenv:setvenv)(env,var,val))

sexpr unifier ();

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
	return unifier(unifier1(env,CAR(x),CAR(y)),CDR(x),CDR(y));
}

sexpr unifier (sexpr env, sexpr x, sexpr y)
{
	if (!VERITE(env))
		return env;
	return unifier1(env,construire(env,x),construire(env,y));
}

integer partie_entiere (reel x)
{
	if (x >= 0)
		return (integer)x;
	if (x == (reel)(integer)x)
		return (integer)x;
	return (integer)x - 1;
}

int polynome (sexpr l, integer n)
{
	if (TYPE(l) != TYPE_CONS)
		return 0;
	return ents_sexpr(CAR(l)) + n * polynome(CDR(l),n);
}

reel rpolynome (sexpr l, reel x)
{
	if (TYPE(l) != TYPE_CONS)
		return 0.0;
	return REEL_SEXPR(CAR(l)) + x * rpolynome(CDR(l),x);
}

int nnpower (integer n, integer p)
{
integer q;
	if (p < 0)
		return 1/nnpower(n,-p);
	if (p == 0)
		return 1;
	q = nnpower(n,p/2);
	if ((p&1)==0)
		return q*q;
	return n*q*q;
}

reel rnpower (reel x, integer p)
{
reel y;
	if (p < 0)
		return 1/rnpower(x,-p);
	if (p == 0)
		return 1.0;
	y = rnpower(x,p/2);
	if ((p&1)==0)
		return y*y;
	return x*y*y;
}

integer nintlog2 (integer n)
{
	if (n < 0)
		n = -n;
	if (n < 2)
		return 0;
	return 1 + nintlog2(n/2);
}

integer rintlog2 (reel x)
{
	if (x < 0.0)
		x = -x;
	if (x < 1.0)
		return -1+rintlog2(2.0*x);
	if (x < 2.0)
		return 0;
	return 1+rintlog2(x/2.0);
}

#define NIVEAU_IMP_DEBUG (tab_varsys[VARSYS_NIVEAU_IMP_DEBUG])

#ifdef BIG_ENDIAN
#define DEPL 1
#else
#define DEPL 0
#endif

#define DEBUG_CONT 	((char *)(tab_varsys+VARSYS_DEBUG_CONT)+DEPL)
#define DEBUG_IMP       ((char *)(tab_varsys+VARSYS_DEBUG_IMP)+DEPL)
#define DEBUG_EXEC	((char *)(tab_varsys+VARSYS_DEBUG_EXEC)+DEPL)
#define DEBUG_CHMD 	((char *)(tab_varsys+VARSYS_DEBUG_CHMD)+DEPL)

#define sordbg sorerr

aff_sordbg (char *s)
{
	fprintf (sordbg, "%s", s);
}

ecrire_ctx (sexpr ctx)
{
FILE *f;
	f = sortie;
	sortie = sordbg;

	/* fprintf (sordbg, "Nombre de processus : %d \n", length(LCTXS(e->racine))-1); */
	sprintf (buf, "Nombre de processus : %d \n", length(LCTXS(e->racine))-1);
	aff_sordbg (buf);
	aff_sordbg ("Processus courant : \n");
	aff_sordbg ("------------------- \n");

	aff_sordbg (" strategie        : ");
	ecrire_sexpr_niveau (STRAT(ctx), NIVEAU_IMP_DEBUG);
	aff_sordbg ("\n");

	aff_sordbg (" programme        : ");
	ecrire_sexpr_niveau (PROG(ctx), NIVEAU_IMP_DEBUG);
	aff_sordbg ("\n");

	aff_sordbg (" flags            : ");
	ecrire_sexpr_niveau (FLAGS(ctx), NIVEAU_IMP_DEBUG);
	aff_sordbg ("\n");

	aff_sordbg (" pile superieure  : ");
	ecrire_sexpr_niveau (HAUT(ctx), NIVEAU_IMP_DEBUG);
	aff_sordbg ("\n");

	aff_sordbg (" pile inferieure  : ");
	ecrire_sexpr_niveau (BAS(ctx), NIVEAU_IMP_DEBUG);
	aff_sordbg ("\n");

	aff_sordbg (" accumulateur     : ");
	ecrire_sexpr_niveau (ACC(ctx), NIVEAU_IMP_DEBUG);
	aff_sordbg ("\n");

	aff_sordbg (" environnement     : ");
	ecrire_sexpr_niveau (ENV(ctx), NIVEAU_IMP_DEBUG);
	aff_sordbg ("\n");

	aff_sordbg (" extension         : ");
	ecrire_sexpr_niveau (CDR(CDR(CDR(CDR(CDR(ctx))))), NIVEAU_IMP_DEBUG);
	aff_sordbg ("\n");

	sortie = f;
}

ecrire_menu_debug ()
{
	aff_sordbg ("Mode pas a pas \n");

	sprintf (buf, "  %s : continuer \n", DEBUG_CONT);
	aff_sordbg (buf);

	sprintf (buf, "  %s : imprimer le contexte \n", DEBUG_IMP);
	aff_sordbg (buf);

	sprintf (buf, "  %s <instr> : executer l'instruction <instr> \n",
		DEBUG_EXEC);
	aff_sordbg (buf);

	sprintf (buf, "  %s <n> : changer le mode debug (0: normal, 1: trace) \n",
		DEBUG_CHMD);
	aff_sordbg (buf);
}

sexpr top_debug (sexpr ctx)
{
char rep[20];
int d;
FILE *f;

	if (tab_varsys[VARSYS_DEBUG] == 0)
		return ctx;
	if (TYPE(CAR(PROG(ctx))) == TYPE_CONS)
		return ctx;
	if (tab_varsys[VARSYS_DEBUG] == 1)
	{
		fprintf (sordbg, "\n");
		ecrire_ctx (ctx);
		return ctx;
	}
debug_imp:
	ecrire_menu_debug();
	aff_sordbg ("\n");
	ecrire_ctx (ctx);
	aff_sordbg ("\n");

	f = entree;
	entree = stdin;
debug_boucle:
	aff_sordbg ("step> ");
	fscanf (entree, "%s", rep);
	if (equal_string (rep, DEBUG_CONT))
	{
		entree = f;
		return ctx;
	}
	if (equal_string (rep, DEBUG_IMP))
		goto debug_imp;
	if (equal_string (rep, DEBUG_EXEC))
	{
		AJOUTER_INSTR (ctx, lire_sexpr());
		goto debug_boucle;
	}
	if (equal_string (rep, DEBUG_CHMD))
	{
		fscanf (entree, "%d", &d);
		tab_varsys[VARSYS_DEBUG] = d;
		goto debug_boucle;
	}
	erreur ("debug : commande non definie \n");
	goto debug_boucle;
}

inserer_ctx (sexpr ctx, sexpr ctxs)
{
	if (TYPE(CDR(ctxs)) != TYPE_CONS)
	{
		RPLACD (ctxs, cons(ctx,CDR(ctxs)/*nil*/));
		return;
	}
	if (ents_sexpr(CAR(STRAT(ctx))) < ents_sexpr(CAR(STRAT(CAR(CDR(ctxs))))))
	{
		inserer_ctx (ctx, CDR(ctxs));
		return;
	}
	RPLACD (ctxs, cons(ctx,CDR(ctxs)));
}

inserer_lctxs (sexpr l1, sexpr l2)
{
	if (TYPE(l1) != TYPE_CONS)
		return;
	inserer_lctxs (CDR(l1), l2);
	inserer_ctx (CAR(l1), l2);
}

sexpr ins_ctx (sexpr ctx, sexpr ctxs)
{
	if (TYPE(ctxs) != TYPE_CONS)
		return cons(ctx,ctxs);
	if (ents_sexpr(CAR(STRAT(ctx))) < ents_sexpr(CAR(STRAT(CAR(ctxs)))))
		return cons (CAR(ctxs), ins_ctx(ctx,CDR(ctxs)));
	return cons(ctx,ctxs);
}

sexpr ins_lctxs (sexpr l1, sexpr l2)
{
	if (TYPE(l1) != TYPE_CONS)
		return l2;
	return ins_ctx (CAR(l1), ins_lctxs(CDR(l1),l2));
}

sexpr instr_I (sexpr ctx)
{
	return LIST1(ctx);
}

sexpr instr_PRINT (sexpr ctx)
{
sexpr x;
	DEPILER (ctx, x);
	ecrire_sexpr (x);
	return LIST1(ctx);
}

#ifdef BIG_ENDIAN
#define NINSTR(a,b,c) ((((long)(a))<<16) | (((long)(b)) << 8) | ((long)(c)))
#else
/*#define NINSTR(a,b,c) (((long)(a)) | (((long)(b))<<8) | (((long)(c))<<16))*/
#define NINSTR(a,b,c) (((long)(a)) | (((long)(b))*0x100L) | (((long)(c))*0x10000L))
#endif

sexpr sexpr_ninstr (integer i)
{
sexpr x;
	SET_OCCUP (x, 1);
	SET_VERITE (x, 1);
	SET_MARQUE (x, 0);
	SET_TYPE (x, TYPE_INSTR);
	SET_VAL (x, i);
	return x;
}

/*
#define INSTR_USER 	NINSTR('U','S','R')

#define INSTR_I         NINSTR('C','B','I')
#define INSTR_K		NINSTR('C','B','K')
#define INSTR_S		NINSTR('C','B','S')
#define INSTR_B		NINSTR('C','B','B')
*/

/* #include "c:\\lpia\\instrint.h" */
#define INSTR_USER	NINSTR('U','S','R')
#define INSTR_I	NINSTR('C','B','I')
#define INSTR_K	NINSTR('C','B','K')
#define INSTR_S	NINSTR('C','B','S')
#define INSTR_B	NINSTR('C','B','B')
#define INSTR_C	NINSTR('C','B','C')
#define INSTR_W	NINSTR('C','B','W')
#define INSTR_APPLYTO	NINSTR('A','P','T')
#define INSTR_P	NINSTR('C','B','P')
#define INSTR_J	NINSTR('C','B','J')
#define INSTR_Y	NINSTR('C','B','Y')
#define INSTR_QUOTE	NINSTR('Q','U','O')
#define INSTR_DEP	NINSTR('D','E','P')
#define INSTR_REP	NINSTR('R','E','P')
#define INSTR_ECH	NINSTR('E','C','H')
#define INSTR_DES	NINSTR('D','E','S')
#define INSTR_MON	NINSTR('M','O','N')
#define INSTR_NOT	NINSTR('N','O','T')
#define INSTR_THEN	NINSTR('T','H','N')
#define INSTR_EXEC	NINSTR('E','X','E')
#define INSTR_GETPROG	NINSTR('G','P','G')
#define INSTR_JUMP	NINSTR('J','M','P')
#define INSTR_CALL	NINSTR('C','A','L')
#define INSTR_ALT2	NINSTR('A','L','T')
#define INSTR_END	NINSTR('E','N','D')
#define INSTR_MKCNL	NINSTR('M','C','N')
#define INSTR_WAITSENDP	NINSTR('W','S','P')
#define INSTR_WAITRECEIVEP	NINSTR('W','R','P')
#define INSTR_SEND	NINSTR('S','N','D')
#define INSTR_RECEIVE	NINSTR('R','C','V')
#define INSTR_EVOL	NINSTR('E','V','O')
#define INSTR_EVOL1	NINSTR('E','V','1')
#define INSTR_INSLCTXS	NINSTR('I','L','C')
#define INSTR_GETCTX	NINSTR('G','C','T')
#define INSTR_SETCTX	NINSTR('S','C','T')
#define INSTR_SETCTXS	NINSTR('S','C','S')
#define INSTR_GETROOT	NINSTR('G','R','T')
#define INSTR_SCHEDULE	NINSTR('/','/','/')
#define INSTR_CAR	NINSTR('C','A','R')
#define INSTR_CDR	NINSTR('C','D','R')
#define INSTR_CONS	NINSTR('C','N','S')
#define INSTR_EQ	NINSTR('E','Q','U')
#define INSTR_RPLACA	NINSTR('R','P','A')
#define INSTR_RPLACD	NINSTR('R','P','D')
#define INSTR_GETTYPE	NINSTR('G','T','Y')
#define INSTR_SETTYPE	NINSTR('S','T','Y')
#define INSTR_GETMARK	NINSTR('G','M','K')
#define INSTR_SETMARK	NINSTR('S','M','K')
#define INSTR_GETVAL	NINSTR('G','V','L')
#define INSTR_SETVAL	NINSTR('S','V','L')
#define INSTR_SYMBOL	NINSTR('S','Y','M')
#define INSTR_MAKESYMBOL	NINSTR('M','S','Y')
#define INSTR_GETDEF	NINSTR('G','D','F')
#define INSTR_SETDEF	NINSTR('S','D','F')
#define INSTR_GETPROPS	NINSTR('G','P','P')
#define INSTR_SETPROPS	NINSTR('S','P','P')
#define INSTR_PLUS	NINSTR('P','L','U')
#define INSTR_MOINS	NINSTR('M','N','S')
#define INSTR_FOIS	NINSTR('F','O','I')
#define INSTR_DIV	NINSTR('D','I','V')
#define INSTR_MOD	NINSTR('M','O','D')
#define INSTR_ETL	NINSTR('E','T','L')
#define INSTR_OUL	NINSTR('O','U','L')
#define INSTR_OXL	NINSTR('O','X','L')
#define INSTR_ZEROP	NINSTR('Z','R','P')
#define INSTR_POSP	NINSTR('P','O','S')
#define INSTR_MAKEVECTOR	NINSTR('M','K','V')
#define INSTR_VREF	NINSTR('V','R','E')
#define INSTR_VSET	NINSTR('V','S','E')
#define INSTR_MAKESTRING	NINSTR('M','K','S')
#define INSTR_CREF	NINSTR('C','R','E')
#define INSTR_CSET	NINSTR('C','S','E')
#define INSTR_SIZE	NINSTR('S','I','Z')
#define INSTR_SETPRIO	NINSTR('S','P','R')
#define INSTR_INCRPRIO	NINSTR('I','P','R')
#define INSTR_SETINCR	NINSTR('S','I','N')
#define INSTR_GETPRIO	NINSTR('G','P','R')
#define INSTR_GETINCR	NINSTR('G','I','N')
#define INSTR_GETSTRAT	NINSTR('G','S','T')
#define INSTR_SETSTRAT	NINSTR('S','S','T')
#define INSTR_GETFLAGS	NINSTR('G','F','L')
#define INSTR_SETFLAGS	NINSTR('S','F','L')
#define INSTR_GETHAUT	NINSTR('G','H','T')
#define INSTR_SETHAUT	NINSTR('S','H','T')
#define INSTR_GETSTACK	NINSTR('G','S','K')
#define INSTR_SETSTACK	NINSTR('S','S','K')
#define INSTR_GETACC	NINSTR('G','A','C')
#define INSTR_SETACC	NINSTR('S','A','C')
#define INSTR_GETENV	NINSTR('G','E','V')
#define INSTR_SETENV	NINSTR('S','E','V')
#define INSTR_GETH	NINSTR('G','T','H')
#define INSTR_MONDEP	NINSTR('M','D','P')
#define INSTR_GETLX	NINSTR('G','L','X')
#define INSTR_SETLX	NINSTR('S','L','X')
#define INSTR_BSI	NINSTR('B','S','I')
#define INSTR_ESI	NINSTR('E','S','I')
#define INSTR_PREMINSTR	NINSTR('P','I','N')
#define INSTR_AJINSTR	NINSTR('A','I','N')
#define INSTR_ENLINSTR	NINSTR('E','I','N')
#define INSTR_SOMPIL	NINSTR('S','P','L')
#define INSTR_EMPIL	NINSTR('E','P','L')
#define INSTR_DEPIL	NINSTR('D','P','L')
#define INSTR_CATENATE	NINSTR('C','A','T')
#define INSTR_READ	NINSTR('R','E','A')
#define INSTR_READSTRING	NINSTR('R','S','T')
#define INSTR_PRINT	NINSTR('P','R','T')
#define INSTR_PRIN	NINSTR('P','R','N')
#define INSTR_PRINTSTRING	NINSTR('P','S','T')
#define INSTR_PRINTF	NINSTR('P','R','F')
#define INSTR_PRINTCTX	NINSTR('P','C','T')
#define INSTR_TYI	NINSTR('T','Y','I')
#define INSTR_TYO	NINSTR('T','Y','O')
#define INSTR_TYE	NINSTR('T','Y','E')
#define INSTR_READCN	NINSTR('R','D','C')
#define INSTR_PRINCN	NINSTR('P','R','C')
#define INSTR_LIRECARAC	NINSTR('L','R','C')
#define INSTR_ECRIRECARAC	NINSTR('E','C','C')
#define INSTR_GETPREVCHAR	NINSTR('G','P','C')
#define INSTR_SETPREVCHAR	NINSTR('S','P','C')
#define INSTR_NEXTCHAR	NINSTR('N','X','C')
#define INSTR_OPEN	NINSTR('O','P','N')
#define INSTR_CLOSE	NINSTR('C','L','O')
#define INSTR_FLUSH	NINSTR('F','L','U')
#define INSTR_EOF	NINSTR('E','O','F')
#define INSTR_GETINCHAN	NINSTR('G','I','C')
#define INSTR_SETINCHAN	NINSTR('S','I','C')
#define INSTR_GETOUTCHAN	NINSTR('G','O','C')
#define INSTR_SETOUTCHAN	NINSTR('S','O','C')
#define INSTR_GETERRCHAN	NINSTR('G','E','C')
#define INSTR_SETERRCHAN	NINSTR('S','E','C')
#define INSTR_STDIN	NINSTR('S','T','I')
#define INSTR_STDOUT	NINSTR('S','T','O')
#define INSTR_STDERR	NINSTR('S','T','E')
#define INSTR_GETPRINTLEVEL	NINSTR('G','L','V')
#define INSTR_SETPRINTLEVEL	NINSTR('S','L','V')
#define INSTR_GETSC	NINSTR('G','S','C')
#define INSTR_SETSC	NINSTR('S','S','C')
#define INSTR_TELLIN	NINSTR('T','L','I')
#define INSTR_TELLOUT	NINSTR('T','L','O')
#define INSTR_SEEKIN	NINSTR('S','K','I')
#define INSTR_SEEKOUT	NINSTR('S','K','O')
#define INSTR_GETAUTOFLUSH	NINSTR('G','A','F')
#define INSTR_SETAUTOFLUSH	NINSTR('S','A','F')
#define INSTR_LOAD	NINSTR('L','O','D')
#define INSTR_SYSFORK	NINSTR('F','R','K')
#define INSTR_SLEEP	NINSTR('S','L','P')
#define INSTR_COMLINE	NINSTR('C','L','N')
#define INSTR_GETSYSENV	NINSTR('G','S','E')
#define INSTR_GC	NINSTR('I','G','C')
#define INSTR_DECLSYM	NINSTR('D','S','Y')
#define INSTR_STEP	NINSTR('S','T','P')
#define INSTR_GETSYSVAR	NINSTR('G','S','V')
#define INSTR_SETSYSVAR	NINSTR('S','S','V')
#define INSTR_QUIT	NINSTR('Q','I','T')
#define INSTR_EQUAL	NINSTR('E','Q','L')
#define INSTR_LENGTH	NINSTR('L','N','G')
#define INSTR_MEMQ	NINSTR('M','M','Q')
#define INSTR_MEMBER	NINSTR('M','M','B')
#define INSTR_APPEND	NINSTR('A','P','N')
#define INSTR_LAST	NINSTR('L','A','S')
#define INSTR_REVERSE	NINSTR('R','E','V')
#define INSTR_REMQ	NINSTR('R','M','Q')
#define INSTR_REMOVE	NINSTR('R','M','V')
#define INSTR_GETVENV	NINSTR('G','V','E')
#define INSTR_SETVENV	NINSTR('S','V','E')
#define INSTR_ADDVEND	NINSTR('A','V','E')
#define INSTR_REMVENV	NINSTR('R','V','E')
#define INSTR_PREFIX	NINSTR('P','F','X')
#define INSTR_RPREFIX	NINSTR('R','P','F')
#define INSTR_GETVSENV	NINSTR('G','V','S')
#define INSTR_BINDVENV	NINSTR('B','V','E')
#define INSTR_UNBINDVENV	NINSTR('U','V','E')
#define INSTR_GENSYM	NINSTR('G','S','Y')
#define INSTR_BOUNDVENV	NINSTR('B','O','V')
#define INSTR_GETRECVENV	NINSTR('G','R','V')
#define INSTR_VARIABLES	NINSTR('V','R','S')
#define INSTR_RENAME	NINSTR('R','N','M')
#define INSTR_CONSTR	NINSTR('C','S','T')
#define INSTR_UNIF	NINSTR('U','N','I')
#define INSTR_SUBST	NINSTR('S','U','B')
#define INSTR_ENSEMBLE	NINSTR('E','N','S')
#define INSTR_SEPARER	NINSTR('S','E','P')
#define INSTR_NCONSPTHEN	NINSTR('N','C','T')
#define INSTR_ECH3	NINSTR('E','C','3')
#define INSTR_LIST1	NINSTR('L','S','1')
#define INSTR_LIST2	NINSTR('L','S','2')
#define INSTR_LIST3	NINSTR('L','S','3')
#define INSTR_UNCONS	NINSTR('U','C','S')
#define INSTR_INCLQ	NINSTR('I','C','Q')
#define INSTR_INCL	NINSTR('I','N','C')
#define INSTR_INSTR	NINSTR('I','N','S')
#define INSTR_ARG	NINSTR('A','R','G')
#define INSTR_GETVSQ	NINSTR('G','V','Q')
#define INSTR_SETVQ	NINSTR('S','V','Q')
#define INSTR_BINDVQ	NINSTR('B','V','Q')
#define INSTR_UNBINDVQ	NINSTR('U','V','Q')
#define INSTR_REAL	NINSTR('r','e','l')
#define INSTR_ROUND	NINSTR('r','o','n')
#define INSTR_TRUNCATE	NINSTR('t','r','c')
#define INSTR_INT	NINSTR('i','n','t')
#define INSTR_RPRINTF	NINSTR('r','p','f')
#define INSTR_RSCANF	NINSTR('r','s','f')
#define INSTR_RPLUS	NINSTR('r','p','l')
#define INSTR_RMOINS	NINSTR('r','m','n')
#define INSTR_RFOIS	NINSTR('r','f','s')
#define INSTR_RDIV	NINSTR('r','d','v')
#define INSTR_RPOSP	NINSTR('r','p','p')
#define INSTR_REQ	NINSTR('r','e','q')
#define INSTR_POLYNOME	NINSTR('P','O','L')
#define INSTR_RPOLYNOME	NINSTR('r','p','o')
#define INSTR_NNPOWER	NINSTR('N','N','P')
#define INSTR_RNPOWER	NINSTR('r','n','p')
#define INSTR_NINTLOG2	NINSTR('L','O','G')
#define INSTR_RINTLOG2	NINSTR('r','l','o')
#define INSTR_POWER	NINSTR('p','w','r')
#define INSTR_EXP	NINSTR('e','x','p')
#define INSTR_LOG	NINSTR('l','o','g')
#define INSTR_SQRT	NINSTR('s','q','r')
#define INSTR_SIN	NINSTR('s','i','n')
#define INSTR_COS	NINSTR('c','o','s')
#define INSTR_TAN	NINSTR('t','a','n')
#define INSTR_ASIN	NINSTR('a','s','i')
#define INSTR_ACOS	NINSTR('a','c','o')
#define INSTR_ATAN	NINSTR('a','t','a')


init_instr ()
{
#if 0
{
	X = symbole (INSTR_USER);
	SET_SYMBOLES (e->racine, cons (X, SYMBOLES(e->racine)));
	/* RPLACA (CAR(CAR(e->racine)), cons (X, SYMBOLES(e->racine)) ); */
	/* { CAR( CAR(CAR(e->racine)) )= cons (X, SYMBOLES(e->racine)) ; SET_OCCUP(CAR( CAR(CAR(e->racine)) ),1); }; */
}
#endif

/*#include "instrex1.c"*/
	DEF_INSTR ("USER", INSTR_USER);
	DEF_INSTR ("I", INSTR_I);
	DEF_INSTR ("K", INSTR_K);
	DEF_INSTR ("S", INSTR_S);
	DEF_INSTR ("B", INSTR_B);
	DEF_INSTR ("C", INSTR_C);
	DEF_INSTR ("W", INSTR_W);
	DEF_INSTR ("APPLYTO", INSTR_APPLYTO);
	DEF_INSTR ("P", INSTR_P);
	DEF_INSTR ("J", INSTR_J);
	DEF_INSTR ("Y", INSTR_Y);
	DEF_INSTR ("QUOTE", INSTR_QUOTE);
	DEF_INSTR ("DEP", INSTR_DEP);
	DEF_INSTR ("REP", INSTR_REP);
	DEF_INSTR ("ECH", INSTR_ECH);
	DEF_INSTR ("DES", INSTR_DES);
	DEF_INSTR ("MON", INSTR_MON);
	DEF_INSTR ("NOT", INSTR_NOT);
	DEF_INSTR ("THEN", INSTR_THEN);
	DEF_INSTR ("EXEC", INSTR_EXEC);
	DEF_INSTR ("GETPROG", INSTR_GETPROG);
	DEF_INSTR ("JUMP", INSTR_JUMP);
	DEF_INSTR ("CALL", INSTR_CALL);
	DEF_INSTR ("ALT2", INSTR_ALT2);
	DEF_INSTR ("END", INSTR_END);
	DEF_INSTR ("MKCNL", INSTR_MKCNL);
	DEF_INSTR ("WAITSENDP", INSTR_WAITSENDP);
	DEF_INSTR ("WAITRECEIVEP", INSTR_WAITRECEIVEP);
	DEF_INSTR ("SEND", INSTR_SEND);
	DEF_INSTR ("RECEIVE", INSTR_RECEIVE);
	DEF_INSTR ("EVOL", INSTR_EVOL);
	DEF_INSTR ("EVOL1", INSTR_EVOL1);
	DEF_INSTR ("INSLCTXS", INSTR_INSLCTXS);
	DEF_INSTR ("GETCTX", INSTR_GETCTX);
	DEF_INSTR ("SETCTX", INSTR_SETCTX);
	DEF_INSTR ("SETCTXS", INSTR_SETCTXS);
	DEF_INSTR ("GETROOT", INSTR_GETROOT);
	DEF_INSTR ("SCHEDULE", INSTR_SCHEDULE);
	DEF_INSTR ("CAR", INSTR_CAR);
	DEF_INSTR ("CDR", INSTR_CDR);
	DEF_INSTR ("CONS", INSTR_CONS);
	DEF_INSTR ("EQ", INSTR_EQ);
	DEF_INSTR ("RPLACA", INSTR_RPLACA);
	DEF_INSTR ("RPLACD", INSTR_RPLACD);
	DEF_INSTR ("GETTYPE", INSTR_GETTYPE);
	DEF_INSTR ("SETTYPE", INSTR_SETTYPE);
	DEF_INSTR ("GETMARK", INSTR_GETMARK);
	DEF_INSTR ("SETMARK", INSTR_SETMARK);
	DEF_INSTR ("GETVAL", INSTR_GETVAL);
	DEF_INSTR ("SETVAL", INSTR_SETVAL);
	DEF_INSTR ("SYMBOL", INSTR_SYMBOL);
	DEF_INSTR ("MAKESYMBOL", INSTR_MAKESYMBOL);
	DEF_INSTR ("GETDEF", INSTR_GETDEF);
	DEF_INSTR ("SETDEF", INSTR_SETDEF);
	DEF_INSTR ("GETPROPS", INSTR_GETPROPS);
	DEF_INSTR ("SETPROPS", INSTR_SETPROPS);
	DEF_INSTR ("PLUS", INSTR_PLUS);
	DEF_INSTR ("MOINS", INSTR_MOINS);
	DEF_INSTR ("FOIS", INSTR_FOIS);
	DEF_INSTR ("DIV", INSTR_DIV);
	DEF_INSTR ("MOD", INSTR_MOD);
	DEF_INSTR ("ETL", INSTR_ETL);
	DEF_INSTR ("OUL", INSTR_OUL);
	DEF_INSTR ("OXL", INSTR_OXL);
	DEF_INSTR ("ZEROP", INSTR_ZEROP);
	DEF_INSTR ("POSP", INSTR_POSP);
	DEF_INSTR ("MAKEVECTOR", INSTR_MAKEVECTOR);
	DEF_INSTR ("VREF", INSTR_VREF);
	DEF_INSTR ("VSET", INSTR_VSET);
	DEF_INSTR ("MAKESTRING", INSTR_MAKESTRING);
	DEF_INSTR ("CREF", INSTR_CREF);
	DEF_INSTR ("CSET", INSTR_CSET);
	DEF_INSTR ("SIZE", INSTR_SIZE);
	DEF_INSTR ("SETPRIO", INSTR_SETPRIO);
	DEF_INSTR ("INCRPRIO", INSTR_INCRPRIO);
	DEF_INSTR ("SETINCR", INSTR_SETINCR);
	DEF_INSTR ("GETPRIO", INSTR_GETPRIO);
	DEF_INSTR ("GETINCR", INSTR_GETINCR);
	DEF_INSTR ("GETSTRAT", INSTR_GETSTRAT);
	DEF_INSTR ("SETSTRAT", INSTR_SETSTRAT);
	DEF_INSTR ("GETFLAGS", INSTR_GETFLAGS);
	DEF_INSTR ("SETFLAGS", INSTR_SETFLAGS);
	DEF_INSTR ("GETHAUT", INSTR_GETHAUT);
	DEF_INSTR ("SETHAUT", INSTR_SETHAUT);
	DEF_INSTR ("GETSTACK", INSTR_GETSTACK);
	DEF_INSTR ("SETSTACK", INSTR_SETSTACK);
	DEF_INSTR ("GETACC", INSTR_GETACC);
	DEF_INSTR ("SETACC", INSTR_SETACC);
	DEF_INSTR ("GETENV", INSTR_GETENV);
	DEF_INSTR ("SETENV", INSTR_SETENV);
	DEF_INSTR ("GETH", INSTR_GETH);
	DEF_INSTR ("MONDEP", INSTR_MONDEP);
	DEF_INSTR ("GETLX", INSTR_GETLX);
        DEF_INSTR ("SETLX", INSTR_SETLX);
	DEF_INSTR ("BSI", INSTR_BSI);
	DEF_INSTR ("ESI", INSTR_ESI);
	DEF_INSTR ("PREMINSTR", INSTR_PREMINSTR);
	DEF_INSTR ("AJINSTR", INSTR_AJINSTR);
	DEF_INSTR ("ENLINSTR", INSTR_ENLINSTR);
	DEF_INSTR ("SOMPIL", INSTR_SOMPIL);
	DEF_INSTR ("EMPIL", INSTR_EMPIL);
	DEF_INSTR ("DEPIL", INSTR_DEPIL);
	DEF_INSTR ("CATENATE", INSTR_CATENATE);
	DEF_INSTR ("READ", INSTR_READ);
	DEF_INSTR ("READSTRING", INSTR_READSTRING);
	DEF_INSTR ("PRINT", INSTR_PRINT);
	DEF_INSTR ("PRIN", INSTR_PRIN);
	DEF_INSTR ("PRINTSTRING", INSTR_PRINTSTRING);
	DEF_INSTR ("PRINTF", INSTR_PRINTF);
	DEF_INSTR ("PRINTCTX", INSTR_PRINTCTX);
	DEF_INSTR ("TYI", INSTR_TYI);
	DEF_INSTR ("TYO", INSTR_TYO);
	DEF_INSTR ("TYE", INSTR_TYE);
	DEF_INSTR ("READCN", INSTR_READCN);
	DEF_INSTR ("PRINCN", INSTR_PRINCN);
	DEF_INSTR ("LIRECARAC", INSTR_LIRECARAC);
	DEF_INSTR ("ECRIRECARAC", INSTR_ECRIRECARAC);
	DEF_INSTR ("GETPREVCHAR", INSTR_GETPREVCHAR);
	DEF_INSTR ("SETPREVCHAR", INSTR_SETPREVCHAR);
	DEF_INSTR ("NEXTCHAR", INSTR_NEXTCHAR);
	DEF_INSTR ("OPEN", INSTR_OPEN);
	DEF_INSTR ("CLOSE", INSTR_CLOSE);
	DEF_INSTR ("FLUSH", INSTR_FLUSH);
	DEF_INSTR ("EOF", INSTR_EOF);
	DEF_INSTR ("GETINCHAN", INSTR_GETINCHAN);
	DEF_INSTR ("SETINCHAN", INSTR_SETINCHAN);
	DEF_INSTR ("GETOUTCHAN", INSTR_GETOUTCHAN);
	DEF_INSTR ("SETOUTCHAN", INSTR_SETOUTCHAN);
	DEF_INSTR ("GETERRCHAN", INSTR_GETERRCHAN);
	DEF_INSTR ("SETERRCHAN", INSTR_SETERRCHAN);
	DEF_INSTR ("STDIN", INSTR_STDIN);
	DEF_INSTR ("STDOUT", INSTR_STDOUT);
	DEF_INSTR ("STDERR", INSTR_STDERR);
	DEF_INSTR ("GETPRINTLEVEL", INSTR_GETPRINTLEVEL);
	DEF_INSTR ("SETPRINTLEVEL", INSTR_SETPRINTLEVEL);
	DEF_INSTR ("GETSC", INSTR_GETSC);
	DEF_INSTR ("SETSC", INSTR_SETSC);
	DEF_INSTR ("TELLIN", INSTR_TELLIN);
	DEF_INSTR ("TELLOUT", INSTR_TELLOUT);
	DEF_INSTR ("SEEKIN", INSTR_SEEKIN);
	DEF_INSTR ("SEEKOUT", INSTR_SEEKOUT);
	DEF_INSTR ("GETAUTOFLUSH", INSTR_GETAUTOFLUSH);
	DEF_INSTR ("SETAUTOFLUSH", INSTR_SETAUTOFLUSH);
	DEF_INSTR ("LOAD", INSTR_LOAD);
	DEF_INSTR ("SYSFORK", INSTR_SYSFORK);
	DEF_INSTR ("SLEEP", INSTR_SLEEP);
	DEF_INSTR ("COMLINE", INSTR_COMLINE);
	DEF_INSTR ("GETSYSENV", INSTR_GETSYSENV);
	DEF_INSTR ("GC", INSTR_GC);
	DEF_INSTR ("DECLSYM", INSTR_DECLSYM);
	DEF_INSTR ("STEP", INSTR_STEP);
	DEF_INSTR ("GETSYSVAR", INSTR_GETSYSVAR);
	DEF_INSTR ("SETSYSVAR", INSTR_SETSYSVAR);
	DEF_INSTR ("QUIT", INSTR_QUIT);
	DEF_INSTR ("EQUAL", INSTR_EQUAL);
	DEF_INSTR ("LENGTH", INSTR_LENGTH);
	DEF_INSTR ("MEMQ", INSTR_MEMQ);
	DEF_INSTR ("MEMBER", INSTR_MEMBER);
	DEF_INSTR ("APPEND", INSTR_APPEND);
	DEF_INSTR ("LAST", INSTR_LAST);
	DEF_INSTR ("REVERSE", INSTR_REVERSE);
	DEF_INSTR ("REMQ", INSTR_REMQ);
	DEF_INSTR ("REMOVE", INSTR_REMOVE);
	DEF_INSTR ("GETVENV", INSTR_GETVENV);
	DEF_INSTR ("SETVENV", INSTR_SETVENV);
	DEF_INSTR ("ADDVEND", INSTR_ADDVEND);
	DEF_INSTR ("REMVENV", INSTR_REMVENV);
	DEF_INSTR ("PREFIX", INSTR_PREFIX);
	DEF_INSTR ("RPREFIX", INSTR_RPREFIX);
	DEF_INSTR ("GETVSENV", INSTR_GETVSENV);
	DEF_INSTR ("BINDVENV", INSTR_BINDVENV);
	DEF_INSTR ("UNBINDVENV", INSTR_UNBINDVENV);
	DEF_INSTR ("GENSYM", INSTR_GENSYM);
	DEF_INSTR ("BOUNDVENV", INSTR_BOUNDVENV);
	DEF_INSTR ("GETRECVENV", INSTR_GETRECVENV);
	DEF_INSTR ("VARIABLES", INSTR_VARIABLES);
	DEF_INSTR ("RENAME", INSTR_RENAME);
	DEF_INSTR ("CONSTR", INSTR_CONSTR);
	DEF_INSTR ("UNIF", INSTR_UNIF);
	DEF_INSTR ("SUBST", INSTR_SUBST);
	DEF_INSTR ("ENSEMBLE", INSTR_ENSEMBLE);
	DEF_INSTR ("SEPARER", INSTR_SEPARER);
	DEF_INSTR ("NCONSPTHEN", INSTR_NCONSPTHEN);
	DEF_INSTR ("ECH3", INSTR_ECH3);
	DEF_INSTR ("LIST1", INSTR_LIST1);
	DEF_INSTR ("LIST2", INSTR_LIST2);
	DEF_INSTR ("LIST3", INSTR_LIST3);
	DEF_INSTR ("UNCONS", INSTR_UNCONS);
	DEF_INSTR ("INCLQ", INSTR_INCLQ);
	DEF_INSTR ("INCL", INSTR_INCL);
	DEF_INSTR ("INSTR", INSTR_INSTR);
	DEF_INSTR ("ARG", INSTR_ARG);
	DEF_INSTR ("GETVSQ", INSTR_GETVSQ);
	DEF_INSTR ("SETVQ", INSTR_SETVQ);
	DEF_INSTR ("BINDVQ", INSTR_BINDVQ);
	DEF_INSTR ("UNBINDVQ", INSTR_UNBINDVQ);
	DEF_INSTR ("REAL", INSTR_REAL);
	DEF_INSTR ("ROUND", INSTR_ROUND);
	DEF_INSTR ("TRUNCATE", INSTR_TRUNCATE);
	DEF_INSTR ("INT", INSTR_INT);
	DEF_INSTR ("RPRINTF", INSTR_RPRINTF);
	DEF_INSTR ("RSCANF", INSTR_RSCANF);
	DEF_INSTR ("RPLUS", INSTR_RPLUS);
	DEF_INSTR ("RMOINS", INSTR_RMOINS);
	DEF_INSTR ("RFOIS", INSTR_RFOIS);
	DEF_INSTR ("RDIV", INSTR_RDIV);
	DEF_INSTR ("RPOSP", INSTR_RPOSP);
	DEF_INSTR ("REQ", INSTR_REQ);
	DEF_INSTR ("POLYNOME", INSTR_POLYNOME);
	DEF_INSTR ("RPOLYNOME", INSTR_RPOLYNOME);
	DEF_INSTR ("NNPOWER", INSTR_NNPOWER);
	DEF_INSTR ("RNPOWER", INSTR_RNPOWER);
	DEF_INSTR ("NINTLOG2", INSTR_NINTLOG2);
	DEF_INSTR ("RINTLOG2", INSTR_RINTLOG2);
	DEF_INSTR ("POWER", INSTR_POWER);
	DEF_INSTR ("EXP", INSTR_EXP);
	DEF_INSTR ("LOG", INSTR_LOG);
	DEF_INSTR ("SQRT", INSTR_SQRT);
	DEF_INSTR ("SIN", INSTR_SIN);
	DEF_INSTR ("COS", INSTR_COS);
	DEF_INSTR ("TAN", INSTR_TAN);
	DEF_INSTR ("ASIN", INSTR_ASIN);
	DEF_INSTR ("ACOS", INSTR_ACOS);
	DEF_INSTR ("ATAN", INSTR_ATAN);

}

/*
init_instr2 ()
{
#include "instrex2.c"
}
*/

sexpr sexpr_instr (int ni)
{
sexpr x;
	x = vrai;
	SET_TYPE (x, TYPE_INSTR);
	SET_VAL (x, ni);
	return x;
}

sexpr instr (char *s)
{
	return sexpr_instr (NINSTR(s[0],s[1],s[2]));
}

sexpr x_evo;

sexpr evolu (int flag, sexpr ctx)
{
	x_evo = ctx;
	evo (flag);
	return x_evo;
}

sexpr evol (sexpr ctx)
{
	return evolu (0, ctx);
}

sexpr evol_elem (sexpr ctx)
{
	return evolu (1, ctx);
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
	if (ents_sexpr(CAR(STRAT(CAR(file)))) < ents_sexpr(CAR(STRAT(ctx))))
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
		      cons(cons(CAR(PILES(ctxc)),
		       ((rs&RS_SND)?cons(x,CDR(BAS(ctxc))):
				    CDR(CDR(BAS(ctxc))) ) ),
		       CDR(CDR(CDR(ctxc)))))),
		cons(
		     cons(STRAT(ctx),
		      cons(PROG(ctx),
		       cons(cons(CAR(PILES(ctx)),
			((rs&RS_SND)?CDR(CDR(BAS(ctx))):
				     cons(xc,CDR(BAS(ctx))) ) ),
			CDR(CDR(CDR(ctx)))))),
		 nil));
}

sexpr instr_send (sexpr ctx)
{
	instr_exchange (ctx, RS_SND);
}

sexpr instr_receive (sexpr ctx)
{
	instr_exchange (ctx, RS_RCV);
}

#define PREMINSTR (car(*pprog))
#define AJINSTR(x) { *pprog=cons(x,*pprog); }
#define ENLINSTR(x) { x=car(*pprog); *pprog=cdr(*pprog); }

#define SOMPIL (car(*pbas))
#define EMPIL(x) { *pbas=cons(x,*pbas); }
#define DEPIL(x) { x=car(*pbas); *pbas=cdr(*pbas); }

sexpr user (sexpr f, sexpr x)
{
	if (equal_string (PNAME(f), "test-user"))
		return cons (symbole("test-user"), x);
	else
		return nil;
}

sexpr CANAL;

#define INSTR1(instruction,fonction) \
	case instruction: \
		DEPIL(x); \
		EMPIL(fonction(x)); \
                break;

#define INSTR2(instruction,fonction) \
	case instruction: \
		DEPIL(x); \
		DEPIL(y); \
		EMPIL(fonction(x,y)); \
		break;

#define INSTR3(instruction,fonction) \
	case instruction: \
		DEPIL(x); \
		DEPIL(y); \
		DEPIL(z); \
		EMPIL(fonction(x,y,z)); \
		break;

sexpr exec_instr1 (integer ni, sexpr *pctx, sexpr *pprog, sexpr *pbas)
{
sexpr x, y, z, l, f, n, r, ctx, h, nf;
integer i;
int j;
FILE *fp, *in, *out;
char *s;
float fl;

	switch (ni)
	{
		case INSTR_USER:

/*
#if (INSTR_I == INSTR_K)
.@&!,:;
#endif
*/
/*
	if (ni == INSTR_USER)
        {
*/

			ENLINSTR(f);
			DEPIL(x);
			EMPIL(user(f,x));

		case INSTR_I:
			break;
		case INSTR_K:
			ENLINSTR(x);
			ENLINSTR(y);
			AJINSTR(x);
			break;
		case INSTR_S:
			ENLINSTR(x);
			ENLINSTR(y);
			ENLINSTR(z);
			AJINSTR(LIST2(y,z));
			AJINSTR(z);
			AJINSTR(x);
			break;
		case INSTR_Y:
			ENLINSTR(x);
			AJINSTR(LIST2(sexpr_instr(INSTR_Y),x));
			AJINSTR(x);
			break;
		case INSTR_B:
			ENLINSTR(x);
			ENLINSTR(y);
			ENLINSTR(z);
			AJINSTR(LIST2(y,z));
			AJINSTR(x);
			break;
		case INSTR_C:
			ENLINSTR(x);
			ENLINSTR(y);
			ENLINSTR(z);
			AJINSTR(y);
			AJINSTR(z);
			AJINSTR(x);
			break;
		case INSTR_W:
			ENLINSTR(x);
			AJINSTR(PREMINSTR);
			AJINSTR(x);
			break;
		case INSTR_APPLYTO:
			ENLINSTR(x);
			ENLINSTR(y);
			AJINSTR(x);
			AJINSTR(y);
			break;
		case INSTR_P:
			ENLINSTR(x);
			ENLINSTR(y);
			ENLINSTR(z);
			AJINSTR(y);
			AJINSTR(x);
			AJINSTR(z);
                        break;			 
		case INSTR_J:
			ENLINSTR(x);
			break;

		case INSTR_QUOTE:
			ENLINSTR(x);
			EMPIL(x);
			break;
		case INSTR_DEP:
			DEPIL(x);
			break;
		case INSTR_REP:
			DEPIL(x);
			EMPIL(x);
			EMPIL(x);
			break;
		case INSTR_ECH:
			DEPIL(x);
			DEPIL(y);
			EMPIL(x);
			EMPIL(y);
			break;
		case INSTR_DES:
			DEPIL(x);
			ctx = *pctx;
			*pctx = cons(STRAT(ctx),cons(PROG(ctx),
				cons(cons(cons(FLAGS(ctx),
					cons(x,HAUT(ctx))),BAS(ctx)),
				CDR(CDR(CDR(ctx))))));
			break;
		case INSTR_MON:
			ctx = *pctx;
			EMPIL(CAR(HAUT(ctx)));
			*pctx = cons(STRAT(ctx),cons(PROG(ctx),
				cons(cons(cons(FLAGS(ctx),
					CDR(HAUT(ctx))),BAS(ctx)),
				CDR(CDR(CDR(ctx))))));
			break;
		case INSTR_MONDEP:
			ctx = *pctx;
			*pctx = cons(STRAT(ctx),cons(PROG(ctx),
				cons(cons(cons(FLAGS(ctx),
					CDR(HAUT(ctx))),BAS(ctx)),
				CDR(CDR(CDR(ctx))))));
			break;
		case INSTR_NOT:
			DEPIL(x);
			SET_VERITE (x, VERITE(x)?0:1);
			EMPIL(x);
			break;
		case INSTR_THEN:
			DEPIL(x);
			ENLINSTR(y);
			if (VERITE(x))
			{
				ENLINSTR(z);
				AJINSTR(y);
			}
			break;

		case INSTR_CAR:
			DEPIL(x);
			EMPIL(car(x));
			break;
		case INSTR_CDR:
			DEPIL(x);
			EMPIL(cdr(x));
			break;
		case INSTR_CONS:
			DEPIL(x);
			DEPIL(y);
			EMPIL(cons(x,y));
			break;
		case INSTR_EQ:
			DEPIL(x);
			DEPIL(y);
			if (EQ(x,y))
                        {
				EMPIL(vrai);
			}
			else
			{
				EMPIL(faux);
			}
			break;

		case INSTR_RPLACA:
			DEPIL(x);
			DEPIL(y);
			RPLACA(x,y);
			break;
		case INSTR_RPLACD:
			DEPIL(x);
			DEPIL(y);
			RPLACD(x,y);
			break;

		case INSTR_GETTYPE:
			DEPIL(x);
			EMPIL(sexpr_ent(TYPE(x)));
			break;
		case INSTR_SETTYPE:
			DEPIL(y);
			DEPIL(x);
			SET_TYPE (x, ent_sexpr(y));
			EMPIL(x);
			break;
		case INSTR_GETMARK:
			DEPIL(x);
			EMPIL(MARQUE(x)?vrai:faux);
			break;
		case INSTR_SETMARK:
			DEPIL(y);
			DEPIL(x);
			SET_MARQUE (x, VERITE(y));
			EMPIL(x);
			break;
		case INSTR_GETVAL:
			DEPIL(x);
			EMPIL(sexpr_ent(VAL(x)));
			break;
		case INSTR_SETVAL:
			DEPIL(y);
			DEPIL(x);
			SET_VAL (x, ent_sexpr(y));
			EMPIL(x);
			break;

		case INSTR_SYMBOL:
			DEPIL(x);
			EMPIL(symbole(ADRESSE_BLOC(x)));
			break;
		case INSTR_MAKESYMBOL:
			DEPIL(x);
			EMPIL(creer_symbole(ADRESSE_BLOC(x)));
			break;

		case INSTR_EXEC:
			DEPIL(x);
			AJINSTR(x);
			break;
		case INSTR_GETPROG:
			EMPIL(*pprog);
			break;
		case INSTR_JUMP:
			DEPIL(x);
			*pprog = x;
			break;
		case INSTR_GETSTACK:
			EMPIL(*pbas);
			break;
		case INSTR_SETSTACK:
			DEPIL(x);
			*pbas = x;
			break;

		case INSTR_ALT2:
			ENLINSTR(x);
			ENLINSTR(y);
			ctx = MAJ_CTX (*pctx, *pprog, *pbas);
			return LIST2 (CTX_AJOUTER_INSTR(ctx,x),
				      CTX_AJOUTER_INSTR(ctx,y));
		case INSTR_END:
			return vrai;

		case INSTR_MKCNL:
			DEPIL(x);
			EMPIL(cons(CANAL,cons(nil,cons(nil,cons(x,nil)))));
			break;
		case INSTR_WAITSENDP:
			DEPIL(x);
			if (VERITE(CAR(CDR(CDR(ctx)))) && !VERITE(CAR(CDR(ctx))))
			{
				EMPIL(vrai);
			}
			else
			{
				EMPIL(faux);
			}
			break;
		case INSTR_WAITRECEIVEP:
			DEPIL(x);
			if (VERITE(CAR(CDR(CDR(x)))) && VERITE(CAR(CDR(x))))
			{
				EMPIL(vrai);
			}
			else
			{
				EMPIL(faux);
			}
			break;
		case INSTR_SEND:
			return instr_send (MAJ_CTX(*pctx,*pprog,*pbas));
		case INSTR_RECEIVE:
			return instr_receive (MAJ_CTX(*pctx,*pprog,*pbas));

		case INSTR_EVOL:
			DEPIL(x);
			EMPIL(evol(x));
			break;
		case INSTR_EVOL1:
			DEPIL(x);
			EMPIL(evol_elem(x));
			break;
		case INSTR_INSLCTXS:
			DEPIL(x);
			DEPIL(y);
			EMPIL(ins_lctxs(x,y));
			break;
		case INSTR_GETCTX:
			EMPIL(MAJ_CTX(*pctx,*pprog,*pbas));
			break;
		case INSTR_SETCTX:
			DEPIL(x);
			if (tab_varsys[VARSYS_AUTOSCHEDULE]&2)
				return LIST1(x);
			else
			{
				*pctx = x;
				*pprog = PROG(x);
				*pbas = BAS(x);
			}
			break;
		case INSTR_SETCTXS:
			DEPIL(x);
			y = x;
			SET_VERITE (y, 1);
			return y;
		case INSTR_GETROOT:
			EMPIL(e->racine);
			break;

		case INSTR_GETDEF:
			DEPIL(x);
			EMPIL(DEF(x));
			break;
		case INSTR_SETDEF:
			DEPIL(x);
			DEPIL(y);
			SET_DEF (x, y);
			break;
		case INSTR_GETPROPS:
			DEPIL(x);
			EMPIL(PROPS(x));
			break;
		case INSTR_SETPROPS:
			DEPIL(x);
			DEPIL(y);
			SET_PROPS (x, y);
			break;

		case INSTR_PLUS:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_ent(ent_sexpr(x)+ent_sexpr(y)));
			break;
		case INSTR_MOINS:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_ent(ent_sexpr(y)-ent_sexpr(x)));
			break;
		case INSTR_FOIS:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_ent(ent_sexpr(x)*ent_sexpr(y)));
                        break;
		case INSTR_DIV:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_ent(ent_sexpr(y)/ent_sexpr(x)));
			break;
		case INSTR_MOD:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_ent(ent_sexpr(y)%ent_sexpr(x)));
                        break;
		case INSTR_ETL:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_ent(ent_sexpr(x)&ent_sexpr(y)));
			break;
		case INSTR_OUL:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_ent(ent_sexpr(x)|ent_sexpr(y)));
			break;
		case INSTR_OXL:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_ent(ent_sexpr(x)^ent_sexpr(y)));
                        break;

		case INSTR_ZEROP:
			DEPIL(x);
			EMPIL(VAL(x)?nil:vrai);
			break;
		case INSTR_POSP:
			DEPIL(x);
			EMPIL(((ent_sexpr(x)&0x800000)==0)?vrai:nil);
			break;

		case INSTR_MAKEVECTOR:
			DEPIL(x);
			EMPIL(alloc_vect(ent_sexpr(x)));
			break;
		case INSTR_MAKESTRING:
			DEPIL(x);
			EMPIL(alloc_bloc(ent_sexpr(x)));
			break;
		case INSTR_VREF:
			DEPIL(y);
			DEPIL(x);
			EMPIL(VREF(x,ents_sexpr(y)));
		case INSTR_CREF:
			DEPIL(y);
			DEPIL(x);
			EMPIL(sexpr_ent(CREF(x,ents_sexpr(y))));
			break;
		case INSTR_VSET:
			DEPIL(z);
			DEPIL(y);
			DEPIL(x);
			VSET(x,ents_sexpr(y),z);
			break;
		case INSTR_CSET:
			DEPIL(z);
			DEPIL(y);
			DEPIL(x);
			CSET(x,ents_sexpr(y),ents_sexpr(z));
			break;

		case INSTR_SIZE:
			DEPIL(x);
                        memcpy (&i, ((integer *)(ADRESSE_BLOC(x)))-1, sizeof(integer));
			EMPIL(sexpr_ent(i));
			break;

		case INSTR_SETPRIO:
			DEPIL(x);
			ctx = *pctx;
			if (tab_varsys[VARSYS_AUTOSCHEDULE]&4)
			{
				ctx = MAJ_CTX (*pctx, *pprog, *pbas);
				return LIST1(cons(cons(x,CDR(STRAT(ctx))),CDR(ctx)));
			}
			*pctx = cons(cons(x,CDR(STRAT(ctx))),CDR(ctx));
			break;
		case INSTR_INCRPRIO:
			DEPIL(x);
			if (tab_varsys[VARSYS_AUTOSCHEDULE]&4)
			{
				ctx = MAJ_CTX (*pctx, *pprog, *pbas);
				return LIST1(cons(cons(
					sexpr_ent(ent_sexpr(x)+ent_sexpr(CAR(STRAT(ctx))))
						,CDR(STRAT(ctx))),CDR(ctx)));
			}
			*pctx = cons(cons(
				sexpr_ent(ent_sexpr(x)+ent_sexpr(CAR(STRAT(ctx))))
					,CDR(STRAT(ctx))),CDR(ctx));
			break;
		case INSTR_SETINCR:
			DEPIL(x);
			ctx = *pctx;
			*pctx = cons(cons(CAR(STRAT(ctx)),cons(x,CDR(CDR(STRAT(ctx))))),
					CDR(ctx));
			break;
		case INSTR_GETPRIO:
			ctx = *pctx;
			EMPIL(CAR(STRAT(ctx)));
			break;
		case INSTR_GETINCR:
			ctx = *pctx;
			EMPIL(CAR(CDR(STRAT(ctx))));
			break;

		case INSTR_GETSTRAT:
			ctx = *pctx;
			EMPIL(STRAT(ctx));
			break;
		case INSTR_SETSTRAT:
			DEPIL(x);
			if (tab_varsys[VARSYS_AUTOSCHEDULE]&4)
			{
				ctx = MAJ_CTX (*pctx, *pprog, *pbas);
				return LIST1(cons(x,CDR(ctx)));
			}
			*pctx = cons(x,cdr(ctx));
			break;

		case INSTR_GETFLAGS:
			ctx = *pctx;
			EMPIL(FLAGS(ctx));
			break;
		case INSTR_SETFLAGS:
			DEPIL(x);
			ctx = *pctx;
			*pctx = cons(STRAT(ctx),cons(PROG(ctx),
					cons(cons(cons(x,HAUT(ctx)),BAS(ctx)),
					CDR(CDR(CDR(ctx))))));
			break;

		case INSTR_GETACC:
			ctx = *pctx;
			EMPIL(ACC(ctx));
			break;
		case INSTR_SETACC:
			DEPIL(x);
			ctx = *pctx;
			*pctx = cons(STRAT(ctx),cons(PROG(ctx),cons(PILES(ctx),cons(x,
					CDR(CDR(CDR(CDR(ctx))))))));
			break;

		case INSTR_GETENV:
			ctx = *pctx;
			EMPIL(ENV(ctx));
			break;
		case INSTR_SETENV:
			DEPIL(x);
			ctx = *pctx;
			*pctx = cons(STRAT(ctx),cons(PROG(ctx),cons(PILES(ctx),
					cons(ACC(ctx),cons(x,CDR(CDR(CDR(CDR(CDR(ctx))))))))));
			break;

		case INSTR_GETH:
			ENLINSTR(x);
			ctx = *pctx;
			h = HAUT(ctx);
			for (j=0; j<ent_sexpr(x); j++)
				h = CDR(h);
			EMPIL(CAR(h));
			break;
		case INSTR_GETHAUT:
			ctx = *pctx;
			EMPIL(HAUT(ctx));
			break;
		case INSTR_SETHAUT:
			DEPIL(x);
			ctx = *pctx;
			*pctx = cons(STRAT(ctx),cons(PROG(ctx),cons(cons(cons(FLAGS(ctx),x),
						BAS(ctx)),
					CDR(CDR(CDR(ctx))))));
			break;

		case INSTR_GETLX:
			ctx = *pctx;
			EMPIL(LX(ctx));
			break;
		case INSTR_SETLX:
			DEPIL(x);
			if (tab_varsys[VARSYS_AUTOSCHEDULE]&8)
			{
				ctx = MAJ_CTX (*pctx, *pprog, *pbas);
				return LIST1 (MODIF_LX(ctx,x));
			}
			ctx = *pctx;
			*pctx = MODIF_LX(ctx,x);
			break;

		case INSTR_BSI:
			ctx = *pctx;
			*pctx = MODIF_LX (ctx, sexpr_ent(1+ent_sexpr(LX(ctx))));
			break;
		case INSTR_ESI:
			ctx = *pctx;
			*pctx = MODIF_LX (ctx, sexpr_ent(-1+ent_sexpr(LX(ctx))));
			if (tab_varsys[VARSYS_AUTOSCHEDULE]&8)
				return LIST1(MAJ_CTX(*pctx,*pprog,*pbas));
			break;

		case INSTR_SCHEDULE:
			return LIST1(MAJ_CTX(*pctx,*pprog,*pbas));

		case INSTR_NCONSPTHEN:
			ENLINSTR(x);
			DEPIL(z);
			if (TYPE(z) != TYPE_CONS)
			{
				ENLINSTR(y);
				AJINSTR(x);
			}
			break;
		case INSTR_ECH3:
			DEPIL(x);
			DEPIL(y);
			DEPIL(z);
			EMPIL(x);
			EMPIL(y);
			EMPIL(z);
			break;

		case INSTR_LIST1:
			DEPIL(x);
			EMPIL(cons(x,nil));
			break;
		case INSTR_LIST2:
			DEPIL(x);
			DEPIL(y);
			EMPIL(cons(x,cons(y,nil)));
			break;
		case INSTR_LIST3:
			DEPIL(x);
			DEPIL(y);
			DEPIL(z);
			EMPIL(cons(x,cons(y,cons(z,nil))));
			break;
		case INSTR_UNCONS:
			DEPIL(x);
			EMPIL(cdr(x));
			EMPIL(car(x));
			break;
		case INSTR_INCLQ:
			DEPIL(x);
			DEPIL(y);
			EMPIL(inclq(x,y)?vrai:faux);
			break;
		case INSTR_INCL:
			DEPIL(x);
			DEPIL(y);
			EMPIL(incl(x,y)?vrai:faux);
			break;

		case INSTR_INSTR:
			ENLINSTR(x);
			ENLINSTR(y);
			ctx = *pctx;
			z = getvsenv(bindvenv(ENV(ctx),x,MAJ_CTX(*pctx,*pprog,*pbas)),y);
			*pctx = z;
			*pprog = PROG(z);
			*pbas = BAS(z);
			break;
		case INSTR_ARG:
			ENLINSTR(x);
			ENLINSTR(y);
			DEPIL(z);
			ctx = *pctx;
			*pctx = cons(STRAT(ctx),cons(PROG(ctx),cons(PILES(ctx),cons(ACC(ctx),
					cons(bindvenv(ENV(ctx),x,z),
					CDR(CDR(CDR(CDR(CDR(ctx))))))))));
			AJINSTR(cons(sexpr_instr(INSTR_UNBINDVQ),cons(x,nil)));
			AJINSTR(y);
			break;
		case INSTR_GETVSQ:
			ENLINSTR(x);
			ctx = *pctx;
			EMPIL(getvsenv(ENV(ctx),x));
			break;
		case INSTR_SETVQ:
			ENLINSTR(x);
			DEPIL(y);
			ctx = *pctx;
			*pctx = cons(STRAT(ctx),cons(PROG(ctx),cons(PILES(ctx),cons(ACC(ctx),
					cons(setvenv(ENV(ctx),x,y),
					CDR(CDR(CDR(CDR(CDR(ctx))))))))));
			break;
		case INSTR_BINDVQ:
			ENLINSTR(x);
			DEPIL(y);
			ctx = *pctx;
			*pctx = cons(STRAT(ctx),cons(PROG(ctx),cons(PILES(ctx),cons(ACC(ctx),
					cons(bindvenv(ENV(ctx),x,y),
					CDR(CDR(CDR(CDR(CDR(ctx))))))))));
			break;
		case INSTR_UNBINDVQ:
			ENLINSTR(x);
			ctx = *pctx;
			*pctx = cons(STRAT(ctx),cons(PROG(ctx),cons(PILES(ctx),cons(ACC(ctx),
					cons(unbindvenv(ENV(ctx),x),
					CDR(CDR(CDR(CDR(CDR(ctx))))))))));
			break;

		case INSTR_PREMINSTR:
			DEPIL(x);
			EMPIL(premiere_instr(x));
			break;
		case INSTR_AJINSTR:
			DEPIL(y);
			DEPIL(x);
			EMPIL(ctx_ajouter_instr(x,y));
			break;
		case INSTR_ENLINSTR:
			DEPIL(x);
			EMPIL(ctx_enlever_instr(x));
			break;

		case INSTR_SOMPIL:
			DEPIL(x);
			EMPIL(sommet(x));
			break;
		case INSTR_EMPIL:
			DEPIL(y);
			DEPIL(x);
			EMPIL(ctx_empiler(x,y));
			break;
		case INSTR_DEPIL:
			DEPIL(x);
			EMPIL(ctx_depiler(x));
			break;

		case INSTR_CATENATE:
			DEPIL(x);
			DEPIL(y);
			EMPIL(catenate(y,x));
			break;

		case INSTR_EQUAL:
			DEPIL(x);
			DEPIL(y);
			EMPIL(equal(x,y)?vrai:faux);
			break;
		case INSTR_LENGTH:
			DEPIL(x);
			EMPIL(sexpr_ent(length(x)));
			break;

		INSTR2 (INSTR_APPEND, append)
                INSTR1 (INSTR_LAST, last)
		INSTR1 (INSTR_REVERSE, reverse)
		INSTR2 (INSTR_REMQ, remq)
		INSTR2 (INSTR_REMOVE, remove)

		INSTR2 (INSTR_GETVENV, getvenv)
		INSTR3 (INSTR_SETVENV, setvenv)
		INSTR3 (INSTR_ADDVEND, addvenv)
		INSTR2 (INSTR_REMVENV, remvenv)
		INSTR2 (INSTR_GETVSENV, getvsenv)
		INSTR3 (INSTR_BINDVENV, bindvenv)
		INSTR2 (INSTR_UNBINDVENV, unbindvenv)	

		case INSTR_PREFIX:
			ENLINSTR(n);
			ENLINSTR(f);
			r = separer (ent_sexpr(n), *pprog);
			*pprog = append (car(r), cons(f,cdr(r)));
			break;
		case INSTR_RPREFIX:
			ENLINSTR(n);
			ENLINSTR(f);
			r = separer (ent_sexpr(n), *pprog);
			*pprog = append (reverse(car(r)), cons(f,cdr(r)));
			break;

		case INSTR_GENSYM:
			EMPIL(gensym());
			break;
		case INSTR_BOUNDVENV:
			DEPIL(x);
			DEPIL(y);
			EMPIL(boundvenv(x,y)?vrai:faux);
			break;
		INSTR2 (INSTR_GETRECVENV, getrecvenv)
		INSTR1 (INSTR_VARIABLES, variables)
		INSTR1 (INSTR_RENAME, renommer)
		INSTR2 (INSTR_CONSTR, construire)
		INSTR3 (INSTR_UNIF, unifier)
		INSTR3 (INSTR_SUBST, subst)
		INSTR1 (INSTR_ENSEMBLE, ensemble)
		case INSTR_SEPARER:
			DEPIL(x);
			DEPIL(y);
			EMPIL(separer(ent_sexpr(x),y));
			break;

		case INSTR_REAL:
			DEPIL(x);
			EMPIL(sexpr_reel((reel)ents_sexpr(x)));
			break;
		case INSTR_ROUND:
			DEPIL(x);
			EMPIL(sexpr_ent(partie_entiere(REEL_SEXPR(x)+0.5)));
			break;
		case INSTR_TRUNCATE:
			DEPIL(x);
			EMPIL(sexpr_ent((integer)REEL_SEXPR(x)));
			break;
		case INSTR_INT:
			DEPIL(x);
			EMPIL(sexpr_ent(partie_entiere(REEL_SEXPR(x))));
			break;
		case INSTR_RPRINTF:
			DEPIL(x);
			DEPIL(y);
			fprintf (sortie, ADRESSE_BLOC(x), REEL_SEXPR(y));
			break;
		case INSTR_RSCANF:
			DEPIL(x);
			fscanf (entree, ADRESSE_BLOC(x), &fl);
			if (tab_varsys[VARSYS_ECHO]&1)
				fprintf (sortie, "%f", fl);
			if (tab_varsys[VARSYS_ECHO]&2)
				fprintf (sorerr, "%f", fl);
			EMPIL(sexpr_reel((reel)f));
	 
		case INSTR_RPLUS:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_reel(REEL_SEXPR(x)+REEL_SEXPR(y)));
			break;
		case INSTR_RMOINS:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_reel(REEL_SEXPR(y)-REEL_SEXPR(x)));
			break;
		case INSTR_RFOIS:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_reel(REEL_SEXPR(x)*REEL_SEXPR(y)));
			break;
		case INSTR_RDIV:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_reel(REEL_SEXPR(y)/REEL_SEXPR(x)));
			break;
		case INSTR_RPOSP:
			DEPIL(x);
			EMPIL((REEL_SEXPR(x)>=0)?vrai:faux);
			break;
		case INSTR_REQ:
			DEPIL(x);
			DEPIL(y);
			EMPIL((REEL_SEXPR(x)==REEL_SEXPR(y))?vrai:faux);
			break;

		case INSTR_POLYNOME:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_ent(polynome(x,ents_sexpr(y))));
			break;
		case INSTR_RPOLYNOME:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_reel(rpolynome(x,REEL_SEXPR(y))));
			break;
		case INSTR_NNPOWER:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_ent(nnpower(ents_sexpr(y),ents_sexpr(x))));
			break;
		case INSTR_RNPOWER:
			DEPIL(x);
			DEPIL(y);
			EMPIL(sexpr_reel(rnpower(REEL_SEXPR(y),ents_sexpr(x))));
			break;
		case INSTR_NINTLOG2:
			DEPIL(x);
			EMPIL(sexpr_ent(nintlog2(ents_sexpr(x))));
			break;
		case INSTR_RINTLOG2:
			DEPIL(x);
			EMPIL(sexpr_ent(rintlog2(REEL_SEXPR(x))));
                        break;


		case INSTR_READ:
			EMPIL(lire_sexpr());
			break;
		case INSTR_READSTRING:
			DEPIL(x);
			EMPIL(sexpr_chaine(read_string(ADRESSE_BLOC(x))));
			break;
		case INSTR_PRIN:
			DEPIL(x);
			ecrire_sexpr(x);
			break;
		case INSTR_PRINT:
			DEPIL(x);
			ecrire_sexpr(x);
			aff_sortie("\n");
			break;
		case INSTR_PRINTSTRING:
			DEPIL(x);
			aff_sortie (ADRESSE_BLOC(x));
			break;

		case INSTR_TYI:
			EMPIL(sexpr_ent(getchar()));
			break;
		case INSTR_TYO:
			DEPIL(x);
			putchar (ent_sexpr(x));
			fflush (stdout);
			break;
		case INSTR_TYE:
			DEPIL(x);
			putc (ent_sexpr(x), stderr);
			fflush (stderr);
			break;

		case INSTR_READCN:
			EMPIL(sexpr_ent(getc(entree)));
			break;
		case INSTR_PRINCN:
			DEPIL(x);
			putc (ent_sexpr(x), sortie);
			autoflush(sortie);
			break;
		case INSTR_LIRECARAC:
			EMPIL(sexpr_ent(lire_carac()));
			break;
		case INSTR_ECRIRECARAC:
			DEPIL(x);
			ecrire_carac(ent_sexpr(x));
			autoflush(sortie);
			break;

		case INSTR_GETPREVCHAR:
			EMPIL(sexpr_ent(carac_lu));
			break;
		case INSTR_SETPREVCHAR:
			DEPIL(x);
			carac_lu = ent_sexpr(x);
			break;
		case INSTR_NEXTCHAR:
			carac_suiv();
			break;

		case INSTR_OPEN:
			DEPIL(x);
			DEPIL(y);
			z = alloc_bloc(sizeof(FILE *));
			fp = fopen (ADRESSE_BLOC(y), ADRESSE_BLOC(x));
			memcpy (ADRESSE_BLOC(z), &fp, sizeof(FILE *));
			EMPIL(z);
			break;
		case INSTR_CLOSE:
			DEPIL(x);
			memcpy (&fp, ADRESSE_BLOC(x), sizeof(FILE *));
			fclose (fp);
			break;
		case INSTR_FLUSH:
			fflush (sortie);
			break;
		case INSTR_EOF:
			EMPIL(feof(entree)?vrai:faux);
			break;
		case INSTR_GETINCHAN:
			x = alloc_bloc(sizeof(FILE *));
			memcpy (ADRESSE_BLOC(x), &entree, sizeof(FILE *));
			EMPIL(x);
			break;
		case INSTR_GETOUTCHAN:
			x = alloc_bloc(sizeof(FILE *));
			memcpy (ADRESSE_BLOC(x), &sortie, sizeof(FILE *));
			EMPIL(x);
			break;
		case INSTR_GETERRCHAN:
			x = alloc_bloc(sizeof(FILE *));
			memcpy (ADRESSE_BLOC(x), &sorerr, sizeof(FILE *));
			EMPIL(x);
			break;

		case INSTR_SETINCHAN:
			DEPIL(x);
			memcpy (&entree, ADRESSE_BLOC(x), sizeof(FILE *));
			break;
		case INSTR_SETOUTCHAN:
			DEPIL(x);
			memcpy (&sortie, ADRESSE_BLOC(x), sizeof(FILE *));
			break;
		case INSTR_SETERRCHAN:
			DEPIL(x);
			memcpy (&sorerr, ADRESSE_BLOC(x), sizeof(FILE *));
			break;

		case INSTR_STDIN:
			x = alloc_bloc (sizeof(FILE *));
                        fp = stdin;
			memcpy (ADRESSE_BLOC(x), &fp, sizeof(FILE *));
			EMPIL(x);
			break;
		case INSTR_STDOUT:
			x = alloc_bloc (sizeof(FILE *));
                        fp = stdout;
			memcpy (ADRESSE_BLOC(x), &fp, sizeof(FILE *));
			EMPIL(x);
			break;
		case INSTR_STDERR:
			x = alloc_bloc (sizeof(FILE *));
                        fp = stderr;
			memcpy (ADRESSE_BLOC(x), &fp, sizeof(FILE *));
			EMPIL(x);
			break;

		case INSTR_TELLIN:
			EMPIL(sexpr_ent(ftell(entree)));
			break;
		case INSTR_TELLOUT:
			EMPIL(sexpr_ent(ftell(sortie)));
			break;
		case INSTR_SEEKIN:
			DEPIL(x);
			DEPIL(y);
			fseek (entree, ents_sexpr(y), ent_sexpr(x));
			break;
		case INSTR_SEEKOUT:
			DEPIL(x);
			DEPIL(y);
			fseek (sortie, ents_sexpr(y), ent_sexpr(x));
			break;

		case INSTR_GETAUTOFLUSH:
			EMPIL(sexpr_ent(flag_autoflush));
			break;
		case INSTR_SETAUTOFLUSH:
			DEPIL(x);
			flag_autoflush = ent_sexpr(x);
			break;

		case INSTR_GETPRINTLEVEL:
			EMPIL(sexpr_ent(niveau_ecrire_sexpr));
			break;
		case INSTR_SETPRINTLEVEL:
			DEPIL(x);
			niveau_ecrire_sexpr = ent_sexpr(x);
			break;

		case INSTR_GETSC:
			DEPIL(x);
			EMPIL(sexpr_ent(tab_cs[ent_sexpr(x)]));
			break;
		case INSTR_SETSC:
			DEPIL(x);
			DEPIL(y);
			tab_cs[ent_sexpr(x)] = ent_sexpr(y);
			break;

		case INSTR_SLEEP:
			DEPIL(x);
			sleep (ent_sexpr(x));
			break;
		case INSTR_COMLINE:
			DEPIL(x);
			system (ADRESSE_BLOC(x));
			break;
		case INSTR_GETSYSENV:
			DEPIL(x);
			s = getenv(ADRESSE_BLOC(x));
			if (s)
			{
				EMPIL(sexpr_chaine(s));
			}
			else
			{
				EMPIL(nil);
			}
			break;
		case INSTR_SYSFORK:
			EMPIL(sexpr_ent(fork()));
			break;

		case INSTR_LOAD:
			in = entree;
			DEPIL(nf);
			entree = fopen (ADRESSE_BLOC(nf), "r");
			if (entree)
			{
				x = lire_sexpr();
				fclose(entree);
				AJINSTR(x);
			}
			else
			{
				erreur ("load: fichier inexistant ou non accessible : ");
				out = sortie;
				sortie = sorerr;
				ecrire_sexpr(nf);
				aff_sorerr (" \n");
				sortie = out;
			}
			entree = in;
			break;

		case INSTR_GC:
			gc();
			break;

		case INSTR_DECLSYM:
			SET_SYMBOLES (e->racine, cons(SOMPIL,SYMBOLES(e->racine)));
			break;

		case INSTR_STEP:
			tab_varsys[VARSYS_DEBUG] = 2;
			break;

		case INSTR_GETSYSVAR:
			DEPIL(x);
			EMPIL(sexpr_ent(tab_varsys[ent_sexpr(x)]));
			break;
		case INSTR_SETSYSVAR:
			DEPIL(x);
			DEPIL(y);
			tab_varsys[ent_sexpr(x)] = ents_sexpr(y);
			break;

		case INSTR_QUIT:
                	term ();

	}
	return nil;
}

sexpr exec_instr (integer ni, sexpr ctx)
{
sexpr prog, bas, lctxs;
	prog = PROG(ctx);
	bas = BAS(ctx);
	lctxs = exec_instr1 (ni, &ctx, &prog, &bas);
	if (VERITE(lctxs))
		return lctxs;
	return LIST1(MAJ_CTX(ctx,prog,bas));
}

sexpr autoid1 (sexpr ctx, integer n)
{
	return cons(cons(CAR(STRAT(ctx)),
		cons(CAR(CDR(STRAT(ctx))),
		 cons(cons(sexpr_ent(n),CAR(CDR(CDR(STRAT(ctx))))),
		  CDR(CDR(CDR(STRAT(ctx))))))),
		 CDR(ctx));
}

sexpr autoid (sexpr lctxs, integer n)
{
	if (length(lctxs) <= 1)
		return lctxs;
	if (TYPE(lctxs) != TYPE_CONS)
		return lctxs;
        return cons (autoid1(CAR(lctxs),n),autoid(CDR(lctxs),n+1));
}

sexpr NDF, SCHEDULEALARM1, SCHEDULEALARM2;

sexpr exec_si (sexpr ctx)
{
sexpr x, prog, bas, lctxs;

	prog = PROG(ctx);
	bas = BAS(ctx);

	if (TYPE(CAR(prog)) == TYPE_SYMBOLE && EQ(DEF(CAR(prog)),nil))
	{
		if (tab_varsys[VARSYS_NDF])
			prog = cons (NDF, prog);
		else
                	prog = CDR(prog);
	}

	if (tab_varsys[VARSYS_SCHEDULEALARM] & 1)
		prog = cons (SCHEDULEALARM1, prog);

	while (tab_varsys[VARSYS_NSEXPRS] < tab_varsys[VARSYS_MAXNSEXPRS])
	{
		x = CAR(prog);
		prog = CDR(prog);

		if (TYPE(x) == TYPE_INSTR)
		{
			lctxs = exec_instr1 (VAL(x), &ctx, &prog, &bas);
			if (VERITE(lctxs))
				return lctxs;
		}
		else if (TYPE(x) == TYPE_SYMBOLE)
		{
			if (TYPE(DEF(x)) == TYPE_INSTR)
			{
				lctxs = exec_instr1 (VAL(DEF(x)), &ctx, &prog, &bas);
				if (VERITE(lctxs))
					return lctxs;
			}
			else if (EQ(DEF(x),nil))
				return LIST1 (MAJ_CTX(ctx,cons(x,prog),bas));
			else
				prog = cons (DEF(x), prog);
		}
		else if (TYPE(x) == TYPE_CONS)
		{
			if (tab_varsys[VARSYS_MARKPROG])
				SET_MARQUE (prog, 1);
			prog = append (x, prog);
		}
		else if (TYPE(x) == TYPE_AS);
		else
			bas = cons(x,bas);
	}
	if (tab_varsys[VARSYS_SCHEDULEALARM] & 2)
		prog = cons (SCHEDULEALARM2, prog);

	return LIST1(MAJ_CTX(ctx,prog,bas));            
}

sexpr NPROG, RESTART;

sexpr evol2 (int flag, sexpr ctx)
{
sexpr x, prog;

	if (TYPE(PROG(ctx)) != TYPE_CONS)
	{
		if (tab_varsys[VARSYS_ERRFAT] & 4)
			return LIST1(cons(STRAT(ctx),cons(cons(NPROG,nil),CDR(CDR(ctx)))));
		else
			return nil;
	}
	if (!flag
		&& (ent_sexpr(LX(ctx))&0x800000)==0
		&& !(tab_varsys[VARSYS_AUTOSCHEDULE]&1))
		return exec_si (ctx);
	ENLEVER_INSTR (ctx, x);
	if (TYPE(x) == TYPE_INSTR)
		return exec_instr (VAL(x), ctx);
	if (TYPE(x) == TYPE_SYMBOLE)
	{
		if (TYPE(DEF(x)) == TYPE_INSTR)
			return exec_instr (VAL(DEF(x)), ctx);

		if (tab_varsys[VARSYS_NDF] && EQ(DEF(x),nil))
		{
			AJOUTER_INSTR(ctx,x);
			AJOUTER_INSTR(ctx,NDF);
		}
		AJOUTER_INSTR(ctx,DEF(x));
		return LIST1(ctx);
	}
	if (TYPE(x) == TYPE_CONS)
	{
		prog = PROG(ctx);
		if (tab_varsys[VARSYS_MARKPROG])
			SET_MARQUE (prog, 1);
		return LIST1(cons(STRAT(ctx),cons(append(x,prog),CDR(CDR(ctx)))));
	}
	if (TYPE(x) == TYPE_AS)
		return LIST1(ctx);
	else
	{
		EMPILER(ctx,x);
		return LIST1(ctx);
	}
}

sexpr evol1 (int flag, sexpr ctx)
{
sexpr nctxs;
	nctxs = evol2 (flag, ctx);
	if (!(tab_varsys[VARSYS_AUTOID]))
		return nctxs;
	return autoid(nctxs,1);
}

sexpr test_int (sexpr ctx)
{
	return ctx;
}

sexpr evol0 (int flag, sexpr ctx)
{
	if (tab_varsys[VARSYS_DEBUG])
		return evol1(flag,maj_strat(test_int(top_debug(ctx))));
	else
		return evol1(flag,maj_strat(test_int(ctx)));
}

evo (int flag)
{
	x_evo = evol0 (flag, x_evo);
}

sexpr evol_lctxs (sexpr lctxs)
{
sexpr nctxs;
	if (TYPE(CDR(lctxs)) != TYPE_CONS)
	{
		if (tab_varsys[VARSYS_ERRFAT] & 2)
			nctxs = LIST1(ctx_ajouter_instr(ctx_enlever_instr(CTX_INIT),RESTART));
		else
			term();
	}
	else
	{
		nctxs = evol(CAR(CDR(lctxs)));
                RPLACD (lctxs, CDR(CDR(lctxs)));
	}
	inserer_lctxs (nctxs, lctxs);
        return lctxs;
}

sexpr evol_racine (sexpr racine)
{
	SET_LCTXS (racine, evol_lctxs(LCTXS(racine)));
	return racine;
}


init ()
{
	init_sexpr ();
	init_interp ();
}

sexpr UNDEFINED;

init_interp ()
{
sexpr start;
	flag_autoflush = 1;
	e->racine = RACINE_INIT;

	start = symbole ("_START");
	/*DEF(start) = START_PROG;*/
        SET_DEF (start, START_PROG);
	init_instr ();

	UNDEFINED = symbole ("UNDEFINED");
	VAR = symbole ("VAR");
	VAN = CREER_VAR (nil);

	NDF = symbole ("_NDF");
	SET_DEF (NDF, sexpr_ninstr (INSTR_J));

	NPROG = symbole ("_NPROG");
	SET_DEF (NPROG, start);

	CANAL = symbole ("CANAL");
	SCHEDULEALARM1 = symbole ("_SCHEDULEALARM1");
	SCHEDULEALARM2 = symbole ("_SCHEDULEALARM2");

	SET_SYMBOLES (e->racine,
		cons (CANAL,
		 cons (SCHEDULEALARM2,
		  cons (SCHEDULEALARM1,
		   cons (RESTART,
		    cons (NPROG,
		     cons (NDF,
		      cons (VAR,
		       cons (UNDEF,
			SYMBOLES(e->racine) )))))))) );
	QUOTE = symbole ("QUOTE");
}

#define ARRET(ctx) (0)

interp ()
{
	init_varsys ();
	init_sexpr ();
	init_interp ();

	while (!ARRET(ctx))
	{
		valid ();
		e->racine = evol_racine (e->racine);
	}
	term ();
}

top_debug_interp ()
{
sexpr f, x, y, z, r;
	while (1)
	{
		/*printf*/ aff_sortie ("Fonction ? ");
		f = lire_sexpr ();
		if (EQ (f, symbole("quit")))
			goto fin_debug;
		else if (EQ (f, symbole ("evol-racine")))
		{
			x = lire_sexpr();
			ecrire_sexpr (x);
			aff_sortie ("\n");
			r = evol_racine (x);
			ecrire_sexpr (r);
			aff_sortie ("\n");
		}
		else if (EQ (f, symbole("evol")))
		{
			x = lire_sexpr ();
			ecrire_sexpr (x);
			aff_sortie ("\n");
			r = evol (x);
			ecrire_sexpr (r);
			aff_sortie ("\n");
		}
		else if (EQ (f, symbole("ctx-empiler")))
		{
			x = lire_sexpr ();
			ecrire_sexpr (x);
			aff_sortie ("\n");
			y = lire_sexpr ();
			ecrire_sexpr (y);
			aff_sortie ("\n");
			r = ctx_empiler (x, y);
			ecrire_sexpr (r);
			aff_sortie ("\n");
		}
		else if (EQ (f, symbole("def")))
		{
			x = lire_sexpr ();
			ecrire_sexpr (x);
			aff_sortie ("\n");
			r = DEF(x);
			ecrire_sexpr (r);
			aff_sortie ("\n");
		}
		else if (EQ (f, symbole("setdef")))
		{
			x = lire_sexpr ();
			ecrire_sexpr (x);
			aff_sortie ("\n");
			y = lire_sexpr ();
			ecrire_sexpr (y);
			aff_sortie ("\n");
			SET_DEF (x, y);
		}
		else if (EQ (f, symbole("ctx-ajouter-instr")))
		{
			x = lire_sexpr ();
			ecrire_sexpr (x);
			aff_sortie ("\n");
			y = lire_sexpr ();
			ecrire_sexpr (y);
			aff_sortie ("\n");
			r = CTX_AJOUTER_INSTR (x, y);
			ecrire_sexpr (r);
			aff_sortie ("\n");
		}
		else if (EQ (f, symbole("val")))
		{
			x = lire_sexpr ();
			ecrire_sexpr (x);
			aff_sortie ("\n");
			sprintf (buf, "%X\n", VAL(x));
			aff_sortie (buf);
		}
		else
			aff_sortie ("Erreur\n");
	}
	fin_debug: ;
}

term ()
{
	printf ("Au revoir !\n");
	exit (0);
}

main ()
{
	init_atoms ();
	init_sexpr ();
	init_varsys ();
	top_test ();
	top_debug_interp ();
        interp ();
}



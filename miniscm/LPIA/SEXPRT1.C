
/* Expressions symboliques
	J 5-2-87
	30/7/1997 - 3/8/1997
*/

#include <stdio.h>
#include <math.h>

#define integer long

#define TAILLE_SEXPR 32

#define TAILLE_TNSEXPRS 0x400 /*0x4000*/

#ifdef DEBUG
#define TAILLE_ZCONS 50
#define TAILLE_ZBLOCS 100
#else
#define TAILLE_ZCONS 0x1000 /*0x10000*/
#define TAILLE_ZBLOCS 0x800 /*0x10000*/
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
	tab_varsys[VARSYS_DEBUG_CONT] = 'c' << 16 /*'c\0\0'*/; /* a verifier */
	tab_varsys[VARSYS_DEBUG_IMP] = 'i' << 16 /*'i\0\0'*/;
	tab_varsys[VARSYS_DEBUG_EXEC] = 'e' << 16 /*'e\0\0'*/;
	tab_varsys[VARSYS_DEBUG_CHMD] = 'd' << 16 /*'d\0\0'*/;
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

term ()
{
}

main ()
{
	init_atoms ();
	init_sexpr ();
	init_varsys ();
	top_test ();
}




#include "ext.h"
#include <stdlib.h>
#include <time.h>

extern char obuf[];

struct test_struct1 xxx1;
struct test_struct2 xxx2;

#define DX(xname) if (!strcmp(name,#xname)) val=(long)xname; else

long val_ext1 (char *name, long *pval)
{
		long val;

		DX(time)
		DX(srand)
		DX(rand) 

		DX(exit)
		DX(atoi)

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

	{
	*offset=-1;
	*size=-1;
	*sizestr = -1;

	sprintf (obuf, "Undefined type <%s> for field <%s>", tname, fname);
	trace(obuf);

	return 0;
	}

}

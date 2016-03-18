
struct test_struct1
{
		char ccc;
		short sss;
		long lll;
};

struct test_struct2
{
		struct test_struct2 *ppp;
		struct test_struct1 ttt;
};

long val_ext (char *name);

/* long field_offset (char *tname, char *fname); */

int field (char *tname, char *fname, long *offset, long *size, long *sizestr);






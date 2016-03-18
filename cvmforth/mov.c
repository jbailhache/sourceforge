
#include "cvm.h"

#if 0
/* #define PPC */
#define INTERP 
/* #define COMPARM */

#ifndef PPC
#include <stdio.h>
#endif

#include <stdlib.h>

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

#endif

int reg_num_x86[] = { 0, 2, 1, -1, -1, -1, -1, -1, -1, -1, 6, 3, 7, 0, 0, 0 };

const char * reg_name_x86 [] = { "ax", "dx", "cx", "", "", "", "", "", "", "", "si", "bx", "di" };

extern FILE *f_tasm;
extern FILE *f_asmi;
extern FILE *f_asml;
extern FILE *f_x86;

#define code_mov_r_r 0x10000

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

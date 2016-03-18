
#include <dos.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
/*#include <sys\stat.h>*/
/*#include "c:\borland\bcc55\include\sys\stat.h"*/

#define S_IFMT   0xF000  /* file type mask */
#define S_IFDIR  0x4000  /* directory */
#define S_IFIFO  0x1000  /* FIFO special */
#define S_IFCHR  0x2000  /* character special */
#define S_IFBLK  0x3000  /* block special */
#define S_IFREG  0x8000  /* or just 0x0000, regular */
#define S_IREAD  0x0100  /* owner may read */
#define S_IWRITE 0x0080  /* owner may write */
#define S_IEXEC  0x0040  /* owner may execute <directory search> */


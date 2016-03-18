
#include <stdio.h>

main ()
{
  int c;

  for (;;)
   {
     c = getchar ();
     if (feof(stdin))
       break;
     if (c != 13)
       putchar(c);
   }
}

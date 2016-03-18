/****************************************************************************
 *                                                                          *
 * File    : main.c                                                         *
 *                                                                          *
 * Purpose : Console mode (command line) program.                           *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/

#include <stdio.h>

void trace (char *s)
{
 // printf ("trace: %s\n",s);
}

void tracew (char *s)
{
}

void message (char *s)
{
 printf ("message: %s\n");
}

void handle_message (void)
{
}

void readb (char *s, int n)
{
 fgets (s, n, stdin);
}

void printb (char *s)
{
 printf ("%s", s);
}

/****************************************************************************
 *                                                                          *
 * Function: main                                                           *
 *                                                                          *
 * Purpose : Main entry point.                                              *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/

void mslpia_main();

int main(int argc, char *argv[])
{
    /* printf("Hello, world!\n"); */
    mslpia_main();
    return 0;
}


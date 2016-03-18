WMFORTH
(c) Jacques Bailhache, 2009

WMForth is a Forth compiler for Pocket PC with ARM processor running Windows Mobile. 
It has been tested on a HTC Athena running Windows Mobile 5.0 and a Samsung Omnia running Windows Mobile 6.1.
It has been compiled with Embedded Visual C++ 4.0.

WMForth is a compiler which reads the Forth source from a file whose name is obtained by adding ".4th.txt" to the name of the compiler. For example, if the name of the compiler is "wmforth.exe", then it will read the source from the file "wmforth.exe.4th.txt". This code is compiled and immediately executed. The output of the program is written to the file "wmforth.exe.out.txt", a dump of the memory content is written to "wmforth.exe.dump.txt" and some debugging traces are written to "wmforth.exe.trace.txt".

WMForth is a non-standard Forth. It includes the most important Forth words but some words included in the different standardfs of Forth may not be defined in WMForth.

WMForth also contains some non-standard words. Some of these words are :

CM which calls a function in a Windows library. It is followed by the name of the library, the name of the function and the number of arguments.
Example : 
: tik
 CM coredll.dll GetTickCount 0 ;

CARGS and VARGS are used to define words with named arguments, respectively constants or variables. They are followed by the name of the arguments, ":", the word to be define, and its definition.  
Examples :
CARGS a : double a a + ;
VARGS a : double a @ a @ + ;

WMForth also includes a non-standard ARM assembler.
Assembler code may be inserted with the word ASM{ :
ASM{
... assembler code ...
}

The source "wmforth.exe.4th.txt" gives an example of a program which first displays a picture of the Mandelbrot set, and then waits for some Forth code typed in a text box. When the "OK" button is tapped, the code is immediately compiled, executed and the result appears in the same text box.

The work remaining to do is :

- Add the missing words for standard compliance

- Write a complete documentation, specially for the non-standard words, and for the non-standard ARM assembler

- Add other useful extensions



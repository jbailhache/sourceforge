/*
program lpia (input, output);
 */

#define DEFINIR
#define TURBO_C

/*
#include <dos.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys\stat.h>
*/

#include "lpia.h"

/*
const
(*  trace = -1; *)
*/

#ifdef DEFINIR

#define maxcons 21001 /*6441*/ /*32001*/
#define maxnse 5000
#define maxchamp 127
#define maxichamp 1000
#define maxvarsys 256

#define nul 0
#define true 4
#define symb 3588
#define stop (256*'.'+4)

#define cnul 'N'
#define ctrue 'T'
#define csymb 'S'
#define cinstr 'I'
#define ccarac 'C'
#define ccons '.'
#define chex '/'

#define varsys_pcons 1
#define varsys_pnse 2
#define varsys_prdca 3

#define VARSYS_GARDER_VARS 0x41
#define VARSYS_OCCUR_CHECK 0x42
#define VARSYS_ADDV 0x43

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

#define type_as 0
#define type_instr 1
#define type_ent 2

#define ievi 39 /* ' */
#define ieval '&'
#define ievol ','
#define ievol_limnse 'E'
#define idep ';'
#define irep 'R'
#define iech 'X'
#define ides '['
#define imon ']'
#define inot 'N'
#define ithen '>'
#define ieq '='
#define icar 'A'
#define icdr 'D'
#define icons 'C'
#define igetf '?'
#define isetf '!'
#define irpa 'a'
#define irpd 'd'
#define iplus '+'
#define imns '-'
#define ietl '.'
#define idiv2 '/'
#define iposp 'P'
#define igf 'F'
#define igv 'V'
#define itys 'S'
#define ityi 'I'
#define ityo 'O'
#define irdca 'r'
#define ilica 'l'
#define ilise 'L'
#define ipeek '%'
#define ipoke '<'
#define iusr 'U'
#define igetsv 'g'
#define isetsv 's'
#define iinp 'i'
#define ioutp 'o'
#define ireset '@'
#define irewrite '#'
#define iclose '$'
#define ieof 'e'
#define iassign 'f'
#define ichsymb 'z'
#define iintr '^'
#define isavecore 'v'
#define irestorecore 't'
#define isf ':'
#define iinport '1'
#define ioutport '2'
#define iexeclm '3'
#define isound '5'

#define icbk 11
#define icbs 19
#define icby 25
#define icbb 2
#define icbc 3
#define icbj 10

#define iprint1 '6'

#define i_append 1

#define maxsource 1000

#endif

/*
type
  registres = record
		ax, bx, cx, dx,
		bp, di, si, ds, cs, flags: integer;
	      end;

var
*/
  struct WORDREGS reg;
  struct SREGS sreg;

#ifdef DEFINIR
  typedef int integer;
  typedef int sexpr;
#endif

  sexpr tabcons[maxcons+1]; /* tabcons : array[0..maxcons] of integer; */
  sexpr tabnse[maxnse+1];
  integer tabvarsys[maxvarsys+1];
  sexpr tabchamp[maxichamp+1];
  char tabsource[maxsource+1]; /* : array [1..maxsource] of char;*/
  integer inp_, outp_;

  /* FILE * fich[9]; */
#define fichier int
  fichier fich[9];
  /* buffer : string; */
  char buffer[256];

  /* FILE *core; */ /* core : file of integer; */

#ifdef DEFINIR
#define core (fich[7])
#endif

#ifdef MICROSOFT_C
sound() { }
nosound () { }
#define inportb inp
#define outportb outp
#endif

save_core()
{
  register integer i;
  /* rewrite(core); */
  /*fseek*/ lseek(core,0,SEEK_SET);
  /* for i:=0 to maxvarsys do write(core,tabvarsys[i]); */
  for (i=0; i<=maxvarsys; i++) /*fwrite(tabvarsys+i,sizeof(integer),1,core);*/
			       write(core,tabvarsys+i,sizeof(integer));
  for (i=0; i<=maxichamp; i++) write(core,tabchamp+i,sizeof(sexpr));
  for (i=0; i<=maxcons; i++) write(core,tabcons+i,sizeof(sexpr));
  for (i=0; i<=maxnse; i++) write(core,tabnse+i,sizeof(sexpr));
}

restore_core()
{
  register integer i;
  /* reset(core); */
  lseek(core,0,SEEK_SET);
  for (i=0; i<=maxvarsys; i++) /*fread(tabvarsys+i,sizeof(integer),1,core);*/
			       read(core,tabvarsys+i,sizeof(integer));
  for (i=0; i<=maxichamp; i++) read(core,tabchamp+i,sizeof(sexpr));
  for (i=0; i<=maxcons; i++) read(core,tabcons+i,sizeof(sexpr));
  for (i=0; i<=maxnse; i++) read(core,tabnse+i,sizeof(sexpr));
}


/*
procedure recup (x: integer); forward;
function car (x: integer) : integer; forward;
function cdr (x: integer) : integer; forward;
procedure inisrc; forward;
function lise: integer; forward;
function append (x, y: integer): integer; forward;
function hex (h, l: char) : char;  forward;
function hex1 (c: char) : integer; forward;
procedure instr(ni: integer); forward;
function val(x: integer): integer; forward;
function lint(n: integer): integer; forward;
*/

recup();
inisrc();
sexpr lise();
sexpr append();
/*
char hex();
integer hex1();
*/
instr();
/*
integer val();
sexpr lint();
*/

#ifdef DEFINIR
#define hl(h,l) ((((h) & 0xFF) << 8) | ((l) & 0xFF))
#define itc(x) ((((x) & -4) >> 1) & 32767)

#define car(x) (tabcons[itc(x)])
#define cdr(x) (tabcons[itc(x)+1])

#define hex1(c) (((c)<='9')?((integer)(c)-(integer)'0'):(integer)(c)-(integer)'A'+10)
/* define hex(h,l) ((char) ( (hex1(h)<<4) + hex1(l) ) ) */
#endif

char hex(h,l) char h,l;
{
  register integer x,y;
  char r;
  x=hex1(h);
  y=hex1(l);
  r=(x << 4) + y;
  return r;
/*
  return (hex1(h) << 4) + hex1(l));
*/
}

#ifdef DEFINIR
#define val(x) (((x) & -256) >> 8)
#define valp(x) (val(x) & 255)

#define lint(n) (((n)<<8)+36)
/*
function trace: integer;
begin
  trace:=val(tabchamp[champ_trace]);
end;
*/

#define trace (val(tabchamp[champ_trace]))

#define begin {
#define end }

/*
integer hl (integer h, l)
begin
  if (l<0) l=l+256;
  if (h>127) h=h-256;
  return 256*h+l;
end

integer itc(integer x)
begin
  itc:=((x and -4) div 2) and 32767;
end;
*/


afse(integer x);
tyo(char c);

#define then
#define do
#define chr(x) ((char)(x))

#endif

gc()
{
  register integer i;
  if ((trace & 64) == 64) then
    begin
      afse(tabchamp[champ_prog]); tyo(chr(10)); tyo(chr(13));
    end;
  for (i=0; i<=maxcons; i+=2) /* step 2 */ do
	 tabcons[i]=tabcons[i] & -2;
  /* indices pairs = car, indices impairs = cdr *)
  (* atomes : High 8 bits val, 4 bits type, marque, verite, 0, occup Low *)
  (* paires : High 14 bits indice/2, 1, occup */
  for (i=0; i<=maxchamp; i++) do
	 recup(tabchamp[i]);
  for (i=0; i<=tabvarsys[varsys_pnse]; i++) do
	 recup(tabnse[i]);
  /* varsys_pnse pointe sur la derniere nse creee */
  i=0;
  while (((tabcons[i] & 1) != 0) && (i<maxcons)) do
	 i=i+2;
  tabvarsys[varsys_pcons]=i;
  if ((trace & 64)==64) then afse(tabchamp[champ_prog]);
end;

#ifdef DEFINIR
#define procedure
#endif

procedure valid()
begin
  tabvarsys[varsys_pnse]=-1 /* 0 */;
end;

procedure recup (sexpr x)
{
  register integer i;
  if ((x & 2) != 0) then
  begin
  i = itc(x); /* (x and -4) div 2; */
  if ((tabcons[i] & 1) == 0) then
  begin
	 i=itc(x);
	 tabcons[i]=tabcons[i] | 1;
	 recup(car(x));
	 recup(cdr(x));
  end;
  end;
end;

sexpr alcons()
{
  register integer i;
  i=tabvarsys[varsys_pcons];
/*
  if ((trace & 64)==64 then write('<AC1:',i,'>');
*/
  while ((i<maxcons-1) && ((tabcons[i] & 1) != 0)) do
	 i=i+2;
/*
  if ((trace & 64)==64) then write('<AC2:',i,'>');
*/
  if ((tabcons[i] & 1) != 0) then
  begin
	 gc();
	 i=0;
	 while /* i<>maxcons */ ((i<maxcons-1) && ((tabcons[i] & 1) != 0)) do
		i=i+2;
	 if ((tabcons[i] & 1) != 0) then printf("Debordement memoire\n");
  end;
/*
  if (trace and 64)=64 then write('<AC3:',i,'>');
*/
  tabcons[i]=tabcons[i] | 1;
  tabvarsys[varsys_pcons]=i;
  if (tabvarsys[varsys_pnse]>=maxnse) printf("Debordement nse \n");
  tabvarsys[varsys_pnse]=tabvarsys[varsys_pnse]+1;
  tabnse[tabvarsys[varsys_pnse]]=i*2+3;
  return i*2+3;
/*if (trace and 64)=64 then write('<AC4:',i,',',i*2+3,'>'); */
end;

#ifdef DEFINIR
typedef int logical;

#define vrai 1
#define faux 0

/*
logical eq (integer x, y)
begin
  eq := (x and -2) = (y and -2);
end;
*/

#define eq(x,y) (((x) & -2)==((y) & -2))

/*
function car (x: integer) : integer;
begin
  car:=tabcons[itc(x) (* (x and -4) div 2 *) ];
end;

function cdr (x: integer) : integer;
begin
  cdr:=tabcons[itc(x) (* (x and -4) div 2 *) + 1];
end;
*/


/*
procedure rplaca (x, y: integer);
begin
  if (trace and 64)=64 then write('<RA:',x and 3,':',(x and -4) div 2,',',itc(x),'>');
  tabcons[itc(x) (* (x and -4) div 2 *) ] := (y or 1);
end;

procedure rplacd (x, y: integer);
begin
  if (trace and 64)=64 then write('<RD:',x and 3,':',(x and -4) div 2,',',itc(x),'>');
  tabcons[itc(x) (* (x and -4) div 2 *) + 1] := y;
end;
*/

#define rplaca(x,y) (car(x)=(y|1))
#define rplacd(x,y) (cdr(x)=y)

#endif

sexpr cons (sexpr x, sexpr y)
{
  register sexpr i;
  i=alcons();
  rplaca(i,x);
  rplacd(i,y);
  return i;
}

#ifdef DEFINIR
#define not !
#endif

logical eql(integer x, integer y)
begin
  if ((x & 2)==0) then
    begin
      if ((y & 2)==0) then return(0==0);
		     else return(0==1);
    end
  else
    begin
      if ((y & 2)==0) then return(0==1);
		     else
		       begin
			 if (not eq(car(x),car(y))) then return(0==1);
						  else return eql(cdr(x),cdr(y));
		       end;
    end;
end;

sexpr ch_symb(sexpr symbs, sexpr pname)
begin
  if ((symbs & 2)==0) then return nul;
  else if (eql(car(cdr(car(symbs))),pname))
  then return car(symbs);
  else return ch_symb(cdr(symbs),pname);
end;

sexpr VAN, VAR, NOM_VAN;

int n_evol, n_break;
FILE *f_evol, *f_break;


procedure init()
begin
  tabchamp[champ_trace]=lint(/*14*/0);
  tabchamp[champ_nivafse]=lint(32);
  tabchamp[champ_prog]=nul;
  tabchamp[champ_bas]=nul;
  tabchamp[champ_limnse]=/* lint(maxnse/3) */ -1;
  tabchamp[champ_symb]=nul;
  inp_=1; outp_=2;
  tabvarsys[varsys_pcons]=0;
  n_evol = 0;
  /*
  f_evol = fopen("lpia.evo","r+");
  */
  f_break = fopen("lpia.brk","r");
  fscanf(f_break,"%d",&n_break);
  fclose(f_break);
  /* assign(core,'lpia.cor'); */
  /* core=fopen("lpia.cor","+b"); */
  core=open("lpia.cor",O_RDWR | O_BINARY);
/*  tabsource := '.I''.C/41.IO.IIN'; *)
(*  tabvarsys[varsys_pnse]:=0; */
  valid();
  inisrc();
/*  gc; */
  valid();
  VAN=cons(VAR,cons(NOM_VAN,nul));
end;

procedure inisrc()
{
  integer x;
  /* assign(fich[inp_],'lpia.ini'); */
  /* fich[inp_]=fopen("lpia.ini","r"); */
  fich[inp_]=open("lpia.ini",O_RDONLY);
  /* reset(fich[inp]); */
  /* assign(fich[outp],''); */
  fich[outp_]=/*stdout*/ 1;
  /* rewrite(fich[outp]); */
  tabvarsys[varsys_prdca]=0;
  tabvarsys[champ_prog]=nul;
  x = true;
  while (x!=nul) do
    begin
      x=lise();
      tabchamp[champ_prog]=append(tabchamp[champ_prog],cons(x,nul));
      if ((trace & 8)==8) then putchar('^');
    end;
  /* tabchamp[champ_prog]:=lise; */
/*
  assign(fich[inp],'');
  reset(fich[inp]);
*/
  fich[inp_]=/*stdin*/ 0;
end;

char tyi()
{
  char c;
  /* read(fich[inp],c); (* read(c) *) */
  /* c=getc(fich[inp]); */
  read(fich[inp_],&c,1);
  if ((trace & 4)==4) then putchar(c);
  return c;
}

procedure tyo (char c)
begin
int n;
  /* putc(c,fich[outp]); */
  n=write(fich[outp_],&c,1);
end;

char rdca()
begin
  char r;
  if (tabvarsys[varsys_prdca]==0)
  then return tyi();
  else
  begin
	 /* lecture en memoire */
	 r = tabsource[tabvarsys[varsys_prdca]];
	 tabvarsys[varsys_prdca] = tabvarsys[varsys_prdca]+1;
	 return r;
  end;
end;

char lica()
{
  register char c;
  c=rdca();
  if (c!=chex) then return c; else
  begin
	 c=rdca();
	 if (c==chex) then return c; else
		{char c1,r;
		 c1=rdca();
		 r=hex(c,c1);
		 return r;}
  end;
end;

/*
char hex (char h, l)
begin
  hex := chr(16*hex1(h)+hex1(l));
end;
*/


/*
function hex1 (c: char) : integer;
begin
  if c<='9' then hex1 := ord(c) - ord('0')
  else hex1 := ord(c) - ord('A') + 10;
end;
*/

#ifdef DEFINIR
#define ord(x) ((integer)(x))
#endif

sexpr lise()
/* label 1; */
{
  char c;
  integer x;
  l1:
  c=lica();
  if (c==cnul) /* 'N' */ then return nul;
  else if (c==ctrue) /* 'T' */ then return true;
  else if (c==csymb) /* 'S' */ then return symb;
  else if (c==cinstr) /* 'I' */ then return hl(ord(lica()),20);
	 /* 256*ord(lica())+20 */
  else /* if c!=ccons *) (* '.' */
	 if (c==ccarac) then return hl(ord(lica()),36);
  else if (c==ccons) then begin
	 x=lise();
	 return cons(x,lise());
  end
  else goto l1;
end;

/*
function consp (x: integer) : boolean;
begin
  consp := (x and 2) != 0;
end;
*/
#ifdef DEFINIR
#define consp(x) (((x) & 2) != 0)
#endif

logical econsp(x) sexpr x;
{
/*
  logical r;
  sexpr a;
*/
  if (!consp(x)) return (0==1);
  return !eq(car(x),symb);
/*
  a=car(x);
  r=eq(a,symb);
  return r;
*/
}

/*
function typ (x: integer) : integer;
begin
  typ := (x and 240 (* 11110000 *)) div 16;
end;
*/

#define typ(x) (((x) & 240) >> 4)

#ifdef DEFINIR
/*
function val (x: integer) : integer;
begin
  val := ((x and -256) div 256) (* and 255 *) ;
end;
*/

/*
function valp (x: integer) : integer;
begin
  valp := val(x) and 255;
end;
*/

/*
function def (x: integer) : integer;
begin
  def := car(cdr(cdr(x)));
end;
*/
#endif

#define def(x) (car(cdr(cdr(x))))

#ifdef DEFINIR
/*
sexpr append (sexpr x, sexpr y)
begin
  if (not consp(x)) then return y;
  else return cons(car(x),append(cdr(x),y));
end;
*/

#endif

sexpr append (sexpr x, sexpr y)
{
sexpr ptr, r, c;
   if (not consp(x)) then return y;
   else
   {
      r=cons(car(x),nul);
      ptr=r;
      x=cdr(x);
      while (consp(x))
      {
	 c=cons(car(x),nul);
	 rplacd(ptr,c);
	 ptr=c;
	 x=cdr(x);
      }
      rplacd(ptr,y);
      return r;
   }
}


procedure afse1(integer l, sexpr x);

procedure afse(sexpr x)
  begin
    afse1(val(tabchamp[champ_nivafse]),x);
  end;

procedure afse1(integer l, sexpr x)
{
  sexpr pname;
  if (l<0) then tyo('~'); else
  if ((x & 2)==2) then
  begin
/*
    writeln('x=',x,' car(x)=',car(x),' eq->',eq(car(x),symb));
    writeln('val->',val(tabchamp[18]));
*/
    if (eq(car(x),symb) && ((val(tabchamp[18]) & 2)!=0)) then
    begin
      tyo('<');
      pname=car(cdr(x));
      while ((pname & 2) !=0) do
	begin
	  tyo(chr(val(car(pname))));
	  pname=cdr(pname);
	end;
      tyo('>');
    end
    else
    begin
      tyo('.');
      afse1(l-1,car(x));
      afse1(l-1,cdr(x));
    end
  end
  else begin
      tyo('#');
      tyo(chr(x /* mod 256*/ & 0xFF)); /* typ(x) -> flags(x) */
      tyo(chr(val(x)));
  end;
end;

tyo1 (char x)
{
char chiffre_hex[]="0123456789ABCDEF";
   if ((' '<=x) && (x<0xFF))
   {
      if (x=='/') tyo('/');
      tyo(x);
   }
   else
   {
      tyo('/');
      tyo(chiffre_hex[(x>>4)&0xF]);
      tyo(chiffre_hex[x&0xF]);
   }
}

print1 (sexpr x)
{
   if ((x & 0xF0) == 0)
   {
      if ((x & 0xFFF6) == 0x0000)
      {
	 tyo('#'); tyo('N');
      }
      else if ((x & 0xFFF6) == 0x0004)
      {
	 tyo('#'); tyo('T');
      }
      else if ((x & 0xFFF6) == 0x0E04)
      {
	 tyo('#'); tyo('S');
      }
      else goto suite;
   }
   else if ((x & 0xF0) == 0x10)
   {
      tyo('#'); tyo('I');
      tyo1(x>>8);
   }
   else if ((x & 0xF0) == 0x20)
   {
      tyo('#'); tyo('C');
      tyo1(x>>8);
   }
   else
   {
   suite:
      tyo('#'); tyo('X');
      tyo1(x>>8);
      tyo1(x&0xFF);
   }
}

procedure top_debug()
{

  char c;
  tyo(chr(13)); tyo(chr(10)); tyo('P'); tyo('=');
  afse(tabchamp[champ_prog]);
  tyo(chr(13)); tyo(chr(10)); tyo('B'); tyo('=');
  afse(tabchamp[champ_bas]);
  tyo(chr(13)); tyo(chr(10));
  c=chr(10);
  while (c==chr(10))
/*
  or (c=chr(13)) *) do (* c:=tyi; *)
    (* c:=chr(13); *)
*/
  /* read(c); */
  read(0,&c,1);
  if (c=='e') then tabchamp[champ_prog]=cons(lise(),tabchamp[champ_prog]);
}


procedure evol()
{
  sexpr x;
  if (trace & 32)
  {
  FILE *f;
     n_evol++;
     f_evol = fopen("lpia.evol","r+");
     fseek(f_evol,0,SEEK_SET);
     fprintf(f_evol,"%d\n",n_evol);
     fclose(f_evol);
     if (n_evol == n_break)
	tabchamp[champ_trace] |= 16 << 8;
  }
  if ((trace & 16) == 16) then top_debug();
  x=car(tabchamp[champ_prog]);
  if (not consp(x))
  then
	 if (typ(x) == type_as) /* 0 */
		then tabchamp[champ_prog] = cdr(tabchamp[champ_prog]);
	 else if (typ(x) != type_instr) /* 1 */
		then begin
		  tabchamp[champ_bas] = cons(x,tabchamp[champ_bas]);
		  tabchamp[champ_prog] = cdr(tabchamp[champ_prog]);
		end
	 else /* instr */ begin
		tabchamp[champ_prog] = cdr(tabchamp[champ_prog]);
		if ((trace & 2)==2) then putchar(chr(val(x)));
		instr(val(x));
	 end
  else /* cons */
	 /* if car(x) <> symb */
	 if (not eq(car(x),symb))
	 then tabchamp[champ_prog] = append(x,cdr(tabchamp[champ_prog]));
	 else tabchamp[champ_prog] = cons(def(x),cdr(tabchamp[champ_prog]));
end;

procedure interp()
begin
  while (0==0) do
  begin
	 evol();
	 valid();
	 /* if fich[3]<>'EB7.LPI' then begin writeln('***'); readln; end
	  */
  end;
end;

procedure rievol(integer lim_nse)
{
/*
var svchamp : array [0 .. maxchamp] of integer;
	 ctx, i, j: integer;
begin
(*  svchamp := tabchamp; *)
*/
  sexpr svchamp[maxchamp+1];
  sexpr ctx;
  integer i,j;
  for (j=/* 0 */ champ_strat; j<=maxchamp; j++)
    svchamp[j] = tabchamp[j];
  ctx = car(tabchamp[champ_bas]);
  i = champ_strat;
  while ((i<=maxchamp) && (consp(ctx))) do
  begin
	 tabchamp[i]=car(ctx);
	 ctx=cdr(ctx);
	 i=i+1;
  end;
  evol();
  while ((tabvarsys[varsys_pnse] < lim_nse) &&
	 !eq(car(tabchamp[champ_prog]),stop))
    evol();
  ctx=nul;
  for (i=maxchamp; i>=champ_strat; i--) do
	 ctx=cons(tabchamp[i],ctx);
  for (j= /* 0 */ champ_strat; j<=maxchamp; j++) do tabchamp[j] = svchamp[j];
  tabchamp[champ_bas]=cons(ctx,cdr(tabchamp[champ_bas]));
end;

/*
function prem (champ: integer) : integer;
begin
  prem := car(tabchamp[champ]);
end;
*/

#define prem(champ) (car(tabchamp[champ]))

sexpr enl (register integer champ)
begin
  register sexpr r;
  r = prem(champ);
  tabchamp[champ] = cdr(tabchamp[champ]);
  return r;
end;

/*
procedure aj (champ, x: integer);
begin
  tabchamp[champ] := cons(x,tabchamp[champ]);
end;
*/
/* define aj(champ,x) tabchamp[champ]=cons(x,tabchamp[champ]) */

aj(integer champ, sexpr x)
{
  tabchamp[champ]=cons(x,tabchamp[champ]);
}

sexpr lnot (register sexpr x)
begin
  if ((x & 2)==2) then return nul; else
  if ((x & 4) == 0)
  then return x | 4;
  else return x & -5;
end;

/*
function verite (x: integer) : boolean;
begin
  verite := ((x and 2) = 2) or ((x and 4) != 0);
end;
*/
/* define verite(x) ((((x) & 2) == 2) || (((x) & 4) != 0)) */
logical verite(register sexpr x)
{
   return ((x & 2) == 2) || ((x & 4) != 0);
}

/*
function llogique (l: boolean) : integer;
begin
  if l
	 then llogique:=true
	 else llogique:=nul;
end;
*/

#define llogique(l) ((l)?true:nul)

procedure drop (integer x)
begin
end;

/*
function lint (n: integer) : integer;
begin
	lint := n*256 + 36;
end;
*/

/*
function hlc(x: integer): integer;
begin
  hlc:=hl(val(car(x)),val(cdr(x)));
end;
*/

/* define hlc(x) (hl(val(car(x)),val(cdr(x)))) */

integer hlc(sexpr x)
{
  return hl(val(car(x)),val(cdr(x)));
}

/*
function chl(x: integer): integer;
begin
  chl:=cons(lint(x div 256),lint(x mod 256));
end;
*/
#define chl(x) (cons(lint((x) >> 8),lint((x) & 0xFF)))

#define i_length ((char)0x91)
integer length (register sexpr l)
{
  if (!consp(l)) return 0;
  return 1+length(cdr(l));
}

#define i_last ((char)0x92)
sexpr last(x) sexpr x;
{
  if (!consp(x)) return nul;
  if (!consp(cdr(x))) return x;
  return last(cdr(x));
}

#define i_reverse ((char)0x93)
sexpr reverse(l) sexpr l;
{
  if (!consp(l)) return l;
  return append(reverse(cdr(l)),cons(car(l),nul));
}

#define i_memq ((char)0x94)
sexpr memq(x,l) sexpr x,l;
{
  if (!consp(l)) return nul;
  if (eq(x,car(l))) return l;
  return memq(x,cdr(l));
}

#define i_equal ((char)0x95)
logical equal(x,y) sexpr x,y;
{
  if (eq(x,y)) return x;
  if (consp(x)!=consp(y)) return faux;
  if (!consp(x)) return eq(x,y);
  return equal(car(x),car(y)) && equal(cdr(x),cdr(y));
}

#define i_member ((char)0x96)
sexpr member(x,l) sexpr x,l;
{
  if (!consp(x)) return nul;
  if (equal(x,car(l))) return l;
  return member(x,cdr(l));
}

#define i_separer ((char)0x97)
sexpr separer(n,l) integer n; sexpr l;
{
  sexpr r;
  if (n==0) return cons(nul,l);
  r=separer(n-1,cdr(l));
  return cons(cons(car(l),car(r)),cdr(r));
}

#define i_remq ((char)0x98)
sexpr remq(x,l) sexpr x,l;
{
  if (!consp(l)) return l;
  if (eq(car(l),x)) return cdr(l);
  return cons(car(l),remq(x,cdr(l)));
}

#define i_remov ((char)0x99)
sexpr remov(x,l) sexpr x,l;
{
  if (!consp(l)) return l;
  if (equal(car(l),x)) return cdr(l);
  return cons(car(l),remov(x,cdr(l)));
}

#define i_inclq ((char)0x9A)
logical inclq(a,b) sexpr a,b;
{
  sexpr x;
  if (!consp(a)) return vrai;
  x=memq(car(a),b);
  return verite(x) && inclq(cdr(a),b);
}

#define i_incl ((char)0x9B)
logical incl(a,b) sexpr a,b;
{
  sexpr x;
  if (!consp(a)) return vrai;
  x=member(car(a),b);
  return verite(x) && incl(cdr(a),b);
}

#define ENV_VIDE true

#define i_getvenv ((char)0xA1)
sexpr getvenv(env,var) sexpr env,var;
{
  if (!consp(env)) return nul;
  if (eq(car(car(env)),var)) return car(cdr(car(env)));
  return getvenv(cdr(env),var);
}

#define i_setvenv ((char)0xA2)
sexpr setvenv(env,var,val) sexpr env,var,val;
{
  if (!consp(env)) return cons(cons(var,cons(val,nul)),ENV_VIDE);
  if (eq(car(car(env)),var)) return cons(cons(var,cons(val,
				      cdr(cdr(car(env))))),cdr(env));
  return cons(car(env),setvenv(cdr(env),var,val));
}

#define i_addvenv ((char)0xA3)
sexpr addvenv(env,var,val) sexpr env,var,val;
{
  if (!consp(env)) return cons(cons(var,cons(val,nul)),ENV_VIDE);
  if (eq(car(car(env)),var)) return cons(cons(var,cons(val,
				      cdr(car(env)))),cdr(env));
  return cons(car(env),addvenv(cdr(env),var,val));
}

#define i_remvenv ((char)0xA4)
sexpr remvenv(env,var) sexpr env,var;
{
  if (!consp(env)) return env;
  if (eq(car(car(env)),var))
  {
    sexpr x;
    x=cdr(cdr(car(env)));
    if (!tabvarsys[VARSYS_GARDER_VARS] && !consp(x)) return cdr(env);
    return cons(cons(var,x),cdr(env));
  }
  return cons(car(env),remvenv(cdr(env),var));
}

#define QUOTE 10005

#define i_getvsenv ((char)0xA9)
sexpr getvsenv(env,vars) sexpr env,vars;
{
  if (!econsp(vars)) return getvenv(env,vars);
  if (eq(car(vars),QUOTE) &&  consp(cdr(vars))) return car(cdr(vars));
  return cons(getvsenv(env,car(vars)),getvsenv(env,cdr(vars)));
}

#define i_bindvenv ((char)0xAB)
sexpr bindvenv(env,vars,vals) sexpr env,vars,vals;
{
  sexpr a,b;
  if (!verite(vars)) return env;
  if (!econsp(vars)) return addvenv(env,vars,vals);
  a=consp(vals)?car(vals):vals;
  b=consp(vals)?cdr(vals):vals;
  return bindvenv(bindvenv(env,car(vars),a),cdr(vars),b);
}

#define i_unbindvenv ((char)0xAC)
sexpr unbindvenv(env,vars) sexpr env,vars;
{
  sexpr a,b;
  if (!verite(vars)) return env;
  if (!econsp(vars)) return remvenv(env,vars);
  return unbindvenv(unbindvenv(env,car(vars)),cdr(vars));
}

#define i_creer_symbole ((char)0xB1)
sexpr creer_symbole(s) char *s;
{
  sexpr pname;
  for (pname=nul; s[0]!=0; s++)
    pname=cons(lint(s[0]),pname);
  pname=reverse(pname);
  return cons(symb,cons(pname,cons(nul,cons(nul,nul))));
}

#define VAR 0x0104
#define NOM_VAN nul
sexpr VAN;

#define CREER_VAR(x) (cons(VAR,cons(x,nul)))
#define NOM_VAR(x) (car(cdr(x)))
#define EST_VAR(x) (consp(x) && eq(VAR,car(x)))

#define EST_VAN(x) (EST_VAR(x) && eq(nul,NOM_VAR(x)))

#define i_gensym ((char)0xB2)
char carac_gensym='0';
sexpr gensym()
{
  char s[6];
  /* s="GEN- "; */
  s[0]='G'; s[1]='E'; s[2]='N'; s[3]='-';
  s[4]=carac_gensym++;
  s[5]=0;
  return creer_symbole(s);
}

#define UNDEF 0xFF00

#define i_boundvenv ((char)0xB3)
logical boundvenv (env,var) sexpr env,var;
{
  if (!consp(env)) return faux;
  if (eq(car(car(env)),var)
      && consp(cdr(car(env)))
      && !eq(car(cdr(car(env))),UNDEF)) return vrai;
  return boundvenv(cdr(env),var);
}

#define i_getrecvenv ((char)0xB4)
sexpr getrecvenv(env,var) sexpr env,var;
{
  sexpr val;
  if (!boundvenv(env,var)) return CREER_VAR(var);
  val=getvenv(env,var);
  if (!EST_VAR(val)) return val;
  return getrecvenv(env,NOM_VAR(val));
}

#define i_ensemble ((char)0xB5)
sexpr ensemble(l) sexpr l;
{
  if (!consp(l)) return l;
  if (verite(memq(car(l),cdr(l)))) return ensemble(cdr(l));
  return cons(car(l),ensemble(cdr(l)));
}

#define i_variables1 ((char)0xB6)
sexpr variables1(x) sexpr x;
{
  if (EST_VAR(x)) return cons(NOM_VAR(x),nul);
  if (!econsp(x)) return nul;
  return append(variables1(car(x)),variables1(cdr(x)));
}

#define i_variables ((char)0xB7)
sexpr variables(x) sexpr x;
{
  return ensemble(variables1(x));
}

#define i_subst ((char)0xB8)
sexpr subst(x,y,z) sexpr x,y,z;
{
  if (eq(y,z)) return x;
  if (!econsp(z)) return z;
  return cons(subst(x,y,car(z)),subst(x,y,cdr(z)));
}

#define i_renommer1 ((char)0xB9)
sexpr renommer1(x,vars) sexpr x,vars;
{
  if (!econsp(vars)) return x;
  return renommer1(subst(gensym(),car(vars),x),cdr(vars));
}

#define i_renommer ((char)0xBA)
sexpr renommer(x) sexpr x;
{
  return(renommer1(x,variables(x)));
}

#define i_construire ((char)0xBB)
sexpr construire(env,x) sexpr env,x;
{
  if (EST_VAR(x)) return getrecvenv(env,NOM_VAR(x));
  if (!econsp(x)) return x;
  return cons(construire(env,car(x)),
	      construire(env,cdr(x)));
}

#define LIER(env,var,val) ((tabvarsys[VARSYS_ADDV]?addvenv:setvenv) (env,var,val))

#define i_unifier1 ((char)0xBC)
sexpr unifier1();

#define i_unifier ((char)0xBD)
sexpr unifier(env,x,y) sexpr env,x,y;
{
  if (!verite(env)) return env;
  return unifier1(env,construire(env,x),construire(env,y));
}

sexpr unifier1(env,x,y) sexpr env,x,y;
{
  if (EST_VAN(x) || EST_VAN(y) || equal(x,y)) return env;
  if (EST_VAR(x) && (!tabvarsys[VARSYS_OCCUR_CHECK]
		     || !verite(memq(NOM_VAR(x),variables(y)))))
    return LIER(env,NOM_VAR(x),y);
  if (EST_VAR(y) && (!tabvarsys[VARSYS_OCCUR_CHECK]
		     || !verite(memq(NOM_VAR(y),variables(x)))))
    return LIER(env,NOM_VAR(y),x);
  if (!econsp(x) || !econsp(y)) return nul;
  return unifier(unifier1(env,car(x),car(y)),cdr(x),cdr(y));
}

#define STRAT car
#define PROG(ctx) (car(cdr(ctx)))
#define BAS(ctx) (car(cdr(cdr(ctx))))

#define i_enfiler ((char)0xC1)
enfiler(prio,ctx,file) sexpr prio,ctx,file;
{
  if (!verite(prio))
  {
    sexpr x;
    x=last(file);
    cdr(x)=cons(ctx,nul);
    return;
  }
  if (val(car(STRAT(car(file)))) < val(car(STRAT(ctx))))
  {
    rplacd(file,cons(car(file),cdr(file)));
    rplaca(file,ctx);
    return;
  }
  if (!verite(cdr(file)))
  {
    rplacd(file,cons(ctx,nul));
    return;
  }
  enfiler(prio,ctx,cdr(file));
}

#define i_instr_send ((char)0xC2)
sexpr instr_send (ctx) sexpr ctx;
{
  sexpr canal, x, flag, file, prio, ctxc;
  canal=car(BAS(ctx));
  x=car(cdr(BAS(ctx)));
  if (!verite(car(cdr(cdr(canal)))))
  {
    rplacd(canal,cons(true,cons(cons(ctx,nul),
	   cons(car(cdr(cdr(cdr(canal)))),nul))));
    return true;
  }
  flag=car(cdr(canal));
  file=car(cdr(cdr(canal)));
  prio=car(cdr(cdr(cdr(canal))));
  if (verite(flag))
  {
    enfiler(prio,ctx,file);
    return true;
  }
  rplaca(cdr(cdr(canal)),cdr(file));
  ctxc=car(file);
  return cons(
	      cons(STRAT(ctxc),
		cons(PROG(ctxc),
		  cons(cons(x,cdr(BAS(ctxc))),
		    cdr(cdr(cdr(ctxc))) ))),
		cons(
		   cons(STRAT(ctx),
		     cons(PROG(ctx),
		       cons(cdr(cdr(BAS(ctx))),
			 cdr(cdr(cdr(ctx))) ))),
		  nul ));
}

#define i_instr_receive ((char)0xC3)
sexpr instr_receive (ctx) sexpr ctx;
{
  sexpr canal, xc, flag, file, prio, ctxc;
  canal=car(BAS(ctx));
  if (!verite(car(cdr(cdr(canal)))))
  {
    rplacd(canal,cons(nul,cons(cons(ctx,nul),
	   cons(car(cdr(cdr(cdr(canal)))),nul))));
    return true;
  }
  flag=car(cdr(canal));
  file=car(cdr(cdr(canal)));
  prio=car(cdr(cdr(cdr(canal))));
  if (!verite(flag))
  {
    enfiler(prio,ctx,file);
    return true;
  }
  rplaca(cdr(cdr(canal)),cdr(file));
  ctxc=car(file);
  xc=car(cdr(BAS(ctxc)));
  return cons(
	      cons(STRAT(ctxc),
		cons(PROG(ctxc),
		  cons(cdr(cdr(BAS(ctxc))),
		    cdr(cdr(cdr(ctxc))) ))),
		cons(
		   cons(STRAT(ctx),
		     cons(PROG(ctx),
		       cons(cons(xc,cdr(BAS(ctx))),
			 cdr(cdr(cdr(ctx))) ))),
		  nul ));

}

#define i_getvq ((char)0xE1)
#define i_setvq ((char)0xE2)
#define i_getvsq ((char)0xE9)
#define i_bindvq ((char)0xEB)
#define i_unbindvq ((char)0xEC)
#define i_arg ((char)0xE6)

typedef void far *fonction();

call(f) void far *f();
{
  (*f)();
}

#define sexpr_instr(x) (((x) << 8) | 0x14)

#define tbuf_execlm 257
#define maxbuf 128
procedure instr (integer ni)
{
/*
var x, y, r, marque, n, h, l, i: integer;
  buf : (* array [0..maxbuf] of char; *)
		  string[maxbuf];
begin
*/
  integer x,y,z,r,marque,n,h,l,i;
  char buf[maxbuf];
  if (ni=='o')
  {
  int x;
     x=0;
  }
  switch (chr(ni))
  {
	case ievi : aj(champ_bas,enl(champ_prog));
/*
	break;
	case 0xFE:
	  printf("bizarre!");
*/
	break; case ieval : aj(champ_prog,enl(champ_bas));
	break; case ievol : rievol(val(tabchamp[champ_limnse]));
	break; case ievol_limnse : rievol(val(enl(champ_bas)));
	break; case idep : drop(enl(champ_bas));
	break; case irep : aj(champ_bas,prem(champ_bas));
	break; case iech : begin
				 x=enl(champ_bas);
				 y=enl(champ_bas);
				 aj(champ_bas,x);
				 aj(champ_bas,y);
			  end;
	break; case ides : aj(champ_haut,enl(champ_bas));
	break; case imon : aj(champ_bas,enl(champ_haut));
	break; case inot : aj(champ_bas,lnot(enl(champ_bas)));
	break; case ithen : if (verite(enl(champ_bas)))
				  then begin
					 x=enl(champ_prog);
					 drop(enl(champ_prog));
					 aj(champ_prog,x);
				  end
				  else drop(enl(champ_prog));
	break; case ieq : aj(champ_bas,llogique(eq(enl(champ_bas),enl(champ_bas))));
	break; case iplus : begin
				  x=enl(champ_bas);
				  y=enl(champ_bas);
				  r=valp(x)+valp(y);
			/*    if (r>127) then r=r-256;
				  if r<-128 then r:=r+256;
			*/
				  if (r>255) then begin r=r-256; marque=1; end else marque=0;
				  aj(champ_bas,hl(r,typ(y)*16+marque*8+4));
				end;
	 break; case imns : begin
				  x=enl(champ_bas);
				  y=enl(champ_bas);
				  r=valp(y)-valp(x);
				  if (r<0) then begin r=r+256; marque=1; end else marque=0;
				  aj(champ_bas,hl(r,typ(y)*16+marque*8+4));
				end;
	 break; case ietl : begin
		   x=enl(champ_bas);
		   y=enl(champ_bas);
		   r=valp(x) & valp(y);
		   aj(champ_bas,hl(r,typ(y)*16+4));
		 end;
	 break; case idiv2 : aj(champ_bas,lint(val(enl(champ_bas) >> 1)));
	  /* autres instructions :
			 car, cdr, cons, rpa, rpd,
			 getf, setf,
			 posp,
			 gf, gv,
			 tys, tyi, tyo, rdca, lica, lise,
			 peek, poke, usr.
		 */
	 break; case icar : aj(champ_bas,car(enl(champ_bas)));
	 break; case icdr : aj(champ_bas,cdr(enl(champ_bas)));
	 break; case icons : begin
				  x=enl(champ_bas);
				  aj(champ_bas,cons(x,enl(champ_bas)));
				end;
	 break; case irpa : begin
				  x=enl(champ_bas);
				  rplaca(x,enl(champ_bas));
				end;
	 break; case irpd : begin
				  x=enl(champ_bas);
				  rplacd(x,enl(champ_bas));
				end;
	 break; case igetf : aj(champ_bas,tabchamp[val(enl(champ_bas))]);
	 break; case isetf : begin
					x=enl(champ_bas);
					tabchamp[val(x)]=enl(champ_bas);
				 end;
	 break; case iposp : aj(champ_bas,llogique(enl(champ_bas)>=0));
	 break; case igf : aj(champ_bas,lint(enl(champ_bas) & 255));
	 break; case isf : begin
		  x=enl(champ_bas); /* flags */
		  aj(champ_bas,hl(val(enl(champ_bas)),val(x)));
		end;
	 break; case ichsymb : begin
		      x=enl(champ_bas); /* symbs */
		      aj(champ_bas,ch_symb(x,enl(champ_bas)));
		    end;
	 break; case icbk :   x=enl(champ_prog);
			      enl(champ_prog);
			      aj(champ_prog,x);
	 break; case icbs : x=enl(champ_prog);
			    y=enl(champ_prog);
			    z=enl(champ_prog);
			    aj(champ_prog,cons(y,cons(z,nul)));
			    aj(champ_prog,z);
			    aj(champ_prog,x);
	 break; case icby :
		  x=enl(champ_prog);
		  aj(champ_prog,cons(sexpr_instr(icby),cons(x,nul)));
		  aj(champ_prog,x);
	 break; case icbb :
		  x=enl(champ_prog);
		  y=enl(champ_prog);
		  z=enl(champ_prog);
		  aj(champ_prog,cons(y,cons(z,nul)));
		  aj(champ_prog,x);
	 break; case icbc :
		  x=enl(champ_prog);
		  y=enl(champ_prog);
		  z=enl(champ_prog);
		  aj(champ_prog,y);
		  aj(champ_prog,z);
		  aj(champ_prog,x);
	 break; case icbj :
		  enl(champ_prog);
	 break; case i_inclq :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,inclq(x,y)?true:nul);
	 break; case i_incl :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,incl(x,y)?true:nul);
	 break; case i_equal :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,equal(x,y)?true:nul);
	 break; case i_length :
		  aj(champ_bas,lint(length(enl(champ_bas))));
	 break; case i_memq :
		  y=enl(champ_bas);
		  x=enl(champ_bas);
		  aj(champ_bas,memq(x,y));
	 break; case i_member :
		  y=enl(champ_bas);
		  x=enl(champ_bas);
		  aj(champ_bas,member(x,y));
	 break; case i_append :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,append(x,y));
	 break; case i_last :
		  aj(champ_bas,last(enl(champ_bas)));
	 break; case i_reverse :
		  aj(champ_bas,reverse(enl(champ_bas)));
	 break; case i_remq :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,remq(x,y));
	 break; case i_remov :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,remov(x,y));
	 break; case i_getvenv :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,getvenv(x,y));
	 break; case i_setvenv :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  z=enl(champ_bas);
		  aj(champ_bas,setvenv(x,y,z));
	 break; case i_addvenv :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  z=enl(champ_bas);
		  aj(champ_bas,addvenv(x,y,z));
	 break; case i_remvenv :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,remvenv(x,y));
	 break; case i_getvsenv :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,getvsenv(x,y));
	 break; case i_bindvenv :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  z=enl(champ_bas);
		  aj(champ_bas,bindvenv(x,y,z));
	 break; case i_unbindvenv :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,unbindvenv(x,y));
	 break; case i_arg :
		  x=enl(champ_prog);
		  y=enl(champ_prog);
		  z=enl(champ_bas);
		  tabchamp[champ_env]=bindvenv(tabchamp[champ_env],x,z);
		  aj(champ_prog,cons(sexpr_instr(i_unbindvq),cons(x,nul)));
		  aj(champ_prog,y);
	 break; case i_getvq :
		  x=enl(champ_prog);
		  aj(champ_bas,getvenv(tabchamp[champ_env],x));
	 break; case i_getvsq :
		  x=enl(champ_prog);
		  aj(champ_bas,getvsenv(tabchamp[champ_prog],x));
	 break; case i_setvq :
		  x=enl(champ_prog);
		  y=enl(champ_bas);
		  tabchamp[champ_env]=setvenv(tabchamp[champ_env],x,y);
	 break; case i_bindvq :
		  x=enl(champ_prog);
		  y=enl(champ_bas);
		  tabchamp[champ_env]=bindvenv(tabchamp[champ_env],x,y);
	 break; case i_unbindvq :
		  x=enl(champ_prog);
		  tabchamp[champ_env]=unbindvenv(tabchamp[champ_env],x);
	 break; case i_gensym :
		  aj(champ_bas,gensym());
	 break; case i_boundvenv :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,boundvenv(x,y)?true:nul);
	 break; case i_getrecvenv :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,getrecvenv(x,y));
	 break; case i_variables :
		  x=enl(champ_bas);
		  aj(champ_bas,variables(x));
	 break; case i_renommer : /* rename */
		  x=enl(champ_bas);
		  aj(champ_bas,renommer(x));
	 break; case i_construire : /* constr */
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,construire(x,y));
	 break; case i_unifier : /* unif */
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  z=enl(champ_bas);
		  aj(champ_bas,unifier(x,y,z));
	 break; case i_subst :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  z=enl(champ_bas);
		  aj(champ_bas,subst(x,y,z));
	 break; case i_ensemble :
		  x=enl(champ_bas);
		  aj(champ_bas,ensemble(x));
	 break; case i_separer :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  aj(champ_bas,separer(val(x),y));

	 break; case i_enfiler :
		  x=enl(champ_bas);
		  y=enl(champ_bas);
		  z=enl(champ_bas);
		  aj(champ_bas,enfiler(x,y,z));
	 break; case i_instr_send :
		  aj(champ_bas,instr_send(enl(champ_bas)));
	 break; case i_instr_receive :
		  aj(champ_bas,instr_receive(enl(champ_bas)));

	 break; case ityi : aj(champ_bas,lint(ord(tyi())));
	 break; case ityo : tyo(chr(val(enl(champ_bas))));
	 break; case isound: /* sound(hlc(enl(champ_bas))); */
			     {
			       x=hlc(enl(champ_bas));
			       if (x!=0) sound(x);
			       else nosound();
			     }
	 break; case irdca : aj(champ_bas,lint(ord(rdca())));
	 break; case ilica : aj(champ_bas,lint(ord(lica())));
	 break; case ilise : aj(champ_bas,lise());
	 break; case igetsv : begin
					 x = enl(champ_bas);
					 n = tabvarsys[val(x)];
					 h = (n & -256) >> 8;
					 l = n & 255;
					 aj(champ_bas,cons(lint(h),lint(l)));
				  end;
	 break; case isetsv : begin
					 x = enl(champ_bas);
					 y = enl(champ_bas);
					 tabvarsys[val(x)] = hl(val(car(y)),val(cdr(y)));
				  end;
	 break; case ireset : lseek(fich[inp_],0,SEEK_SET); /* reset(fich[inp]); */
	 break; case irewrite : lseek(fich[outp_],0,SEEK_SET); /* rewrite(fich[outp]); */
	 break; case iclose : /*f*/close(fich[val(enl(champ_bas))]);
	 break; case ieof : aj(champ_bas,llogique(eof(fich[inp_])));
	/*iinp : inp:=fich[val(enl(champ_bas))];
	  ioutp : outp:=fich[val(enl(champ_bas))];
	*/
	 break; case iinp : inp_=val(enl(champ_bas));
	 break;
	 case ioutp :
	    outp_=val(enl(champ_bas));
	 break; case iassign : begin
					  /* assign(fichier dont numero sommet pile,nom s.sommet */
					  x=val(enl(champ_bas)); /* numero fichier */
					  y=enl(champ_bas); /* nom fichier, liste caracteres */
					  i = /* 0 */ /* 1 */ 0 ;
					  while ((i<maxbuf) & consp(y)) do
						 begin
							buf[i]=chr(val(car(y)));
							i=i+1;
							y=cdr(y);
						 end;
					  buf[i]=chr(0);
					  /* assign(fich[x],buf); */
					  /* fich[x]=fopen(buf,"b"); */
					  fich[x]=open(buf,O_RDWR | O_BINARY);
					  if (fich[x]==-1)
					    {
					      fich[x]=creat(buf,S_IFREG | S_IREAD | S_IWRITE);
					      fich[x]=open(buf,O_RDWR | O_BINARY);
					    }
					  end;
	  break; case isavecore: save_core();
	  break; case irestorecore: restore_core();
	  break; case ipeek: begin
			       /* segment */ x=hlc(enl(champ_bas));
			       /* offset */  y=hlc(enl(champ_bas));
			       aj(champ_bas,lint(peekb(x,y)));
			     end;
	  break; case ipoke: begin
			       x=hlc(enl(champ_bas));
			       y=hlc(enl(champ_bas));
			       pokeb(x,y,val(enl(champ_bas)));
			     end;
	  break; case iusr: begin
			      long z;
			      x=hlc(enl(champ_bas));
			      y=hlc(enl(champ_bas));
			      /*
			      f=(fonction)((x << 16) & y);
			      f();
			      */
			      z=(x << 16) & y;
			      call(z);
			    end;
	  break; case iexeclm: begin
				 char buf_execlm[tbuf_execlm+1];
				 integer i;
				 x=enl(champ_bas);
				 for (i=0;
				      (i<tbuf_execlm) && ((x & 2) != 0);
				      i++)
				 {
				   buf_execlm[i]=val(car(x));
				   x=cdr(x);
				 }
				 call(buf_execlm);
			       end;
	  break; case iinport: aj(champ_bas,lint(inportb(hlc(enl(champ_bas)))));
	  break; case ioutport: begin
				  x=hlc(enl(champ_bas));
				  outportb(x,val(enl(champ_bas)));
				end;
	  break; case '6': x=enl(champ_bas);
			       print1(x);
	  break;
	    case ((char)0xFF):
	      {int x;
	       x=0;
	       }
	  break; case iintr: begin
		   x=val(enl(champ_bas)); /* numero intr */
		   y=enl(champ_bas); /* registres */
		   reg.ax=hlc(car(y)); y=cdr(y);
		   reg.bx=hlc(car(y)); y=cdr(y);
		   reg.cx=hlc(car(y)); y=cdr(y);
		   reg.dx=hlc(car(y)); y=cdr(y);
		/* reg.bp:=hlc(car(y)); */ y=cdr(y);
		   reg.di=hlc(car(y)); y=cdr(y);
		   reg.si=hlc(car(y)); y=cdr(y);
		   sreg.ds=hlc(car(y)); y=cdr(y);
		   sreg.es=hlc(car(y)); y=cdr(y);
		   reg.
#ifdef MICROSOFT_C
		       cflag
#endif
#ifdef TURBO_C
		       flags
#endif
			     =hlc(car(y));
		   int86x(x,&reg,&reg,&sreg);
		   y=cons(chl(reg.
/*#ifdef MICROSOFT_C
*/
				  cflag
/*#endif
#ifdef TURBO_C
				  flags
#endif*/
				       ),y);
		   y=cons(chl(sreg.es),y);
		   y=cons(chl(sreg.ds),y);
		   y=cons(chl(reg.si),y);
		   y=cons(chl(reg.di),y);
		   y=cons(chl(0 /* reg.bp */),y);
		   y=cons(chl(reg.dx),y);
		   y=cons(chl(reg.cx),y);
		   y=cons(chl(reg.bx),y);
		   y=cons(chl(reg.ax),y);
		   aj(champ_bas,y);
		 end;
	end;
end;

lpia ()
begin
  reg.ax=256;
  reg.cx=1*16*16*16+2*16*16+3*16+4;
  int86(16,&reg,&reg);
/*
  buffer:='abcdef';
*/
  init();
  interp();
end

main ()
{
   lpia();
}










									      

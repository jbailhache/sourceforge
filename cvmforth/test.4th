
: suc 1 + ;
5 suc .

3 4 + .
18 10 - .
11 29 AND .
12 6 XOR .

: CONSTANT CREATE , DOES> @ ;
10 CONSTANT DIX
1 DIX + .

: testif 
 0 IF 999 . THEN
 1 IF 12 . THEN ;
testif

: testim 14 . [ 13 . ] 15 . ;
testim

: plus + ;
: testex 10 6 ' plus EXECUTE . ;
testex

: testim1 17 . ; IMMEDIATE
: testim2 testim1 ;

10 EMIT 13 EMIT
BYE

# -*- coding: iso-8859-15 -*-
import os
import sys
os.chdir('\\carte m�moire\\python')
execfile('load.py')
cours=lirecourslf(['\\carte m�moire\\bourse\\cours\\srd-0911.xls'])
execfile('lc.py')
#print(valor(pf))
#sys.stdin.readline()
while 1:
 a=raw_input()
 if a=='':
  break
 graphid(a)




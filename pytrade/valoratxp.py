# -*- coding: iso-8859-15 -*-
import os
import sys
# os.chdir('\\carte m�moire\\python')
execfile('load.py')
execfile('po_345_3_20.py')
# cours=lirecourslf(['\\carte m�moire\\bourse\\cours\\srd-0910.xls','\\carte m�moire\\bourse\\cours\\srd-0911.xls'])
cours=lirecourslf(['\\jacques\\bourse\\cours\\srd-0910.xls','\\jacques\\bourse\\cours\\srd-0911.xls'])
execfile('lc.py')
print(valor(pf,1))
#sys.stdin.readline()
# while 1:
#  x=0



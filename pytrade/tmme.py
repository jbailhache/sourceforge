

from Tkinter import *

wd=800
hg=500

v = -1

def depl(d):
  global v,l,w,c
  # print 'len(c)=', len(c), ' v=',v,' d=',d,
  if v+d >= 0 and v+d < len(c):
    # print ' ok ',
    v=v+d
  print ' v=',v,
  if len(c[v])>0:
    if len(c[v][0])>1:
      l['text']=c[v][0][1]
  w.create_rectangle(1,1,wd-1,hg-1,fill="yellow")
  max=0
  min=1000000000 
  a = c[v]
  nj = len(a)
  # j0 = nj/2
  j0 = 230
  # j0 = 520
  x = j0 * wd / len(a)
  w.create_line (x, 1, x, hg-1, fill='black')
  for j in range(nj):
    if len(c)>v:
      if len(c[v])>j:
        if len(c[v][j])>7:
          if (c[v][j][5]<min): min=c[v][j][5]
          if (c[v][j][4]>max): max=c[v][j][4]
  if len(c[v])>0:
    if (len(c[v][0])>1):
      lmax['text'] = max
      lmin['text'] = min
      lnj['text'] = nj
  for t in range(1,len(a)-1):
    tp=t-1
    xp=tp*wd/len(a)
    x=t*wd/len(a)
    if t % 10 == 0:
      if t % 100 == 0:
        couleur = 'black'
      else:
        couleur = 'grey'
      w.create_line (x, 1, x, hg, fill=couleur)
    # for i in range(len(a[t])): 
    i = 6
    vp=a[tp][i]
    yp=hg-(vp-min)*hg/(max-min)
    # print ' vp=', vp, ' hg=', hg, ' min=', min, ' max=', max, ' yp=', yp
    clo=a[t][i]
    y=hg-(clo-min)*hg/(max-min)
    # w.create_line(xp,yp,x,y,fill=colors[i])
    w.create_line(xp,yp,x,y,fill='blue')
    # print ' xp=', xp, ' x=', x, ' yp=', yp, ' y=', y
  y = 0
  for p in range(1,j0):
    pp = p-1
    b = 2 / float(p+1)
    m = a[0][6]
    for u in range(j0):
      m = b * a[u][6] + (1-b) * m
    x = (j0-p)*wd/len(a)
    xp = (j0-pp)*wd/len(a)
    if y==0:
      yp = 0
    else: 
      yp = y
    y = hg - (m - min) * hg / (max - min)
    if yp == 0:
      yp = y
    w.create_line (xp, yp, x, y, fill='red')
  for p in range(1,nj-j0):
    pp=p-1
    b = 2/float(p+1)
    m=a[nj-1][6]
    for u in range(j-j0):
      m = b * a[nj-1-u][6] + (1-b)*m
    x = (j0+p)*wd/len(a)
    xp = (j0+pp)*wd/len(a)
    if y==0:
      yp = 0
    else: 
      yp = y
    y = hg - (m - min) * hg / (max - min)
    if yp == 0:
      yp = y
    w.create_line (xp, yp, x, y, fill='red')

#  for j in range(nj):
#       w.create_line (dx*j+12, (max-c[v][j][4])*(hg-10)/(max-min), 5*j+12, (max-c[v][j][5])*(hg-10)/(max-min), fill="blue")
#    w.create_line (dx*j+10, (max-c[v][j][3])*(hg-10)/(max-min), 5*j+12, (max-c[v][j][3])*(hg-10)/(max-min), fill="blue")
#    w.create_line (dx*j+12, (max-c[v][j][6])*(hg-10)/(max-min), 5*j+14, (max-c[v][j][6])*(hg-10)/(max-min), fill="blue")    

def suivant ():
  depl(1)

def prec():
  depl(-1)

def affcours(c1):
  global w,l,lmax,lmin,bp,b,v,c,lnj
  c=c1
  master=Tk()
  w=Canvas(master,width=wd,height=hg)
  w.pack(side=LEFT)
  w.create_rectangle(1,1,wd-1,hg-1,fill="yellow")
  l=Label(master,text="")
  l.pack()
  lmax=Label(master,text="")
  lmax.pack()
  lmin=Label(master,text="")
  lmin.pack()
  bp=Button(master,text="Precedent",command=prec)
  bp.pack()
  b = Button (master, text="Suivant", command=suivant)
  b.pack()
  lnj = Label(master,text="")
  lnj.pack()
  v=-1
  mainloop()



#!/usr/bin/env python

#create png files
import sys
filename = sys.argv[1]
lines = [x.strip() for x in file(filename, 'r').readlines()]

def avg(mesh, X, Y):
  s=0
  for x in range(X):
    for y in range(Y):
      s+=abs(mesh[x][y])
  return float(s)/(X*Y)

def writeFile(mesh, num):
  import PIL
  import Image
  X = len(mesh);Y=len(mesh[0])
  img = Image.new('RGB', (X,Y))
  sumc=0

  cs=[]
  print "avg=",avg(mesh, X, Y)
  for i in range(X):
    prev_v = 0
    cs.append([])
    for j in range(Y):
      cs[-1].append(0)
      #c = min(max(abs(int(mesh[i][j]*50)), 0), 255)
      #c = min(abs(int(mesh[i][j]*100)), 255)
      v = int(mesh[i][j]*100)
      if v>prev_v:
        cs[i][j]=128
        prev_v = v
      elif v<prev_v:
        cs[i][j]=255
        prev_v = v
      else:
        pass#c=0
  for j in range(Y):
    prev_v=0;
    for i in range(X):
      v = int(mesh[i][j]*100)
      if v>prev_v:
        cs[i][j]=128
        prev_v = v
      elif v<prev_v:
        cs[i][j]=255
        prev_v = v
      sumc+=v/255.0
      c=cs[i][j]
      img.putpixel( (i,j), (255,c,255-c) )
  img.save("data/img"+str(num)+".png", "PNG")
  return sumc/(X*Y)
 

i = 0
l = 0
while l<len(lines):
  if lines[l] == "omega:":
    mesh = []
    while l<len(lines)-1:
      l+=1
      sp = lines[l].split()
      if len(sp)<10:
        break
      mesh.append(map(float, sp))
    if len(mesh)>0:
      print writeFile(mesh, i)
      i+=1
  l+=1

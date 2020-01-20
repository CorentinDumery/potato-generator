# -*- coding: utf-8 -*-

#potato

from stl import mesh
from math import sin,cos,sqrt
from random import randrange,random
import numpy as np

class Vertex:
    def __init__(self, v3):
        self.x = v3[0]
        self.y = v3[1]
        self.z = v3[2]
        
    def p(self):
        return [self.x,self.y,self.z]
    
    def d(self,v2):
        return sqrt( (self.x-v2.x)**2 +(self.y-v2.y)**2 +(self.z-v2.z)**2) 
    
    def copy(self):
        return Vertex([self.x,self.y,self.z])
        
    def __str__(self): 
        return "V("+str(self.x)[0:5]+","+str(self.y)[0:5]+","+str(self.z)[0:5]+")"

    def __add__(self, v2):
        return Vertex([self.x+v2.x,self.y+v2.y,self.z+v2.z])
    def __sub__(self, v2):
        return Vertex([self.x-v2.x,self.y-v2.y,self.z-v2.z])
    def __truediv__(self, f):
        return Vertex([self.x/f,self.y/f,self.z/f])
    def __rmul__(self, f):
        return Vertex([self.x*f,self.y*f,self.z*f])
    def __mul__(self, f):
        return Vertex([self.x*f,self.y*f,self.z*f])
    def __eq__(self,v2):
        return abs(self.x-v2.x)<epsilon and abs(self.y-v2.y)<epsilon and abs(self.z-v2.z)<epsilon
    def __ge__(self,v2):
        if self.x > v2.x+epsilon:
            return True
        if self.x < v2.x-epsilon:
            return False
        if self.y > v2.y+epsilon:
            return True
        if self.y < v2.y-epsilon:
            return False
        if self.z > v2.z+epsilon:
            return True
        if self.z < v2.z-epsilon:
            return False
        return True
    def __le__(self,v2):
        return (self==v2) or not(self >= v2)
    def __lt__(self,v2):
        return self <= v2 and not(self==v2)
    def dot(self,v2): 
        return self.x*v2.x+self.y*v2.y+self.z*v2.z
    
    def cross(self,v2):
        return Vertex([self.y*v2.z-self.z*v2.y,self.z*v2.x-self.x*v2.z,self.x*v2.y-self.y*v2.x])
    
    def norm(self):
        return sqrt(self.x**2 +self.y**2 +self.z**2)
    
    
def genSph(n=81,m=64): #outputs a disk
    res = []
    scale = 10
    p0 = [0,0,1*scale]
    layers=[[p0]]
    for i in range(1,n+1):
        d = sin(i*3.1415/(n))
        l=[]
        for j in range(m):
            pos = [d*sin(2*j*3.1415/m)*scale,d*cos(2*j*3.1415/m)*scale,cos(i*3.1415/(n))*scale] 
            l.append(pos)
        for j in range(m):
            if i==1:
                res.append([p0,l[(j+1)%m],l[j]])
                continue
            res.append([layers[-1][j],layers[-1][(j+1)%m],l[j]])
            res.append([layers[-1][(j+1)%m],l[(j+1)%m],l[j]])
        layers.append(l)
        
    
    waves = 10
    
    res0 = res
    
    for i in range(100):
        
        # début : petite fréquence grande amplitude
        # fin : grande f petit A
        
        direction = Vertex([randrange(100)-50,randrange(100)-50,randrange(100)-50])
        direction = direction/direction.norm()
        #print(direction)
        #direction=Vertex([0.3,0.8,0])
        r = (random()-0.5)*2
        off = randrange(100)
        #direction = Vertex([1,0,0])
        ampl = (60-i)*scale*randrange(100)/100000
        freq = i*0.01*randrange(50)/(5*scale*2)
        #print("dir amp freq r",direction,ampl,freq,r)
        for j in range(len(res)):
            for k in range(3):
                v = Vertex(res[j][k])
                proj = v.dot(direction)
                #res[j][k]=[v.x + r*sin(off+2*3.14*freq*v.x)*direction.x,v.y + r*sin(off+2*3.14*freq*v.y)*direction.y,v.z + r*sin(off+2*3.14*freq*v.z)*direction.z]
                res[j][k]=[v.x + r*sin(off+2*3.14*freq*proj)*ampl*direction.x,v.y + r*sin(off+2*3.14*freq*proj)*ampl*direction.y,v.z + r*sin(off+2*3.14*freq*proj)*ampl*direction.z]
        if i%10 ==0:
            res0 = res0 + offsetStl(res,i*10,0,0)
    
    sendOut(res)
    #return Mesh(res)
    
def sendOut(outData):
    data = np.zeros(len(outData), dtype=mesh.Mesh.dtype)
    out = mesh.Mesh(data, remove_empty_areas=False)
    out.vectors[:len(outData)] = outData
    out.save('out.stl')
    
def offsetStl(stl,x,y,z):
    res = []
    for t in stl:
        newt = []
        for l in t:
            newt.append([l[0]+x,l[1]+y,l[2]+z])
        res.append(newt)
    return res

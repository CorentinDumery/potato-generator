# -*- coding: utf-8 -*-

#potato

from stl import mesh
from math import sin,cos,sqrt
from random import randrange,random
import numpy as np

## Change the number of perturbations here
numberPerturbations = 100

## Change the amplitude/frequency
## For better results, try to give low amplitude to high frequencies signals
def amplitude(i):
    return (100-i)*randrange(100)/10000

def frequency(i):
    return i*randrange(50)/(10000)

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
    
    
def genSph(n=81,m=64):
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
    
    res0 = res #output res0 to see intermediate steps
    
    for i in range(numberPerturbations):
        direction = Vertex([randrange(100)-50,randrange(100)-50,randrange(100)-50])
        direction = direction/direction.norm()
        r = (random()-0.5)*2
        off = randrange(100) #offset for sinus function
        ampl = amplitude(i) #ampl = (100-i)*scale*randrange(100)/100000
        freq = frequency(i) #freq = i*0.01*randrange(50)/(5*scale*2)
        for j in range(len(res)):
            for k in range(3):
                v = Vertex(res[j][k])
                proj = v.dot(direction)
                res[j][k]=[v.x + r*sin(off+2*3.14*freq*proj)*ampl*direction.x,v.y + r*sin(off+2*3.14*freq*proj)*ampl*direction.y,v.z + r*sin(off+2*3.14*freq*proj)*ampl*direction.z]
        if i%10 ==0:
            res0 = res0 + offsetStl(res,i*10,0,0)
    sendOut(res)
    
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

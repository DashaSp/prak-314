from create import *
from math import *
from random import *
from numpy import *
import turtle


def RU(a,hlu):
    hlu=array([hlu[0],hlu[1],hlu[2]]).transpose()
    ru=array([(cos(a),sin(a),0),
              (-sin(a),cos(a),0),
              (0,0,1)])
    hlu=dot(hlu,ru)
    H,L,U=hlu.transpose()[0],hlu.transpose()[1],hlu.transpose()[2]
    return list(H),list(L),list(U)

def RH(a,hlu):
    hlu=array([hlu[0],hlu[1],hlu[2]]).transpose()
    rh=array([(1,0,0),
              (0,cos(a),-sin(a)),
              (0,sin(a),cos(a))])
    hlu=dot(hlu,rh)
    H,L,U=hlu.transpose()[0],hlu.transpose()[1],hlu.transpose()[2]
    return list(H),list(L),list(U)

def RL(a,hlu):
    hlu=array([hlu[0],hlu[1],hlu[2]]).transpose()
    rl=array([(cos(a),0,-sin(a)),
              (0,1,0),
              (sin(a),0,cos(a))])
    hlu=dot(hlu,rl)
    H,L,U=hlu.transpose()[0],hlu.transpose()[1],hlu.transpose()[2]
    return list(H),list(L),list(U)


def translate(xyz,l,h):
    x=xyz[0] + l*h[0]
    y=xyz[1] + l*h[1]
    z=xyz[2] + l*h[2]

    return x,y,z



def L_horizontal(hlu):
    H=hlu[0]
    [xh,yh,zh]=H
    V=[0,0,1]
    L=[-yh,-xh,0]
    U=[xh*zh,-zh*yh,-xh**2-yh**2]

    return H,L,U


def normalize(vect):
    norm = linalg.norm(vect)
    if norm == 0: 
       return vect
    return vect / norm

def torque(hlu,t):
    H=hlu[0]
    [xh,yh,zh]=H
    [xt,yt,zt]=t
    torq=[yh*zt-zh*yt,zh*xt-xh*zt,xh*yt-yh*xt]

    return torq


def rotation(hlu,u,a):
    u=normalize(u)
    [ux,uy,uz]=u
    c=cos(a)
    s=sin(a)
    R=array([(ux**2*(1-c)+c,ux*uy*(1-c)-uz*s,ux*uz*(1-c)+uy*s),
            (ux*uy*(1-c)+uz*s,uy**2*(1-c)+c,uy*uz*(1-c)-ux*s),
            (ux*uz*(1-c)-uy*s,uy*uz*(1-c)+ux*s,uz**2*(1-c)+c)])
    
    [H,L,U]=hlu
    H=array(H)
    L=array(L)
    U=array(U)
    H=R.dot(H)
    L=R.dot(L)
    U=R.dot(U)
    return list(H),list(L),list(U)
    

def tropism(hlu,t):
    M=torque(hlu,t)
    alpha=linalg.norm(M)
    
    H,L,U = rotation(hlu,M,alpha)
    

    return H,L,U

def draw_leaf(xyz,hlu,word,alphabet):
    modules=word_to_modules(word, alphabet)
    polygon=[]
    xyzf=xyz
    for module in modules:
        
        if module[0]=='G':
            turtle.up()
            h=hlu[0]
            xyzf=translate(xyz,eval(parameters(module)[0]),h)
            xf,yf,zf=xyzf
            turtle.goto(xf,zf)
    
        elif module[0]=='^':
            angle=eval(parameters(module)[0])
            hlu=RU(angle,hlu)

        elif module[0]=='{':
            turtle.begin_fill()
        
        elif module[0]=='}':
            #print(polygon)
            turtle.down()
            for v in polygon :
                turtle.goto(v[0],v[2])
            turtle.end_fill()
            polygon=[]
            
        elif module[0]=='°':
            polygon.append(xyzf)
    turtle.up()


def draw(words,alphabet):
    trunk_color = "#753313"  
    
    turtle.color(trunk_color)
    turtle.width(3)

    sigma=pi/158
    T=[0,0,-0.5]
    e=0.27
    T=list(e*array(T))
    
    
    xyz=(0,0,-200) 
    x,y,z=xyz
    turtle.up()
    turtle.goto(x,z)
    

    teta=pi/8
    HLU=([0,0,1],[-sin(teta),-cos(teta),0],[-cos(teta),sin(teta),0])
    stack=[]
    polygon=[]
    modules_t=word_to_modules(words[0], alphabet)
    
    for module in modules_t :
        turtle.down()
        if module[0]=='F':
            turtle.down()
            H=HLU[0]
            xyz=translate(xyz,eval(parameters(module)[0]),H)
            x,y,z=xyz
            turtle.goto(x,z)
            HLU=tropism(HLU,T)
            
        elif module[0]=='^':
            angle=eval(parameters(module)[0])
            HLU=RU(gauss(angle,sigma),HLU)
        
        elif module[0]=='&':
            angle=eval(parameters(module)[0])
            HLU=RL(gauss(angle,sigma),HLU)
        
        elif module[0]=='|':
            angle=eval(parameters(module)[0])
            HLU=RH(gauss(angle,sigma),HLU)

        elif module[0]=='[':
            stack.append((xyz,HLU))
        
        elif module[0]==']':
            turtle.up()
            xyz=stack[-1][0]
            x,y,z=xyz
            turtle.goto(x, z)
            HLU=stack[-1][1]
            stack=stack[:-1]
            turtle.down()
        
        elif module[0]=='!':
            turtle.width(eval(parameters(module)[0]))

        elif module[0]=='$':
            HLU=L_horizontal(HLU)
        
        if module[0]=='G':
            #turtle.up()
            turtle.down()
            H=HLU[0]
            xyz=translate(xyz,eval(parameters(module)[0]),H)
            x,y,z=xyz
            turtle.goto(x,z)
    
        elif module[0]=='{':
            turtle.begin_fill()
        
        elif module[0]=='}':
            turtle.down()
            for v in polygon :
                turtle.goto(v[0],v[2])
            turtle.end_fill()
            polygon=[]

        elif module[0]=='°':
            polygon.append(xyz)

    
        elif module[0]=='L':
            draw_leaf(xyz,HLU,words[1],alphabet)

def exporteps(name):
    ts=turtle.getscreen()
    ts.getcanvas().postscript(file =name, colormode = 'color')

N=5
NAME='Tree'
AXIOME_T,AXIOME_L=AXIOMES
PRODUCTION_T,PRODUCTION_L=PRODUCTIONS
PATTERNS=[parametric_word(AXIOME_T,PRODUCTION_T,ALPHABET,N),parametric_word(AXIOME_L,PRODUCTION_L,ALPHABET,N+4)] 
turtle.reset()
turtle.hideturtle()
draw(PATTERNS, ALPHABET) 
exporteps(name=NAME+'.ps') 

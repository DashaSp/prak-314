import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from create import *
from math import *
from random import *
import numpy as np
from numpy import linalg

def RU(a, hlu):
    hlu = np.array([hlu[0], hlu[1], hlu[2]]).transpose()
    ru = np.array([[cos(a), sin(a), 0],
                  [-sin(a), cos(a), 0],
                  [0, 0, 1]])
    hlu = np.dot(hlu, ru)
    H, L, U = hlu.transpose()[0], hlu.transpose()[1], hlu.transpose()[2]
    return list(H), list(L), list(U)

def RH(a, hlu):
    hlu = np.array([hlu[0], hlu[1], hlu[2]]).transpose()
    rh = np.array([[1, 0, 0],
                  [0, cos(a), -sin(a)],
                  [0, sin(a), cos(a)]])
    hlu = np.dot(hlu, rh)
    H, L, U = hlu.transpose()[0], hlu.transpose()[1], hlu.transpose()[2]
    return list(H), list(L), list(U)

def RL(a, hlu):
    hlu = np.array([hlu[0], hlu[1], hlu[2]]).transpose()
    rl = np.array([[cos(a), 0, -sin(a)],
                  [0, 1, 0],
                  [sin(a), 0, cos(a)]])
    hlu = np.dot(hlu, rl)
    H, L, U = hlu.transpose()[0], hlu.transpose()[1], hlu.transpose()[2]
    return list(H), list(L), list(U)


def normalize(vect):
    norm = linalg.norm(vect)
    if norm == 0: 
       return vect
    return vect / norm

def torque(hlu, t):
    H = np.array(hlu[0])
    t = np.array(t)
    return np.cross(H, t)

def rotation(hlu, u, a):
    u = normalize(u)
    c = cos(a)
    s = sin(a)
    R = np.array([
        [u[0]**2*(1-c)+c, u[0]*u[1]*(1-c)-u[2]*s, u[0]*u[2]*(1-c)+u[1]*s],
        [u[0]*u[1]*(1-c)+u[2]*s, u[1]**2*(1-c)+c, u[1]*u[2]*(1-c)-u[0]*s],
        [u[0]*u[2]*(1-c)-u[1]*s, u[1]*u[2]*(1-c)+u[0]*s, u[2]**2*(1-c)+c]
    ])
    
    H, L, U = np.array(hlu[0]), np.array(hlu[1]), np.array(hlu[2])
    return R.dot(H), R.dot(L), R.dot(U)

def tropism(hlu, t):
    M = torque(hlu, t)
    alpha = linalg.norm(M)
    if alpha > 0:
        return rotation(hlu, M, alpha)
    return hlu

def draw_3d_tree(words, alphabet):
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    

    sigma = pi/158
    T = np.array([0, 0, -0.5]) * 0.27
    

    xyz = np.array([0.0, 0.0, -200.0])
    teta = pi/8
    HLU = (
        np.array([0.0, 0.0, 1.0]),
        np.array([-sin(teta), -cos(teta), 0.0]),
        np.array([-cos(teta), sin(teta), 0.0])
    )
    
    stack = []
    modules_t = word_to_modules(words[0], alphabet)
    
    for module in modules_t:
        if module[0] == 'F':
            H = np.array(HLU[0])
            length = float(eval(parameters(module)[0]))
            new_xyz = xyz + length * H
            ax.plot([xyz[0], new_xyz[0]], [xyz[1], new_xyz[1]], [xyz[2], new_xyz[2]], 
                   color='#753313', linewidth=3)
            xyz = new_xyz
            HLU = tropism(HLU, T)
            
        elif module[0] == '^':
            angle = float(eval(parameters(module)[0]))
            HLU = RU(gauss(angle, sigma), HLU)
            
        elif module[0] == '&':
            angle = float(eval(parameters(module)[0]))
            HLU = RL(gauss(angle, sigma), HLU)
            
        elif module[0] == '|':
            angle = float(eval(parameters(module)[0]))
            HLU = RH(gauss(angle, sigma), HLU)
            
        elif module[0] == '[':
            stack.append((xyz.copy(), [h.copy() for h in HLU]))
            
        elif module[0] == ']':
            xyz, HLU = stack.pop()
            
        elif module[0] == 'L':
            ax.scatter([xyz[0]], [xyz[1]], [xyz[2]], color='green', s=20)
    
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('3D Tree Model')
    plt.tight_layout()
    plt.show()

N = 5
AXIOME_T, AXIOME_L = AXIOMES
PRODUCTION_T, PRODUCTION_L = PRODUCTIONS
PATTERNS = [
    parametric_word(AXIOME_T, PRODUCTION_T, ALPHABET, N),
    parametric_word(AXIOME_L, PRODUCTION_L, ALPHABET, N+4)
]


draw_3d_tree(PATTERNS, ALPHABET)

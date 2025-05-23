g = 9.8; 
l = 2; 
m = 1;
th0 = 5; 
v0 = 3;

%input real system

A = [0,1; g/l,0];
b=[0;-1/m/l/l];
c=[1,0;0,1];

%system for realisation

cn=[1,0;0,0];
p = [-100 -2];
l = (place(A',cn',p))';
k = (place(A,b,p));

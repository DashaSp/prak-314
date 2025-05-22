from math import pi

ALPHABET=['A','B','C','F','!','^','&','$','|','[',']']
PARAMETERS=['l','w']


angle_1 = 100*pi/200
angel_2 = 180*pi/200 
branching_angle = 18.95*pi/180 
growth_rate = 1.15 
width_rate = 1.3

AXIOME='!(1.9)F(40)|(pi/4)A()'
PRODUCTION=['A():True→!(1.5*width_rate)F(30)[&(branching_angle)!(width_rate)F(20)A()$()]|(angle_1)[&(branching_angle)!(width_rate)F(20)A()$()]|(angel_2)[&(branching_angle)!(width_rate)F(20)A()$()]',
             'F(l):True→F(l*growth_rate)',
             '!(w):True→!(w*width_rate)']

AXIOMES=[AXIOME,AXIOME]
PRODUCTIONS=[PRODUCTION,PRODUCTION]


def split_production(production):
    predecessor=production.split(':')[0]
    condition=production.split(':')[1].split('→')[0]
    successor=production.split(':')[1].split('→')[1]
    return [predecessor, condition,successor]


def word_to_modules(word,alphabet):
    modules=[]
    module=''
    for character in word:
        if character in alphabet :
            modules.append(module)
            module=character
        else:
            if character not in ['[',']','{','}','°']:
                module=module + character
    modules.append(module) #append last module
    modules=modules[1:]
    return modules
        

def n_parameters(module):
    n=0
    for car in module:
        if car==',':
            n=n+1
    if '()' in module:
        return 0
    else:
        return n+1
    

def parameters(module):
    param=''
    if module not in ['[',']','{','}','°']:
        param=module.split('(')[1].split(')')[0].split(',')
    return param


def match(actual,production):
    [predecessor,condition,successor]=split_production(production)
    if predecessor[0]==actual[0]  and n_parameters(predecessor)==n_parameters(actual) :
        for i in range(n_parameters(actual)):
            exec(parameters(predecessor)[i] + '=' + parameters(actual)[i])
        if eval(condition)==True:
            return True
    return False


def apply(module,production,alphabet):
    [predecessor,condition,successor]=split_production(production)
    result=''
    for i in range(n_parameters(module)):
            exec(parameters(predecessor)[i] + '=' + parameters(module)[i])
    successor_modules = word_to_modules(successor,alphabet) #modules in the successor
    for module in successor_modules:
        if module in ['[',']','{','}','°']:
            actual=module
        else:
            actual=module[0]+'('
            for parameter in parameters(module):
                if parameter !='':
                    actual=actual+str(eval(parameter))+','
            if actual[-1]==',':
                actual=actual[:-1] #delete last comma
            actual=actual + ')'
        result=result+ actual
    return result


def next(word,productions,alphabet):
    modules=word_to_modules(word,alphabet)
    result=''
    for module in modules :
        i=0
        for production in productions:
            if match(module,production):
                result=result+apply(module,production,alphabet)
                i=i+1
        if i==0 or module in ['[',']','{','}','°']:
                result=result+module
    return result


def parametric_word(axiome,productions,alphabet,n):
    word = axiome
    for i in range(n):
        word=next(word,productions,alphabet)
    return word

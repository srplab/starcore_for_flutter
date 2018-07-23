def tt(a,b) :
    print(a,b)
    return 666,777
g1 = 123
def yy(a,b,z) :
    print(a,b,z)
    return {'jack': 4098, 'sape': 4139}

class Multiply :
    def __init__(self,x,y) :
        self.a = x
        self.b = y
    
    def multiply(self,a,b):
        print("multiply....",self,a,b)
        return a * b
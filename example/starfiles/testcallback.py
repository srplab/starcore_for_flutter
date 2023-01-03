def tt(a,b) :
    print(a,b)
    ParaPkg = CallBackObj.PrintHello("---------------",{'Name': 'Zara', 'Age': 7, 'Class': True},1234)
    print(ParaPkg)
    print(ParaPkg._Get(0))
    print(ParaPkg._Get(1))  
    print(ParaPkg._Get(1)._ToTuple())
    print(ParaPkg._Get(2))
    return a,b
g1 = 123

def rr() :
    print("ffffffffffffffffffff")
    print(CallBackObj)
    rrv = CallBackObj.HelloVar(234.33,555.66)
    print(rrv)
	
def ss() :
    print(CallBackObj_S[0])	
    print(CallBackObj_S[1])
    print(CallBackObj_S[2])
    print(CallBackObj_S[3])		
	
def mm() :
    print(CallBackObj_M["sss"])	
    print(CallBackObj_M["dddd"])
    print(CallBackObj_M["eeee"])
	
def st() :
    print(CallBackObj_T.Val1)	
    print(CallBackObj_T.Val2)
    print(CallBackObj_T.Val3)	
	
	
def set_ss() :
    print(CallBackObj_S.length)
    CallBackObj_S[0] = CallBackObj_S[0] + "-----python"	
    CallBackObj_S[1] = CallBackObj_S[1] + "-----python"	
    CallBackObj_S[2] = CallBackObj_S[2] + "-----python"	
	
def set_st() :
    CallBackObj_T.Val1 = CallBackObj_T.Val1 + 123
    CallBackObj_T.Val2 = CallBackObj_T.Val2 + 456
    CallBackObj_T.Val3 = CallBackObj_T.Val3 + "---------python"		
	
	
def pf() :
    print(CallBackObj)	
    print(CallBackObj("eeeeeeeeeee",111.222))	
	
	
def attach() :
    SrvGroup = libstarpy._GetSrvGroup(0)
    Service = SrvGroup._GetService("","")
    TestGo = Service._ImportRawContext("go","gofunc",False,"");
    print(TestGo("eeeeeeeeeee",111.222))
    pa = CallBackObj_S._RawToParaPkg()
    print(pa[0],pa[1],pa[2])
		

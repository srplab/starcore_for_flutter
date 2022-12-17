def tt(a,b)
    puts(a,b)
    paraPkg = $CallBackObj.PrintHello("---------------",{'Name'=>'Zara', 'Age'=>7, 'Class'=>true},1234)
    puts(paraPkg)
    puts(paraPkg._Get(0))
    puts(paraPkg._Get(1))  
    puts(paraPkg._Get(1)._ToTuple())
    puts(paraPkg._Get(2))
    return a,b
end
$g1 = 123

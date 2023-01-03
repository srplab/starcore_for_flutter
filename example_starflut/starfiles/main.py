

testValue = 123


def getJsonValue(name, age):
    print(name, age)
    return {'name': name, 'age': age, 'extra': 'GotFromPython'}


class Calculator:
    def __init__(self, x, y):
        self.a = x
        self.b = y

    def multiply(self, a, b):
        print("multiply....", a, b)
        return a * b

    def add(self, a, b):
        print("add....", a, b)
        return a + b

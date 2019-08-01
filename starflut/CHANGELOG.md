## 0.6.0

* release at 2019/08/01
* starcore upgrade to 3.5.0

* Newly added API Interface:
> StarServiceClass:
  >> [allObject](#)            : Future<List> allObject() async 
  >> [restfulCall](#)          : Future<List> restfulCall (String url, String opCode, String jsonString) async

> StarParaPkg:
  >> [equals](#)               : Future<bool> equals (StarParaPkgClass srcParaPkg) async
  >> [v](#)                    : Future<String> get ValueStr async

> StarObject :
  >> [instNumber](#)           : Future<int> instNumber () async
  >> [jsonCall](#)             : Future<String> jsonCall (String jsonString) async

 
## 0.5.2

* release at 2018/08/03
 - bug fix
 - starcore upgrade to 2.6.5 for android

## 0.5.1

* release at 2018/07/29
 - slightly modified readme

## 0.5.0

* First release at 2018/07/22
 - support lua,python,ruby,golang
 - support ios, android

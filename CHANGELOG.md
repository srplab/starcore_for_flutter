# 2.0.0

- Add null safety support
- update outdated libraries

## 1.0.0 version 2021/02/05

- To link with the static frameworks for ios, add STARCORE_FRAMEWORK and STARCORE_FRAMEWORKPATH environment variables to the podspec
- Fix the bug of StarSrvGroup.newParaPkg(null)
- For object's "call" function, if the object is a python, lua or ruby function, then funcName should be null, and, when call class's construct function, funcName should be null
- Add two functions "setEnv" and "getEnv" for Android
- Delete "pushLocalFrame" and "popLocalFrame", and, add functions "newLocalFrame", "freeLocalFrame"
- Add "moveTo" function to starflut object, such as StarServiceClass, StarObjectClass,..., which is used to move the object to other local frame
- Modify the following functions: newParaPkg, createService, newBinBuf, getObject, getObjectEx, newObject, newRawProxy, importRawContext, allObject, restfulCall, add parameters "FrameTag", the FrameTag maybe null.
- Add parameter "FrameTag" for object's callback function StarObjectScriptProc
- Adds version constants : Major_Version, Minor_Version, Build_Version, and string constant "Version" to Starflut
- Adds gc function to Starflut: After running freeLocalFrame, the gc function may be called to release the cle object
- upgrade cle to v3.7.6

## 0.9.5 version 2021/01/25

- add functions for StarBinBufClass : setOffset, print, asString
- enable call starflut functions in callback

## 0.9.0 version 2021/01/13

- support ios, android, and desktop( windows, linux, macos )
- based on starcore 3.7.5
- support python 3.7,3.8,3.9
- Newly added API Interface:

> Starflut:
>
> > [getPlatform](#) : static Future<int> getPlatform() async, note:return value : starflut.ANDROID, IOS, WINDOWS, LINUX, MACOS, WEB

> > [setEnv](#) : static Future<bool> setEnv(String Name,String Value) async,note: android does not support this

> > [getEnv](#) : static Future<String> getEnv(String Name) async,note: android does not support this

> > [loadLibrary](#) : static Future<bool> loadLibrary(String name) async,load sharelibrarynote: ios does not support this

> > [copyFileFromAssetsEx](#) : static Future<bool> copyFileFromAssetsEx(String name,String srcRelatePath,String desRelatePath,bool OverwriteIfExist) async

> StarCoreFactory:
>
> > [setShareLibraryPath](#) : Future<void> setShareLibraryPath(String path) async, note: macos, windows, linux. the location of script interface share library, such as : libstarpy.dylib/so...

> StarObject:
>
> > [active](#) : Future<bool> active () async

> > [deActive](#) : Future<void> deActive () async

> > [isActive](#) : Future<bool> isActive () async

## 0.6.0 version 2019/08/01

- support starcore 3.5.0
- support python 3.7
- Newly added API Interface:
  > StarServiceClass:
  >
  > > [allObject](#) : Future<List> allObject() async
  > > [restfulCall](#) : Future<List> restfulCall (String url, String opCode, String jsonString) async

> StarParaPkg:
>
> > [equals](#) : Future<bool> equals (StarParaPkgClass srcParaPkg) async

> > [v](#) : Future<String> get ValueStr async

> StarObject :
>
> > [instNumber](#) : Future<int> instNumber () async

> > [jsonCall](#) : Future<String> jsonCall (String jsonString) async

## 0.0.1

- supports flutter to interact with other scripting languages such as python, java, ruby, golang, rust, etc. It is easy to use, supports android, ios, and desktop

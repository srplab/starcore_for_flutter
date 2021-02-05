## 0.0.1

* supports flutter to interact with other scripting languages such as python, java, ruby, golang, rust, etc. It is easy to use, supports android, ios, and desktop

## 0.9.0

* add support for macos,windows,linux
* add more interface functions.

## 0.9.1

* setShareLibraryPath need not be called on linux
* fix deadlock problem of linux/ios/macos

## 0.9.5

* add functions for StarBinBufClass : setOffset, print, asString
* enable call starflut functions in callback

## 1.0.0

* To link with the static frameworks for ios, add STARCORE_FRAMEWORK and STARCORE_FRAMEWORKPATH environment variables to the podspec
* Fix the bug of StarSrvGroup.newParaPkg(null)
* For object's "call" function, if the object is a python, lua or ruby function, then funcName should be null, and, when call class's construct function, funcName should be null
* Add two functions "setEnv" and "getEnv" for Android
* Delete "pushLocalFrame" and "popLocalFrame", and, add functions "newLocalFrame", "freeLocalFrame"
* Add "moveTo" function to starflut object, such as StarServiceClass, StarObjectClass,..., which is used to move the object to other local frame
* Modify the following functions: newParaPkg, createService, newBinBuf, getObject, getObjectEx, newObject, newRawProxy, importRawContext, allObject, restfulCall, add parameters "FrameTag", the FrameTag maybe null.
* Add parameter "FrameTag" for object's callback function StarObjectScriptProc
* Adds version constants  : Major_Version, Minor_Version, Build_Version, and string constant "Version" to Starflut
* Adds gc function to Starflut: After running freeLocalFrame, the gc function may be called to release the cle object
* upgrade cle to v3.7.6


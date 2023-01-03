# starflut

A new flutter plugin project, which supports flutter to interact with other scripting languages such as python, java, ruby, golang, rust, etc. It is easy to use, supports android and ios platform.

starflut is based on "starcore-for-android" and "starfore-for-ios project".

### Get Started

To import this null safe version of starflut , add in pubspec.yaml

```pubspec
starflut:
  git:
    url: https://github.com/rohitsangwan01/starcore_for_flutter.git
    ref: null_safe
```

### 1. Get source code

```sh
 $ git clone https://github.com/srplab/starcore_for_flutter
```

- a. ** download "starcore_for_android" **

[srplab：starcore_for_android.3.7.5.zip](https://github.com/srplab/starcore_for_android/raw/master/starcore_for_android.3.5.0.zip).

- b. ** download "starcore_for_ios" **

[srplab：starcore_for_ios.3.7.5.tar.gz](https://github.com/srplab/starcore_for_ios/raw/master/starcore_for_ios.3.5.0.tar.gz).

### 2. How to use, with an example of calling python

**For Android**

create sub folder under "main" and add python share libraries as follow

```
[app]
  [src]
    [main]
      [assets]
        [arm64-v8a]
          unicodedata.cpython-39.so
          zlib.cpython-39.so
        [armeabi]
          unicodedata.cpython-39.so
          zlib.cpython-39.so
        [x86]
          unicodedata.cpython-39.so
          zlib.cpython-39.so
        [x86_64]
          unicodedata.cpython-39.so
          zlib.cpython-39.so
        python3.9.zip
      [java]
        [jniLibs]
          [arm64-v8a]
            libpython3.9.so
            libstar_python39.so
          [armeabi]
            libpython3.9.so
            libstar_python39.so
          [x86]
            libpython3.9.so
            libstar_python39.so
          [x86_64]
            libpython3.9.so
            libstar_python39.so
        res
```

**_Add code in main.dart_**

copy python files (share libraries and python3.9.zip) to app folder using function "copyFileFromAssets"

```
bool isAndroid = await Starflut.isAndroid();
if( isAndroid == true ){
  await Starflut.copyFileFromAssets("testcallback.py", "flutter_assets/starfiles","flutter_assets/starfiles");
  await Starflut.copyFileFromAssets("testpy.py", "flutter_assets/starfiles","flutter_assets/starfiles");
  await Starflut.copyFileFromAssets("python3.9.zip", null,null);   //desRelatePath must be null

  var nativepath = await Starflut.getNativeLibraryDir();
  var LibraryPath = "";
  if( nativepath.contains("x86_64"))
    LibraryPath = "x86_64";
  else if( nativepath.contains("arm64"))
    LibraryPath = "arm64-v8a";
  else if( nativepath.contains("arm"))
    LibraryPath = "armeabi";
  else if( nativepath.contains("x86"))
    LibraryPath = "x86";

  await Starflut.copyFileFromAssets("zlib.cpython-39.so", LibraryPath,null);
  await Starflut.copyFileFromAssets("unicodedata.cpython-39.so", LibraryPath,null);
  await Starflut.loadLibrary("libpython3.9.so");
}
```

**For IOS**

Using environment variable to control the compiling process.

1. set "STARCORE_PATH" to the folder of "starcore_for_ios"
2. if python is used, set "STARCORE_PYTHONVERSION" and "STARCORE_PYTHONCOREPATH"
   > [STARCORE_PYTHONVERSION](#) : '3.9' '3.8' 3.7' '3.6' '3.5' '2.7'
   > [STARCORE_PYTHONCOREPATH](#) :the folder of 'python3.6m' 'python3.6.zip' or 'python3.7m','pyton3.7.zip'...
3. if ruby is used, set "STARCORE_RUBYCOREPATH"
   > [STARCORE_RUBYCOREPATH](#): the folder of 'ruby 2.4' 'ruby 2.5',...
4. if golang module is used ,set "STARCORE_GOLIBRARY",and "STARCORE_GOLIBRARYPATH".
   > [STARCORE_GOLIBRARY](#) : name of go static library, ex: if the name is libxxx.a, then here is 'xxx'
   > [STARCORE_GOLIBRARYPATH](#) : where static library is located
5. if starcore native service is used, set "STARCORE_GETEXPORTFUNCTIONTABLENAME" ,"STARCORE_EXPORTFUNCTIONDEFINE", "STARCORE_EXPORTFUNCTIONNAME", "STARCORE_STATICLIBRARY", and "STARCORE_STATICLIBRARYPATH"
   > [STARCORE_GETEXPORTFUNCTIONTABLENAME](#) : if the function name is xxx_GetExportFunctionTable,yyy_GetExportFunctionTable, then here is xxx,yyy
   > [STARCORE_EXPORTFUNCTIONDEFINE](#) : 'extern \"C\" void *xxx();extern \"C\" void *yyy();'
   > [v_STARCORE_EXPORTFUNCTIONNAME](#) : 'xxx,yyy'
   > [STARCORE_STATICLIBRARY](#) : name of static library, ex: if the name is libxxx.a, then here is 'xxx'
   > [STARCORE_STATICLIBRARYPATH](#) : the folder of the static library
   > [STARCORE_FRAMEWORK](#) : the name of framework, ex: 'xxx,yyy'
   > [STARCORE_FRAMEWORKPATH](#) : the folder of the static framework

```
for example
$ flutter clean
$ export STARCORE_PATH="/Users/srplab/Desktop"
$ export STARCORE_PYTHONVERSION="3.9"
$ export STARCORE_PYTHONCOREPATH="/Users/srplab/Desktop/python-3.9"
$ flutter build ios --no-codesign
```

```
for example
$ flutter clean
$ export STARCORE_PATH="/Users/srplab/Desktop"
$ export STARCORE_GOLIBRARYPATH="/Users/srplab/Desktop/go.lib"
$ flutter build ios --no-codesign
```

**For Windows**

- create folder under [windows], copy share libraries into the folder, for example,

```
  [windows]
    [starcore]
      libstar_python39.pyd
```

- modify "CMakeLists.txt", instal the share libraries needed.

```
install(FILES "starcore/libstar_python39.pyd" DESTINATION "${INSTALL_BUNDLE_LIB_DIR}"
  COMPONENT Runtime)
```

**For linux**

- create folder under [linux], copy share libraries into the folder, for example,

```
  [linux]
    [starcore]
      libstar_python39.so
```

- modify "CMakeLists.txt", instal the share libraries needed.

```
install(FILES "starcore/libstar_python39.so" DESTINATION "${INSTALL_BUNDLE_LIB_DIR}"
  COMPONENT Runtime)
```

**For macos**

> open the xxx.xcworkspace with xcode.
> click "Runner", and under the "build phase" > "Copy Bundle Resources"
> add file,
>
> > libstar_python39.so: which is from the folder "unlink" of starcore.
> > libpython3.9.dylib: compiled from source code of python3.9

```
String Path1  = await Starflut.getResourcePath();
if( Platform == Starflut.MACOS ) {
      /*macos*/
      await starcore.setShareLibraryPath(
          Path1); //set path for interface library
      await Starflut.loadLibrary(
          Path1 + "/libpython3.9.dylib");
      await Starflut.setEnv("PYTHONPATH",
          "/Library/Frameworks/Python.framework/Versions/3.9/lib/python3.9");
}
```

The "/Library/Frameworks/Python.framework/Versions/3.9/lib/python3.9" is folder of python script, which may be changed.

**Add Python file to be called**

1. create folder "starfiles" under the project, add python scripts. ** The folder name must be "starfiles" **

```
starfiles
  python3.6.zip
  testpy.py
```

2. modify "pubspec.yaml", add assets folder

```
flutter:

  # The following line ensures that the Material Icons font is
  # included with your application, so that you can use the icons in
  # the material Icons class.
  uses-material-design: true

  # To add assets to your application, add an assets section, like this:
  assets:
    - starfiles/
```

**Call python code**

1.Init starcore

```
StarCoreFactory starcore = await Starflut.getFactory();
await starcore.regMsgCallBackP(
            (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
          print("$serviceGroupID  $uMsg   $wParam   $lParam");
          return null;
        });

StarServiceClass Service = await starcore.initSimple("test", "123", 0, 0, []);
StarSrvGroupClass SrvGroup = await Service["_ServiceGroup"];
```

2.Init python

```
await SrvGroup.initRaw("python39", Service);
StarObjectClass python = await Service.importRawContext(null,"python", "", false, "");
```

3.Run python file

```
await SrvGroup.doFile("python", assetsPath + "/flutter_assets/starfiles/" + "testcallback.py")
```

## API Interface:

```
note:
  !! important
  !! for dart does not support destruct function of class, so the starobject must be freed manually, or else will cause memory leak
  !!    newLocalFrame, freeLocalFrame and gc can be used to help releasing starobjects.

  !! FrameTag may be null

  !! starflut must be run in main ui thread.

Starflut:
  isAndroid            : static Future<bool> isAndroid() async
  getFactory           : static Future<StarCoreFactory> getFactory() async
  getDocumentPath      : static Future<String> getDocumentPath() async
                       : andoid : "/data/data/"+activity.getPackageName()+"/files"
                       : ios/macos: NSDocumentDirectory
                       : others: reserved
  getResourcePath      : static Future<String> getResourcePath() async
                       : for ios/macos : [[NSBundle mainBundle] resourcePath]
                       : for android : is same with getDocumentPath
                       : for windows : the executable file path, where the share library under starcore folder located
                       : for linux : the lib path, where the share library under starcore folder located
  getAssetsPath         : static Future<String> getAssetsPath() async
                       : for android : is same with getDocumentPath
                       : for others : flutter assets path

  pushLocalFrame       : static void pushLocalFrame()
                         start from 1.0.0, this function is invalid
  popLocalFrame        : static void popLocalFrame()
                          start from 1.0.0, this function is invalid
  --v0.9.0
  getPlatform          : static Future<int> getPlatform() async
                         return value : starflut.ANDROID, IOS, WINDOWS, LINUX, MACOS, WEB

  setEnv               : static Future<bool> setEnv(String Name,String Value) async

  getEnv               : static Future<String> getEnv(String Name) async

  loadLibrary          : static Future<bool> loadLibrary(String name) async
                         load sharelibrary
                         note: ios does not support this

  --v1.0.0
  newLocalFrame       :  static String newLocalFrame()
                      :  return a FrameTag

  freeLocalFrame       :  static void freeLocalFrame(String FrameTag)
                         note: when freeLocalFrame is called, the starobject allocated in FrameTag may be freed
                               If starobject will be used after this function, it's lock function must be called.

                       : StarSrvGroupClass instance always in root frame
                       : StarServiceClass instance returned from getService or initSimple always in root frame

  gc                   : static Future<void> gc() async
                       : After running freeLocalFrame, you can call the gc function to release the cle object

  ** for android
  getNativeLibraryDir  : static Future<String> getNativeLibraryDir() async
  getPackageName       :
  unzipFromAssets      : static Future<bool> unzipFromAssets(String fileName,String desPath,bool overWriteFlag) async
                       : fileName is fullname with path relative to assets
                       : desPath is fullname with path relative to '/'
  copyFileFromAssets   : static Future<bool> copyFileFromAssets(String name,String srcRelatePath,String desRelatePath) async
                       : srcRelatePath is the path fromm assets, may be null, if at assets's root
                       : desRelatePath is the path relative to "/data/data/"+activity.getPackageName()+"/files"
                       :               if null, the desRelatePath is "/data/data/"+activity.getPackageName()+"/files"
  getActivity          : static Future<StarObjectClass> getActivity(StarServiceClass service) async
                       : note: this function must be called after the service is created
                         note: only valid for android

  --v0.9.0
  copyFileFromAssetsEx   : static Future<bool> copyFileFromAssetsEx(String name,String srcRelatePath,String desRelatePath,bool OverwriteIfExist) async

  ** for ios
  rubyInitExt          : static Future<String> rubyInitExt() async
                       : this function should be called after initRaw("ruby",Service);
                       : this function load ext:  md5,rmd160,sha1,sha2,openssl


StarCoreBase
  lock                 : void lock({String FrameTag=null})
                       : move the starobject from local frame to global frame, or to other local frame
  moveTo               : void moveTo(String FrameTag)
                       : same as lock if FrameTag = null

StarCoreFactory:
  initCore             : Future<int> initCore(bool serverFlag,bool showMenuFlag,bool showClientWndFlag,bool sRPPrintFlag,String debugInterface,int debugPort,String clientInterface,int clientPort) async
  initSimpleEx         : Future<StarSrvGroupClass> initSimpleEx(int clientPort,int webPort,List<String> dependService) async
  initSimple           : Future<StarServiceClass> initSimple(String serviceName,String servicePass,int clientPortNumber,int webPortNumber,List<String> dependService) async
  getSrvGroup          : Future<StarSrvGroupClass> getSrvGroup(Object ServiceName_GroupID) async
  moduleExit           : Future<void> moduleExit() async
  moduleClear          : Future<void> moduleClear() async
  regMsgCallBackP      : Future<void> regMsgCallBackP(MsgCallBackProc callBack) async
                       : note: !!! In callback function, do not call any starflut functions
  //regDispatchRequestP  : Future<void> regDispatchRequestP(DispatchRequestProc callBack) async
  sRPDispatch          : Future<bool> sRPDispatch(bool waitFalg) async
  sRPLock              : Future<void> sRPLock() async
  sRPUnLock            : Future<void> sRPUnLock() async
  setRegisterCode      : Future<bool> setRegisterCode(String codeString,bool single) async
  isRegistered         : Future<bool> isRegistered () async
  setLocale            : Future<void> setLocale(String lang) async
  getLocale            : Future<String> getLocale () async
  version              : Future<List> version () async
  getScriptIndex       : Future<int> getScriptIndex (String interface) async
  setScript            : Future<bool> setScript(String scriptInterface,String module, String para) async
  detachCurrentThread  : Future<void> detachCurrentThread() async
  coreHandle           : Future<int> coreHandle () async

  --v0.6.0
  releaseScriptGIL     : Future<void> releaseScriptGIL() async
  captureScriptGIL     : Future<void> captureScriptGIL() async

  --v0.9.0
  setShareLibraryPath  : Future<void> setShareLibraryPath(String path) async
                       : note: macos, windows, linux
                       : the location of script interface share library, such as : libstarpy.dylib/so...

StarSrvGroupClass :

  getString            : Future<String> getString() async
                         = toString

  createService        : Future<StarServiceClass> createService(String FrameTag,String servicePath,String serviceName,String rootPass,int frameInterval,int netPkgSize,int uploadPkgSize,int downloadPkgSize,int dataUpPkgSize,int dataDownPkgSize,String derviceID) async
                       : FrameTag maybe null, or the return value from newLocalFrame
  getService           : Future<StarServiceClass> getService(String username, String userpassword) async
  clearService         : Future<void> clearService() async
  newParaPkg           : Future<StarParaPkgClass> newParaPkg(String FrameTag,Object args) async , example: StarSrvGroup.newParaPkg(['aaaa',234,[345]])
                       : args maybe List or Map
                       : FrameTag maybe null, or the return value from newLocalFrame
  newBinBuf            : Future<StarBinBufClass> newBinBuf(String FrameTag) async
  //newSXml              : Future<StarSXmlClass> newSXml() async
                       : FrameTag maybe null, or the return value from newLocalFrame
  isObject             : bool isObject(Object which)
  isParaPkg            : bool isParaPkg(Object which)
  isBinBuf             : bool isBinBuf(Object which)
  //isSXml               : bool isSXml(Object which)
  getServicePath       : Future<String> getServicePath() async
  setServicePath       : Future<void> setServicePath (String args) async
  servicePathIsSet     : Future<bool> servicePathIsSet() async
  getCurrentPath       : Future<String> getCurrentPath() async
  importService        : Future<bool> importService (String serviceName,bool loadRunModule) async
  clearServiceEx       : Future<void> clearServiceEx() async
  runScript            : Future<bool> runScript (String scriptInterface,String scriptBuf,String moduleName) async
  runScriptEx          : Future<bool> runScriptEx (String scriptInterface,StarBinBufClass binBuf,String moduleName) async
  doFile               : Future<bool> doFile (String scriptInterface,String fileName) async
  doFileEx             : Future<bool> doFileEx (String scriptInterface,String fileName,String moduleName) async
  setClientPort        : Future<bool> setClientPort (String lInterface,int portnumber) async
  setTelnetPort        : Future<bool> setTelnetPort (int portnumber) async
  setOutputPort        : Future<bool> setOutputPort (String host,int portnumber) async
  setWebServerPort     : Future<bool> setWebServerPort (String host,int portnumber,int connectionNumber,int postSize) async
  initRaw              : Future<bool> initRaw (String scriptInterface,StarServiceClass service) async : note: always returns true, if error occured,it will be print in log window
  loadRawModule        : Future<bool> loadRawModule (String scriptInterface,String moduleName,String fileOrString, bool isString) async : note: always returns true, if error occured,it will be print in log window
  getLastError         : Future<int> getLastError () async
  getLastErrorInfo     : Future<String> getLastErrorInfo () async
  getCorePath          : Future<String> getCorePath () async
  getUserPath          : Future<String> getUserPath () async
  getLocalIP           : Future<String> getLocalIP () async
  getLocalIPEx         : Future<List> getLocalIPEx () async
  getObjectNum         : Future<int> getObjectNum () async
  activeScriptInterface : Future<List> activeScriptInterface (String scriptInterfaceName) async
  preCompile           : Future<List> preCompile (String scriptInterfaceName,String ScriptSegment) async

StarServiceClass:
  getString            : Future<String> getString() async
                         = toString

  []                   : Future<Object> operator [](Object object) async
                       : == Get : "_Name"  "_ServiceGroup" "_ID"
  getObject            : Future<StarObjectClass> getObject (String FrameTag,String objectName) async
                       : FrameTag maybe null, or the return value from newLocalFrame
  getObjectEx          : Future<StarObjectClass> getObjectEx (String FrameTag,String objectID) async
                       : FrameTag maybe null, or the return value from newLocalFrame
  newObject            : Future<StarObjectClass> newObject (String FrameTag,List args) async
                       : FrameTag maybe null, or the return value from newLocalFrame
                       : == "_New()"
  runScript            : Future<List> runScript (String scriptInterface,String scriptBuf,String moduleName,String workDirectory) async
  runScriptEx          : Future<List> runScriptEx (String scriptInterface,StarBinBufClass binBuf,String moduleName,String workDirectory) async
  doFile               : Future<List> doFile (String scriptInterface,String fileName,String workDirectory) async
  doFileEx             : Future<List> doFileEx (String scriptInterface,String fileName,String workDirectory,String moduleName) async
  isServiceRegistered  : Future<bool> isServiceRegistered () async
  checkPassword        : Future<void> checkPassword (bool flag) async
  newRawProxy          : Future<StarObjectClass> newRawProxy (String FrameTag,String scriptInterface,StarObjectClass attachObject,String attachFunction,String proyInfo,int proxyType) async
                       : FrameTag maybe null, or the return value from newLocalFrame
  importRawContext     : Future<StarObjectClass> importRawContext (String FrameTag,String scriptInterface,String contextName,bool isClass,String contextInfo) async
                       : FrameTag maybe null, or the return value from newLocalFrame
  getLastError         : Future<int> getLastError () async
  getLastErrorInfo     : Future<String> getLastErrorInfo () async

  --v0.6.0
  allObject            : Future<List> allObject(String FrameTag) async
                       : FrameTag maybe null, or the return value from newLocalFrame
  restfulCall          : Future<List> restfulCall (String FrameTag,String url, String opCode, String jsonString) async
                       : FrameTag maybe null, or the return value from newLocalFrame

StarParaPkg:
  getString            : Future<String> getString() async
                         = toString

  number               : Future<int> get number async

  []                   : Future<Object> operator [](int index) async
                       : == Get :
  getValue             : Future<Object> getValue(Object value) async
                       : == Get:  value may be integer or List<int> which will returns multiple values corresponding to each index
                       :
  setValue             : Future<StarParaPkgClass> setValue(int index,Object value) async
                       : == Set
  build                : Future<StarParaPkgClass> build (Object args) async
                       : args maybe List or Map
                       : return self
  clear                : Future<StarParaPkgClass> clear () async
  appendFrom           : Future<bool> appendFrom (StarParaPkgClass srcParaPkg) async
  free                 : Future<void> free () async
  dispose              : Future<void> dispose () async
  releaseOwner         : Future<void> releaseOwner () async
  asDict               : Future<StarParaPkgClass> asDict (bool isDict) async
  isDict               : Future<bool> isDict () async
  fromJSon             : Future<bool> fromJSon (String jsonstring) async
  toJSon               : Future<String> toJSon () async
  toTuple              : Future<Object> toTuple () async

  --v0.6.0
  equals               : Future<bool> equals (StarParaPkgClass srcParaPkg) async
  v                    : Future<String> get ValueStr async

  --v0.9.6
  fromTuple            : Future<StarParaPkgClass> fromTuple (Object args) async
                       : args maybe List or Map
                       : return self

StarBinBuf :
  getString            : Future<String> getString() async
                         = toString
  offset               : Future<int> get offset async
                         = _Offset
  init                 : Future<void> init (int bufSize) async
  clear                : Future<void> clear (int bufSize) async
  saveToFile           : Future<bool> saveToFile (String fileName,bool txtFileFlag) async
  loadFromFile         : Future<bool> loadFromFile (String fileName,bool txtFileFlag) async
  read                 : Future<Uint8List> read (int offset,int length) async
  write                : Future<int> write (int offset,Uint8List buf,int length) async
  free                 : Future<void> free () async
  dispose              : Future<void> dispose () async
  releaseOwner         : Future<void> releaseOwner () async

  --v0.9.5
  setOffset              : Future<bool> setOffset (int Offset) async
  print                  : Future<void> print (String Arg) async
  asString               : Future<String> asString() async
                         : note : return the buf as a string

StarObject :
  getString            : Future<String> getString() async
                         = toString
  []                   : Future<Object> operator [](Object indexOrName) async
                         = Get : "_Service"  "_Class"  "_ID"  "_Name"
  getValue             : Future<Object> getValue(Object indexOrNameOrList) async
                         = Get : "_Service"  "_Class"  "_ID"  "_Name"
                         value may be integer or List<int or String> which will returns multiple values corresponding to each index or Name
  setValue             : Future<void> setValue(Object indexOrName,Object value) async
                         = Set : "_Name"
  call                 : Future<Object> call (String funcName,List args) async
                       : note:  !!! In callback function, do not call any starflut functions, this limition has been removed from 0.9.5
                       : note:  if the object is python, lua or ruby function, funcName should be null.  0.9.6
                       : note:  when call class's construct function, funcName should be null.  0.9.6
  newObject            : Future<StarObjectClass> newObject (List args) async
                       : = "_New()"
  free                 : Future<void> free () async
  dispose              : Future<void> dispose () async
  hasRawContext        : Future<bool> hasRawContext () async
  rawToParaPkg         : Future<Object> rawToParaPkg () async
  getSourceScript      : Future<int> getSourceScript () async
  getLastError         : Future<int> getLastError () async
  getLastErrorInfo     : Future<String> getLastErrorInfo () async
  releaseOwnerEx       : Future<void> releaseOwnerEx () async
  regScriptProcP       : Future<void> regScriptProcP(String scriptName,StarObjectScriptProc callBack) async

  --v0.6.0
  instNumber           : Future<int> instNumber () async
  changeParent         : Future<void> changeParent (StarObjectClass parentObject,String queueName) async
  jsonCall             : Future<String> jsonCall (String jsonString) async

  --v0.9.0
  active               : Future<bool> active () async
  deActive             : Future<void> deActive () async
  isActive             : Future<bool> isActive () async
```

## More Info:

For more information about use how to use cle's APIs, please refer to

[common language extension_programmer's guide_en.pdf](https://github.com/srplab/starcore_for_android/blob/master/docs/common%20language%20extension_programmer's%20guide_en.pdf)

and,

[common language extension_interface for script_en.pdf](https://github.com/srplab/starcore_for_android/blob/master/docs/common%20language%20extension_interface%20for%20script_en.pdf)

For more examples, please refer to

[srplab.github.io](https://srplab.github.io)

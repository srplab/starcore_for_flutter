import 'dart:async';
import 'dart:core';
import 'dart:typed_data';
import 'package:uuid/uuid.dart';
//import 'package:stack_trace/stack_trace.dart';

import 'package:flutter/services.dart';

typedef Future<List> MsgCallBackProc(int serviceGroupID, int uMsg,Object wParam,Object lParam); /*IsProcessed bool, Result Object*/
typedef Future<Object> StarObjectScriptProc(StarObjectClass cleObject,String FrameTag,List paras);
typedef void DispatchRequestProc();

/*flutter build ios --no-codesign */
/*for more details about the interface, please refer to "common language extension_interface for script_en" */

/*--------------------------------------------------------------------------
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


---------------------------------------------------------------------------*/

/*----may be supported in future-----
class StarObjectWeakReference{
  String starTag;
  Expando<String> starExpando;
  StarObjectWeakReference(String starTag,Object starObject){
    this.starTag = starTag;
    this.starExpando = new Expando<String>();
    this.starExpando[starTag] = starObject;
  }
}
*/

class Starflut {
  static const int ANDROID = 0;
  static const int IOS = 1;
  static const int WINDOWS = 2;
  static const int LINUX = 3;
  static const int MACOS = 4;
  static const int WEB = 5;

  static const int Major_Version = 1;
  static const int Minor_Version = 0;
  static const int Build_Version = 0;
  static const String Version = "1.0.0";

  static StarCoreFactory starcore;

  /*--dart no destruct, so we use Expando the track the starcore object which will be collected by gc--*/
  /*----may be supported in future-----
  static List<StarObjectWeakReference> starObjectWeakRefList = new List<StarObjectWeakReference>();
  static void checkStarObjectValid(String triggerStarTag)
  {
    if( starObjectWeakRefList.length == 0 )
      return;
    StarObjectWeakReference lwr = starObjectWeakRefList.removeAt(0);
    if( lwr.starExpando[lwr.starTag] == null ){
      print("is colleted");
    }
    starObjectWeakRefList.add(lwr);
  }
  */

  static const String StarCorePrefix =     "@s_s_c";
  static const String StarSrvGroupPrefix = "@s_s_g";
  static const String StarServicePrefix =  "@s_s_e";
  static const String StarParaPkgPrefix =  "@s_s_p";
  static const String StarBinBufPrefix  =  "@s_s_b";
  //static const String StarSXmlPrefix  =  "@s_s_x";
  static const String StarObjectPrefix = "@s_s_o";
  static const String StarValueBooleanPrefix_TRUE = "@s_s_t_bool_true";  /*-fix bug for return boolean in arraylist or hashmap-*/
  static const String StarValueBooleanPrefix_FALSE = "@s_s_t_bool_false";

  static Map<String,List<StarObjectRecordClass>> starObjectRecordFrame =  new Map<String,List<StarObjectRecordClass>>();
  static String starObjectRecordListRoot_Key;
  static List<StarObjectRecordClass> starObjectRecordListRoot;
  static List<String> starObjectRecordFrameWaitFree = new List.filled(0, null,growable: true);

  static const MethodChannel channel =
  const MethodChannel('starflut');

  static Future<StarCoreFactory> getFactory() async
  {
    if( starcore == null ){
      List<StarObjectRecordClass> ll = new List.filled(0, null,growable: true);
      var uuid = Uuid();
      starObjectRecordListRoot_Key = uuid.v1();
      starObjectRecordFrame[starObjectRecordListRoot_Key] = ll; // create root frame
      starObjectRecordListRoot = ll;

      channel.setMethodCallHandler(handler);
      String starTag = await channel.invokeMethod('starcore_init');
      if( starTag == null || starTag.length == 0 )
          return null;
      starcore = new StarCoreFactory(starTag);
      return starcore;
    }else{
      return starcore;
    }
  }

  static  pushLocalFrame(){
    throw 'pushLocalFrame does not supported from v1.0.0. please use newLocalFrame!';
  }

  static void popLocalFrame(String FrameTag){
    throw 'popLocalFrame does not supported from v1.0.0. please use freeLocalFrame!';
  }

  static String newLocalFrame(){
    if( starcore == null ){
      throw 'newLocalFrame failed. Starflut.getFactory must be called first!';
    }
    var uuid = Uuid();
    String FrameTag = uuid.v1();
    List<StarObjectRecordClass> ll = new List.filled(0, null,growable: true);
    starObjectRecordFrame[FrameTag] = ll; // create frame
    return FrameTag;
  }

  static void freeLocalFrame(String FrameTag){
    if( starObjectRecordFrame.containsKey(FrameTag) == false ){
      return; /*--no action--*/
    }
    List<StarObjectRecordClass> starObjectRecordList = starObjectRecordFrame[FrameTag];
    for(int i=0; i < starObjectRecordList.length; i++ ){
      starObjectRecordFrameWaitFree.add(starObjectRecordList[i].starObject.starTag);
    }
    //print("free localframe   "+FrameTag);
    starObjectRecordFrame.remove(FrameTag);
  }

  static void insertLocalFrame(String FrameTag,StarObjectRecordClass record)
  {
    if( FrameTag == null || FrameTag.length == 0 )
      starObjectRecordListRoot.add(record);
    else{
      if( starObjectRecordFrame.containsKey(FrameTag) == false ){
        return; /*--no action--*/
      }
      List<StarObjectRecordClass> starObjectRecordList = starObjectRecordFrame[FrameTag];
      starObjectRecordList.add(record);
    }
  }

  static Future<void> gc() async
  {
    try {
      if( Starflut.starObjectRecordFrameWaitFree.length != 0 ){
        List<String> starObjectRecordFrameWaitFree_Local = Starflut.starObjectRecordFrameWaitFree;
        Starflut.starObjectRecordFrameWaitFree = new List.filled(0, null,growable: true);
        await Starflut.channel.invokeMethod('starcore_poplocalframe',starObjectRecordFrameWaitFree_Local);
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  static StarSrvGroupClass getStarSrvGroupFromRecordByTag(String tag)
  {
    for(int i=0; i < starObjectRecordListRoot.length; i++ ){
      if( starObjectRecordListRoot[i].starObject.starTag == tag ){
        return starObjectRecordListRoot[i].starObject;
      }
    }
    return null;
  }

  static StarCoreBase getObjectFromRecordById(String id)
  {
    for(int i=0; i < starObjectRecordListRoot.length; i++ ){
      if( starObjectRecordListRoot[i].starObject.starId == id ){
        return starObjectRecordListRoot[i].starObject;
      }
    }
    return null;
  }

  static Future<List> typeCheck(List value) async
  {
    return await channel.invokeMethod('starcore_typeCheck',value);
  }

  static Future<String> getNativeLibraryDir() async
  {
    return await channel.invokeMethod('starcore_nativeLibraryDir');
  }

  static Future<String> getDocumentPath() async
  {
    return await channel.invokeMethod('starcore_getDocumentPath');
  }
  static Future<String> getResourcePath() async
  {
    return await channel.invokeMethod('starcore_getResourcePath');
  }
  static Future<String> getAssetsPath() async
  {
    return await channel.invokeMethod('starcore_getAssetsPath');
  }
  static Future<String> rubyInitExt() async
  {
    return await channel.invokeMethod('starcore_rubyInitExt');
  }
  static Future<String> getPackageName() async
  {
    return await channel.invokeMethod('starcore_getPackageName');
  }
  static Future<bool> unzipFromAssets(String fileName,String desPath,bool overWriteFlag) async
  {
    return await channel.invokeMethod('starcore_unzipFromAssets',[fileName,desPath,overWriteFlag]);
  }
  static Future<bool> copyFileFromAssets(String name,String srcRelatePath,String desRelatePath) async
  {
    return await channel.invokeMethod('starcore_copyFileFromAssets',[name,srcRelatePath,desRelatePath,true]);
  }
  static Future<bool> copyFileFromAssetsEx(String name,String srcRelatePath,String desRelatePath,bool OverwriteIfExist) async
  {
    return await channel.invokeMethod('starcore_copyFileFromAssets',[name,srcRelatePath,desRelatePath,OverwriteIfExist]);
  }

  static Future<bool> isAndroid() async
  {
    return await channel.invokeMethod('starcore_isAndroid');
  }
  static Future<int> getPlatform() async
  {
    return await channel.invokeMethod('starcore_getPlatform');
  }

  static Future<bool> loadLibrary(String name) async
  {
    return await channel.invokeMethod('starcore_loadLibrary',[name]);
  }

  static Future<bool> setEnv(String name,String value) async
  {
    return await channel.invokeMethod('starcore_setEnv',[name,value]);
  }

  static Future<String> getEnv(String name) async
  {
    return await channel.invokeMethod('starcore_getEnv',name);
  }

  static Future<StarObjectClass> getActivity(StarServiceClass service) async
  {
    try{
      String lTag = await channel.invokeMethod('starcore_getActivity',[service.starTag]);
      if( lTag == null )
        return null;
      else{
        List<String> tags = lTag.split("+");
        StarObjectClass lObject = getObjectFromRecordById(tags[1]);
        if( lObject == null ){
          lObject = new StarObjectClass(null,tags[0],tags[1]);
          Starflut.insertLocalFrame(null,new StarObjectRecordClass(lObject,"StarObjectClass",2));
        }else{
          Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
        }
        return lObject;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  /*--callback function from java--*/
  static Future<dynamic> handler(MethodCall call) async{
    String method = call.method;

    switch (method) {
      case "starcore_msgCallBack":
        {
          List lL = call.arguments;
          try{
            List result = await starcore.flutterMsgCallBack(lL[0],lL[1],lL[2],lL[3]);
            return [lL[4],result];
          } on Exception catch (e) {
            print("$e");
            return [lL[4],null];
          }
        }
        break;
      case "starobjectclass_scriptproc":
        {
          List lL =  call.arguments;

          String FrameTag = Starflut.newLocalFrame();

          List<String> tags = lL[0].split("+");

          StarObjectClass lObject = getObjectFromRecordById(tags[1]);
          if( lObject == null ){
            lObject = new StarObjectClass(FrameTag,tags[0],tags[1]);
            Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lObject,"StarObjectClass",2));
          }else{
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          String funcName = lL[1];
          List args = processOutputArgs(FrameTag,lL[2]);
          if( !lObject.scriptCallBack.containsKey(funcName) ){
            Starflut.freeLocalFrame(FrameTag);
            return [lL[3],funcName,null];
          }
          Object result;
          try{
            result = await lObject.scriptCallBack[funcName](lObject,FrameTag,args);
          } on Exception catch (e) {
            print("$e");
            result = null;
          }
          if(result == null ){
            Starflut.freeLocalFrame(FrameTag);
            return [lL[3],funcName,null];
          }if( result is List ) {
            List ReturnResult = processInputArgs(result); /*if object is return, it will be locked, should not be freed by popLocalFrame*/
            return [lL[3],funcName,[FrameTag,ReturnResult]];
          }else{
            List result_list = new List.filled(1,result);
            List ReturnResult = processInputArgs(result_list);
            return [lL[3],funcName,[FrameTag,ReturnResult[0]]];
          }
        }
        break;
      case "starobjectclass_scriptproc_freeLlocalframe":
        {
          List lL =  call.arguments;
          String FrameTag = lL[0];
          Starflut.freeLocalFrame(FrameTag);
        }
    }
    return null;
  }

  static List processInputArgs(List args)
  {
    return processInputArgs_withlock(args,false);
  }
  static List processInputArgs_withlock(List args,bool LockObject)
  {
    List out = new List.filled(args.length,null);
    for( int i=0; i < args.length; i++ ){
      Object value = args[i];
      if( value is StarObjectClass ){
        StarObjectClass vo = value;
        if( LockObject == true )
          vo.lock();
        out[i] = vo.starTag;
      }else if( value is StarParaPkgClass ){
        StarParaPkgClass vo = value;
        if( LockObject == true )
          vo.lock();
        out[i] = vo.starTag;
      }else if( value is StarBinBufClass ){
        StarBinBufClass vo = value;
        if( LockObject == true )
          vo.lock();
        out[i] = vo.starTag;
      }else if( value is StarServiceClass ){
        StarServiceClass vo = value;
        if( LockObject == true )
          vo.lock();
        out[i] = vo.starTag;
      }else{
        out[i] = value;
      }
    }
    return out;
  }

  static Object processOutputArgs(String FrameTag,Object argsListOrMap)
  {
    List args;
    bool isMap = false;
    if( argsListOrMap is List ){
      args = argsListOrMap;
    }else if( argsListOrMap is Map ){
      Map argMap = argsListOrMap;
      args = [];
      for( var item in argMap.entries ){
        args.add(item.key);
        args.add(item.value);
      }
      isMap = true;
    }else{
      return argsListOrMap;
    }
    List out = new List.filled(args.length,null);
    for( int i=0; i < args.length; i++ ){
      Object retVal = args[i];
      if( retVal is String){
        String val = retVal;
        if( val.startsWith(Starflut.StarObjectPrefix)){
          List<String> tags = val.split("+");
          StarObjectClass lStarObject = getObjectFromRecordById(tags[1]);
          if( lStarObject == null ){
            lStarObject = new StarObjectClass(FrameTag,tags[0],tags[1]);
            Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lStarObject,"StarObjectClass",2));
          }else{
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          out[i] = lStarObject;
        }else if( val.startsWith(Starflut.StarBinBufPrefix)){
          StarBinBufClass lStarBinBuf = new StarBinBufClass(FrameTag,val);
          Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lStarBinBuf,"StarBinBufClass",2));
          out[i] = lStarBinBuf;
        }else if( val.startsWith(Starflut.StarParaPkgPrefix)){
          StarParaPkgClass lStarParaPkg = new StarParaPkgClass(FrameTag,val);
          Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lStarParaPkg,"StarParaPkgClass",2));
          out[i] = lStarParaPkg;
        }else if( val.startsWith(Starflut.StarServicePrefix)){
          List<String> tags = val.split("+");
          StarServiceClass lStarService = getObjectFromRecordById(tags[1]);
          if( lStarService == null ){
            lStarService = new StarServiceClass(FrameTag,tags[0],tags[1]);
            Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lStarService,"StarServiceClass",2));
          }else{
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          out[i] = lStarService;
        }else if( val.startsWith(Starflut.StarValueBooleanPrefix_TRUE)){
          out[i] = true;
        }else if( val.startsWith(Starflut.StarValueBooleanPrefix_FALSE)){
          out[i] = false;
        }else{
          out[i] = val;
        }
      }else if( retVal is List){
        out[i] = processOutputArgs(FrameTag,retVal);
      }else if( retVal is Map){
        out[i] = processOutputArgs(FrameTag,retVal);
      }else{
        out[i] = retVal;
      }
    }
    if( isMap == false )
      return out;
    Map mo = new Map();
    for( int ii=0; ii < (out.length/2); ii++ ){
      mo[out[2*ii]] = out[2*ii+1];
    }
    return mo;
  }

  /*--constant begin--*/
  static const int VSINIT_OK = 0;
  static const int VSINIT_HAS = 1;
  static const int VSINIT_ERROR = -1;

  static const int SYNC_NOT = 0;
  static const int SYNC = 1;
  static const int SYNC_IN = 2;

  static const int FAULT_IND = 0;
  static const int FAULT_WARN = 1;
  static const int FAULT_NORMAL = 2;
  static const int FAULT_CRITICAL = 3;
  static const int FAULT_SYSTEM = 4;

  static const int ALLOC_STATIC = 1;
  static const int ALLOC_GLOBAL = 2;
  static const int ALLOC_CLIENT = 3;
  static const int ALLOC_LOCAL = 4;

  static const int SAVE_SAVE = 0;
  static const int SAVE_LOCAL = 1;
  static const int SAVE_GLOBAL = 2;
  static const int SAVE_NONE = 3;

  static const int ACTIVE_ALONE = 0;
  static const int ACTIVE_FOLLOW = 1;
  static const int ACTIVE_ACTIVE = 2;
  static const int ACTIVE_DEACTIVE = 3;

  static const int RCALL_OK = 0;
  static const int RCALL_COMMERROR = -1;
  static const int RCALL_OBJNOTEXIST = -2;
  static const int RCALL_FUNCNOTEXIST = -3;
  static const int RCALL_PARAERROR = -4;
  static const int RCALL_SYSERROR = -5;
  static const int RCALL_INVALIDUSR = -6;
  static const int RCALL_OVERTIME = -7;
  static const int RCALL_UNKNOWN = -8;

  static const int RCALLSRC_C = 0;
  static const int RCALLSRC_SCRIPT = 1;
  static const int RCALLSRC_WEBSERVICE = 2;

  static const int TYPE_BOOL = 1;
  static const int TYPE_INT8 = 2;
  static const int TYPE_UINT8 = 3;
  static const int TYPE_INT16 = 4;

  static const int TYPE_UINT16 = 5;
  static const int TYPE_INT32 = 6;
  static const int TYPE_UINT32 = 7;
  static const int TYPE_INT64 = 60;

  static const int TYPE_FLOAT = 8;
  static const int TYPE_DOUBLE = 58;
  static const int TYPE_LONG = 9;
  static const int TYPE_ULONG = 10;

  static const int TYPE_LONGHEX = 11;
  static const int TYPE_ULONGHEX = 12;
  static const int TYPE_VSTRING = 51;
  static const int TYPE_PTR = 14;

  static const int TYPE_MEMORY = 15;
  static const int TYPE_STRUCT = 16;
  static const int TYPE_COLOR = 19;
  static const int TYPE_RECT = 20;

  static const int TYPE_FONT = 21;
  static const int TYPE_TIME = 49;
  static const int TYPE_CHAR = 13;
  static const int TYPE_UUID = 41;

  static const int TYPE_STATICID = 29;
  static const int TYPE_CHARPTR = 30;
  static const int TYPE_PARAPKGPTR = 40;
  static const int TYPE_BINBUFPTR = 59;

  static const int TYPE_INT8PTR = 55;
  static const int TYPE_UINT8PTR = 54;
  static const int TYPE_INT16PTR = 31;
  static const int TYPE_UINT16PTR = 52;

  static const int TYPE_INT32PTR = 32;
  static const int TYPE_UINT32PTR = 53;
  static const int TYPE_INT64PTR = 62;
  static const int TYPE_FLOATPTR = 33;

  static const int TYPE_DOUBLEPTR = 63;
  static const int TYPE_ULONGPTR = 48;
  static const int TYPE_LONGPTR = 34;
  static const int TYPE_STRUCTPTR = 35;

  static const int TYPE_COLORPTR = 37;
  static const int TYPE_RECTPTR = 38;
  static const int TYPE_FONTPTR = 39;
  static const int TYPE_TIMEPTR = 50;

  static const int TYPE_UUIDPTR = 47;
  static const int TYPE_VOID = 254;
  static const int TYPE_OBJPTR = 57;
  static const int TYPE_TABLE = 56;

  static const int TYPE_UWORD = 61;
  static const int TYPE_UWORDPTR = 64;

  static const String INVALID_UUID = "00000000-0000-0000-0000-000000000000";

  static const int MSG_VSDISPMSG = 0x00000001;
  static const int MSG_VSDISPLUAMSG = 0x00000002;
  static const int MSG_DISPMSG = 0x00000003;
  static const int MSG_DISPLUAMSG = 0x00000004;

  static const int MSG_MESSAGEBOX = 0x00000005;
  static const int MSG_EXIT = 0x00000006;
  static const int MSG_GETWNDHANDLE = 0x00000007;
  static const int MSG_SETWNDSIZE = 0x00000008;

  static const int MSG_GETWNDSIZE = 0x00000009;
  static const int MSG_CLEARWND = 0x0000000A;
  static const int MSG_HIDEWND = 0x0000000B;
  static const int MSG_SHOWWND = 0x0000000C;

  static const int MSG_SETWNDBK = 0x0000000D;
  static const int MSG_SETFOCUS = 0x0000000E;
  static const int MSG_ISAPPACTIVE = 0x0000000F;
  static const int MSG_SETIDLEACTIVE = 0x00000010;

  static const int MSG_SETINFOCOLOR = 0x00000011;
  static const int MSG_SETINFOBK = 0x00000012;
  static const int MSG_KILLFOCUS = 0x00000013;
  static const int MSG_ONBEFORESTOPSERVICE = 0x000000020;

  static const int MSG_ONSTOPSERVICE = 0x000000021;
  static const int MSG_ONACTIVESERVICE = 0x00000022;
  static const int MSG_SAVESERVICE = 0x000000023;
  static const int MSG_SETMSGHOOK = 0x00000024;

  static const int MSG_GETMSGHOOK = 0x00000025;
  static const int MSG_HYPERLINK = 0x00000026;
  static const int MSG_SERVERTERM = 0x00000027;
  static const int MSG_APPEVENT = 0x00000028;

  static const int MSG_ISMANAGERVISIBLE = 0x00000030;
  static const int MSG_HIDEMANAGER = 0x00000031;
  static const int MSG_SHOWMANAGER = 0x00000032;
  static const int MSG_SETMANAGERCAPTION = 0x00000033;

  static const int MSG_GETMANAGERSIZE = 0x00000034;
  static const int MSG_GETMANAGERHANDLE = 0x00000035;
  static const int MSG_SHOWMANAGERSTATUSMENU = 0x00000036;
  static const int MSG_SETMANAGERSTYLE = 0x00000037;

  static const int MSG_MOVEMANAGER = 0x00000038;
  static const int MSG_GETMANAGERPOS = 0x00000039;
  static const int MSG_SETMANAGERSTATUS = 0x0000003A;
  static const int MSG_REDIRECTTOURLREQUEST = 0x00000040;

  static const int MSG_REDIRECTTOURLINFO = 0x00000041;
  static const int MSG_GETURLREQUEST = 0x00000050;
  static const int MSG_SETPROGRAMTYPE = 0x00000060;
  static const int MSG_ISWINDOWLESSSITE = 0x00000070;

  static const int MSG_ONTELNETSTRING = 0x0000007A;
  static const int MSG_ONTELNETSTRING_PREEXECUTE = 0x0000007B;

/*--constant end--*/
}



/*https://www.dartlang.org/tools/sdk */
/*https://github.com/dart-lang/sdk*/

class StarCoreBase{
  String starTag;
  String starId;  /*--valid for serviceclass or starobjectclass--*/
  String m_FrameTag;

  StarCoreBase(String FrameTag)
  {
    this.m_FrameTag = FrameTag;
  }

  void lock({String FrameTag = null})
  {
    if( starId == null )
      return; // not
    if( FrameTag != null && Starflut.starObjectRecordFrame.containsKey(FrameTag) == false ){
      throw 'Frame is ['+FrameTag+'] is not found, call lock failed.';
      return;
    }
    if(m_FrameTag == null ){
      if(FrameTag == null)
        return;
    }else{
      if(FrameTag != null && FrameTag == m_FrameTag )
        return;
    }
    StarObjectRecordClass ll = null;
    if(m_FrameTag == null){
      /*--from global frame--*/
      for (int i = 0; i < Starflut.starObjectRecordListRoot.length; i++) {
        if (identical(Starflut.starObjectRecordListRoot[i].starObject, this)) {
          ll = Starflut.starObjectRecordListRoot[i];
          Starflut.starObjectRecordListRoot.removeAt(i);
          break;
        }
      }
    }else{
      List<StarObjectRecordClass> starObjectRecordList = Starflut.starObjectRecordFrame[m_FrameTag];
      for( int i=0; i < starObjectRecordList.length; i++ ){
        if( identical( starObjectRecordList[i].starObject, this ) ){
          ll = starObjectRecordList[i];
          starObjectRecordList.removeAt(i);
          break;
        }
      }
    }
    m_FrameTag = FrameTag;
    if( ll == null )
        return;
    if(m_FrameTag == null){
      Starflut.starObjectRecordListRoot.add(ll);
    }else{
      List<StarObjectRecordClass> starObjectRecordList = Starflut.starObjectRecordFrame[m_FrameTag];
      starObjectRecordList.add(ll);
    }
    return;
  }

  void moveTo(String In_FrameTag)
  {
    lock(FrameTag:In_FrameTag);
  }

  void _free()
  {
    if( m_FrameTag == null) {
      /*--from global frame--*/
      for (int i = 0; i < Starflut.starObjectRecordListRoot.length; i++) {
        if (identical(Starflut.starObjectRecordListRoot[i].starObject, this)) {
          Starflut.starObjectRecordListRoot.removeAt(i);
          return;
        }
      }
      return; // not
    }
    if( Starflut.starObjectRecordFrame.containsKey(m_FrameTag) == false ){
      return;
    }
    List<StarObjectRecordClass> starObjectRecordList = Starflut.starObjectRecordFrame[m_FrameTag];
    m_FrameTag = null;
    for( int i=0; i < starObjectRecordList.length; i++ ){
      if( identical( starObjectRecordList[i].starObject, this ) ){
        starObjectRecordList.removeAt(i);
        return;
      }
    }
    return;
  }

  bool isEqual(StarCoreBase des)
  {
    return false;
  }
}

class StarObjectRecordClass{
  StarCoreBase starObject;
  //String codeLocation;
  DateTime lastAccessTag;
  String typeString;

  /*--from https://github.com/dart-lang/logging/issues/32--*/
  /*
  Frame _findCallerFrame(Trace trace) {
    bool foundLogging = false;
    for (int i = 0; i < trace.frames.length; ++i) {
      Frame frame = trace.frames[i];

      bool loggingPackage = frame.package == 'logging';
      if (foundLogging && !loggingPackage) {
        return frame;
      }
      foundLogging = loggingPackage;
    }
    return null;
  }
  */

  StarObjectRecordClass(StarCoreBase starObject,String typeString,int callerDeepth){
    this.starObject = starObject;
    lastAccessTag = new DateTime.now();
    this.typeString = typeString;
/*
    //--record current caller line number and file name
    Trace trace = new Trace.current();
    Frame callerFrame;
    if( trace.frames.length > callerDeepth )
      callerFrame = trace.frames[callerDeepth];
    else
      callerFrame = null;
    //print(trace);
    //print(callerFrame);
    if (callerFrame != null) {
      if (callerFrame.uri.pathSegments.isNotEmpty) {
        String filename = callerFrame.uri.pathSegments.last;
        String line = callerFrame.line != null ? '(${callerFrame.line})' : '';
        codeLocation = '$filename$line';
      }
    }
*/
    //print("new starobject[$typeString] : $codeLocation");
  }

}

/*------StarCoreFactory---*/
class StarCoreFactory extends StarCoreBase{

  StarCoreFactory(String starTag) : super(null)
  {
    this.starTag = starTag;
  }

  @override
  bool isEqual(StarCoreBase des)
  {
    if( identical(this,des ) )
      return true;
    return false;
  }

  Future<int> initCore(bool serverFlag,bool showMenuFlag,bool showClientWndFlag,bool sRPPrintFlag,String debugInterface,int debugPort,String clientInterface,int clientPort) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('starcore_InitCore',[serverFlag,showMenuFlag,showClientWndFlag,sRPPrintFlag,debugInterface,debugPort,clientInterface,clientPort]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return Starflut.VSINIT_ERROR;
    }
  }

  Future<StarSrvGroupClass> initSimpleEx(int clientPort,int webPort,List<String> dependService) async
  {
    try {
      await Starflut.gc();
      List paralist = [clientPort,webPort];
      paralist.addAll(dependService);
      String starTag = await Starflut.channel.invokeMethod('starcore_InitSimpleEx',paralist);
      if( starTag == null )
        return null;
      else{
        StarSrvGroupClass lSrvGroup = Starflut.getStarSrvGroupFromRecordByTag(starTag);
        if( lSrvGroup == null ){
          lSrvGroup = new StarSrvGroupClass(null,starTag);
          Starflut.starObjectRecordListRoot.add(new StarObjectRecordClass(lSrvGroup,"StarSrvGroupClass",2)); //---to root frame
        }
        return lSrvGroup;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  Future<StarServiceClass> initSimple(String serviceName,String servicePass,int clientPortNumber,int webPortNumber,List<String> dependService) async
  {
    try {
      await Starflut.gc();
      List paralist = [serviceName,servicePass,clientPortNumber,webPortNumber];
      paralist.addAll(dependService);
      String starTag = await Starflut.channel.invokeMethod('starcore_InitSimple',paralist);
      if( starTag == null )
        return null;
      else{
        List<String> tags = starTag.split("+");
        StarServiceClass lService = Starflut.getObjectFromRecordById(tags[1]);
        if( lService == null ){
          lService = new StarServiceClass(null,tags[0],tags[1]);
          Starflut.starObjectRecordListRoot.add(new StarObjectRecordClass(lService,"StarServiceClass",2));  //add to root
        }else{
          Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
        }
        return lService;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  Future<StarSrvGroupClass> getSrvGroup(Object serviceNameWithGroupID) async
  {
    try {
      await Starflut.gc();
      String starTag = await Starflut.channel.invokeMethod('starcore_GetSrvGroup',serviceNameWithGroupID);
      if( starTag == null )
        return null;
      else{
        StarSrvGroupClass lSrvGroup = Starflut.getStarSrvGroupFromRecordByTag(starTag);
        if( lSrvGroup == null ){
          lSrvGroup = new StarSrvGroupClass(null,starTag);
          Starflut.starObjectRecordListRoot.add(new StarObjectRecordClass(lSrvGroup,"StarSrvGroupClass",2));  //---to root frame
        }
        return lSrvGroup;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  Future<void> moduleExit() async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_moduleExit');
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  Future<void> moduleClear() async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_moduleClear');
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  Future<void> regMsgCallBackP(MsgCallBackProc callBack) async
  {
    try {
      await Starflut.gc();
      if( callBack == null){
        if( gMsgCallBack != null )
          await Starflut.channel.invokeMethod('starcore_regMsgCallBackP',false);
        gMsgCallBack = null;
      }else{
        if( gMsgCallBack == null )
          await Starflut.channel.invokeMethod('starcore_regMsgCallBackP',true);
        gMsgCallBack = callBack;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }
  MsgCallBackProc gMsgCallBack;

  Future<List> flutterMsgCallBack(int serviceGroupID, int uMsg,Object wParam,Object lParam) async
  {
    if( gMsgCallBack != null ){
      try{
        return await gMsgCallBack(serviceGroupID, uMsg, wParam,lParam);
      }
      on Exception{
      }
    }else{
    }
    return null;
  }

  /*--regDispatchRequestP--*/
  /*
  Future<void> regDispatchRequestP(DispatchRequestProc callBack) async
  {
    try {
      if( callBack == null){
        if( gDispatchRequestBack != null )
          await Starflut.channel.invokeMethod('starcore_regDispatchRequestP',false); 
        gDispatchRequestBack = null;                 
      }else{
        if( gDispatchRequestBack == null )
          await Starflut.channel.invokeMethod('starcore_regDispatchRequestP',true);
        gDispatchRequestBack = callBack;        
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }    
  }    
  DispatchRequestProc gDispatchRequestBack;

  void flutterDispatchRequestBack()
  {
    if( gDispatchRequestBack != null ){
      try{
        return gDispatchRequestBack();
      }
      on Exception{
      }
    }else{
    }      
  }
  */

  /*--sRPDispatch--*/
  Future<bool> sRPDispatch(bool waitFalg) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('starcore_sRPDispatch',waitFalg);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }

  /*--sRPLock--*/
  Future<void> sRPLock() async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_sRPLock');
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  /*--sRPUnLock--*/
  Future<void> sRPUnLock() async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_sRPUnLock');
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  /*--setRegisterCode--*/
  Future<bool> setRegisterCode(String codeString,bool single) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('starcore_setRegisterCode',[codeString,single]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }

  /*--isRegistered--*/
  Future<bool> isRegistered () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('starcore_isRegistered');
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }

  /*--setLocale--*/
  Future<void> setLocale(String lang) async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_setLocale',lang);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  /*--getLocale--*/
  Future<String> getLocale () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('starcore_getLocale');
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "";
    }
  }

  /*--version--*/
  Future<List> version () async
  {
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('starcore_version');
      return Starflut.processOutputArgs(null,result);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return [0,0,0];
    }
  }

  /*--getScriptIndex--*/
  Future<int> getScriptIndex (String interface) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('starcore_getScriptIndex',interface);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return 0;
    }
  }

  /*--starcore_setScript--*/
  Future<bool> setScript(String scriptInterface,String module, String para) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('starcore_setScript',[scriptInterface,module, para]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }

  /*--detachCurrentThread--*/
  Future<void> detachCurrentThread() async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_detachCurrentThread');
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  /*--coreHandle--*/
  Future<int> coreHandle () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('starcore_coreHandle');
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return 0;
    }
  }

  /*--captureScriptGIL--*/
  Future<void> captureScriptGIL() async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_captureScriptGIL');
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  /*--releaseScriptGIL--*/
  Future<void> releaseScriptGIL() async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_releaseScriptGIL');
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  /*--setShareLibraryPath--*/
  Future<void> setShareLibraryPath(String path) async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_setShareLibraryPath',path);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return;
    }
  }

}

/*------StarSrvGroupClass---*/
class StarSrvGroupClass extends StarCoreBase {

  StarSrvGroupClass(String FrameTag,String starTag) : super(FrameTag)
  {
    this.starTag = starTag;
  }

  @override
  bool isEqual(StarCoreBase des)
  {
    if( identical(this,des ) )
      return true;
    return false;
  }

  Future<String> getString() async
  {
    try {
      await Starflut.gc();
      String result = await Starflut.channel.invokeMethod('StarSrvGroupClass_toString',[this.starTag]);
      return result;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "Instance of 'StarSrvGroupClass'";
    }
  }

  /*--StarSrvGroupClass_createService--*/
  Future<StarServiceClass> createService(String FrameTag,String servicePath,String serviceName,String rootPass,int frameInterval,int netPkgSize,int uploadPkgSize,int downloadPkgSize,int dataUpPkgSize,int dataDownPkgSize,String derviceID) async
  {
    try {
      await Starflut.gc();
      String starTag = await Starflut.channel.invokeMethod('StarSrvGroupClass_createService',[this.starTag,servicePath,serviceName,rootPass,frameInterval,netPkgSize,uploadPkgSize,downloadPkgSize,dataUpPkgSize,dataDownPkgSize,derviceID]);
      if( starTag == null )
        return null;
      else{
        List<String> tags = starTag.split("+");
        StarServiceClass lService = Starflut.getObjectFromRecordById(tags[1]);
        if( lService == null ){
          lService = new StarServiceClass(FrameTag,tags[0],tags[1]);
          Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lService,"StarServiceClass",2));
        }else{
          Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
        }
        return lService;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }
  /*--StarSrvGroupClass_getService--*/
  Future<StarServiceClass> getService(String username, String userpassword) async
  {
    try {
      await Starflut.gc();
      String starTag = await Starflut.channel.invokeMethod('StarSrvGroupClass_getService',[this.starTag,username,userpassword]);
      if( starTag == null )
        return null;
      else{
        List<String> tags = starTag.split("+");
        StarServiceClass lService = Starflut.getObjectFromRecordById(tags[1]);
        if( lService == null ){
          lService = new StarServiceClass(null,tags[0],tags[1]);
          Starflut.starObjectRecordListRoot.add(new StarObjectRecordClass(lService,"StarServiceClass",2));  //add rooot
        }else{
          Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
        }
        return lService;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  /*--StarSrvGroupClass_clearService--*/
  Future<void> clearService() async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarSrvGroupClass_clearService',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  /*--StarSrvGroupClass_newParaPkg--*/
  Future<StarParaPkgClass> newParaPkg(String FrameTag,Object args) async
  {
    try {
      await Starflut.gc();
      String starTag = await Starflut.channel.invokeMethod('StarSrvGroupClass_newParaPkg',[this.starTag,args]);
      if( starTag == null )
        return null;
      else{
        StarParaPkgClass lParaPkg = new StarParaPkgClass(FrameTag,starTag);
        Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lParaPkg,"StarParaPkgClass",2));
        return lParaPkg;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  /*--StarSrvGroupClass_newBinBuf--*/
  Future<StarBinBufClass> newBinBuf(String FrameTag) async
  {
    try {
      await Starflut.gc();
      String starTag = await Starflut.channel.invokeMethod('StarSrvGroupClass_newBinBuf',[this.starTag]);
      if( starTag == null )
        return null;
      else{
        StarBinBufClass lBinBuf = new StarBinBufClass(FrameTag,starTag);
        Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lBinBuf,"StarBinBufClass",2));
        return lBinBuf;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  /*--StarSrvGroupClass_newSXml--*/
  /*
  Future<StarSXmlClass> newSXml() async
  {
     try {
        await Starflut.gc();
        String starTag = await Starflut.channel.invokeMethod('StarSrvGroupClass_newSXml',[this.starTag]);          
        if( starTag == null )
            return null;
        else{
          StarSXmlClass lSXml = new StarSXmlClass(starTag);
          Starflut.starObjectRecordList.add(new StarObjectRecordClass(lSXml,"StarSXmlClass",2));
          return lSXml;          
        }           
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }      
  }     
  */

  /*--StarSrvGroupClass_isObject--*/
  bool isObject(Object which)
  {
    if( which == null)
      return false;
    if(which is StarObjectClass)
      return true;
    return false;
  }
  bool isParaPkg(Object which)
  {
    if( which == null)
      return false;
    if(which is StarParaPkgClass)
      return true;
    return false;
  }
  bool isBinBuf(Object which)
  {
    if( which == null)
      return false;
    if(which is StarBinBufClass)
      return true;
    return false;
  }
  /* 
  bool isSXml(Object which)
  {
    if( which == null)
      return false;
    if(which is StarSXmlClass)
      return true;
    return false;
  }  
  */
  /*--StarSrvGroupClass_getServicePath--*/
  Future<String> getServicePath() async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_getServicePath',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }
  /*--StarSrvGroupClass_setServicePath--*/
  Future<void> setServicePath (String args) async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarSrvGroupClass_setServicePath',[this.starTag,args]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }
  /*--StarSrvGroupClass_servicePathIsSet--*/
  Future<bool> servicePathIsSet() async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_servicePathIsSet',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarSrvGroupClass_getCurrentPath--*/
  Future<String> getCurrentPath() async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_getCurrentPath',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }
  /*--StarSrvGroupClass_importService--*/
  Future<bool> importService (String serviceName,bool loadRunModule) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_importService',[this.starTag,serviceName,loadRunModule]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_clearServiceEx--*/
  Future<void> clearServiceEx() async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarSrvGroupClass_clearServiceEx',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  /*--StarSrvGroupClass_runScript--*/
  Future<bool> runScript (String scriptInterface,String scriptBuf,String moduleName) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_runScript',[this.starTag,scriptInterface,scriptBuf,moduleName]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_runScriptEx--*/
  Future<bool> runScriptEx (String scriptInterface,StarBinBufClass binBuf,String moduleName) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_runScriptEx',[this.starTag,scriptInterface,binBuf.starTag,moduleName]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_doFile--*/
  Future<bool> doFile (String scriptInterface,String fileName) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_doFile',[this.starTag,scriptInterface,fileName]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarSrvGroupClass_doFileEx--*/
  Future<bool> doFileEx (String scriptInterface,String fileName,String moduleName) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_doFileEx',[this.starTag,scriptInterface,fileName,moduleName]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarSrvGroupClass_setClientPort--*/
  Future<bool> setClientPort (String lInterface,int portnumber) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_setClientPort',[this.starTag,lInterface,portnumber]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarSrvGroupClass_setTelnetPort--*/
  Future<bool> setTelnetPort (int portnumber) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_setTelnetPort',[this.starTag,portnumber]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarSrvGroupClass_setOutputPort--*/
  Future<bool> setOutputPort (String host,int portnumber) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_setOutputPort',[this.starTag,host,portnumber]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarSrvGroupClass_setWebServerPort--*/
  Future<bool> setWebServerPort (String host,int portnumber,int connectionNumber,int postSize) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_setWebServerPort',[this.starTag,host,portnumber,connectionNumber,postSize]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarSrvGroupClass_initRaw--*/
  Future<bool> initRaw (String scriptInterface,StarServiceClass service) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_initRaw',[this.starTag,scriptInterface,service.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarSrvGroupClass_loadRawModule--*/
  Future<bool> loadRawModule (String scriptInterface,String moduleName,String fileOrString, bool isString) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_loadRawModule',[this.starTag,scriptInterface,moduleName,fileOrString,isString]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarSrvGroupClass_getLastError--*/
  Future<int> getLastError () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_getLastError',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return 0;
    }
  }
  /*--StarSrvGroupClass_getLastErrorInfo--*/
  Future<String> getLastErrorInfo () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_getLastErrorInfo',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "";
    }
  }
  /*--StarSrvGroupClass_getCorePath--*/
  Future<String> getCorePath () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_getCorePath',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "";
    }
  }
  /*--StarSrvGroupClass_getUserPath--*/
  Future<String> getUserPath () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_getUserPath',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "";
    }
  }
  /*--StarSrvGroupClass_getLocalIP--*/
  Future<String> getLocalIP () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_getLocalIP',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "127.0.0.1";
    }
  }
  /*--StarSrvGroupClass_getLocalIPEx--*/
  Future<List> getLocalIPEx () async
  {
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarSrvGroupClass_getLocalIPEx',[this.starTag]);
      return Starflut.processOutputArgs(null,result);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return ["127.0.0.1"];
    }
  }
  /*--StarSrvGroupClass_getObjectNum--*/
  Future<int> getObjectNum () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_getObjectNum',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return 0;
    }
  }
  /*--StarSrvGroupClass_activeScriptInterface--*/
  Future<List> activeScriptInterface (String scriptInterfaceName) async
  {
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarSrvGroupClass_activeScriptInterface',[this.starTag,scriptInterfaceName]);
      return Starflut.processOutputArgs(null,result);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return [false,false];
    }
  }
  /*--StarSrvGroupClass_preCompile--*/
  Future<List> preCompile (String scriptInterfaceName,String scriptSegment) async
  {
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarSrvGroupClass_preCompile',[this.starTag,scriptInterfaceName,scriptSegment]);
      return Starflut.processOutputArgs(null,result);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return [false,"error..."];
    }
  }


}

/*------StarServiceClass---*/
class StarServiceClass extends StarCoreBase {

  StarServiceClass(String FrameTag,String starTag,String starId) : super(FrameTag)
  {
    this.starTag = starTag;
    this.starId = starId;
  }

  @override
  bool isEqual(StarCoreBase des)
  {
    if( identical(this,des ) )
      return true;
    if( des is StarServiceClass ){
      StarServiceClass dd = des;
      if( dd.starId == starId )
        return true;
    }
    return false;
  }

  Future<String> getString() async
  {
    try {
      await Starflut.gc();
      String result = await Starflut.channel.invokeMethod('StarServiceClass_toString',[this.starTag]);
      return result;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "Instance of 'StarServiceClass'";
    }
  }

  /*--StarServiceClass_get--*/
  Future<Object> operator [](Object object) async{
    try {
      await Starflut.gc();
      Object retVal = await Starflut.channel.invokeMethod('StarServiceClass_get',[this.starTag,object]);
      if( retVal is String){
        String val = retVal;
        if( val.startsWith(Starflut.StarSrvGroupPrefix)){
          StarSrvGroupClass lSrvGroup = Starflut.getStarSrvGroupFromRecordByTag(val);
          if( lSrvGroup == null ){
            lSrvGroup = new StarSrvGroupClass(null,val);
            Starflut.starObjectRecordListRoot.add(new StarObjectRecordClass(lSrvGroup,"StarSrvGroupClass",2));  //---add root
          }
          return lSrvGroup;
        }
      }
      return retVal;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  /*--StarServiceClass_getObject--*/
  Future<StarObjectClass> getObject (String FrameTag,String objectName) async
  {
    try {
      await Starflut.gc();
      String lTag = await Starflut.channel.invokeMethod('StarServiceClass_getObject',[this.starTag,objectName]);
      if( lTag == null )
        return null;
      else{
        List<String> tags = lTag.split("+");
        StarObjectClass lObject = Starflut.getObjectFromRecordById(tags[1]);
        if( lObject == null ){
          lObject = new StarObjectClass(FrameTag,tags[0],tags[1]);
          Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lObject,"StarObjectClass",2));
        }else{
          Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
        }
        return lObject;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  /*--StarServiceClass_getObjectEx--*/
  Future<StarObjectClass> getObjectEx (String FrameTag,String objectID) async
  {
    try {
      await Starflut.gc();
      String lTag = await Starflut.channel.invokeMethod('StarServiceClass_getObjectEx',[this.starTag,objectID]);
      if( lTag == null )
        return null;
      else{
        List<String> tags = lTag.split("+");
        StarObjectClass lObject = Starflut.getObjectFromRecordById(tags[1]);
        if( lObject == null ){
          lObject = new StarObjectClass(FrameTag,tags[0],tags[1]);
          Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lObject,"StarObjectClass",2));
        }else{
          Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
        }
        return lObject;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  /*--StarServiceClass_newObject--*/
  Future<StarObjectClass> newObject (String FrameTag,List args) async
  {
    try {
      await Starflut.gc();
      String lTag = await Starflut.channel.invokeMethod('StarServiceClass_newObject',[this.starTag,Starflut.processInputArgs(args)]);
      if( lTag == null )
        return null;
      else{
        List<String> tags = lTag.split("+");
        StarObjectClass lObject = Starflut.getObjectFromRecordById(tags[1]);
        if( lObject == null ){
          lObject = new StarObjectClass(FrameTag,tags[0],tags[1]);
          Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lObject,"StarObjectClass",2));
        }else{
          Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
        }
        return lObject;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  /*--StarServiceClass_runScript--*/
  Future<List> runScript (String scriptInterface,String scriptBuf,String moduleName,String workDirectory) async
  {
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarServiceClass_runScript',[this.starTag,scriptInterface,scriptBuf,moduleName,workDirectory]);
      return Starflut.processOutputArgs(null,result);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return [false,"error..."];
    }
  }

  /*--StarServiceClass_runScriptEx--*/
  Future<List> runScriptEx (String scriptInterface,StarBinBufClass binBuf,String moduleName,String workDirectory) async
  {
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarServiceClass_runScriptEx',[this.starTag,scriptInterface,binBuf.starTag,moduleName,workDirectory]);
      return Starflut.processOutputArgs(null,result);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return [false,"error..."];
    }
  }
  /*--StarServiceClass_doFile--*/
  Future<List> doFile (String scriptInterface,String fileName,String workDirectory) async
  {
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarServiceClass_doFile',[this.starTag,scriptInterface,fileName,workDirectory]);
      return Starflut.processOutputArgs(null,result);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return [false,"error..."];
    }
  }
  /*--StarServiceClass_doFileEx--*/
  Future<List> doFileEx (String scriptInterface,String fileName,String workDirectory,String moduleName) async
  {
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarServiceClass_doFileEx',[this.starTag,scriptInterface,fileName,moduleName]);
      return Starflut.processOutputArgs(null,result);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return [false,"error..."];
    }
  }
  /*--StarServiceClass_isServiceRegistered--*/
  Future<bool> isServiceRegistered () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarServiceClass_isServiceRegistered',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarServiceClass_checkPassword--*/
  Future<void> checkPassword (bool flag) async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarServiceClass_checkPassword',[this.starTag,flag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }
  /*--StarServiceClass_newRawProxy--*/
  Future<StarObjectClass> newRawProxy (String FrameTag,String scriptInterface,StarObjectClass attachObject,String attachFunction,String proyInfo,int proxyType) async
  {
    try {
      await Starflut.gc();
      if( attachObject == null || (!(attachObject is StarObjectClass)) )
        return null;
      String lTag = await Starflut.channel.invokeMethod('StarServiceClass_newRawProxy',[this.starTag,scriptInterface,attachObject.starTag,attachFunction,proyInfo,proxyType]);
      if( lTag == null )
        return null;
      else{
        List<String> tags = lTag.split("+");
        StarObjectClass lObject = Starflut.getObjectFromRecordById(tags[1]);
        if( lObject == null ){
          lObject = new StarObjectClass(FrameTag,tags[0],tags[1]);
          Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lObject,"StarObjectClass",2));
        }else{
          Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
        }
        return lObject;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }
  /*--StarServiceClass_importRawContext--*/
  Future<StarObjectClass> importRawContext (String FrameTag,String scriptInterface,String contextName,bool isClass,String contextInfo) async
  {
    try {
      await Starflut.gc();
      String lTag = await Starflut.channel.invokeMethod('StarServiceClass_importRawContext',[this.starTag,scriptInterface,contextName,isClass,contextInfo]);
      if( lTag == null )
        return null;
      else{
        List<String> tags = lTag.split("+");
        StarObjectClass lObject = Starflut.getObjectFromRecordById(tags[1]);
        if( lObject == null ){
          lObject = new StarObjectClass(FrameTag,tags[0],tags[1]);
          Starflut.insertLocalFrame(FrameTag,new StarObjectRecordClass(lObject,"StarObjectClass",2));
        }else{
          Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
        }
        return lObject;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }
  /*--StarServiceClass_getLastError--*/
  Future<int> getLastError () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarServiceClass_getLastError',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return 0;
    }
  }
  /*--StarServiceClass_getLastErrorInfo--*/
  Future<String> getLastErrorInfo () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarServiceClass_getLastErrorInfo',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "";
    }
  }

  /*--StarServiceClass_allObject--*/
  Future<List> allObject (String FrameTag) async
  {
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarServiceClass_allObject',[this.starTag]);
      return Starflut.processOutputArgs(FrameTag,result);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return [false,"error..."];
    }
  }

  /*--StarServiceClass_restfulCall--*/
  Future<List> restfulCall (String FrameTag,String url, String opCode, String jsonString) async
  {
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarServiceClass_restfulCall',[this.starTag,url,opCode,jsonString]);
      return Starflut.processOutputArgs(FrameTag,result);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return [400,"{\"code\": -32600, \"message\": \"call _RestfulCall failed,input para error\"}"];
    }
  }

}

/*------StarParaPkgClass---*/
class StarParaPkgClass extends StarCoreBase {

  StarParaPkgClass(String FrameTag,String starTag) : super(FrameTag)
  {
    this.starTag = starTag;
  }

  @override
  bool isEqual(StarCoreBase des)
  {
    if( identical(this,des ) )
      return true;
    return false;
  }

  Future<String> getString() async
  {
    try {
      await Starflut.gc();
      String result = await Starflut.channel.invokeMethod('StarParaPkgClass_toString',[this.starTag]);
      return result;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "Instance of 'StarParaPkgClass'";
    }
  }

  Future<int> get number async
  {
    return await Starflut.channel.invokeMethod('StarParaPkgClass_GetNumber',[this.starTag]);
  }

  Future<String> get v async
  {
    return await Starflut.channel.invokeMethod('StarParaPkgClass_V',[this.starTag]);
  }

  /*--StarParaPkgClass_get--*/
  Future<Object> operator [](int index) async{
    try {
      await Starflut.gc();
      Object retVal = await Starflut.channel.invokeMethod('StarParaPkgClass_get',[this.starTag,index]);
      if( retVal is String){
        String val = retVal;
        if( val.startsWith(Starflut.StarObjectPrefix)){
          List<String> tags = val.split("+");
          StarObjectClass lStarObject = Starflut.getObjectFromRecordById(tags[1]);
          if( lStarObject == null ){
            lStarObject = new StarObjectClass(m_FrameTag,tags[0],tags[1]);
            Starflut.insertLocalFrame(m_FrameTag,new StarObjectRecordClass(lStarObject,"StarObjectClass",2));
          }else{
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          return lStarObject;
        }else if( val.startsWith(Starflut.StarBinBufPrefix)){
          StarBinBufClass lStarBinBuf = new StarBinBufClass(m_FrameTag,val);
          Starflut.insertLocalFrame(m_FrameTag,new StarObjectRecordClass(lStarBinBuf,"StarBinBufClass",2));
          return lStarBinBuf;
        }else if( val.startsWith(Starflut.StarParaPkgPrefix)){
          StarParaPkgClass lStarParaPkg = new StarParaPkgClass(m_FrameTag,val);
          Starflut.insertLocalFrame(m_FrameTag,new StarObjectRecordClass(lStarParaPkg,"StarParaPkgClass",2));
          return lStarParaPkg;
        }
      }
      return retVal;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }
  /*--StarParaPkgClass_clear--*/
  Future<StarParaPkgClass> clear () async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarParaPkgClass_clear',[this.starTag]);
      return this;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return this;
    }
  }
  /*--StarParaPkgClass_appendFrom--*/
  Future<bool> appendFrom (StarParaPkgClass srcParaPkg) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarParaPkgClass_appendFrom',[this.starTag,srcParaPkg.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }

  /*--StarParaPkgClass_equals--*/
  Future<bool> equals (StarParaPkgClass srcParaPkg) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarParaPkgClass_equals',[this.starTag,srcParaPkg.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }

  Future<Object> getValue(Object value) async{
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarParaPkgClass_get',[this.starTag,value]);
      if( (result is List) || (result is Map) ){
        return Starflut.processOutputArgs(m_FrameTag,result);
      }else{
        List rr = Starflut.processOutputArgs(m_FrameTag,[result]);
        return rr[0];
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  Future<StarParaPkgClass> setValue(int index,Object value) async{
    try {
      await Starflut.gc();
      if( value is StarObjectClass ){
        StarObjectClass vo = value;
        await Starflut.channel.invokeMethod('StarParaPkgClass_set',[this.starTag,index,vo.starTag]);
      }else if( value is StarParaPkgClass ){
        StarParaPkgClass vo = value;
        await Starflut.channel.invokeMethod('StarParaPkgClass_set',[this.starTag,index,vo.starTag]);
      }else if( value is StarBinBufClass ){
        StarBinBufClass vo = value;
        await Starflut.channel.invokeMethod('StarParaPkgClass_set',[this.starTag,index,vo.starTag]);
      }else{
        await Starflut.channel.invokeMethod('StarParaPkgClass_set',[this.starTag,index,value]);
      }
      return this;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return this;
    }
  }
  /*--StarParaPkgClass_build--*/
  Future<StarParaPkgClass> build (Object args) async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarParaPkgClass_build',[this.starTag,args]);
      return this;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return this;
    }
  }
  /*--StarParaPkgClass_free--*/
  Future<void> free () async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarParaPkgClass_free',[this.starTag]);
      _free();
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }
  /*--StarParaPkgClass_dispose--*/
  Future<void> dispose () async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarParaPkgClass_dispose',[this.starTag]);
      _free();
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }
  /*--StarParaPkgClass_releaseOwner--*/
  Future<void> releaseOwner () async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarParaPkgClass_releaseOwner',[this.starTag]);
      _free();
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }
  /*--StarParaPkgClass_asDict--*/
  Future<StarParaPkgClass> asDict (bool isDict) async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarParaPkgClass_asDict',[this.starTag,isDict]);
      return this;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return this;
    }
  }
  /*--StarParaPkgClass_isDict--*/
  Future<bool> isDict () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarParaPkgClass_isDict',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarParaPkgClass_fromJSon--*/
  Future<bool> fromJSon (String jsonstring) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarParaPkgClass_fromJSon',[this.starTag,jsonstring]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarParaPkgClass_toJSon--*/
  Future<String> toJSon () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarParaPkgClass_toJSon',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "";
    }
  }
  /*--StarParaPkgClass_toTuple--*/
  Future<Object> toTuple () async
  {
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarParaPkgClass_toTuple',[this.starTag]);
      if( result != null ) /*must be list or map--*/
      {
        return Starflut.processOutputArgs(m_FrameTag,result);
      }else{
        return null;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  /*--StarParaPkgClass_fromTuple--*/
  Future<StarParaPkgClass> fromTuple (Object args) async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarParaPkgClass_build',[this.starTag,args]);
      return this;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return this;
    }
  }
}

/*------StarBinBufClass---*/
class StarBinBufClass extends StarCoreBase {

  StarBinBufClass(String FrameTag,String starTag) : super(FrameTag)
  {
    this.starTag = starTag;
  }

  @override
  bool isEqual(StarCoreBase des)
  {
    if( identical(this,des ) )
      return true;
    return false;
  }

  Future<String> getString() async
  {
    try {
      await Starflut.gc();
      String result = await Starflut.channel.invokeMethod('StarBinBufClass_toString',[this.starTag]);
      return result;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "Instance of 'StarBinBufClass'";
    }
  }
  Future<int> get offset async
  {
    return await Starflut.channel.invokeMethod('StarBinBufClass_GetOffset',[this.starTag]);
  }
  /*--StarBinBufClass_init--*/
  Future<void> init (int bufSize) async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarBinBufClass_init',[this.starTag,bufSize]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }
  /*--StarBinBufClass_clear--*/
  Future<void> clear (int bufSize) async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarBinBufClass_clear',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }
  /*--StarBinBufClass_saveToFile--*/
  Future<bool> saveToFile (String fileName,bool txtFileFlag) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarBinBufClass_saveToFile',[this.starTag,fileName,txtFileFlag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarBinBufClass_loadFromFile--*/
  Future<bool> loadFromFile (String fileName,bool txtFileFlag) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarBinBufClass_loadFromFile',[this.starTag,fileName,txtFileFlag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarBinBufClass_read--*/
  Future<Uint8List> read (int offset,int length) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarBinBufClass_read',[this.starTag,offset,length]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return new Uint8List(0);
    }
  }
  /*--StarBinBufClass_write--*/
  Future<int> write (int offset,Uint8List buf,int length) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarBinBufClass_write',[this.starTag,offset,buf,length]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return 0;
    }
  }
  /*--StarBinBufClass_free--*/
  Future<void> free () async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarBinBufClass_free',[this.starTag]);
      _free();
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }
  /*--StarBinBufClass_dispose--*/
  Future<void> dispose () async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarBinBufClass_dispose',[this.starTag]);
      _free();
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }
  /*--StarBinBufClass_releaseOwner--*/
  Future<void> releaseOwner () async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarBinBufClass_releaseOwner',[this.starTag]);
      _free();
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  /*--StarBinBufClass_setOffset--*/
  Future<bool> setOffset (int Offset) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarBinBufClass_setOffset',[this.starTag,Offset]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }

  /*--StarBinBufClass_print--*/
  Future<void> print (String Arg) async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarBinBufClass_print',[this.starTag,Arg]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return;
    }
  }

  /*--StarBinBufClass_asString--*/
  Future<String> asString () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarBinBufClass_asString',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "";
    }
  }
}

/*------StarSXmlClass---*/
/*
class StarSXmlClass extends StarCoreBase {

  StarSXmlClass(String starTag)
  {
    this.starTag = starTag;
  }
}
*/

/*------StarObjectClass---*/
class StarObjectClass extends StarCoreBase {

  StarObjectClass(String FrameTag,String starTag,String starId) : super(FrameTag)
  {
    this.starTag = starTag;
    this.starId = starId;
    scriptCallBack = new Map<String,StarObjectScriptProc>();
  }

  @override
  bool isEqual(StarCoreBase des)
  {
    if( identical(this,des ) )
      return true;
    if( des is StarObjectClass ){
      StarObjectClass dd = des;
      if( dd.starId == starId )
        return true;
    }
    return false;
  }
  Future<String> getString() async
  {
    try {
      await Starflut.gc();
      String result = await Starflut.channel.invokeMethod('StarObjectClass_toString',[this.starTag]);
      return result;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "Instance of 'StarObjectClass'";
    }
  }
  /*--StarObjectClass_get--*/
  Future<Object> operator [](Object indexOrName) async{
    try {
      await Starflut.gc();
      Object retVal = await Starflut.channel.invokeMethod('StarObjectClass_get',[this.starTag,indexOrName]);
      if( retVal is String){
        String val = retVal;
        if( val.startsWith(Starflut.StarObjectPrefix)){
          List<String> tags = val.split("+");
          StarObjectClass lStarObject = Starflut.getObjectFromRecordById(tags[1]);
          if( lStarObject == null ){
            lStarObject = new StarObjectClass(m_FrameTag,tags[0],tags[1]);
            Starflut.insertLocalFrame(m_FrameTag,new StarObjectRecordClass(lStarObject,"StarObjectClass",2));
          }else{
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          return lStarObject;
        }else if( val.startsWith(Starflut.StarBinBufPrefix)){
          StarBinBufClass lStarBinBuf = new StarBinBufClass(m_FrameTag,val);
          Starflut.insertLocalFrame(m_FrameTag,new StarObjectRecordClass(lStarBinBuf,"StarBinBufClass",2));
          return lStarBinBuf;
        }else if( val.startsWith(Starflut.StarParaPkgPrefix)){
          StarParaPkgClass lStarParaPkg = new StarParaPkgClass(m_FrameTag,val);
          Starflut.insertLocalFrame(m_FrameTag,new StarObjectRecordClass(lStarParaPkg,"StarParaPkgClass",2));
          return lStarParaPkg;
        }else if( val.startsWith(Starflut.StarServicePrefix)){
          List<String> tags = val.split("+");
          StarServiceClass lStarService = Starflut.getObjectFromRecordById(tags[1]);
          if( lStarService == null ){
            lStarService = new StarServiceClass(m_FrameTag,tags[0],tags[1]);
            Starflut.insertLocalFrame(m_FrameTag,new StarObjectRecordClass(lStarService,"StarServiceClass",2));
          }else{
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          return lStarService;
        }
      }
      return retVal;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  /*--StarObjectClass_get--*/
  Future<Object> getValue(Object indexOrNameOrList) async{
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarObjectClass_get',[this.starTag,indexOrNameOrList]);
      if( (result is List) || (result is Map) ){
        return Starflut.processOutputArgs(m_FrameTag,result);
      }else{
        List rr = Starflut.processOutputArgs(m_FrameTag,[result]);
        return rr[0];
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  Future<void> setValue(Object indexOrName,Object value) async{
    try {
      await Starflut.gc();
      if( value is StarObjectClass ){
        StarObjectClass vo = value;
        await Starflut.channel.invokeMethod('StarObjectClass_set',[this.starTag,indexOrName,vo.starTag]);
      }else if( value is StarParaPkgClass ){
        StarParaPkgClass vo = value;
        await Starflut.channel.invokeMethod('StarObjectClass_set',[this.starTag,indexOrName,vo.starTag]);
      }else if( value is StarBinBufClass ){
        StarBinBufClass vo = value;
        await Starflut.channel.invokeMethod('StarObjectClass_set',[this.starTag,indexOrName,vo.starTag]);
      }else{
        await Starflut.channel.invokeMethod('StarObjectClass_set',[this.starTag,indexOrName,value]);
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  /*--StarObjectClass_call--*/
  Future<Object> call (String funcName,List args) async
  {
    try {
      await Starflut.gc();
      if( funcName == null)
        funcName = "_StarCall";
      Object retVal = await Starflut.channel.invokeMethod('StarObjectClass_call',[this.starTag,funcName,Starflut.processInputArgs(args)]);
      if( retVal is String){
        String val = retVal;
        if( val.startsWith(Starflut.StarObjectPrefix)){
          List<String> tags = val.split("+");
          StarObjectClass lStarObject = Starflut.getObjectFromRecordById(tags[1]);
          if( lStarObject == null ){
            lStarObject = new StarObjectClass(m_FrameTag,tags[0],tags[1]);
            Starflut.insertLocalFrame(m_FrameTag,new StarObjectRecordClass(lStarObject,"StarObjectClass",2));
          }else{
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          return lStarObject;
        }else if( val.startsWith(Starflut.StarBinBufPrefix)){
          StarBinBufClass lStarBinBuf = new StarBinBufClass(m_FrameTag,val);
          Starflut.insertLocalFrame(m_FrameTag,new StarObjectRecordClass(lStarBinBuf,"StarBinBufClass",2));
          return lStarBinBuf;
        }else if( val.startsWith(Starflut.StarParaPkgPrefix)){
          StarParaPkgClass lStarParaPkg = new StarParaPkgClass(m_FrameTag,val);
          Starflut.insertLocalFrame(m_FrameTag,new StarObjectRecordClass(lStarParaPkg,"StarParaPkgClass",2));
          return lStarParaPkg;
        }else if( val.startsWith(Starflut.StarServicePrefix)){
          List<String> tags = val.split("+");
          StarServiceClass lStarService = Starflut.getObjectFromRecordById(tags[1]);
          if( lStarService == null ){
            lStarService = new StarServiceClass(m_FrameTag,tags[0],tags[1]);
            Starflut.insertLocalFrame(m_FrameTag,new StarObjectRecordClass(lStarService,"StarServiceClass",2));
          }else{
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          return lStarService;
        }
      }
      return retVal;
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }

  /*--StarObjectClass_newObject--*/
  Future<StarObjectClass> newObject (List args) async
  {
    try {
      await Starflut.gc();
      String lTag = await Starflut.channel.invokeMethod('StarObjectClass_newObject',[this.starTag,Starflut.processInputArgs(args)]);
      if( lTag == null )
        return null;
      else{
        List<String> tags = lTag.split("+");
        StarObjectClass lObject = Starflut.getObjectFromRecordById(tags[1]);
        if( lObject == null ){
          lObject = new StarObjectClass(m_FrameTag,tags[0],tags[1]);
          Starflut.insertLocalFrame(m_FrameTag,new StarObjectRecordClass(lObject,"StarObjectClass",2));
        }else{
          Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
        }
        return lObject;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }
  /*--StarObjectClass_free--*/
  Future<void> free () async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarObjectClass_free',[this.starTag]);
      _free();
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }
  /*--StarObjectClass_dispose--*/
  Future<void> dispose () async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarObjectClass_dispose',[this.starTag]);
      _free();
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }
  /*--StarObjectClass_hasRawContext--*/
  Future<bool> hasRawContext () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarObjectClass_hasRawContext',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }
  /*--StarObjectClass_rawToParaPkg--*/
  Future<Object> rawToParaPkg () async
  {
    try {
      await Starflut.gc();
      Object result = await Starflut.channel.invokeMethod('StarObjectClass_rawToParaPkg',[this.starTag]);
      if( result != null ){
        return Starflut.processOutputArgs(m_FrameTag,result);
      }else{
        return null;
      }
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return null;
    }
  }
  /*--StarObjectClass_getSourceScript--*/
  Future<int> getSourceScript () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarObjectClass_getSourceScript',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return 0;
    }
  }
  Future<int> getLastError () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarObjectClass_getLastError',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return 0;
    }
  }
  /*--StarObjectClass_getLastErrorInfo--*/
  Future<String> getLastErrorInfo () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarObjectClass_getLastErrorInfo',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "";
    }
  }
  /*--StarObjectClass_releaseOwnerEx--*/
  Future<void> releaseOwnerEx () async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarObjectClass_releaseOwnerEx',[this.starTag]);
      _free();
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }
  /*--StarObjectClass_regScriptProcP--*/
  Map<String,StarObjectScriptProc> scriptCallBack;
  Future<void> regScriptProcP(String scriptName,StarObjectScriptProc callBack) async
  {
    try {
      await Starflut.gc();
      if( callBack == null){
        if( scriptCallBack[scriptName] != null )
          await Starflut.channel.invokeMethod('StarObjectClass_regScriptProcP',[this.starTag,scriptName,false]);
        scriptCallBack.remove(scriptName);
      }else{
        if( scriptCallBack[scriptName] == null )
          await Starflut.channel.invokeMethod('StarObjectClass_regScriptProcP',[this.starTag,scriptName,true]);
        scriptCallBack[scriptName] = callBack;
      }

      lock();

    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  /*--StarObjectClass_instNumber--*/
  Future<int> instNumber () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarObjectClass_instNumber',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return 0;
    }
  }

  /*--StarObjectClass_changeParent--*/
  Future<void> changeParent (StarObjectClass parentObject,String queueName) async
  {
    try {
      await Starflut.gc();
      if( parentObject == null || (!(parentObject is StarObjectClass)) )
        await Starflut.channel.invokeMethod('StarObjectClass_changeParent',[this.starTag,null,queueName]);
      else
        await Starflut.channel.invokeMethod('StarObjectClass_changeParent',[this.starTag,parentObject.starTag,queueName]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
    }
  }

  /*--StarObjectClass_jsonCall--*/
  Future<String> jsonCall (String jsonString) async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarObjectClass_jsonCall',[this.starTag,jsonString]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return "{\"jsonrpc\": \"2.0\", \"error\": {\"code\": -32603, \"message\": \"call _JSonCall failed,Internal error\"}, \"id\": null}";
    }
  }

  /*--StarObjectClass_active--*/
  Future<bool> active () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarObjectClass_active',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }

  /*--StarObjectClass_deActive--*/
  Future<void> deActive () async
  {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('StarObjectClass_deActive',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return;
    }
  }

  /*--StarObjectClass_isActive--*/
  Future<bool> isActive () async
  {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarObjectClass_isActive',[this.starTag]);
    }
    on PlatformException catch (e){
      print( "{$e.message}");
      return false;
    }
  }

}

/*

  static _checkType(object) {

    if ((object == null) ||

        (object is bool) ||

        (object is num) ||

        (object is String)) {

      throw new ArgumentError.value(object,

          "Expandos are not allowed on strings, numbers, booleans or null");

    }

  }

    Isolate isd = Isolate.current; 

void Operator []=(Object object,T Value){

} 

T Operator [](Object object){

}

get _size => _data.length;
*/  
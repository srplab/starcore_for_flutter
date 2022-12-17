import 'dart:core';
import 'package:starflut/src/star_para_pkg.dart';
import 'package:starflut/src/star_service.dart';
import 'package:starflut/src/starcore_base.dart';
import 'package:starflut/src/starcore_factory.dart';
import 'package:starflut/src/starobject_record.dart';
import 'package:starflut/src/starsrv_group.dart';
import 'package:uuid/uuid.dart';
import 'package:flutter/services.dart';
import 'package:starflut/src/starbin_buf.dart';
import 'package:starflut/src/star_object.dart';

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

  static StarCoreFactory? starcore;

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

  static const String StarCorePrefix = "@s_s_c";
  static const String StarSrvGroupPrefix = "@s_s_g";
  static const String StarServicePrefix = "@s_s_e";
  static const String StarParaPkgPrefix = "@s_s_p";
  static const String StarBinBufPrefix = "@s_s_b";
  //static const String StarSXmlPrefix  =  "@s_s_x";
  static const String StarObjectPrefix = "@s_s_o";
  static const String StarValueBooleanPrefix_TRUE =
      "@s_s_t_bool_true"; /*-fix bug for return boolean in arraylist or hashmap-*/
  static const String StarValueBooleanPrefix_FALSE = "@s_s_t_bool_false";

  static Map<String, List<StarObjectRecordClass?>> starObjectRecordFrame = {};
  static String? starObjectRecordListRootKey;
  static late List<StarObjectRecordClass?> starObjectRecordListRoot;
  static List<String?> starObjectRecordFrameWaitFree =
      new List.filled(0, null, growable: true);

  static const MethodChannel channel = const MethodChannel('starflut');

  static Future<StarCoreFactory?> getFactory() async {
    if (starcore == null) {
      List<StarObjectRecordClass?> ll =
          new List.filled(0, null, growable: true);
      var uuid = Uuid();
      starObjectRecordListRootKey = uuid.v1();
      // create root frame
      starObjectRecordFrame[starObjectRecordListRootKey!] = ll;
      starObjectRecordListRoot = ll;

      channel.setMethodCallHandler(handler);
      String? starTag = await channel.invokeMethod('starcore_init');
      if (starTag == null || starTag.length == 0) return null;
      starcore = new StarCoreFactory(starTag);
      return starcore;
    } else {
      return starcore;
    }
  }

  static pushLocalFrame() {
    throw 'pushLocalFrame does not supported from v1.0.0. please use newLocalFrame!';
  }

  static void popLocalFrame(String frameTag) {
    throw 'popLocalFrame does not supported from v1.0.0. please use freeLocalFrame!';
  }

  static String newLocalFrame() {
    if (starcore == null) {
      throw 'newLocalFrame failed. Starflut.getFactory must be called first!';
    }
    var uuid = Uuid();
    String frameTag = uuid.v1();
    List<StarObjectRecordClass?> ll = new List.filled(0, null, growable: true);
    starObjectRecordFrame[frameTag] = ll; // create frame
    return frameTag;
  }

  static void freeLocalFrame(String frameTag) {
    if (starObjectRecordFrame.containsKey(frameTag) == false) {
      return; /*--no action--*/
    }
    List<StarObjectRecordClass?> starObjectRecordList =
        starObjectRecordFrame[frameTag]!;
    for (int i = 0; i < starObjectRecordList.length; i++) {
      starObjectRecordFrameWaitFree
          .add(starObjectRecordList[i]!.starObject!.starTag);
    }
    //print("free localframe   "+frameTag);
    starObjectRecordFrame.remove(frameTag);
  }

  static void insertLocalFrame(String? frameTag, StarObjectRecordClass record) {
    if (frameTag == null || frameTag.length == 0)
      starObjectRecordListRoot.add(record);
    else {
      if (starObjectRecordFrame.containsKey(frameTag) == false) {
        return; /*--no action--*/
      }
      List<StarObjectRecordClass?> starObjectRecordList =
          starObjectRecordFrame[frameTag]!;
      starObjectRecordList.add(record);
    }
  }

  static Future<void> gc() async {
    try {
      if (Starflut.starObjectRecordFrameWaitFree.length != 0) {
        List<String?> starObjectRecordFrameWaitFreeLocal =
            Starflut.starObjectRecordFrameWaitFree;
        Starflut.starObjectRecordFrameWaitFree =
            new List.filled(0, null, growable: true);
        await Starflut.channel.invokeMethod(
            'starcore_poplocalframe', starObjectRecordFrameWaitFreeLocal);
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  static StarSrvGroupClass? getStarSrvGroupFromRecordByTag(String tag) {
    for (int i = 0; i < starObjectRecordListRoot.length; i++) {
      if (starObjectRecordListRoot[i]!.starObject!.starTag == tag) {
        return starObjectRecordListRoot[i]!.starObject as StarSrvGroupClass?;
      }
    }
    return null;
  }

  static StarCoreBase? getObjectFromRecordById(String id) {
    for (int i = 0; i < starObjectRecordListRoot.length; i++) {
      if (starObjectRecordListRoot[i]!.starObject!.starId == id) {
        return starObjectRecordListRoot[i]!.starObject;
      }
    }
    return null;
  }

  static Future<List?> typeCheck(List value) async {
    return await channel.invokeMethod('starcore_typeCheck', value);
  }

  static Future<String?> getNativeLibraryDir() async {
    return await channel.invokeMethod('starcore_nativeLibraryDir');
  }

  static Future<String?> getDocumentPath() async {
    return await channel.invokeMethod('starcore_getDocumentPath');
  }

  static Future<String?> getResourcePath() async {
    return await channel.invokeMethod('starcore_getResourcePath');
  }

  static Future<String?> getAssetsPath() async {
    return await channel.invokeMethod('starcore_getAssetsPath');
  }

  static Future<String?> rubyInitExt() async {
    return await channel.invokeMethod('starcore_rubyInitExt');
  }

  static Future<String?> getPackageName() async {
    return await channel.invokeMethod('starcore_getPackageName');
  }

  static Future<bool?> unzipFromAssets(
      String fileName, String desPath, bool overWriteFlag) async {
    return await channel.invokeMethod(
        'starcore_unzipFromAssets', [fileName, desPath, overWriteFlag]);
  }

  static Future<bool?> copyFileFromAssets(
      String name, String? srcRelatePath, String? desRelatePath) async {
    return await channel.invokeMethod('starcore_copyFileFromAssets',
        [name, srcRelatePath, desRelatePath, true]);
  }

  static Future<bool?> copyFileFromAssetsEx(String name, String srcRelatePath,
      String desRelatePath, bool overwriteIfExist) async {
    return await channel.invokeMethod('starcore_copyFileFromAssets',
        [name, srcRelatePath, desRelatePath, overwriteIfExist]);
  }

  static Future<bool?> isAndroid() async {
    return await channel.invokeMethod('starcore_isAndroid');
  }

  static Future<int?> getPlatform() async {
    return await channel.invokeMethod('starcore_getPlatform');
  }

  static Future<bool?> loadLibrary(String name) async {
    return await channel.invokeMethod('starcore_loadLibrary', [name]);
  }

  static Future<bool?> setEnv(String name, String value) async {
    return await channel.invokeMethod('starcore_setEnv', [name, value]);
  }

  static Future<String?> getEnv(String name) async {
    return await channel.invokeMethod('starcore_getEnv', name);
  }

  static Future<StarObjectClass?> getActivity(StarServiceClass service) async {
    try {
      String? lTag =
          await channel.invokeMethod('starcore_getActivity', [service.starTag]);
      if (lTag == null)
        return null;
      else {
        List<String> tags = lTag.split("+");
        StarObjectClass? lObject =
            getObjectFromRecordById(tags[1]) as StarObjectClass?;
        if (lObject == null) {
          lObject = new StarObjectClass(null, tags[0], tags[1]);
          Starflut.insertLocalFrame(
              null, new StarObjectRecordClass(lObject, "StarObjectClass", 2));
        } else {
          Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
        }
        return lObject;
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  /*--callback function from java--*/
  static Future<dynamic> handler(MethodCall call) async {
    String method = call.method;

    switch (method) {
      case "starcore_msgCallBack":
        {
          List lL = call.arguments;
          try {
            List? result =
                await starcore!.flutterMsgCallBack(lL[0], lL[1], lL[2], lL[3]);
            return [lL[4], result];
          } on Exception catch (e) {
            print("$e");
            return [lL[4], null];
          }
        }

      case "starobjectclass_scriptproc":
        {
          List lL = call.arguments;

          String frameTag = Starflut.newLocalFrame();

          List<String> tags = lL[0].split("+");

          StarObjectClass? lObject =
              getObjectFromRecordById(tags[1]) as StarObjectClass?;
          if (lObject == null) {
            lObject = new StarObjectClass(frameTag, tags[0], tags[1]);
            Starflut.insertLocalFrame(frameTag,
                new StarObjectRecordClass(lObject, "StarObjectClass", 2));
          } else {
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          String funcName = lL[1];
          List? args = processOutputArgs(frameTag, lL[2]) as List<dynamic>?;
          if (!lObject.scriptCallBack.containsKey(funcName)) {
            Starflut.freeLocalFrame(frameTag);
            return [lL[3], funcName, null];
          }
          Object? result;
          try {
            result = await lObject.scriptCallBack[funcName]!(
                lObject, frameTag, args);
          } on Exception catch (e) {
            print("$e");
            result = null;
          }
          if (result == null) {
            Starflut.freeLocalFrame(frameTag);
            return [lL[3], funcName, null];
          }
          if (result is List) {
            List returnResult = processInputArgs(
                result); /*if object is return, it will be locked, should not be freed by popLocalFrame*/
            return [
              lL[3],
              funcName,
              [frameTag, returnResult]
            ];
          } else {
            List resultList = new List.filled(1, result);
            List returnResult = processInputArgs(resultList);
            return [
              lL[3],
              funcName,
              [frameTag, returnResult[0]]
            ];
          }
        }
      case "starobjectclass_scriptproc_freeLlocalframe":
        {
          List lL = call.arguments;
          String frameTag = lL[0];
          Starflut.freeLocalFrame(frameTag);
        }
    }
    return null;
  }

  static List processInputArgs(List args) {
    return processInputArgsWithlock(args, false);
  }

  static List processInputArgsWithlock(List args, bool lockObject) {
    List out = new List.filled(args.length, null);
    for (int i = 0; i < args.length; i++) {
      Object value = args[i];
      if (value is StarObjectClass) {
        StarObjectClass vo = value;
        if (lockObject == true) vo.lock();
        out[i] = vo.starTag;
      } else if (value is StarParaPkgClass) {
        StarParaPkgClass vo = value;
        if (lockObject == true) vo.lock();
        out[i] = vo.starTag;
      } else if (value is StarBinBufClass) {
        StarBinBufClass vo = value;
        if (lockObject == true) vo.lock();
        out[i] = vo.starTag;
      } else if (value is StarServiceClass) {
        StarServiceClass vo = value;
        if (lockObject == true) vo.lock();
        out[i] = vo.starTag;
      } else {
        out[i] = value;
      }
    }
    return out;
  }

  static Object? processOutputArgs(String? frameTag, Object? argsListOrMap) {
    List args;
    bool isMap = false;
    if (argsListOrMap is List) {
      args = argsListOrMap;
    } else if (argsListOrMap is Map) {
      Map argMap = argsListOrMap;
      args = [];
      for (var item in argMap.entries) {
        args.add(item.key);
        args.add(item.value);
      }
      isMap = true;
    } else {
      return argsListOrMap;
    }
    List out = new List.filled(args.length, null);
    for (int i = 0; i < args.length; i++) {
      Object retVal = args[i];
      if (retVal is String) {
        String val = retVal;
        if (val.startsWith(Starflut.StarObjectPrefix)) {
          List<String> tags = val.split("+");
          StarObjectClass? lStarObject =
              getObjectFromRecordById(tags[1]) as StarObjectClass?;
          if (lStarObject == null) {
            lStarObject = new StarObjectClass(frameTag, tags[0], tags[1]);
            Starflut.insertLocalFrame(frameTag,
                new StarObjectRecordClass(lStarObject, "StarObjectClass", 2));
          } else {
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          out[i] = lStarObject;
        } else if (val.startsWith(Starflut.StarBinBufPrefix)) {
          StarBinBufClass lStarBinBuf = new StarBinBufClass(frameTag, val);
          Starflut.insertLocalFrame(frameTag,
              new StarObjectRecordClass(lStarBinBuf, "StarBinBufClass", 2));
          out[i] = lStarBinBuf;
        } else if (val.startsWith(Starflut.StarParaPkgPrefix)) {
          StarParaPkgClass lStarParaPkg = new StarParaPkgClass(frameTag, val);
          Starflut.insertLocalFrame(frameTag,
              new StarObjectRecordClass(lStarParaPkg, "StarParaPkgClass", 2));
          out[i] = lStarParaPkg;
        } else if (val.startsWith(Starflut.StarServicePrefix)) {
          List<String> tags = val.split("+");
          StarServiceClass? lStarService =
              getObjectFromRecordById(tags[1]) as StarServiceClass?;
          if (lStarService == null) {
            lStarService = new StarServiceClass(frameTag, tags[0], tags[1]);
            Starflut.insertLocalFrame(frameTag,
                new StarObjectRecordClass(lStarService, "StarServiceClass", 2));
          } else {
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          out[i] = lStarService;
        } else if (val.startsWith(Starflut.StarValueBooleanPrefix_TRUE)) {
          out[i] = true;
        } else if (val.startsWith(Starflut.StarValueBooleanPrefix_FALSE)) {
          out[i] = false;
        } else {
          out[i] = val;
        }
      } else if (retVal is List) {
        out[i] = processOutputArgs(frameTag, retVal);
      } else if (retVal is Map) {
        out[i] = processOutputArgs(frameTag, retVal);
      } else {
        out[i] = retVal;
      }
    }
    if (isMap == false) return out;
    Map mo = new Map();
    for (int ii = 0; ii < (out.length / 2); ii++) {
      mo[out[2 * ii]] = out[2 * ii + 1];
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

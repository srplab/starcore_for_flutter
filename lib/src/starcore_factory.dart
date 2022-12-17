/*------StarCoreFactory---*/
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:starflut/src/star_service.dart';
import 'package:starflut/src/starcore_base.dart';
import 'package:starflut/src/starobject_record.dart';
import 'package:starflut/src/starsrv_group.dart';
import 'package:starflut/src/type_defs.dart';
import 'package:starflut/src/starflut.dart';

class StarCoreFactory extends StarCoreBase {
  StarCoreFactory(String starTag) : super(null) {
    this.starTag = starTag;
  }

  @override
  bool isEqual(StarCoreBase des) {
    if (identical(this, des)) return true;
    return false;
  }

  Future<int?> initCore(
      bool serverFlag,
      bool showMenuFlag,
      bool showClientWndFlag,
      bool sRPPrintFlag,
      String debugInterface,
      int debugPort,
      String clientInterface,
      int clientPort) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('starcore_InitCore', [
        serverFlag,
        showMenuFlag,
        showClientWndFlag,
        sRPPrintFlag,
        debugInterface,
        debugPort,
        clientInterface,
        clientPort
      ]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return Starflut.VSINIT_ERROR;
    }
  }

  Future<StarSrvGroupClass?> initSimpleEx(
      int clientPort, int webPort, List<String> dependService) async {
    try {
      await Starflut.gc();
      List paralist = [clientPort, webPort];
      paralist.addAll(dependService);
      String? starTag = await Starflut.channel
          .invokeMethod('starcore_InitSimpleEx', paralist);
      if (starTag == null)
        return null;
      else {
        StarSrvGroupClass? lSrvGroup =
            Starflut.getStarSrvGroupFromRecordByTag(starTag);
        if (lSrvGroup == null) {
          lSrvGroup = new StarSrvGroupClass(null, starTag);
          Starflut.starObjectRecordListRoot.add(new StarObjectRecordClass(
              lSrvGroup, "StarSrvGroupClass", 2)); //---to root frame
        }
        return lSrvGroup;
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  Future<StarServiceClass?> initSimple(
      String serviceName,
      String servicePass,
      int clientPortNumber,
      int webPortNumber,
      List<String> dependService) async {
    try {
      await Starflut.gc();
      List paralist = [
        serviceName,
        servicePass,
        clientPortNumber,
        webPortNumber
      ];
      paralist.addAll(dependService);
      String? starTag =
          await Starflut.channel.invokeMethod('starcore_InitSimple', paralist);
      if (starTag == null)
        return null;
      else {
        List<String> tags = starTag.split("+");
        StarServiceClass? lService =
            Starflut.getObjectFromRecordById(tags[1]) as StarServiceClass?;
        if (lService == null) {
          lService = new StarServiceClass(null, tags[0], tags[1]);
          Starflut.starObjectRecordListRoot.add(new StarObjectRecordClass(
              lService, "StarServiceClass", 2)); //add to root
        } else {
          Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
        }
        return lService;
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  Future<StarSrvGroupClass?> getSrvGroup(Object serviceNameWithGroupID) async {
    try {
      await Starflut.gc();
      String? starTag = await Starflut.channel
          .invokeMethod('starcore_GetSrvGroup', serviceNameWithGroupID);
      if (starTag == null)
        return null;
      else {
        StarSrvGroupClass? lSrvGroup =
            Starflut.getStarSrvGroupFromRecordByTag(starTag);
        if (lSrvGroup == null) {
          lSrvGroup = new StarSrvGroupClass(null, starTag);
          Starflut.starObjectRecordListRoot.add(new StarObjectRecordClass(
              lSrvGroup, "StarSrvGroupClass", 2)); //---to root frame
        }
        return lSrvGroup;
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  Future<void> moduleExit() async {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_moduleExit');
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  Future<void> moduleClear() async {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_moduleClear');
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  Future<void> regMsgCallBackP(MsgCallBackProc? callBack) async {
    try {
      await Starflut.gc();
      if (callBack == null) {
        if (gMsgCallBack != null)
          await Starflut.channel
              .invokeMethod('starcore_regMsgCallBackP', false);
        gMsgCallBack = null;
      } else {
        if (gMsgCallBack == null)
          await Starflut.channel.invokeMethod('starcore_regMsgCallBackP', true);
        gMsgCallBack = callBack;
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  MsgCallBackProc? gMsgCallBack;

  Future<List?> flutterMsgCallBack(
      int serviceGroupID, int uMsg, Object wParam, Object lParam) async {
    if (gMsgCallBack != null) {
      try {
        return await gMsgCallBack!(serviceGroupID, uMsg, wParam, lParam);
      } on Exception {}
    } else {}
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
  Future<bool?> sRPDispatch(bool waitFalg) async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('starcore_sRPDispatch', waitFalg);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--sRPLock--*/
  Future<void> sRPLock() async {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_sRPLock');
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--sRPUnLock--*/
  Future<void> sRPUnLock() async {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_sRPUnLock');
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--setRegisterCode--*/
  Future<bool?> setRegisterCode(String codeString, bool single) async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('starcore_setRegisterCode', [codeString, single]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--isRegistered--*/
  Future<bool?> isRegistered() async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('starcore_isRegistered');
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--setLocale--*/
  Future<void> setLocale(String lang) async {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_setLocale', lang);
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--getLocale--*/
  Future<String?> getLocale() async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('starcore_getLocale');
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "";
    }
  }

  /*--version--*/
  Future<List?> version() async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel.invokeMethod('starcore_version');
      return Starflut.processOutputArgs(null, result)
          as FutureOr<List<dynamic>?>;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return [0, 0, 0];
    }
  }

  /*--getScriptIndex--*/
  Future<int?> getScriptIndex(String interface) async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('starcore_getScriptIndex', interface);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return 0;
    }
  }

  /*--starcore_setScript--*/
  Future<bool?> setScript(
      String scriptInterface, String module, String para) async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('starcore_setScript', [scriptInterface, module, para]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--detachCurrentThread--*/
  Future<void> detachCurrentThread() async {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_detachCurrentThread');
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--coreHandle--*/
  Future<int?> coreHandle() async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('starcore_coreHandle');
    } on PlatformException catch (e) {
      print("{$e.message}");
      return 0;
    }
  }

  /*--captureScriptGIL--*/
  Future<void> captureScriptGIL() async {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_captureScriptGIL');
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--releaseScriptGIL--*/
  Future<void> releaseScriptGIL() async {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_releaseScriptGIL');
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--setShareLibraryPath--*/
  Future<void> setShareLibraryPath(String path) async {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod('starcore_setShareLibraryPath', path);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return;
    }
  }
}

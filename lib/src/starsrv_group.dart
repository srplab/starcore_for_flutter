/*------StarSrvGroupClass---*/
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:starflut/src/star_para_pkg.dart';
import 'package:starflut/src/star_service.dart';
import 'package:starflut/src/starcore_base.dart';
import 'package:starflut/src/starobject_record.dart';
import 'package:starflut/src/starflut.dart';
import 'package:starflut/src/starbin_buf.dart';
import 'package:starflut/src/star_object.dart';

class StarSrvGroupClass extends StarCoreBase {
  StarSrvGroupClass(String? frameTag, String starTag) : super(frameTag) {
    this.starTag = starTag;
  }

  @override
  bool isEqual(StarCoreBase des) {
    if (identical(this, des)) return true;
    return false;
  }

  Future<String?> getString() async {
    try {
      await Starflut.gc();
      String? result = await Starflut.channel
          .invokeMethod('StarSrvGroupClass_toString', [this.starTag]);
      return result;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "Instance of 'StarSrvGroupClass'";
    }
  }

  /*--StarSrvGroupClass_createService--*/
  Future<StarServiceClass?> createService(
      String frameTag,
      String servicePath,
      String serviceName,
      String rootPass,
      int frameInterval,
      int netPkgSize,
      int uploadPkgSize,
      int downloadPkgSize,
      int dataUpPkgSize,
      int dataDownPkgSize,
      String derviceID) async {
    try {
      await Starflut.gc();
      String? starTag = await Starflut.channel
          .invokeMethod('StarSrvGroupClass_createService', [
        this.starTag,
        servicePath,
        serviceName,
        rootPass,
        frameInterval,
        netPkgSize,
        uploadPkgSize,
        downloadPkgSize,
        dataUpPkgSize,
        dataDownPkgSize,
        derviceID
      ]);
      if (starTag == null)
        return null;
      else {
        List<String> tags = starTag.split("+");
        StarServiceClass? lService =
            Starflut.getObjectFromRecordById(tags[1]) as StarServiceClass?;
        if (lService == null) {
          lService = new StarServiceClass(frameTag, tags[0], tags[1]);
          Starflut.insertLocalFrame(frameTag,
              new StarObjectRecordClass(lService, "StarServiceClass", 2));
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

  /*--StarSrvGroupClass_getService--*/
  Future<StarServiceClass?> getService(
      String username, String userpassword) async {
    try {
      await Starflut.gc();
      String? starTag = await Starflut.channel.invokeMethod(
          'StarSrvGroupClass_getService',
          [this.starTag, username, userpassword]);
      if (starTag == null)
        return null;
      else {
        List<String> tags = starTag.split("+");
        StarServiceClass? lService =
            Starflut.getObjectFromRecordById(tags[1]) as StarServiceClass?;
        if (lService == null) {
          lService = new StarServiceClass(null, tags[0], tags[1]);
          Starflut.starObjectRecordListRoot.add(new StarObjectRecordClass(
              lService, "StarServiceClass", 2)); //add rooot
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

  /*--StarSrvGroupClass_clearService--*/
  Future<void> clearService() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarSrvGroupClass_clearService', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarSrvGroupClass_newParaPkg--*/
  Future<StarParaPkgClass?> newParaPkg(String? frameTag, Object args) async {
    try {
      await Starflut.gc();
      String? starTag = await Starflut.channel
          .invokeMethod('StarSrvGroupClass_newParaPkg', [this.starTag, args]);
      if (starTag == null)
        return null;
      else {
        StarParaPkgClass lParaPkg = new StarParaPkgClass(frameTag, starTag);
        Starflut.insertLocalFrame(frameTag,
            new StarObjectRecordClass(lParaPkg, "StarParaPkgClass", 2));
        return lParaPkg;
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  /*--StarSrvGroupClass_newBinBuf--*/
  Future<StarBinBufClass?> newBinBuf(String? frameTag) async {
    try {
      await Starflut.gc();
      String? starTag = await Starflut.channel
          .invokeMethod('StarSrvGroupClass_newBinBuf', [this.starTag]);
      if (starTag == null)
        return null;
      else {
        StarBinBufClass lBinBuf = new StarBinBufClass(frameTag, starTag);
        Starflut.insertLocalFrame(
            frameTag, new StarObjectRecordClass(lBinBuf, "StarBinBufClass", 2));
        return lBinBuf;
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
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
  bool isObject(which) {
    if (which == null) return false;
    if (which is StarObjectClass) return true;
    return false;
  }

  bool isParaPkg(which) {
    if (which == null) return false;
    if (which is StarParaPkgClass) return true;
    return false;
  }

  bool isBinBuf(which) {
    if (which == null) return false;
    if (which is StarBinBufClass) return true;
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
  Future<String?> getServicePath() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarSrvGroupClass_getServicePath', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  /*--StarSrvGroupClass_setServicePath--*/
  Future<void> setServicePath(String args) async {
    try {
      await Starflut.gc();
      await Starflut.channel.invokeMethod(
          'StarSrvGroupClass_setServicePath', [this.starTag, args]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  /*--StarSrvGroupClass_servicePathIsSet--*/
  Future<bool?> servicePathIsSet() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarSrvGroupClass_servicePathIsSet', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_getCurrentPath--*/
  Future<String?> getCurrentPath() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarSrvGroupClass_getCurrentPath', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  /*--StarSrvGroupClass_importService--*/
  Future<bool?> importService(String serviceName, bool loadRunModule) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod(
          'StarSrvGroupClass_importService',
          [this.starTag, serviceName, loadRunModule]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_clearServiceEx--*/
  Future<void> clearServiceEx() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarSrvGroupClass_clearServiceEx', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarSrvGroupClass_runScript--*/
  Future<bool?> runScript(
      String scriptInterface, String scriptBuf, String moduleName) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_runScript',
          [this.starTag, scriptInterface, scriptBuf, moduleName]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_runScriptEx--*/
  Future<bool?> runScriptEx(
      String scriptInterface, StarBinBufClass binBuf, String moduleName) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod(
          'StarSrvGroupClass_runScriptEx',
          [this.starTag, scriptInterface, binBuf.starTag, moduleName]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_doFile--*/
  Future<bool?> doFile(String scriptInterface, String fileName) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_doFile',
          [this.starTag, scriptInterface, fileName]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_doFileEx--*/
  Future<bool?> doFileEx(
      String scriptInterface, String fileName, String moduleName) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_doFileEx',
          [this.starTag, scriptInterface, fileName, moduleName]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_setClientPort--*/
  Future<bool?> setClientPort(String lInterface, int portnumber) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod(
          'StarSrvGroupClass_setClientPort',
          [this.starTag, lInterface, portnumber]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_setTelnetPort--*/
  Future<bool?> setTelnetPort(int portnumber) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod(
          'StarSrvGroupClass_setTelnetPort', [this.starTag, portnumber]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_setOutputPort--*/
  Future<bool?> setOutputPort(String host, int portnumber) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod(
          'StarSrvGroupClass_setOutputPort', [this.starTag, host, portnumber]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_setWebServerPort--*/
  Future<bool?> setWebServerPort(
      String host, int portnumber, int connectionNumber, int postSize) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod(
          'StarSrvGroupClass_setWebServerPort',
          [this.starTag, host, portnumber, connectionNumber, postSize]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_initRaw--*/
  Future<bool?> initRaw(
      String scriptInterface, StarServiceClass service) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarSrvGroupClass_initRaw',
          [this.starTag, scriptInterface, service.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_loadRawModule--*/
  Future<bool?> loadRawModule(String scriptInterface, String moduleName,
      String fileOrString, bool isString) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod(
          'StarSrvGroupClass_loadRawModule',
          [this.starTag, scriptInterface, moduleName, fileOrString, isString]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarSrvGroupClass_getLastError--*/
  Future<int?> getLastError() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarSrvGroupClass_getLastError', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return 0;
    }
  }

  /*--StarSrvGroupClass_getLastErrorInfo--*/
  Future<String?> getLastErrorInfo() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarSrvGroupClass_getLastErrorInfo', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "";
    }
  }

  /*--StarSrvGroupClass_getCorePath--*/
  Future<String?> getCorePath() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarSrvGroupClass_getCorePath', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "";
    }
  }

  /*--StarSrvGroupClass_getUserPath--*/
  Future<String?> getUserPath() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarSrvGroupClass_getUserPath', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "";
    }
  }

  /*--StarSrvGroupClass_getLocalIP--*/
  Future<String?> getLocalIP() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarSrvGroupClass_getLocalIP', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "127.0.0.1";
    }
  }

  /*--StarSrvGroupClass_getLocalIPEx--*/
  Future<List?> getLocalIPEx() async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel
          .invokeMethod('StarSrvGroupClass_getLocalIPEx', [this.starTag]);
      return Starflut.processOutputArgs(null, result)
          as FutureOr<List<dynamic>?>;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return ["127.0.0.1"];
    }
  }

  /*--StarSrvGroupClass_getObjectNum--*/
  Future<int?> getObjectNum() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarSrvGroupClass_getObjectNum', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return 0;
    }
  }

  /*--StarSrvGroupClass_activeScriptInterface--*/
  Future<List?> activeScriptInterface(String scriptInterfaceName) async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel.invokeMethod(
          'StarSrvGroupClass_activeScriptInterface',
          [this.starTag, scriptInterfaceName]);
      return Starflut.processOutputArgs(null, result)
          as FutureOr<List<dynamic>?>;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return [false, false];
    }
  }

  /*--StarSrvGroupClass_preCompile--*/
  Future<List?> preCompile(
      String scriptInterfaceName, String scriptSegment) async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel.invokeMethod(
          'StarSrvGroupClass_preCompile',
          [this.starTag, scriptInterfaceName, scriptSegment]);
      return Starflut.processOutputArgs(null, result)
          as FutureOr<List<dynamic>?>;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return [false, "error..."];
    }
  }
}

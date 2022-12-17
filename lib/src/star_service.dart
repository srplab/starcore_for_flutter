/*------StarServiceClass---*/
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:starflut/src/starbin_buf.dart';
import 'package:starflut/src/star_object.dart';
import 'package:starflut/src/starcore_base.dart';
import 'package:starflut/src/starobject_record.dart';
import 'package:starflut/src/starsrv_group.dart';
import 'package:starflut/src/starflut.dart';

class StarServiceClass extends StarCoreBase {
  StarServiceClass(String? frameTag, String starTag, String starId)
      : super(frameTag) {
    this.starTag = starTag;
    this.starId = starId;
  }

  @override
  bool isEqual(StarCoreBase des) {
    if (identical(this, des)) return true;
    if (des is StarServiceClass) {
      StarServiceClass dd = des;
      if (dd.starId == starId) return true;
    }
    return false;
  }

  Future<String?> getString() async {
    try {
      await Starflut.gc();
      String? result = await Starflut.channel
          .invokeMethod('StarServiceClass_toString', [this.starTag]);
      return result;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "Instance of 'StarServiceClass'";
    }
  }

  /*--StarServiceClass_get--*/
  Future<Object?> operator [](Object object) async {
    try {
      await Starflut.gc();
      Object? retVal = await Starflut.channel
          .invokeMethod('StarServiceClass_get', [this.starTag, object]);
      if (retVal is String) {
        String val = retVal;
        if (val.startsWith(Starflut.StarSrvGroupPrefix)) {
          StarSrvGroupClass? lSrvGroup =
              Starflut.getStarSrvGroupFromRecordByTag(val);
          if (lSrvGroup == null) {
            lSrvGroup = new StarSrvGroupClass(null, val);
            Starflut.starObjectRecordListRoot.add(new StarObjectRecordClass(
                lSrvGroup, "StarSrvGroupClass", 2)); //---add root
          }
          return lSrvGroup;
        }
      }
      return retVal;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  /*--StarServiceClass_getObject--*/
  Future<StarObjectClass?> getObject(String? frameTag, String? objectName) async {
    try {
      await Starflut.gc();
      String? lTag = await Starflut.channel.invokeMethod(
          'StarServiceClass_getObject', [this.starTag, objectName]);
      if (lTag == null)
        return null;
      else {
        List<String> tags = lTag.split("+");
        StarObjectClass? lObject =
            Starflut.getObjectFromRecordById(tags[1]) as StarObjectClass?;
        if (lObject == null) {
          lObject = new StarObjectClass(frameTag, tags[0], tags[1]);
          Starflut.insertLocalFrame(frameTag,
              new StarObjectRecordClass(lObject, "StarObjectClass", 2));
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

  /*--StarServiceClass_getObjectEx--*/
  Future<StarObjectClass?> getObjectEx(String frameTag, String objectID) async {
    try {
      await Starflut.gc();
      String? lTag = await Starflut.channel.invokeMethod(
          'StarServiceClass_getObjectEx', [this.starTag, objectID]);
      if (lTag == null)
        return null;
      else {
        List<String> tags = lTag.split("+");
        StarObjectClass? lObject =
            Starflut.getObjectFromRecordById(tags[1]) as StarObjectClass?;
        if (lObject == null) {
          lObject = new StarObjectClass(frameTag, tags[0], tags[1]);
          Starflut.insertLocalFrame(frameTag,
              new StarObjectRecordClass(lObject, "StarObjectClass", 2));
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

  /*--StarServiceClass_newObject--*/
  Future<StarObjectClass?> newObject(String? frameTag, List args) async {
    try {
      await Starflut.gc();
      String? lTag = await Starflut.channel.invokeMethod(
          'StarServiceClass_newObject',
          [this.starTag, Starflut.processInputArgs(args)]);
      if (lTag == null)
        return null;
      else {
        List<String> tags = lTag.split("+");
        StarObjectClass? lObject =
            Starflut.getObjectFromRecordById(tags[1]) as StarObjectClass?;
        if (lObject == null) {
          lObject = new StarObjectClass(frameTag, tags[0], tags[1]);
          Starflut.insertLocalFrame(frameTag,
              new StarObjectRecordClass(lObject, "StarObjectClass", 2));
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

  /*--StarServiceClass_runScript--*/
  Future<List?> runScript(String scriptInterface, String scriptBuf,
      String? moduleName, String? workDirectory) async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel.invokeMethod(
          'StarServiceClass_runScript', [
        this.starTag,
        scriptInterface,
        scriptBuf,
        moduleName,
        workDirectory
      ]);
      return Starflut.processOutputArgs(null, result)
          as FutureOr<List<dynamic>?>;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return [false, "error..."];
    }
  }

  /*--StarServiceClass_runScriptEx--*/
  Future<List?> runScriptEx(String scriptInterface, StarBinBufClass binBuf,
      String moduleName, String workDirectory) async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel.invokeMethod(
          'StarServiceClass_runScriptEx', [
        this.starTag,
        scriptInterface,
        binBuf.starTag,
        moduleName,
        workDirectory
      ]);
      return Starflut.processOutputArgs(null, result)
          as FutureOr<List<dynamic>?>;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return [false, "error..."];
    }
  }

  /*--StarServiceClass_doFile--*/
  Future<List?> doFile(
      String scriptInterface, String fileName, String workDirectory) async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel.invokeMethod(
          'StarServiceClass_doFile',
          [this.starTag, scriptInterface, fileName, workDirectory]);
      return Starflut.processOutputArgs(null, result)
          as FutureOr<List<dynamic>?>;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return [false, "error..."];
    }
  }

  /*--StarServiceClass_doFileEx--*/
  Future<List?> doFileEx(String scriptInterface, String fileName,
      String workDirectory, String moduleName) async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel.invokeMethod(
          'StarServiceClass_doFileEx',
          [this.starTag, scriptInterface, fileName, moduleName]);
      return Starflut.processOutputArgs(null, result)
          as FutureOr<List<dynamic>?>;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return [false, "error..."];
    }
  }

  /*--StarServiceClass_isServiceRegistered--*/
  Future<bool?> isServiceRegistered() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarServiceClass_isServiceRegistered', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarServiceClass_checkPassword--*/
  Future<void> checkPassword(bool flag) async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarServiceClass_checkPassword', [this.starTag, flag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarServiceClass_newRawProxy--*/
  Future<StarObjectClass?> newRawProxy(
      String frameTag,
      String scriptInterface,
      StarObjectClass attachObject,
      String attachFunction,
      String proyInfo,
      int proxyType) async {
    try {
      await Starflut.gc();
      if (attachObject == null || (!(attachObject is StarObjectClass)))
        return null;
      String? lTag =
          await Starflut.channel.invokeMethod('StarServiceClass_newRawProxy', [
        this.starTag,
        scriptInterface,
        attachObject.starTag,
        attachFunction,
        proyInfo,
        proxyType
      ]);
      if (lTag == null)
        return null;
      else {
        List<String> tags = lTag.split("+");
        StarObjectClass? lObject =
            Starflut.getObjectFromRecordById(tags[1]) as StarObjectClass?;
        if (lObject == null) {
          lObject = new StarObjectClass(frameTag, tags[0], tags[1]);
          Starflut.insertLocalFrame(frameTag,
              new StarObjectRecordClass(lObject, "StarObjectClass", 2));
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

  /*--StarServiceClass_importRawContext--*/
  Future<StarObjectClass?> importRawContext(
      String? frameTag,
      String? scriptInterface,
      String? contextName,
      bool? isClass,
      String? contextInfo) async {
    try {
      await Starflut.gc();
      String? lTag = await Starflut.channel.invokeMethod(
          'StarServiceClass_importRawContext',
          [this.starTag, scriptInterface, contextName, isClass, contextInfo]);
      if (lTag == null)
        return null;
      else {
        List<String> tags = lTag.split("+");
        StarObjectClass? lObject =
            Starflut.getObjectFromRecordById(tags[1]) as StarObjectClass?;
        if (lObject == null) {
          lObject = new StarObjectClass(frameTag, tags[0], tags[1]);
          Starflut.insertLocalFrame(frameTag,
              new StarObjectRecordClass(lObject, "StarObjectClass", 2));
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

  /*--StarServiceClass_getLastError--*/
  Future<int?> getLastError() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarServiceClass_getLastError', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return 0;
    }
  }

  /*--StarServiceClass_getLastErrorInfo--*/
  Future<String?> getLastErrorInfo() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarServiceClass_getLastErrorInfo', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "";
    }
  }

  /*--StarServiceClass_allObject--*/
  Future<List?> allObject(String? frameTag) async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel
          .invokeMethod('StarServiceClass_allObject', [this.starTag]);
      return Starflut.processOutputArgs(frameTag, result)
          as FutureOr<List<dynamic>?>;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return [false, "error..."];
    }
  }

  /*--StarServiceClass_restfulCall--*/
  Future<List?> restfulCall(
      String? frameTag, String url, String opCode, String jsonString) async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel.invokeMethod(
          'StarServiceClass_restfulCall',
          [this.starTag, url, opCode, jsonString]);
      return Starflut.processOutputArgs(frameTag, result)
          as FutureOr<List<dynamic>?>;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return [
        400,
        "{\"code\": -32600, \"message\": \"call _RestfulCall failed,input para error\"}"
      ];
    }
  }
}

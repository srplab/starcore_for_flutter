/*------StarObjectClass---*/
import 'package:flutter/services.dart';
import 'package:starflut/src/starbin_buf.dart';
import 'package:starflut/src/star_para_pkg.dart';
import 'package:starflut/src/star_service.dart';
import 'package:starflut/src/starcore_base.dart';
import 'package:starflut/src/starflut.dart';
import 'package:starflut/src/starobject_record.dart';
import 'package:starflut/src/type_defs.dart';

class StarObjectClass extends StarCoreBase {
  StarObjectClass(String? frameTag, String starTag, String starId)
      : super(frameTag) {
    this.starTag = starTag;
    this.starId = starId;
    scriptCallBack = new Map<String, StarObjectScriptProc>();
  }

  @override
  bool isEqual(StarCoreBase des) {
    if (identical(this, des)) return true;
    if (des is StarObjectClass) {
      StarObjectClass dd = des;
      if (dd.starId == starId) return true;
    }
    return false;
  }

  Future<String?> getString() async {
    try {
      await Starflut.gc();
      String? result = await Starflut.channel
          .invokeMethod('StarObjectClass_toString', [this.starTag]);
      return result;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "Instance of 'StarObjectClass'";
    }
  }

  /*--StarObjectClass_get--*/
  Future<Object?> operator [](Object indexOrName) async {
    try {
      await Starflut.gc();
      Object? retVal = await Starflut.channel
          .invokeMethod('StarObjectClass_get', [this.starTag, indexOrName]);
      if (retVal is String) {
        String val = retVal;
        if (val.startsWith(Starflut.StarObjectPrefix)) {
          List<String> tags = val.split("+");
          StarObjectClass? lStarObject =
              Starflut.getObjectFromRecordById(tags[1]) as StarObjectClass?;
          if (lStarObject == null) {
            lStarObject = new StarObjectClass(m_frameTag, tags[0], tags[1]);
            Starflut.insertLocalFrame(m_frameTag,
                new StarObjectRecordClass(lStarObject, "StarObjectClass", 2));
          } else {
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          return lStarObject;
        } else if (val.startsWith(Starflut.StarBinBufPrefix)) {
          StarBinBufClass lStarBinBuf = new StarBinBufClass(m_frameTag, val);
          Starflut.insertLocalFrame(m_frameTag,
              new StarObjectRecordClass(lStarBinBuf, "StarBinBufClass", 2));
          return lStarBinBuf;
        } else if (val.startsWith(Starflut.StarParaPkgPrefix)) {
          StarParaPkgClass lStarParaPkg = new StarParaPkgClass(m_frameTag, val);
          Starflut.insertLocalFrame(m_frameTag,
              new StarObjectRecordClass(lStarParaPkg, "StarParaPkgClass", 2));
          return lStarParaPkg;
        } else if (val.startsWith(Starflut.StarServicePrefix)) {
          List<String> tags = val.split("+");
          StarServiceClass? lStarService =
              Starflut.getObjectFromRecordById(tags[1]) as StarServiceClass?;
          if (lStarService == null) {
            lStarService = new StarServiceClass(m_frameTag, tags[0], tags[1]);
            Starflut.insertLocalFrame(m_frameTag,
                new StarObjectRecordClass(lStarService, "StarServiceClass", 2));
          } else {
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          return lStarService;
        }
      }
      return retVal;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  /*--StarObjectClass_get--*/
  Future getValue(Object indexOrNameOrList) async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel.invokeMethod(
          'StarObjectClass_get', [this.starTag, indexOrNameOrList]);
      if ((result is List) || (result is Map)) {
        return Starflut.processOutputArgs(m_frameTag, result);
      } else {
        List rr =
            Starflut.processOutputArgs(m_frameTag, [result]) as List<dynamic>;
        return rr[0];
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  Future<void> setValue(Object indexOrName, Object value) async {
    try {
      await Starflut.gc();
      if (value is StarObjectClass) {
        StarObjectClass vo = value;
        await Starflut.channel.invokeMethod(
            'StarObjectClass_set', [this.starTag, indexOrName, vo.starTag]);
      } else if (value is StarParaPkgClass) {
        StarParaPkgClass vo = value;
        await Starflut.channel.invokeMethod(
            'StarObjectClass_set', [this.starTag, indexOrName, vo.starTag]);
      } else if (value is StarBinBufClass) {
        StarBinBufClass vo = value;
        await Starflut.channel.invokeMethod(
            'StarObjectClass_set', [this.starTag, indexOrName, vo.starTag]);
      } else {
        await Starflut.channel.invokeMethod(
            'StarObjectClass_set', [this.starTag, indexOrName, value]);
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarObjectClass_call--*/
  Future<Object?> call(String funcName, List args) async {
    try {
      await Starflut.gc();
      if (funcName == null) funcName = "_StarCall";
      Object? retVal = await Starflut.channel.invokeMethod(
          'StarObjectClass_call',
          [this.starTag, funcName, Starflut.processInputArgs(args)]);
      if (retVal is String) {
        String val = retVal;
        if (val.startsWith(Starflut.StarObjectPrefix)) {
          List<String> tags = val.split("+");
          StarObjectClass? lStarObject =
              Starflut.getObjectFromRecordById(tags[1]) as StarObjectClass?;
          if (lStarObject == null) {
            lStarObject = new StarObjectClass(m_frameTag, tags[0], tags[1]);
            Starflut.insertLocalFrame(m_frameTag,
                new StarObjectRecordClass(lStarObject, "StarObjectClass", 2));
          } else {
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          return lStarObject;
        } else if (val.startsWith(Starflut.StarBinBufPrefix)) {
          StarBinBufClass lStarBinBuf = new StarBinBufClass(m_frameTag, val);
          Starflut.insertLocalFrame(m_frameTag,
              new StarObjectRecordClass(lStarBinBuf, "StarBinBufClass", 2));
          return lStarBinBuf;
        } else if (val.startsWith(Starflut.StarParaPkgPrefix)) {
          StarParaPkgClass lStarParaPkg = new StarParaPkgClass(m_frameTag, val);
          Starflut.insertLocalFrame(m_frameTag,
              new StarObjectRecordClass(lStarParaPkg, "StarParaPkgClass", 2));
          return lStarParaPkg;
        } else if (val.startsWith(Starflut.StarServicePrefix)) {
          List<String> tags = val.split("+");
          StarServiceClass? lStarService =
              Starflut.getObjectFromRecordById(tags[1]) as StarServiceClass?;
          if (lStarService == null) {
            lStarService = new StarServiceClass(m_frameTag, tags[0], tags[1]);
            Starflut.insertLocalFrame(m_frameTag,
                new StarObjectRecordClass(lStarService, "StarServiceClass", 2));
          } else {
            Starflut.starObjectRecordFrameWaitFree.add(tags[0]);
          }
          return lStarService;
        }
      }
      return retVal;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  /*--StarObjectClass_newObject--*/
  Future<StarObjectClass?> newObject(List args) async {
    try {
      await Starflut.gc();
      String? lTag = await Starflut.channel.invokeMethod(
          'StarObjectClass_newObject',
          [this.starTag, Starflut.processInputArgs(args)]);
      if (lTag == null)
        return null;
      else {
        List<String> tags = lTag.split("+");
        StarObjectClass? lObject =
            Starflut.getObjectFromRecordById(tags[1]) as StarObjectClass?;
        if (lObject == null) {
          lObject = new StarObjectClass(m_frameTag, tags[0], tags[1]);
          Starflut.insertLocalFrame(m_frameTag,
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

  /*--StarObjectClass_free--*/
  Future<void> free() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarObjectClass_free', [this.starTag]);
      free();
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarObjectClass_dispose--*/
  Future<void> dispose() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarObjectClass_dispose', [this.starTag]);
      free();
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarObjectClass_hasRawContext--*/
  Future<bool?> hasRawContext() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarObjectClass_hasRawContext', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarObjectClass_rawToParaPkg--*/
  Future<Object?> rawToParaPkg() async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel
          .invokeMethod('StarObjectClass_rawToParaPkg', [this.starTag]);
      if (result != null) {
        return Starflut.processOutputArgs(m_frameTag, result);
      } else {
        return null;
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  /*--StarObjectClass_getSourceScript--*/
  Future<int?> getSourceScript() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarObjectClass_getSourceScript', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return 0;
    }
  }

  Future<int?> getLastError() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarObjectClass_getLastError', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return 0;
    }
  }

  /*--StarObjectClass_getLastErrorInfo--*/
  Future<String?> getLastErrorInfo() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarObjectClass_getLastErrorInfo', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "";
    }
  }

  /*--StarObjectClass_releaseOwnerEx--*/
  Future<void> releaseOwnerEx() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarObjectClass_releaseOwnerEx', [this.starTag]);
      free();
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarObjectClass_regScriptProcP--*/
  late Map<String, StarObjectScriptProc> scriptCallBack;
  Future<void> regScriptProcP(
      String scriptName, StarObjectScriptProc callBack) async {
    try {
      await Starflut.gc();
      if (callBack == null) {
        if (scriptCallBack[scriptName] != null)
          await Starflut.channel.invokeMethod('StarObjectClass_regScriptProcP',
              [this.starTag, scriptName, false]);
        scriptCallBack.remove(scriptName);
      } else {
        if (scriptCallBack[scriptName] == null)
          await Starflut.channel.invokeMethod('StarObjectClass_regScriptProcP',
              [this.starTag, scriptName, true]);
        scriptCallBack[scriptName] = callBack;
      }

      lock();
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarObjectClass_instNumber--*/
  Future<int?> instNumber() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarObjectClass_instNumber', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return 0;
    }
  }

  /*--StarObjectClass_changeParent--*/
  Future<void> changeParent(
      StarObjectClass parentObject, String queueName) async {
    try {
      await Starflut.gc();
      if ((!(parentObject is StarObjectClass)))
        await Starflut.channel.invokeMethod(
            'StarObjectClass_changeParent', [this.starTag, null, queueName]);
      else
        await Starflut.channel.invokeMethod('StarObjectClass_changeParent',
            [this.starTag, parentObject.starTag, queueName]);
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarObjectClass_jsonCall--*/
  Future<String?> jsonCall(String jsonString) async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarObjectClass_jsonCall', [this.starTag, jsonString]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "{\"jsonrpc\": \"2.0\", \"error\": {\"code\": -32603, \"message\": \"call _JSonCall failed,Internal error\"}, \"id\": null}";
    }
  }

  /*--StarObjectClass_active--*/
  Future<bool?> active() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarObjectClass_active', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarObjectClass_deActive--*/
  Future<void> deActive() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarObjectClass_deActive', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return;
    }
  }

  /*--StarObjectClass_isActive--*/
  Future<bool?> isActive() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarObjectClass_isActive', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }
}

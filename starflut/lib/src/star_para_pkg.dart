/*------StarParaPkgClass---*/
import 'package:flutter/services.dart';
import 'package:starflut/src/starbin_buf.dart';
import 'package:starflut/src/star_object.dart';
import 'package:starflut/src/starcore_base.dart';
import 'package:starflut/src/starobject_record.dart';
import 'package:starflut/src/starflut.dart';

class StarParaPkgClass extends StarCoreBase {
  StarParaPkgClass(String? frameTag, String starTag) : super(frameTag) {
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
          .invokeMethod('StarParaPkgClass_toString', [this.starTag]);
      return result;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "Instance of 'StarParaPkgClass'";
    }
  }

  Future<int?> get number async {
    return await Starflut.channel
        .invokeMethod('StarParaPkgClass_GetNumber', [this.starTag]);
  }

  Future<String?> get v async {
    return await Starflut.channel
        .invokeMethod('StarParaPkgClass_V', [this.starTag]);
  }

  /*--StarParaPkgClass_get--*/
  Future<Object?> operator [](int index) async {
    try {
      await Starflut.gc();
      Object? retVal = await Starflut.channel
          .invokeMethod('StarParaPkgClass_get', [this.starTag, index]);
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
        }
      }
      return retVal;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  /*--StarParaPkgClass_clear--*/
  Future<StarParaPkgClass> clear() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarParaPkgClass_clear', [this.starTag]);
      return this;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return this;
    }
  }

  /*--StarParaPkgClass_appendFrom--*/
  Future<bool?> appendFrom(StarParaPkgClass srcParaPkg) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod(
          'StarParaPkgClass_appendFrom', [this.starTag, srcParaPkg.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarParaPkgClass_equals--*/
  Future<bool?> equals(StarParaPkgClass srcParaPkg) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod(
          'StarParaPkgClass_equals', [this.starTag, srcParaPkg.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  Future<Object?> getValue(Object value) async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel
          .invokeMethod('StarParaPkgClass_get', [this.starTag, value]);
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

  Future<StarParaPkgClass> setValue(int index, Object value) async {
    try {
      await Starflut.gc();
      if (value is StarObjectClass) {
        StarObjectClass vo = value;
        await Starflut.channel.invokeMethod(
            'StarParaPkgClass_set', [this.starTag, index, vo.starTag]);
      } else if (value is StarParaPkgClass) {
        StarParaPkgClass vo = value;
        await Starflut.channel.invokeMethod(
            'StarParaPkgClass_set', [this.starTag, index, vo.starTag]);
      } else if (value is StarBinBufClass) {
        StarBinBufClass vo = value;
        await Starflut.channel.invokeMethod(
            'StarParaPkgClass_set', [this.starTag, index, vo.starTag]);
      } else {
        await Starflut.channel
            .invokeMethod('StarParaPkgClass_set', [this.starTag, index, value]);
      }
      return this;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return this;
    }
  }

  /*--StarParaPkgClass_build--*/
  Future<StarParaPkgClass> build(Object args) async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarParaPkgClass_build', [this.starTag, args]);
      return this;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return this;
    }
  }

  /*--StarParaPkgClass_free--*/
  Future<void> free() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarParaPkgClass_free', [this.starTag]);
      free();
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarParaPkgClass_dispose--*/
  Future<void> dispose() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarParaPkgClass_dispose', [this.starTag]);
      free();
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarParaPkgClass_releaseOwner--*/
  Future<void> releaseOwner() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarParaPkgClass_releaseOwner', [this.starTag]);
      free();
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarParaPkgClass_asDict--*/
  Future<StarParaPkgClass> asDict(bool isDict) async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarParaPkgClass_asDict', [this.starTag, isDict]);
      return this;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return this;
    }
  }

  /*--StarParaPkgClass_isDict--*/
  Future<bool?> isDict() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarParaPkgClass_isDict', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarParaPkgClass_fromJSon--*/
  Future<bool?> fromJSon(String jsonstring) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod(
          'StarParaPkgClass_fromJSon', [this.starTag, jsonstring]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarParaPkgClass_toJSon--*/
  Future<String?> toJSon() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarParaPkgClass_toJSon', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "";
    }
  }

  /*--StarParaPkgClass_toTuple--*/
  Future<Object?> toTuple() async {
    try {
      await Starflut.gc();
      Object? result = await Starflut.channel
          .invokeMethod('StarParaPkgClass_toTuple', [this.starTag]);
      if (result != null) /*must be list or map--*/
      {
        return Starflut.processOutputArgs(m_frameTag, result);
      } else {
        return null;
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
      return null;
    }
  }

  /*--StarParaPkgClass_fromTuple--*/
  Future<StarParaPkgClass> fromTuple(Object args) async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarParaPkgClass_build', [this.starTag, args]);
      return this;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return this;
    }
  }
}

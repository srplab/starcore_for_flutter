/*------StarBinBufClass---*/
import 'package:flutter/services.dart';
import 'package:starflut/src/starcore_base.dart';
import 'package:starflut/src/starflut.dart';

class StarBinBufClass extends StarCoreBase {
  StarBinBufClass(String? frameTag, String starTag) : super(frameTag) {
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
          .invokeMethod('StarBinBufClass_toString', [this.starTag]);
      return result;
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "Instance of 'StarBinBufClass'";
    }
  }

  Future<int?> get offset async {
    return await Starflut.channel
        .invokeMethod('StarBinBufClass_GetOffset', [this.starTag]);
  }

  /*--StarBinBufClass_init--*/
  Future<void> init(int bufSize) async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarBinBufClass_init', [this.starTag, bufSize]);
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarBinBufClass_clear--*/
  Future<void> clear(int bufSize) async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarBinBufClass_clear', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarBinBufClass_saveToFile--*/
  Future<bool?> saveToFile(String fileName, bool txtFileFlag) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod(
          'StarBinBufClass_saveToFile', [this.starTag, fileName, txtFileFlag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarBinBufClass_loadFromFile--*/
  Future<bool?> loadFromFile(String fileName, bool txtFileFlag) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod('StarBinBufClass_loadFromFile',
          [this.starTag, fileName, txtFileFlag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarBinBufClass_read--*/
  Future<Uint8List?> read(int offset, int length) async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarBinBufClass_read', [this.starTag, offset, length]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return new Uint8List(0);
    }
  }

  /*--StarBinBufClass_write--*/
  Future<int?> write(int offset, Uint8List buf, int length) async {
    try {
      await Starflut.gc();
      return await Starflut.channel.invokeMethod(
          'StarBinBufClass_write', [this.starTag, offset, buf, length]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return 0;
    }
  }

  /*--StarBinBufClass_free--*/
  Future<void> free() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarBinBufClass_free', [this.starTag]);
      free();
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarBinBufClass_dispose--*/
  Future<void> dispose() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarBinBufClass_dispose', [this.starTag]);
      free();
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarBinBufClass_releaseOwner--*/
  Future<void> releaseOwner() async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarBinBufClass_releaseOwner', [this.starTag]);
      free();
    } on PlatformException catch (e) {
      print("{$e.message}");
    }
  }

  /*--StarBinBufClass_setOffset--*/
  Future<bool?> setOffset(int Offset) async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarBinBufClass_setOffset', [this.starTag, Offset]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return false;
    }
  }

  /*--StarBinBufClass_print--*/
  Future<void> print(String Arg) async {
    try {
      await Starflut.gc();
      await Starflut.channel
          .invokeMethod('StarBinBufClass_print', [this.starTag, Arg]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return;
    }
  }

  /*--StarBinBufClass_asString--*/
  Future<String?> asString() async {
    try {
      await Starflut.gc();
      return await Starflut.channel
          .invokeMethod('StarBinBufClass_asString', [this.starTag]);
    } on PlatformException catch (e) {
      print("{$e.message}");
      return "";
    }
  }
}

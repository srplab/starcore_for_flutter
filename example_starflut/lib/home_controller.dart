// ignore_for_file: prefer_typing_uninitialized_variables

import 'dart:io';

import 'package:flutter/services.dart';
import 'package:get/get.dart';
import 'package:path_provider/path_provider.dart';
import 'package:starflut/starflut.dart';

class HomeController extends GetxController {
  StarCoreFactory? starcore;
  StarServiceClass? starService;
  StarSrvGroupClass? srvGroup;
  StarObjectClass? python;

  @override
  void onInit() {
    _initStarCore();
    super.onInit();
  }

  Future<void> runPythonCode() async {
    try {
      String? resPath = await Starflut.getResourcePath();
      if (resPath == null) return;

      await srvGroup?.initRaw("python39", starService!);
      var file =
          await _getFileFromAssets('starfiles/main.py', loadCached: false);
      await srvGroup?.loadRawModule("python", "", file.path, false);

      python = await starService?.importRawContext("", "python", "", false, "");

      // Calling all methods from starfiles/main.py file

      // Get testValue from Python
      var testValue = await python?["testValue"];

      // Call getJsonValue method from python
      StarObjectClass? retobj = await python
          ?.call("getJsonValue", ["Rohit", "24"]) as StarObjectClass?;
      var result = await retobj?.getValue('extra');

      // Call Calculator class from python
      StarObjectClass? calculator = await starService?.importRawContext(
          "", "python", "Calculator", true, "");
      StarObjectClass? calculatorInstance =
          await calculator?.newObject(["", "", 33, 44]);

      var mltply;
      var add;
      // call methods of calculator class
      if (GetPlatform.isMobile) {
        // for mobile we have to pass self -> instance
        mltply = await calculatorInstance
            ?.call("multiply", [calculatorInstance, 5, 10]);
        add =
            await calculatorInstance?.call('add', [calculatorInstance, 5, 10]);
      } else if (GetPlatform.isDesktop) {
        // we don't have to pass instance on desktop
        mltply = await calculatorInstance?.call("multiply", [5, 10]);
        add = await calculatorInstance?.call('add', [5, 10]);
      }

      Get.log("testValue : $testValue");
      Get.log("getJsonValue.extra :  $result");
      Get.log("Multiply : $mltply");
      Get.log("Add : $add");
    } on PlatformException catch (e) {
      Get.log("{$e.message}");
    }
  }

  Future<void> _initStarCore() async {
    starcore = await Starflut.getFactory();
    starService = await starcore?.initSimple("test", "123", 0, 0, []);
    await starcore?.regMsgCallBackP(_messageCallback);
    srvGroup = await starService?["_ServiceGroup"] as StarSrvGroupClass?;
    if (GetPlatform.isAndroid) await _loadAndroidAssets();
  }

  Future<List?> _messageCallback(
      int serviceGroupID, int uMsg, Object wParam, Object lParam) async {
    Get.log(
      "MessageCallback : { serviceGroupID :  $serviceGroupID, uMsg : $uMsg, wParam :  $wParam,lParam :  $lParam }",
    );
    return null;
  }

  Future<void> _loadAndroidAssets() async {
    // Load python files
    await Starflut.copyFileFromAssets(
        "main.py", "flutter_assets/starfiles", "flutter_assets/starfiles");

    // Load python libraries
    await Starflut.copyFileFromAssets("python3.9.zip", null, null);
    String? nativepath = await Starflut.getNativeLibraryDir();
    if (nativepath == null) return;
    var libraryPath = "";
    if (nativepath.contains("x86_64")) {
      libraryPath = "x86_64";
    } else if (nativepath.contains("arm64")) {
      libraryPath = "arm64-v8a";
    } else if (nativepath.contains("arm")) {
      libraryPath = "armeabi";
    } else if (nativepath.contains("x86")) {
      libraryPath = "x86";
    }
    await Starflut.copyFileFromAssets("zlib.cpython-39.so", libraryPath, null);
    await Starflut.copyFileFromAssets(
        "unicodedata.cpython-39.so", libraryPath, null);
    await Starflut.loadLibrary("libpython3.9.so");
  }

  Future<File> _getFileFromAssets(String path,
      {bool loadCached = false}) async {
    Directory tempDir = await getTemporaryDirectory();
    String tempPath = tempDir.path;
    var filePath = "$tempPath/$path";
    var file = File(filePath);
    if (loadCached && file.existsSync()) {
      return file;
    }
    final byteData = await rootBundle.load(path);
    final buffer = byteData.buffer;
    await file.create(recursive: true);
    return file.writeAsBytes(
        buffer.asUint8List(byteData.offsetInBytes, byteData.lengthInBytes));
  }

  @override
  void dispose() async {
    await srvGroup?.clearService();
    await starcore?.moduleExit();
    super.dispose();
  }
}

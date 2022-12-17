// ignore_for_file: unnecessary_overrides, non_constant_identifier_names

import 'package:flutter/services.dart';
import 'package:get/get.dart';
import 'package:starflut/starflut.dart';

class HomeController extends GetxController {
  StarCoreFactory? starcore;
  StarServiceClass? starService;
  StarSrvGroupClass? SrvGroup;
  StarObjectClass? python;

  @override
  void onInit() {
    _initStarCore();
    super.onInit();
  }

  Future<void> runPythonCode() async {
    try {
      String? resPath = await Starflut.getResourcePath();
      await SrvGroup?.initRaw("python36", starService!);
      await SrvGroup?.loadRawModule(
          "python", "", "$resPath/flutter_assets/starfiles/main.py", false);

      python = await starService?.importRawContext("", "python", "", false, "");

      /// Calling all methods from starfiles/main.py file

      // Get testValue from Python
      var testValue = await python?["testValue"];
      Get.log("testValue : $testValue");

      // Call getJsonValue method from python
      StarObjectClass? retobj = await python
          ?.call("getJsonValue", ["Rohit", "24"]) as StarObjectClass?;
      var result = await retobj?.getValue('extra');
      Get.log("getJsonValue.extra :  $result");

      // Call Calculator class from python
      StarObjectClass? calculator = await starService?.importRawContext(
          "", "python", "Calculator", true, "");
      StarObjectClass? calculatorInstance =
          await calculator?.newObject(["", "", 33, 44]);

      // call methods of calculator class
      var mltply = await calculatorInstance?.call("multiply", [5, 10]);
      Get.log("Multiply : $mltply");
      var add = await calculatorInstance?.call('add', [5, 10]);
      Get.log("Add : $add");
    } on PlatformException catch (e) {
      Get.log("{$e.message}");
    }
  }

  Future<void> _initStarCore() async {
    starcore = await Starflut.getFactory();
    starService = await starcore?.initSimple("test", "123", 0, 0, []);
    await starcore?.regMsgCallBackP(_messageCallback);
    SrvGroup = await starService?["_ServiceGroup"] as StarSrvGroupClass?;
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
    await Starflut.copyFileFromAssets(
        "main.py", "flutter_assets/starfiles", "flutter_assets/starfiles");
    await Starflut.copyFileFromAssets(
        "python3.6.zip", "flutter_assets/starfiles", null);
    await Starflut.copyFileFromAssets("zlib.cpython-36m.so", null, null);
    await Starflut.copyFileFromAssets("unicodedata.cpython-36m.so", null, null);
    await Starflut.loadLibrary("libpython3.6m.so");
  }

  @override
  void dispose() async {
    await SrvGroup?.clearService();
    await starcore?.moduleExit();
    super.dispose();
  }
}

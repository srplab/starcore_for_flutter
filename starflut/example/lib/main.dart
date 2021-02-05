import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:starflut/starflut.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String _platformVersion = 'Unknown';

  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  static String format(String fmt,List<Object> params) {
    int matchIndex = 0;
    String replace(Match m) {
      if (matchIndex<params.length) {
        switch (m[4]) {
          case "f":
            num val = params[matchIndex++];
            String str;
            if (m[3]!=null && m[3].startsWith(".")) {
              str = val.toStringAsFixed(int.parse(m[3].substring(1)));
            } else {
              str = val.toString();
            }
            if (m[2]!=null && m[2].startsWith("0")) {
              if (val<0) {
                str = "-"+str.substring(1).padLeft(int.parse(m[2]),"0");
              } else {
                str = str.padLeft(int.parse(m[2]),"0");
              }
            }
            return str;
          case "d":
          case "x":
          case "X":
            int val = params[matchIndex++];
            String str = (m[4]=="d")?val.toString():val.toRadixString(16);
            if (m[2]!=null && m[2].startsWith("0")) {
              if (val<0) {
                str = "-"+str.substring(1).padLeft(int.parse(m[2]),"0");
              } else {
                str = str.padLeft(int.parse(m[2]),"0");
              }
            }
            return (m[4]=="X")?str.toUpperCase():str.toLowerCase();
          case "s":
            return params[matchIndex++].toString();
        }
      } else {
        throw new Exception("Missing parameter for string format");
      }
      throw new Exception("Invalid format string: "+m[0].toString());
    }
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    String platformVersion;
    // Platform messages may fail, so we use a try/catch PlatformException.
    try {
      int TestCase = 3;  //7 -- lua //6 -- go, test for ios only //---5 ruby(android/ios/desktop)  //---4 ios //--3 call back(android/ios/desktop);  //--2  call python(android/desktop) //--1 android api //--0 develop(android/desktop)


      if (TestCase == 0) {
        String Path1  = await Starflut.getResourcePath();
        String Path2 = await Starflut.getAssetsPath();
        StarCoreFactory starcore = await Starflut.getFactory();
        int Result = 0;
        Result = await starcore.initCore(true, false, false, true, "", 0, "", 0);
        await starcore.regMsgCallBackP(
                (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
              print("$serviceGroupID  $uMsg   $wParam   $lParam");
              return null;
            });
        int Platform = await Starflut.getPlatform();
        if( Platform == Starflut.MACOS ) {
          await starcore.setShareLibraryPath(
              Path1); //set path for interface library
          bool LoadResult = await Starflut.loadLibrary(Path1+"/libpython3.9.dylib");
          print("$LoadResult");  //--load
          await Starflut.setEnv("PYTHONPATH","/Library/Frameworks/Python.framework/Versions/3.9/lib/python3.9");
          String pypath = await Starflut.getEnv("PYTHONPATH");
          print("$pypath");
        }else if( Platform == Starflut.WINDOWS ) {
          await starcore.setShareLibraryPath(
              Path1.replaceAll("\\","/")); //set path for interface library
        }
        //StarSrvGroupClass StarSrvGroup = await starcore.getSrvGroup(0);
        //print("$Result");
        //StarSrvGroupClass StarSrvGroup = await starcore.initSimpleEx(0, 0, []);
        StarServiceClass StarService =
        await starcore.initSimple("test", "123", 0, 0, []);
        StarSrvGroupClass StarSrvGroup = await StarService["_ServiceGroup"];
        print("starsrvgroup = " + await StarSrvGroup.getString());

        print("$Result");
        var lang = await starcore.getLocale();
        print("$lang");

        var ver = await starcore.version();
        print("$ver");

        var hand = await starcore.coreHandle();
        print("$hand");

        StarParaPkgClass ParaPkg = await StarSrvGroup.newParaPkg(null,[
          'aaaa',
          234,
          [345]
        ]);
        var isp = StarSrvGroup.isParaPkg(ParaPkg);
        print("$isp");

        print("parapkg " + await ParaPkg[0]);
        print(await ParaPkg[1]);

        await ParaPkg.setValue(0, '22222222');
        print("parapkg new value =" + await ParaPkg[0]);

        await ParaPkg.setValue(1, ['22222222']);

        print(await ParaPkg.toTuple());

        //--map
        StarParaPkgClass ParaPkg1 = await StarSrvGroup.newParaPkg(null,{"a":1, "b":2, "c":3});
        print(await ParaPkg1[0]);
        print(await ParaPkg1.toTuple());

        StarParaPkgClass ParaPkg2 = await StarSrvGroup.newParaPkg(null,[345.67,{"a":1, "b":2, "c":3}]);
        print(await ParaPkg2[1]);
        print(await ParaPkg2.toTuple());

        await ParaPkg1.build([345.67,{"a":1, "b":2, "c":3}]);
        print(await ParaPkg1.toTuple());

        await ParaPkg1.build({"a":1, "b":2, "c":3});
        print(await ParaPkg1.toTuple());


        var srvp = await StarSrvGroup.getServicePath();
        print("$srvp");

        var curp = await StarSrvGroup.getCurrentPath();
        print("$curp");

        /*---script python--*/
        bool isAndroid = await Starflut.isAndroid();
        if( isAndroid == true ){
          var py_result1 =
          await Starflut.copyFileFromAssets("python3.9.zip", null,null);   //desRelatePath must be null
          print("$py_result1");
          var nativepath = await Starflut.getNativeLibraryDir();
          var LibraryPath = "";
          if( nativepath.contains("x86_64"))
            LibraryPath = "x86_64";
          else if( nativepath.contains("arm64"))
            LibraryPath = "arm64-v8a";
          else if( nativepath.contains("arm"))
            LibraryPath = "armeabi";
          else if( nativepath.contains("x86"))
            LibraryPath = "x86";
          py_result1 =
          await Starflut.copyFileFromAssets("zlib.cpython-39.so", LibraryPath,null);
          print("$py_result1");
          py_result1 = await Starflut.copyFileFromAssets(
              "unicodedata.cpython-39.so", LibraryPath,null);
          print("$py_result1");

          py_result1 = await Starflut.loadLibrary("libpython3.9.so");
          print("$py_result1");
        }

        bool run_result =
        await StarSrvGroup.runScript("python39", "print(123)", "");
        print("$run_result");

        //---moduleExit run before runScript, so will cause exception,
        //---moduleExit should be run on mainthread.
        await starcore.moduleExit();
      }else if (TestCase == 1) {
        StarCoreFactory starcore = await Starflut.getFactory();
        StarServiceClass Service = await starcore.initSimple("test", "123", 0, 0, []);
        await starcore.regMsgCallBackP(
                (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
              print("$serviceGroupID  $uMsg   $wParam   $lParam");
              return null;
            });
        StarSrvGroupClass SrvGroup = await Service["_ServiceGroup"];
        await SrvGroup.initRaw("java",Service);
        var Android_Build = await Service.importRawContext(null,"java","android.os.Build",false,"");
        print("Android_Build = "+ await Android_Build.getString());
        StarObjectClass VERSION = await Android_Build["VERSION"];
        print("$VERSION = ");
        var c_acti = await Starflut.getActivity(Service);
        print("Activity = "+await c_acti.getString());

        dynamic obj1 = await c_acti.call("getApplicationContext",[]);
        print("getApplicationContext = $obj1");
        print("getApplicationContext = "+ await obj1.getString());

        dynamic Android_Context = await Service.importRawContext(null,"java","android.content.Context",false,"");
        dynamic TELEPHONY_SERVICE = await Android_Context['TELEPHONY_SERVICE'];
        print("TELEPHONY_SERVICE = $TELEPHONY_SERVICE ");

        dynamic obj2 = await obj1.call("getPackageManager",[]);
        print("getPackageManager = "+ await obj2.getString());

        dynamic obj3 = await obj2["FEATURE_CAMERA"];
        print("FEATURE_CAMERA = $obj3");

        dynamic Android_PackageManager = await Service.importRawContext(null,"java","android.content.pm.PackageManager",false,"");
        print("Android_PackageManager = "+ await Android_PackageManager.getString());

        dynamic obj4 = await Android_PackageManager["FEATURE_CAMERA"];
        print("FEATURE_CAMERA = $obj4");

      }else if (TestCase == 2) {
        String Path1  = await Starflut.getResourcePath();
        String Path2 = await Starflut.getAssetsPath();
        StarCoreFactory starcore = await Starflut.getFactory();
        StarServiceClass Service = await starcore.initSimple("test", "123", 0, 0, []);
        await starcore.regMsgCallBackP(
                (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
              print("$serviceGroupID  $uMsg   $wParam   $lParam");
              return null;
            });
        StarSrvGroupClass SrvGroup = await Service["_ServiceGroup"];

        /*--macos--*/
        int Platform = await Starflut.getPlatform();
        if( Platform == Starflut.MACOS ) {
          await starcore.setShareLibraryPath(
              Path1); //set path for interface library
          bool LoadResult = await Starflut.loadLibrary(Path1+"/libpython3.9.dylib");
          print("$LoadResult");  //--load
          await Starflut.setEnv("PYTHONPATH","/Library/Frameworks/Python.framework/Versions/3.9/lib/python3.9");
          String pypath = await Starflut.getEnv("PYTHONPATH");
          print("$pypath");
        }else if( Platform == Starflut.WINDOWS ) {
          await starcore.setShareLibraryPath(
              Path1.replaceAll("\\","/")); //set path for interface library
        }

        /*---script python--*/
        bool isAndroid = await Starflut.isAndroid();
        if( isAndroid == true ){
          await Starflut.copyFileFromAssets("testcallback.py", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.copyFileFromAssets("testpy.py", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.copyFileFromAssets("python3.9.zip", null,null);  //desRelatePath must be null

          var nativepath = await Starflut.getNativeLibraryDir();
          var LibraryPath = "";
          if( nativepath.contains("x86_64"))
            LibraryPath = "x86_64";
          else if( nativepath.contains("arm64"))
            LibraryPath = "arm64-v8a";
          else if( nativepath.contains("arm"))
            LibraryPath = "armeabi";
          else if( nativepath.contains("x86"))
            LibraryPath = "x86";

          await Starflut.copyFileFromAssets("zlib.cpython-39.so", LibraryPath,null);
          await Starflut.copyFileFromAssets("unicodedata.cpython-39.so", LibraryPath,null);
          await Starflut.loadLibrary("libpython3.9.so");
        }

        String docPath = await Starflut.getDocumentPath();
        print("docPath = $docPath");

        String resPath = await Starflut.getResourcePath();
        print("resPath = $resPath");

        String assetsPath = await Starflut.getAssetsPath();
        print("assetsPath = $assetsPath");

        dynamic rr1 = await SrvGroup.initRaw("python39", Service);

        print("initRaw = $rr1");
        var Result = await SrvGroup.loadRawModule("python", "", assetsPath + "/flutter_assets/starfiles/" + "testpy.py", false);
        print("loadRawModule = $Result");

        dynamic python = await Service.importRawContext(null,"python", "", false, "");
        print("python = "+ await python.getString());

        StarObjectClass retobj = await python.call("tt", ["hello ", "world"]);
        print(await retobj[0]);
        print(await retobj[1]);

        print(await python["g1"]);

        StarObjectClass yy = await python.call("yy", ["hello ", "world", 123]);
        print(await yy.call("__len__",[]));

        StarObjectClass multiply = await Service.importRawContext(null,"python", "Multiply", true, "");
        StarObjectClass multiply_inst = await multiply.newObject(["", "", 33, 44]);
        print(await multiply_inst.getString());

        print(await multiply_inst.call("multiply", [11, 22]));

        await SrvGroup.clearService();
        await starcore.moduleExit();

        print("finish");

      }else if (TestCase == 3) {
        String Path1  = await Starflut.getResourcePath();
        String Path2 = await Starflut.getAssetsPath();
        StarCoreFactory starcore = await Starflut.getFactory();

        String FrameTag = Starflut.newLocalFrame();

        StarServiceClass Service = await starcore.initSimple("test", "123", 0, 0, []);
        await starcore.regMsgCallBackP(
                (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
              print("$serviceGroupID  $uMsg   $wParam   $lParam");
              return null;
            });
        StarSrvGroupClass SrvGroup = await Service["_ServiceGroup"];

        int Platform = await Starflut.getPlatform();
        if( Platform == Starflut.MACOS ) {
          await starcore.setShareLibraryPath(
              Path1); //set path for interface library
          bool LoadResult = await Starflut.loadLibrary(Path1+"/libpython3.9.dylib");
          print("$LoadResult");  //--load
          await Starflut.setEnv("PYTHONPATH","/Library/Frameworks/Python.framework/Versions/3.9/lib/python3.9");
          String pypath = await Starflut.getEnv("PYTHONPATH");
          print("$pypath");
        }else if( Platform == Starflut.WINDOWS ) {
          //await starcore.setShareLibraryPath(
          //    Path1.replaceAll("\\","/")); //set path for interface library
        }
        /*---script python--*/
        bool isAndroid = await Starflut.isAndroid();
        if( isAndroid == true ){
          await Starflut.copyFileFromAssets("testcallback.py", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.copyFileFromAssets("testpy.py", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.copyFileFromAssets("python3.9.zip", null,null);   //desRelatePath must be null

          var nativepath = await Starflut.getNativeLibraryDir();
          var LibraryPath = "";
          if( nativepath.contains("x86_64"))
            LibraryPath = "x86_64";
          else if( nativepath.contains("arm64"))
            LibraryPath = "arm64-v8a";
          else if( nativepath.contains("arm"))
            LibraryPath = "armeabi";
          else if( nativepath.contains("x86"))
            LibraryPath = "x86";

          await Starflut.copyFileFromAssets("zlib.cpython-39.so", LibraryPath,null);
          await Starflut.copyFileFromAssets("unicodedata.cpython-39.so", LibraryPath,null);
          await Starflut.loadLibrary("libpython3.9.so");
        }

        String docPath = await Starflut.getDocumentPath();
        print("docPath = $docPath");

        String resPath = await Starflut.getResourcePath();
        print("resPath = $resPath");

        String assetsPath = await Starflut.getAssetsPath();
        print("assetsPath = $assetsPath");

        print("begin init Raw)");
        bool rr1 = await SrvGroup.initRaw("python39", Service);
        print("initRaw = $rr1");

        StarObjectClass python = await Service.importRawContext(FrameTag,"python", "", false, "");

        python.lock();

        print(await python.getValue(["_Service","_Class","_ID","_Name"]));

        print("python = "+ await python.getString());

        await SrvGroup.doFile("python", assetsPath + "/flutter_assets/starfiles/" + "testcallback.py");

        StarObjectClass CallBackObj = await Service.newObject(FrameTag,[]);
        await python.setValue("CallBackObj", CallBackObj);

        await CallBackObj.regScriptProcP("PrintHello", (StarObjectClass cleObject, String FrameTag,List paras ) async {
          print("$paras");
          return ["return from python", {"pa":1, "pb":2, "pc":3},345.4];
        });

        /*
        StarObjectClass retobj = await python.call("tt", ["hello ", "world"]);
        print("retobj  = $retobj");
        */

        var rrr = await python.call("tt", [{"a":1, "b":2, "c":3}, "world"]);
        print("return from python $rrr");

        Starflut.freeLocalFrame(FrameTag);

        await starcore.moduleExit();

        print("finish");

      }else if (TestCase == 4) {
        StarCoreFactory starcore = await Starflut.getFactory();
        int Result = 0;
        /*Result =
            await starcore.initCore(true, false, false, true, "", 0, "", 0);
            */
        StarServiceClass Service = await starcore.initSimple("test", "123", 0, 0, []);
        print("$Service");
        await starcore.regMsgCallBackP(
                (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
              print("$serviceGroupID  $uMsg   $wParam   $lParam");
              return null;
            });
        StarSrvGroupClass SrvGroup = await Service["_ServiceGroup"];
        print("$SrvGroup");

        print(await SrvGroup.activeScriptInterface("ruby"));
        print(await Service.runScript("python39","print(123)",null,null));
        print("+++++++++++++++++++++++");

        var srvp = await SrvGroup.getServicePath();
        print("$srvp");

        var curp = await SrvGroup.getCurrentPath();
        print("$curp");

        List<int> listB = [1, 2, 3, 4, 5];
        List<String> numbersB = ['one', 'two', 'three', 'four'];
        Map<String, int> map = {"a":1, "b":2, "c":3};

        StarParaPkgClass ParaPkg = await SrvGroup.newParaPkg(null,[
          34359738367,
          'aaaa',
          234,
          false,
          [345,345],
          listB,
          numbersB,
          map
        ]);
        print(await ParaPkg.getValue([1,4,5]));

        print(await ParaPkg.toTuple());

        StarParaPkgClass ParaPkg1 = await SrvGroup.newParaPkg(null,[map]);

        StarBinBufClass BinBuf = await SrvGroup.newBinBuf(null);
        print(await BinBuf.read(0,100));

        String docPath = await Starflut.getDocumentPath();
        print("docPath = $docPath");

        /*---script python--*/
        bool isAndroid = await Starflut.isAndroid();
        if( isAndroid == true ){
          await Starflut.copyFileFromAssets("testcallback.py", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.copyFileFromAssets("testpy.py", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.copyFileFromAssets("python3.9.zip", null,null);  //desRelatePath must be null

          var nativepath = await Starflut.getNativeLibraryDir();
          var LibraryPath = "";
          if( nativepath.contains("x86_64"))
            LibraryPath = "x86_64";
          else if( nativepath.contains("arm64"))
            LibraryPath = "arm64-v8a";
          else if( nativepath.contains("arm"))
            LibraryPath = "armeabi";
          else if( nativepath.contains("x86"))
            LibraryPath = "x86";

          await Starflut.copyFileFromAssets("zlib.cpython-39.so", LibraryPath,null);
          await Starflut.copyFileFromAssets("unicodedata.cpython-39.so", LibraryPath,null);
          await Starflut.loadLibrary("libpython3.9.so");
        }

        print("begin init Raw)");
        bool rr1 = await SrvGroup.initRaw("python39", Service);
        print("initRaw = $rr1");

        StarObjectClass python = await Service.importRawContext(null,"python", "", false, "");
        print(await python.getString());

        bool run_result =
        await SrvGroup.runScript("python", "print(123)", "");
        print("$run_result");

/*
        run_result =
            await SrvGroup.runScript("ruby", "puts(123)", "");
        print("ruby == $run_result");
*/

        python.lock();

        print(await python.getValue(["_Service","_Class","_ID","_Name"]));

        print("python = "+ await python.getString());

        String resPath = await Starflut.getResourcePath();
        print("resPath = $resPath");

        await SrvGroup.doFile("python", resPath + "/flutter_assets/starfiles/" + "testcallback.py");

        StarObjectClass CallBackObj = await Service.newObject(null,[]);
        await python.setValue("CallBackObj", CallBackObj);

        await CallBackObj.regScriptProcP("PrintHello", (StarObjectClass cleObject, String FrameTag, List paras ) async {
          print("$paras");
          return ["return from python", 345.4];
        });
        await python.call("tt", ["hello ", "world"]);

      }else if (TestCase == 5) {
        String Path1  = await Starflut.getResourcePath();
        String Path2 = await Starflut.getAssetsPath();
        StarCoreFactory starcore = await Starflut.getFactory();
        StarServiceClass Service = await starcore.initSimple("test", "123", 0, 0, []);
        print("$Service");
        await starcore.regMsgCallBackP(
                (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
              print("$serviceGroupID  $uMsg   $wParam   $lParam");
              return null;
            });
        StarSrvGroupClass SrvGroup = await Service["_ServiceGroup"];
        print("$SrvGroup");

        /*--macos--*/
        int Platform = await Starflut.getPlatform();
        if( Platform == Starflut.MACOS ) {
          await starcore.setShareLibraryPath(
              Path1); //set path for interface library
          bool LoadResult = await Starflut.loadLibrary(Path1+"/libruby.2.5.1.dylib");
          print("$LoadResult");  //--load
          bool SetResult = await starcore.setScript("ruby","","-m "+Path1+"/libruby.2.5.1.dylib");
          print("SetResult = $SetResult");  //--set ruby module
        }

        String docPath = await Starflut.getDocumentPath();
        print("docPath = $docPath");

        String resPath = await Starflut.getResourcePath();
        print("resPath = $resPath");

        String assetsPath = await Starflut.getAssetsPath();
        print("assetsPath = $assetsPath");

        bool InitResult = await SrvGroup.initRaw("ruby",Service);
        print("InitResult = $InitResult");
        StarObjectClass ruby = await Service.importRawContext(null,"ruby","",false,"");
        print(await ruby["LOAD_PATH"]);
        print(await ruby["File"]);

        StarObjectClass loadPATH = await ruby["LOAD_PATH"];
        print(loadPATH);
        await loadPATH.call("unshift", [docPath]);

        bool isAndroid = await Starflut.isAndroid();
        if( isAndroid == true ){
          await Starflut.copyFileFromAssets("testcallback.rb", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.loadLibrary("libruby.so");
        }

        await SrvGroup.doFile("ruby", assetsPath + "/flutter_assets/starfiles/" + "testcallback.rb");

        var srvp = await SrvGroup.getServicePath();
        print("$srvp");

        print(await ruby["\$g1"]);
        print("\$g1");

        StarObjectClass CallBackObj = await Service.newObject(null,[]);
        await ruby.setValue("\$CallBackObj", CallBackObj);

        await CallBackObj.regScriptProcP("PrintHello", (StarObjectClass cleObject, String FrameTag,List paras ) async {
          print("$paras");
          return ["return from ruby", {"pa":1, "pb":2, "pc":3},345.4];
        });

        StarObjectClass rrr = await ruby.call("tt", [{"a":1, "b":2, "c":3}, "world"]);
        print("return from ruby $rrr");

        var rw = await rrr.rawToParaPkg();
        print(rw);
      }else if (TestCase == 6) {
        /*ios  only*/
        StarCoreFactory starcore = await Starflut.getFactory();
        StarServiceClass Service = await starcore.initSimple("test", "123", 0, 0, []);
        print("$Service");
        await starcore.regMsgCallBackP(
                (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
              print("$serviceGroupID  $uMsg   $wParam   $lParam");
              return null;
            });
        await Service.checkPassword(false);
        StarSrvGroupClass SrvGroup = await Service["_ServiceGroup"];
        print("$SrvGroup");

        String docPath = await Starflut.getDocumentPath();
        print("docPath = $docPath");

        String resPath = await Starflut.getResourcePath();
        print("resPath = $resPath");

        print(await Service.doFile("","libstar_go.dynlib",""));
        var goObj = await Service.getObject(null,"GoObject");
        print(goObj);

        print("--------------");

        var result = await goObj.call("PrintHello",["------------1",234.56]);
        print(result);

        print("----call lua---");
        await SrvGroup.initRaw("lua",Service);
        StarObjectClass lua = await Service.importRawContext(null,"lua","",false,"");
        print(await lua.getString());

        StarObjectClass luapackage = await lua["package"];
        String luapath = await luapackage["path"];
        print(luapath);

        bool isAndroid = await Starflut.isAndroid();
        if( isAndroid == true ){
          await Starflut.copyFileFromAssets("mymath.lua", "flutter_assets/starfiles","flutter_assets/starfiles");
        }

        await luapackage.setValue("path",luapath + ";$resPath/flutter_assets/starfiles/?.lua");
        print(await luapackage["path"]);
        await lua.call("execute",['mymath=require \"mymath\"']);

        StarObjectClass mymath = await lua["mymath"];
        print(await mymath.call("add",[3334,5666]));

      }else if (TestCase == 7) {
        StarCoreFactory starcore = await Starflut.getFactory();
        StarServiceClass Service = await starcore.initSimple("test", "123", 0, 0, []);
        print("$Service");
        await starcore.regMsgCallBackP(
                (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
              print("$serviceGroupID  $uMsg   $wParam   $lParam");
              return null;
            });
        await Service.checkPassword(false);
        StarSrvGroupClass SrvGroup = await Service["_ServiceGroup"];
        print("$SrvGroup");

        String docPath = await Starflut.getDocumentPath();
        print("docPath = $docPath");

        String resPath = await Starflut.getResourcePath();
        print("resPath = $resPath");

        print("----call lua---");
        await SrvGroup.initRaw("lua",Service);
        StarObjectClass lua = await Service.importRawContext(null,"lua","",false,"");
        print(await lua.getString());

        StarObjectClass luapackage = await lua["package"];
        String luapath = await luapackage["path"];
        print(luapath);

        bool isAndroid = await Starflut.isAndroid();
        if( isAndroid == true ){
          await Starflut.copyFileFromAssets("mymath.lua", "flutter_assets/starfiles","flutter_assets/starfiles");
        }

        await luapackage.setValue("path",luapath + ";$resPath/flutter_assets/starfiles/?.lua");
        print(await luapackage["path"]);
        await lua.call("execute",['mymath=require \"mymath\"']);

        StarObjectClass mymath = await lua["mymath"];
        print(await mymath.call("add",[3334,5666]));

      }else if (TestCase == 8) {
        StarCoreFactory starcore = await Starflut.getFactory();
        StarServiceClass Service = await starcore.initSimple("test", "123", 0, 0, []);
        print("$Service");
        await starcore.regMsgCallBackP(
                (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
              print("$serviceGroupID  $uMsg   $wParam   $lParam");
              return null;
            });
        await Service.checkPassword(false);
        StarSrvGroupClass SrvGroup = await Service["_ServiceGroup"];
        print("$SrvGroup");

        String docPath = await Starflut.getDocumentPath();
        print("docPath = $docPath");

        String resPath = await Starflut.getResourcePath();
        print("resPath = $resPath");

        StarObjectClass testObj = await Service.newObject(null,[]);

        await testObj.regScriptProcP("GetInfo", (StarObjectClass cleObject,String FrameTag, List paras ) async {
          print("$paras");
          return ["return from ", {"pa":1, "pb":2, "pc":3},345.4];
        });

        StarObjectClass testObj_inst = await testObj.newObject([]);
        int InstNumber = await testObj.instNumber();
        print("$InstNumber");

        List allobjects = await Service.allObject(null);
        print("$allobjects");

        StarParaPkgClass ParaPkg = await SrvGroup.newParaPkg(null,[
          'aaaa',
          234,
          [345]
        ]);

        String val = await ParaPkg.v;
        print("$val");

        StarParaPkgClass ParaPkg1 = await SrvGroup.newParaPkg(null,[
          'aaaab',
          234,
          [345]
        ]);
        StarParaPkgClass ParaPkg2 = await SrvGroup.newParaPkg(null,[
          'aaaa',
          234,
          [345]
        ]);
        bool e1 = await ParaPkg.equals(ParaPkg1);
        print("$e1");
        bool e2 = await ParaPkg.equals(ParaPkg2);
        print("$e2");

        String res = await testObj.jsonCall("{\"jsonrpc\": \"2.0\", \"method\": \"GetInfo\", \"params\": [34,{\"sape\":4139,\"jack\":4098}], \"id\": 1}");
        print("$res");

        String objid = await testObj.getValue("_ID");
        List res1 = await Service.restfulCall(null,"/"+objid+"/proc/GetInfo","POST","{\"params\": [34,{\"sape\":4139,\"jack\":4098}]}");
        print("$res1");
      }
    } on PlatformException catch (e) {
      print("{$e.message}");
      platformVersion = 'Failed to get platform version.';
    }
    // If the widget was removed from the tree while the asynchronous platform
    // message was in flight, we want to discard the reply rather than calling
    // setState to update our non-existent appearance.
    if (!mounted) return;

    setState(() {
      _platformVersion = platformVersion;
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Text('Running on: $_platformVersion\n'),
        ),
      ),
    );
  }
}

import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:starflut/starflut.dart';

void main() => runApp(new MyApp());

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => new _MyAppState();
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
      int TestCase = 7;  //6 -- go //---5 ruby  //---4 ios //--3 call back;  //--2  call python //--1 android api //--0 develop

      if (TestCase == 0) {
        StarCoreFactory starcore = await Starflut.getFactory();
        int Result = 0;
        Result =
            await starcore.initCore(true, false, false, true, "", 0, "", 0);
        await starcore.regMsgCallBackP(
            (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
          print("$serviceGroupID  $uMsg   $wParam   $lParam");
          return null;
        });
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

        StarParaPkgClass ParaPkg = await StarSrvGroup.newParaPkg([
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
        StarParaPkgClass ParaPkg1 = await StarSrvGroup.newParaPkg({"a":1, "b":2, "c":3});
        print(await ParaPkg1[0]);
        print(await ParaPkg1.toTuple());

        StarParaPkgClass ParaPkg2 = await StarSrvGroup.newParaPkg([345.67,{"a":1, "b":2, "c":3}]);
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
            await Starflut.copyFileFromAssets("python3.6.zip", "flutter_assets/starfiles",null);   //desRelatePath must be null
          print("$py_result1");
          py_result1 =
              await Starflut.copyFileFromAssets("zlib.cpython-36m.so", null,null);
          print("$py_result1");
          py_result1 = await Starflut.copyFileFromAssets(
            "unicodedata.cpython-36m.so", null,null);
          print("$py_result1");

          py_result1 = await Starflut.loadLibrary("libpython3.6m.so");
           print("$py_result1");
        }

        bool run_result =
            await StarSrvGroup.runScript("python36", "print(123)", "");
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
        var Android_Build = await Service.importRawContext("java","android.os.Build",false,""); 
        print("Android_Build = "+ await Android_Build.getString());
        StarObjectClass VERSION = await Android_Build["VERSION"];  
        print("$VERSION = ");
        var c_acti = await Starflut.getActivity(Service);
        print("Activity = "+await c_acti.getString());
        
        dynamic obj1 = await c_acti.call("getApplicationContext",[]);
        print("getApplicationContext = $obj1");
        print("getApplicationContext = "+ await obj1.getString());

        dynamic Android_Context = await Service.importRawContext("java","android.content.Context",false,""); 
        dynamic TELEPHONY_SERVICE = await Android_Context['TELEPHONY_SERVICE'];
        print("TELEPHONY_SERVICE = $TELEPHONY_SERVICE ");

        dynamic obj2 = await obj1.call("getPackageManager",[]);
        print("getPackageManager = "+ await obj2.getString());

        dynamic obj3 = await obj2["FEATURE_CAMERA"];
        print("FEATURE_CAMERA = $obj3");

        dynamic Android_PackageManager = await Service.importRawContext("java","android.content.pm.PackageManager",false,"");
        print("Android_PackageManager = "+ await Android_PackageManager.getString());

        dynamic obj4 = await Android_PackageManager["FEATURE_CAMERA"];
        print("FEATURE_CAMERA = $obj4");        

      }else if (TestCase == 2) {
        StarCoreFactory starcore = await Starflut.getFactory();
        StarServiceClass Service = await starcore.initSimple("test", "123", 0, 0, []);
        await starcore.regMsgCallBackP(
            (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
          print("$serviceGroupID  $uMsg   $wParam   $lParam");
          return null;
        });
        StarSrvGroupClass SrvGroup = await Service["_ServiceGroup"];

        /*---script python--*/
        bool isAndroid = await Starflut.isAndroid();
        if( isAndroid == true ){
          await Starflut.copyFileFromAssets("testcallback.py", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.copyFileFromAssets("testpy.py", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.copyFileFromAssets("python3.6.zip", "flutter_assets/starfiles",null);  //desRelatePath must be null 
          await Starflut.copyFileFromAssets("zlib.cpython-36m.so", null,null);
          await Starflut.copyFileFromAssets("unicodedata.cpython-36m.so", null,null);
          await Starflut.loadLibrary("libpython3.6m.so");
        }

        String docPath = await Starflut.getDocumentPath();
        print("docPath = $docPath");

        String resPath = await Starflut.getResourcePath();
        print("resPath = $resPath");

        dynamic rr1 = await SrvGroup.initRaw("python36", Service);

        print("initRaw = $rr1");
		    var Result = await SrvGroup.loadRawModule("python", "", resPath + "/flutter_assets/starfiles/" + "testpy.py", false);
        print("loadRawModule = $Result");

		    dynamic python = await Service.importRawContext("python", "", false, "");
        print("python = "+ await python.getString());

		    StarObjectClass retobj = await python.call("tt", ["hello ", "world"]);
        print(await retobj[0]);
        print(await retobj[1]);

        print(await python["g1"]);
        
        StarObjectClass yy = await python.call("yy", ["hello ", "world", 123]);
        print(await yy.call("__len__",[]));

        StarObjectClass multiply = await Service.importRawContext("python", "Multiply", true, "");
        StarObjectClass multiply_inst = await multiply.newObject(["", "", 33, 44]);
        print(await multiply_inst.getString());

        print(await multiply_inst.call("multiply", [11, 22]));

        await SrvGroup.clearService();
		    await starcore.moduleExit();

      }else if (TestCase == 3) {      
        StarCoreFactory starcore = await Starflut.getFactory();
        
        Starflut.pushLocalFrame();

        StarServiceClass Service = await starcore.initSimple("test", "123", 0, 0, []);
        await starcore.regMsgCallBackP(
            (int serviceGroupID, int uMsg, Object wParam, Object lParam) async{
          print("$serviceGroupID  $uMsg   $wParam   $lParam");
          return null;
        });
        StarSrvGroupClass SrvGroup = await Service["_ServiceGroup"];

        /*---script python--*/
        bool isAndroid = await Starflut.isAndroid();
        if( isAndroid == true ){
          await Starflut.copyFileFromAssets("testcallback.py", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.copyFileFromAssets("testpy.py", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.copyFileFromAssets("python3.6.zip", "flutter_assets/starfiles",null);   //desRelatePath must be null
          await Starflut.copyFileFromAssets("zlib.cpython-36m.so", null,null);
          await Starflut.copyFileFromAssets("unicodedata.cpython-36m.so", null,null);
          await Starflut.loadLibrary("libpython3.6m.so");
        }

        String docPath = await Starflut.getDocumentPath();
        print("docPath = $docPath");

        String resPath = await Starflut.getResourcePath();
        print("resPath = $resPath");

        print("begin init Raw)");
        bool rr1 = await SrvGroup.initRaw("python36", Service);
        print("initRaw = $rr1");

		    StarObjectClass python = await Service.importRawContext("python", "", false, "");
        
        python.lock();

        print(await python.getValue(["_Service","_Class","_ID","_Name"]));

        print("python = "+ await python.getString());

		    await SrvGroup.doFile("python", resPath + "/flutter_assets/starfiles/" + "testcallback.py");

		    StarObjectClass CallBackObj = await Service.newObject([]);
		    await python.setValue("CallBackObj", CallBackObj);

		    await CallBackObj.regScriptProcP("PrintHello", (StarObjectClass cleObject, List paras ) async {
			    print("$paras");
			    return ["return from go", {"pa":1, "pb":2, "pc":3},345.4];
		      });
        
        /*
        StarObjectClass retobj = await python.call("tt", ["hello ", "world"]);
        print("retobj  = $retobj");
        */

        Starflut.popLocalFrame();

        var rrr = await python.call("tt", [{"a":1, "b":2, "c":3}, "world"]);
        print("return from python $rrr");

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
        print(await Service.runScript("python36","print(123)",null,null));
        print("+++++++++++++++++++++++");

        var srvp = await SrvGroup.getServicePath();
        print("$srvp");

        var curp = await SrvGroup.getCurrentPath();
        print("$curp");

        List<int> listB = [1, 2, 3, 4, 5];
        List<String> numbersB = ['one', 'two', 'three', 'four'];
        Map<String, int> map = {"a":1, "b":2, "c":3};

        StarParaPkgClass ParaPkg = await SrvGroup.newParaPkg([
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

        StarParaPkgClass ParaPkg1 = await SrvGroup.newParaPkg([map]);

        StarBinBufClass BinBuf = await SrvGroup.newBinBuf();
        print(await BinBuf.read(0,100));
        
        String docPath = await Starflut.getDocumentPath();
        print("docPath = $docPath");

        /*---script python--*/
        bool isAndroid = await Starflut.isAndroid();
        if( isAndroid == true ){
          await Starflut.copyFileFromAssets("testcallback.py", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.copyFileFromAssets("testpy.py", "flutter_assets/starfiles","flutter_assets/starfiles");
          await Starflut.copyFileFromAssets("python3.6.zip", "flutter_assets/starfiles",null);  //desRelatePath must be null 
          await Starflut.copyFileFromAssets("zlib.cpython-36m.so", null,null);
          await Starflut.copyFileFromAssets("unicodedata.cpython-36m.so", null,null);
          await Starflut.loadLibrary("libpython3.6m.so");
        }

        print("begin init Raw)");
        bool rr1 = await SrvGroup.initRaw("python36", Service);
        print("initRaw = $rr1");

		    StarObjectClass python = await Service.importRawContext("python", "", false, "");
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

		    StarObjectClass CallBackObj = await Service.newObject([]);
		    await python.setValue("CallBackObj", CallBackObj);

		    await CallBackObj.regScriptProcP("PrintHello", (StarObjectClass cleObject, List paras ) async {
			    print("$paras");
			    return ["return from go", 345.4];
		      });
         await python.call("tt", ["hello ", "world"]);

      }else if (TestCase == 5) {
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

        String docPath = await Starflut.getDocumentPath();
        print("docPath = $docPath");

        String resPath = await Starflut.getResourcePath();
        print("resPath = $resPath");

        await SrvGroup.initRaw("ruby",Service);
		    StarObjectClass ruby = await Service.importRawContext("ruby","",false,"");
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
 
        await SrvGroup.doFile("ruby", resPath + "/flutter_assets/starfiles/" + "testcallback.rb");

        var srvp = await SrvGroup.getServicePath();
        print("$srvp");   

        print(await ruby["\$g1"]);
        print("\$g1");

        StarObjectClass CallBackObj = await Service.newObject([]);
		    await ruby.setValue("\$CallBackObj", CallBackObj);

		    await CallBackObj.regScriptProcP("PrintHello", (StarObjectClass cleObject, List paras ) async {
			    print("$paras");
			    return ["return from go", {"pa":1, "pb":2, "pc":3},345.4];
		      });
          
        StarObjectClass rrr = await ruby.call("tt", [{"a":1, "b":2, "c":3}, "world"]);
        print("return from ruby $rrr");

        var rw = await rrr.rawToParaPkg();
        print(rw);
      }else if (TestCase == 6) {
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
        var goObj = await Service.getObject("GoObject"); 
        print(goObj);

        print("--------------"); 
        
        var result = await goObj.call("PrintHello",["------------1",234.56]);
        print(result);

        print("----call lua---");
        await SrvGroup.initRaw("lua",Service);
        StarObjectClass lua = await Service.importRawContext("lua","",false,"");  
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
        StarObjectClass lua = await Service.importRawContext("lua","",false,"");  
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

      }
    } on PlatformException catch (e) {
      print("{e.message}");
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
    return new MaterialApp(
      home: new Scaffold(
        appBar: new AppBar(
          title: const Text('Plugin example app'),
        ),
        body: new Center(
          child: new Text('Running on: $_platformVersion\n'),
        ),
      ),
    );
  }
}

/*
static const platform = const MethodChannel('samples.flutter.io/initialiseRanging');
static const stream =
  const EventChannel('samples.flutter.io/ranging');
try {
        await platform.invokeMethod('initialiseRanging').then((result){
          print(result);
          setState(() {
            _currentValue = result;
          });

          stream.receiveBroadcastStream().listen(_onEvent, onError: _onError);
        });

  } on PlatformException catch (e) {
        print( "{e.message}");
  }

*/

/*
String selfType = runtimeType.toString();




typedef dynamic ApplyType(List positionalArguments);

class Variadic implements Function {
  final ApplyType _apply;

  Variadic(this._apply) {}

  @override
  dynamic noSuchMethod(Invocation invocation) {
    if (invocation.memberName == #call) {
      if (invocation.isMethod)
        return _apply(invocation.positionalArguments);
      if (invocation.isGetter)
        return this;
    }
    return super.noSuchMethod(invocation);
  }
}

num sumList(List<num> xs) =>
  xs.fold(0, (num acc, num x) => acc + x);

final Function sumVariadic = new Variadic(sumList);

void main() {
  print(sumList([100, 200, 300]));
  print(sumVariadic(100, 200, 300));
}



Multiple Inheritance
In terms of multiple inheritance, this is not possible in Dart. You can, however, implement multiple interfaces and provide your own implementations of the required methods, eg:
class MultiDuck implements Quackable, EnterpriseDuck, Swimable {
  // snip...
}











Interfaces are useful because they allow you to switch implementations of a class, whilst still allowing validation that the type being passed in meets the requirements of the interface.
Take the following (often used) example:
interface Quackable {
  void quack();
}
This defines the requirements of a class that will be passed to a method such as:
sayQuack(Quackable quackable) {
   quackable.quack();
}
which allows you to make use of any implementation of a Quackable object, such as:
class MockDuck implements Quackable {
  void quack() => print("quack");
}

class EnterpriseDuck implements Quackable {
  void quack() {
    // connect to three enterprise "ponds"
    // and eat some server bread
    // and say "quack" using an messaging system
  }

}
Both of these implementations will work with the sayQuack() function, but one requires significantly less infrastructure than the other. 
sayQuack(new EnterpriseDuck());
sayQuack(new MockDuck());




object之所以能够被赋值为任意类型的原因，其实都知道，因为所有的类型都派生自object. 
它可以赋值为任何类型:
object a = 1;  
a = "Test";

dynamic不是在编译时候确定实际类型的, 而是在运行时。
所以下面的代码是能够通过编译的，但是会在运行时报错:
dynamic a = "test";  
a++;  


不指定返回值类型的函数

 * 我们可以不指定返回值类型，这样的函数返回值默认为Object，也就是说你可以返回任意类型 
 * params: 可以是任意类型 
  
printReturnObjectParams(Object params){  
  print(params);  
  return params;  
}  
printObjectParams(Object params){  
  print(params);  
}  



这里我们提到了num关键字，在上篇博客中没有讲到，这里简单说一下。

 * An integer or floating-point number. 
 * It is a compile-time error for any type other than [int] or [double] 
 * to attempt to extend or implement num. 
翻译：  

  * 整数或浮点数。 
  * 除[int]或[double]之外的任何类型都是编译时错误 
  * 试图扩展或实现num。 




main() {
  var callbacks = [];
  for (var i = 0; i < 2; i++) {
    callbacks.add(() => print(i)); // 在列表 callbacks 中添加一个函数对象，这个函数会记住 for 循环中当前 i 的值。
  }
  callbacks.forEach((c) => c()); // 分别输出 0 和 1
}

Typedef
可以使用 typedef 关键字定义函数的类型，也就是一个别名，类型信息包含函数的参数和返回值类型。
typedef int Compare(int a, int b);
int sort(int a, int b) => a - b;









*/

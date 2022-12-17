import Cocoa
import FlutterMacOS

public class StarflutPlugin: NSObject, FlutterPlugin {
    public var StarflutPluginStub : StarflutPluginCommon?
    
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "starflut", binaryMessenger: registrar.messenger)
    let instance = StarflutPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
    instance.StarflutPluginStub = StarflutPluginCommon.starflut_plugin_common_init(channel)
  }
    
    override init(){
        super.init()
        self.StarflutPluginStub = nil
    }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
/*
    switch call.method {
    case "getPlatformVersion":
      result("macOS " + ProcessInfo.processInfo.operatingSystemVersionString)
    default:
      result(FlutterMethodNotImplemented)
    }
*/
    let stub : StarflutPluginCommon = StarflutPluginStub!
    stub.handleMethodCall_Common(call,result:result);
  }
}



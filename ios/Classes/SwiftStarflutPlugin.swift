import Flutter
import UIKit

public class SwiftStarflutPlugin: NSObject, FlutterPlugin {

  public var StarflutPluginStub : StarflutPluginCommon?

  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "starflut", binaryMessenger: registrar.messenger())
    let instance = SwiftStarflutPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
    instance.StarflutPluginStub = StarflutPluginCommon.starflut_plugin_common_init(channel)
  }

  override init(){
    super.init()
    self.StarflutPluginStub = nil
  }
  /*
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "starflut", binaryMessenger: registrar.messenger())
    let instance = SwiftStarflutPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
  }
  */

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
  /*
    result("iOS " + UIDevice.current.systemVersion)
  */
    let stub : StarflutPluginCommon = StarflutPluginStub!
    stub.handleMethodCall_Common(call,result:result);
  }
}

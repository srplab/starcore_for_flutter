#import <Foundation/Foundation.h>
#import <Flutter/Flutter.h>

@interface StarflutPluginCommon : NSObject

+ (StarflutPluginCommon *)starflut_plugin_common_init:(FlutterMethodChannel *)channel_arg;
- (void)handleMethodCall_Common:(FlutterMethodCall*)call result:(FlutterResult)result;

@end

#import "StarflutPlugin.h"
#if __has_include(<starflut/starflut-Swift.h>)
#import <starflut/starflut-Swift.h>
#else
// Support project import fallback if the generated compatibility header
// is not copied when this plugin is created as a library.
// https://forums.swift.org/t/swift-static-libraries-dont-copy-generated-objective-c-header/19816
#import "starflut-Swift.h"
#endif

@implementation StarflutPlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftStarflutPlugin registerWithRegistrar:registrar];
}
@end

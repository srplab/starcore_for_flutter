import 'package:starflut/src/starcore_base.dart';

class StarObjectRecordClass {
  StarCoreBase? starObject;
  //String codeLocation;
  DateTime? lastAccessTag;
  String? typeString;

  /*--from https://github.com/dart-lang/logging/issues/32--*/
  /*
  Frame _findCallerFrame(Trace trace) {
    bool foundLogging = false;
    for (int i = 0; i < trace.frames.length; ++i) {
      Frame frame = trace.frames[i];

      bool loggingPackage = frame.package == 'logging';
      if (foundLogging && !loggingPackage) {
        return frame;
      }
      foundLogging = loggingPackage;
    }
    return null;
  }
  */

  StarObjectRecordClass(
      StarCoreBase starObject, String typeString, int callerDeepth) {
    this.starObject = starObject;
    lastAccessTag = new DateTime.now();
    this.typeString = typeString;
/*
    //--record current caller line number and file name
    Trace trace = new Trace.current();
    Frame callerFrame;
    if( trace.frames.length > callerDeepth )
      callerFrame = trace.frames[callerDeepth];
    else
      callerFrame = null;
    //print(trace);
    //print(callerFrame);
    if (callerFrame != null) {
      if (callerFrame.uri.pathSegments.isNotEmpty) {
        String filename = callerFrame.uri.pathSegments.last;
        String line = callerFrame.line != null ? '(${callerFrame.line})' : '';
        codeLocation = '$filename$line';
      }
    }
*/
    //print("new starobject[$typeString] : $codeLocation");
  }
}

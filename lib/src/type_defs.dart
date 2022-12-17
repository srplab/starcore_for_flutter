/*IsProcessed bool, Result Object*/
import 'package:starflut/src/star_object.dart';

typedef Future<List?> MsgCallBackProc(
  int serviceGroupID,
  int uMsg,
  Object wParam,
  Object lParam,
);

typedef Future<Object> StarObjectScriptProc(
  StarObjectClass cleObject,
  String frameTag,
  List? paras,
);

typedef void DispatchRequestProc();

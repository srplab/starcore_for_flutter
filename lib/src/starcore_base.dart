import 'package:starflut/src/starobject_record.dart';
import 'package:starflut/src/starflut.dart';

class StarCoreBase {
  String? starTag;
  String? starId; /*--valid for serviceclass or starobjectclass--*/
  String? m_frameTag;

  StarCoreBase(String? frameTag) {
    this.m_frameTag = frameTag;
  }

  void lock({String? frameTag = null}) {
    if (starId == null) return; // not
    if (frameTag != null &&
        Starflut.starObjectRecordFrame.containsKey(frameTag) == false) {
      throw 'Frame is [' + frameTag + '] is not found, call lock failed.';
      return;
    }
    if (m_frameTag == null) {
      if (frameTag == null) return;
    } else {
      if (frameTag != null && frameTag == m_frameTag) return;
    }
    StarObjectRecordClass? ll = null;
    if (m_frameTag == null) {
      /*--from global frame--*/
      for (int i = 0; i < Starflut.starObjectRecordListRoot.length; i++) {
        if (identical(Starflut.starObjectRecordListRoot[i]!.starObject, this)) {
          ll = Starflut.starObjectRecordListRoot[i];
          Starflut.starObjectRecordListRoot.removeAt(i);
          break;
        }
      }
    } else {
      List<StarObjectRecordClass?> starObjectRecordList =
          Starflut.starObjectRecordFrame[m_frameTag]!;
      for (int i = 0; i < starObjectRecordList.length; i++) {
        if (identical(starObjectRecordList[i]!.starObject, this)) {
          ll = starObjectRecordList[i];
          starObjectRecordList.removeAt(i);
          break;
        }
      }
    }
    m_frameTag = frameTag;
    if (ll == null) return;
    if (m_frameTag == null) {
      Starflut.starObjectRecordListRoot.add(ll);
    } else {
      List<StarObjectRecordClass?> starObjectRecordList =
          Starflut.starObjectRecordFrame[m_frameTag]!;
      starObjectRecordList.add(ll);
    }
    return;
  }

  void moveTo(String In_frameTag) {
    lock(frameTag: In_frameTag);
  }

  void free() {
    if (m_frameTag == null) {
      /*--from global frame--*/
      for (int i = 0; i < Starflut.starObjectRecordListRoot.length; i++) {
        if (identical(Starflut.starObjectRecordListRoot[i]!.starObject, this)) {
          Starflut.starObjectRecordListRoot.removeAt(i);
          return;
        }
      }
      return; // not
    }
    if (Starflut.starObjectRecordFrame.containsKey(m_frameTag) == false) {
      return;
    }
    List<StarObjectRecordClass?> starObjectRecordList =
        Starflut.starObjectRecordFrame[m_frameTag]!;
    m_frameTag = null;
    for (int i = 0; i < starObjectRecordList.length; i++) {
      if (identical(starObjectRecordList[i]!.starObject, this)) {
        starObjectRecordList.removeAt(i);
        return;
      }
    }
    return;
  }

  bool isEqual(StarCoreBase des) {
    return false;
  }
}

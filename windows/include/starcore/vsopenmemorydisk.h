#ifndef _VSMEMORYDISK
#define _VSMEMORYDISK

#include "vsopenapi.h"

#define VSMEMORYDISKFILE_HEADTAG "VSEXECUTEFILE1"

struct StructOfVSMemoryDiskFileHeader{
    /*---FileHeader 128Bytes  fixed*/
    VS_CHAR Tag[16];
    VS_CHAR ServiceName[64];
    VS_UUID  FileHeaderUUID;         /*---if does not match with server, need not download*/
    VS_ULONG UnCompressLength;
    VS_ULONG CompressLength;
    VS_UINT32 SupportOsType;
    VS_UINT8 CoreVersion;   /*--0 for randxor   1 for randxor1*/    
    VS_UINT8 Reserved0[3];
    VS_CHAR ScriptInterface[16];
    VS_UINT8 Reserved[64];
    /*VS_CHAR StartFileName[256];       0 is end*/
};
/*Depend Service Name List : ServiceName1,0,DownLoadType1,...,0*/
/*0 is end of Header part*/
/*ex: DownLoadType1 == 0 default download from http://www.srplab.com, ==1 include in the file, ==2 download from current website*/

/*--------------------------------------------------------*/
/*FileHeader*/
/*0 is end*/
/*extend data size*/
/*extend data*/
/*DependFileFullName,UnCompressedSize,CompressedSize,Offset,FileMD5*/
/*0 is end*/
/*ServiceFileFullName,UnCompressedSize,CompressedSize,Offset,FileMD5*/
/*0 is end*/
/*DataFileFullName,UnCompressedSize,CompressedSize,Offset,FileMD5*/
/*0 is end*/

/*file path and name, UnCompressedSize,CompressedSize,FileMD5*/
/*0 is end*/
/*extend data size*/
/*extend data*/

/*file begin,FileStartOffset*/
/*....reserved*/
/*file1 data, -----FileStartOffset*/
/*file2 data, -----FileStartOffset + Offset*/
/*....*/

#endif

#ifndef VSOPENCOMMTYPE
#define VSOPENCOMMTYPE

#include "vsopendatatype.h"

#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#pragma warning (disable:4819)
#pragma warning (disable:4244)
#pragma warning (disable:4996)
#pragma warning (disable:4800)
#endif

/*-----sync status*/
#define VSSYNCSTATUS_NOTSYNC 0    /*  not sync */
#define VSSYNCSTATUS_SYNC    1    /*  sync */
#define VSSYNCSTATUS_INSYNC  2    /*  being sync */

/*-----remotecall result  */
#define VSRCALL_OK             0   /*---success */
#define VSRCALL_COMMERROR      ((VS_ULONG)0xFFFFFFFF)
#define VSRCALL_OBJNOTEXIST    ((VS_ULONG)0xFFFFFFFE)
#define VSRCALL_FUNCNOTEXIST   ((VS_ULONG)0xFFFFFFFD)
#define VSRCALL_PARAERROR      ((VS_ULONG)0xFFFFFFFC)
#define VSRCALL_SYSERROR       ((VS_ULONG)0xFFFFFFFB)
#define VSRCALL_INVALIDUSR     ((VS_ULONG)0xFFFFFFFA)
#define VSRCALL_OVERTIME       ((VS_ULONG)0xFFFFFFF9)
#define VSRCALL_UNKNOWN        ((VS_ULONG)0xFFFFFFF8)   /*---other error */

/*-----remotecall source tag */
#define VSRCALLSRC_C           0   /*---remotecall from c/c++   */
#define VSRCALLSRC_SCRIPT      1   /*---remtotecall from script   */
#define VSRCALLSRC_WEBSERVICE  2   /*---remotecall from WebServie  */

/*-----Lua variable type  */
#define VSLUATYPE_NIL          0
#define VSLUATYPE_NUMBER       1
#define VSLUATYPE_BOOL         2
#define VSLUATYPE_STRING       3
#define VSLUATYPE_FUNCTION     4
#define VSLUATYPE_TABLE        5
#define VSLUATYPE_OBJECT       6
#define VSLUATYPE_PARAPKG      7
#define VSLUATYPE_QUERYRECORD  8
#define VSLUATYPE_TIME         9
#define VSLUATYPE_FONT         10
#define VSLUATYPE_RECT         11
#define VSLUATYPE_BINBUF       12
#define VSLUATYPE_SXML         13
#define VSLUATYPE_FUNCTIONPARA 14
#define VSLUATYPE_COMMINTERFACE 15
#define VSLUATYPE_INT          16  /* is integer, always be VSLUATYPE_NUMBER */
#define VSLUATYPE_USERDATA      17      /*only valid for lua  */
#define VSLUATYPE_LIGHTUSERDATA 18      /*only valid for lua  */
#define VSLUATYPE_INT64         19      /*add v2.1.1*/
#define VSLUATYPE_UWORD         20      /*add v2.1.1, 4 bytes on 32bit os, and 8bytes on 64bit os*/
#define VSLUATYPE_MALLOC        126     /*add v3.5.0, used in VS_RefCountChangeCallBackProc*/
#define VSLUATYPE_AVLTREE       127     /*add v3.5.0, used in VS_RefCountChangeCallBackProc*/
#define VSLUATYPE_UNKNOWN      255

/*-----sysevent process flag */
#define VSSYSEVENT_PROCESS_TICKET             0x0001  /*---10ms Ticket event       */
#define VSSYSEVENT_PROCESS_FRAMETICKET        0x0002  /*---server frame pulse event  */
#define VSSYSEVENT_PROCESS_IDLE               0x0004  /*---main program idle event   */
#define VSSYSEVENT_PROCESS_APPACTIVE          0x0008
#define VSSYSEVENT_PROCESS_APPDEACTIVE        0x0010
#define VSSYSEVENT_PROCESS_SERVICEACTIVE      0x0020
#define VSSYSEVENT_PROCESS_SERVICEDEACTIVE    0x0040
#define VSSYSEVENT_PROCESS_SELFEVENT          0x0080
#define VSSYSEVENT_PROCESS_ACTIVESET          0x0100  /*---activeset change  */

/*-----child object event process flag*/
#define VSSYSEVENT_PROCESS_CREATE             0x0001
#define VSSYSEVENT_PROCESS_DESTROY            0x0002
#define VSSYSEVENT_PROCESS_ACTIVATE           0x0004
#define VSSYSEVENT_PROCESS_DEACTIVATE         0x0008
#define VSSYSEVENT_PROCESS_SYNCGROUPCHANGE    0x0010

#define VSSYSEVENT_PROCESS_ANYCREATE          0x0100    /*--include child objects */
#define VSSYSEVENT_PROCESS_ANYDESTROY         0x0200
#define VSSYSEVENT_PROCESS_ANYACTIVATE        0x0400
#define VSSYSEVENT_PROCESS_ANYDEACTIVATE      0x0800
#define VSSYSEVENT_PROCESS_ANYSYNCGROUPCHANGE 0x1000

/*------------text display format type */
#define TEXTDISPLAY_CLASSID_NORMALTEXT     0
#define TEXTDISPLAY_CLASSID_EXPLANE        1  /* Annotation */
#define TEXTDISPLAY_CLASSID_OBJECTNAME     2
#define TEXTDISPLAY_CLASSID_ATTRIBUTETYPE  3
#define TEXTDISPLAY_CLASSID_NUMBER         4
#define TEXTDISPLAY_CLASSID_ERRORORWARN    5

#define TEXTDISPLAY_FORMAT "\\Fmt"  /*--end with space,such as "\\Fmt1 ljsdflksdf"   */

#define VS_DEFAULT_SERVICEGROUPID          0    /*--default servicegroup ID */
#define VS_INVALID_SERVICEGROUPID          0xFFFFFFFF

/*=========name string length */
#define DEFAULT_NAMELENGTH            40
#define SCRIPTINTERFACE_LENGTH        16

/*=========define ProgramTypeID */
/*--basic type */
#define VS_SERVER       ((VS_UINT16)0x0000)
#define VS_CLIENT       ((VS_UINT16)0x0001)
#define VS_DEBUG        ((VS_UINT16)0x0004)
#define VS_TOOLS        ((VS_UINT16)0x0008)
#define VS_CORE         ((VS_UINT16)0x0080)  /*use by starcore to get share file path */
/*--extend type */
#define VS_SERVER_SERVER      (((VS_UINT16)0x0000) | VS_SERVER)   /*--server size         */
#define VS_SERVER_USER        (((VS_UINT16)0x0100) | VS_SERVER)   /*--single server side  */
#define VS_CLIENT_USER        (((VS_UINT16)0x0000) | VS_CLIENT)   /*--client side         */
#define VS_CLIENT_COOPERATOR  (((VS_UINT16)0x0100) | VS_CLIENT)   /*--cooperator client   */
#define VS_CLIENT_CALLER      (((VS_UINT16)0x0200) | VS_CLIENT)   /*--caller client       */

#define VS_BASIC_PROGRAMTYPE(X)  (X & 0x00FF)

/*=========module type */
#define VSMODULE_ALL               ((VS_UINT16)0)   /*--valid at client,debug,server    */
#define VSMODULE_SERVER_SERVER     ((VS_UINT16)1)   /*--valid at server                 */
#define VSMODULE_SERVER_USER       ((VS_UINT16)2)   /*--valid at server user            */
#define VSMODULE_CLIENT_USER       ((VS_UINT16)4)   /*--valid at client user            */
#define VSMODULE_CLIENT_COOPERATOR ((VS_UINT16)8)   /*--valid at client cooperator      */
#define VSMODULE_CLIENT_CALLER     ((VS_UINT16)16)  /*--valid at server client end      */
#define VSMODULE_DEBUG             ((VS_UINT16)32)  /*--valid at debug                  */

#define VS_ISMODULEEXIST(Type,X)  ( (X==0) || \
                                    ((Type==VS_SERVER || Type==VS_SERVER_SERVER) && (X&VSMODULE_SERVER_SERVER)!=0 ) || \
                                    ((Type==VS_CLIENT || Type==VS_CLIENT_USER) && (X&VSMODULE_CLIENT_USER)!=0 ) || \
                                    (Type==VS_DEBUG && (X&VSMODULE_DEBUG)!=0 ) || \
                                    (Type==VS_SERVER_USER && (X&VSMODULE_SERVER_USER)!=0 ) || \
                                    (Type==VS_CLIENT_COOPERATOR && (X&VSMODULE_CLIENT_COOPERATOR)!=0 ) || \
                                    (Type==VS_CLIENT_CALLER && (X&VSMODULE_CLIENT_CALLER)!=0 ) )
#define VS_ISMODULENOTEXIST(Type,X) (!(VS_ISMODULEEXIST(Type,X)))

/*=========define os type, total kinds is 32 */
typedef struct{
    VS_UINT32 OsType;
    VS_UINT16 ProgramRunType;
    VS_UINT16 Reserved;
}VS_SERVICEOSRUNINFO;

/*=========client operating globals permissions,ClientOPPermission,  */
#define VSCLIENTOP_CREATE         ((VS_ULONG)0x00000001)
#define VSCLIENTOP_DELETE         ((VS_ULONG)0x00000002)    /*can delete object not belong to this client   */
#define VSCLIENTOP_CHANGE         ((VS_ULONG)0x00000004)    /*can change object not belong to this client   */

/*------define warning level */
#define VSFAULT_INDICATION        0x00  /* indication */
#define VSFAULT_WARNING           0x01  /* warning    */
#define VSFAULT_NORMALERROR       0x02  /* normal error, can continue execute, client should resync */
#define VSFAULT_CRITICALERROR     0x03  /* serious error,should exit current service               */
#define VSFAULT_SYSTEMERROR       0x04  /* serious error,should exit appplication                  */
#define VSFAULT_DISP              0x06  /* display single line info                                 */
#define VSFAULT_OPENSHOW          0x07  /* extern info,etc, extern calls pSRP->Print to print info  */

/*=========define object alloc type */
#define VSALLOCTYPE_STATIC         1
#define VSALLOCTYPE_GLOBAL         2
#define VSALLOCTYPE_CLIENT         3
#define VSALLOCTYPE_LOCAL          4

/*=========define object save flag */
#define VSSAVE_SAVE           0   /*---save, and reserve alloc type   */
#define VSSAVE_LOCAL          1   /*---save object as local object    */
#define VSSAVE_GLOBAL         2   /*---save object as global object   */
#define VSSAVE_NONE           3   /*---do not save, is default value  */

/*=========define object activate command */
#define VSACTIVE_ALONE        0   /*---through the command control activation    */
#define VSACTIVE_FOLLOW       1   /*---activate with the parent object, and will be automatically activated if its parent is serviceitem object */
/*--The following two active command is a dynamic, not to store, store all the converted VSACTIVE_ALONE, servers and clients effectively        */
#define VSACTIVE_ACTIVE       2   /*---If the service is running, object whose activate command becomes the value is automatically activated    */
#define VSACTIVE_DEACTIVE     3   /*---If the service is running, object whose activate command becomes the value is automatically deactivated  */

/*=========define object static data save flag */
#define VSSTATIC_SAVE           0   /*---save,valid for global static object                          */
#define VSSTATIC_CLIENTSAVE     1   /*---save at clienside, valid for global object and client object  */
#define VSSTATIC_NONE           2   /*---not save                                                      */

/*-------------------------------------------------------------- */
typedef VS_UINT8 OBJECTATTRIBUTEINDEX;     /* attribute index,scope[0-127],The definition of each object up to 127 properties */
#define INVALID_OBJECTATTRIBUTEINDEX ((OBJECTATTRIBUTEINDEX)0xFF)

typedef struct{
    VS_ULONG SysAttributeMap;     /*---property for internal use              */
    VS_ULONG AppAttributeMap[4];  /*---property defined by app module         */
}VS_ATTRIBUTEINDEXMAP;
#define VS_SETSYSATTRMAP(MapPtr,Attr) {MapPtr -> SysAttributeMap = MapPtr -> SysAttributeMap | ( 0x80000000 >> (Attr) );}
#define VS_SETAPPATTRMAP(MapPtr,Attr) {MapPtr -> AppAttributeMap[ ((Attr) >> 5) & 0x00000003 ] = MapPtr -> AppAttributeMap[ ((Attr) >> 5) & 0x00000003 ] | ( 0x80000000 >> ( (Attr) & 0x0000001F ) );}
#define VS_GETSYSATTRMAP(MapPtr,Attr) ( MapPtr -> SysAttributeMap & ( 0x80000000 >> (Attr) ) )
#define VS_GETAPPATTRMAP(MapPtr,Attr) ( MapPtr -> AppAttributeMap[ ((Attr) >> 5) & 0x00000003 ] & ( 0x80000000 >> ( (Attr) & 0x0000001F ) ) )

/*------------------------------------------------------------------------------*/
#define UUID_ISEQUAL(X,Y)   (((VS_ULONG *)&X)[0] == ((VS_ULONG *)&Y)[0] && ((VS_ULONG *)&X)[1] == ((VS_ULONG *)&Y)[1] && ((VS_ULONG *)&X)[2] == ((VS_ULONG *)&Y)[2] && ((VS_ULONG *)&X)[3] == ((VS_ULONG *)&Y)[3])
#define UUID_ISUNEQUAL(X,Y) (((VS_ULONG *)&X)[0] != ((VS_ULONG *)&Y)[0] || ((VS_ULONG *)&X)[1] != ((VS_ULONG *)&Y)[1] || ((VS_ULONG *)&X)[2] != ((VS_ULONG *)&Y)[2] || ((VS_ULONG *)&X)[3] != ((VS_ULONG *)&Y)[3])
#define INIT_UUID(X) {((VS_ULONG *)&X)[0] = 0; ((VS_ULONG *)&X)[1] = 0; ((VS_ULONG *)&X)[2] = 0; ((VS_ULONG *)&X)[3] = 0;}
#define UUID_ISINVALID(X)   (((VS_ULONG *)&X)[0] == 0 && ((VS_ULONG *)&X)[1] == 0 && ((VS_ULONG *)&X)[2] == 0 && ((VS_ULONG *)&X)[3] == 0)
#define UUID_ISVALID(X)     (((VS_ULONG *)&X)[0] != 0 || ((VS_ULONG *)&X)[1] != 0 || ((VS_ULONG *)&X)[2] != 0 || ((VS_ULONG *)&X)[3] != 0)

/*=========max number of activeset */
#define DEFAULT_ACTIVESET_NUMBER    256   /*--16 * 16 sync region   */
typedef struct{
    VS_INT32 ActiveSetNumber;                /*--If equal to 0, then the only group 0 sync */
    VS_ULONG ActiveSet[DEFAULT_ACTIVESET_NUMBER];  /* groupindex ==0 is sync by default    */
                                             /*--if [0] is equal to 0xFFFFFFFF,Indicated that all groups are synchronized [Note: The overall synchronization can only be set on the server side] */
}VS_ACTIVESETITEM;

/*==============================================================================*/
/*=========object syncgroup                                                     */
typedef VS_ULONG VS_SYNCGROUP;

/*--------------sequence pointer  */
struct SrtuctOfClassSkeleton_PointerSequence{  /*  Dynamic definition, does not require synchronization */
    VS_INT32 Number;
    VS_INT8 *Sequence[1];
};

/*============================================================================== */
/*=========define property for module depend */
#define VSMODULEDEPEND_OBJECT         0x00
#define VSMODULEDEPEND_FUNCRETURN     0x01
#define VSMODULEDEPEND_FUNCPARA       0x02
typedef struct{
    VS_INT32 Type;
    VS_INT32 Offset;
}VS_DEPENDATTRIBUTE;

/*------------------------------------------------------------------------------*/
/*---------------------define variable                                          */
/*------------------------------------------------------------------------------*/
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
typedef RECT VS_RECT;
typedef COLORREF VS_COLOR;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
typedef struct{
    VS_INT32 left,top,right,bottom;
}VS_RECT;
typedef VS_ULONG VS_COLOR;
#endif

typedef struct{
      VS_INT8 *Ptr;
      VS_INT8 *SequencePtr;
}VS_SEQUENCEPTR;

#define LUASTATE void *

#define SRP_ARGB_ABGR(X)  ( ( ( X & 0x000000FF ) << 16) | ( ( X & 0x00FF0000) >> 16 ) | ( X & 0xFF00FF00 ) )

#define VSFONT_BOLD      0x01
#define VSFONT_ITALIC    0x02
#define VSFONT_UNDERLINE 0x04
#define VSFONT_STRIKEOUT 0x08
#define VSFONT_NAMELENGTH 32

typedef struct StructOfLocalVSFont{
      VS_COLOR Color;
      VS_INT32 Height,Size;
      VS_UINT8 CharSet;
      VS_UINT8 Style;
      VS_UINT8 Position;
      VS_INT8 Reserve;
      VS_INT32 Pitch;
      VS_CHAR Name[VSFONT_NAMELENGTH];
}VS_FONT;

typedef struct{
	VS_INT8 Reserved[128];
}VS_QUERYRECORD;

typedef VS_TIME_T VS_TIME;

typedef struct StructOfLocalVString{     /*---Variable-length strings */
    VS_CHAR *Buf;    /*--The actual length of the buffer Length + 1 (including the last 0), if the application directly set, you must use the interface pSRP -> Malloc*/
}VS_VSTRING;

typedef VS_UUID VS_STATICID;
typedef void * VS_OBJPTR;

#if defined(__cplusplus) || defined(c_plusplus)
typedef class ClassOfSRPParaPackageInterface * VS_PARAPKGPTR;
typedef class ClassOfSRPBinBufInterface * VS_BINBUFPTR;
#else
typedef void * VS_PARAPKGPTR;
typedef void * VS_BINBUFPTR;
#endif

#define VS_LUAREFNIL      (-1)     /* ---LUA_REFNIL*/

#ifdef LUA_VERSION_5_2
#define VSLUA_NOREF       (-2)
#define VSLUA_REFNIL      (-1)

#define VSLUAI_BITSINT	32

#if VSLUAI_BITSINT >= 32
#define VSLUAI_MAXSTACK		1000000
#else
#define VSLUAI_MAXSTACK		15000
#endif

/* reserve some space for error handling */
#define VSLUAI_FIRSTPSEUDOIDX	(-VSLUAI_MAXSTACK - 1000)
#define VSLUA_REGISTRYINDEX	VSLUAI_FIRSTPSEUDOIDX
#define VSLUA_MULTRET	(-1)
#endif

/*------------------------------------------------------------------------------*/
/*---vatiable type*/
#define VSTYPE_BOOL             1
#define VSTYPE_INT8             2
#define VSTYPE_UINT8            3
#define VSTYPE_INT16            4
#define VSTYPE_UINT16           5
#define VSTYPE_INT32            6
#define VSTYPE_UINT32           7
#define VSTYPE_FLOAT            8
#define VSTYPE_LONG             9
#define VSTYPE_ULONG            10
#define VSTYPE_LONGHEX          11
#define VSTYPE_ULONGHEX         12
#define VSTYPE_VSTRING          51
#define VSTYPE_PTR              14
#define VSTYPE_MEMORY           15
#define VSTYPE_STRUCT           16
#define VSTYPE_COLOR            19
#define VSTYPE_RECT             20
#define VSTYPE_FONT             21
#define VSTYPE_TIME             49
#define VSTYPE_CHAR             13
#define VSTYPE_UUID             41
#define VSTYPE_STATICID         29
#define VSTYPE_CHARPTR          30
#define VSTYPE_PARAPKGPTR       40
/*--Can be used for function parameters*/
#define VSTYPE_INT8PTR          55
#define VSTYPE_UINT8PTR         54
#define VSTYPE_INT16PTR         31
#define VSTYPE_UINT16PTR        52
#define VSTYPE_INT32PTR         32
#define VSTYPE_UINT32PTR        53
#define VSTYPE_FLOATPTR         33
#define VSTYPE_ULONGPTR         48
#define VSTYPE_LONGPTR          34
#define VSTYPE_STRUCTPTR        35
#define VSTYPE_COLORPTR         37
#define VSTYPE_RECTPTR          38
#define VSTYPE_FONTPTR          39
#define VSTYPE_TIMEPTR          50
#define VSTYPE_UUIDPTR          47
#define VSTYPE_BINBUFPTR        59

#define VSTYPE_VOID             254   /*---does not exist within the Platform*/
#define VSTYPE_OBJPTR           57
#define VSTYPE_TABLE            56    /*---only used for remotecall*/

#define VSTYPE_DOUBLE           58
#define VSTYPE_INT64            60
#define VSTYPE_UWORD            61

#define VSTYPE_DOUBLEPTR        63
#define VSTYPE_INT64PTR         62

#define VSTYPE_UWORDPTR         64

#define VSTYPE_IGNORE           255   /*---  does not exist within the Platform*/

/*---variable edit*/
#define VSEDIT_EDIT             0x00
#define VSEDIT_COMBOBOX         0x01
#define VSEDIT_CHECKBOX         0x02
#define VSEDIT_HIDE             0x03  /*---can not edit and display*/
#define VSEDIT_MASK             0x04

typedef struct{
    VS_INT32 NumberOfContent;
    VS_CHAR Content[256][50];
    VS_LONG ComboBoxToValueIndex[256];   /* Subscript is the ComboBox index, value is the content*/
}VS_COMBOBOXITEM;

typedef struct{
    VS_CHAR  Name[DEFAULT_NAMELENGTH];
    VS_CHAR  Caption[DEFAULT_NAMELENGTH];
    VS_CHAR  DefaultString[DEFAULT_NAMELENGTH];  /*  only support string*/
    VS_UINT8 Type;    /*--Property type*/
    VS_UINT8  EditType;  /* the variable Edit type, the general variable, combo boxes, buttons*/
    VS_UINT8  EditReadOnly; /* ==0 can edit == 1 readonly*/
    VS_BOOL  SyncType;         /*--0 local property ==1 Global synchronization property*/
    VS_UINT8 CreateNeedFlag;   /*--1 create needed   0 not needed*/
    VS_UINT8 ChangeNotifyFlag; /*--0 Change without notice, 1 notice ,2 notice before the change*/
    VS_INT32 Length;            /*--length of the property*/
    VS_INT32 Offset;            /*--Offset from the object, the definition does not include the variables within the platform*/
    VS_UINT8 ComboBoxID[20];  /*---combobox ID*/
    VS_UUID  StructID;        /*---struct ID,valid When the type is structure, or pointer*/
    VS_ULONG StaticID;    /*--static data ID,meaningful when the the property type is static data*/
    VS_UINT8 AtomicAttributeIndex; /*---The global property index, including the platform within the defined properties*/
    VS_UINT8 AttributeIndex;       /*---Properties of local index, not including the platform, the properties within the defined*/
    VS_UINT8 Reserved[2];
    void     *AtomicAttributeObject;  /*---Attribute atomic Object*/
}VS_ATTRIBUTEINFO;

typedef struct{
    VS_CHAR  Name[DEFAULT_NAMELENGTH];
    VS_UINT8 CanNotBeOVLFlag;               /*  This function can overload*/
    VS_BOOL LuaFunctionFlag;                /*  the function is whether Lua function*/
    VS_BOOL CallBackFlag;                   /*  the function is whether call back function*/
    VS_UUID FunctionID;            /*  function id*/
    VS_UUID OriginFunctionID;      /*  if is overload function,then it is origin functionid*/
    void *CallFunction;                     /*  function address*/
}VS_FUNCTIONINFO;

typedef struct StructOfLocalOutEventInfo{
    VS_CHAR  Name[DEFAULT_NAMELENGTH];
    VS_BOOL  DynamicFlag;                /* --true dynamic event,--false static event*/
    VS_UUID EventID;           /*  event id*/
}VS_OUTEVENTINFO;

/*------------------------------------------------------------------------------*/
/*---------------------client Qos,On the server side can be different for each client*/
/*------------------------------------------------------------------------------*/
#define VSCLIENTQOS_SERVICECLASS_NORMAL     0   /*---General Service class*/
#define VSCLIENTQOS_SERVICECLASS_PRIORITY   1   /*---Priority Service class*/

#define VSCLIENTQOS_LOADRATE_MIN  1024     /*---Minimum*/
#define VSCLIENTQOS_LOADRATE_MAX  100*1024    /*---Max*/

typedef struct{
    VS_ULONG ServiceClass;  /*---service class*/
    VS_ULONG UpLoadRatePerFrame;    /*---client upload rate,  Bytes/Frame*/
    VS_ULONG DownLoadRatePerFrame;  /*---client download rate,  Bytes/Frame*/
    VS_ULONG Reserved[5];
}VS_CLIENTQOS;

/*------------------------------------------------------------------------------*/
/*---------------------client to create, modify and delete the object callback function, valid at the server*/
/*------------------------------------------------------------------------------*/
#define VSCLIENTOP_DELOBJECT     0
#define VSCLIENTOP_CHANGEOBJECT  1
#define VSCLIENTOP_CREATEOBJECT  2
#define VSCLIENTOP_CREATEITEMOBJECT  3
typedef VS_BOOL (SRPAPI *VS_ClientOperationCallBackProc)( VS_UWORD Para,VS_ULONG uMsg,VS_ULONG ClientID,VS_ULONG ClientPrivateTag,void *Object,OBJECTATTRIBUTEINDEX ParentIndex,VS_UUID *ClassID);
/*---ClientPrivateTag Used to determine the legality of the client*/
/*---for delete uMsg = VSCLIENTOP_DELOBJECT; Object is valid*/
/*---for modiify uMsg = VSCLIENTOP_CHANGEOBJECT; Object is valid*/
/*---for create uMsg = VSCLIENTOP_CREATEOBJECT; Object is parent object,ParentIndex is queue of parentobject,ClassID is class*/
/*---for create uMsg = VSCLIENTOP_CREATEITEMOBJECT; Object is serviceitem,ParentIndex is queue of parentobject,ClassID is class*/
/*---If it returns false, is not able to carry out such operations*/

/*------------------------------------------------------------------------------*/
/*---------------------client infomation*/
/*------------------------------------------------------------------------------*/
typedef struct{
    VS_ULONG ClientMachineID;
	VS_ULONG Reserve1;
    VSSOCKADDR_IN ClientSockAddr;  /*---client addreee*/
    VS_ULONG ClientModuleID;        /*---client program id*/
    VS_ULONG ClientServiceGroupID;  /*---client servicegroup*/
    VS_INT32   DirectConnectFlag;     /*--- ==1 direct connect, ==0 through SRPDispatch*/
	VS_INT32 Reserve2;
    VSSOCKADDR_IN SRPDispatchSockAddr;  /*---SRPDispatch address*/
    VS_ULONG SRPDispatchModuleID;        /*---SRPDispatch programID*/
    VS_ULONG SRPDispatchServiceGroupID;  /*---SRPDispatch service group*/
    VS_SERVICEOSRUNINFO OsRunInfo;
    VS_INT8  Reserved[32];
}VS_CLIENTINFO;

/*------------------------------------------------------------------------------*/
/*---------------------service info*/
/*------------------------------------------------------------------------------*/

typedef struct{
    VS_ULONG AttributeNumber;  /*---attribute number*/
    VS_ULONG FunctionNumber;   /*---function number*/
    VS_ULONG InputEventNumber; /*---input event number*/
    VS_ULONG OutputEventNumber;/*---output event number*/
    VS_ULONG NameScriptNumber; /*---name script number*/
    VS_ULONG ObjectNumber;     /*---object number*/
    VS_ULONG Reserved[16];
}VS_SERVICEINFO;

/*------------------------------------------------------------------------------*/
/*---------------------client connection callback*/
/*------------------------------------------------------------------------------*/
#define VSCLIENTCONNECT_ONCONNECT      0    /*---After the incident, said the connection has been successfully established. Initialization message for the service will follow*/
#define VSCLIENTCONNECT_ONFAILURE      1    /*---If ConnectionID equal to 0, then the request is released, no longer continue to generate the callback, otherwise, would be to try*/
#define VSCLIENTCONNECT_ONINITFAILUER  2    /*---Client service failed to initialize the connection is not successfully established, the call DisConnect, will also tirigger the callback*/
#define VSCLIENTCONNECT_ONINITSUCCESS  3    /*---Client Service initialized successfully*/
#define VSCLIENTCONNECT_ONSERVICESYNC  4    /*---Complete synchronization client service*/
#define VSCLIENTCONNECT_ONDISCONNECT   5    /*---Termination of client connections*/

typedef void (SRPAPI *VS_ClientConnectCallBackProc)( VS_ULONG ServiceGroupID, VS_ULONG uMsg,VS_ULONG ConnectionID, VS_ULONG LinkInterfaceStatus, VS_CHAR *ServerName,VS_UINT16 ServerPortNumber, VS_UWORD Para);

/*------------------------------------------------------------------------------*/
/*---------------------client redirect callback                                 */
/*------------------------------------------------------------------------------*/
#define VSREDIRECT_ONCONNECT      0        /*---Connect to the target, will not generate a callback*/
#define VSREDIRECT_ONFAILURE      1        /*---After the message will no longer have callback*/

typedef void (SRPAPI *VS_RedirectCallBackProc)( VS_ULONG uMsg,VS_ULONG ClientID,VS_CHAR *DesServerName,VS_UINT16 DesServerPortNumber, VS_UWORD Para);

/*------------------------------------------------------------------------------*/
/*---------------------file upload or download                                  */
/*------------------------------------------------------------------------------*/
typedef struct{
    /*------------upload*/
    VS_INT32 UpDataFile;  /* --0 static data  -- 1 file*/
	VS_INT32 Reserve1;
    union{
        struct{
            VS_UUID ObjectID;                      /*---If invalid, then the object or service does not exist*/
            VS_ULONG UniqueDataUnitID;
			VS_ULONG Reserve2;
            VS_STATICID Version;  /*--- Valid only for static data*/
        }StaticData;
        struct{
            VS_CHAR FileName[256];  /*---filename     Equal to "" that is not valid*/
        }FileData;
    }Up;
    VS_INT32  UpLoadFileSize;
    VS_INT32  UpLoadTransferSize;
    /*-------------download*/
    VS_INT32 DownDataFile;  /* --0 static data  -- 1 file*/
	VS_INT32 Reserve3;
    union{
        struct{
            VS_UUID ObjectID;                      /*---If invalid, then the object or service does not exist*/
            VS_ULONG UniqueDataUnitID;
			VS_ULONG Reserve4;
            VS_STATICID Version;  /*--- Valid only for static data*/
        }StaticData;
        struct{
            VS_CHAR FileName[256];  /*---filename     Equal to "" that is not valid*/
        }FileData;
    }Down;
    VS_INT32  DownLoadFileSize;
    VS_INT32  DownLoadTransferSize;
}VS_UPDOWNFILEINFO;

/*------------------------------------------------------------------------------*/
/*---Callback notification message*/
#define VSFILE_ONDOWNSTART      0    /*---begin start download*/
#define VSFILE_ONDOWNPROGRESS   1    /*---download process*/
#define VSFILE_ONDOWNFINISH     2    /*---download finish*/
#define VSFILE_ONDOWNERROR      3    /*---download error*/
#define VSFILE_ONUPSTART        4    /*---begin start upload*/
#define VSFILE_ONUPPROGRESS     5    /*---upload process*/
#define VSFILE_ONUPFINISH       6    /*---upload finish*/
#define VSFILE_ONUPERROR        7    /*---upload error*/

#define VSFILE_STATUSIDLE       0    /*---not upload or download*/
#define VSFILE_STATUSUP         1    /*---download*/
#define VSFILE_STATUSDOWN      -1    /*---upload*/

typedef struct{
    VS_INT32 DataFile;  /* --0 static data  -- 1 file*/
	VS_INT32 Reserve1;
    union{
        struct{
            VS_UUID ObjectID;                      /*---If invalid, then the object or service does not exist*/
            VS_ULONG UniqueDataUnitID;
			VS_ULONG Reserve2;
            VS_STATICID Version;  /*--- Valid only for static data*/
            VS_UINT8 *DataBuf;      /*---set only when VSFILE_ONDOWNPROGRESS,VSFILE_ONDOWNFINISH,VSFILE_ONUPPROGRESS*/
        }StaticData;
        struct{
            VS_UUID ObjectID;                      /*---If invalid, then the object or service does not exist*/
            VS_CHAR FileName[256];  /*---filename*/
            VS_UINT8 *FileBuf;      /*---set only when VSFILE_ONDOWNPROGRESS,VSFILE_ONDOWNFINISH,VSFILE_ONUPPROGRESS*/
        }FileData;
    }u;
    VS_INT32  DataSize;       /*---file size*/

    VS_INT32  ReceiveOrSendOffset;    /*---File size has been received, or has been uploaded*/
}VS_UPDOWNFILEMSG;

#define VSFILE_RET_OK      0    /*---continue*/
#define VSFILE_RET_ABORT   1    /*---Cancel the file download, the return value is only meaningful in the VSFILE_ONDOWNPROGRESS process, and for single file*/

typedef VS_ULONG (SRPAPI *VS_FileUpDownLoadCallBackProc)( void *Object, VS_UWORD Para, VS_ULONG uMsg, VS_UPDOWNFILEMSG *Msg);
/*---used at server, support VSFILE_ONDOWNSTART and VSFILE_ONUPSTART*/
/*   if return == VS_FALSE the process will be cancel by server*/
typedef VS_BOOL (SRPAPI *VS_FileUpDownLoadRequestCallBackProc)( VS_ULONG ClientID, VS_ULONG ClientPrivateTag, VS_UWORD Para, VS_ULONG uMsg, VS_UPDOWNFILEMSG *Msg);

/*------------------------------------------------------------------------------*/
/*---------------------The structure of the object module information           */
/*------------------------------------------------------------------------------*/
typedef struct{
    /*--------object version*/
    VS_UINT8 ObjectVersion;
    VS_UINT8 ObjectSubVersion;
    VS_UINT16 ObjcetBugFixVersion;
    /*-------Supported platform versions*/
    VS_UINT16 VSVersion;
    VS_UINT16 VSSubVersion;
    VS_TIME CreateTime;
    VS_TIME UpdateTime;
    VS_CHAR AuthorityInfo[128];
    VS_CHAR LicenseInfo[256];
    VS_CHAR ExtendInfo[256];     /*---Reservations, to be extended*/
}VS_OBJECTMODULEINFO;

/*------------------------------------------------------------------------------*/
/*---------------------define event                                             */
/*------------------------------------------------------------------------------*/
#if defined(VSPLAT_64)
#define VSMODULE_UNDEFINEPROC (void *)0xFFFFFFFFFFFFFFFF
#else
#define VSMODULE_UNDEFINEPROC (void *)0xFFFFFFFF
#endif
#define VSEVENTTRIGGER_NORMAL        0x01           /*general event*/
#define VSEVENTMANAGER_ATTACHBUFSIZE 64
/*----Returns the result of events */
#define VSEVENTMANAGER_STOP     1    /* Stop the distribution of events, some events during follow-up will not be triggered*/
#define VSEVENTMANAGER_DISPATCH 0    /* Continue to trigger events*/

/*----Event handler*/
typedef VS_INT32 (SRPAPI *VSSystemEvent_EventProc)(VS_UWORD FunctionChoice,void *EventPara);
typedef void (SRPAPI *VSSystemEvent_ParaFreeProc)(void *EventRunParam);

/*---------------*/
typedef struct StructOfVSEventParamRunParam{
    VS_UINT64 LParam;
    VS_UINT64 SParam;
    VS_UINT64 TParam;
    VS_UINT64 FParam;
    VS_UINT64 WParam;
    VS_UINT64 Param6;
    VS_UINT64 Param7;
    VS_UINT64 Param8;
    VS_UINT64 Param9;
    VS_UINT64 Param10;
    VS_INT8 *AttachBuf;         /*  External trigger event processing set, and release automatically by the platform*/
#if defined(__cplusplus) || defined(c_plusplus)
    class ClassOfSRPParaPackageInterface *ParaPkg;  /*if not NULL, it will be released by platform*/
#else
    void *ParaPkg;
#endif
    VSSystemEvent_ParaFreeProc ParaFreeProc;  /* Events released, the release of the function is called to free outside of the parameters*/
    void *Context;                  /*---The caller to set*/
    struct StructOfVSEventParamRunParam *Up,*Down;
}VS_EVENTPARAM_RUNPARAM;

#define INITVS_EVENTPARAM_RUNPARAM(X) { \
                                          X -> LParam = 0; \
                                          X -> SParam = 0; \
                                          X -> TParam = 0; \
                                          X -> FParam = 0; \
                                          X -> WParam = 0; \
                                          X -> Param6 = 0; \
                                          X -> Param7 = 0; \
                                          X -> Param8 = 0; \
                                          X -> Param9 = 0; \
                                          X -> Param10 = 0; \
                                          X -> ParaPkg = NULL; \
                                          X -> AttachBuf = NULL; \
                                          X -> ParaFreeProc = NULL; \
                                          X -> Context = NULL; \
                                          X -> Up = NULL; \
                                          X -> Down = NULL; \
                                          }

#define INITVS_EVENTPARAM_COPYPARAM(X,Y) { \
                                          X.LParam = Y.LParam;\
                                          X.SParam = Y.SParam;\
                                          X.TParam = Y.TParam;\
                                          X.FParam = Y.FParam;\
                                          X.WParam = Y.WParam;\
                                          X.Param6 = Y.Param6;\
                                          X.Param7 = Y.Param7;\
                                          X.Param8 = Y.Param8;\
                                          X.Param9 = Y.Param9;\
                                          X.Param10 = Y.Param10;\
                                          X.AttachBuf = Y.AttachBuf; \
                                          X.ParaPkg = Y.ParaPkg; \
                                          X.ParaFreeProc = Y.ParaFreeProc; \
                                          X.Context = Y.Context; \
                                          }

/*------------------------------------------------------------------------------*/
typedef struct{
      void *SrcObject;
      void *DesObject;              
      VS_BOOL ThisObject;                /* Object handling the event, consistent with the purpose of the object, was true, otherwise false  */
      VS_EVENTPARAM_RUNPARAM *RequestParam;      /* request parameter*/
      VS_UUID EventID;
      VS_INT8 Reserver[112];             /* reserved*/
}VS_EVENTPARAM;

/*------------------------------------------------------------------------------*/
/*---old version functions,*/
void SRPCreateUserInfoInit();
VS_INT8 *SRPCreateUserInfo(VS_CHAR *UserName,VS_INT8 *UserPassword,VS_CHAR *SRPUserName,VS_INT8 *SRPUserPassword);
void SRPCreateUserInfoTerm();

typedef VS_INT8 *(SRPAPI *VS_GetUserInfoCallBackProc)( VS_UUID ServiceID, VS_CHAR *ServiceName, VS_CHAR *UserNameBuf, VS_INT8 *UserPassBuf ); 

/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/*---Socket message*/
#define VS_LINKINTERFACESTATUS_OK         0x00000000
#define VS_LINKINTERFACESTATUS_DOWNLOAD   0x00000001
#define VS_LINKINTERFACESTATUS_ERROR      0x00000002

#define VS_SOCKET_ONACCEPT        0x00000001
#define VS_SOCKET_ONCONNECT       0x00000002            /*--Successfully establish a connection, Mes point structure,MesLength = 0*/
typedef struct{
 	VSSOCKADDR_IN SockAddr;      /*  peer ip address and port*/
}VS_SOCKETONCONNECT;

#define VS_SOCKET_ONDISCONNECT    0x00000003            /*--Close the connection, then no callback, Mes = NULL, MesLength = 0*/
#define VS_SOCKET_ONFAILCONNECT   0x00000004            /*--Connection fails, then no callback, Mes = NULL, MesLength = 0*/
#define VS_SOCKET_ONRECEIVE       0x00000005            /*--Received a packet of data, Mes point, Mes point ClassOfSRPParaPackageInterface, MesLength = 0*/
#define VS_SOCKET_ONRECEIVEBIN    0x00000006            /*--Received a packet of data, Mes point Buf,MesLength*/
#define VS_SOCKET_ONTIMER         0x00000007            /*--timer message, Mes = (VS_ULONG)TimerID,MesLength = 0*/

typedef void (SRPAPI *VS_SockEventCallBackProc)( VS_ULONG ServiceGroupID, void *Machine, VS_ULONG uMsg, VS_ULONG MachineID, VS_ULONG LinkInterfaceStatus,void *Mes, VS_INT32 MesLength, VS_UWORD Para  );
typedef void (SRPAPI *VS_SockAcceptCallBackProc)( VS_ULONG ServiceGroupID, VS_ULONG uMsg, VS_ULONG ConnectionID, VSSOCKADDR_IN SockAddr, VS_UWORD Para, VS_ULONG MachineID, VS_SockEventCallBackProc *ClientCallBack, VS_UWORD *ClientPara  );
/*---Accept the callback function in, uMsg value :VS_SOCKET_ONACCEPT,VS_SOCKET_ONDISCONNECT*/

/*------------------------------------------------------------------------------*/
/*---client login*/
#define VS_CLIENT_LOGIN            0x00000001
#define VS_CLIENT_LOADSERVICEOK    0x00000002
#define VS_CLIENT_LOGOUT           0x00000003      /*--Client connection is broken, the platform produced before processing the callback, the server can carry out some processing*/

#if defined(__cplusplus) || defined(c_plusplus)
typedef VS_BOOL (SRPAPI *VS_ClientMachineProcessProc)(void *Machine,void *Object,VS_UWORD Para,VS_ULONG uMes,VS_UUID *SrcServiceID, VS_ULONG SrcServiceAdd,VS_UINT16 SrcServicePort,class ClassOfSRPParaPackageInterface *ParaPkg,VS_CHAR *UserName,VS_CHAR *UserPassword);
#else
typedef VS_BOOL (SRPAPI *VS_ClientMachineProcessProc)(void *Machine,void *Object,VS_UWORD Para,VS_ULONG uMes,VS_UUID *SrcServiceID, VS_ULONG SrcServiceAdd,VS_UINT16 SrcServicePort,void *ParaPkg,VS_CHAR *UserName,VS_CHAR *UserPassword);
#endif
/*--Meaningful only for VS_CLIENT_LOGIN, return VS_TRUE, said the deal with the access request; return VS_FALSE, that did not deal with access requests*/

/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/*----Platform Communication Statistics*/
typedef struct{
    VS_INT32    ClientConnectionNumber;   /*---The number of client connections*/
    VS_INT32    DebugConnectionNumber;    /*---The number of debug connections*/
    VS_INT32    ServerConnectionNumber;   /*---The number of server connections*/
    VS_INT32    DataConnectionNumber;     /*---The number of data connections*/
    VS_INT32    RawSocketServerNumber;    /*---The number of RawSocket server*/
    VS_INT32    RawSocketClientNumber;    /*---The number of RawSocket client*/

    /*------The following are statistics based on state machine*/
    VS_INT32    ReceiveMsgItemNumber;     /*---The number of messages received*/
    VS_INT32    ReceiveMsgItemBytes;      /*---The number of bytes to receive messages*/
    VS_INT32    SendMsgItemNumber;        /*---The number of messages sent*/
    VS_INT32    SendMsgItemBytes;         /*---The number of bytes to send a message*/
    VS_INT32    SysSendQueueOccupyRate;      /*---Platform to send the buffer occupancy */
    VS_INT32    ObjSendQueueOccupyRate;      /*---Objects transmit buffer occupancy */
    VS_ULONG    PeerDelayTicket;           /*---To the delay on the side,(ms)*/
	VS_ULONG    Reserved;
}VS_STATISTICINFO;

/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/*---System classes and system objects*/
#define VSSYSOBJ_OBJNAME "VSSysObj"
#define VSSYSOBJ_WNDADJUSTNAME "OnWndAdjust"
#define VSSYSOBJ_WNDRESIZENAME "OnWndResize"
#define VSSYSOBJ_WNDCANBERESIZENAME "OnWndCanBeResize"
#define VSSYSOBJ_EDITSELECTNAME "OnEditSelect"
#define VSSYSOBJ_SETFOCUSNAME "OnSetFocus"
#define VSSYSOBJ_WNDMSGNAME "OnWinMsg"

extern VS_UUID VSSYSOBJ_OBJID;
/*+++event*/
extern VS_UUID VSSYSOBJ_WNDADJUST;           /*---Management request to adjust the window client area*/
/*--IN   none*/
/*--OUT  none*/
extern VS_UUID VSSYSOBJ_WNDRESIZE;           /*---Management of change in the size of the window*/
/*--IN   none*/
/*--OUT  none*/
extern VS_UUID VSSYSOBJ_WNDCANBERESIZE;      /*---Determine whether to allow management to change the size of the window*/
/*--IN   none*/
/*--OUT  EventPara.LParam = 0 allow ==1 not allow*/
extern VS_UUID VSSYSOBJ_EDITSELECT;
/*--IN  EventPara.LParam = SelectObject*/

extern VS_UUID VSSYSOBJ_SETFOCUS;            /*---Application to set the focus*/
/*+++event*/

extern VS_UUID VSSYSOBJ_WNDMSG;            /*---Windows message */
/*+++event  EventPara.LParam = uMes  EventPara.LParam = wParam  EventPara.LParam = lParam*/

/*---------*/
#define VSSYSDOC_ONTEXTCHANGENAME "OnTextChange"
#define VSSYSDOC_ONTEXTSELECTNAME "OnTextSelect"
#define VSSYSDOC_ONGETTEXTNAME "OnGetText"
#define VSSYSDOC_ONSETTEXTNAME "OnSetText"
#define VSSYSDOC_LUA_GETTEXTNAME "Lua_GetText"
#define VSSYSDOC_LUA_SETTEXTNAME "Lua_SetText"

extern VS_UUID VSSYSDOC_CLASSID;
extern VS_UUID VSSYSDOC_ONGETTEXT;
/*+++event  EventPara.LParam = class ClassOfSRPBinBufInterface *BinBuf*/
extern VS_UUID VSSYSDOC_ONSETTEXT;
/*+++event  EventPara.LParam = class ClassOfSRPBinBufInterface *BinBuf  EventPara.SParam = 0 combined  ==1 refresh*/
extern VS_UUID VSSYSDOC_LUA_GETTEXT;
extern VS_UUID VSSYSDOC_LUA_SETTEXT;
extern VS_UUID VSSYSDOC_ONTEXTCHANGE;
/*+++event*/
extern VS_UUID VSSYSDOC_ONTEXTSELECT;
/*+++event  EventPara.LParam = StartPos SParam = EndPos*/


#define VSSYSID_VSSYSOBJ_OBJID            0
#define VSSYSID_VSSYSOBJ_WNDADJUST        1
#define VSSYSID_VSSYSOBJ_WNDCANBERESIZE   2
#define VSSYSID_VSSYSOBJ_WNDRESIZE        3
#define VSSYSID_VSSYSOBJ_EDITSELECT       4
#define VSSYSID_VSSYSOBJ_SETFOCUS         5
#define VSSYSID_VSSYSOBJ_WNDMSG           6

#define VSSYSID_VSSYSDOC_CLASSID          7
#define VSSYSID_VSSYSDOC_ONGETTEXT        8
#define VSSYSID_VSSYSDOC_ONSETTEXT        9
#define VSSYSID_VSSYSDOC_LUA_GETTEXT      10
#define VSSYSID_VSSYSDOC_LUA_SETTEXT      11
#define VSSYSID_VSSYSDOC_ONTEXTCHANGE     12
#define VSSYSID_VSSYSDOC_ONTEXTSELECT     13

#define VS_SOURCESCRIPT_C        0
#define VS_SOURCESCRIPT_LUA      1
#define VS_SOURCESCRIPT_PYTHON   2
#define VS_SOURCESCRIPT_JAVA     3
#define VS_SOURCESCRIPT_CSHARP   4
#define VS_SOURCESCRIPT_PHP      5
#define VS_SOURCESCRIPT_RUBY     6

#endif



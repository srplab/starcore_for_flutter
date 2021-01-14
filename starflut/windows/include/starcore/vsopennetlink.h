#ifndef _VSOPENNETLINK
#define _VSOPENNETLINK

#include "vsopendatatype.h"

struct o_in_addr {
        union {
                struct { VS_UINT8 s_b1,s_b2,s_b3,s_b4; } S_un_b;
				struct { VS_UINT16 s_w1, s_w2; } S_un_w;
				VS_ULONG S_addr;
        } S_un;
};
typedef struct{
    VS_INT16            sin_family;
    VS_UINT16           sin_port;
    struct o_in_addr    sin_addr;
	VS_CHAR             sin_zero[8];
}O_SOCKADDR_IN;

#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10  )
#ifdef MYLIBAPI
#else
#if defined(__cplusplus) || defined(c_plusplus)
    #define MYLIBAPI extern "C" __declspec(dllexport)
#else
	#define MYLIBAPI extern __declspec(dllexport)
#endif
#endif
#else
    #define MYLIBAPI
#endif

#define NETCOMM_LINKLAYER_INTERFACENAME_LEN    128
#define NETCOMM_LINKLAYER_INTERFACEFTPSITE_LEN 512
#define NETCOMM_LINKLAYER_INTERFACE_LEN        512

#define NETCOMM_PAYLOADLENGTH 1024 + 12  /*  sizeof valid payload*/

#define NETCOMM_LINKLAYER_SOCKETID_MAX      0x000FFFFF

/*========error code of link layer, uses for send data====*/
#define NETCOMM_LINKLAYER_OK                0x00
#define NETCOMM_LINKLAYER_ERROR_LINKCLOSE   0x01
#define NETCOMM_LINKLAYER_ERROR_WOULDBLOCK  0x02    /*  use congestion control*/

#define NETCOMM_LINKLAYER_MESSAGESTART 0x0100

/*======define netlayer msg send from linklayer*/
#define NETCOMM_LINKLAYER_ADDLINK      NETCOMM_LINKLAYER_MESSAGESTART + 0x0001  /*  add link*/
#define NETCOMM_LINKLAYER_DELLINK      NETCOMM_LINKLAYER_MESSAGESTART + 0x0002  /*  remove link*/
#define NETCOMM_LINKLAYER_LINKFAIL     NETCOMM_LINKLAYER_MESSAGESTART + 0x0003  /*  link setup fail*/
#define NETCOMM_LINKLAYER_FRAME        NETCOMM_LINKLAYER_MESSAGESTART + 0x0004  /*  receive one frame*/

/*=====define response msg for uplayer*/
#define NETCOMM_LINKLAYER_SETUPSERVER_RESPONSE NETCOMM_LINKLAYER_MESSAGESTART + 0x0006
#define NETCOMM_LINKLAYER_SETUPCLIENT_RESPONSE NETCOMM_LINKLAYER_MESSAGESTART + 0x0007

/*---common msg struct*/
struct StructOfCommonMessage{
       VS_UINT16  MsgClass;
	   VS_UINT16  OperateCode;
       void       *PrivateBuf;           /*  Used By Kernel, Set to NULL*/
       VS_INT8  Buf[1];
};

/*-----struct*/
struct StructOfNetcomm_LinkLayerMessage_SetupServer_Response{
     VS_HANDLE InterfaceHandle;
	 VS_ULONG RequestID;
	 VS_ULONG SocketIDOfClient;  /*  invalid  == 0xFFFFFFFF*/
};

struct StructOfNetcomm_LinkLayerMessage_SetupClient_Response{
     VS_HANDLE InterfaceHandle;
	 VS_ULONG RequestID;
	 VS_ULONG SocketIDOfClient;  /*  invalid  == 0xFFFFFFFF*/
};

/*+++++*/
struct StructOfNetComm_LinkLayerMessage_AddLink{
     VS_HANDLE InterfaceHandle;
	 VS_ULONG         RequestID;
	 VS_ULONG         ServerSocketIDOfClient;  /* if invalid, is the reponse for request,or else is new connection received by listen*/
	 VS_ULONG         SocketIDOfClient;
	 O_SOCKADDR_IN    LocalSockAddr;          /*  local netlayer address*/
	 O_SOCKADDR_IN    PeerSockAddr;           /*  peer netlayer address*/
	 VS_ULONG         BandWidth;              /*  KB*/
};

struct StructOfNetComm_LinkLayerMessage_DelLink{
     VS_HANDLE InterfaceHandle;
	 VS_ULONG         SocketIDOfClient;
};

struct StructOfNetComm_LinkLayerMessage_LinkFail{
     VS_HANDLE InterfaceHandle;
	 VS_ULONG         RequestID;
};

struct StructOfNetComm_LinkLayerMessage_Frame{
     VS_HANDLE InterfaceHandle;
	 VS_ULONG         SocketIDOfClient;
	 VS_INT32           FrameBufSize;  /*  discard sizeof frame header*/
	 VS_INT8          *FrameBuf;
};

/*------------------------------------------------------------------------------*/
typedef VS_INT8 *(SRPAPI *VSNetComm_GetControlMsgBufProc)(VS_HANDLE MsgHandle);
typedef VS_INT8 *(SRPAPI *VSNetComm_GetDataMsgBufProc)(VS_HANDLE MsgHandle);
typedef VS_INT32 (SRPAPI *VSNetComm_AddMsgToQueueProc)(VS_HANDLE MsgHandle,VS_INT8 *MsgBuf);
typedef VS_INT32 (SRPAPI *VSNetComm_AddMsgQueueToQueueProc)(VS_HANDLE MsgHandle,VS_INT8 *HeadMsg);
typedef VS_INT8 *(SRPAPI *VSNetComm_QueueMsgProc)(VS_INT8 *MsgTail,VS_INT8 *Msg);  /* link msg, and return MsgTail*/
typedef void (SRPAPI *VSNetComm_FreeMsgBufProc)(VS_HANDLE MsgHandle,VS_INT8 *MsgBuf);
typedef void (SRPAPI *VSNetComm_FreeMsgBufQueueProc)(VS_HANDLE MsgHandle,VS_INT8 *MsgBufHead);
#if defined(__cplusplus) || defined(c_plusplus)
typedef class ClassOfLinkControlInterface_Lock *(SRPAPI *VSNetComm_GetLockInterfaceProc)();
typedef class ClassOfLinkControlInterface_IndexTree *(SRPAPI *VSNetComm_GetIndexTreeInterfaceProc)();
typedef class ClassOfLinkControlInterface_MemoryManager *(SRPAPI *VSNetComm_GetMemoryManagerInterfaceProc)(VS_ULONG ItemSize);
#else
typedef void *(SRPAPI *VSNetComm_GetLockInterfaceProc)();
typedef void *(SRPAPI *VSNetComm_GetIndexTreeInterfaceProc)();
typedef void *(SRPAPI *VSNetComm_GetMemoryManagerInterfaceProc)(VS_ULONG ItemSize);
#endif
/*------------------------------------------------------------------------------*/
#if defined(__cplusplus) || defined(c_plusplus)
class ClassOfLinkControlInterface_Lock{
public:
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS ) 		
	  virtual ~ClassOfLinkControlInterface_Lock(){};
#endif	  
    virtual void SRPAPI Release() = 0;
    virtual void SRPAPI Lock() = 0;
    virtual void SRPAPI UnLock() = 0;
};    

/*---index, support one key, VS_ULONG*/
class ClassOfLinkControlInterface_IndexTree{
public:
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS ) 		
	virtual ~ClassOfLinkControlInterface_IndexTree(){};
#endif	
    virtual void SRPAPI Release() = 0;
    virtual VS_INT32 SRPAPI InsertNode(VS_UWORD MainKey,VS_INT8 *Buf) = 0;
    virtual VS_INT8 *SRPAPI FindNode(VS_UWORD MainKey) = 0;
    virtual VS_INT8 *SRPAPI DelNode(VS_UWORD MainKey) = 0;
};

class ClassOfLinkControlInterface_MemoryManager{
public:
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS ) 		
	virtual ~ClassOfLinkControlInterface_MemoryManager(){};
#endif	
    virtual void SRPAPI Release() = 0;
    virtual void *SRPAPI GetItem() = 0;  /*  get one buffer*/
    virtual VS_BOOL SRPAPI FreeItem(void *In_Buf) = 0;
	virtual void  SRPAPI LockItem(void *In_Buf) = 0;   /*  increase frame ref count,when free,if ref count is not zero,then the free will be delayed*/
	virtual void  SRPAPI UnLockItem(void *In_Buf) = 0; /*  decrease frame ref count,if has free, and refcount is zero, then real free memory*/
};
#endif
/*------------------------------------------------------------------------------*/
/*----linklayer interface function*/
struct StructOfNetComm_LinkControlInterface{
    /*----msg interface*/
    VSNetComm_GetControlMsgBufProc GetControlMsgBufProc;
    VSNetComm_GetDataMsgBufProc GetDataMsgBufProc;
    VSNetComm_AddMsgToQueueProc AddMsgToQueueProc;
    VSNetComm_AddMsgQueueToQueueProc AddMsgQueueToQueueProc;
    VSNetComm_FreeMsgBufProc FreeMsgBufProc;
    VSNetComm_FreeMsgBufQueueProc FreeMsgBufQueueProc;
    VSNetComm_QueueMsgProc QueueMsgProc;
    /*----function interface*/
    VSNetComm_GetLockInterfaceProc GetLockInterfaceProc;
    VSNetComm_GetIndexTreeInterfaceProc GetIndexTreeInterfaceProc;
    VSNetComm_GetMemoryManagerInterfaceProc GetMemoryManagerInterfaceProc;
};

MYLIBAPI VS_INT32 SRPAPI VSNetComm_LinkLayer_Init(VS_HANDLE InterfaceHandle,VS_HANDLE In_UpLayerMessageHandle,struct StructOfNetComm_LinkControlInterface *LinkControlInterface);  /*   ==0 success; or else fail*/
MYLIBAPI void SRPAPI VSNetComm_LinkLayer_Term();

/*----for SetupServer.if RequestID is 0,then return result directly, if the result is 0xFFFFFFFF, means fail*/
MYLIBAPI VS_ULONG SRPAPI VSNetComm_LinkLayer_SetupServer(VS_ULONG RequestID,VS_ULONG BandWidth,VS_INT8 *LocalServerName,VS_UINT16 PortNumber,VS_INT8 *Para);
MYLIBAPI void SRPAPI VSNetComm_LinkLayer_SetupClient(VS_ULONG RequestID,VS_ULONG BandWidth,VS_INT8 *ServerName,VS_UINT16 PortNumber,VS_INT8 *Para);

MYLIBAPI void SRPAPI VSNetComm_LinkLayer_ReleaseClient(VS_ULONG SocketIDOfClient);   

/*FreeFrameBufOrNot  FreeFrameBufOrNot == 0 do not freebuf  ==1 free buf*/
/*====NetComm_LinkLayer_Send if the connection has free, then free the send buf*/
MYLIBAPI VS_INT32 SRPAPI VSNetComm_LinkLayer_Send(VS_ULONG SocketIDOfClient,VS_INT32 Length,VS_INT8 *FrameBuf,VS_UINT8 FreeFrameBufOrNot);  /*  senddata,FrameBuf is frame structure*/
MYLIBAPI void SRPAPI VSNetComm_LinkLayer_Flush();  /*  time for send or recv*/

MYLIBAPI VS_INT8 *SRPAPI VSQueryLinkLayerFramePaylodOffset(void *Ptr);
MYLIBAPI VS_INT8 *SRPAPI VSGetLinkLayerFrameBufPtr();
MYLIBAPI void SRPAPI VSFreeLinkLayerFrameBufPtr(void *Ptr);




#endif

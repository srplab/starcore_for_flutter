#include "vsopencommtype.h"
#include "vsopenapi.h"

#ifndef _VSOPENCOREDLL
#define _VSOPENCOREDLL

#ifndef SRPAPI
#define SRPAPI __cdecl
#endif

//------------------------------------------------------------------------------

#define VSCORE_REGISTERCALLBACKINFO_NAME "VSCore_RegisterCallBackInfo"
typedef void (SRPAPI *VSCore_RegisterCallBackInfoProc)( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
#define VSCORE_UNREGISTERCALLBACKINFO_NAME "VSCore_UnRegisterCallBackInfo"
typedef void (SRPAPI *VSCore_UnRegisterCallBackInfoProc)( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );

#define VSCORE_INIT_NAME "VSCore_Init"
typedef VS_INT32 (SRPAPI *VSCore_InitProc)( VS_BOOL ServerFlag, VS_BOOL SRPPrintFlag, const VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, const VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient, VS_STARCONFIGEX *ConfigEx );

#define VSCORE_LUAINIT_NAME "VSCore_LuaInit"
typedef VS_INT32 (SRPAPI *VSCore_LuaInitProc)( const VS_INT8 *LuaFile, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory, VS_STARCONFIGEX *ConfigEx );   //WorkDirectory maybe NULL

#define VSCORE_LUAINITBUF_NAME "VSCore_LuaInitBuf"
typedef VS_INT32 (SRPAPI *VSCore_LuaInitBufProc)( const VS_INT8 *LuaBuf, VS_INT32 LuaBufSize, const VS_INT8 *Name, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory, VS_STARCONFIGEX *ConfigEx );

#define VSCORE_TERM_NAME "VSCore_Term"
typedef void (SRPAPI *VSCore_TermProc)( );

#define VSCORE_TERMEX_NAME "VSCore_TermEx"
typedef void (SRPAPI *VSCore_TermExProc)( );

#define VSCORE_HASINIT_NAME "VSCore_HasInit"
typedef VS_BOOL (SRPAPI *VSCore_HasInitProc)( );

#define VSCORE_QUERYCONTROLINTERFACE_NAME "VSCore_QueryControlInterface"
#if defined(__cplusplus) || defined(c_plusplus)
typedef class ClassOfSRPControlInterface *(SRPAPI *VSCore_QueryControlInterfaceProc)( );
#else
typedef void *(SRPAPI *VSCore_QueryControlInterfaceProc)( );
#endif

#define VSCORE_GETSXMLINTERFACE_NAME "VSCore_GetSXMLInterface"
#if defined(__cplusplus) || defined(c_plusplus)
typedef class ClassOfSRPSXMLInterface *(SRPAPI *VSCore_GetSXMLInterfaceProc)( );
#else
typedef void *(SRPAPI *VSCore_GetSXMLInterfaceProc)( );
#endif

#define VSCORE_GETCFUNCTIONTABLE_NAME "VSCore_GetCFunctionTable"
typedef void *(SRPAPI *VSCore_GetCFunctionTableProc)( );

#define VSCORE_GETCONFIGPATH_NAME "VSCore_GetConfigPath"
typedef void (SRPAPI *VSCore_GetConfigPathProc)( VS_UINT16 ProgramRunType,VS_CHAR *Buf,VS_INT32 BufSize);

extern void SRPAPI VSCoreLib_RegisterCallBackInfo( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
extern void SRPAPI VSCoreLib_UnRegisterCallBackInfo( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
extern VS_INT32 SRPAPI VSCoreLib_Init( VS_BOOL ServerFlag, VS_BOOL SRPPrintFlag, const VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, const VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient, VS_STARCONFIGEX *ConfigEx );
extern VS_INT32 SRPAPI VSCoreLib_LuaInit( const VS_INT8 *LuaFile, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory );
extern VS_INT32 SRPAPI VSCoreLib_LuaInitBuf( const VS_INT8 *LuaBuf, VS_INT32 LuaBufSize, const VS_INT8 *Name, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory );
extern void SRPAPI VSCoreLib_Term( );
extern void SRPAPI VSCoreLib_TermEx( );
extern VS_BOOL SRPAPI VSCoreLib_HasInit( );
#if defined(__cplusplus) || defined(c_plusplus)
extern class ClassOfSRPControlInterface *SRPAPI VSCoreLib_QueryControlInterface( );
extern class ClassOfSRPSXMLInterface *SRPAPI VSCoreLib_GetSXMLInterface( );
#else
extern void *SRPAPI VSCoreLib_QueryControlInterface( );
extern void *SRPAPI VSCoreLib_GetSXMLInterface( );
#endif
extern void SRPAPI VSCoreLib_GetConfigPath( VS_UINT16 ProgramRunType,VS_CHAR *Buf,VS_INT32 BufSize);
extern void *SRPAPI VSCoreLib_GetCFunctionTable( );

/*--------------------------------------------------*/
#ifndef _CORELIB
#ifdef _COREDLL
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" SRPDLLEXPORT void SRPAPI VSCore_RegisterCallBackInfo( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
extern "C" SRPDLLEXPORT void SRPAPI VSCore_UnRegisterCallBackInfo( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
extern "C" SRPDLLEXPORT VS_INT32 SRPAPI VSCore_Init( VS_BOOL ServerFlag, VS_BOOL SRPPrintFlag, const VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, const VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient, VS_STARCONFIGEX *ConfigEx );
extern "C" SRPDLLEXPORT VS_INT32 SRPAPI VSCore_LuaInit( const VS_INT8 *LuaFile, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory, VS_STARCONFIGEX *ConfigEx );
extern "C" SRPDLLEXPORT VS_INT32 SRPAPI VSCore_LuaInitBuf( const VS_INT8 *LuaBuf, VS_INT32 LuaBufSize, const VS_INT8 *Name, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory, VS_STARCONFIGEX *ConfigEx );
extern "C" SRPDLLEXPORT void SRPAPI VSCore_Term( );
extern "C" SRPDLLEXPORT void SRPAPI VSCore_TermEx( );
extern "C" SRPDLLEXPORT VS_BOOL SRPAPI VSCore_HasInit( );
extern "C" SRPDLLEXPORT class ClassOfSRPControlInterface *SRPAPI VSCore_QueryControlInterface( );
extern "C" SRPDLLEXPORT class ClassOfSRPSXMLInterface *SRPAPI VSCore_GetSXMLInterface( );
extern "C" SRPDLLEXPORT void SRPAPI VSCore_GetConfigPath( VS_UINT16 ProgramRunType,VS_CHAR *Buf,VS_INT32 BufSize);
extern "C" SRPDLLEXPORT void *SRPAPI VSCore_GetCFunctionTable( );
/*---lua init function*/
extern "C" SRPDLLEXPORT int SRPAPI luaopen_libstarcore( void *L );
extern "C" SRPDLLEXPORT int SRPAPI luaopen_libstarcore53( void *L );
#else
extern SRPDLLEXPORT void SRPAPI VSCore_RegisterCallBackInfo( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
extern SRPDLLEXPORT void SRPAPI VSCore_UnRegisterCallBackInfo( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
extern SRPDLLEXPORT VS_INT32 SRPAPI VSCore_Init( VS_BOOL ServerFlag, VS_BOOL SRPPrintFlag, const VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, const VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient, VS_STARCONFIGEX *ConfigEx );
extern SRPDLLEXPORT VS_INT32 SRPAPI VSCore_LuaInit( const VS_INT8 *LuaFile, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory, VS_STARCONFIGEX *ConfigEx );
extern SRPDLLEXPORT VS_INT32 SRPAPI VSCore_LuaInitBuf( const VS_INT8 *LuaBuf, VS_INT32 LuaBufSize, const VS_INT8 *Name, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory, VS_STARCONFIGEX *ConfigEx );
extern SRPDLLEXPORT void SRPAPI VSCore_Term( );
extern SRPDLLEXPORT void SRPAPI VSCore_TermEx( );
extern SRPDLLEXPORT VS_BOOL SRPAPI VSCore_HasInit( );
extern SRPDLLEXPORT void *SRPAPI VSCore_QueryControlInterface( );
extern SRPDLLEXPORT void *SRPAPI VSCore_GetSXMLInterface( );
extern SRPDLLEXPORT void SRPAPI VSCore_GetConfigPath( VS_UINT16 ProgramRunType,VS_CHAR *Buf,VS_INT32 BufSize);
extern SRPDLLEXPORT void *SRPAPI VSCore_GetCFunctionTable( );
/*---lua init function */
extern SRPDLLEXPORT int SRPAPI luaopen_libstarcore( void *L );
extern SRPDLLEXPORT int SRPAPI luaopen_libstarcore53( void *L );
#endif
#else
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" SRPDLLIMPORT void SRPAPI VSCore_RegisterCallBackInfo( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
extern "C" SRPDLLIMPORT void SRPAPI VSCore_UnRegisterCallBackInfo( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
extern "C" SRPDLLIMPORT VS_INT32 SRPAPI VSCore_Init( VS_BOOL ServerFlag, VS_BOOL SRPPrintFlag, const VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, const VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient, VS_STARCONFIGEX *ConfigEx );
extern "C" SRPDLLIMPORT VS_INT32 SRPAPI VSCore_LuaInit( const VS_INT8 *LuaFile, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory, VS_STARCONFIGEX *ConfigEx );
extern "C" SRPDLLIMPORT VS_INT32 SRPAPI VSCore_LuaInitBuf( const VS_INT8 *LuaBuf, VS_INT32 LuaBufSize, const VS_INT8 *Name, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory, VS_STARCONFIGEX *ConfigEx );
extern "C" SRPDLLIMPORT void SRPAPI VSCore_Term( );
extern "C" SRPDLLIMPORT void SRPAPI VSCore_TermEx( );
extern "C" SRPDLLIMPORT VS_BOOL SRPAPI VSCore_HasInit( );
extern "C" SRPDLLIMPORT class ClassOfSRPControlInterface *SRPAPI VSCore_QueryControlInterface( );
extern "C" SRPDLLIMPORT class ClassOfSRPSXMLInterface *SRPAPI VSCore_GetSXMLInterface( );
extern "C" SRPDLLIMPORT void SRPAPI VSCore_GetConfigPath( VS_UINT16 ProgramRunType,VS_CHAR *Buf,VS_INT32 BufSize);
extern "C" SRPDLLIMPORT void *SRPAPI VSCore_GetCFunctionTable( );
/*---lua init function */
extern "C" SRPDLLIMPORT int SRPAPI luaopen_libstarcore( void *L );
extern "C" SRPDLLIMPORT int SRPAPI luaopen_libstarcore53( void *L );
#else
extern SRPDLLIMPORT void SRPAPI VSCore_RegisterCallBackInfo( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
extern SRPDLLIMPORT void SRPAPI VSCore_UnRegisterCallBackInfo( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
extern SRPDLLIMPORT VS_INT32 SRPAPI VSCore_Init( VS_BOOL ServerFlag, VS_BOOL SRPPrintFlag, const VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, const VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient, VS_STARCONFIGEX *ConfigEx );
extern SRPDLLIMPORT VS_INT32 SRPAPI VSCore_LuaInit( const VS_INT8 *LuaFile, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory, VS_STARCONFIGEX *ConfigEx );
extern SRPDLLIMPORT VS_INT32 SRPAPI VSCore_LuaInitBuf( const VS_INT8 *LuaBuf, VS_INT32 LuaBufSize, const VS_INT8 *Name, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory, VS_STARCONFIGEX *ConfigEx );
extern SRPDLLIMPORT void SRPAPI VSCore_Term( );
extern SRPDLLIMPORT void SRPAPI VSCore_TermEx( );
extern SRPDLLIMPORT VS_BOOL SRPAPI VSCore_HasInit( );
extern SRPDLLIMPORT void *SRPAPI VSCore_QueryControlInterface( );
extern SRPDLLIMPORT void *SRPAPI VSCore_GetSXMLInterface( );
extern SRPDLLIMPORT void SRPAPI VSCore_GetConfigPath( VS_UINT16 ProgramRunType,VS_CHAR *Buf,VS_INT32 BufSize);
extern SRPDLLIMPORT void *SRPAPI VSCore_GetCFunctionTable( );
/*---lua init function */
extern SRPDLLIMPORT int SRPAPI luaopen_libstarcore( void *L );
extern SRPDLLIMPORT int SRPAPI luaopen_libstarcore53( void *L );
#endif
#endif
#else
extern void SRPAPI VSCore_RegisterCallBackInfo( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
extern void SRPAPI VSCore_UnRegisterCallBackInfo( VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara );
extern VS_INT32 SRPAPI VSCore_Init( VS_BOOL ServerFlag, VS_BOOL SRPPrintFlag, const VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, const VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient, VS_STARCONFIGEX *ConfigEx );
extern VS_INT32 SRPAPI VSCore_LuaInit( const VS_INT8 *LuaFile, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory, VS_STARCONFIGEX *ConfigEx );
extern VS_INT32 SRPAPI VSCore_LuaInitBuf( const VS_INT8 *LuaBuf, VS_INT32 LuaBufSize, const VS_INT8 *Name, VS_INT8 **ErrorInfo, const VS_CHAR *WorkDirectory, VS_STARCONFIGEX *ConfigEx );
extern void SRPAPI VSCore_Term( );
extern void SRPAPI VSCore_TermEx( );
extern VS_BOOL SRPAPI VSCore_HasInit( );
#if defined(__cplusplus) || defined(c_plusplus)
extern class ClassOfSRPControlInterface *SRPAPI VSCore_QueryControlInterface( );
extern class ClassOfSRPSXMLInterface *SRPAPI VSCore_GetSXMLInterface( );
#else
extern void *SRPAPI VSCore_QueryControlInterface( );
extern void *SRPAPI VSCore_GetSXMLInterface( );
#endif
extern void SRPAPI VSCore_GetConfigPath( VS_UINT16 ProgramRunType,VS_CHAR *Buf,VS_INT32 BufSize);
extern void *SRPAPI VSCore_GetCFunctionTable( );
/*---lua init function */
extern int SRPAPI luaopen_libstarcore( void *L );
extern int SRPAPI luaopen_libstarcore53( void *L );
#endif

#endif

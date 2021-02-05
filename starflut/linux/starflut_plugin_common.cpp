#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#include <glib.h>
#include <cstring>

#include "include/starcore/vsopenapi.h"

//#define WAITFORCALLRESULT  /*for test, if defined, return call result defer.*/
#define CleObjectID_LENGTH 64

static FlMethodChannel* channel; 

///--------------------StarParaPkg_ToTuple
///In_SRPInterface->LuaPushString(str);  /*--can not accept 0 at middle--*/

/*--------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
static class ClassOfSRPControlInterface *SRPControlInterface = NULL;
static class ClassOfBasicSRPInterface *BasicSRPInterface = NULL; /*--add to dict, do not call release--*/
static class ClassOfSRPInterface *SRPInterface = NULL; /*---add to dict, should call release--*/

const char StarCorePrefix[] =     "@s_s_c";
const char StarSrvGroupPrefix[] = "@s_s_g";  /*--with group index: not supported */
const char StarServicePrefix[] =  "@s_s_e";  /*--with service id--*/
const char StarParaPkgPrefix[] =  "@s_s_p";
const char StarBinBufPrefix[]  =  "@s_s_b";
//const char StarSXmlPrefix[]  =  "@s_s_x";
const char StarObjectPrefix[] = "@s_s_o";    /*--with object id--*/
/*--objectag:  prefix + uuid + '+' + (objectid/serviceid/groupindex) */

static bool ModuleInitFlag = false; 

static VS_MUTEX mutexObject;
static bool ExitAppFlag;                           

static void *CleObjectMap = NULL;
static void *starcoreCmdMap = NULL;

static VS_MUTEX g_WaitResultMap_mutexObject;
static int g_WaitResultMap_Index;
static void *g_WaitResultMap = NULL;;

static VS_MUTEX starCoreThreadCallDeepSyncObject;
static int starCoreThreadCallDeep = 0;

static VS_INT8 ObjectCreate_AttachBuf[64*1024];

/*--command--*/
#define starcore_getDocumentPath 1
#define starcore_isAndroid  2
#define starcore_getResourcePath 3
#define starcore_rubyInitExt     4
#define starcore_getPesudoExport 5   /*this command should not be called*/
#define starcore_getPlatform     6
#define starcore_loadLibrary 7
#define starcore_setEnv 8
#define starcore_getEnv 9
#define starcore_getAssetsPath 10
#define starcore_typeCheck       11  /*only for debug for windows and linux, do not call this*/

#define starcore_init  100
#define starcore_sRPDispatch  101
#define starcore_sRPLock  102
#define starcore_sRPUnLock  103

#define starcore_poplocalframe  201
#define starcore_InitCore       202
#define starcore_InitSimpleEx   203
#define starcore_InitSimple     204
#define starcore_GetSrvGroup    205
#define starcore_moduleExit     206
#define starcore_moduleClear    207
#define starcore_regMsgCallBackP 208
#define starcore_setRegisterCode 209
#define starcore_isRegistered    210
#define starcore_setLocale       211
#define starcore_getLocale       212
#define starcore_version         213
#define starcore_getScriptIndex  214
#define starcore_setScript       215
#define starcore_detachCurrentThread 216
#define starcore_coreHandle      217
#define starcore_captureScriptGIL 218
#define starcore_releaseScriptGIL 219
#define starcore_setShareLibraryPath 220

#define StarSrvGroupClass_toString 300
#define StarSrvGroupClass_createService  301
#define StarSrvGroupClass_getService 302
#define StarSrvGroupClass_clearService 303
#define StarSrvGroupClass_newParaPkg 304
#define StarSrvGroupClass_newBinBuf 305
#define StarSrvGroupClass_getServicePath 306
#define StarSrvGroupClass_setServicePath 307
#define StarSrvGroupClass_servicePathIsSet 308
#define StarSrvGroupClass_getCurrentPath 309
#define StarSrvGroupClass_importService 310
#define StarSrvGroupClass_clearServiceEx 311
#define StarSrvGroupClass_runScript 312
#define StarSrvGroupClass_runScriptEx 313
#define StarSrvGroupClass_doFile 314
#define StarSrvGroupClass_doFileEx 315
#define StarSrvGroupClass_setClientPort 316
#define StarSrvGroupClass_setTelnetPort 317
#define StarSrvGroupClass_setOutputPort 318
#define StarSrvGroupClass_setWebServerPort 319
#define StarSrvGroupClass_initRaw 320
#define StarSrvGroupClass_loadRawModule 321
#define StarSrvGroupClass_getLastError 322
#define StarSrvGroupClass_getLastErrorInfo 323
#define StarSrvGroupClass_getCorePath 324
#define StarSrvGroupClass_getUserPath 325
#define StarSrvGroupClass_getLocalIP 326
#define StarSrvGroupClass_getLocalIPEx 327
#define StarSrvGroupClass_getObjectNum 328
#define StarSrvGroupClass_activeScriptInterface 329
#define StarSrvGroupClass_preCompile 330

#define StarServiceClass_toString  400
#define StarServiceClass_get 401
#define StarServiceClass_getObject 402
#define StarServiceClass_getObjectEx 403
#define StarServiceClass_newObject 404
#define StarServiceClass_runScript 405
#define StarServiceClass_runScriptEx 406
#define StarServiceClass_doFile 408
#define StarServiceClass_doFileEx 409
#define StarServiceClass_isServiceRegistered 410
#define StarServiceClass_checkPassword 411
#define StarServiceClass_newRawProxy 412
#define StarServiceClass_importRawContext 413
#define StarServiceClass_getLastError 414
#define StarServiceClass_getLastErrorInfo 415
#define StarServiceClass_allObject 416
#define StarServiceClass_restfulCall 417

#define StarParaPkgClass_toString 500
#define StarParaPkgClass_GetNumber 501
#define StarParaPkgClass_get 502
#define StarParaPkgClass_clear 503
#define StarParaPkgClass_appendFrom 504
#define StarParaPkgClass_set 505
#define StarParaPkgClass_build 506
#define StarParaPkgClass_free 507
#define StarParaPkgClass_dispose 508
#define StarParaPkgClass_releaseOwner 509
#define StarParaPkgClass_asDict 510
#define StarParaPkgClass_isDict 511
#define StarParaPkgClass_fromJSon 512
#define StarParaPkgClass_toJSon 513
#define StarParaPkgClass_toTuple 514
#define StarParaPkgClass_equals 515
#define StarParaPkgClass_V 516

#define StarBinBufClass_toString 600
#define StarBinBufClass_GetOffset 601
#define StarBinBufClass_init 602
#define StarBinBufClass_clear 603
#define StarBinBufClass_saveToFile 604
#define StarBinBufClass_loadFromFile 605
#define StarBinBufClass_read 606
#define StarBinBufClass_write 607
#define StarBinBufClass_free 608
#define StarBinBufClass_dispose 609
#define StarBinBufClass_releaseOwner 610
#define StarBinBufClass_setOffset 611
#define StarBinBufClass_print 612
#define StarBinBufClass_asString 613

#define StarObjectClass_toString 700
#define StarObjectClass_get 701
#define StarObjectClass_set 702
#define StarObjectClass_call 703
#define StarObjectClass_newObject 704
#define StarObjectClass_free 705
#define StarObjectClass_dispose 706
#define StarObjectClass_hasRawContext 707
#define StarObjectClass_rawToParaPkg 708
#define StarObjectClass_getSourceScript 709
#define StarObjectClass_getLastError 710
#define StarObjectClass_getLastErrorInfo 711
#define StarObjectClass_releaseOwnerEx 712
#define StarObjectClass_regScriptProcP 713
#define StarObjectClass_instNumber 714
#define StarObjectClass_changeParent 715
#define StarObjectClass_jsonCall 716

#define StarObjectClass_active 717
#define StarObjectClass_deActive 718
#define StarObjectClass_isActive 719


static FlValue *fromBool(VS_BOOL val)
{
    if( val == VS_TRUE )
        return fl_value_new_bool(true);
    return fl_value_new_bool(false);
}

static VS_BOOL toBool(FlValue *val)
{
    if(fl_value_get_type(val) != FL_VALUE_TYPE_BOOL )
        return VS_FALSE;
    if( fl_value_get_bool(val) == true )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL IsNull(FlValue* Value)
{
    if(fl_value_get_type(Value) == FL_VALUE_TYPE_NULL )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL IsBool(FlValue* Value)
{
    if(fl_value_get_type(Value) == FL_VALUE_TYPE_BOOL )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL IsInt32(FlValue* Value)
{
    if(fl_value_get_type(Value) == FL_VALUE_TYPE_INT )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL IsInt64(FlValue* Value)
{
    if(fl_value_get_type(Value) == FL_VALUE_TYPE_INT )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL IsDouble(FlValue* Value)
{
    if(fl_value_get_type(Value) == FL_VALUE_TYPE_FLOAT )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL IsString(FlValue* Value)
{
    if(fl_value_get_type(Value) == FL_VALUE_TYPE_STRING )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL IsListInt8(FlValue* Value)
{
    if(fl_value_get_type(Value) == FL_VALUE_TYPE_UINT8_LIST )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL IsListInt32(FlValue* Value)
{
    if(fl_value_get_type(Value) == FL_VALUE_TYPE_INT32_LIST )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL IsListInt64(FlValue* Value)
{
    if(fl_value_get_type(Value) == FL_VALUE_TYPE_INT64_LIST )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL IsListDouble(FlValue* Value)
{
    if(fl_value_get_type(Value) == FL_VALUE_TYPE_FLOAT_LIST )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL IsList(FlValue* Value)
{
    if(fl_value_get_type(Value) == FL_VALUE_TYPE_LIST )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL IsMap(FlValue* Value)
{
    if(fl_value_get_type(Value) == FL_VALUE_TYPE_MAP )
        return VS_TRUE;
    return VS_FALSE;
}

/*--the returned value must be free--*/
static VS_CHAR *toString(const FlValue *val)
{
    if( val == NULL || fl_value_get_type((FlValue *)val) != FL_VALUE_TYPE_STRING)
        return NULL;
    const gchar* str_value = fl_value_get_string((FlValue *)val);
    const char* destDir = (const char *)str_value;
    return (VS_CHAR *)destDir;
}

static FlValue *fromString(const VS_CHAR *val)
{
    if( val == NULL )
        return fl_value_new_string("");
    FlValue *fl_value = fl_value_new_string(val);
    return fl_value;
}

static FlValue* fromStringEx(VS_CHAR* val,VS_INT32 val_Len)
{
    if (val == NULL)
        return fl_value_new_string("");
    return fl_value_new_string_sized(val,val_Len);
}

static VS_INT32 toInt32(FlValue *val)
{
    if( val == NULL )
        return 0;
    if(fl_value_get_type(val) == FL_VALUE_TYPE_INT )
        return (VS_INT32)fl_value_get_int(val);
    if(fl_value_get_type(val) == FL_VALUE_TYPE_FLOAT )
        return (VS_INT32)fl_value_get_float(val);
    return (VS_INT32)0;
}

static FlValue *fromInt32(VS_INT32 val)
{
    return fl_value_new_int((VS_INT64)val);
}

static VS_INT64 toInt64(FlValue *val)
{
    if( val == NULL )
        return 0;
    if(fl_value_get_type(val) == FL_VALUE_TYPE_INT )
        return (VS_INT64)fl_value_get_int(val);
    if(fl_value_get_type(val) == FL_VALUE_TYPE_FLOAT )
        return (VS_INT64)fl_value_get_float(val);
    return (VS_INT64)0;
}

static FlValue *fromInt64(VS_INT64 val)
{
    return fl_value_new_int((VS_INT64)val);
}

static VS_DOUBLE toDouble(FlValue *val)
{
    if( val == NULL )
        return 0.0f;
    if(fl_value_get_type(val) == FL_VALUE_TYPE_INT )
        return (VS_DOUBLE)fl_value_get_int(val);
    if(fl_value_get_type(val) == FL_VALUE_TYPE_FLOAT )
        return (VS_DOUBLE)fl_value_get_float(val);
    return (VS_DOUBLE)0.0f;
}

static FlValue *fromDouble(VS_DOUBLE val)
{
    return fl_value_new_float((VS_DOUBLE)val);
}

static FlValue *fromPointer(void *ptr)
{
    return fl_value_new_int((VS_INT64)ptr);
}

static void *toPointer(void *val)
{
    return val;
}

static FlValue *SRPObject_GetArrayObject(VS_INT32 argc,FlValue *args,VS_INT32 Index)
{
    if( Index >= argc )
        return NULL;
    return fl_value_get_list_value(args,Index);
}

/*--star message class--*/
#define starcore_ThreadTick_MessageID 0
#define starcore_ThreadTick_MethodCall 1
#define starcore_ThreadTick_Exit 2

struct StarflutMessage{
  VS_UINT16  MsgClass;              /*  User type, you can not use the parameter                              */
  VS_UINT16  OperateCode;           /*  Operation code, which is defined to separate the different layers     */
  void       *PrivateBuf;           /*  Used By Kernel    */
  FlMethodCall *call;
  struct StarCoreWaitResult* WaitResult;
};

/*--add 0.9.5--*/
#define MAX_STARTHREAD_NUMBER 8

struct StructOfStarThreadWorker {
    VS_THREADID ThreadID;
    VS_COND ThreadExitCond;
    VS_BOOL IsBusy;

    GAsyncQueue* StarflutMessageQueue;
};

static struct StructOfStarThreadWorker* StarThreadWorker[MAX_STARTHREAD_NUMBER];  /*the index 0 is main thread, and will create at init stage*/
static VS_MUTEX StarThreadWorkerSyncObject;

static VS_BOOL CreateStarThreadWorker(VS_THREADID ThreadID)  /*failed if max number */
{
    vs_mutex_lock(&StarThreadWorkerSyncObject);
    for (VS_INT32 i = 0; i < MAX_STARTHREAD_NUMBER; i++) {
        if (StarThreadWorker[i] == NULL) {
            struct StructOfStarThreadWorker* Worker = g_new0(StructOfStarThreadWorker, 1);
            Worker->IsBusy = VS_FALSE;
            Worker->ThreadID = ThreadID;
            vs_cond_init(&Worker->ThreadExitCond);
            Worker->StarflutMessageQueue = g_async_queue_new();
            StarThreadWorker[i] = Worker;
            vs_mutex_unlock(&StarThreadWorkerSyncObject);
            return VS_TRUE;
        }
    }
    vs_mutex_unlock(&StarThreadWorkerSyncObject);
    return VS_FALSE;
}

static VS_BOOL CanCreateStarThreadWorker()
{
    vs_mutex_lock(&StarThreadWorkerSyncObject);
    for (VS_INT32 i = 0; i < MAX_STARTHREAD_NUMBER; i++) {
        if (StarThreadWorker[i] == NULL) {
            vs_mutex_unlock(&StarThreadWorkerSyncObject);
            return VS_TRUE;
        }
    }
    vs_mutex_unlock(&StarThreadWorkerSyncObject);
    return VS_FALSE;
}

static struct StructOfStarThreadWorker* GetStarThreadWorker()  /*get idle worker*/
{
    vs_mutex_lock(&StarThreadWorkerSyncObject);
    for (VS_INT32 i = 0; i < MAX_STARTHREAD_NUMBER; i++) {
        if (StarThreadWorker[i] != NULL && StarThreadWorker[i]->IsBusy == VS_FALSE) {
            vs_mutex_unlock(&StarThreadWorkerSyncObject);
            return StarThreadWorker[i];
        }
    }
    vs_mutex_unlock(&StarThreadWorkerSyncObject);
    return NULL;
}

static struct StructOfStarThreadWorker* GetStarThreadWorkerCurrent()  /*get idle worker*/
{
    vs_mutex_lock(&StarThreadWorkerSyncObject);
    VS_THREADID ThreadID = vs_thread_currentid();
    for (VS_INT32 i = 0; i < MAX_STARTHREAD_NUMBER; i++) {
        if (StarThreadWorker[i] != NULL && StarThreadWorker[i]->ThreadID == ThreadID) {
            vs_mutex_unlock(&StarThreadWorkerSyncObject);
            return StarThreadWorker[i];
        }
    }
    vs_mutex_unlock(&StarThreadWorkerSyncObject);
    return NULL;
}

static void SetStarThreadWorkerBusy(VS_THREADID ThreadID, VS_BOOL BusyFlag)
{
    vs_mutex_lock(&StarThreadWorkerSyncObject);
    for (VS_INT32 i = 0; i < MAX_STARTHREAD_NUMBER; i++) {
        if (StarThreadWorker[i] != NULL && StarThreadWorker[i]->ThreadID == ThreadID) {
            StarThreadWorker[i]->IsBusy = BusyFlag;
            vs_mutex_unlock(&StarThreadWorkerSyncObject);
            return;
        }
    }
    vs_mutex_unlock(&StarThreadWorkerSyncObject);
    return;
}

static void SetStarThreadWorkerBusy(VS_BOOL BusyFlag)
{
    vs_mutex_lock(&StarThreadWorkerSyncObject);
    VS_THREADID ThreadID = vs_thread_currentid();
    for (VS_INT32 i = 0; i < MAX_STARTHREAD_NUMBER; i++) {
        if (StarThreadWorker[i] != NULL && StarThreadWorker[i]->ThreadID == ThreadID) {
            StarThreadWorker[i]->IsBusy = BusyFlag;
            vs_mutex_unlock(&StarThreadWorkerSyncObject);
            return;
        }
    }
    vs_mutex_unlock(&StarThreadWorkerSyncObject);
    return;
}

static StarflutMessage* initStarflutMessage(VS_UINT16 messageID, FlMethodCall *call)
{
    StarflutMessage *obj = (StarflutMessage *)g_new0(StarflutMessage,1);
    obj->MsgClass = messageID;
    obj->call = call;
    return obj;
}

static StarflutMessage* initStarflutMessage(VS_UINT16 messageID)
{
    StarflutMessage *obj = (StarflutMessage *)g_new0(StarflutMessage,1);
    obj->MsgClass = messageID;
    obj->call = NULL;
    return obj;
}

static void sendStarMessage(struct StructOfStarThreadWorker* Worker, StarflutMessage *message)
{
    g_async_queue_lock (Worker->StarflutMessageQueue);
    g_async_queue_push_unlocked (Worker->StarflutMessageQueue, message);
    g_async_queue_unlock (Worker->StarflutMessageQueue);
}

static StarflutMessage *getStarMessage(struct StructOfStarThreadWorker* Worker)
{
    g_async_queue_lock (Worker->StarflutMessageQueue);
    StarflutMessage *obj =(StarflutMessage *)g_async_queue_pop_unlocked (Worker->StarflutMessageQueue);
    g_async_queue_unlock (Worker->StarflutMessageQueue);
    return obj;
}

/*-------------*/
struct StarCoreWaitResult{
public:    
    VS_COND cond;
    FlValue* Result;
    int Tag;
public:
    StarCoreWaitResult();
    ~StarCoreWaitResult();    

    FlValue* WaitResult();
    void SetResult(FlValue *Val);
};

StarCoreWaitResult::StarCoreWaitResult()
{
    vs_cond_init(&cond);
    Result = NULL;
    Tag = 0;
}

StarCoreWaitResult::~StarCoreWaitResult()
{
    if( Result != NULL )
       fl_value_unref(Result);
    vs_cond_destroy(&cond);
}

static void vs_cond_wait_local( VS_COND *cond)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	WaitForSingleObject((*cond),INFINITE);
	ResetEvent((*cond));
	return;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	WaitForSingleObjectEx((*cond),INFINITE,VS_FALSE);
	ResetEvent((*cond));
	return;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	pthread_cond_wait(&cond ->cond,&cond ->mutex);
#endif
}

FlValue* StarCoreWaitResult::WaitResult()
{
    vs_cond_wait_local(&cond);
    FlValue* Ret_Result = Result;
    Result = NULL;
    return Ret_Result;
}

void StarCoreWaitResult::SetResult(FlValue* result)
{
    Result = result;
    vs_mutex_lock(&cond.mutex);
    vs_cond_signal(&cond);
    vs_mutex_unlock(&cond.mutex);
}


int get_WaitResultIndex()
{
    vs_mutex_lock(&g_WaitResultMap_mutexObject);
    int Index = g_WaitResultMap_Index;
    g_WaitResultMap_Index = g_WaitResultMap_Index + 1;
    if( g_WaitResultMap_Index == 0x7FFFFFFF)
        g_WaitResultMap_Index = 1;
    vs_mutex_unlock(&g_WaitResultMap_mutexObject);
    return Index;
}

StarCoreWaitResult *new_WaitResult(int Index)
{
    vs_mutex_lock(&g_WaitResultMap_mutexObject);
    StarCoreWaitResult *m_result = new StarCoreWaitResult();
    BasicSRPInterface->InsertOneKey(g_WaitResultMap,(VS_UWORD)Index,(VS_INT8 *)m_result );
    vs_mutex_unlock(&g_WaitResultMap_mutexObject);
    return m_result;
}

StarCoreWaitResult *get_WaitResult(int Index)
{
    vs_mutex_lock(&g_WaitResultMap_mutexObject);
    StarCoreWaitResult *m_result = (StarCoreWaitResult *)BasicSRPInterface->FindOneKey(g_WaitResultMap,(VS_UWORD)Index);
    vs_mutex_unlock(&g_WaitResultMap_mutexObject);
    return m_result;
}

void remove_WaitResult(int Index)
{
    vs_mutex_lock(&g_WaitResultMap_mutexObject);
    StarCoreWaitResult *m_result = (StarCoreWaitResult *)BasicSRPInterface->DelOneKey(g_WaitResultMap,(VS_UWORD)Index);    
    vs_mutex_unlock(&g_WaitResultMap_mutexObject);
    if( m_result != NULL)
        delete m_result;
}

/*---*/
static void Starflut_SRPDispatchRequestCallBack(VS_UWORD Para)
{
    StarflutMessage* message = initStarflutMessage(starcore_ThreadTick_MessageID);
    sendStarMessage(StarThreadWorker[0], message);
}


struct StructOfGlobalMsgCallBack_TimerHandlerArgs{
    FlValue *call_arg;
};

void GlobalMsgCallBack_RESULT_Handler_CallBack(GObject *source_object,GAsyncResult *res,gpointer user_data)
{
    g_autoptr(GError) error = NULL;
    g_autoptr(FlMethodResponse) response = fl_method_channel_invoke_method_finish (channel, res,&error);
    if (response == NULL) {
      g_warning ("Failed to call method: %s", error->message);
      return;
    }
 
    FlValue* value = fl_method_response_get_result (response, &error);
    if (value == NULL) {
      g_warning ("Method returned error: %s", error->message);
      return;
    }
    FlValue* plist = value;
    FlValue* o = fl_value_get_list_value(plist,2);
    int w_tag = toInt32(fl_value_get_list_value(plist,0));
    StarCoreWaitResult* t_WaitResult = NULL;
    if (w_tag != 0) {
        t_WaitResult = get_WaitResult(w_tag);
        t_WaitResult->SetResult(fl_value_new_null());
    }
    else {
        SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "Object function [%s] can not return value, for it is called in ui thread\n", toString(fl_value_get_list_value(plist,1)));
    }
}

static gboolean GlobalMsgCallBack_RESULT_Handler(struct StructOfGlobalMsgCallBack_TimerHandlerArgs *Args)
{
    fl_method_channel_invoke_method(channel,"starcore_msgCallBack",Args->call_arg,NULL,GlobalMsgCallBack_RESULT_Handler_CallBack,NULL);
    fl_value_unref(Args->call_arg);
    g_free(Args);
    return false;
}

static VS_UWORD SRPAPI GlobalMsgCallBack(VS_ULONG ServiceGroupID, VS_ULONG uMsg, VS_UWORD wParam, VS_UWORD lParam, VS_BOOL* IsProcessed, VS_UWORD Para)
{
    SetStarThreadWorkerBusy(VS_TRUE);
    VS_CHAR *Info = (VS_CHAR *)wParam;
    switch (uMsg) {
    case MSG_VSDISPMSG:
    case MSG_VSDISPLUAMSG:
    case MSG_DISPMSG:
    case MSG_DISPLUAMSG:
    {
        //"(IIsI)"
        int w_tag = get_WaitResultIndex();
        StarCoreWaitResult* m_WaitResult = new_WaitResult(w_tag);
        FlValue *cP = fl_value_new_list();
        fl_value_append_take(cP,fromInt32(ServiceGroupID));
        fl_value_append_take(cP,fromInt32(uMsg));
        fl_value_append_take(cP,fromString((char*)wParam));
        fl_value_append_take(cP,fromInt64(lParam));
        fl_value_append_take(cP,fromInt32(w_tag));
        /*--need run in main thread*/
        struct StructOfGlobalMsgCallBack_TimerHandlerArgs *Args = g_new0(struct StructOfGlobalMsgCallBack_TimerHandlerArgs,1);
        Args->call_arg = cP;
        vs_mutex_lock(&m_WaitResult->cond.mutex);
        g_timeout_add(0,(GSourceFunc) GlobalMsgCallBack_RESULT_Handler,(gpointer)Args);   
        SRPControlInterface->SRPUnLock();
        //id result = [m_WaitResult WaitResult];
        FlValue* result_wait = m_WaitResult->WaitResult();
        vs_mutex_unlock(&m_WaitResult->cond.mutex);
        remove_WaitResult(w_tag);
        if( result_wait != NULL)
          fl_value_unref(result_wait);
        SRPControlInterface->SRPLock();
        SetStarThreadWorkerBusy(VS_FALSE);
        return 0;       
    }
    default:
        SetStarThreadWorkerBusy(VS_FALSE);
        return 0;
    }
}

static FlValue* LuaToFlutterObject(class ClassOfSRPInterface* In_SRPInterface, VS_INT32 Index, VS_BOOL* Result);
static VS_BOOL FlutterObjectToLua(class ClassOfSRPInterface* In_SRPInterface,FlValue* valueobj);


struct StructOfScriptCallBack_TimerHandlerArgs{
    FlValue *call_arg;
};

void ScriptCallBack_RESULT_Handler_CallBack(GObject *source_object,GAsyncResult *res,gpointer user_data)
{
    g_autoptr(GError) error = NULL;
    g_autoptr(FlMethodResponse) response = fl_method_channel_invoke_method_finish (channel, res,&error);
    if (response == NULL) {
      g_warning ("Failed to call method: %s", error->message);
      return;
    }
 
    FlValue* value = fl_method_response_get_result (response, &error);
    if (value == NULL) {
      g_warning ("Method returned error: %s", error->message);
      return;
    }
    FlValue* plist = value;
    FlValue* o = fl_value_get_list_value(plist,2);
    int w_tag = toInt32(fl_value_get_list_value(plist,0));
    StarCoreWaitResult* t_WaitResult = NULL;
    if (w_tag != 0) {
        fl_value_ref(o);
        t_WaitResult = get_WaitResult(w_tag);
        t_WaitResult->SetResult(o);
    }
    else {
        SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "Object function [%s] can not return value, for it is called in ui thread\n", toString(fl_value_get_list_value(plist,1)));
    }    
}

static gboolean ScriptCallBack_RESULT_Handler(struct StructOfScriptCallBack_TimerHandlerArgs *Args)
{
    fl_method_channel_invoke_method(channel,"starobjectclass_scriptproc",Args->call_arg,NULL,ScriptCallBack_RESULT_Handler_CallBack,NULL);
    fl_value_unref(Args->call_arg);
    g_free(Args);
    return false;
}

static gboolean ScriptCallBack_FreeLocalFrame_RESULT_Handler(struct StructOfScriptCallBack_TimerHandlerArgs *Args)
{
    fl_method_channel_invoke_method(channel,"starobjectclass_scriptproc_freeLlocalframe",Args->call_arg,NULL,NULL,NULL);
    fl_value_unref(Args->call_arg);
    g_free(Args);
    return false;
}

static VS_INT32 SRPObject_ScriptCallBack(void* L)
{
    SetStarThreadWorkerBusy(VS_TRUE);
    //VS_ULONG ServiceGroupID = SRPControlInterface ->LuaGetInt( L, SRPControlInterface->LuaUpValueIndex(L, 1) );
    VS_CHAR* ScriptName = SRPInterface->LuaToString(SRPInterface->LuaUpValueIndex(3));
    void* Object = SRPInterface->LuaToObject(1);

    class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(Object);
    if (l_Service->IsRemoteCall(Object) == VS_TRUE)
        l_Service->SetRetCode(Object, VSRCALL_PARAERROR);
    //---create parameter
    if (l_Service->LuaGetTop() == 0) {
        l_Service->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "Call Object[%s] FlutterFunction [%s] Error Parameter Number ", l_Service->GetName(Object), ScriptName);
        SetStarThreadWorkerBusy(VS_FALSE);
        return 0;
    }
    VS_INT32 n = SRPInterface->LuaGetTop() - 1;
    FlValue *out = fl_value_new_list();
    VS_BOOL LuaToFlutterResult;
    for (VS_INT32 i = 0; i < n; i++) {
        FlValue * vv = LuaToFlutterObject(SRPInterface, i + 2, &LuaToFlutterResult);
        if (vv != NULL) {
            fl_value_append_take(out,vv);
        }
        else {
            fl_value_append_take(out,fl_value_new_null());
        }
        if (LuaToFlutterResult == VS_FALSE) {
            l_Service->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "Call Object[%s] JavaFunction [%s] Error,Parameter[%d] failed ", l_Service->GetName(Object), ScriptName, i);
            fl_value_unref(out);
            SetStarThreadWorkerBusy(VS_FALSE);
            return 0;
        }
    }
    //--
    int w_tag = get_WaitResultIndex();
    StarCoreWaitResult* m_WaitResult = new_WaitResult(w_tag);

    FlValue *cP = fl_value_new_list();

    VS_UUID ObjectID;
    l_Service->GetID(Object, &ObjectID);
    l_Service->AddRefEx(Object);
    VS_CHAR CleObjectID[CleObjectID_LENGTH];

    VS_UUID ObjectID_Temp;
    l_Service->CreateUuid(&ObjectID_Temp);

    sprintf(CleObjectID,"%s%s", StarObjectPrefix, l_Service->UuidToString(&ObjectID_Temp));
    l_Service->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)Object);

    VS_CHAR CleObjectID_ID[CleObjectID_LENGTH*2];
    sprintf(CleObjectID_ID, "%s+%s", CleObjectID, l_Service->UuidToString(&ObjectID));

    fl_value_append_take(cP,fromString(CleObjectID_ID));
    fl_value_append_take(cP,fromString(ScriptName));
    fl_value_append_take(cP,out);
    fl_value_append_take(cP,fromInt32(w_tag));

    /*--need run in main thread*/
    struct StructOfScriptCallBack_TimerHandlerArgs *Args = g_new0(struct StructOfScriptCallBack_TimerHandlerArgs,1);
    Args->call_arg = cP;

    vs_mutex_lock(&m_WaitResult->cond.mutex);
    g_timeout_add(0,(GSourceFunc) ScriptCallBack_RESULT_Handler,(gpointer)Args);   

    SRPControlInterface->SRPUnLock();
    FlValue *RetValue = m_WaitResult->WaitResult();
    vs_mutex_unlock(&m_WaitResult->cond.mutex);
    remove_WaitResult(w_tag);
    SRPControlInterface->SRPLock();

    l_Service->SetRetCode(Object, VSRCALL_OK);
    if (RetValue == NULL) {
        SetStarThreadWorkerBusy(VS_FALSE);
        return 0;
    }
    else {
        if( IsNull(RetValue) == VS_TRUE ){
            fl_value_unref(RetValue);
            SetStarThreadWorkerBusy(VS_FALSE);
            return 0;
        }
        FlValue *RetValueList = RetValue;
        VS_CHAR* FrameTag = toString(fl_value_get_list_value(RetValueList,0));

        n = l_Service->LuaGetTop();
        FlutterObjectToLua(l_Service, fl_value_get_list_value(RetValueList,1));

        FlValue *cP_l = fl_value_new_list();
        fl_value_append_take(cP_l,fromString(FrameTag));
        struct StructOfScriptCallBack_TimerHandlerArgs *Args_l = g_new0(struct StructOfScriptCallBack_TimerHandlerArgs,1);
        Args_l->call_arg = cP_l;
        g_timeout_add(0,(GSourceFunc) ScriptCallBack_FreeLocalFrame_RESULT_Handler,(gpointer)Args_l);

        fl_value_unref(RetValue);
        SetStarThreadWorkerBusy(VS_FALSE);
        return l_Service->LuaGetTop() - n;
    }
}

/*---*/
static VS_CHAR *getTagFromObjectMap(void* v)
{
    VS_QUERYRECORD QueryRecord;
    VS_INT8* Key = BasicSRPInterface->QueryFirstStringKeyEx(CleObjectMap, &QueryRecord);
    while (Key != NULL) {
        if (BasicSRPInterface->FindStringKey(CleObjectMap, Key) == v)
            return Key;
        Key = BasicSRPInterface->QueryNextStringKeyEx(CleObjectMap, &QueryRecord);
    }
    return NULL;
}

static void removeFromObjectMap(void* v)
{
    VS_QUERYRECORD QueryRecord;
    void* ObjectMapKeySet = BasicSRPInterface->CreateStringIndex(CleObjectID_LENGTH*2, 0);
    VS_INT8* Key = BasicSRPInterface->QueryFirstStringKeyEx(CleObjectMap, &QueryRecord);
    while (Key != NULL) {
        void* obj = BasicSRPInterface->FindStringKey(CleObjectMap, Key);
        if (v == NULL || obj == v) {
            BasicSRPInterface->InsertStringKey(ObjectMapKeySet, Key, (VS_INT8*)1);
            if (vs_string_strnicmp(Key, StarServicePrefix,vs_string_strlen(StarServicePrefix)) == 0 ) {
                class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)obj;
                l_Service->Release();
            }
            else if (vs_string_strnicmp(Key, StarObjectPrefix, vs_string_strlen(StarObjectPrefix)) == 0) {
                void* l_Object = obj;
                SRPInterface->UnLockGC(l_Object);
            }
            else if (vs_string_strnicmp(Key, StarParaPkgPrefix, vs_string_strlen(StarParaPkgPrefix)) == 0) {
                class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)obj;
                l_ParaPkg->Release();
            }
            else if (vs_string_strnicmp(Key, StarBinBufPrefix, vs_string_strlen(StarBinBufPrefix)) == 0) {
                class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)obj;
                l_BinBuf->Release();
            }
            else if (vs_string_strnicmp(Key, StarSrvGroupPrefix, vs_string_strlen(StarSrvGroupPrefix)) == 0) {
                class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)obj;
                if (l_SrvGroup != BasicSRPInterface)
                    l_SrvGroup->Release();
            }
        }
        Key = BasicSRPInterface->QueryNextStringKeyEx(CleObjectMap, &QueryRecord);
    }
    Key = BasicSRPInterface->QueryFirstStringKeyEx(ObjectMapKeySet, &QueryRecord);
    while (Key != NULL) {
        BasicSRPInterface->DelStringKey(CleObjectMap, Key);
        Key = BasicSRPInterface->QueryNextStringKeyEx(ObjectMapKeySet, &QueryRecord);
    }
    BasicSRPInterface->DestoryIndex(ObjectMapKeySet);
    return;
}

void removeFromObjectMapByKey(FlValue *kk)
{
    VS_QUERYRECORD QueryRecord;
    void* ObjectMapKeySet = BasicSRPInterface->CreateStringIndex(CleObjectID_LENGTH * 2, 0);
    for (size_t i = 0; i < fl_value_get_length(kk); i++) {
        VS_CHAR* key = toString(fl_value_get_list_value(kk,i));
        void* obj = BasicSRPInterface->FindStringKey(CleObjectMap, (VS_CHAR *)key);
        {
            BasicSRPInterface->InsertStringKey(ObjectMapKeySet, (VS_CHAR*)key, (VS_INT8*)1);
            if (vs_string_strnicmp((VS_CHAR*)key, StarServicePrefix, vs_string_strlen(StarServicePrefix)) == 0) {
                class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)obj;
                l_Service->Release();
            }
            else if (vs_string_strnicmp((VS_CHAR*)key, StarObjectPrefix, vs_string_strlen(StarObjectPrefix)) == 0) {
                void* l_Object = obj;
                SRPInterface->UnLockGC(l_Object);
            }
            else if (vs_string_strnicmp((VS_CHAR*)key, StarParaPkgPrefix, vs_string_strlen(StarParaPkgPrefix)) == 0) {
                class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)obj;
                l_ParaPkg->Release();
            }
            else if (vs_string_strnicmp((VS_CHAR*)key, StarBinBufPrefix, vs_string_strlen(StarBinBufPrefix)) == 0) {
                class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)obj;
                l_BinBuf->Release();
            }
            else if (vs_string_strnicmp((VS_CHAR*)key, StarSrvGroupPrefix, vs_string_strlen(StarSrvGroupPrefix)) == 0) {
                class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)obj;
                if (l_SrvGroup != BasicSRPInterface)
                    l_SrvGroup->Release();
            }
        }
    }
    VS_INT8* Key = BasicSRPInterface->QueryFirstStringKeyEx(ObjectMapKeySet, &QueryRecord);
    while (Key != NULL) {
        BasicSRPInterface->DelStringKey(CleObjectMap, Key);
        Key = BasicSRPInterface->QueryNextStringKeyEx(ObjectMapKeySet, &QueryRecord);
    }
    BasicSRPInterface->DestoryIndex(ObjectMapKeySet);
    return;
}

static VS_BOOL StartsWithString(VS_CHAR* str1, const VS_CHAR* str2)
{
    size_t Str1Length;
    size_t Str2Length;

    Str1Length = vs_string_strlen(str1);
    Str2Length = vs_string_strlen(str2);
    if (Str1Length < Str2Length)
        return VS_FALSE;
    if (vs_string_strnicmp(str1, str2, Str2Length) == 0)
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL StartsWithString(const FlValue* in_str1, const VS_CHAR* str2)
{
    VS_CHAR* str1 = toString(in_str1);
    size_t Str1Length;
    size_t Str2Length;

    Str1Length = vs_string_strlen(str1);
    Str2Length = vs_string_strlen(str2);
    if (Str1Length < Str2Length)
        return VS_FALSE;
    if (vs_string_strnicmp(str1, str2, Str2Length) == 0)
        return VS_TRUE;
    return VS_FALSE;
}

static VS_BOOL StarParaPkg_FromTuple_Sub(FlValue *tuple, VS_INT32 StartIndex, class ClassOfSRPParaPackageInterface* ParaPkg, VS_INT32 PkgStartIndex)
{
    VS_INT32 Index;

    if (StartIndex == 0 && (tuple == NULL || fl_value_get_length(tuple) == 0)) {
        return VS_TRUE;
    }
    if (StartIndex >= fl_value_get_length(tuple)) {
        return VS_FALSE;
    }
    for (Index = PkgStartIndex; Index < PkgStartIndex + fl_value_get_length(tuple) - StartIndex; Index++) {
        FlValue *valueobj = fl_value_get_list_value(tuple,Index - PkgStartIndex + StartIndex);
        if (valueobj == NULL || IsNull(valueobj) == VS_TRUE) {
            ParaPkg->InsertEmpty(Index);
        }
        else if (IsBool(valueobj) == VS_TRUE) {
            ParaPkg->InsertBool(Index, toBool(valueobj));
        }
        else if (IsInt32(valueobj) == VS_TRUE) {
            ParaPkg->InsertInt(Index, toInt32(valueobj));
        }
        else if (IsInt64(valueobj) == VS_TRUE) {
            ParaPkg->InsertInt64(Index, toInt32(valueobj));
        }
        else if (IsDouble(valueobj) == VS_TRUE) {
            ParaPkg->InsertFloat(Index, toDouble(valueobj));
        }
        else if (IsString(valueobj) == VS_TRUE) {
            VS_CHAR* str = toString(valueobj);
            if (StartsWithString(str, (VS_CHAR *)StarBinBufPrefix)) {
                class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)BasicSRPInterface->FindStringKey(CleObjectMap, str);
                if (l_BinBuf == NULL) {
                    SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", str);
                    return VS_FALSE;
                }
                ParaPkg->InsertBinEx(Index, l_BinBuf->GetBuf(), l_BinBuf->GetOffset(), l_BinBuf->IsFromRaw());
            }
            else if (StartsWithString(str, (VS_CHAR*)StarParaPkgPrefix)) {
                class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)BasicSRPInterface->FindStringKey(CleObjectMap, str);
                if (l_ParaPkg == NULL) {
                    SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", str);
                    return VS_FALSE;
                }
                ParaPkg->InsertParaPackage(Index, l_ParaPkg);
            }
            else if (StartsWithString(str, (VS_CHAR*)StarObjectPrefix)) {
                void* SRPObject = (void*)BasicSRPInterface->FindStringKey(CleObjectMap, str);
                if (SRPObject == NULL) {
                    SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starobject object[%s] can not be found..", str);
                    return VS_FALSE;
                }
                ParaPkg->InsertObject(Index, SRPObject);
            }
            else
                ParaPkg->InsertStr(Index, str);
        }
        else if (IsList(valueobj) == VS_TRUE) {
            class ClassOfSRPParaPackageInterface* l_ParaPkg = BasicSRPInterface->GetParaPkgInterface();
            if (StarParaPkg_FromTuple_Sub(valueobj, 0, l_ParaPkg, 0) == VS_FALSE) {
                l_ParaPkg->Release();
                return VS_FALSE;
            }
            ParaPkg->InsertParaPackage(Index, l_ParaPkg);
            l_ParaPkg->Release();
        }
        else if (IsMap(valueobj) == VS_TRUE) {
            FlValue *newt = fl_value_new_list();
            for (size_t iter =0; iter < fl_value_get_length(valueobj); iter++) {
                fl_value_append(newt,fl_value_get_map_key(valueobj,iter));
                fl_value_append(newt,fl_value_get_map_value(valueobj,iter));
            }
            class ClassOfSRPParaPackageInterface* l_ParaPkg = BasicSRPInterface->GetParaPkgInterface();
            if (StarParaPkg_FromTuple_Sub(newt, 0, l_ParaPkg, 0) == VS_FALSE) {
                fl_value_unref(newt);
                l_ParaPkg->Release();
                return VS_FALSE;
            }
            fl_value_unref(newt);
            l_ParaPkg->AsDict(VS_TRUE);
            ParaPkg->InsertParaPackage(Index, l_ParaPkg);
            l_ParaPkg->Release();
        }
        else {
            return VS_FALSE;
        }
    }
    return VS_TRUE;
}

static FlValue *StarParaPkg_GetAtIndex(class ClassOfSRPParaPackageInterface* ParaPkg, VS_INT32 Index);  /*return new value*/
static FlValue *StarParaPkg_ToTuple(class ClassOfSRPParaPackageInterface* l_ParaPkg)
{
    int Number = l_ParaPkg->GetNumber();
    if (l_ParaPkg->IsDict() == VS_TRUE && Number % 2 == 0) {
        VS_BOOL IsMap = VS_TRUE;
        for (int ii = 0; ii < Number / 2; ii++) {
            if( l_ParaPkg->GetType(ii*2) != SRPPARATYPE_CHARPTR ){
                IsMap = VS_FALSE;
                break;
            }
        }
        if( IsMap == VS_TRUE ){
            /*--convert to NSDictionary--*/
            FlValue* nd = fl_value_new_map();
            for (int ii = 0; ii < Number / 2; ii++) {
                FlValue *Key = StarParaPkg_GetAtIndex(l_ParaPkg, ii*2);
                FlValue *Value = StarParaPkg_GetAtIndex(l_ParaPkg, ii*2+1);
                fl_value_set_take(nd,Key,Value);
            }
            return nd;
        }else{
            FlValue* oo = fl_value_new_list();
            for (int i = 0; i < Number; i++) {
                FlValue* vv = StarParaPkg_GetAtIndex(l_ParaPkg, i);
                if (vv != NULL) {
                    fl_value_append_take(oo,vv);
                }
                else
                    fl_value_append_take(oo,fl_value_new_null());
            }        
            return oo;            
        }
    }
    else {
        FlValue* oo = fl_value_new_list();
        for (int i = 0; i < Number; i++) {
            FlValue* vv = StarParaPkg_GetAtIndex(l_ParaPkg, i);
            if (vv != NULL) {
                fl_value_append_take(oo,vv);
            }
            else
                fl_value_append_take(oo,fl_value_new_null());
        }        
        return oo;
    }
}

static FlValue* StarParaPkg_GetAtIndex(class ClassOfSRPParaPackageInterface* ParaPkg, VS_INT32 Index)
{
    class ClassOfSRPBinBufInterface* SRPBinBufInterface;
    class ClassOfSRPParaPackageInterface* SRPParaPackageInterface;
    VS_INT32 Length;
    void* Buf;
    void* SRPObject = NULL;

    if (Index >= ParaPkg->GetNumber())
        return NULL;
    switch (ParaPkg->GetType(Index)) {
    case SRPPARATYPE_BOOL:
        return fromBool(ParaPkg->GetBool(Index));
    case SRPPARATYPE_INT:
        return fromInt32(ParaPkg->GetInt(Index));
    case SRPPARATYPE_INT64:
        return fromInt64(ParaPkg->GetInt64(Index));
    case SRPPARATYPE_FLOAT:
        return fromDouble(ParaPkg->GetFloat(Index));
    case SRPPARATYPE_BIN:
    {
        VS_BOOL FromRaw;
        Buf = ParaPkg->GetBinEx(Index, &Length, &FromRaw);
        if (Buf == NULL)
            return NULL;
        if (FromRaw == VS_FALSE) {
            SRPBinBufInterface = BasicSRPInterface->GetSRPBinBufInterface();
            SRPBinBufInterface->Set(0, Length, (VS_INT8*)Buf);

            VS_UUID ObjectID;
            BasicSRPInterface->CreateUuid(&ObjectID);
            VS_CHAR CleObjectID_Buf[CleObjectID_LENGTH];
            sprintf(CleObjectID_Buf,"%s%s", StarBinBufPrefix, BasicSRPInterface->UuidToString(&ObjectID));
            BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID_Buf, (VS_INT8*)SRPBinBufInterface);
            return fromString(CleObjectID_Buf);
        }
        else {
            GBytes *adata = g_bytes_new(Buf,(gsize)Length);
            FlValue *fl = fl_value_new_uint8_list_from_bytes(adata);
            g_bytes_unref(adata);
            return fl;
        }
    }
    case SRPPARATYPE_CHARPTR:
    {
        VS_CHAR* Str;
        VS_UINT32 StrLen;
        Str = ParaPkg->GetStrEx(Index, &StrLen);
        if (StrLen == 0)
            return fromString("");
        else {
            return fromStringEx(Str, StrLen);
        }
    }
    case SRPPARATYPE_OBJECT:
    {
        SRPObject = ParaPkg->GetObject(Index);
        if (SRPObject == NULL)
            return NULL;
        /*
        class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(SRPObject);
        class ClassOfSRPParaPackageInterface *l_ParaPkg = l_Service -> RawToParaPkg(SRPObject);
         */
        class ClassOfSRPParaPackageInterface* l_ParaPkg = NULL;
        if (l_ParaPkg != NULL) {
            return StarParaPkg_ToTuple(l_ParaPkg);
        }
        else {
            VS_UUID ObjectID;
            class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(SRPObject);
            l_Service->AddRefEx(SRPObject);
            l_Service->GetID(SRPObject, &ObjectID);

            VS_UUID ObjectID_Temp;
            BasicSRPInterface->CreateUuid(&ObjectID_Temp);
            VS_CHAR CleObjectID_Buf[CleObjectID_LENGTH];
            sprintf(CleObjectID_Buf, "%s%s", StarObjectPrefix, BasicSRPInterface->UuidToString(&ObjectID_Temp));
            BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID_Buf, (VS_INT8*)SRPObject);

            VS_CHAR CleObjectID_Buf_Result[CleObjectID_LENGTH*2];
            sprintf(CleObjectID_Buf_Result, "%s+%s", CleObjectID_Buf, BasicSRPInterface->UuidToString(&ObjectID));
            return fromString(CleObjectID_Buf_Result);
        }
    }
    case SRPPARATYPE_PARAPKG:
    {
        SRPParaPackageInterface = ParaPkg->GetParaPackage(Index);
        if (SRPParaPackageInterface == NULL)
            return NULL;
        return StarParaPkg_ToTuple(SRPParaPackageInterface);
        /*
        SRPParaPackageInterface ->AddRef();
        NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarParaPkgPrefix,[[NSUUID UUID] UUIDString]];
        [CleObjectMap setObject:fromPointer(SRPParaPackageInterface) forKey:CleObjectID];
        return CleObjectID;
         */
    }
    case SRPPARATYPE_INVALID:
        return NULL;
    }
    return NULL;
}

static FlValue *SRPObject_AttributeToFlutterObject(VS_ATTRIBUTEINFO* AttributeInfo, VS_UINT8 AttributeIndex, class ClassOfSRPInterface* In_SRPInterface, VS_UINT8 AttributeType, VS_INT32 AttributeLength, VS_UUID* StructID, VS_ULONG BufOffset, VS_UINT8* Buf, VS_BOOL UseStructObject)
{
    void* AtomicStructObject;

    switch (AttributeType) {
    case VSTYPE_BOOL:
        return fromBool(((VS_BOOL*)&Buf[BufOffset])[0]);

    case VSTYPE_INT8:
        return fromInt32(((VS_INT8*)&Buf[BufOffset])[0]);

    case VSTYPE_UINT8:
        return fromInt32(((VS_INT8*)&Buf[BufOffset])[0]);

    case VSTYPE_INT16:
        return fromInt32(((VS_INT16*)&Buf[BufOffset])[0]);

    case VSTYPE_UINT16:
        return fromInt32(((VS_UINT16*)&Buf[BufOffset])[0]);

    case VSTYPE_INT32:
        return fromInt32(((VS_INT32*)&Buf[BufOffset])[0]);

    case VSTYPE_UINT32:
        return fromInt32(((VS_UINT32*)&Buf[BufOffset])[0]);

    case VSTYPE_INT64:
        return fromInt64(((VS_INT64*)&Buf[BufOffset])[0]);

    case VSTYPE_FLOAT:
        return fromDouble(((VS_FLOAT*)&Buf[BufOffset])[0]);

    case VSTYPE_DOUBLE:
        return fromDouble(((VS_DOUBLE*)&Buf[BufOffset])[0]);

    case VSTYPE_LONG:
        return fromInt32(((VS_LONG*)&Buf[BufOffset])[0]);

    case VSTYPE_ULONG:
        return fromInt32(((VS_ULONG*)&Buf[BufOffset])[0]);

    case VSTYPE_LONGHEX:
        return fromInt32(((VS_LONG*)&Buf[BufOffset])[0]);

    case VSTYPE_ULONGHEX:
        return fromInt32(((VS_ULONG*)&Buf[BufOffset])[0]);

    case VSTYPE_VSTRING:
        if (((VS_VSTRING*)&Buf[BufOffset])->Buf == NULL) {
            return fromString("");
        }
        else {
            return fromString(((VS_VSTRING*)&Buf[BufOffset])->Buf);
        }

    case VSTYPE_PTR:
    {
        if (AttributeInfo->SyncType != 0) {
            void* SRPObject = In_SRPInterface->QueryFirst(((void**)&Buf[BufOffset])[0]);
            if (SRPObject == NULL)
                return fl_value_new_null();
            VS_UUID ObjectID;
            class ClassOfSRPInterface* l_Service = In_SRPInterface->GetSRPInterface(SRPObject);
            l_Service->AddRefEx(SRPObject);
            l_Service->GetID(SRPObject, &ObjectID);

            VS_CHAR CleObjectID[CleObjectID_LENGTH];
            VS_UUID ObjectIDTemp;
            In_SRPInterface->CreateUuid(&ObjectIDTemp);

            sprintf(CleObjectID,"%s%s", StarObjectPrefix, In_SRPInterface->UuidToString(&ObjectIDTemp));
            In_SRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)SRPObject);

            VS_CHAR CleObjectID_Result[CleObjectID_LENGTH*2];
            sprintf(CleObjectID_Result, "%s+%s", CleObjectID, In_SRPInterface->UuidToString(&ObjectID));
            return fromString(CleObjectID_Result);
        }
        else {
#if defined(VSPLAT_64)
            return fromInt64(((VS_UWORD*)&Buf[BufOffset])[0]);
#else
            return fromInt32(((VS_UWORD*)&Buf[BufOffset])[0]);
#endif
        }
    }

    case VSTYPE_STRUCT:
        AtomicStructObject = In_SRPInterface->GetAtomicObject(StructID);
        if (AtomicStructObject == NULL)
            return fl_value_new_null();
        {
            FlValue *NewObject = fl_value_new_list();
            VS_INT32 AttributeNumber, i;
            VS_ATTRIBUTEINFO StructAttributeInfo;

            AttributeNumber = In_SRPInterface->GetAtomicStructAttributeNumber(AtomicStructObject);
            for (i = 0; i < AttributeNumber; i++) {
                if (In_SRPInterface->GetAtomicStructAttributeInfoEx(AtomicStructObject, i, &StructAttributeInfo) == VS_TRUE) {
                    FlValue *vv = SRPObject_AttributeToFlutterObject(&StructAttributeInfo, StructAttributeInfo.AttributeIndex, In_SRPInterface, StructAttributeInfo.Type, StructAttributeInfo.Length, &StructAttributeInfo.StructID, StructAttributeInfo.Offset + BufOffset, (VS_UINT8*)Buf, UseStructObject);
                    if (vv != NULL) {
                        fl_value_append_take(NewObject,vv);
                    }
                    else
                        fl_value_append_take(NewObject,fl_value_new_null());
                }
            }
            return NewObject;
        }

    case VSTYPE_COLOR:
        return fromInt32(((VS_COLOR*)&Buf[BufOffset])[0]);

    case VSTYPE_RECT:
        return fl_value_new_null();

    case VSTYPE_FONT:
        return fl_value_new_null();

    case VSTYPE_TIME:
        return fl_value_new_null();

    case VSTYPE_CHAR:
        if (AttributeLength == 1) {
            return fromInt32(((VS_CHAR*)&Buf[BufOffset])[0]);
        }
        else {
            return fromString((VS_CHAR*)&Buf[BufOffset]);
        }
    case VSTYPE_UUID:
    case VSTYPE_STATICID:
        return fromString(In_SRPInterface->UuidToString((VS_UUID*)&Buf[BufOffset]));

    default:
        return fl_value_new_null();
    }
}

static VS_BOOL SRPObject_FlutterObjectToAttribute(class ClassOfSRPInterface* In_SRPInterface, VS_UINT8 AttributeType, VS_INT32 AttributeLength, VS_UUID* StructID, FlValue *ObjectTemp, VS_ULONG BufOffset, VS_UINT8* Buf)
{
    VS_CHAR* CharValue;
    void* AtomicStruct;
    VS_ATTRIBUTEINFO AttributeInfo;

    switch (AttributeType) {
    case VSTYPE_BOOL:
        if (IsBool(ObjectTemp) == VS_TRUE) {
            ((VS_BOOL*)&Buf[BufOffset])[0] = toBool(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_INT8:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE) {
            ((VS_INT8*)&Buf[BufOffset])[0] = (VS_INT8)toInt32(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_UINT8:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE) {
            ((VS_UINT8*)&Buf[BufOffset])[0] = (VS_UINT8)toInt32(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_INT16:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE) {
            ((VS_INT16*)&Buf[BufOffset])[0] = (VS_INT16)toInt32(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_UINT16:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE) {
            ((VS_UINT16*)&Buf[BufOffset])[0] = (VS_UINT16)toInt32(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_INT32:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE) {
            ((VS_INT32*)&Buf[BufOffset])[0] = (VS_INT32)toInt32(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_UINT32:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE) {
            ((VS_UINT32*)&Buf[BufOffset])[0] = (VS_UINT32)toInt32(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_INT64:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE) {
            ((VS_INT64*)&Buf[BufOffset])[0] = (VS_INT64)toInt64(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_FLOAT:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE || IsDouble(ObjectTemp) == VS_TRUE) {
            ((VS_FLOAT*)&Buf[BufOffset])[0] = (VS_FLOAT)toDouble(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_DOUBLE:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE || IsDouble(ObjectTemp) == VS_TRUE) {
            ((VS_DOUBLE*)&Buf[BufOffset])[0] = (VS_DOUBLE)toDouble(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_LONG:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE) {
            ((VS_INT32*)&Buf[BufOffset])[0] = (VS_INT32)toInt32(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_ULONG:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE) {
            ((VS_UINT32*)&Buf[BufOffset])[0] = (VS_UINT32)toInt32(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_LONGHEX:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE) {
            ((VS_INT32*)&Buf[BufOffset])[0] = (VS_INT32)toInt32(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_ULONGHEX:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE) {
            ((VS_UINT32*)&Buf[BufOffset])[0] = (VS_UINT32)toInt32(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_VSTRING:
        CharValue = toString(ObjectTemp);
        if (CharValue == NULL)
            return VS_FALSE;
        strcpy((char*)&Buf[BufOffset], CharValue);
        return VS_TRUE;

    case VSTYPE_STRUCT:
        if (ObjectTemp == NULL || IsList(ObjectTemp) == VS_FALSE)
            return VS_FALSE;
        else {
            VS_INT32 num_index, i;
            FlValue* ObjectTempList = ObjectTemp;
            FlValue *LocalObjectTemp;

            AtomicStruct = In_SRPInterface->GetAtomicObject(StructID);
            if (AtomicStruct == NULL)
                return VS_FALSE;
            for (num_index = 0; num_index < fl_value_get_length(ObjectTempList); num_index++) {
                i = num_index;
                if (i >= 0 && i < In_SRPInterface->GetAtomicStructAttributeNumber(AtomicStruct)) {
                    In_SRPInterface->GetAtomicStructAttributeInfoEx(AtomicStruct, i, &AttributeInfo);
                    LocalObjectTemp = fl_value_get_list_value(ObjectTempList,num_index);
                    if (SRPObject_FlutterObjectToAttribute(In_SRPInterface, AttributeInfo.Type, AttributeInfo.Length, NULL, LocalObjectTemp, AttributeInfo.Offset + BufOffset, Buf) == VS_FALSE) {
                        return VS_FALSE;
                    }
                }
            }
        }
        return VS_TRUE;

    case VSTYPE_COLOR:
        if (IsInt32(ObjectTemp) == VS_TRUE || IsInt64(ObjectTemp) == VS_TRUE) {
            ((VS_UINT32*)&Buf[BufOffset])[0] = (VS_UINT32)toInt32(ObjectTemp);
            return VS_TRUE;
        }
        else
            return VS_FALSE;

    case VSTYPE_RECT:
        return VS_FALSE;

    case VSTYPE_FONT:
        return VS_FALSE;

    case VSTYPE_TIME:
        return VS_FALSE;

    case VSTYPE_CHAR:
        CharValue = toString(ObjectTemp);
        if (CharValue == NULL)
            return VS_FALSE;
        strncpy((char*)&Buf[BufOffset], CharValue, AttributeLength);
        ((VS_CHAR*)&Buf[BufOffset])[AttributeLength - 1] = 0;
        return VS_TRUE;

    case VSTYPE_UUID:
    case VSTYPE_STATICID:
        CharValue = toString(ObjectTemp);
        if (CharValue == NULL)
            return VS_FALSE;
        In_SRPInterface->StringToUuid(CharValue, (VS_UUID*)&Buf[BufOffset]);
        return VS_TRUE;

    default:
        return VS_FALSE;
    }
    return VS_FALSE;
}

static FlValue* LuaToFlutterObject(class ClassOfSRPInterface* In_SRPInterface, VS_INT32 Index, VS_BOOL* Result);

static FlValue* LuaTableToFlutter(class ClassOfSRPInterface* In_SRPInterface, VS_INT32 Index)
{
    FlValue *RetVal;
    FlValue *Val;
    FlValue **Val_temp;
    VS_INT32 IntTemp, Size;

    if (Index < 0)
        Index = Index - 1;
    Size = 0;
    In_SRPInterface->LuaPushNil();
    while (In_SRPInterface->LuaNext(Index) != 0) {
        switch (In_SRPInterface->LuaType(-2)) {
        case VSLUATYPE_INT:
        case VSLUATYPE_INT64:
        case VSLUATYPE_UWORD:
        case VSLUATYPE_NUMBER:
            IntTemp = In_SRPInterface->LuaToInt(-2);
            if (IntTemp > 0) {
                if (IntTemp > Size)
                    Size = IntTemp;
            }
            else {
                In_SRPInterface->LuaPop(2);
                return NULL;
            }
            break;
        default:
            In_SRPInterface->LuaPop(2);
            return NULL;
        }
        In_SRPInterface->LuaPop(1);
    }
    Val_temp = (FlValue **)g_new0(FlValue *,Size);
    In_SRPInterface->LuaPushNil();
    while (In_SRPInterface->LuaNext(Index) != 0) {
        switch (In_SRPInterface->LuaType(-2)) {
        case VSLUATYPE_INT:
        case VSLUATYPE_INT64:
        case VSLUATYPE_UWORD:
        case VSLUATYPE_NUMBER:
            IntTemp = In_SRPInterface->LuaToInt(-2);
            if (IntTemp > 0 && IntTemp <= Size) {
                VS_BOOL Result;
                Val = LuaToFlutterObject(In_SRPInterface, -1, &Result);
                if (Result == VS_FALSE) {
                    In_SRPInterface->LuaPop(2);
                    for( VS_INT32 i=0; i < Size; i++ ){
                        if( Val_temp[i] != NULL )
                            fl_value_unref(Val_temp[i]);
                    }                    
                    g_free(Val_temp);
                    return NULL;
                }
                if (Val != NULL) {
                    Val_temp[IntTemp - 1] = Val;
                }
                else
                    Val_temp[IntTemp - 1] = NULL;
            }
            else {
                In_SRPInterface->LuaPop(2);
                for( VS_INT32 i=0; i < Size; i++ ){
                    if( Val_temp[i] != NULL )
                        fl_value_unref(Val_temp[i]);
                }
                g_free(Val_temp);
                return NULL;
            }
            break;
        default:
            In_SRPInterface->LuaPop(2);
            for( VS_INT32 i=0; i < Size; i++ ){
                if( Val_temp[i] != NULL )
                    fl_value_unref(Val_temp[i]);
            }            
            g_free(Val_temp);
            return NULL;
        }
        In_SRPInterface->LuaPop(1);
    }
    RetVal = fl_value_new_list();
    for( VS_INT32 i=0; i < Size; i++ ){
        if( Val_temp[i] == NULL )
            fl_value_append_take(RetVal,fl_value_new_null());
        else
            fl_value_append_take(RetVal,Val_temp[i]);
    }
    g_free(Val_temp);
    return RetVal;
}

#if 0
static void FlutterToLuaTable(ClassOfSRPInterface* In_SRPInterface, id arr_data)
{
    VS_INT32 num_index;
    id localobject;

    In_SRPInterface->LuaNewTable();
    for (num_index = 0; num_index < [arr_data count]; num_index++) {
        In_SRPInterface->LuaPushInt(num_index + 1);
        localobject = [arr_data objectAtIndex : num_index];
        FlutterObjectToLua(In_SRPInterface, localobject);
        if (In_SRPInterface->LuaIsNil(-1) == VS_TRUE) {
            In_SRPInterface->LuaPop(3);
            In_SRPInterface->LuaPushNil();
            return;
        }
        In_SRPInterface->LuaSetTable(-3);
    }
    return;
}
#endif

static FlValue* LuaToFlutterObject(class ClassOfSRPInterface* In_SRPInterface, VS_INT32 Index, VS_BOOL* Result)
{
    (*Result) = VS_TRUE;
    switch (In_SRPInterface->LuaType(Index)) {
    case VSLUATYPE_INT: return fromInt32(In_SRPInterface->LuaToInt(Index));
    case VSLUATYPE_INT64: return fromInt64(In_SRPInterface->LuaToInt64(Index));
    case VSLUATYPE_UWORD:
    {
#if defined(ENV_M64)
        return fromInt64(In_SRPInterface->LuaToUWord(Index));
#else
        return fromInt32(In_SRPInterface->LuaToUWord(Index));
#endif
    }
    case VSLUATYPE_NUMBER: return fromDouble(In_SRPInterface->LuaToNumber(Index));
    case VSLUATYPE_BOOL:   return fromBool(In_SRPInterface->LuaToBool(Index));
    case VSLUATYPE_STRING:
    {
        VS_CHAR* StringBuf;
        VS_ULONG StringSize;

        StringBuf = In_SRPInterface->LuaToLString(Index, &StringSize);
        if (StringBuf != NULL) {
            return fromStringEx(StringBuf, StringSize);
        }
        else {
            return fromString("");
        }
    }
    case VSLUATYPE_TABLE:
        if (In_SRPInterface->LuaTableToParaPkg(Index, NULL, VS_TRUE) == VS_TRUE)
            return LuaTableToFlutter(In_SRPInterface, Index);
    case VSLUATYPE_FUNCTION:
    case VSLUATYPE_USERDATA:
    case VSLUATYPE_LIGHTUSERDATA:
    {
        void* SRPObject = In_SRPInterface->LuaToRaw(Index, VS_FALSE);
        if (SRPObject == NULL)
            return NULL;
        VS_UUID ObjectID;
        In_SRPInterface->GetID(SRPObject, &ObjectID);

        VS_CHAR CleObjectID[CleObjectID_LENGTH];
        VS_UUID ObjectIDTemp;
        In_SRPInterface->CreateUuid(&ObjectIDTemp);

        sprintf(CleObjectID, "%s%s", StarObjectPrefix, In_SRPInterface->UuidToString(&ObjectIDTemp));
        In_SRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)SRPObject);

        VS_CHAR CleObjectID_Result[CleObjectID_LENGTH * 2];
        sprintf(CleObjectID_Result, "%s+%s", CleObjectID, In_SRPInterface->UuidToString(&ObjectID));
        return fromString(CleObjectID_Result);
    }
    case VSLUATYPE_OBJECT:
    {
        void* SRPObject = In_SRPInterface->LuaToObject(Index);
        if (SRPObject == NULL)
            return NULL;
        /*
        class ClassOfSRPInterface *l_Service = In_SRPInterface->GetSRPInterface(SRPObject);
        class ClassOfSRPParaPackageInterface *l_ParaPkg = l_Service -> RawToParaPkg(SRPObject);
        */
        class ClassOfSRPParaPackageInterface* l_ParaPkg = NULL;
        if (l_ParaPkg != NULL)
            return StarParaPkg_ToTuple(l_ParaPkg);
        else {
            VS_UUID ObjectID;
            In_SRPInterface->AddRefEx(SRPObject);
            In_SRPInterface->GetID(SRPObject, &ObjectID);

            VS_CHAR CleObjectID[CleObjectID_LENGTH];
            VS_UUID ObjectIDTemp;
            In_SRPInterface->CreateUuid(&ObjectIDTemp);

            sprintf(CleObjectID, "%s%s", StarObjectPrefix, In_SRPInterface->UuidToString(&ObjectIDTemp));
            In_SRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)SRPObject);

            VS_CHAR CleObjectID_Result[CleObjectID_LENGTH * 2];
            sprintf(CleObjectID_Result, "%s+%s", CleObjectID, In_SRPInterface->UuidToString(&ObjectID));
            return fromString(CleObjectID_Result);
        }
    }
    case VSLUATYPE_PARAPKG:
    {
        class ClassOfSRPParaPackageInterface* SRPParaPackageInterface = In_SRPInterface->LuaToParaPkg(Index);
        if (SRPParaPackageInterface == NULL)
            return NULL;
        return StarParaPkg_ToTuple(SRPParaPackageInterface);
        /*
        SRPParaPackageInterface ->AddRef();
        NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarParaPkgPrefix,[[NSUUID UUID] UUIDString]];
        [CleObjectMap setObject:fromPointer(SRPParaPackageInterface) forKey:CleObjectID];
        return CleObjectID;
         */
    }
    case VSLUATYPE_BINBUF:
    {
        class ClassOfSRPBinBufInterface* BinBuf = In_SRPInterface->LuaToBinBuf(Index);
        VS_INT8* Buf = BinBuf->GetBuf();
        VS_BOOL FromRaw = BinBuf->IsFromRaw();
        VS_INT32 Length = BinBuf->GetOffset();

        if (Buf == NULL)
            return NULL;
        if (FromRaw == VS_FALSE) {
            BinBuf->AddRef();

            VS_CHAR CleObjectID[CleObjectID_LENGTH];
            VS_UUID ObjectIDTemp;
            In_SRPInterface->CreateUuid(&ObjectIDTemp);

            sprintf(CleObjectID, "%s%s", StarBinBufPrefix, In_SRPInterface->UuidToString(&ObjectIDTemp));
            In_SRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)BinBuf);
            return fromString(CleObjectID);
        }
        else {
            GBytes *adata = g_bytes_new(Buf,(gsize)Length);
            FlValue *fl = fl_value_new_uint8_list_from_bytes(adata);
            g_bytes_unref(adata);   
            return fl;         
        }
    }
    case VSLUATYPE_NIL: return NULL;
    default: (*Result) = VS_FALSE; return NULL;
    }
}

static VS_BOOL FlutterObjectToLua(class ClassOfSRPInterface* In_SRPInterface, FlValue* valueobj)
{
    if (IsBool(valueobj) == VS_TRUE) {
        In_SRPInterface->LuaPushBool(toBool(valueobj));
        return VS_TRUE;
    }
    else if (IsInt32(valueobj) == VS_TRUE) {
        In_SRPInterface->LuaPushInt(toInt32(valueobj));
        return VS_TRUE;
    }
    else if (IsInt64(valueobj) == VS_TRUE) {
        In_SRPInterface->LuaPushInt64(toInt64(valueobj));
        return VS_TRUE;
    }
    else if (IsDouble(valueobj) == VS_TRUE) {
        In_SRPInterface->LuaPushNumber(toDouble(valueobj));
        return VS_TRUE;
    } else if (IsString(valueobj) == VS_TRUE) {
        VS_CHAR* str = toString(valueobj);
        if (StartsWithString(str, StarBinBufPrefix)) {
            class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)BasicSRPInterface->FindStringKey(CleObjectMap, str);
            if (l_BinBuf == NULL) {
                SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", str);
                return VS_FALSE;
            }
            In_SRPInterface->LuaPushBinBuf(l_BinBuf, VS_FALSE);
            return VS_TRUE;
        }
        else if (StartsWithString(str, StarParaPkgPrefix)) {
            class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)BasicSRPInterface->FindStringKey(CleObjectMap, str);
            if (l_ParaPkg == NULL) {
                SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", str);
                return VS_FALSE;
            }
            In_SRPInterface->LuaPushParaPkg(l_ParaPkg, VS_FALSE);
            return VS_TRUE;
        }
        else if (StartsWithString(str, StarObjectPrefix)) {
            void* SRPObject = (void*)BasicSRPInterface->FindStringKey(CleObjectMap, str);
            if (SRPObject == NULL) {
                SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starobject object[%s] can not be found..", str);
                return VS_FALSE;
            }
            In_SRPInterface->LuaPushObject(SRPObject);
            return VS_TRUE;
        }
        else {
            //In_SRPInterface->LuaPushLString(fl_value_get_string(valueobj), (VS_ULONG)fl_value_get_length(valueobj));
            In_SRPInterface->LuaPushString(str);  /*--can not accept 0 at middle--*/
            return VS_TRUE;
        }
    }
    else if (IsList(valueobj) == VS_TRUE) {
        FlValue* valueobjList = valueobj;
        class ClassOfSRPParaPackageInterface* l_ParaPkg = BasicSRPInterface->GetParaPkgInterface();
        if (StarParaPkg_FromTuple_Sub(valueobjList, 0, l_ParaPkg, 0) == VS_FALSE) {
            l_ParaPkg->Release();
            return VS_FALSE;
        }
        In_SRPInterface->LuaPushParaPkg(l_ParaPkg, VS_TRUE);
        return VS_TRUE;
    }
    else if (IsMap(valueobj) == VS_TRUE) {
        FlValue *valueobjList = fl_value_new_list();
        for (size_t iter =0; iter < fl_value_get_length(valueobj); iter++) {
            fl_value_append(valueobjList,fl_value_get_map_key(valueobj,iter));
            fl_value_append(valueobjList,fl_value_get_map_value(valueobj,iter));
        }
        class ClassOfSRPParaPackageInterface* l_ParaPkg = BasicSRPInterface->GetParaPkgInterface();
        if (StarParaPkg_FromTuple_Sub(valueobjList, 0, l_ParaPkg, 0) == VS_FALSE) {
            l_ParaPkg->Release();
            fl_value_unref(valueobjList);
            return VS_FALSE;
        }
        l_ParaPkg->AsDict(VS_TRUE);
        fl_value_unref(valueobjList);
        In_SRPInterface->LuaPushParaPkg(l_ParaPkg, VS_TRUE);
        return VS_TRUE;
    }
    else if (valueobj == NULL || IsNull(valueobj) == VS_TRUE) {
        In_SRPInterface->LuaPushNil();
        return VS_TRUE;
    }
    else {
        return VS_FALSE;
    }
}

static FlValue *StarObject_getValue(class ClassOfSRPInterface* In_SRPInterface, void* SRPObject, FlValue* Name)
{
    VS_CHAR LocalNameBuf[64];
    VS_CHAR* ParaName;

    if (IsString(Name) ==VS_TRUE) {
        ParaName = toString(Name);
    }
    else {
        if (IsInt32(Name) == VS_TRUE || IsInt64(Name) == VS_TRUE) {
            sprintf(LocalNameBuf, "\"%d\"", toInt32(Name));
            ParaName = LocalNameBuf;
        }
        else {
            return NULL;
        }
    }
    if (ParaName[0] == '_' && ParaName[1] == '_' && ParaName[2] == '_') {
        //---process object namevalue
        VS_UINT8 Type;
        VS_INT32 IntValue;
        VS_DOUBLE FloatValue;
        VS_BOOL BoolValue;

        Type = In_SRPInterface->GetNameValueType(SRPObject, &ParaName[3]);
        switch (Type) {
        case SRPPARATYPE_BOOL:
            In_SRPInterface->GetNameBoolValue(SRPObject, &ParaName[3], &BoolValue, VS_FALSE);
            return fromBool(BoolValue);
        case SRPPARATYPE_INT:
            In_SRPInterface->GetNameIntValue(SRPObject, &ParaName[3], &IntValue, 0);
            return fromInt32(IntValue);
        case SRPPARATYPE_FLOAT:
            In_SRPInterface->GetNameFloatValue(SRPObject, &ParaName[3], &FloatValue, 0);
            return fromDouble(FloatValue);
        case SRPPARATYPE_CHARPTR:
            return fromString(In_SRPInterface->GetNameStrValue(SRPObject, &ParaName[3], ""));
        default:
            return NULL;
        }
    }
    if (vs_string_strcmp(ParaName, "_Service") == 0) {
        VS_UUID ObjectID;
        In_SRPInterface->AddRef();
        In_SRPInterface->GetServiceID(&ObjectID);

        VS_CHAR CleObjectID[CleObjectID_LENGTH];
        VS_UUID ObjectIDTemp;
        In_SRPInterface->CreateUuid(&ObjectIDTemp);

        sprintf(CleObjectID, "%s%s", StarServicePrefix, In_SRPInterface->UuidToString(&ObjectIDTemp));
        In_SRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)In_SRPInterface);

        VS_CHAR CleObjectID_Result[CleObjectID_LENGTH*2];
        sprintf(CleObjectID_Result, "%s+%s", CleObjectID, In_SRPInterface->UuidToString(&ObjectID));

        return fromString(CleObjectID_Result);
    }
    else if (vs_string_strcmp(ParaName, "_Class") == 0) {
        void* SRPObjectTemp = In_SRPInterface->GetClass(SRPObject);
        if (SRPObjectTemp == NULL)
            return NULL;
        VS_UUID ObjectID;
        In_SRPInterface->AddRefEx(SRPObjectTemp);
        In_SRPInterface->GetID(SRPObject, &ObjectID);

        VS_CHAR CleObjectID[CleObjectID_LENGTH];
        VS_UUID ObjectIDTemp;
        In_SRPInterface->CreateUuid(&ObjectIDTemp);

        sprintf(CleObjectID, "%s%s", StarObjectPrefix, In_SRPInterface->UuidToString(&ObjectIDTemp));
        In_SRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)SRPObject);

        VS_CHAR CleObjectID_Result[CleObjectID_LENGTH*2];
        sprintf(CleObjectID_Result, "%s+%s", CleObjectID, In_SRPInterface->UuidToString(&ObjectID));

        return fromString(CleObjectID_Result);
    }
    else if (vs_string_strcmp(ParaName, "_ID") == 0) {
        VS_UUID IDTemp;

        In_SRPInterface->GetID(SRPObject, &IDTemp);
        return fromString(In_SRPInterface->UuidToString(&IDTemp));
    }
    else if (vs_string_strcmp(ParaName, "_Name") == 0) {
        return fromString(In_SRPInterface->GetName(SRPObject));
    }
    //---check if is object's attribute
    VS_ATTRIBUTEINFO AttributeInfo;
    if (In_SRPInterface->GetAttributeInfoEx(In_SRPInterface->GetClass(SRPObject), ParaName, &AttributeInfo) == VS_TRUE) {
        return SRPObject_AttributeToFlutterObject(&AttributeInfo, AttributeInfo.AttributeIndex, In_SRPInterface, AttributeInfo.Type, AttributeInfo.Length, &AttributeInfo.StructID, AttributeInfo.Offset, (VS_UINT8*)SRPObject, VS_TRUE);
    }
    else {
        In_SRPInterface->LuaPushObject(SRPObject);
        In_SRPInterface->LuaPushString(ParaName);
        In_SRPInterface->LuaGetTable(-2);
        if (In_SRPInterface->LuaIsNil(-1) == VS_FALSE) {
            VS_BOOL LuaToFlutterResult;

            if (In_SRPInterface->LuaIsFunction(-1) == VS_TRUE) {
                In_SRPInterface->LuaPop(2);
                //--is lua function, return NULL, should use call
                return NULL;
            }
            //--change lua attribute to java attribute
            FlValue *ObjectTemp1 = LuaToFlutterObject(In_SRPInterface, -1, &LuaToFlutterResult);
            In_SRPInterface->LuaPop(2);
            if (LuaToFlutterResult == VS_FALSE) {
                In_SRPInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "Get Object[%s] Attribute [%s] Error", In_SRPInterface->GetName(SRPObject), ParaName);
                return NULL;
            }
            return ObjectTemp1;
        }
        In_SRPInterface->LuaPop(2);
        return NULL;
    }
}


/*-------------*/
void starflut_plugin_common_init(FlMethodChannel* in_channel)
{
    ModuleInitFlag = false;

    ExitAppFlag = false;
    vs_mutex_init(&mutexObject);

    CleObjectMap = NULL;
    starcoreCmdMap = NULL;

    vs_mutex_init(&starCoreThreadCallDeepSyncObject);
    vs_mutex_init(&g_WaitResultMap_mutexObject);
    g_WaitResultMap_Index = 1;
    g_WaitResultMap = NULL;

    memset(StarThreadWorker, 0, sizeof(StarThreadWorker));
    vs_mutex_init(&StarThreadWorkerSyncObject);

    channel = in_channel;
}

static 	VS_THREADID hCoreThreadId;  
static 	VS_HANDLE hCoreThreadHandle;

#if !defined(WAITFORCALLRESULT)
struct StructOfMainThreadTimerHandlerArgs{
    FlMethodCall *method_call;
    FlValue *result;
};

static gboolean MainThread_MESSAGE_RESULT_Handler(struct StructOfMainThreadTimerHandlerArgs *Args)
{
    g_autoptr(FlMethodResponse) response = nullptr;
    if( Args->result != NULL ){
        response = FL_METHOD_RESPONSE(fl_method_success_response_new(Args->result));    
        fl_method_call_respond(Args->method_call, response, nullptr);
        fl_value_unref(Args->result);
    }else{
        g_autoptr(FlValue) result = fl_value_new_null();
        response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
        fl_method_call_respond(Args->method_call, response, nullptr);
    }
    g_object_unref(Args->method_call);
    free(Args);
    return false;
}
#endif

struct StructOfCoreThreadArgs{
    FlMethodCall *method_call;
    StarCoreWaitResult *WaitResult;
};

static VS_HANDLE hDllInstance = NULL;
static VSCore_RegisterCallBackInfoProc RegisterCallBackInfoProc;
static VSCore_UnRegisterCallBackInfoProc UnRegisterCallBackInfoProc;
static VSCore_InitProc VSInitProc;
static VSCore_TermExProc VSTermExProc;
static VSCore_QueryControlInterfaceProc QueryControlInterfaceProc;

static FlValue* handleMethodCall_Do(FlMethodCall *arguments);

static 	VS_THREADID hCoreTimerThreadId;
static 	VS_HANDLE hCoreTimerThreadHandle;
static VS_ULONG VSTHREADAPI Core_Timer_Thread(struct StructOfCoreThreadArgs* method_call)
{
    while (true) {
        vs_mutex_lock(&mutexObject);
        if (ExitAppFlag == true) {
            vs_mutex_unlock(&mutexObject);
            for (VS_INT32 i = 0; i < MAX_STARTHREAD_NUMBER; i++) {
                if (StarThreadWorker[i] != NULL) {
                    StarflutMessage* message = initStarflutMessage(starcore_ThreadTick_Exit);
                    sendStarMessage(StarThreadWorker[i], message);
                }
            }
            /*--need not wait to exit*/
            //vs_thread_join(hCoreThreadHandle);
            break;
        }
        vs_mutex_unlock(&mutexObject);
        vs_thread_sleep(10);
        StarflutMessage* message = initStarflutMessage(starcore_ThreadTick_MessageID);
        sendStarMessage(StarThreadWorker[0], message);
    }
    return 0;
}

static void getExePath(char *ModuleName,int Size)
{
	vs_memset(ModuleName,0,Size);
  readlink( "/proc/self/exe", ModuleName, Size );
}

static VS_ULONG VSTHREADAPI Core_Thread(struct StructOfCoreThreadArgs *Call_Args)
{
    char ModuleName[512];
    getExePath(ModuleName,512);
    char* CharPtr = vs_file_strrchr(ModuleName, '/');
    (*CharPtr) = 0;
    strcat(ModuleName, "/lib/starcore/libstarcore.so");
    hDllInstance = vs_dll_open(ModuleName);
    if (hDllInstance == NULL) {
#if defined(ENV_M64)
        printf("load library [%s] 64bit version for %s error....\n", ModuleName, VS_OSALIAS);
#else
        printf("load library [%s] 32bit version for %s error....\n", ModuleName, VS_OSALIAS);
#endif
        FlValue *result = fl_value_new_null();
#if defined(WAITFORCALLRESULT)         
        Call_Args->WaitResult->SetResult(result);
#else  
        /*--need send response in main thread--*/
        struct StructOfMainThreadTimerHandlerArgs *Args = (struct StructOfMainThreadTimerHandlerArgs *)malloc(sizeof(struct StructOfMainThreadTimerHandlerArgs));
        Args->result = result;
        Args->method_call = Call_Args->method_call;
        g_timeout_add(0,(GSourceFunc) MainThread_MESSAGE_RESULT_Handler,(gpointer)Args);      
#endif   
    }

    RegisterCallBackInfoProc = (VSCore_RegisterCallBackInfoProc)vs_dll_sym(hDllInstance, VSCORE_REGISTERCALLBACKINFO_NAME);
    UnRegisterCallBackInfoProc = (VSCore_UnRegisterCallBackInfoProc)vs_dll_sym(hDllInstance, VSCORE_UNREGISTERCALLBACKINFO_NAME);
    VSInitProc = (VSCore_InitProc)vs_dll_sym(hDllInstance, VSCORE_INIT_NAME);
    VSTermExProc = (VSCore_TermExProc)vs_dll_sym(hDllInstance, VSCORE_TERMEX_NAME);
    QueryControlInterfaceProc = (VSCore_QueryControlInterfaceProc)vs_dll_sym(hDllInstance, VSCORE_QUERYCONTROLINTERFACE_NAME);

   VS_BOOL Result = VSInitProc(true, true, "", 0, "", 0, NULL);
   if( Result == VSINIT_ERROR ){    
      FlValue *result = fl_value_new_null();
#if defined(WAITFORCALLRESULT)         
      Call_Args->WaitResult->SetResult(result);
#else  
      /*--need send response in main thread--*/
      struct StructOfMainThreadTimerHandlerArgs *Args = (struct StructOfMainThreadTimerHandlerArgs *)malloc(sizeof(struct StructOfMainThreadTimerHandlerArgs));
      Args->result = result;
      Args->method_call = Call_Args->method_call;
      g_timeout_add(0,(GSourceFunc) MainThread_MESSAGE_RESULT_Handler,(gpointer)Args);      
#endif        
   }else{
       SRPControlInterface = (class ClassOfSRPControlInterface*)QueryControlInterfaceProc();
      BasicSRPInterface = SRPControlInterface->QueryBasicInterface(0);

      {
          getExePath(ModuleName,512);
          char* CharPtr = vs_file_strrchr(ModuleName, '/');
          (*CharPtr) = 0;
          strcat(ModuleName, "/lib");
          SRPControlInterface->SetShareLibraryPath(ModuleName);
      }

      /*--init CleObjectMap & starcoreCmdMap --*/
      starcoreCmdMap = BasicSRPInterface->CreateStringIndex(CleObjectID_LENGTH*2,0);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_getDocumentPath",(VS_INT8 *)starcore_getDocumentPath);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_isAndroid",(VS_INT8 *)starcore_isAndroid);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_getResourcePath",(VS_INT8 *)starcore_getResourcePath);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_getPlatform",(VS_INT8 *)starcore_getPlatform);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_loadLibrary",(VS_INT8 *)starcore_loadLibrary);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_setEnv",(VS_INT8 *)starcore_setEnv);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_getEnv",(VS_INT8 *)starcore_getEnv);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_getAssetsPath",(VS_INT8 *)starcore_getAssetsPath);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_typeCheck",(VS_INT8 *)starcore_typeCheck);

      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_init",(VS_INT8 *)starcore_init);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_sRPDispatch",(VS_INT8 *)starcore_sRPDispatch);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_sRPLock",(VS_INT8 *)starcore_sRPLock);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_sRPUnLock",(VS_INT8 *)starcore_sRPUnLock);

      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_rubyInitExt",(VS_INT8 *)starcore_rubyInitExt);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_poplocalframe",(VS_INT8 *)starcore_poplocalframe);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_InitCore",(VS_INT8 *)starcore_InitCore);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_InitSimpleEx",(VS_INT8 *)starcore_InitSimpleEx);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_InitSimple",(VS_INT8 *)starcore_InitSimple);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_GetSrvGroup",(VS_INT8 *)starcore_GetSrvGroup);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_moduleExit",(VS_INT8 *)starcore_moduleExit);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_moduleClear",(VS_INT8 *)starcore_moduleClear);      

      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_regMsgCallBackP",(VS_INT8 *)starcore_regMsgCallBackP);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_setRegisterCode",(VS_INT8 *)starcore_setRegisterCode);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_isRegistered",(VS_INT8 *)starcore_isRegistered);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_setLocale",(VS_INT8 *)starcore_setLocale);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_getLocale",(VS_INT8 *)starcore_getLocale);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_version",(VS_INT8 *)starcore_version);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_getScriptIndex",(VS_INT8 *)starcore_getScriptIndex);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_setScript",(VS_INT8 *)starcore_setScript);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_detachCurrentThread",(VS_INT8 *)starcore_detachCurrentThread);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_coreHandle",(VS_INT8 *)starcore_coreHandle);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_captureScriptGIL",(VS_INT8 *)starcore_captureScriptGIL);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_releaseScriptGIL",(VS_INT8 *)starcore_releaseScriptGIL);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"starcore_setShareLibraryPath",(VS_INT8 *)starcore_setShareLibraryPath);

      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_toString",(VS_INT8 *)StarSrvGroupClass_toString);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_createService",(VS_INT8 *)StarSrvGroupClass_createService);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_getService",(VS_INT8 *)StarSrvGroupClass_getService);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_clearService",(VS_INT8 *)StarSrvGroupClass_clearService);   
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_newParaPkg",(VS_INT8 *)StarSrvGroupClass_newParaPkg);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_newBinBuf",(VS_INT8 *)StarSrvGroupClass_newBinBuf);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_getServicePath",(VS_INT8 *)StarSrvGroupClass_getServicePath);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_setServicePath",(VS_INT8 *)StarSrvGroupClass_setServicePath);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_servicePathIsSet",(VS_INT8 *)StarSrvGroupClass_servicePathIsSet);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_getCurrentPath",(VS_INT8 *)StarSrvGroupClass_getCurrentPath);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_importService",(VS_INT8 *)StarSrvGroupClass_importService);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_clearServiceEx",(VS_INT8 *)StarSrvGroupClass_clearServiceEx);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_runScript",(VS_INT8 *)StarSrvGroupClass_runScript);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_runScriptEx",(VS_INT8 *)StarSrvGroupClass_runScriptEx);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_doFile",(VS_INT8 *)StarSrvGroupClass_doFile);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_doFileEx",(VS_INT8 *)StarSrvGroupClass_doFileEx);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_setClientPort",(VS_INT8 *)StarSrvGroupClass_setClientPort);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_setTelnetPort",(VS_INT8 *)StarSrvGroupClass_setTelnetPort);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_setOutputPort",(VS_INT8 *)StarSrvGroupClass_setOutputPort);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_setWebServerPort",(VS_INT8 *)StarSrvGroupClass_setWebServerPort);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_initRaw",(VS_INT8 *)StarSrvGroupClass_initRaw);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_loadRawModule",(VS_INT8 *)StarSrvGroupClass_loadRawModule);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_getLastError",(VS_INT8 *)StarSrvGroupClass_getLastError);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_getLastErrorInfo",(VS_INT8 *)StarSrvGroupClass_getLastErrorInfo);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_getCorePath",(VS_INT8 *)StarSrvGroupClass_getCorePath);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_getUserPath",(VS_INT8 *)StarSrvGroupClass_getUserPath);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_getLocalIP",(VS_INT8 *)StarSrvGroupClass_getLocalIP);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_getLocalIPEx",(VS_INT8 *)StarSrvGroupClass_getLocalIPEx);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_getObjectNum",(VS_INT8 *)StarSrvGroupClass_getObjectNum);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_activeScriptInterface",(VS_INT8 *)StarSrvGroupClass_activeScriptInterface);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarSrvGroupClass_preCompile",(VS_INT8 *)StarSrvGroupClass_preCompile);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_toString",(VS_INT8 *)StarServiceClass_toString);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_get",(VS_INT8 *)StarServiceClass_get);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_getObject",(VS_INT8 *)StarServiceClass_getObject);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_getObjectEx",(VS_INT8 *)StarServiceClass_getObjectEx);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_newObject",(VS_INT8 *)StarServiceClass_newObject);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_runScript",(VS_INT8 *)StarServiceClass_runScript);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_runScriptEx",(VS_INT8 *)StarServiceClass_runScriptEx);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_doFile",(VS_INT8 *)StarServiceClass_doFile);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_doFileEx",(VS_INT8 *)StarServiceClass_doFileEx);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_isServiceRegistered",(VS_INT8 *)StarServiceClass_isServiceRegistered);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_checkPassword",(VS_INT8 *)StarServiceClass_checkPassword);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_newRawProxy",(VS_INT8 *)StarServiceClass_newRawProxy);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_importRawContext",(VS_INT8 *)StarServiceClass_importRawContext);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_getLastError",(VS_INT8 *)StarServiceClass_getLastError);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_getLastErrorInfo",(VS_INT8 *)StarServiceClass_getLastErrorInfo);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_allObject",(VS_INT8 *)StarServiceClass_allObject);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarServiceClass_restfulCall",(VS_INT8 *)StarServiceClass_restfulCall);      

      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_toString",(VS_INT8 *)StarParaPkgClass_toString);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_GetNumber",(VS_INT8 *)StarParaPkgClass_GetNumber);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_get",(VS_INT8 *)StarParaPkgClass_get);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_clear",(VS_INT8 *)StarParaPkgClass_clear);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_appendFrom",(VS_INT8 *)StarParaPkgClass_appendFrom);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_set",(VS_INT8 *)StarParaPkgClass_set);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_build",(VS_INT8 *)StarParaPkgClass_build);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_free",(VS_INT8 *)StarParaPkgClass_free);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_dispose",(VS_INT8 *)StarParaPkgClass_dispose);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_releaseOwner",(VS_INT8 *)StarParaPkgClass_releaseOwner);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_asDict",(VS_INT8 *)StarParaPkgClass_asDict);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_isDict",(VS_INT8 *)StarParaPkgClass_isDict);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_fromJSon",(VS_INT8 *)StarParaPkgClass_fromJSon);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_toJSon",(VS_INT8 *)StarParaPkgClass_toJSon);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_toTuple",(VS_INT8 *)StarParaPkgClass_toTuple);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_equals",(VS_INT8 *)StarParaPkgClass_equals);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarParaPkgClass_V",(VS_INT8 *)StarParaPkgClass_V);      

      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_toString",(VS_INT8 *)StarBinBufClass_toString);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_GetOffset",(VS_INT8 *)StarBinBufClass_GetOffset);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_init",(VS_INT8 *)StarBinBufClass_init);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_clear",(VS_INT8 *)StarBinBufClass_clear);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_saveToFile",(VS_INT8 *)StarBinBufClass_saveToFile);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_loadFromFile",(VS_INT8 *)StarBinBufClass_loadFromFile);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_read",(VS_INT8 *)StarBinBufClass_read);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_write",(VS_INT8 *)StarBinBufClass_write);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_free",(VS_INT8 *)StarBinBufClass_free);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_dispose",(VS_INT8 *)StarBinBufClass_dispose);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_releaseOwner",(VS_INT8 *)StarBinBufClass_releaseOwner);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_setOffset",(VS_INT8 *)StarBinBufClass_setOffset);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_print",(VS_INT8 *)StarBinBufClass_print);
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarBinBufClass_asString",(VS_INT8 *)StarBinBufClass_asString);

      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_toString",(VS_INT8 *)StarObjectClass_toString);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_get",(VS_INT8 *)StarObjectClass_get);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_set",(VS_INT8 *)StarObjectClass_set);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_call",(VS_INT8 *)StarObjectClass_call);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_newObject",(VS_INT8 *)StarObjectClass_newObject);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_free",(VS_INT8 *)StarObjectClass_free);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_dispose",(VS_INT8 *)StarObjectClass_dispose);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_hasRawContext",(VS_INT8 *)StarObjectClass_hasRawContext);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_rawToParaPkg",(VS_INT8 *)StarObjectClass_rawToParaPkg);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_getSourceScript",(VS_INT8 *)StarObjectClass_getSourceScript);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_getLastError",(VS_INT8 *)StarObjectClass_getLastError);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_getLastErrorInfo",(VS_INT8 *)StarObjectClass_getLastErrorInfo);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_releaseOwnerEx",(VS_INT8 *)StarObjectClass_releaseOwnerEx);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_regScriptProcP",(VS_INT8 *)StarObjectClass_regScriptProcP);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_instNumber",(VS_INT8 *)StarObjectClass_instNumber);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_changeParent",(VS_INT8 *)StarObjectClass_changeParent);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_jsonCall",(VS_INT8 *)StarObjectClass_jsonCall);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_active",(VS_INT8 *)StarObjectClass_active);      
      BasicSRPInterface->InsertStringKey(starcoreCmdMap,(VS_CHAR *)"StarObjectClass_deActive",(VS_INT8 *)StarObjectClass_deActive);

      CleObjectMap = BasicSRPInterface->CreateStringIndex(CleObjectID_LENGTH * 2,0);
      g_WaitResultMap = BasicSRPInterface->CreateIndex(1,0);

      VS_UUID ObjectID;
      SRPControlInterface->CreateUuid(&ObjectID);

      VS_CHAR CleObjectID[64];
      sprintf(CleObjectID,"%s%s",StarCorePrefix,SRPControlInterface->UuidToString(&ObjectID));
      FlValue *result = fl_value_new_string(CleObjectID);

#if defined(WAITFORCALLRESULT)   
      Call_Args->WaitResult->SetResult(result);
#else   
      /*--need send response in main thread--*/
      struct StructOfMainThreadTimerHandlerArgs *Args = (struct StructOfMainThreadTimerHandlerArgs *)malloc(sizeof(struct StructOfMainThreadTimerHandlerArgs));
      Args->result = result;
      Args->method_call = Call_Args->method_call;
      g_timeout_add(0,(GSourceFunc) MainThread_MESSAGE_RESULT_Handler,(gpointer)Args);     
#endif      

      SRPControlInterface->SRPUnLock();

      VS_THREADID hThreadIDTemp = vs_thread_currentid();
      CreateStarThreadWorker(hThreadIDTemp);
      struct StructOfStarThreadWorker* ThreadWorker = GetStarThreadWorkerCurrent();

      /*--create timer thread--*/
      hCoreTimerThreadHandle = vs_thread_create((vs_thread_routineproc)Core_Timer_Thread, (void*)NULL, &hCoreTimerThreadId);

      /*--enter loop*/
      while (true) {
          StarflutMessage* message = getStarMessage(ThreadWorker);
          switch (message->MsgClass) {
          case starcore_ThreadTick_MessageID:
          {
              if (SRPControlInterface == NULL || ModuleInitFlag == VS_FALSE )
                  break;
              SRPControlInterface->SRPLock();
              while (SRPControlInterface->SRPDispatch(VS_FALSE) == VS_TRUE);
              SRPControlInterface->SRPUnLock();
          }
          break;
          case starcore_ThreadTick_MethodCall:
          {
              /*SetStarThreadWorkerBusy(VS_TRUE);  need not, the caller will queue*/
              vs_mutex_lock(&starCoreThreadCallDeepSyncObject);
              starCoreThreadCallDeep++;
              vs_mutex_unlock(&starCoreThreadCallDeepSyncObject);
              SRPControlInterface->SRPLock();
              const VS_CHAR* method = fl_method_call_get_name(message->call);
              FlValue *result_value_do = handleMethodCall_Do(message->call);
              if( strcmp(method,"starcore_moduleExit") != 0 )
                  SRPControlInterface->SRPUnLock();
              vs_mutex_lock(&starCoreThreadCallDeepSyncObject);
              starCoreThreadCallDeep--;
              vs_mutex_unlock(&starCoreThreadCallDeepSyncObject);

#if defined(WAITFORCALLRESULT)   
              message->WaitResult->SetResult(result_value_do);
#else   
              /*--need send response in main thread--*/
              struct StructOfMainThreadTimerHandlerArgs* Return_Args = (struct StructOfMainThreadTimerHandlerArgs*)malloc(sizeof(struct StructOfMainThreadTimerHandlerArgs));
              Return_Args->result = result_value_do;
              Return_Args->method_call = message->call;
              g_timeout_add(0, (GSourceFunc)MainThread_MESSAGE_RESULT_Handler, (gpointer)Return_Args);
#endif
              /*SetStarThreadWorkerBusy(VS_FALSE);*/
          }
          break;
          case starcore_ThreadTick_Exit:
          {
              vs_mutex_lock(&ThreadWorker->ThreadExitCond.mutex);
              vs_cond_signal(&ThreadWorker->ThreadExitCond);
              vs_mutex_unlock(&ThreadWorker->ThreadExitCond.mutex);
              g_free(message);
              free(Call_Args);
              return 0;
          }
          }
          g_free(message);
      }
   }
   free(Call_Args);
   return 0;
}

void starflut_plugin_common_handle_method_call_direct(
    FlPluginRegistrar* registrar,
    FlMethodChannel* channel,
    const VS_CHAR *method,
    FlMethodCall* method_call) {       

  g_autoptr(FlMethodResponse) response = nullptr;

   if (strcmp(method, "starcore_typeCheck") == 0) {
        FlValue* args = fl_method_call_get_args(method_call);
        g_autoptr(FlValue) ResultValue = fl_value_new_list();
        
        for (size_t i = 0; i < fl_value_get_length(args); i++) {
            FlValue *item = fl_value_get_list_value(args, i);
            if (IsBool(item) == VS_TRUE) {
                FlValue* value = fromBool(toBool(item));
                fl_value_append_take(ResultValue, value);
            }else if (IsInt32(item) == VS_TRUE) {
                FlValue* value = fromInt32(toInt32(item));
                fl_value_append_take(ResultValue, value);
            }else if (IsInt64(item) == VS_TRUE) {
                FlValue* value = fromInt64(toInt64(item));
                fl_value_append_take(ResultValue, value);
            }else if (IsDouble(item) == VS_TRUE) {
                FlValue* value = fromDouble(toDouble(item));
                fl_value_append_take(ResultValue, value);
            }else if (IsString(item) == VS_TRUE) {
                FlValue* value = fromString(toString(item));
                fl_value_append_take(ResultValue, value);
            }
            else {
                fl_value_append_take(ResultValue, fl_value_new_null());
            }
        }
        response = FL_METHOD_RESPONSE(fl_method_success_response_new(ResultValue));
   }else  if (strcmp(method, "starcore_getDocumentPath") == 0 ) {
    g_autoptr(FlValue) result = fl_value_new_string("");
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  }else if (strcmp(method, "starcore_isAndroid") == 0 ) {
    g_autoptr(FlValue) result = fl_value_new_bool(VS_FALSE);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  }else if (strcmp(method, "starcore_getPlatform") == 0 ) {
    g_autoptr(FlValue) result = fl_value_new_int(3);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  }else if (strcmp(method, "starcore_loadLibrary") == 0 ) {
      FlValue* args = fl_method_call_get_args(method_call);
      VS_CHAR* Name = toString(args);
      VS_CHAR* ErrorInfo = NULL;
      VS_HANDLE Handle = vs_dll_openex(Name, 0, &ErrorInfo);
      if (Handle == 0) {
          if(ErrorInfo != NULL )
              printf("%s\n", ErrorInfo);
           g_autoptr(FlValue) result = fromBool(VS_FALSE);
           response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
      }
      else{
          g_autoptr(FlValue) result = fromBool(VS_TRUE);
          response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
      }
  }else if (strcmp(method, "starcore_setEnv") == 0 ) {
      FlValue* args = fl_method_call_get_args(method_call);
      VS_CHAR* Name = toString(fl_value_get_list_value(args, 0));
      VS_CHAR* Value = toString(fl_value_get_list_value(args, 1));
      VS_BOOL Result = vs_set_env(Name, Value);
      g_autoptr(FlValue) result = fromBool(Result);
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  }else if (strcmp(method, "starcore_getEnv") == 0 ) {
      FlValue* args = fl_method_call_get_args(method_call);
      VS_CHAR *Name = toString(args);
      VS_CHAR LocalBuf[4096];
      LocalBuf[0] = 0;
      VS_BOOL Result = vs_get_env(Name, LocalBuf, 4096);
      if (Result == VS_FALSE){
          g_autoptr(FlValue) result = fromString("");
          response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
      }else{
          g_autoptr(FlValue) result = fromString(LocalBuf);
          response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
      }
  }else if (strcmp(method, "starcore_getResourcePath") == 0 ) {
      char ModuleName[512];
      getExePath(ModuleName,512);
      char* CharPtr = vs_file_strrchr(ModuleName, '/');
      (*CharPtr) = 0;
      strcat(ModuleName, "/lib");
      g_autoptr(FlValue) result = fromString(ModuleName);
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  }else if (strcmp(method, "starcore_getAssetsPath") == 0 ) {
      char ModuleName[512];
      getExePath(ModuleName,512);
      char* CharPtr = vs_file_strrchr(ModuleName, '/');
      (*CharPtr) = 0;
      strcat(ModuleName, "/data");
      g_autoptr(FlValue) result = fromString(ModuleName);
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  }else if (strcmp(method, "starcore_init") == 0 ) {
   /*---create a new thread, and, init cle in the new thread*/
   struct StructOfCoreThreadArgs *Call_Args;
   Call_Args = (struct StructOfCoreThreadArgs*)malloc(sizeof(struct StructOfCoreThreadArgs));

#if defined(WAITFORCALLRESULT)      
   StarCoreWaitResult *WaitResult = new StarCoreWaitResult();
   Call_Args->WaitResult = WaitResult;
   Call_Args->method_call = method_call;
   vs_mutex_lock(&WaitResult->cond.mutex);
#else
   Call_Args->method_call = method_call;
   g_object_ref((GObject *)method_call);  /*hold the referencce*/
#endif   
   hCoreThreadHandle = vs_thread_create((vs_thread_routineproc)Core_Thread, (void *)Call_Args, &hCoreThreadId);
#if defined(WAITFORCALLRESULT)     
   FlValue *result = WaitResult->WaitResult();
   vs_mutex_unlock(&WaitResult->cond.mutex);
   delete WaitResult;
   FlMethodSuccessResponse* res = fl_method_success_response_new(result);
   fl_value_unref(result);
   response = FL_METHOD_RESPONSE(res);
#else   
   return;
#endif   
  }else if (strcmp(method, "starcore_sRPDispatch") == 0) {
    SRPControlInterface -> SRPLock();
    VS_BOOL Result = SRPControlInterface -> SRPDispatch(toBool(fl_method_call_get_args(method_call)));
    SRPControlInterface -> SRPUnLock();
    g_autoptr(FlValue) result = fl_value_new_bool(Result);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  }else if (strcmp(method, "starcore_sRPLock") == 0 ) {
    SRPControlInterface -> SRPLock();
    g_autoptr(FlValue) result = fl_value_new_null();
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  }else if (strcmp(method, "starcore_sRPUnLock") == 0  ) {
    SRPControlInterface -> SRPUnLock();
    g_autoptr(FlValue) result = fl_value_new_null();
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  }
  fl_method_call_respond(method_call, response, nullptr); 
}

static VS_ULONG VSTHREADAPI Core_Worker_Thread(void* Call_Args)
{
    struct StructOfStarThreadWorker* ThreadWorker = GetStarThreadWorkerCurrent();

    /*--enter loop*/
    while (true) {
        StarflutMessage* message = getStarMessage(ThreadWorker);
        switch (message->MsgClass) {
        case starcore_ThreadTick_MethodCall:
        {
            /*SetStarThreadWorkerBusy(VS_TRUE);  need not, the caller will queue*/
            vs_mutex_lock(&starCoreThreadCallDeepSyncObject);
            starCoreThreadCallDeep++;
            vs_mutex_unlock(&starCoreThreadCallDeepSyncObject);
            SRPControlInterface->SRPLock();
            const VS_CHAR* method = fl_method_call_get_name(message->call);
            FlValue* result_value_do = handleMethodCall_Do(message->call);
            if (strcmp(method, "starcore_moduleExit") != 0)
                SRPControlInterface->SRPUnLock();
            vs_mutex_lock(&starCoreThreadCallDeepSyncObject);
            starCoreThreadCallDeep--;
            vs_mutex_unlock(&starCoreThreadCallDeepSyncObject);

#if defined(WAITFORCALLRESULT)   
            message->WaitResult->SetResult(result_value_do);
#else   
            /*--need send response in main thread--*/
            struct StructOfMainThreadTimerHandlerArgs* Return_Args = (struct StructOfMainThreadTimerHandlerArgs*)malloc(sizeof(struct StructOfMainThreadTimerHandlerArgs));
            Return_Args->result = result_value_do;
            Return_Args->method_call = message->call;
            g_timeout_add(0, (GSourceFunc)MainThread_MESSAGE_RESULT_Handler, (gpointer)Return_Args);
#endif
            /*SetStarThreadWorkerBusy(VS_FALSE);*/
        }
        break;
        case starcore_ThreadTick_Exit:
        {
            SRPControlInterface->DetachCurrentThread();
            vs_mutex_lock(&ThreadWorker->ThreadExitCond.mutex);
            vs_cond_signal(&ThreadWorker->ThreadExitCond);
            vs_mutex_unlock(&ThreadWorker->ThreadExitCond.mutex);
            free(message);
            return 0;
        }
        }
        free(message);
    }
    return 0;
}


void starflut_plugin_common_handle_method_call(
    FlPluginRegistrar* registrar,
    FlMethodChannel* channel,
    FlMethodCall* method_call) {
     
  const VS_CHAR* method = fl_method_call_get_name(method_call);
  if (strcmp(method, "starcore_getDocumentPath") == 0 || strcmp(method, "starcore_isAndroid") == 0 || strcmp(method, "starcore_getResourcePath") == 0 || strcmp(method, "starcore_getAssetsPath") == 0 ||
      strcmp(method, "starcore_init") == 0 || strcmp(method, "starcore_sRPDispatch") == 0 || strcmp(method, "starcore_sRPLock") == 0 || strcmp(method, "starcore_sRPUnLock") == 0 || strcmp(method, "starcore_getPlatform") == 0 ||
      strcmp(method, "starcore_loadLibrary") == 0 || strcmp(method, "starcore_setEnv") == 0 || strcmp(method, "starcore_getEnv") == 0 || strcmp(method, "starcore_typeCheck") == 0 ) {
      starflut_plugin_common_handle_method_call_direct(registrar,channel,method,method_call);
  } else {
      /* in some case, for thread switching, starCoreThreadCallDeep may be not 0, so remove it */
      /*
      vs_mutex_lock(&starCoreThreadCallDeepSyncObject);
      if (starCoreThreadCallDeep != 0) {
          printf("call starflut function [%s]  may be error, current is in starcore thread process", method);
      }
      vs_mutex_unlock(&starCoreThreadCallDeepSyncObject);
      */
#if defined(WAITFORCALLRESULT)  
      g_autoptr(FlMethodResponse) response = nullptr;
      g_autoptr(FlValue) result_value = handleMethodCall_Do( method_call);
      if (result_value == NULL) {
          result_value = fl_value_new_null();
          response = FL_METHOD_RESPONSE(fl_method_success_response_new(result_value));
      }
      else {
          response = FL_METHOD_RESPONSE(fl_method_success_response_new(result_value));
      }
      fl_method_call_respond(method_call, response, nullptr); `
#else
      if (strcmp(method, "starcore_moduleExit") == 0 || strcmp(method, "starcore_moduleClear") == 0) {
          g_object_ref((GObject*)method_call);  /*hold the referencce*/
          StarflutMessage* message = initStarflutMessage(starcore_ThreadTick_MethodCall, method_call);
          sendStarMessage(StarThreadWorker[0], message);
      }
      else {
          struct StructOfStarThreadWorker* ThreadWorker = GetStarThreadWorker();
          if (ThreadWorker != NULL) {
              g_object_ref((GObject*)method_call);  /*hold the referencce*/
              StarflutMessage* message = initStarflutMessage(starcore_ThreadTick_MethodCall, method_call);
              sendStarMessage(ThreadWorker, message);
          }
          else if (CanCreateStarThreadWorker() == VS_TRUE) {
              /*--create a new worker*/
              VS_THREADID hCoreWorkerThreadId;
              vs_thread_create((vs_thread_routineproc)Core_Worker_Thread, (void*)NULL, &hCoreWorkerThreadId);
              CreateStarThreadWorker(hCoreWorkerThreadId);

              struct StructOfStarThreadWorker* ThreadWorker_Local = GetStarThreadWorker();
              g_object_ref((GObject*)method_call);  /*hold the referencce*/
              StarflutMessage* message = initStarflutMessage(starcore_ThreadTick_MethodCall, method_call);
              sendStarMessage(ThreadWorker_Local, message);
          }
          else {
              g_autoptr(FlValue) result = fl_value_new_null();
              fl_method_call_respond_error(method_call, "-1", "Can not create worker thread", result, NULL);
          }
      }
      return;
#endif    
  }  
}

/*return NULL of EncodableValue, the EncodableValue must be deleted*/
static FlValue* handleMethodCall_Do(FlMethodCall* method_call)
{
    const VS_CHAR* method = fl_method_call_get_name(method_call);
    FlValue *arguments = fl_method_call_get_args(method_call);
    VS_INT32 CallIndex = (VS_INT32)(VS_UWORD)BasicSRPInterface->FindStringKey(starcoreCmdMap,(VS_CHAR *)method);
    switch (CallIndex) {
    case starcore_poplocalframe:
    {
        FlValue* args = fl_method_call_get_args(method_call);
        removeFromObjectMapByKey(args);
        return NULL;
    }
    case starcore_InitCore:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        SRPControlInterface->SRPLock();
        /*VS_BOOL ServerFlag, VS_BOOL SRPPrintFlag, const VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, const VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient, VS_STARCONFIGEX *ConfigEx*/
        VS_BOOL ServerFlag = toBool(fl_value_get_list_value(plist,0));
        VS_BOOL SRPPrintFlag = toBool(fl_value_get_list_value(plist,3));
        VS_CHAR* DebugInterface = toString(fl_value_get_list_value(plist,4));
        VS_UINT16 PortNumberForDebug = (VS_UINT16)toInt32(fl_value_get_list_value(plist,5));
        VS_CHAR* ClientInterface = (VS_CHAR*)toString(fl_value_get_list_value(plist,6));
        VS_UINT16 PortNumberForDirectClient = (VS_UINT16)toInt32(fl_value_get_list_value(plist,7));

        VS_INT32 Result = VSInitProc(ServerFlag, SRPPrintFlag, DebugInterface, PortNumberForDebug, ClientInterface, PortNumberForDirectClient, NULL);
        if (Result >= 0) {
            ModuleInitFlag = true;
            /*--register dispatch request--*/
            SRPControlInterface->RegDispatchRequest(Starflut_SRPDispatchRequestCallBack, 0);
        }
        SRPControlInterface->SRPUnLock();
        return fromInt32(Result);
    }
    case starcore_InitSimpleEx:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        SRPControlInterface->SRPLock();
        VS_UINT16 PortNumberForDirectClient = (VS_UINT16)toInt32(fl_value_get_list_value(plist,0));
        VS_INT32 Result = VSInitProc(VS_TRUE, VS_TRUE, (VS_CHAR*)"", 0, (VS_CHAR*)"", PortNumberForDirectClient, NULL);
        if (Result < 0) {
            SRPControlInterface->SRPUnLock();
            return NULL;
        }
        BasicSRPInterface = SRPControlInterface->QueryBasicInterface(0);
        if (fl_value_get_length(plist) > 2) {
            for (size_t i = 2; i < fl_value_get_length(plist); i++) {
                VS_CHAR* DependServiceName = (VS_CHAR*)toString(fl_value_get_list_value(plist,i));
                if (vs_string_strlen(DependServiceName) != 0 && BasicSRPInterface->ImportService(DependServiceName, VS_TRUE) == VS_FALSE) {
                    SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "import depend service [%s] fail", DependServiceName);
                    BasicSRPInterface->Release();
                    BasicSRPInterface = NULL;
                    SRPControlInterface->SRPUnLock();
                    return NULL;
                }
            }
        }
        VS_INT32 WebPortNumber = (VS_UINT16)toInt32(fl_value_get_list_value(plist,1));
        if (WebPortNumber != 0)
            BasicSRPInterface->SetWebServerPort("", WebPortNumber, 100, 2048);

        ModuleInitFlag = true;
        /*--register dispatch request--*/
        SRPControlInterface->RegDispatchRequest(Starflut_SRPDispatchRequestCallBack, 0);
        SRPControlInterface->SRPUnLock();

        VS_CHAR CleObjectID_Local[CleObjectID_LENGTH];
        VS_CHAR* CleObjectID = getTagFromObjectMap(BasicSRPInterface);
        if (CleObjectID == NULL) {
            VS_UUID ObjectID;
            CleObjectID = CleObjectID_Local;
            BasicSRPInterface->CreateUuid(&ObjectID);
            sprintf(CleObjectID, "%s%s", StarSrvGroupPrefix, BasicSRPInterface->UuidToString(&ObjectID));
            BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)BasicSRPInterface);
        }
        return fromString(CleObjectID);
    }

    case starcore_InitSimple:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        VS_UINT16 PortNumberForDirectClient = (VS_UINT16)toInt32(fl_value_get_list_value(plist,2));
        SRPControlInterface->SRPLock();
        VS_INT32 Result = VSInitProc(VS_TRUE, VS_TRUE, (VS_CHAR*)"", 0, (VS_CHAR*)"", PortNumberForDirectClient, NULL);
        if (Result < 0) {
            SRPControlInterface->SRPUnLock();
            return NULL;
        }
        const VS_CHAR* ServiceName = (VS_CHAR*)toString(fl_value_get_list_value(plist,0));
        const VS_CHAR* ServicePass = (VS_CHAR*)toString(fl_value_get_list_value(plist,1));
        BasicSRPInterface = SRPControlInterface->QueryBasicInterface(0);
        if (fl_value_get_length(plist) > 4) {
            for (size_t i = 4; i < fl_value_get_length(plist); i++) {
                VS_CHAR* DependServiceName = (VS_CHAR*)toString(fl_value_get_list_value(plist,i));
                if (vs_string_strlen(DependServiceName) != 0 && BasicSRPInterface->ImportService(DependServiceName, VS_TRUE) == VS_FALSE) {
                    SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "import depend service [%s] fail", DependServiceName);
                    BasicSRPInterface->Release();
                    BasicSRPInterface = NULL;
                    SRPControlInterface->SRPUnLock();
                    return NULL;
                }
            }
        }
        if (BasicSRPInterface->CreateService("", ServiceName, NULL, ServicePass, 5, 10240, 10240, 10240, 10240, 10240) == VS_FALSE) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "create service [%s] fail", ServiceName);
            BasicSRPInterface->Release();
            BasicSRPInterface = NULL;
            SRPControlInterface->SRPUnLock();
            return NULL;
        }
        else {
            class ClassOfSRPInterface* l_SRPInterface;

            l_SRPInterface = BasicSRPInterface->GetSRPInterface(ServiceName, "root", ServicePass);
            VS_INT32 WebPortNumber = (VS_INT32)toInt32(fl_value_get_list_value(plist,3));
            if (WebPortNumber != 0)
                BasicSRPInterface->SetWebServerPort("", WebPortNumber, 100, 2048);

            ModuleInitFlag = true;
            /*--register dispatch request--*/
            SRPControlInterface->RegDispatchRequest(Starflut_SRPDispatchRequestCallBack, 0);
            SRPControlInterface->SRPUnLock();

            SRPInterface = l_SRPInterface;
            /*Star_ObjectCBridge_Init(SRPInterface,NULL,NULL);  for macos, need not call this*/

            VS_UUID ObjectID;
            l_SRPInterface->GetServiceID(&ObjectID);
            VS_CHAR* newServiceID = l_SRPInterface->UuidToString(&ObjectID);

            VS_CHAR CleObjectID[CleObjectID_LENGTH];
            sprintf(CleObjectID, "%s%s", StarServicePrefix, newServiceID);
            BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)l_SRPInterface);

            VS_CHAR ResultValue[CleObjectID_LENGTH * 2];
            sprintf(ResultValue, "%s+%s", CleObjectID, newServiceID);
            return fromString(ResultValue);
        }
    }

       case starcore_GetSrvGroup:
    {
       FlValue* plist = fl_method_call_get_args(method_call);
        if (IsNull(plist) == VS_TRUE || fl_value_get_length(plist) == 0) {
            if (SRPControlInterface == NULL || BasicSRPInterface == NULL)
                return NULL;
            VS_CHAR* CleObjectID = getTagFromObjectMap(BasicSRPInterface);
            VS_CHAR CleObjectID_Local[CleObjectID_LENGTH];
            if (CleObjectID == NULL) {
                CleObjectID = CleObjectID_Local;
                VS_UUID ObjectID;
                BasicSRPInterface->CreateUuid(&ObjectID);
                sprintf(CleObjectID, "%s%s", StarSrvGroupPrefix, BasicSRPInterface->UuidToString(&ObjectID));
                BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)BasicSRPInterface);
            }
            return fromString(CleObjectID);
        }
        else {
            if (SRPControlInterface == NULL)
                return NULL;
            FlValue* ServiceName_GroupID = fl_value_get_list_value(plist,0);
            if (IsInt32(ServiceName_GroupID) == VS_TRUE || IsInt64(ServiceName_GroupID) == VS_TRUE) {
                int ServiceGroupID = toInt32(ServiceName_GroupID);
                if (ServiceGroupID == 0) {
                    if (BasicSRPInterface == NULL)
                        return NULL;
                    VS_CHAR* CleObjectID = getTagFromObjectMap(BasicSRPInterface);
                    VS_CHAR CleObjectID_Local[CleObjectID_LENGTH];
                    if (CleObjectID == NULL) {
                        CleObjectID = CleObjectID_Local;
                        VS_UUID ObjectID;
                        BasicSRPInterface->CreateUuid(&ObjectID);
                        sprintf(CleObjectID, "%s%s", StarSrvGroupPrefix, BasicSRPInterface->UuidToString(&ObjectID));
                        BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)BasicSRPInterface);
                    }
                    return fromString(CleObjectID);
                }
                else {
                    SRPControlInterface->SRPLock();
                    class ClassOfBasicSRPInterface* l_BasicSRPInterface = SRPControlInterface->QueryBasicInterface(ServiceGroupID);
                    if (l_BasicSRPInterface == NULL) {
                        SRPControlInterface->SRPUnLock();
                        return NULL;
                    }
                    if (l_BasicSRPInterface->GetServiceGroupID() == 0) {
                        if (BasicSRPInterface == NULL) {
                            BasicSRPInterface = l_BasicSRPInterface;
                        }
                        else {
                            l_BasicSRPInterface->Release();
                        }
                        SRPControlInterface->SRPUnLock();
                        VS_CHAR* CleObjectID = getTagFromObjectMap(BasicSRPInterface);
                        VS_CHAR CleObjectID_Local[CleObjectID_LENGTH];
                        if (CleObjectID == NULL) {
                            CleObjectID = CleObjectID_Local;
                            VS_UUID ObjectID;
                            BasicSRPInterface->CreateUuid(&ObjectID);
                            sprintf(CleObjectID, "%s%s", StarSrvGroupPrefix, BasicSRPInterface->UuidToString(&ObjectID));
                            BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)BasicSRPInterface);
                        }
                        return fromString(CleObjectID);
                    }
                    else {
                        SRPControlInterface->SRPUnLock();
                        VS_CHAR* CleObjectID = getTagFromObjectMap(l_BasicSRPInterface);
                        VS_CHAR CleObjectID_Local[CleObjectID_LENGTH];
                        if (CleObjectID == NULL) {
                            CleObjectID = CleObjectID_Local;
                            VS_UUID ObjectID;
                            l_BasicSRPInterface->CreateUuid(&ObjectID);
                            sprintf(CleObjectID, "%s%s", StarSrvGroupPrefix, l_BasicSRPInterface->UuidToString(&ObjectID));
                            l_BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)l_BasicSRPInterface);
                        }
                        return fromString(CleObjectID);
                    }
                }
            }
            else if (IsString(ServiceName_GroupID) == VS_TRUE) {
                VS_CHAR* ServiceName, * ActiveServiceName;
                VS_UUID ServiceID;
                VS_ULONG ServiceGroupID;

                SRPControlInterface->SRPLock();
                ServiceName = toString(ServiceName_GroupID);
                ServiceGroupID = SRPControlInterface->QueryFirstServiceGroup();
                class ClassOfBasicSRPInterface* l_BasicSRPInterface = NULL;
                while (ServiceGroupID != VS_INVALID_SERVICEGROUPID) {
                    l_BasicSRPInterface = SRPControlInterface->QueryBasicInterface(ServiceGroupID);
                    if (l_BasicSRPInterface == NULL) {
                        SRPControlInterface->SRPUnLock();
                        return NULL;
                    }
                    ActiveServiceName = l_BasicSRPInterface->QueryActiveService(&ServiceID);
                    if (ActiveServiceName != NULL && vs_string_strcmp(ActiveServiceName, ServiceName) == 0)
                        break;
                    l_BasicSRPInterface->Release();
                    ServiceGroupID = SRPControlInterface->QueryNextServiceGroup();
                }
                if (ServiceGroupID == VS_INVALID_SERVICEGROUPID) {
                    if (l_BasicSRPInterface != NULL)
                        l_BasicSRPInterface->Release();
                    SRPControlInterface->SRPUnLock();
                    return NULL;
                }
                if (l_BasicSRPInterface->GetServiceGroupID() == 0) {
                    if (BasicSRPInterface == NULL) {
                        BasicSRPInterface = l_BasicSRPInterface;
                    }
                    else {
                        l_BasicSRPInterface->Release();
                    }
                    SRPControlInterface->SRPUnLock();
                    VS_CHAR* CleObjectID = getTagFromObjectMap(BasicSRPInterface);
                    VS_CHAR CleObjectID_Local[CleObjectID_LENGTH];
                    if (CleObjectID == NULL) {
                        CleObjectID = CleObjectID_Local;
                        VS_UUID ObjectID;
                        BasicSRPInterface->CreateUuid(&ObjectID);
                        sprintf(CleObjectID, "%s%s", StarSrvGroupPrefix, BasicSRPInterface->UuidToString(&ObjectID));
                        BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)BasicSRPInterface);
                    }
                    return fromString(CleObjectID);
                }
                else {
                    SRPControlInterface->SRPUnLock();
                    VS_CHAR* CleObjectID = getTagFromObjectMap(l_BasicSRPInterface);
                    VS_CHAR CleObjectID_Local[CleObjectID_LENGTH];
                    if (CleObjectID == NULL) {
                        CleObjectID = CleObjectID_Local;
                        VS_UUID ObjectID;
                        l_BasicSRPInterface->CreateUuid(&ObjectID);
                        sprintf(CleObjectID, "%s%s", StarSrvGroupPrefix, l_BasicSRPInterface->UuidToString(&ObjectID));
                        l_BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)l_BasicSRPInterface);
                    }
                    return fromString(CleObjectID);
                }
            }
            else {
                return NULL;
            }
        }
    }

    case starcore_moduleExit :
    {
        if (ModuleInitFlag == VS_TRUE) {
            for (VS_INT32 i = 1; i < MAX_STARTHREAD_NUMBER; i++) {
                if (StarThreadWorker[i] != NULL) {
                    vs_mutex_lock(&StarThreadWorker[i]->ThreadExitCond.mutex);
                }
            }
            vs_mutex_lock(&mutexObject);
            ExitAppFlag = true;
            vs_mutex_unlock(&mutexObject);
            for (VS_INT32 i = 1; i < MAX_STARTHREAD_NUMBER; i++) {
                if (StarThreadWorker[i] != NULL) {
                    vs_cond_wait_local(&StarThreadWorker[i]->ThreadExitCond);
                }
            }

            for (VS_INT32 i = 1; i < MAX_STARTHREAD_NUMBER; i++) {
                if (StarThreadWorker[i] != NULL) {
                    vs_mutex_unlock(&StarThreadWorker[i]->ThreadExitCond.mutex);
                }
            }
            /*--need not wait to exit*/
            //vs_thread_join(hCoreTimerThreadHandle);
            removeFromObjectMap(NULL);

            BasicSRPInterface->ClearService();
            BasicSRPInterface->Release();
            BasicSRPInterface = NULL;

            SRPControlInterface->UnRegDispatchRequest(Starflut_SRPDispatchRequestCallBack, 0);

            UnRegisterCallBackInfoProc(GlobalMsgCallBack, 0);
            VSTermExProc();
            ModuleInitFlag = VS_FALSE;
        }
        /* can not delete hMainWnd, this 
        if (hMainWnd != NULL) {
            ::DestroyWindow(hMainWnd);
            hMainWnd = NULL;
        }
        */
        if (hDllInstance != NULL) {
            vs_dll_close(hDllInstance);
            hDllInstance = NULL;
        }
        return NULL;
    }
    
    case starcore_moduleClear:
    {
        removeFromObjectMap(NULL);

        BasicSRPInterface->ClearService();

        SRPControlInterface->UnRegDispatchRequest(Starflut_SRPDispatchRequestCallBack, 0);

        if (SRPControlInterface != NULL)
            while (SRPControlInterface->SRPDispatch(VS_FALSE) == VS_TRUE);
        return NULL;
    }
    case starcore_regMsgCallBackP:
    {
        if (toBool(arguments) == VS_FALSE) {
            UnRegisterCallBackInfoProc(GlobalMsgCallBack, 0);
        }
        else {
            RegisterCallBackInfoProc(GlobalMsgCallBack, 0);
        }
        return NULL;
    }

    case starcore_setRegisterCode:
    {
       FlValue* plist = fl_method_call_get_args(method_call);
        VS_BOOL Result = SRPControlInterface->SetRegisterCode(toString(fl_value_get_list_value(plist,0)), toBool(fl_value_get_list_value(plist,1)));
        return fromBool(Result);
    }
    case starcore_isRegistered:
    {
        VS_BOOL Result = SRPControlInterface->IsRegistered();
        return fromBool(Result);
    }
    case starcore_setLocale:
    {
        SRPControlInterface->SetLocale(toString(arguments));
        return fromBool(VS_TRUE);
    }
    case starcore_getLocale:
    {
        VS_CHAR* Lang = SRPControlInterface->GetLocale();
        return fromString(Lang);
    }
    case starcore_version:
    {
        FlValue* ret_value = fl_value_new_list();
        fl_value_append_take(ret_value,fromInt32(VS_MAINVERSION));
        fl_value_append_take(ret_value,fromInt32(VS_SUBVERSION));
        fl_value_append_take(ret_value,fromInt32(VS_BUILDVERSION));
        return ret_value;
    }
    case starcore_getScriptIndex:
    {
        VS_INT32 Index = SRPControlInterface->GetScriptInterfaceIndex(toString(arguments));
        return fromInt32(Index);
    }
    case starcore_setScript:
    {
       FlValue* plist = fl_method_call_get_args(method_call);
        VS_BOOL Result = SRPControlInterface->SetScriptInterface(toString(fl_value_get_list_value(plist,0)), toString(fl_value_get_list_value(plist,1)), toString(fl_value_get_list_value(plist,2)));
        return fromBool(Result);
    }
    case starcore_detachCurrentThread:
    {
        SRPControlInterface->DetachCurrentThread();
        return NULL;
    }
    case starcore_coreHandle:
    {
        return fromInt64(0xFFFFFFFFFFFFFFFFL);
    }
    case starcore_captureScriptGIL:
    {
        SRPControlInterface->CaptureScriptGIL(NULL, NULL);
        return NULL;
    }
    case starcore_releaseScriptGIL:
    {
        SRPControlInterface->ReleaseScriptGIL(NULL, NULL);
        return NULL;
    }
    case starcore_setShareLibraryPath:
    {
        SRPControlInterface->SetShareLibraryPath(toString(arguments));
        return NULL;
    }

   /*-----------------------------------------------*/
    case StarSrvGroupClass_toString:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap,toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        if (l_SrvGroup->QueryActiveService(NULL) == NULL) {
            return fromString("service not load");
        }
        else {
            return fromString(l_SrvGroup->QueryActiveService(NULL));
        }
    }
    case StarSrvGroupClass_createService:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        VS_UUID ObjectID;
        VS_CHAR* ServiceID = toString(fl_value_get_list_value(plist,10));
        if (ServiceID != NULL)
            l_SrvGroup->StringToUuid(ServiceID, &ObjectID);
        else {
            INIT_UUID(ObjectID);
        }
        VS_BOOL Result = l_SrvGroup->CreateService(toString(fl_value_get_list_value(plist,1)), toString(fl_value_get_list_value(plist,2)), &ObjectID, toString(fl_value_get_list_value(plist,3)), toInt32(fl_value_get_list_value(plist,4)), toInt32(fl_value_get_list_value(plist,5)), toInt32(fl_value_get_list_value(plist,6)), toInt32(fl_value_get_list_value(plist,7)), toInt32(fl_value_get_list_value(plist,8)), toInt32(fl_value_get_list_value(plist,9)));
        if (Result == VS_FALSE) {
            return NULL;
        }
        else {
            class ClassOfSRPInterface* l_SRPInterface = l_SrvGroup->GetSRPInterface(toString(fl_value_get_list_value(plist,2)), "root", toString(fl_value_get_list_value(plist,3)));
            SRPInterface = l_SRPInterface;

            /*Star_ObjectCBridge_Init(SRPInterface,NULL,NULL);  for macos, need not call this*/

            l_SRPInterface->GetServiceID(&ObjectID);

            VS_CHAR CleObjectID_Buf[CleObjectID_LENGTH];
            VS_CHAR* CleObjectID = CleObjectID_Buf;
            VS_UUID ObjectIDTemp;
            l_SRPInterface->CreateUuid(&ObjectIDTemp);
            sprintf(CleObjectID,"%s%s", StarServicePrefix, l_SRPInterface->UuidToString(&ObjectIDTemp));
            l_SRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)l_SRPInterface);

            VS_CHAR ResultBuf[CleObjectID_LENGTH*2];
            sprintf(ResultBuf, "%s+%s", CleObjectID, l_SRPInterface->UuidToString(&ObjectID));
            return  fromString(ResultBuf);
        }
    }
    case StarSrvGroupClass_getService:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        VS_UUID ServiceID;
        if (l_SrvGroup->QueryActiveService(&ServiceID) == NULL) {
            return NULL;
        }
        class ClassOfSRPInterface* l_SRPInterface = l_SrvGroup->GetSRPInterfaceEx(&ServiceID, toString(fl_value_get_list_value(plist,1)), toString(fl_value_get_list_value(plist,2)));
        if (l_SRPInterface == NULL)
            return NULL;

        SRPInterface = l_SRPInterface;  /*--save it--*/

        VS_CHAR CleObjectID_Buf[CleObjectID_LENGTH];
        VS_CHAR* CleObjectID = CleObjectID_Buf;
        VS_UUID ObjectIDTemp;
        l_SRPInterface->CreateUuid(&ObjectIDTemp);
        sprintf(CleObjectID, "%s%s", StarServicePrefix, l_SRPInterface->UuidToString(&ObjectIDTemp));
        l_SRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)l_SRPInterface);

        VS_CHAR ResultBuf[CleObjectID_LENGTH * 2];
        sprintf(ResultBuf, "%s+%s", CleObjectID, l_SRPInterface->UuidToString(&ServiceID));
        return  fromString(ResultBuf);
    }
    case StarSrvGroupClass_clearService:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        removeFromObjectMap(NULL);
        l_SrvGroup->ClearService();
        return NULL;
    }
    case StarSrvGroupClass_newParaPkg:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPParaPackageInterface* ParaPkg = l_SrvGroup->GetParaPkgInterface();
        if (IsList(fl_value_get_list_value(plist,1))) {
            FlValue* vv = fl_value_get_list_value(plist,1);
            if (StarParaPkg_FromTuple_Sub(vv, 0, ParaPkg, 0) == VS_FALSE) {
                ParaPkg->Release();
                return NULL;
            }
        }
        else if (IsMap(fl_value_get_list_value(plist,1))) {
            FlValue *newt = fl_value_new_list();
            FlValue* vv = fl_value_get_list_value(plist,1);
            for (size_t iter =0; iter < fl_value_get_length(vv); iter++) {
                fl_value_append(newt,fl_value_get_map_key(vv,iter));
                fl_value_append(newt,fl_value_get_map_value(vv,iter));
            }
            if (StarParaPkg_FromTuple_Sub(newt, 0, ParaPkg, 0) == VS_FALSE) {
                fl_value_unref(newt);
                ParaPkg->Release();
                return NULL;
            }
            fl_value_unref(newt);
            ParaPkg->AsDict(VS_TRUE);
        }
        else {
        }
        VS_UUID ObjectID;
        BasicSRPInterface->CreateUuid(&ObjectID);
        VS_CHAR CleObjectID_Buf[CleObjectID_LENGTH];
        VS_CHAR* CleObjectID = CleObjectID_Buf;
        sprintf(CleObjectID,"%s%s", StarParaPkgPrefix, BasicSRPInterface->UuidToString(&ObjectID));
        BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)ParaPkg);

        return  fromString(CleObjectID);
    }
    case StarSrvGroupClass_newBinBuf:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPBinBufInterface* BinBuf = l_SrvGroup->GetSRPBinBufInterface();
        VS_UUID ObjectID;
        BasicSRPInterface->CreateUuid(&ObjectID);
        VS_CHAR CleObjectID_Buf[CleObjectID_LENGTH];
        VS_CHAR* CleObjectID = CleObjectID_Buf;
        sprintf(CleObjectID, "%s%s", StarBinBufPrefix, BasicSRPInterface->UuidToString(&ObjectID));
        BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)BinBuf);

        return  fromString(CleObjectID);
    }
    case StarSrvGroupClass_getServicePath:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return  fromString("");
        }
        VS_CHAR LocalBuf[512];
        l_SrvGroup->GetDefaultPath(LocalBuf, 512);
        return  fromString(LocalBuf);
    }
    case StarSrvGroupClass_setServicePath:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        l_SrvGroup->SetDefaultPath(toString(fl_value_get_list_value(plist,1)));
        return NULL;
    }
    case StarSrvGroupClass_servicePathIsSet:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        return fromBool(l_SrvGroup->DefaultPathIsSet());
    }
    case StarSrvGroupClass_getCurrentPath:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        VS_CHAR LocalBuf[512];
        vs_dir_getcwd(LocalBuf, 512);
        return fromString(LocalBuf);
    }
    case StarSrvGroupClass_importService:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        VS_BOOL Result = l_SrvGroup->ImportService(toString(fl_value_get_list_value(plist,1)), toBool(fl_value_get_list_value(plist,2)));
        return fromBool(Result);
    }
    case StarSrvGroupClass_clearServiceEx:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        removeFromObjectMap(NULL);
        l_SrvGroup->ClearServiceEx();
        return NULL;
    }
    case StarSrvGroupClass_runScript:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        VS_CHAR* ScriptBuf = toString(fl_value_get_list_value(plist,2));
        VS_BOOL RetResult = l_SrvGroup->DoBuffer(toString(fl_value_get_list_value(plist,1)), ScriptBuf, (VS_INT32)vs_string_strlen(ScriptBuf), VS_FALSE, toString(fl_value_get_list_value(plist,3)));
        return fromBool(RetResult);
    }
    case StarSrvGroupClass_runScriptEx:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,2))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,2)));
            return fromBool(VS_FALSE);
        }
        VS_BOOL RetResult = l_SrvGroup->DoBuffer(toString(fl_value_get_list_value(plist,1)), l_BinBuf->GetBufPtr(0), l_BinBuf->GetOffset(), VS_FALSE, toString(fl_value_get_list_value(plist,3)));
        return fromBool(RetResult);
    }
    case StarSrvGroupClass_doFile:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        VS_BOOL Result = l_SrvGroup->DoFileEx(toString(fl_value_get_list_value(plist,1)), toString(fl_value_get_list_value(plist,2)), NULL, NULL, VS_FALSE, NULL);
        return fromBool(Result);
    }
    case StarSrvGroupClass_doFileEx:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        VS_BOOL Result = l_SrvGroup->DoFileEx(toString(fl_value_get_list_value(plist,1)), toString(fl_value_get_list_value(plist,2)), NULL, NULL, VS_FALSE, toString(fl_value_get_list_value(plist,3)));
        return fromBool(Result);
    }
    case StarSrvGroupClass_setClientPort:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        VS_BOOL RetResult = l_SrvGroup->SetClientPort(toString(fl_value_get_list_value(plist,1)), toInt32(fl_value_get_list_value(plist,2)));
        return fromBool(RetResult);
    }
    case StarSrvGroupClass_setTelnetPort:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        VS_BOOL RetResult = l_SrvGroup->SetTelnetPort(toInt32(fl_value_get_list_value(plist,1)));
        return fromBool(RetResult);
    }
    case StarSrvGroupClass_setOutputPort:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        VS_BOOL RetResult = l_SrvGroup->SetOutputPort(toString(fl_value_get_list_value(plist,1)), toInt32(fl_value_get_list_value(plist,2)));
        return fromBool(RetResult);
    }
    case StarSrvGroupClass_setWebServerPort:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        VS_BOOL RetResult = l_SrvGroup->SetWebServerPort(toString(fl_value_get_list_value(plist,1)), toInt32(fl_value_get_list_value(plist,2)), toInt32(fl_value_get_list_value(plist,3)), toInt32(fl_value_get_list_value(plist,4)));
        return fromBool(RetResult);
    }
    case StarSrvGroupClass_initRaw:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,2))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,2)));
            return fromBool(VS_FALSE);
        }
        VS_BOOL Result = l_SrvGroup->InitRaw(toString(fl_value_get_list_value(plist,1)), l_Service);
        return fromBool(Result);
    }
    case StarSrvGroupClass_loadRawModule:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        VS_BOOL Result = l_SrvGroup->LoadRawModule(toString(fl_value_get_list_value(plist,1)), toString(fl_value_get_list_value(plist,2)), toString(fl_value_get_list_value(plist,3)), toBool(fl_value_get_list_value(plist,4)), NULL);
        return fromBool(Result);
    }
    case StarSrvGroupClass_getLastError:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromInt32(0);
        }
        return fromInt32(l_SrvGroup->GetLastError());
    }
    case StarSrvGroupClass_getLastErrorInfo:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        VS_UINT32 LineIndex;
        VS_CHAR* SourceName, * TextBuf;
        VS_CHAR LocalBuf[512];
        TextBuf = l_SrvGroup->GetLastErrorInfo(&LineIndex, &SourceName);
        vs_string_snprintf(LocalBuf, 512, "[%s:%d]%s", SourceName, LineIndex, TextBuf);
        return fromString(TextBuf);
    }
    case StarSrvGroupClass_getCorePath:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        return fromString(l_SrvGroup->GetCorePath());
    }
    case StarSrvGroupClass_getUserPath:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        return fromString(l_SrvGroup->GetUserPath());
    }
    case StarSrvGroupClass_getLocalIP:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("127.0.0.1");
        }
        return fromString(l_SrvGroup->GetLocalIP());
    }
    case StarSrvGroupClass_getLocalIPEx:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        FlValue *Result = fl_value_new_list();
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            FlValue* val = fromString("127.0.0.1");
            fl_value_append_take(Result,val);
            return Result;
        }
        SOCKADDR_IN SockAddr[64];
        VS_CHAR Buf[64];
        VS_INT32 Number, i;
        Number = l_SrvGroup->GetLocalIPEx(SockAddr, 64);
        for (i = 0; i < Number; i++) {
            sprintf(Buf, "%d.%d.%d.%d", ((struct _in_addr*)&SockAddr[i].sin_addr)->S_un.S_un_b.s_b1,
                ((struct _in_addr*)&SockAddr[i].sin_addr)->S_un.S_un_b.s_b2,
                ((struct _in_addr*)&SockAddr[i].sin_addr)->S_un.S_un_b.s_b3,
                ((struct _in_addr*)&SockAddr[i].sin_addr)->S_un.S_un_b.s_b4);
            FlValue* val = fromString(Buf);
            fl_value_append_take(Result,val);
        }
        return Result;
    }
    case StarSrvGroupClass_getObjectNum:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromInt32(0);
        }
        return fromInt32(l_SrvGroup->GetObjectNum());
    }
    case StarSrvGroupClass_activeScriptInterface:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        FlValue* Result = fl_value_new_list();
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            FlValue* val = fromBool(VS_FALSE);
            fl_value_append_take(Result,val);
            val = fromBool(VS_FALSE);
            fl_value_append_take(Result,val);
            return Result;
        }
        VS_BOOL Result_Bool, OnLineScriptFlag;
        Result_Bool = SRPControlInterface->ActiveScriptInterface(toString(fl_value_get_list_value(plist,1)), &OnLineScriptFlag, NULL);
        FlValue* val = fromBool(Result_Bool);
        fl_value_append_take(Result,val);
        val = fromBool(OnLineScriptFlag);
        fl_value_append_take(Result,val);
        return Result;
    }
    case StarSrvGroupClass_preCompile:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        FlValue* Result = fl_value_new_list();
        class ClassOfBasicSRPInterface* l_SrvGroup = (class ClassOfBasicSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_SrvGroup == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "starsrvgroup object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            FlValue* val = fromBool(VS_FALSE);
            fl_value_append_take(Result,val);
            val = fromString("error...");
            fl_value_append_take(Result,val);
            return Result;
        }
        VS_CHAR* In_ScriptBuf = toString(fl_value_get_list_value(plist,2));
        VS_CHAR* ErrorInfo;
        VS_BOOL Result_Value = l_SrvGroup->PreCompile(toString(fl_value_get_list_value(plist,1)), In_ScriptBuf, (VS_INT32)vs_string_strlen(In_ScriptBuf), "", &ErrorInfo);
        FlValue* val = fromBool(Result_Value);
        fl_value_append_take(Result,val);
        val = fromString(ErrorInfo);
        fl_value_append_take(Result,val);
        return Result;
    }

    /*-----------------------------------------------*/
    case StarServiceClass_toString:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        if (l_Service->GetServiceName() == NULL) {
            return fromString("service not load");
        }
        else {
            return fromString(l_Service->GetServiceName());
        }
    }
    case StarServiceClass_get:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        VS_CHAR* ParaName = toString(fl_value_get_list_value(plist,1));
        if (vs_string_strcmp(ParaName, "_Name") == 0) {
            return fromString(l_Service->GetServiceName());
        }
        else if (vs_string_strcmp(ParaName, "_ID") == 0) {
            VS_UUID ServiceID;
            l_Service->GetServiceID(&ServiceID);
            return fromString(l_Service->UuidToString(&ServiceID));
        }
        else if (vs_string_strcmp(ParaName, "_ServiceGroup") == 0) {
            class ClassOfBasicSRPInterface* l_BasicSRPInterface = l_Service->GetBasicInterface();
            if (l_BasicSRPInterface->GetServiceGroupID() == 0) {
                if (BasicSRPInterface == NULL) {
                    BasicSRPInterface = l_BasicSRPInterface;
                }
                else {
                    l_BasicSRPInterface->Release();
                }
                VS_CHAR* CleObjectID = getTagFromObjectMap(BasicSRPInterface);
                VS_CHAR CleObjectID_Buf[CleObjectID_LENGTH];
                if (CleObjectID == NULL) {
                    CleObjectID = CleObjectID_Buf;
                    VS_UUID ObjectTemp;
                    SRPControlInterface->CreateUuid(&ObjectTemp);
                    sprintf(CleObjectID_Buf,"%s%s", StarSrvGroupPrefix, SRPControlInterface->UuidToString(&ObjectTemp));
                    BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)BasicSRPInterface);
                }
                return fromString(CleObjectID);
            }
            else {
                VS_CHAR* CleObjectID = getTagFromObjectMap(l_BasicSRPInterface);
                VS_CHAR CleObjectID_Buf[CleObjectID_LENGTH];
                if (CleObjectID == NULL) {
                    CleObjectID = CleObjectID_Buf;
                    VS_UUID ObjectIDTemp;
                    SRPControlInterface->CreateUuid(&ObjectIDTemp);
                    sprintf(CleObjectID_Buf, "%s%s", StarSrvGroupPrefix, SRPControlInterface->UuidToString(&ObjectIDTemp));
                    l_BasicSRPInterface->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)l_BasicSRPInterface);
                }
                return fromString(CleObjectID);
            }
        }
        return NULL;
    }
    case StarServiceClass_getObject:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        void* Object = l_Service->GetObjectEx(NULL, toString(fl_value_get_list_value(plist,1)));
        if (Object == NULL) {
            return NULL;
        }
        l_Service->AddRefEx(Object);
        VS_UUID ObjectID;
        l_Service->GetID(Object, &ObjectID);
        
        VS_CHAR CleObjectID[CleObjectID_LENGTH];
        VS_UUID ObjectIDTemp;
        SRPControlInterface->CreateUuid(&ObjectIDTemp);

        sprintf(CleObjectID,"%s%s", StarObjectPrefix, SRPControlInterface->UuidToString(&ObjectIDTemp));
        l_Service->InsertStringKey(CleObjectMap, CleObjectID,(VS_INT8 *)Object);

        VS_CHAR CleObjectID_Result[CleObjectID_LENGTH*2];

        sprintf(CleObjectID_Result,"%s+%s", CleObjectID, l_Service->UuidToString(&ObjectID));
        return fromString(CleObjectID_Result);
    }
    case StarServiceClass_getObjectEx:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        VS_UUID LocalObjectID;
        l_Service->StringToUuid(toString(fl_value_get_list_value(plist,1)), &LocalObjectID);
        void* Object = l_Service->GetObject(&LocalObjectID);
        if (Object == NULL) {
            return NULL;
        }
        l_Service->AddRefEx(Object);
        VS_UUID ObjectID;
        l_Service->GetID(Object, &ObjectID);

        VS_CHAR CleObjectID[CleObjectID_LENGTH];
        VS_UUID ObjectIDTemp;
        SRPControlInterface->CreateUuid(&ObjectIDTemp);

        sprintf(CleObjectID, "%s%s", StarObjectPrefix, SRPControlInterface->UuidToString(&ObjectIDTemp));
        l_Service->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)Object);

        VS_CHAR CleObjectID_Result[CleObjectID_LENGTH * 2];

        sprintf(CleObjectID_Result, "%s+%s", CleObjectID, l_Service->UuidToString(&ObjectID));
        return fromString(CleObjectID_Result);
    }
    case StarServiceClass_newObject:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        FlValue* args = fl_value_get_list_value(plist,1);
        VS_INT32 Index = 0;
        VS_INT32 argc = (VS_INT32)fl_value_get_length(args);
        VS_ATTRIBUTEINFO AttributeInfo;
        VS_CHAR* QueueAttrName, * ObjectNameString, * AttributeChangeString;
        void* SRPObject = NULL, * SRPParentObject = NULL;
        FlValue *ObjectTemp;

        QueueAttrName = NULL;
        SRPParentObject = NULL;
        AttributeChangeString = NULL;
        ObjectNameString = NULL;
        ObjectTemp = NULL;

        vs_memset(&AttributeInfo, 0, sizeof(AttributeInfo));

        ObjectTemp = SRPObject_GetArrayObject(argc, args, Index);
        if (ObjectTemp != NULL && IsString(ObjectTemp) == VS_TRUE) {
            if (!StartsWithString(ObjectTemp, StarObjectPrefix)) {
                QueueAttrName = toString(ObjectTemp);
                Index++;
                ObjectTemp = SRPObject_GetArrayObject(argc, args, Index);
                if (ObjectTemp == NULL) {  //no more parameter
                    ObjectNameString = QueueAttrName;
                    QueueAttrName = NULL;
                    SRPParentObject = NULL;
                }
                else if (IsString(ObjectTemp) == VS_TRUE) {
                    if (!StartsWithString(ObjectTemp, StarObjectPrefix)) {
                        ObjectNameString = QueueAttrName;
                        QueueAttrName = NULL;
                        SRPParentObject = NULL;
                        AttributeChangeString = toString(ObjectTemp);
                        Index++;
                        ObjectTemp = NULL;  //end
                    }
                    else {
                        SRPParentObject = toPointer(l_Service->FindStringKey(CleObjectMap,toString(ObjectTemp)));
                        Index++;
                        ObjectTemp = SRPObject_GetArrayObject(argc, args, Index);
                    }
                }
                else {
                    return NULL;  /*--input parameter error--*/
                }
            }
            else {
                /*--is starobject--*/
                SRPParentObject = toPointer(l_Service->FindStringKey(CleObjectMap, toString(ObjectTemp)));
                Index++;
                ObjectTemp = SRPObject_GetArrayObject(argc, args, Index);
            }
        }
        else if (ObjectTemp != NULL) {
            return NULL;  /*--input parameter error--*/
        }
        if (ObjectTemp != NULL) {
            if (IsString(ObjectTemp) == VS_TRUE) {
                ObjectNameString = toString(ObjectTemp);
                Index++;
                ObjectTemp = SRPObject_GetArrayObject(argc, args, Index);
                if (ObjectTemp == NULL) { //no more parameter
                }
                else if (IsString(ObjectTemp) == VS_TRUE)
                    AttributeChangeString = toString(ObjectTemp);
            }
        }
        if (SRPParentObject == NULL)
            SRPObject = l_Service->MallocObjectL(NULL, 0, NULL);
        else {
            if (l_Service->IsObject(SRPParentObject)) {
                if (QueueAttrName != NULL) {
                    if (l_Service->GetAttributeInfoEx(l_Service->GetClass(SRPParentObject), QueueAttrName, &AttributeInfo) == VS_FALSE) {
                        l_Service->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "Get Parent Attribute [%s]", QueueAttrName);
                        return NULL;
                    }
                }
                else {
                    VS_INT32 AttributeNumber = l_Service->GetAttributeNumber(l_Service->GetClass(SRPParentObject));
                    VS_INT32 i;
                    for (i = 0; i < AttributeNumber; i++) {
                        l_Service->GetAttributeInfo(l_Service->GetClass(SRPParentObject), i, &AttributeInfo);
                        if (AttributeInfo.Type == VSTYPE_PTR && AttributeInfo.SyncType == VS_TRUE && UUID_ISINVALID(AttributeInfo.StructID))
                            break;
                    }
                    if (i >= AttributeNumber) {
                        l_Service->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "not Found Parent Sync Attribute Queue");
                        return NULL;
                    }
                }
                SRPObject = l_Service->MallocObject(SRPParentObject, AttributeInfo.AttributeIndex, NULL, 0, NULL);
            }
            else {
                SRPObject = l_Service->MallocObject(SRPParentObject, 0, NULL, 0, NULL);
            }
        }
        if (SRPObject == NULL) {
            return NULL;
        }
        l_Service->SetSourceScript(SRPObject, VS_SOURCESCRIPT_C);
        if (ObjectNameString != NULL && vs_string_strlen(ObjectNameString) != 0)
            l_Service->SetName(SRPObject, ObjectNameString);
        if (AttributeChangeString != NULL && vs_string_strlen(AttributeChangeString) != 0)
            l_Service->LuaInitObject(SRPObject, AttributeChangeString);

        VS_UUID ObjectID;
        l_Service->GetID(SRPObject, &ObjectID);

        VS_CHAR CleObjectID[CleObjectID_LENGTH];
        VS_UUID ObjectIDTemp;
        SRPControlInterface->CreateUuid(&ObjectIDTemp);

        sprintf(CleObjectID, "%s%s", StarObjectPrefix, SRPControlInterface->UuidToString(&ObjectIDTemp));
        l_Service->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)SRPObject);

        VS_CHAR CleObjectID_Result[CleObjectID_LENGTH * 2];

        sprintf(CleObjectID_Result, "%s+%s", CleObjectID, l_Service->UuidToString(&ObjectID));
        return fromString(CleObjectID_Result);
    }

    case StarServiceClass_runScript:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        FlValue *Result = fl_value_new_list();
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            fl_value_append_take(Result,fromBool(VS_FALSE));
            fl_value_append_take(Result,fromString("error..."));
            return Result;
        }
        VS_CHAR* ScriptBuf = toString(fl_value_get_list_value(plist,2));
        VS_CHAR* ErrorInfo;
        VS_BOOL Ex_Result = l_Service->DoBuffer(toString(fl_value_get_list_value(plist,1)), ScriptBuf, (VS_INT32)vs_string_strlen(ScriptBuf), toString(fl_value_get_list_value(plist,3)), &ErrorInfo, toString(fl_value_get_list_value(plist,4)), VS_FALSE);
        fl_value_append_take(Result,fromBool(Ex_Result));
        fl_value_append_take(Result,fromString(ErrorInfo));
        return Result;
    }
    case StarServiceClass_runScriptEx:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        FlValue *Result = fl_value_new_list();
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            fl_value_append_take(Result,fromBool(VS_FALSE));
            fl_value_append_take(Result,fromString("error..."));
            return Result;
        }
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,2))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            fl_value_append_take(Result,fromBool(VS_FALSE));
            fl_value_append_take(Result,fromString("error..."));
            return Result;
        }
        VS_CHAR* ErrorInfo;
        VS_BOOL Ex_Result = l_Service->DoBuffer(toString(fl_value_get_list_value(plist,1)), l_BinBuf->GetBufPtr(0), l_BinBuf->GetOffset(), toString(fl_value_get_list_value(plist,3)), &ErrorInfo, toString(fl_value_get_list_value(plist,4)), VS_FALSE);
        fl_value_append_take(Result,fromBool(Ex_Result));
        fl_value_append_take(Result,fromString(ErrorInfo));
        return Result;
    }
    case StarServiceClass_doFile:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        FlValue *Result = fl_value_new_list();
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            fl_value_append_take(Result,fromBool(VS_FALSE));
            fl_value_append_take(Result,fromString("error..."));
            return Result;
        }
        VS_CHAR* ErrorInfo;
        VS_BOOL Ex_Result = l_Service->DoFileEx(toString(fl_value_get_list_value(plist,1)), toString(fl_value_get_list_value(plist,2)), &ErrorInfo, toString(fl_value_get_list_value(plist,3)), VS_FALSE, NULL);
        fl_value_append_take(Result,fromBool(Ex_Result));
        fl_value_append_take(Result,fromString(ErrorInfo));
        return Result;
    }
    case StarServiceClass_doFileEx:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        FlValue *Result = fl_value_new_list();
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            fl_value_append_take(Result,fromBool(VS_FALSE));
            fl_value_append_take(Result,fromString("error..."));
            return Result;
        }
        VS_CHAR* ErrorInfo;
        VS_BOOL Ex_Result = l_Service->DoFileEx(toString(fl_value_get_list_value(plist,1)), toString(fl_value_get_list_value(plist,2)), &ErrorInfo, toString(fl_value_get_list_value(plist,3)), VS_FALSE, toString(fl_value_get_list_value(plist,4)));
        fl_value_append_take(Result,fromBool(Ex_Result));
        fl_value_append_take(Result,fromString(ErrorInfo));
        return Result;
    }
    case StarServiceClass_isServiceRegistered:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        return fromBool(l_Service->IsRegistered());
    }
    case StarServiceClass_checkPassword:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        l_Service->CheckPassword(toBool(fl_value_get_list_value(plist,1)));
        return NULL;
    }
    case StarServiceClass_newRawProxy:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        void* l_Object = toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,2))));
        if (l_Object == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,2)));
            return NULL;
        }
        void* Result = l_Service->NewRawProxy(toString(fl_value_get_list_value(plist,1)), l_Object, toString(fl_value_get_list_value(plist,3)), toString(fl_value_get_list_value(plist,4)), toInt32(fl_value_get_list_value(plist,5)));
        if (Result == NULL)
            return NULL;
        VS_UUID ObjectID;
        l_Service->GetID(Result, &ObjectID);

        VS_CHAR CleObjectID[CleObjectID_LENGTH];
        VS_UUID ObjectIDTemp;
        SRPControlInterface->CreateUuid(&ObjectIDTemp);

        sprintf(CleObjectID, "%s%s", StarObjectPrefix, SRPControlInterface->UuidToString(&ObjectIDTemp));
        l_Service->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)Result);

        VS_CHAR CleObjectID_Result[CleObjectID_LENGTH * 2];

        sprintf(CleObjectID_Result, "%s+%s", CleObjectID, l_Service->UuidToString(&ObjectID));
        return fromString(CleObjectID_Result);
    }
    case StarServiceClass_importRawContext:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        void* Result = l_Service->ImportRawContext(toString(fl_value_get_list_value(plist,1)), toString(fl_value_get_list_value(plist,2)), toBool(fl_value_get_list_value(plist,3)), toString(fl_value_get_list_value(plist,4)));
        if (Result == NULL)
            return NULL;
        VS_UUID ObjectID;
        l_Service->GetID(Result, &ObjectID);

        VS_CHAR CleObjectID[CleObjectID_LENGTH];
        VS_UUID ObjectIDTemp;
        SRPControlInterface->CreateUuid(&ObjectIDTemp);

        sprintf(CleObjectID, "%s%s", StarObjectPrefix, SRPControlInterface->UuidToString(&ObjectIDTemp));
        l_Service->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)Result);

        VS_CHAR CleObjectID_Result[CleObjectID_LENGTH * 2];

        sprintf(CleObjectID_Result, "%s+%s", CleObjectID, l_Service->UuidToString(&ObjectID));
        return fromString(CleObjectID_Result);
    }
    case StarServiceClass_getLastError:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromInt32(0);
        }
        return fromInt32(l_Service->GetLastError());
    }
    case StarServiceClass_getLastErrorInfo:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        VS_UINT32 LineIndex;
        VS_CHAR* SourceName, * TextBuf;
        VS_CHAR LocalBuf[512];
        TextBuf = l_Service->GetLastErrorInfo(&LineIndex, &SourceName);
        vs_string_snprintf(LocalBuf, 512, "[%s:%d]%s", SourceName, LineIndex, TextBuf);
        return fromString(TextBuf);
    }

    case StarServiceClass_allObject:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_Service == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "service object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fl_value_new_list();;
        }
        VS_PARAPKGPTR RetParaPkg;
        RetParaPkg = l_Service->AllObject();
        if (RetParaPkg->GetNumber() == 0) {
            RetParaPkg->Release();
            return fl_value_new_list();;
        }
        else {
            FlValue *res = fl_value_new_list();
            for (int i = 0; i < RetParaPkg->GetNumber(); i++) {
                FlValue *vv = StarParaPkg_GetAtIndex(RetParaPkg, i);
                if (vv != NULL) {
                    fl_value_append_take(res,vv);
                }
                else
                    fl_value_append_take(res,fl_value_new_null());
            }
            return res;
        }
    }

    case StarServiceClass_restfulCall:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPInterface* l_Service = (class ClassOfSRPInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        FlValue *Result = fl_value_new_list();
        if (l_Service == NULL) {
            fl_value_append_take(Result,fromInt32(400));
            fl_value_append_take(Result,fromString((VS_CHAR*)"{\"code\": -32600, \"message\": \"call _RestfulCall failed,input para error\"}"));
            return Result;
        }
        VS_CHAR* Url = toString(fl_value_get_list_value(plist,1));
        VS_CHAR* OpCode = toString(fl_value_get_list_value(plist,2));
        VS_CHAR* JsonString = toString(fl_value_get_list_value(plist,3));
        VS_INT32 ResultCode;
        VS_CHAR* Ex_Result;

        if (Url == NULL || OpCode == NULL) {
            fl_value_append_take(Result,fromInt32(400));
            fl_value_append_take(Result,fromString((VS_CHAR*)"{\"code\": -32600, \"message\": \"call _RestfulCall failed,input para error\"}"));
            return Result;
        }
        if (JsonString == NULL) {
            Ex_Result = l_Service->RestfulCall(Url, OpCode, NULL, &ResultCode);
            fl_value_append_take(Result,fromInt32(ResultCode));
            fl_value_append_take(Result,fromString(Ex_Result));
            return Result;
        }
        else {
            Ex_Result = l_Service->RestfulCall(Url, OpCode, JsonString, &ResultCode);
            fl_value_append_take(Result,fromInt32(ResultCode));
            fl_value_append_take(Result,fromString(Ex_Result));
            return Result;
        }
    }


    /*-----------------------------------------------*/
    case StarParaPkgClass_toString:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        class ClassOfSRPParaPackageInterface* ResultParaPkg = l_ParaPkg->GetDesc();
        VS_CHAR* JsonStr = ResultParaPkg->ToJSon();
        FlValue* ResultObject;
        if (JsonStr == NULL)
            ResultObject = fromString("parapkg");
        else {
            ResultObject = fromString(JsonStr);
            ResultParaPkg->FreeBuf(JsonStr);
        }
        ResultParaPkg->Release();
        return ResultObject;
    }
    case StarParaPkgClass_GetNumber:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromInt32(0);
        }
        return fromInt32(l_ParaPkg->GetNumber());
    }
    case StarParaPkgClass_get:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        FlValue *IndexOrList = fl_value_get_list_value(plist,1);
        if ( IsList(IndexOrList) == VS_TRUE ) {
            FlValue *res = fl_value_new_list();
            FlValue* IndexOrList_List = IndexOrList;
            for (int i = 0; i < fl_value_get_length(IndexOrList_List); i++) {
                FlValue* vv = StarParaPkg_GetAtIndex(l_ParaPkg, toInt32(fl_value_get_list_value(IndexOrList_List,i)));
                if (vv != NULL) {
                    fl_value_append_take(res,vv);
                }
                else
                    fl_value_append_take(res,fl_value_new_null());
            }
            return res;
        }
        else {
            return StarParaPkg_GetAtIndex(l_ParaPkg, toInt32(IndexOrList));
        }
    }
    case StarParaPkgClass_clear:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        l_ParaPkg->Clear();
        return NULL;
    }
    case StarParaPkgClass_appendFrom:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        class ClassOfSRPParaPackageInterface* s_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,1))));
        if (s_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,1)));
            return fromBool(VS_FALSE);
        }
        return fromBool(l_ParaPkg->AppendFrom(s_ParaPkg));
    }
    case StarParaPkgClass_equals:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        class ClassOfSRPParaPackageInterface* s_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,1))));
        if (s_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,1)));
            return fromBool(VS_FALSE);
        }
        return fromBool(l_ParaPkg->Equals(s_ParaPkg));
    }
    case StarParaPkgClass_V:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        VS_CHAR* CharValue = l_ParaPkg->GetValueStr();
        if (CharValue == NULL)
            return fromString("");
        else {
            return fromString(CharValue);
        }
    }
    case StarParaPkgClass_set:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        FlValue *iv = fl_value_get_list_value(plist,2);
        FlValue *arg = fl_value_new_list();
        fl_value_append_take(arg,iv);
        StarParaPkg_FromTuple_Sub(arg, 0, l_ParaPkg, toInt32(fl_value_get_list_value(plist,1)));
        fl_value_unref(arg);
        return NULL;
    }
    case StarParaPkgClass_build:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        l_ParaPkg->Clear();
        FlValue* vv = fl_value_get_list_value(plist,1);
        if (IsList(vv) == VS_TRUE) {
            FlValue* parglist = vv;
            if (StarParaPkg_FromTuple_Sub(parglist, 0, l_ParaPkg, 0) == VS_FALSE) {
                l_ParaPkg->Clear();
                return NULL;
            }
        }
        else if (IsMap(vv) == VS_TRUE) {
            FlValue* nd = vv;
            FlValue *parglist = fl_value_new_list();
           for (size_t iter =0; iter < fl_value_get_length(nd); iter++) {
                fl_value_append(parglist,fl_value_get_map_key(nd,iter));
                fl_value_append(parglist,fl_value_get_map_value(nd,iter));
            }
            if (StarParaPkg_FromTuple_Sub(parglist, 0, l_ParaPkg, 0) == VS_FALSE) {
                l_ParaPkg->Clear();
                fl_value_unref(parglist);
                return NULL;
            }
            fl_value_unref(parglist);
            l_ParaPkg->AsDict(VS_TRUE);
        }
        else {
            return NULL;
        }
        return NULL;
    }
    case StarParaPkgClass_free:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        BasicSRPInterface->DelStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0)));
        l_ParaPkg->Release();
        return NULL;
    }
    case StarParaPkgClass_dispose:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        BasicSRPInterface->DelStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0)));
        l_ParaPkg->Release();
        return NULL;
    }
    case StarParaPkgClass_releaseOwner:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        BasicSRPInterface->DelStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0)));
        l_ParaPkg->ReleaseOwner();
        return NULL;
    }
    case StarParaPkgClass_asDict:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        l_ParaPkg->AsDict(toBool(fl_value_get_list_value(plist,1)));
        return NULL;
    }
    case StarParaPkgClass_isDict:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        return fromBool(l_ParaPkg->IsDict());
    }
    case StarParaPkgClass_fromJSon:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        return fromBool(l_ParaPkg->FromJSon(toString(fl_value_get_list_value(plist,1))));
    }
    case StarParaPkgClass_toJSon:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        return fromString(l_ParaPkg->ToJSon());
    }
    case StarParaPkgClass_toTuple:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = (class ClassOfSRPParaPackageInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_ParaPkg == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "parapkg object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        return StarParaPkg_ToTuple(l_ParaPkg);
    }

   /*-----------------------------------------------*/
    case StarBinBufClass_toString:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        return fromString(l_BinBuf->GetName());
    }
    case StarBinBufClass_GetOffset:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromInt32(0);
        }
        return fromInt32(l_BinBuf->GetOffset());
    }
    case StarBinBufClass_init:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        l_BinBuf->Init((VS_UINT32)toInt32(fl_value_get_list_value(plist,1)));
        return NULL;
    }
    case StarBinBufClass_clear:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        l_BinBuf->Clear();
        return NULL;
    }
    case StarBinBufClass_saveToFile:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        VS_CHAR* FileName;
        FILE* hFile;
        VS_INT32 Length;
        VS_INT8* Buf;

        Buf = l_BinBuf->GetBuf();
        Length = l_BinBuf->GetOffset();
        if (Buf == NULL || Length == 0)
            return fromBool(VS_FALSE);
        FileName = toString(fl_value_get_list_value(plist,1));
        VS_BOOL TxtFileFlag = toBool(fl_value_get_list_value(plist,2));
        if (TxtFileFlag == VS_TRUE)
            hFile = vs_file_fopen(FileName, "wt");
        else
            hFile = vs_file_fopen(FileName, "wb");
        if (hFile == NULL) {
            return fromBool(VS_FALSE);
        }
        if (Length != 0)
            fwrite(Buf, 1, Length, hFile);
        fclose(hFile);
        return fromBool(VS_TRUE);
    }
    case StarBinBufClass_loadFromFile:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        VS_CHAR* FileName;
        FILE* hFile;
        VS_INT32 Length;
        VS_INT8* Buf;
        VS_BOOL Result;

        FileName = toString(fl_value_get_list_value(plist,1));
        VS_BOOL TxtFileFlag = toBool(fl_value_get_list_value(plist,2));
        if (TxtFileFlag == VS_TRUE)
            hFile = vs_file_fopen(FileName, "rt");
        else
            hFile = vs_file_fopen(FileName, "rb");
        if (hFile == NULL) {
            return fromBool(VS_FALSE);
        }
        fseek(hFile, 0, SEEK_END);
        Length = (VS_INT32)ftell(hFile);
        fseek(hFile, 0, SEEK_SET);
        Buf = (VS_INT8*)malloc(Length);
        Length = (VS_INT32)fread(Buf, 1, Length, hFile);
        fclose(hFile);
        l_BinBuf->Clear();
        Result = l_BinBuf->Set(0, Length, Buf);
        free(Buf);
        return fromBool(Result);
    }
    case StarBinBufClass_read:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fl_value_new_uint8_list(NULL,0);
        }
        int Length = toInt32(fl_value_get_list_value(plist,2));
        if (Length <= 0) {
            return fl_value_new_uint8_list(NULL,0);
        }
        VS_UINT32 Offset = (VS_UINT32)toInt32(fl_value_get_list_value(plist,1));
        VS_UINT32 Size;
        VS_INT8* BufPtr;
        BufPtr = (VS_INT8*)l_BinBuf->GetBufPtr(0);
        Size = l_BinBuf->GetOffset();
        if (BufPtr == NULL || Offset >= (VS_INT32)Size) {
            return fl_value_new_uint8_list(NULL,0);
        }
        if (Offset + Length > (VS_INT32)Size)
            Length = Size - Offset;
    
        GBytes *adata = g_bytes_new(&BufPtr[Offset],(gsize)Length);
        FlValue *fl = fl_value_new_uint8_list_from_bytes(adata);
        g_bytes_unref(adata);
        return fl;
    }
    case StarBinBufClass_write:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromInt32(0);
        }
        VS_INT32 Length = toInt32(fl_value_get_list_value(plist,3));
        VS_INT32 Offset = toInt32(fl_value_get_list_value(plist,1));
        l_BinBuf->FromRaw(VS_TRUE);
        if (Length == 0)
            return fromInt32(0);
        VS_INT8 * BufPtr = (VS_INT8 *)fl_value_get_uint8_list(fl_value_get_list_value(plist,2));
        if (BufPtr == NULL)
            return fromInt32(0);
        if (l_BinBuf->Set(Offset, Length, BufPtr) == VS_FALSE) {
            return fromInt32(0);
        }
        return fromInt32(Length);
    }
    case StarBinBufClass_free:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        BasicSRPInterface->DelStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0)));
        l_BinBuf->Release();
        return NULL;
    }
    case StarBinBufClass_dispose:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        BasicSRPInterface->DelStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0)));
        l_BinBuf->Release();
        return NULL;
    }
    case StarBinBufClass_releaseOwner:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        BasicSRPInterface->DelStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0)));
        l_BinBuf->ReleaseOwner();
        return NULL;
    }
    case StarBinBufClass_setOffset:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        VS_INT32 Offset = toInt32(fl_value_get_list_value(plist,1));
        if( Offset < 0 )
            Offset = 0;
        VS_BOOL Result = l_BinBuf -> SetOffset((VS_UINT32)Offset);
        return fromBool(Result);
    }
    case StarBinBufClass_print:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        VS_CHAR *Arg = toString(fl_value_get_list_value(plist,1));
        l_BinBuf->Print(0,"%s",Arg);
        return NULL;
    }

    case StarBinBufClass_asString:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        class ClassOfSRPBinBufInterface* l_BinBuf = (class ClassOfSRPBinBufInterface*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_BinBuf == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "binbuf object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        VS_INT32 Length = l_BinBuf -> GetOffset();
        if( Length == 0 ){
            return fromString("");
        }else{
            VS_CHAR LocalBuf[10240+1];
            VS_CHAR *StringBufPtr;
            if( Length >= 10240 )
                StringBufPtr = (VS_CHAR *)BasicSRPInterface -> Malloc(Length + 1);
            else
                StringBufPtr = ObjectCreate_AttachBuf;
            vs_memcpy( StringBufPtr, l_BinBuf->GetBuf(), Length );
            StringBufPtr[Length] = 0;
            FlValue* RetValue = fromString(StringBufPtr );
            if( Length >= 10240 )
                BasicSRPInterface -> Free(StringBufPtr);
			return RetValue;
		}
    }
    /*-----------------------------------------------*/
    case StarObjectClass_toString:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        VS_CHAR* CharPtr, CharBuf[10240], * InterfaceName;
        CharPtr = l_Service->GetRawContextType(l_StarObject, &InterfaceName);
        if (CharPtr == NULL) {
            VS_CHAR* StrName = (VS_CHAR*)l_Service->ScriptCall(l_StarObject, NULL, VS_OBJECTTOSTRINGFUNCNAME, "()s");
            if (StrName != NULL) {
                return fromString(StrName);
            }
            return fromString(l_Service->GetName(l_StarObject));
        }
        else {
            VS_CHAR* StrName = (VS_CHAR*)l_Service->ScriptCall(l_StarObject, NULL, VS_OBJECTTOSTRINGFUNCNAME, "()s");
            if (StrName != NULL) {
                vs_string_snprintf(CharBuf, 10240, "%s[%s:%s]%s", l_Service->GetName(l_StarObject), InterfaceName, CharPtr, StrName);
                return fromString(CharBuf);
            }
            vs_string_snprintf(CharBuf, 10240, "%s[%s:%s]", l_Service->GetName(l_StarObject), InterfaceName, CharPtr);
            return fromString(CharBuf);
        }
    }
    case StarObjectClass_get:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        FlValue *IndexOrList = fl_value_get_list_value(plist,1);
        if (IsList(IndexOrList) == VS_TRUE) {
            FlValue *res =  fl_value_new_list();
            FlValue* IndexOrList_List = IndexOrList;
            for (int i = 0; i < fl_value_get_length(IndexOrList_List); i++) {
                FlValue *vv = StarObject_getValue(l_Service, l_StarObject,fl_value_get_list_value(IndexOrList_List,i));
                if (vv != NULL) {
                    fl_value_append_take(res,vv);
                }
                else
                    fl_value_append_take(res,fl_value_new_null());
            }
            return res;
        }
        else {
            return StarObject_getValue(l_Service, l_StarObject, IndexOrList);
        }
    }

    case StarObjectClass_set:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        VS_CHAR LocalNameBuf[64];
        VS_CHAR* ParaName;
        FlValue* Name = fl_value_get_list_value(plist,1);
        FlValue* Para = fl_value_get_list_value(plist,2);

        if (IsString(Name) == VS_TRUE) {
            ParaName = toString(Name);
        }
        else {
            if (IsInt32(Name) == VS_TRUE || IsInt64(Name) == VS_TRUE) {
                sprintf(LocalNameBuf, "\"%d\"", toInt32(Name));
                ParaName = LocalNameBuf;
            }
            else {
                return NULL;
            }
        }
        if (ParaName[0] == '_' && ParaName[1] == '_' && ParaName[2] == '_') {
            //---process object namevalue
            if (IsBool(Para) == VS_TRUE) 
                l_Service->SetNameBoolValue(l_StarObject, &ParaName[3], toBool(Para), VS_FALSE);
            else if (IsInt32(Para) == VS_TRUE)
                l_Service->SetNameIntValue(l_StarObject, &ParaName[3], toInt32(Para), VS_FALSE);
            else if (IsInt64(Para) == VS_TRUE)
                l_Service->SetNameIntValue(l_StarObject, &ParaName[3], (VS_INT32)toInt64(Para), VS_FALSE);
            else if (IsDouble(Para) == VS_TRUE)
                l_Service->SetNameFloatValue(l_StarObject, &ParaName[3], toDouble(Para), VS_FALSE);
            else if (IsString(Para) == VS_TRUE) {
                l_Service->SetNameStrValue(l_StarObject, &ParaName[3], toString(Para), VS_FALSE);
            }else
                return NULL;
        }
        if (vs_string_strcmp(ParaName, "_Name") == 0) {
            l_Service->SetName(l_StarObject, toString(Para));
            return NULL;
        }
        //---is object atribute?
        VS_ATTRIBUTEINFO AttributeInfo;
        VS_CHAR Info[256];
        if (l_Service->GetAttributeInfoEx(l_Service->GetClass(l_StarObject), ParaName, &AttributeInfo) == VS_TRUE) {
            if (SRPObject_FlutterObjectToAttribute(l_Service, AttributeInfo.Type, AttributeInfo.Length, &AttributeInfo.StructID, Para, 0, (VS_UINT8*)ObjectCreate_AttachBuf) == VS_FALSE) {
                sprintf(Info, "Set Object[%s] Attribute [%s] Error", l_Service->GetName(l_StarObject), AttributeInfo.Name);
                l_Service->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, Info);
                return NULL;
            }
            l_Service->ChangeObject(l_StarObject, AttributeInfo.AttributeIndex, ObjectCreate_AttachBuf);
            return NULL;
        }
        else {
            VS_UUID ObjectID;

            l_Service->GetID(l_StarObject, &ObjectID);
            //---is script defined attribute?
            l_Service->LuaPushObject(l_StarObject);
            l_Service->LuaPushString(ParaName);
            l_Service->LuaGetTable(-2);
            if (l_Service->LuaIsNil(-1) == VS_FALSE) {
                VS_UUID DefinedClassID;

                l_Service->LuaGetDefinedClass(l_StarObject, &DefinedClassID);
                if (l_Service->LuaIsFunction(-1) == VS_FALSE && UUID_ISEQUAL(ObjectID, DefinedClassID)) {
                    l_Service->LuaPop(2);
                    l_Service->LuaPushObject(l_StarObject);
                    l_Service->LuaPushString(ParaName);

                    if (FlutterObjectToLua(l_Service, Para) == VS_FALSE) {
                        sprintf(Info, "Set Object[%s] Attribute [%s] Error", l_Service->GetName(l_StarObject), ParaName);
                        l_Service->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, Info);
                    }
                    l_Service->LuaSetTable(-3);
                    l_Service->LuaPop(1);
                    return NULL;
                }
            }
            l_Service->LuaPop(2);
            if (l_Service->GetRawLuaSetValueFunc(l_StarObject, ParaName, NULL) != NULL) {
                l_Service->LuaPushObject(l_StarObject);
                l_Service->LuaPushString(ParaName);

                if (FlutterObjectToLua(l_Service, Para) == VS_FALSE) {
                    sprintf(Info, "Set Object[%s] Attribute [%s] Error", l_Service->GetName(l_StarObject), ParaName);
                    l_Service->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, Info);
                }
                l_Service->LuaSetTable(-3);
                l_Service->LuaPop(1);
                return NULL;
            }
            return NULL;
        }
    }
    case StarObjectClass_call:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* SRPObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (SRPObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(SRPObject);
        FlValue* Args =fl_value_get_list_value(plist,2);
        VS_INT32 argc, i, m, n;
        FlValue *RetValue;
        FlValue* localobject;
        VS_CHAR* FunctionName;
        VS_BOOL Result;

        argc = (VS_INT32)fl_value_get_length(Args);
        n = l_Service->LuaGetTop();
        for (i = 0; i < argc; i++) {
            localobject = fl_value_get_list_value(Args,i);
            FlutterObjectToLua(l_Service, localobject);
        }
        FunctionName = toString(fl_value_get_list_value(plist,1));
        if (l_Service->LuaCall(SRPObject, FunctionName, argc, -1) == VS_FALSE) {
            m = l_Service->LuaGetTop();
            if (m > n)
                l_Service->LuaPop(m - n);
            return NULL;
        }
        m = l_Service->LuaGetTop();
        if (m == n) {
            return NULL;
        }
        else if (m > n) {
            if (m - n == 1) {
                RetValue = LuaToFlutterObject(l_Service, -1, &Result);
            }
            else {
                FlValue* objvalue;

                FlValue *RetValue_List = fl_value_new_list();
                for (i = 0; i < m - n; i++) {
                    objvalue = LuaToFlutterObject(l_Service, i - (m - n), &Result);
                    if (objvalue != NULL) {
                        fl_value_append_take(RetValue_List,objvalue);
                    }
                    else {
                        fl_value_append_take(RetValue_List,fl_value_new_null());
                    }
                }
                RetValue = RetValue_List;
            }
            l_Service->LuaPop(m - n);
            return RetValue;
        }
        else {
            l_Service->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "call function[%s] failed,lua stack corrupted", FunctionName);
            return NULL;
        }
    }

    case StarObjectClass_newObject:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        FlValue* args = fl_value_get_list_value(plist,1);
        VS_INT32 Index = 0;
        VS_INT32 argc = (VS_INT32)fl_value_get_length(args);
        VS_ATTRIBUTEINFO AttributeInfo;
        VS_CHAR* QueueAttrName, * ObjectNameString, * AttributeChangeString;
        void* SRPParentObject;
        FlValue *ObjectTemp;
        VS_BOOL SkipObjectNameString = VS_FALSE;

        QueueAttrName = NULL;
        SRPParentObject = NULL;
        AttributeChangeString = NULL;
        ObjectNameString = NULL;
        ObjectTemp = NULL;

        vs_memset(&AttributeInfo, 0, sizeof(AttributeInfo));

        ObjectTemp = SRPObject_GetArrayObject(argc, args, Index);
        if (ObjectTemp != NULL && IsString(ObjectTemp) == VS_TRUE) {
            if (!StartsWithString(ObjectTemp, StarObjectPrefix)) {
                QueueAttrName = toString(ObjectTemp);
                Index++;
                ObjectTemp = SRPObject_GetArrayObject(argc, args, Index);
                if (ObjectTemp == NULL) {  //no more parameter
                    ObjectNameString = QueueAttrName;
                    QueueAttrName = NULL;
                    SRPParentObject = NULL;
                    SkipObjectNameString = VS_TRUE;
                }
                else if (IsString(ObjectTemp) == VS_TRUE) {
                    if (!StartsWithString(ObjectTemp, StarObjectPrefix)) {
                        ObjectNameString = QueueAttrName;
                        QueueAttrName = NULL;
                        SRPParentObject = NULL;
                        AttributeChangeString = toString(ObjectTemp);
                        Index++;
                        SkipObjectNameString = VS_TRUE;
                        ObjectTemp = SRPObject_GetArrayObject(argc, args, Index);  //end
                    }
                    else {
                        SRPParentObject = toPointer(l_Service->FindStringKey(CleObjectMap,toString(ObjectTemp)));
                        Index++;
                        ObjectTemp = SRPObject_GetArrayObject(argc, args, Index);
                    }
                }
                else {
                    return NULL;  /*--input parameter error--*/
                }
            }
            else {
                /*--is starobject--*/
                SRPParentObject = toPointer(l_Service->FindStringKey(CleObjectMap, toString(ObjectTemp)));
                Index++;
                ObjectTemp = SRPObject_GetArrayObject(argc, args, Index);
            }
        }
        else if (ObjectTemp != NULL) {
            return NULL;  /*--input parameter error--*/
        }
        if (ObjectTemp != NULL && SkipObjectNameString == VS_FALSE) {
            if (IsString(ObjectTemp) == VS_TRUE) {
                ObjectNameString = toString(ObjectTemp);
                Index++;
                ObjectTemp = SRPObject_GetArrayObject(argc, args, Index);
                if (ObjectTemp == NULL) { //no more parameter
                }
                else if (IsString(ObjectTemp) == VS_TRUE) {
                    AttributeChangeString = toString(ObjectTemp);
                    Index++;
                    ObjectTemp = SRPObject_GetArrayObject(argc, args, Index);  //end
                }
            }
        }
        VS_PARAPKGPTR InitBuf = NULL;
        if (Index < argc) {
            InitBuf = l_Service->GetParaPkgInterface();
            StarParaPkg_FromTuple_Sub(args, Index, InitBuf, 0);
        }

        VS_UUID ObjectClassID;
        VS_INT32 AttachBufSize, AttributeNumber, i;
        void* AttachBuf;
        l_Service->GetID(l_StarObject, &ObjectClassID);
        AttachBufSize = 0;
        AttachBuf = NULL;
        if (SRPParentObject != NULL) {
            if (QueueAttrName != NULL) {
                if (l_Service->GetAttributeInfoEx(l_Service->GetClass(SRPParentObject), QueueAttrName, &AttributeInfo) == VS_FALSE) {
                    l_Service->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "Get Parent Attribute [%s]", QueueAttrName);
                    if (InitBuf != NULL)
                        InitBuf->Release();
                    return NULL;
                }
            }
            else {
                if (l_Service->IsObject(SRPParentObject) == VS_TRUE) {
                    VS_CHAR* ClassName;

                    ClassName = l_Service->GetName(l_StarObject);
                    AttributeNumber = l_Service->GetAttributeNumber(l_Service->GetClass(SRPParentObject));
                    for (i = 0; i < AttributeNumber; i++) {
                        l_Service->GetAttributeInfo(l_Service->GetClass(SRPParentObject), i, &AttributeInfo);
                        if (AttributeInfo.Type == VSTYPE_PTR && AttributeInfo.SyncType == VS_TRUE && (UUID_ISEQUAL(AttributeInfo.StructID, ObjectClassID) || l_Service->IsInst(&AttributeInfo.StructID, l_StarObject) == VS_TRUE))
                            break;
                    }
                    if (i >= AttributeNumber) {
                        for (i = 0; i < AttributeNumber; i++) {
                            l_Service->GetAttributeInfo(l_Service->GetClass(SRPParentObject), i, &AttributeInfo);
                            if (AttributeInfo.Type == VSTYPE_PTR && AttributeInfo.SyncType == VS_TRUE && UUID_ISINVALID(AttributeInfo.StructID))
                                break;
                        }
                        if (i >= AttributeNumber) {
                            l_Service->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "not Found Parent Sync Attribute Queue");
                            if (InitBuf != NULL)
                                InitBuf->Release();
                            return NULL;
                        }
                    }
                }
                else {
                    AttributeInfo.AttributeIndex = 0;
                }
            }
        }

        void* NewSRPObject = NULL;
        if (SRPParentObject != NULL)
            NewSRPObject = SRPInterface->IMallocObjectEx(NULL, SRPParentObject, AttributeInfo.AttributeIndex, &ObjectClassID, InitBuf);
        else
            NewSRPObject = SRPInterface->IMallocObjectLEx(NULL, &ObjectClassID, InitBuf);

        if (InitBuf != NULL)
            InitBuf->Release();

        if (NewSRPObject == NULL) {
            return NULL;
        }
        l_Service->SetSourceScript(NewSRPObject, VS_SOURCESCRIPT_C);
        if (ObjectNameString != NULL && vs_string_strlen(ObjectNameString) != 0)
            l_Service->SetName(NewSRPObject, ObjectNameString);
        if (AttributeChangeString != NULL && vs_string_strlen(AttributeChangeString) != 0)
            l_Service->LuaInitObject(NewSRPObject, AttributeChangeString);

        VS_UUID ObjectID;
        l_Service->GetID(NewSRPObject, &ObjectID);

        VS_CHAR CleObjectID[CleObjectID_LENGTH];
        VS_UUID ObjectIDTemp;
        SRPControlInterface->CreateUuid(&ObjectIDTemp);

        sprintf(CleObjectID, "%s%s", StarObjectPrefix, SRPControlInterface->UuidToString(&ObjectIDTemp));
        l_Service->InsertStringKey(CleObjectMap, CleObjectID, (VS_INT8*)NewSRPObject);

        VS_CHAR CleObjectID_Result[CleObjectID_LENGTH * 2];

        sprintf(CleObjectID_Result, "%s+%s", CleObjectID, l_Service->UuidToString(&ObjectID));
        return fromString(CleObjectID_Result);
    }
    case StarObjectClass_free:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        l_Service->DelStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0)));
        l_Service->UnLockGC(l_StarObject);
        return NULL;
    }
    case StarObjectClass_dispose:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        l_Service->DelStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0)));
        l_Service->UnLockGC(l_StarObject);
        return NULL;
    }
    case StarObjectClass_hasRawContext:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        return fromBool(l_Service->HasRawContext(l_StarObject));
    }
    case StarObjectClass_rawToParaPkg:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        class ClassOfSRPParaPackageInterface* l_ParaPkg = l_Service->RawToParaPkg(l_StarObject);
        if (l_ParaPkg == NULL)
            return NULL;
        return StarParaPkg_ToTuple(l_ParaPkg);
        /*
        NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarParaPkgPrefix,[[NSUUID UUID] UUIDString]];
        [CleObjectMap setObject:fromPointer(ParaPkg) forKey:CleObjectID];
        return CleObjectID;
        */
    }
    case StarObjectClass_getSourceScript:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromInt32(0);
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        return fromInt32(l_Service->GetSourceScript(l_StarObject));
    }
    case StarObjectClass_getLastError:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromInt32(0);
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        return fromInt32(l_Service->GetLastError());
    }
    case StarObjectClass_getLastErrorInfo:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("");
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        VS_UINT32 LineIndex;
        VS_CHAR* SourceName, * TextBuf;
        VS_CHAR LocalBuf[512];
        TextBuf = l_Service->GetLastErrorInfo(&LineIndex, &SourceName);
        vs_string_snprintf(LocalBuf, 512, "[%s:%d]%s", SourceName, LineIndex, TextBuf);
        return fromString(TextBuf);
    }
    case StarObjectClass_releaseOwnerEx:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        l_Service->DelStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0)));
        l_Service->ReleaseOwnerEx(l_StarObject);
        return NULL;
    }
    case StarObjectClass_regScriptProcP:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        if (toBool(fl_value_get_list_value(plist,2)) == VS_FALSE)
            l_Service->UnRegLuaFunc(l_StarObject, toString(fl_value_get_list_value(plist,1)), (void*)SRPObject_ScriptCallBack, 0);
        else {
            l_Service->RegLuaFunc(l_StarObject, toString(fl_value_get_list_value(plist,1)), (void*)SRPObject_ScriptCallBack, 0);
        }
        return NULL;
    }

    case StarObjectClass_instNumber:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromInt32(0);
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        return fromInt32(l_Service->InstNumberEx(l_StarObject));
    }
    case StarObjectClass_changeParent:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        FlValue* parentObjectTag = fl_value_get_list_value(plist,1);
        if (parentObjectTag == NULL || IsNull(parentObjectTag) == VS_TRUE) {
            l_Service->ChangeParent(l_StarObject, NULL, 0);
            return NULL;
        }
        void* l_ParentObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(parentObjectTag)));
        if (l_ParentObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(parentObjectTag));
            return NULL;
        }
        VS_ATTRIBUTEINFO AttributeInfo;
        VS_INT32 i, AttributeNumber;
        vs_memset(&AttributeInfo, 0, sizeof(AttributeInfo));
        VS_CHAR* QueueAttrName = toString(fl_value_get_list_value(plist,2));
        if (QueueAttrName != NULL) {
            if (l_Service->GetAttributeInfoEx(l_ParentObject, QueueAttrName, &AttributeInfo) == VS_FALSE) {
                return NULL;
            }
        }
        else {
            AttributeNumber = l_Service->GetAttributeNumber(SRPInterface->GetClass(l_ParentObject));
            for (i = 0; i < AttributeNumber; i++) {
                l_Service->GetAttributeInfo(l_Service->GetClass(l_ParentObject), i, &AttributeInfo);
                if (AttributeInfo.Type == VSTYPE_PTR && AttributeInfo.SyncType == VS_TRUE && (l_Service->IsInst(&AttributeInfo.StructID, l_StarObject) == VS_TRUE || UUID_ISINVALID(AttributeInfo.StructID)))
                    break;
            }
            if (i >= AttributeNumber) {
                return NULL;
            }
        }
        l_Service->ChangeParent(l_StarObject, l_ParentObject, AttributeInfo.AttributeIndex);
        return NULL;
    }

    case StarObjectClass_jsonCall:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromString("{\"jsonrpc\": \"2.0\", \"error\": {\"code\": -32603, \"message\": \"call _JSonCall failed,Internal error\"}, \"id\": null}");
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        VS_CHAR* InputStr = toString(fl_value_get_list_value(plist,1));
        if (InputStr == NULL) {
            return fromString("{\"jsonrpc\": \"2.0\", \"error\": {\"code\": -32603, \"message\": \"call _JSonCall failed,Internal error\"}, \"id\": null}");
        }
        VS_CHAR* ResultStr = l_Service->JSonCall(l_StarObject, InputStr);
        return fromString(ResultStr);
    }

    case StarObjectClass_active:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        return fromBool(l_Service->Active(l_StarObject));
    }

    case StarObjectClass_deActive:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return NULL;
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        l_Service->Deactive(l_StarObject);
        return NULL;
    }

    case StarObjectClass_isActive:
    {
        FlValue* plist = fl_method_call_get_args(method_call);
        void* l_StarObject = (void*)toPointer(BasicSRPInterface->FindStringKey(CleObjectMap, toString(fl_value_get_list_value(plist,0))));
        if (l_StarObject == NULL) {
            SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__, "star object[%s] can not be found..", toString(fl_value_get_list_value(plist,0)));
            return fromBool(VS_FALSE);
        }
        class ClassOfSRPInterface* l_Service = SRPInterface->GetSRPInterface(l_StarObject);
        return fromBool(l_Service->IsActive(l_StarObject));
    }
    /*-----------------------------------------------*/
    //------------------------
    
    }
    return NULL;
}
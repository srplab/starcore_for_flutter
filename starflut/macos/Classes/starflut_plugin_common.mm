#import "starflut_plugin_common.h"
#include "vsopenapi.h"

static class ClassOfSRPControlInterface *SRPControlInterface = NULL;
static class ClassOfBasicSRPInterface *BasicSRPInterface = NULL; /*--add to dict, do not call release--*/
static class ClassOfSRPInterface *SRPInterface = NULL; /*---add to dict, should call release--*/

static NSString *StarCorePrefix =     @"@s_s_c";
static NSString *StarSrvGroupPrefix = @"@s_s_g";  /*--with group index: not supported */
static NSString *StarServicePrefix =  @"@s_s_e";  /*--with service id--*/
static NSString *StarParaPkgPrefix =  @"@s_s_p";
static NSString *StarBinBufPrefix  =  @"@s_s_b";
//static NSString *StarSXmlPrefix  =  @"@s_s_x";
static NSString *StarObjectPrefix = @"@s_s_o";    /*--with object id--*/
/*--objectag:  prefix + uuid + '+' + (objectid/serviceid/groupindex) */

static bool ModuleInitFlag = false;

static NSRecursiveLock *mutexObject;
static bool ExitAppFlag;

static NSMutableDictionary *CleObjectMap;
static NSMutableDictionary *starcoreCmdMap;

static NSRecursiveLock *g_WaitResultMap_mutexObject;
static int g_WaitResultMap_Index;
static NSMutableDictionary *g_WaitResultMap;

//static NSRecursiveLock *starCoreThreadCallDeepSyncObject;
//static int starCoreThreadCallDeep = 0;
static dispatch_queue_t starCoreHandler = nil;

static FlutterMethodChannel* channel;

static VS_INT8 ObjectCreate_AttachBuf[64*1024];

/*--python--*/
extern "C" void PyInit__ssl(void);
extern "C" void PyInit__hashlib(void);

/*--ruby--*/
extern "C" void ruby_init_ext(const char*,void*);
extern "C" void Init_md5(void);
extern "C" void Init_rmd160(void);
extern "C" void Init_sha1(void);
extern "C" void Init_sha2(void);
extern "C" void Init_openssl(void);

/*--command--*/
#define starcore_getDocumentPath 1
#define starcore_isAndroid  2
#define starcore_getResourcePath 3
#define starcore_rubyInitExt     4
#define starcore_getPesudoExport 5   /*this command should not be called*/
#define starcore_getPlatform  6
#define starcore_loadLibrary 7
#define starcore_setEnv 8
#define starcore_getEnv 9
#define starcore_getAssetsPath 10

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


static NSNumber *fromBool(VS_BOOL val)
{
    if( val == VS_TRUE )
        return [NSNumber numberWithBool:true];
    return [NSNumber numberWithBool:false];
}

static VS_BOOL toBool(NSNumber *val)
{
    if( [val boolValue] == true )
        return VS_TRUE;
    return VS_FALSE;
}

static VS_CHAR *toString(NSString *val)
{
    if( val == nil || [val isKindOfClass:[NSNull class]] )
        return NULL;
    const char* destDir = [val UTF8String];
    return (VS_CHAR *)destDir;
}

static VS_CHAR *toStringEx(NSString *val,VS_INT32 *Length)
{
    if( val == nil || [val isKindOfClass:[NSNull class]] ){
        if( Length != NULL)
            (*Length) = 0;
        return NULL;
    }
    const char* destDir = [val UTF8String];
    if( Length != NULL)
        (*Length) = (VS_INT32)[val lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
    return (VS_CHAR *)destDir;
}

static NSString *fromString(const VS_CHAR *val)
{
    if( val == NULL )
        return @"";
    return [NSString stringWithUTF8String:val];
}

static VS_INT32 toInt32(NSNumber *val)
{
    return (VS_INT32)[val intValue];
}

static NSNumber *fromInt32(VS_INT32 val)
{
    return [NSNumber numberWithInt:val];
}

static VS_INT64 toInt64(NSNumber *val)
{
    return (VS_INT64)[val longLongValue];
}

static NSNumber *fromInt64(VS_INT64 val)
{
    return [NSNumber numberWithLongLong:val];
}

static VS_DOUBLE toDouble(NSNumber *val)
{
    return (VS_DOUBLE)[val doubleValue];
}

static NSNumber *fromDouble(VS_DOUBLE val)
{
    return [NSNumber numberWithDouble:val];
}

static NSNumber *fromPointer(void *ptr)
{
    return [NSNumber numberWithLongLong:(long long)ptr];
}

static void *toPointer(NSNumber *val)
{
    return (void *)[val longLongValue];
}

static id SRPObject_GetArrayObject(VS_INT32 argc,NSArray *args,VS_INT32 Index)
{
    if( Index >= argc )
        return nil;
    return [args objectAtIndex:Index];
}

/*--star message class--*/
#define starcore_ThreadTick_MessageID 0
#define starcore_ThreadTick_MethodCall 1
#define starcore_ThreadTick_Exit 2

@interface StarflutMessage :NSObject
{
@public int messageID;
@public FlutterMethodCall *call;
@public FlutterResult result;
}
@end

@implementation StarflutMessage

+(StarflutMessage*)initStarflutMessage:(int)messageID call:(FlutterMethodCall *)call result:(FlutterResult)result
{
    StarflutMessage *obj = [[StarflutMessage alloc] init];
    obj->messageID = messageID;
    obj->call = call;
    obj->result = result;
    return obj;
}

+(StarflutMessage*)initStarflutMessage:(int)messageID
{
    StarflutMessage *obj = [[StarflutMessage alloc] init];
    obj->messageID = messageID;
    obj->call = nil;
    obj->result = nil;
    return obj;
}
@end

static VS_COND waitThreadCond;
/*--add 0.9.5--*/
#define MAX_STARTHREAD_NUMBER 8

struct StructOfStarThreadWorker {
    VS_THREADID ThreadID;
    VS_COND ThreadExitCond;
    VS_BOOL IsBusy;

    NSMutableArray *starMessageQueue;
    NSRecursiveLock *starMessageQueueLock;
    NSCondition *starMessageQueueCond;
};

static struct StructOfStarThreadWorker *StarThreadWorker[MAX_STARTHREAD_NUMBER];  /*the index 0 is main thread, and will create at init stage*/
static VS_MUTEX StarThreadWorkerSyncObject;

static VS_BOOL CreateStarThreadWorker(VS_THREADID ThreadID)  /*failed if max number */
{
    vs_mutex_lock(&StarThreadWorkerSyncObject);
    for (VS_INT32 i = 0; i < MAX_STARTHREAD_NUMBER; i++) {
        if (StarThreadWorker[i] == NULL) {
            struct StructOfStarThreadWorker* Worker = new StructOfStarThreadWorker(); // (struct StructOfStarThreadWorker*)malloc(sizeof(struct StructOfStarThreadWorker));
            Worker->IsBusy = VS_FALSE;
            Worker->ThreadID = ThreadID;
            vs_cond_init(&Worker->ThreadExitCond);
            StarThreadWorker[i] = Worker;

            Worker->starMessageQueue = [[NSMutableArray alloc] init];
            Worker->starMessageQueueLock = [[NSRecursiveLock alloc] init];
            Worker->starMessageQueueCond = [[NSCondition alloc] init];

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
        if (StarThreadWorker[i] != NULL && StarThreadWorker[i]->IsBusy == VS_FALSE ) {
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

/*
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
*/

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

void sendStarMessage(struct StructOfStarThreadWorker *Worker,StarflutMessage *message)
{
    [Worker->starMessageQueueLock lock];
    [Worker->starMessageQueue addObject:message];
    [Worker->starMessageQueueLock unlock];

    [Worker->starMessageQueueCond lock];
    [Worker->starMessageQueueCond signal];
    [Worker->starMessageQueueCond unlock];
}

StarflutMessage *getStarMessage(struct StructOfStarThreadWorker *Worker)
{
    [Worker->starMessageQueueLock lock];
    while( [Worker->starMessageQueue count] == 0 ){
        [Worker->starMessageQueueLock unlock];
        [Worker->starMessageQueueCond lock];
        [Worker->starMessageQueueCond wait];
        [Worker->starMessageQueueCond unlock];
        [Worker->starMessageQueueLock lock];
    }
    StarflutMessage *message = [Worker->starMessageQueue objectAtIndex:0];
    [Worker->starMessageQueue removeObjectAtIndex:0];
    [Worker->starMessageQueueLock unlock];
    return message;
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


/*---*/
@interface StarCoreWaitResult:NSObject{
        NSCondition *cond;
@public NSObject* Result;
@public NSObject* Tag;
}

+(StarCoreWaitResult*)initStarCoreWaitResult;
-(void)dealloc;
-(void)Lock;
-(void)UnLock;
-(NSObject*)WaitResult;
-(void)SetResult:(NSObject*)result;
@end

@implementation StarCoreWaitResult
+(StarCoreWaitResult*)initStarCoreWaitResult
{
    StarCoreWaitResult* obj = [[StarCoreWaitResult alloc] init];
    obj->cond = [[NSCondition alloc] init];
    obj->Result = nil;
    obj->Tag = nil;
    return obj;
}
-(void)dealloc
{

}

-(void)Lock
{
    [cond lock];
}

-(void)UnLock
{
    [cond unlock];
}

-(NSObject*)WaitResult
{
    //[cond lock];
    [cond wait];
    //[cond unlock];
    return Result;
}
-(void)SetResult:(NSObject*)result
{
    Result = result;
    [cond lock];
    [cond signal];
    [cond unlock];
}
@end


int get_WaitResultIndex()
{
    [g_WaitResultMap_mutexObject lock];
    int Index = g_WaitResultMap_Index;
    g_WaitResultMap_Index = g_WaitResultMap_Index + 1;
    if( g_WaitResultMap_Index == 0x7FFFFFFF)
        g_WaitResultMap_Index = 1;
    [g_WaitResultMap_mutexObject unlock];
    return Index;
}

StarCoreWaitResult *new_WaitResult(int Index)
{
    [g_WaitResultMap_mutexObject lock];
    StarCoreWaitResult *m_result = [StarCoreWaitResult initStarCoreWaitResult];
    [g_WaitResultMap setObject:m_result forKey:[NSNumber numberWithInt:Index]];
    [g_WaitResultMap_mutexObject unlock];
    return m_result;
}

StarCoreWaitResult *get_WaitResult(int Index)
{
    [g_WaitResultMap_mutexObject lock];
    StarCoreWaitResult *m_result = [g_WaitResultMap objectForKey:[NSNumber numberWithInt:Index]];
    [g_WaitResultMap_mutexObject unlock];
    return m_result;
}

void remove_WaitResult(int Index)
{
    [g_WaitResultMap_mutexObject lock];
    [g_WaitResultMap removeObjectForKey:[NSNumber numberWithInt:Index]];
    [g_WaitResultMap_mutexObject unlock];
}


/*---*/
static void Starflut_SRPDispatchRequestCallBack(VS_UWORD Para)
{
    StarflutMessage *message = [StarflutMessage initStarflutMessage:starcore_ThreadTick_MessageID];
    sendStarMessage(StarThreadWorker[0],message);
}

static VS_UWORD SRPAPI GlobalMsgCallBack(VS_ULONG ServiceGroupID, VS_ULONG uMsg, VS_UWORD wParam, VS_UWORD lParam, VS_BOOL *IsProcessed, VS_UWORD Para)
{
    SetStarThreadWorkerBusy(VS_TRUE);
    switch( uMsg ){
        case MSG_VSDISPMSG :
        case MSG_VSDISPLUAMSG :
        case MSG_DISPMSG :
        case MSG_DISPLUAMSG :
        {
            //"(IIsI)"
            int w_tag = get_WaitResultIndex();
            StarCoreWaitResult *m_WaitResult = new_WaitResult(w_tag);
            NSMutableArray *cP = [[NSMutableArray alloc] init];
            [cP addObject:[NSNumber numberWithInt:ServiceGroupID]];
            [cP addObject:[NSNumber numberWithInt:uMsg]];
            [cP addObject:[NSString stringWithUTF8String:(char *)wParam]];
            [cP addObject:[NSNumber numberWithLongLong:lParam]];
            [cP addObject:[NSNumber numberWithInt:w_tag]];
            [m_WaitResult Lock];
            dispatch_async(dispatch_get_main_queue(), ^{
                /*
                - Parameters:
                - method: The name of the method to invoke.
                - arguments: The arguments. Must be a value supported by the codec of this
                channel.
                - result: A callback that will be invoked with the asynchronous result.
                The result will be a `FlutterError` instance, if the method call resulted
                    in an error on the Flutter side. Will be `FlutterMethodNotImplemented`, if
                        the method called was not implemented on the Flutter side. Any other value,
                        including `nil`, should be interpreted as successful results.
                */
                [channel invokeMethod:@"starcore_msgCallBack" arguments:cP result:^(id  _Nullable result) {
                    if( [result isKindOfClass:[FlutterError class]] ){
                        /*--critical error--*/
                        FlutterError *fe = result;
                        NSLog(@"object script callback error %@",fe.message);
                        [m_WaitResult SetResult:nil];
                        return;
                    }
#if 0
                    NSLog(@"%@",result);
                    if( [result isKindOfClass:[FlutterMethodNotImplemented class]] ){
                        /*--critical error--*/
                        [m_WaitResult SetResult:nil];
                        return;
                    }
#endif
                    NSArray *plist = (NSArray *)result;
                    id o = [plist objectAtIndex:1];
                    int w_tag = toInt32([plist objectAtIndex:0]);
                    StarCoreWaitResult *t_WaitResult = nil;
                    if( w_tag != 0 )
                        t_WaitResult = get_WaitResult(w_tag);
                    else
                        t_WaitResult = nil;
                    if( o == nil || (!([o isKindOfClass:[NSArray class]])) ){
                        if( t_WaitResult != nil )
                            [t_WaitResult SetResult:nil];
                        return;
                    }
                    if( t_WaitResult != nil ){
                        [t_WaitResult SetResult:o];
                    }else{
                        NSLog(@"MsgCallBack can not return value");
                    }
                    return;
                }];
            });
            SRPControlInterface->SRPUnLock();
            //id result = [m_WaitResult WaitResult];
            [m_WaitResult WaitResult];
            [m_WaitResult UnLock];
            remove_WaitResult(w_tag);
            SRPControlInterface->SRPLock();
            SetStarThreadWorkerBusy(VS_FALSE);
            return 0;
        }
        default :
            SetStarThreadWorkerBusy(VS_FALSE);
            return 0;
    }
}

static bool StartsWithString(NSString *str1,NSString *str2)
{
    if( [str1 length] < [str2 length])
        return false;
    NSString* str3 = [str1 substringWithRange:NSMakeRange(0, [str2 length])];
    NSComparisonResult result = [str3 caseInsensitiveCompare:str2];
    if( result == NSOrderedAscending )
        return false;
    else if( result == NSOrderedSame )
        return true;
    return false;
}

static VS_BOOL StarParaPkg_FromTuple_Sub(NSArray *tuple, VS_INT32 StartIndex,class ClassOfSRPParaPackageInterface *ParaPkg, VS_INT32 PkgStartIndex)
{
    VS_INT32 Index;

    if( StartIndex == 0 && (tuple == nil || [tuple count] == 0 ) ){
        return VS_TRUE;
    }
    if( StartIndex >= [tuple count] ){
        return VS_FALSE;
    }
    for(Index = PkgStartIndex;Index< PkgStartIndex + [tuple count]-StartIndex;Index++){
        id valueobj = [tuple objectAtIndex:Index-PkgStartIndex+StartIndex];
        if( valueobj == nil || [valueobj isKindOfClass:[NSNull class]] ){
            ParaPkg -> InsertEmpty(Index);
        }else if( [valueobj isKindOfClass:[NSNumber class]] ){
            if (strcmp([valueobj objCType], @encode(char)) == 0 || strcmp([valueobj objCType], @encode(bool)) == 0 )
                ParaPkg -> InsertBool(Index,toBool(valueobj));
            else if(strcmp([valueobj objCType], @encode(int)) == 0)
                ParaPkg -> InsertInt(Index,toInt32(valueobj));
            else if(strcmp([valueobj objCType], @encode(long)) == 0)
                ParaPkg -> InsertInt64(Index,toInt64(valueobj));
            else if(strcmp([valueobj objCType], @encode(double)) == 0)
                ParaPkg -> InsertFloat(Index,toDouble(valueobj));
            else{
                return VS_FALSE;
            }
        }else if( [valueobj isKindOfClass:[NSString class]] ){
            NSString *str = valueobj;
            if( StartsWithString(str,StarBinBufPrefix)){
                class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:str]);
                if( l_BinBuf == NULL ){
                    NSLog(@"binbuf object[%@] can not be found..",str);
                    return VS_FALSE;
                }
                ParaPkg -> InsertBinEx( Index, l_BinBuf -> GetBuf(),l_BinBuf -> GetOffset(), l_BinBuf -> IsFromRaw() );
            }else if( StartsWithString(str,StarParaPkgPrefix)){
                class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:str]);
                if( l_ParaPkg == NULL ){
                    NSLog(@"parapkg object[%@] can not be found..",str);
                    return VS_FALSE;
                }
                ParaPkg -> InsertParaPackage( Index, l_ParaPkg );
            }else if( StartsWithString(str,StarObjectPrefix)){
                void *SRPObject = (void *)toPointer([CleObjectMap objectForKey:str]);
                if( SRPObject == NULL ){
                    NSLog(@"starobject object[%@] can not be found..",str);
                    return VS_FALSE;
                }
                ParaPkg -> InsertObject( Index, SRPObject);
            }else{
                VS_INT32 Length;
                VS_CHAR *CharPtr = toStringEx(valueobj,&Length);
                ParaPkg -> InsertStrEx(Index,CharPtr,(VS_UINT32)Length );
            }
        }else if( [valueobj isKindOfClass:[NSArray class]] ){
            class ClassOfSRPParaPackageInterface *l_ParaPkg = BasicSRPInterface ->GetParaPkgInterface();
            if( StarParaPkg_FromTuple_Sub(valueobj,0, l_ParaPkg, 0) == VS_FALSE ){
                l_ParaPkg ->Release();
                return VS_FALSE;
            }
            ParaPkg -> InsertParaPackage( Index, l_ParaPkg );
            l_ParaPkg ->Release();
        }else if( [valueobj isKindOfClass:[NSDictionary class]] ){
            NSMutableArray *newt = [[NSMutableArray alloc] init];
            for (id key in valueobj) {
                id value = valueobj[key];
                [newt addObject:key];
                [newt addObject:value];
            }
            class ClassOfSRPParaPackageInterface *l_ParaPkg = BasicSRPInterface ->GetParaPkgInterface();
            if( StarParaPkg_FromTuple_Sub(newt,0,l_ParaPkg,0) == VS_FALSE ){
                l_ParaPkg ->Release();
                return VS_FALSE;
            }
            l_ParaPkg ->AsDict(VS_TRUE);
            ParaPkg -> InsertParaPackage( Index, l_ParaPkg );
            l_ParaPkg ->Release();
        }else{
            return VS_FALSE;
        }
    }
    return VS_TRUE;
}

static id StarParaPkg_GetAtIndex(class ClassOfSRPParaPackageInterface *ParaPkg, VS_INT32 Index);
static id StarParaPkg_ToTuple(class ClassOfSRPParaPackageInterface *l_ParaPkg)
{
    int Number = l_ParaPkg ->GetNumber();
    NSMutableArray *oo = [[NSMutableArray alloc] init];
    for( int i=0; i < Number ; i++ ){
        id vv = StarParaPkg_GetAtIndex(l_ParaPkg, i);
        if( vv != nil )
            [oo addObject:vv];
        else
            [oo addObject:[NSNull null]];
    }
    if( l_ParaPkg -> IsDict() == VS_TRUE && [oo count] % 2 == 0){
        /*--convert to NSDictionary--*/
        NSMutableDictionary *nd = [[NSMutableDictionary alloc] init];
        for( int ii=0; ii < ([oo count] / 2); ii++ ){
            [nd setValue:[oo objectAtIndex:ii*2+1] forKey:[oo objectAtIndex:ii*2]];
        }
        return nd;
    }else
        return oo;
}

static id StarParaPkg_GetAtIndex(class ClassOfSRPParaPackageInterface *ParaPkg, VS_INT32 Index)
{
    class ClassOfSRPBinBufInterface *SRPBinBufInterface;
    class ClassOfSRPParaPackageInterface *SRPParaPackageInterface;
    VS_INT32 Length;
    void *Buf;
    void *SRPObject=NULL;

    if( Index >= ParaPkg -> GetNumber() )
        return nil;
    switch( ParaPkg -> GetType(Index) ){
        case SRPPARATYPE_BOOL :
            return fromBool(ParaPkg -> GetBool(Index) );
        case SRPPARATYPE_INT :
            return fromInt32(ParaPkg -> GetInt(Index) );
        case SRPPARATYPE_INT64 :
            return fromInt64(ParaPkg -> GetInt64(Index) );
        case SRPPARATYPE_FLOAT :
            return fromDouble(ParaPkg -> GetFloat(Index) );
        case SRPPARATYPE_BIN :
        {
            VS_BOOL FromRaw;
            Buf = ParaPkg -> GetBinEx(Index,&Length,&FromRaw);
            if( Buf == NULL )
                return nil;
            if( FromRaw == VS_FALSE ){
                SRPBinBufInterface = BasicSRPInterface -> GetSRPBinBufInterface();
                SRPBinBufInterface -> Set(0,Length,(VS_INT8 *)Buf);
                NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarBinBufPrefix,[[NSUUID UUID] UUIDString]];
                [CleObjectMap setObject:fromPointer(SRPBinBufInterface) forKey:CleObjectID];
                return CleObjectID;
            }else{
                NSData *adata = [[NSData alloc] initWithBytes:Buf length:Length];
                return [FlutterStandardTypedData typedDataWithBytes:adata];
            }
        }
        case SRPPARATYPE_CHARPTR :
        {
            VS_CHAR *Str;
            VS_UINT32 StrLen;
            Str = ParaPkg -> GetStrEx(Index,&StrLen);
            if( StrLen == 0 )
                return @"";
            else{
                NSData *adata = [[NSData alloc] initWithBytes:Str length:StrLen];
                NSString *dataString = [[NSString alloc]initWithData:adata encoding:NSUTF8StringEncoding];
                if( dataString != nil )
                    return dataString;
                return [FlutterStandardTypedData typedDataWithBytes:adata];
            }
        }
        case SRPPARATYPE_OBJECT :
        {
            SRPObject = ParaPkg -> GetObject(Index);
            if( SRPObject == NULL )
                return nil;
            /*
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(SRPObject);
            class ClassOfSRPParaPackageInterface *l_ParaPkg = l_Service -> RawToParaPkg(SRPObject);
             */
            class ClassOfSRPParaPackageInterface *l_ParaPkg = NULL;
            if( l_ParaPkg != NULL ){
                return StarParaPkg_ToTuple(l_ParaPkg);
            }else{
                VS_UUID ObjectID;
                class ClassOfSRPInterface *l_Service  =SRPInterface ->GetSRPInterface(SRPObject);
                l_Service ->AddRefEx(SRPObject);
                l_Service ->GetID(SRPObject, &ObjectID);
                NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarObjectPrefix,[[NSUUID UUID] UUIDString]];
                [CleObjectMap setObject:fromPointer(SRPObject) forKey:CleObjectID];
                return [NSString stringWithFormat:@"%@+%@",CleObjectID,fromString(l_Service->UuidToString(&ObjectID))];
            }
        }
        case SRPPARATYPE_PARAPKG :
        {
            SRPParaPackageInterface = ParaPkg -> GetParaPackage(Index);
            if( SRPParaPackageInterface == NULL )
                return nil;
            return StarParaPkg_ToTuple(SRPParaPackageInterface);
            /*
            SRPParaPackageInterface ->AddRef();
            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarParaPkgPrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(SRPParaPackageInterface) forKey:CleObjectID];
            return CleObjectID;
             */
        }
        case SRPPARATYPE_INVALID :
            return nil;
    }
    return nil;
}

static id SRPObject_AttributeToFlutterObject(VS_ATTRIBUTEINFO *AttributeInfo,VS_UINT8 AttributeIndex, class ClassOfSRPInterface *SRPInterface, VS_UINT8 AttributeType, VS_INT32 AttributeLength, VS_UUID *StructID, VS_ULONG BufOffset, VS_UINT8 *Buf, VS_BOOL UseStructObject)
{
    void *AtomicStructObject;

    switch( AttributeType ){
        case VSTYPE_BOOL :
            return fromBool(((VS_BOOL *)&Buf[BufOffset])[0] );

        case VSTYPE_INT8 :
            return fromInt32(((VS_INT8 *)&Buf[BufOffset])[0] );

        case VSTYPE_UINT8 :
            return fromInt32(((VS_INT8 *)&Buf[BufOffset])[0] );

        case VSTYPE_INT16 :
            return fromInt32(((VS_INT16 *)&Buf[BufOffset])[0] );

        case VSTYPE_UINT16 :
            return fromInt32(((VS_UINT16 *)&Buf[BufOffset])[0] );

        case VSTYPE_INT32 :
            return fromInt32(((VS_INT32 *)&Buf[BufOffset])[0] );

        case VSTYPE_UINT32 :
            return fromInt32(((VS_UINT32 *)&Buf[BufOffset])[0] );

        case VSTYPE_INT64 :
            return fromInt64(((VS_INT64 *)&Buf[BufOffset])[0] );

        case VSTYPE_FLOAT :
            return fromDouble(((VS_FLOAT *)&Buf[BufOffset])[0] );

        case VSTYPE_DOUBLE :
            return fromDouble(((VS_DOUBLE *)&Buf[BufOffset])[0] );

        case VSTYPE_LONG :
            return fromInt32(((VS_LONG *)&Buf[BufOffset])[0] );

        case VSTYPE_ULONG :
            return fromInt32(((VS_ULONG *)&Buf[BufOffset])[0] );

        case VSTYPE_LONGHEX :
            return fromInt32(((VS_LONG *)&Buf[BufOffset])[0] );

        case VSTYPE_ULONGHEX :
            return fromInt32(((VS_ULONG *)&Buf[BufOffset])[0] );

        case VSTYPE_VSTRING :
            if( ((VS_VSTRING *)&Buf[BufOffset]) -> Buf == NULL ){
                return @"";
            }else{
                return fromString(((VS_VSTRING *)&Buf[BufOffset]) -> Buf );
            }

    case VSTYPE_PTR :
        {
            if( AttributeInfo->SyncType != 0 ){
                void *SRPObject = SRPInterface ->QueryFirst(((void **)&Buf[BufOffset])[0]);
                if( SRPObject == NULL )
                    return nil;
                VS_UUID ObjectID;
                class ClassOfSRPInterface *l_Service  =SRPInterface ->GetSRPInterface(SRPObject);
                l_Service ->AddRefEx(SRPObject);
                l_Service ->GetID(SRPObject, &ObjectID);
                NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarObjectPrefix,[[NSUUID UUID] UUIDString]];
                [CleObjectMap setObject:fromPointer(SRPObject) forKey:CleObjectID];
                return [NSString stringWithFormat:@"%@+%@",CleObjectID,fromString(l_Service->UuidToString(&ObjectID))];
            }else{
#if defined(VSPLAT_64)
                return fromInt64(((VS_UWORD *)&Buf[BufOffset])[0]);
#else
                return fromInt32(((VS_UWORD *)&Buf[BufOffset])[0]);
#endif
            }
        }

        case VSTYPE_STRUCT :
            AtomicStructObject = SRPInterface -> GetAtomicObject(StructID);
            if( AtomicStructObject == NULL )
                return nil;
            {
                NSMutableArray *NewObject;
                VS_INT32 AttributeNumber,i;
                VS_ATTRIBUTEINFO StructAttributeInfo;

                AttributeNumber = SRPInterface -> GetAtomicStructAttributeNumber(AtomicStructObject);
                NewObject = [[NSMutableArray alloc] init];
                for(i=0;i<AttributeNumber;i++){
                    if( SRPInterface -> GetAtomicStructAttributeInfoEx( AtomicStructObject, i, &StructAttributeInfo ) == VS_TRUE ){
                        id vv = SRPObject_AttributeToFlutterObject( &StructAttributeInfo,StructAttributeInfo.AttributeIndex,SRPInterface,StructAttributeInfo.Type, StructAttributeInfo.Length,&StructAttributeInfo.StructID,StructAttributeInfo.Offset + BufOffset,(VS_UINT8 *)Buf,UseStructObject);
                        if( vv != nil )
                            [NewObject addObject:vv];
                        else
                            [NewObject addObject:[NSNull null]];
                    }
                }
                return NewObject;
            }

        case VSTYPE_COLOR :
            return fromInt32(((VS_COLOR *)&Buf[BufOffset])[0] );

        case VSTYPE_RECT :
            return nil;

        case VSTYPE_FONT :
            return nil;

        case VSTYPE_TIME :
            return nil;

        case VSTYPE_CHAR :
            if( AttributeLength == 1 ){
                return fromInt32(((VS_CHAR *)&Buf[BufOffset])[0] );
            }else{
                return fromString((VS_CHAR *)&Buf[BufOffset]);
            }
        case VSTYPE_UUID :
        case VSTYPE_STATICID :
            return fromString(SRPInterface -> UuidToString( (VS_UUID *)&Buf[BufOffset] ));

        default :
            return nil;
    }
}

static VS_BOOL SRPObject_FlutterObjectToAttribute(class ClassOfSRPInterface *SRPInterface, VS_UINT8 AttributeType, VS_INT32 AttributeLength, VS_UUID *StructID, id ObjectTemp, VS_ULONG BufOffset, VS_UINT8 *Buf)
{
    VS_CHAR *CharValue;
    void *AtomicStruct;
    VS_ATTRIBUTEINFO AttributeInfo;

    switch( AttributeType ){
        case VSTYPE_BOOL :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_BOOL *)&Buf[BufOffset])[0] = toBool(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_INT8 :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_INT8 *)&Buf[BufOffset])[0] = (VS_INT8)toInt32(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_UINT8 :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_UINT8 *)&Buf[BufOffset])[0] = (VS_UINT8)toInt32(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_INT16 :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_INT16 *)&Buf[BufOffset])[0] = (VS_INT16)toInt32(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_UINT16 :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_UINT16 *)&Buf[BufOffset])[0] = (VS_UINT16)toInt32(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_INT32 :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_INT32 *)&Buf[BufOffset])[0] = (VS_INT32)toInt32(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_UINT32 :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_UINT32 *)&Buf[BufOffset])[0] = (VS_UINT32)toInt32(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_INT64 :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_INT64 *)&Buf[BufOffset])[0] = (VS_INT64)toInt64(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_FLOAT :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_FLOAT *)&Buf[BufOffset])[0] = (VS_FLOAT)toDouble(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_DOUBLE :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_DOUBLE *)&Buf[BufOffset])[0] = (VS_DOUBLE)toDouble(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_LONG :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_INT32 *)&Buf[BufOffset])[0] = (VS_INT32)toInt32(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_ULONG :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_UINT32 *)&Buf[BufOffset])[0] = (VS_UINT32)toInt32(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_LONGHEX :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_INT32 *)&Buf[BufOffset])[0] = (VS_INT32)toInt32(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_ULONGHEX :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_UINT32 *)&Buf[BufOffset])[0] = (VS_UINT32)toInt32(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_VSTRING :
            CharValue = toString(ObjectTemp);
            if( CharValue == NULL )
                return VS_FALSE;
            strcpy( (char *)&Buf[BufOffset], CharValue );
            return VS_TRUE;

        case VSTYPE_STRUCT :
            if( ObjectTemp == NULL || (![ObjectTemp isKindOfClass:[NSArray class]]) )
                return VS_FALSE;
            else{
                VS_INT32 num_index,i;
                id LocalObjectTemp;

                AtomicStruct = SRPInterface -> GetAtomicObject(StructID);
                if( AtomicStruct == NULL )
                    return VS_FALSE;
                for(num_index=0;num_index<[ObjectTemp count];num_index++){
                    i = num_index;
                    if( i >= 0 && i < SRPInterface -> GetAtomicStructAttributeNumber(AtomicStruct) ){
                        SRPInterface -> GetAtomicStructAttributeInfoEx( AtomicStruct, i, &AttributeInfo );
                        LocalObjectTemp = [ObjectTemp objectAtIndex:num_index];
                        if( SRPObject_FlutterObjectToAttribute(SRPInterface,AttributeInfo.Type, AttributeInfo.Length,NULL,LocalObjectTemp,AttributeInfo.Offset+BufOffset,Buf ) == VS_FALSE ){
                            return VS_FALSE;
                        }
                    }
                }
            }
            return VS_TRUE;

        case VSTYPE_COLOR :
            if( [ObjectTemp isKindOfClass:[NSNumber class]] ){
                ((VS_UINT32 *)&Buf[BufOffset])[0] = (VS_UINT32)toInt32(ObjectTemp);
                return VS_TRUE;
            }else
                return VS_FALSE;

        case VSTYPE_RECT :
            return VS_FALSE;

        case VSTYPE_FONT :
            return VS_FALSE;

        case VSTYPE_TIME :
            return VS_FALSE;

        case VSTYPE_CHAR :
            CharValue = toString(ObjectTemp);
            if( CharValue == NULL )
                return VS_FALSE;
            strncpy( (char *)&Buf[BufOffset], CharValue, AttributeLength );
            ((VS_CHAR *)&Buf[BufOffset])[AttributeLength-1] = 0;
            return VS_TRUE;

        case VSTYPE_UUID :
        case VSTYPE_STATICID :
            CharValue = toString(ObjectTemp);
            if( CharValue == NULL )
                return VS_FALSE;
            SRPInterface -> StringToUuid( CharValue, (VS_UUID *)&Buf[BufOffset] );
            return VS_TRUE;

        default :
            return VS_FALSE;
    }
    return VS_FALSE;
}

static id LuaToFlutterObject ( class ClassOfSRPInterface *SRPInterface, VS_INT32 Index,VS_BOOL *Result );
static VS_BOOL FlutterObjectToLua ( class ClassOfSRPInterface *SRPInterface, id valueobj );

static id LuaTableToFlutter(class ClassOfSRPInterface *SRPInterface, VS_INT32 Index)
{
    NSMutableArray *RetVal;
    id Val;
    VS_INT32 IntTemp,Size;

    if( Index < 0 )
        Index = Index - 1;
    Size = 0;
    SRPInterface -> LuaPushNil();
    while( SRPInterface -> LuaNext( Index ) != 0 ){
        switch( SRPInterface -> LuaType(-2) ){
            case VSLUATYPE_INT :
            case VSLUATYPE_INT64 :
            case VSLUATYPE_UWORD :
            case VSLUATYPE_NUMBER :
                IntTemp = SRPInterface -> LuaToInt(-2);
                if( IntTemp > 0 ){
                    if( IntTemp > Size )
                        Size = IntTemp;
                }else{
                    SRPInterface -> LuaPop(2);
                    return nil;
                }
                break;
            default :
                SRPInterface -> LuaPop(2);
                return nil;
        }
        SRPInterface -> LuaPop(1);
    }
    RetVal = [NSMutableArray arrayWithCapacity:Size];
    SRPInterface -> LuaPushNil();
    while( SRPInterface -> LuaNext( Index ) != 0 ){
        switch( SRPInterface -> LuaType(-2) ){
            case VSLUATYPE_INT :
            case VSLUATYPE_INT64 :
            case VSLUATYPE_UWORD :
            case VSLUATYPE_NUMBER :
                IntTemp = SRPInterface -> LuaToInt(-2);
                if( IntTemp > 0 && IntTemp <= Size ){
                    VS_BOOL Result;
                    Val = LuaToFlutterObject( SRPInterface, -1, &Result );
                    if( Result == VS_FALSE ){
                        SRPInterface -> LuaPop(2);
                        return nil;
                    }
                    if( Val != nil )
                        RetVal[IntTemp-1] = Val;
                    else
                        RetVal[IntTemp-1] = [NSNull null];
                }else{
                    SRPInterface -> LuaPop(2);
                    return nil;
                }
                break;
            default :
                SRPInterface -> LuaPop(2);
                return nil;
        }
        SRPInterface -> LuaPop(1);
    }
    return RetVal;
}

#if 0
static void FlutterToLuaTable(ClassOfSRPInterface *SRPInterface, id arr_data)
{
    VS_INT32 num_index;
    id localobject;

    SRPInterface -> LuaNewTable();
    for( num_index = 0;num_index<[arr_data count];num_index++){
        SRPInterface -> LuaPushInt(num_index+1);
        localobject = [arr_data objectAtIndex:num_index];
        FlutterObjectToLua( SRPInterface, localobject );
        if( SRPInterface -> LuaIsNil( -1 ) == VS_TRUE ){
            SRPInterface -> LuaPop(3);
            SRPInterface -> LuaPushNil();
            return;
        }
        SRPInterface -> LuaSetTable( -3 );
    }
    return ;
}
#endif

static id LuaToFlutterObject ( class ClassOfSRPInterface *SRPInterface, VS_INT32 Index,VS_BOOL *Result )
{
    (*Result) = VS_TRUE;
    switch( SRPInterface -> LuaType( Index ) ){
    case VSLUATYPE_INT : return fromInt32(SRPInterface -> LuaToInt(Index));
    case VSLUATYPE_INT64 : return fromInt64(SRPInterface -> LuaToInt64(Index));
    case VSLUATYPE_UWORD :
        {
#if defined(ENV_M64)
            return fromInt64(SRPInterface -> LuaToUWord(Index));
#else
            return fromInt32(SRPInterface -> LuaToUWord(Index));
#endif
        }
    case VSLUATYPE_NUMBER : return fromDouble(SRPInterface -> LuaToNumber(Index));
    case VSLUATYPE_BOOL :   return fromBool(SRPInterface -> LuaToBool(Index));
    case VSLUATYPE_STRING :
        {
            VS_CHAR *StringBuf;
            VS_ULONG StringSize;

            StringBuf = SRPInterface ->LuaToLString(Index,&StringSize);
            if( StringBuf != NULL ){
                NSData *adata = [[NSData alloc] initWithBytes:StringBuf length:StringSize];
                NSString *dataString = [[NSString alloc]initWithData:adata encoding:NSUTF8StringEncoding];
                if( dataString != nil )
                    return dataString;
                return [FlutterStandardTypedData typedDataWithBytes:adata];
            }else{
                return @"";
            }
        }
        case VSLUATYPE_TABLE :
            if( SRPInterface ->LuaTableToParaPkg(Index,NULL,VS_TRUE) == VS_TRUE )
                return LuaTableToFlutter( SRPInterface, Index );
        case VSLUATYPE_FUNCTION :
        case VSLUATYPE_USERDATA :
        case VSLUATYPE_LIGHTUSERDATA :
        {
            void *SRPObject = SRPInterface ->LuaToRaw(Index, VS_FALSE);
            if( SRPObject == NULL )
                return nil;
            VS_UUID ObjectID;
            SRPInterface ->GetID(SRPObject, &ObjectID);
            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarObjectPrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(SRPObject) forKey:CleObjectID];
            return [NSString stringWithFormat:@"%@+%@",CleObjectID,fromString(SRPInterface->UuidToString(&ObjectID))];
        }
        case VSLUATYPE_OBJECT :
        {
            void *SRPObject = SRPInterface ->LuaToObject(Index);
            if( SRPObject == NULL )
                return nil;
            /*
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(SRPObject);
            class ClassOfSRPParaPackageInterface *l_ParaPkg = l_Service -> RawToParaPkg(SRPObject);
            */
            class ClassOfSRPParaPackageInterface *l_ParaPkg = NULL;
            if( l_ParaPkg != NULL )
                return StarParaPkg_ToTuple(l_ParaPkg);
            else{
                VS_UUID ObjectID;
                SRPInterface ->AddRefEx(SRPObject);
                SRPInterface ->GetID(SRPObject, &ObjectID);
                NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarObjectPrefix,[[NSUUID UUID] UUIDString]];
                [CleObjectMap setObject:fromPointer(SRPObject) forKey:CleObjectID];
                return [NSString stringWithFormat:@"%@+%@",CleObjectID,fromString(SRPInterface->UuidToString(&ObjectID))];
            }
        }
        case VSLUATYPE_PARAPKG :
        {
            class ClassOfSRPParaPackageInterface *SRPParaPackageInterface = SRPInterface -> LuaToParaPkg(Index);
            if( SRPParaPackageInterface == NULL )
                return nil;
            return StarParaPkg_ToTuple(SRPParaPackageInterface);
            /*
            SRPParaPackageInterface ->AddRef();
            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarParaPkgPrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(SRPParaPackageInterface) forKey:CleObjectID];
            return CleObjectID;
             */
        }
        case VSLUATYPE_BINBUF :
            {
                class ClassOfSRPBinBufInterface *BinBuf = SRPInterface -> LuaToBinBuf(Index);
                VS_INT8 *Buf = BinBuf->GetBuf();
                VS_BOOL FromRaw = BinBuf->IsFromRaw();
                VS_INT32 Length = BinBuf ->GetOffset();

                if( Buf == NULL )
                    return nil;
                if( FromRaw == VS_FALSE ){
                    BinBuf ->AddRef();
                    NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarBinBufPrefix,[[NSUUID UUID] UUIDString]];
                    [CleObjectMap setObject:fromPointer(BinBuf) forKey:CleObjectID];
                    return CleObjectID;
                }else{
                    NSData *adata = [[NSData alloc] initWithBytes:Buf length:Length];
                    return [FlutterStandardTypedData typedDataWithBytes:adata];
                }
            }
        case VSLUATYPE_NIL  : return nil;
        default : (*Result) = VS_FALSE;return nil;
    }
}

static VS_BOOL FlutterObjectToLua ( class ClassOfSRPInterface *SRPInterface, id valueobj )
{
    if( [valueobj isKindOfClass:[NSNumber class]] ){
        if (strcmp([valueobj objCType], @encode(char)) == 0 || strcmp([valueobj objCType], @encode(bool)) == 0 ){
            SRPInterface -> LuaPushBool(toBool(valueobj));
            return VS_TRUE;
        }else if(strcmp([valueobj objCType], @encode(int)) == 0){
            SRPInterface -> LuaPushInt(toInt32(valueobj));
            return VS_TRUE;
        }else if(strcmp([valueobj objCType], @encode(long)) == 0){
            SRPInterface -> LuaPushInt64( toInt64(valueobj));
            return VS_TRUE;
        }else if(strcmp([valueobj objCType], @encode(double)) == 0){
            SRPInterface -> LuaPushNumber( toDouble(valueobj));
            return VS_TRUE;
        }else{
            return VS_FALSE;
        }
    }else if( [valueobj isKindOfClass:[NSString class]] ){
        NSString *str = valueobj;
        if( StartsWithString(str,StarBinBufPrefix)){
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:str]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",str);
                return VS_FALSE;
            }
            SRPInterface -> LuaPushBinBuf( l_BinBuf,VS_FALSE );
            return VS_TRUE;
        }else if( StartsWithString(str,StarParaPkgPrefix)){
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:str]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",str);
                return VS_FALSE;
            }
            SRPInterface -> LuaPushParaPkg( l_ParaPkg,VS_FALSE);
            return VS_TRUE;
        }else if( StartsWithString(str,StarObjectPrefix)){
            void *SRPObject = (void *)toPointer([CleObjectMap objectForKey:str]);
            if( SRPObject == NULL ){
                NSLog(@"starobject object[%@] can not be found..",str);
                return VS_FALSE;
            }
            SRPInterface -> LuaPushObject( SRPObject );
            return VS_TRUE;
        }else{
            VS_INT32 Length;
            VS_CHAR *CharPtr = toStringEx(valueobj,&Length);
            SRPInterface -> LuaPushLString(CharPtr,Length);
            return VS_TRUE;
        }
    }else if( [valueobj isKindOfClass:[NSArray class]] ){
        class ClassOfSRPParaPackageInterface *l_ParaPkg = BasicSRPInterface ->GetParaPkgInterface();
        if( StarParaPkg_FromTuple_Sub(valueobj,0, l_ParaPkg, 0) == VS_FALSE ){
            l_ParaPkg ->Release();
            return VS_FALSE;
        }
        SRPInterface -> LuaPushParaPkg( l_ParaPkg,VS_TRUE);
        return VS_TRUE;
    }else if( [valueobj isKindOfClass:[NSDictionary class]] ){
        NSMutableArray *newt = [[NSMutableArray alloc] init];
        for (id key in valueobj) {
            id value = valueobj[key];
            [newt addObject:key];
            [newt addObject:value];
        }
        class ClassOfSRPParaPackageInterface *l_ParaPkg = BasicSRPInterface ->GetParaPkgInterface();
        if( StarParaPkg_FromTuple_Sub(newt,0,l_ParaPkg,0) == VS_FALSE ){
            l_ParaPkg ->Release();
            return VS_FALSE;
        }
        l_ParaPkg ->AsDict(VS_TRUE);
        SRPInterface -> LuaPushParaPkg( l_ParaPkg,VS_TRUE);
        return VS_TRUE;
    }else if( valueobj == nil || [valueobj isKindOfClass:[NSNull class]]){
        SRPInterface -> LuaPushNil();
        return VS_TRUE;
    }else{
        return VS_FALSE;
    }
}

id StarObject_getValue(class ClassOfSRPInterface *SRPInterface,void *SRPObject,id Name)
{
    VS_CHAR LocalNameBuf[64];
    VS_CHAR * ParaName;

    if( [Name isKindOfClass:[NSString class]] ){
        ParaName=toString(Name);
    }else{
        if( [Name isKindOfClass:[NSNumber class]] ){
            sprintf( LocalNameBuf, "\"%d\"", toInt32( Name) );
            ParaName = LocalNameBuf;
        }else{
            return nil;
        }
    }
    if( ParaName[0] == '_' && ParaName[1] == '_' && ParaName[2] == '_' ){
        //---process object namevalue
        VS_UINT8 Type;
        VS_INT32 IntValue;
        VS_DOUBLE FloatValue;
        VS_BOOL BoolValue;

        Type = SRPInterface -> GetNameValueType( SRPObject, &ParaName[3] );
        switch( Type ){
            case SRPPARATYPE_BOOL :
                SRPInterface -> GetNameBoolValue( SRPObject, &ParaName[3], &BoolValue, VS_FALSE );
                return fromBool(BoolValue);
            case SRPPARATYPE_INT :
                SRPInterface -> GetNameIntValue( SRPObject, &ParaName[3], &IntValue, 0 );
                return fromInt32(IntValue);
            case SRPPARATYPE_FLOAT :
                SRPInterface -> GetNameFloatValue( SRPObject, &ParaName[3], &FloatValue, 0 );
                return fromDouble(FloatValue );
            case SRPPARATYPE_CHARPTR :
                return fromString(SRPInterface -> GetNameStrValue( SRPObject, &ParaName[3], "" ) );
            default:
                return nil;
        }
    }
    if( vs_string_strcmp( ParaName, "_Service") == 0 ){
        VS_UUID ObjectID;
        SRPInterface ->AddRef();
        SRPInterface->GetServiceID(&ObjectID);
        NSString *newServiceID = fromString(SRPInterface->UuidToString(&ObjectID));

        NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarServicePrefix,[[NSUUID UUID] UUIDString]];
        [CleObjectMap setObject:fromPointer(SRPInterface) forKey:CleObjectID];
        return [NSString stringWithFormat:@"%@+%@",CleObjectID,newServiceID];
    }else if( vs_string_strcmp( ParaName, "_Class") == 0 ){
        void *SRPObjectTemp = SRPInterface -> GetClass(SRPObject);
        if( SRPObjectTemp == NULL )
            return nil;
        VS_UUID ObjectID;
        SRPInterface ->AddRefEx(SRPObjectTemp);
        SRPInterface ->GetID(SRPObject, &ObjectID);
        NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarObjectPrefix,[[NSUUID UUID] UUIDString]];
        [CleObjectMap setObject:fromPointer(SRPObject) forKey:CleObjectID];
        return [NSString stringWithFormat:@"%@+%@",CleObjectID,fromString(SRPInterface->UuidToString(&ObjectID))];
    }else if( vs_string_strcmp( ParaName, "_ID") == 0 ){
        VS_UUID IDTemp;

        SRPInterface -> GetID(SRPObject,&IDTemp);
        return fromString(SRPInterface -> UuidToString(&IDTemp));
    }else if( vs_string_strcmp( ParaName, "_Name") == 0 ){
        return fromString(SRPInterface -> GetName(SRPObject));
    }
    //---check if is object's attribute
    VS_ATTRIBUTEINFO AttributeInfo;
    if( SRPInterface -> GetAttributeInfoEx(SRPInterface -> GetClass(SRPObject),ParaName,&AttributeInfo) == VS_TRUE ){
        return SRPObject_AttributeToFlutterObject( &AttributeInfo,AttributeInfo.AttributeIndex,SRPInterface,AttributeInfo.Type, AttributeInfo.Length,&AttributeInfo.StructID,AttributeInfo.Offset,(VS_UINT8 *)SRPObject,VS_TRUE );
    }else{
        SRPInterface ->LuaPushObject(SRPObject);
        SRPInterface ->LuaPushString(ParaName);
        SRPInterface ->LuaGetTable(-2);
        if( SRPInterface ->LuaIsNil(-1) == VS_FALSE ){
            VS_BOOL LuaToFlutterResult;

            if( SRPInterface ->LuaIsFunction(-1) == VS_TRUE ){
                SRPInterface ->LuaPop(2);
                //--is lua function, return NULL, should use call
                return nil;
            }
            //--change lua attribute to java attribute
            id ObjectTemp1 = LuaToFlutterObject( SRPInterface, -1, &LuaToFlutterResult );
            SRPInterface ->LuaPop(2);
            if( LuaToFlutterResult == VS_FALSE ){
                SRPInterface->ProcessError(VSFAULT_WARNING,__FILE__,__LINE__,"Get Object[%s] Attribute [%s] Error",SRPInterface -> GetName(SRPObject),ParaName);
                return nil;
            }
            return ObjectTemp1;
        }
        SRPInterface ->LuaPop(2);
        return nil;
    }
}

static VS_INT32 SRPObject_ScriptCallBack(void *L)
{
    SetStarThreadWorkerBusy(VS_TRUE);
    //VS_ULONG ServiceGroupID = SRPControlInterface ->LuaGetInt( L, SRPControlInterface->LuaUpValueIndex(L, 1) );
    VS_CHAR *ScriptName = SRPInterface -> LuaToString( SRPInterface -> LuaUpValueIndex(3) );
    void *Object = SRPInterface -> LuaToObject(1);

    class ClassOfSRPInterface *l_Service = SRPInterface ->GetSRPInterface(Object);
    if( l_Service -> IsRemoteCall(Object) == VS_TRUE )
        l_Service -> SetRetCode(Object,VSRCALL_PARAERROR);
    //---create parameter
    if( l_Service -> LuaGetTop() == 0 ){
        l_Service->ProcessError(VSFAULT_WARNING, __FILE__,__LINE__,"Call Object[%s] FlutterFunction [%s] Error Parameter Number ",l_Service -> GetName(Object),ScriptName );
        SetStarThreadWorkerBusy(VS_FALSE);
        return 0;
    }
    VS_INT32 n = SRPInterface -> LuaGetTop() - 1;
    NSMutableArray *out = [NSMutableArray arrayWithCapacity:n];
    VS_BOOL LuaToFlutterResult;
    for( VS_INT32 i=0; i < n; i++ ){
        id vv = LuaToFlutterObject( SRPInterface, i+2, &LuaToFlutterResult );
        if( vv != nil )
            out[i] = vv;
        else
            out[i] = [NSNull null];
        if( LuaToFlutterResult == VS_FALSE ){
            l_Service->ProcessError(VSFAULT_WARNING, __FILE__,__LINE__,"Call Object[%s] JavaFunction [%s] Error,Parameter[%d] failed ",l_Service -> GetName(Object),ScriptName,i );
            SetStarThreadWorkerBusy(VS_FALSE);
            return 0;
        }
    }
    //--
    int w_tag = get_WaitResultIndex();
    StarCoreWaitResult *m_WaitResult = new_WaitResult(w_tag);

    NSMutableArray *cP = [[NSMutableArray alloc] init];

    VS_UUID ObjectID;
    l_Service ->GetID(Object, &ObjectID);
    l_Service->AddRefEx(Object);
    NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarObjectPrefix,[[NSUUID UUID] UUIDString]];
    [CleObjectMap setObject:fromPointer(Object) forKey:CleObjectID];

    [cP addObject:[NSString stringWithFormat:@"%@+%@",CleObjectID,fromString(l_Service->UuidToString(&ObjectID))]];
    [cP addObject:fromString(ScriptName)];
    [cP addObject:out];
    [cP addObject:[NSNumber numberWithInt:w_tag]];

    [m_WaitResult Lock];
    dispatch_async(dispatch_get_main_queue(), ^{
        /*
         - Parameters:
         - method: The name of the method to invoke.
         - arguments: The arguments. Must be a value supported by the codec of this
         channel.
         - result: A callback that will be invoked with the asynchronous result.
         The result will be a `FlutterError` instance, if the method call resulted
         in an error on the Flutter side. Will be `FlutterMethodNotImplemented`, if
         the method called was not implemented on the Flutter side. Any other value,
         including `nil`, should be interpreted as successful results.
         */
        [channel invokeMethod:@"starobjectclass_scriptproc" arguments:cP result:^(id  _Nullable result) {
            if( [result isKindOfClass:[FlutterError class]] ){
                /*--critical error--*/
                FlutterError *fe = result;
                NSLog(@"object script callback error %@",fe.message);
                [m_WaitResult SetResult:nil];
                return;
            }
            NSArray *plist = (NSArray *)result;
            id o = [plist objectAtIndex:2];
            int w_tag = toInt32([plist objectAtIndex:0]);
            StarCoreWaitResult *t_WaitResult = nil;
            if( w_tag != 0 ){
                t_WaitResult = get_WaitResult(w_tag);
                [t_WaitResult SetResult:o];
            }else{
                if( o != nil ){
                    NSLog(@"Object function [%@] can not return value, for it is called in ui thread",[plist objectAtIndex:1]);
                }
                //[t_WaitResult SetResult:nil];
            }
            return;
        }];
    });
    SRPControlInterface->SRPUnLock();
    id RetValue = [m_WaitResult WaitResult];
    [m_WaitResult UnLock];
    remove_WaitResult(w_tag);
    SRPControlInterface->SRPLock();
    l_Service -> SetRetCode(Object,VSRCALL_OK);
    if( RetValue == nil || [RetValue isKindOfClass:[NSNull class]] ){
        SetStarThreadWorkerBusy(VS_FALSE);
        return 0;
    }else{
        NSArray *RetValue_List = (NSArray *)RetValue;
        VS_CHAR *FrameTag = toString([RetValue_List objectAtIndex:0]);
        n = l_Service ->LuaGetTop();
        FlutterObjectToLua( l_Service, [RetValue_List objectAtIndex:1]);

        NSMutableArray *cP_l = [[NSMutableArray alloc] init];
        [cP_l addObject:fromString(FrameTag)];
        dispatch_async(dispatch_get_main_queue(), ^{
                /*
                 - Parameters:
                 - method: The name of the method to invoke.
                 - arguments: The arguments. Must be a value supported by the codec of this
                 channel.
                 - result: A callback that will be invoked with the asynchronous result.
                 The result will be a `FlutterError` instance, if the method call resulted
                 in an error on the Flutter side. Will be `FlutterMethodNotImplemented`, if
                 the method called was not implemented on the Flutter side. Any other value,
                 including `nil`, should be interpreted as successful results.
                 */
                [channel invokeMethod:@"starobjectclass_scriptproc_freeLlocalframe" arguments:cP_l result:nil];
                }
                );

        SetStarThreadWorkerBusy(VS_FALSE);
        return l_Service ->LuaGetTop()-n;
    }
}

/*-------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/

@implementation StarflutPluginCommon
+ (StarflutPluginCommon *)starflut_plugin_common_init:(FlutterMethodChannel *)channel_arg {
    channel = channel_arg;
    StarflutPluginCommon* instance = [[StarflutPluginCommon alloc] init];
    return instance;
}

-(id)init{
    if (self = [super init]) {
        ModuleInitFlag = false;

        ExitAppFlag = false;
        CleObjectMap = [NSMutableDictionary dictionary];
        starcoreCmdMap = [NSMutableDictionary dictionary];

        //starCoreThreadCallDeepSyncObject = [[NSRecursiveLock alloc] init];
        mutexObject = [[NSRecursiveLock alloc] init];

        g_WaitResultMap_mutexObject = [[NSRecursiveLock alloc] init];
        g_WaitResultMap_Index = 1;
        g_WaitResultMap = [NSMutableDictionary dictionary];

        memset(StarThreadWorker, 0, sizeof(StarThreadWorker));
        vs_mutex_init(&StarThreadWorkerSyncObject);

        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_getDocumentPath] forKey:@"starcore_getDocumentPath"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_isAndroid] forKey:@"starcore_isAndroid"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_getResourcePath] forKey:@"starcore_getResourcePath"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_getPlatform] forKey:@"starcore_getPlatform"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_loadLibrary] forKey:@"starcore_loadLibrary"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_setEnv] forKey:@"starcore_setEnv"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_getEnv] forKey:@"starcore_getEnv"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_getAssetsPath] forKey:@"starcore_getAssetsPath"];

        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_init] forKey:@"starcore_init"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_sRPDispatch] forKey:@"starcore_sRPDispatch"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_sRPLock] forKey:@"starcore_sRPLock"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_sRPUnLock] forKey:@"starcore_sRPUnLock"];

        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_rubyInitExt] forKey:@"starcore_rubyInitExt"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_poplocalframe] forKey:@"starcore_poplocalframe"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_InitCore] forKey:@"starcore_InitCore"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_InitSimpleEx] forKey:@"starcore_InitSimpleEx"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_InitSimple] forKey:@"starcore_InitSimple"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_GetSrvGroup] forKey:@"starcore_GetSrvGroup"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_moduleExit] forKey:@"starcore_moduleExit"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_moduleClear] forKey:@"starcore_moduleClear"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_regMsgCallBackP] forKey:@"starcore_regMsgCallBackP"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_setRegisterCode] forKey:@"starcore_setRegisterCode"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_isRegistered] forKey:@"starcore_isRegistered"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_setLocale] forKey:@"starcore_setLocale"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_getLocale] forKey:@"starcore_getLocale"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_version] forKey:@"starcore_version"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_getScriptIndex] forKey:@"starcore_getScriptIndex"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_setScript] forKey:@"starcore_setScript"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_detachCurrentThread] forKey:@"starcore_detachCurrentThread"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_coreHandle] forKey:@"starcore_coreHandle"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_captureScriptGIL] forKey:@"starcore_captureScriptGIL"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_releaseScriptGIL] forKey:@"starcore_releaseScriptGIL"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:starcore_setShareLibraryPath] forKey:@"starcore_setShareLibraryPath"];

        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_toString] forKey:@"StarSrvGroupClass_toString"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_createService] forKey:@"StarSrvGroupClass_createService"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_getService] forKey:@"StarSrvGroupClass_getService"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_clearService] forKey:@"StarSrvGroupClass_clearService"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_newParaPkg] forKey:@"StarSrvGroupClass_newParaPkg"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_newBinBuf] forKey:@"StarSrvGroupClass_newBinBuf"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_getServicePath] forKey:@"StarSrvGroupClass_getServicePath"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_setServicePath] forKey:@"StarSrvGroupClass_setServicePath"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_servicePathIsSet] forKey:@"StarSrvGroupClass_servicePathIsSet"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_getCurrentPath] forKey:@"StarSrvGroupClass_getCurrentPath"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_importService] forKey:@"StarSrvGroupClass_importService"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_clearServiceEx] forKey:@"StarSrvGroupClass_clearServiceEx"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_runScript] forKey:@"StarSrvGroupClass_runScript"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_runScriptEx] forKey:@"StarSrvGroupClass_runScriptEx"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_doFile] forKey:@"StarSrvGroupClass_doFile"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_doFileEx] forKey:@"StarSrvGroupClass_doFileEx"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_setClientPort] forKey:@"StarSrvGroupClass_setClientPort"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_setTelnetPort] forKey:@"StarSrvGroupClass_setTelnetPort"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_setOutputPort] forKey:@"StarSrvGroupClass_setOutputPort"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_setWebServerPort] forKey:@"StarSrvGroupClass_setWebServerPort"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_initRaw] forKey:@"StarSrvGroupClass_initRaw"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_loadRawModule] forKey:@"StarSrvGroupClass_loadRawModule"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_getLastError] forKey:@"StarSrvGroupClass_getLastError"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_getLastErrorInfo] forKey:@"StarSrvGroupClass_getLastErrorInfo"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_getCorePath] forKey:@"StarSrvGroupClass_getCorePath"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_getUserPath] forKey:@"StarSrvGroupClass_getUserPath"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_getLocalIP] forKey:@"StarSrvGroupClass_getLocalIP"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_getLocalIPEx] forKey:@"StarSrvGroupClass_getLocalIPEx"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_getObjectNum] forKey:@"StarSrvGroupClass_getObjectNum"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_activeScriptInterface] forKey:@"StarSrvGroupClass_activeScriptInterface"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarSrvGroupClass_preCompile] forKey:@"StarSrvGroupClass_preCompile"];

        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_toString] forKey:@"StarServiceClass_toString"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_get] forKey:@"StarServiceClass_get"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_getObject] forKey:@"StarServiceClass_getObject"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_getObjectEx] forKey:@"StarServiceClass_getObjectEx"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_newObject] forKey:@"StarServiceClass_newObject"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_runScript] forKey:@"StarServiceClass_runScript"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_runScriptEx] forKey:@"StarServiceClass_runScriptEx"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_doFile] forKey:@"StarServiceClass_doFile"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_doFileEx] forKey:@"StarServiceClass_doFileEx"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_isServiceRegistered] forKey:@"StarServiceClass_isServiceRegistered"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_checkPassword] forKey:@"StarServiceClass_checkPassword"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_newRawProxy] forKey:@"StarServiceClass_newRawProxy"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_importRawContext] forKey:@"StarServiceClass_importRawContext"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_getLastError] forKey:@"StarServiceClass_getLastError"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_getLastErrorInfo] forKey:@"StarServiceClass_getLastErrorInfo"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_allObject] forKey:@"StarServiceClass_allObject"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarServiceClass_restfulCall] forKey:@"StarServiceClass_restfulCall"];

        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_toString] forKey:@"StarParaPkgClass_toString"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_GetNumber] forKey:@"StarParaPkgClass_GetNumber"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_get] forKey:@"StarParaPkgClass_get"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_clear] forKey:@"StarParaPkgClass_clear"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_appendFrom] forKey:@"StarParaPkgClass_appendFrom"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_set] forKey:@"StarParaPkgClass_set"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_build] forKey:@"StarParaPkgClass_build"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_free] forKey:@"StarParaPkgClass_free"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_dispose] forKey:@"StarParaPkgClass_dispose"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_releaseOwner] forKey:@"StarParaPkgClass_releaseOwner"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_asDict] forKey:@"StarParaPkgClass_asDict"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_isDict] forKey:@"StarParaPkgClass_isDict"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_fromJSon] forKey:@"StarParaPkgClass_fromJSon"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_toJSon] forKey:@"StarParaPkgClass_toJSon"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_toTuple] forKey:@"StarParaPkgClass_toTuple"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_equals] forKey:@"StarParaPkgClass_equals"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarParaPkgClass_V] forKey:@"StarParaPkgClass_V"];


        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_toString] forKey:@"StarBinBufClass_toString"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_GetOffset] forKey:@"StarBinBufClass_GetOffset"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_init] forKey:@"StarBinBufClass_init"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_clear] forKey:@"StarBinBufClass_clear"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_saveToFile] forKey:@"StarBinBufClass_saveToFile"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_loadFromFile] forKey:@"StarBinBufClass_loadFromFile"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_read] forKey:@"StarBinBufClass_read"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_write] forKey:@"StarBinBufClass_write"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_free] forKey:@"StarBinBufClass_free"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_dispose] forKey:@"StarBinBufClass_dispose"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_releaseOwner] forKey:@"StarBinBufClass_releaseOwner"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_setOffset] forKey:@"StarBinBufClass_setOffset"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_print] forKey:@"StarBinBufClass_print"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarBinBufClass_asString] forKey:@"StarBinBufClass_asString"];

        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_toString] forKey:@"StarObjectClass_toString"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_get] forKey:@"StarObjectClass_get"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_set] forKey:@"StarObjectClass_set"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_call] forKey:@"StarObjectClass_call"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_newObject] forKey:@"StarObjectClass_newObject"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_free] forKey:@"StarObjectClass_free"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_dispose] forKey:@"StarObjectClass_dispose"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_hasRawContext] forKey:@"StarObjectClass_hasRawContext"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_rawToParaPkg] forKey:@"StarObjectClass_rawToParaPkg"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_getSourceScript] forKey:@"StarObjectClass_getSourceScript"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_getLastError] forKey:@"StarObjectClass_getLastError"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_getLastErrorInfo] forKey:@"StarObjectClass_getLastErrorInfo"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_releaseOwnerEx] forKey:@"StarObjectClass_releaseOwnerEx"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_regScriptProcP] forKey:@"StarObjectClass_regScriptProcP"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_instNumber] forKey:@"StarObjectClass_instNumber"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_changeParent] forKey:@"StarObjectClass_changeParent"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_jsonCall] forKey:@"StarObjectClass_jsonCall"];

        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_active] forKey:@"StarObjectClass_active"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_deActive] forKey:@"StarObjectClass_deActive"];
        [starcoreCmdMap setObject:[NSNumber numberWithInt:StarObjectClass_isActive] forKey:@"StarObjectClass_isActive"];

    }
    return self;
}

-(NSString*) getTagFromObjectMap:(void *)v
{
    NSArray *keys = [CleObjectMap allKeys];
    NSUInteger length = [keys count];
    for (NSUInteger i = 0; i < length; i ++) {
        id key = [keys objectAtIndex:i];
        id obj = [CleObjectMap objectForKey:key];
        if( toPointer(obj) == v ){
            return (NSString*)key;
        }
    }
    return nil;
}

-(void) removeFromObjectMap:(void *)v
{
    NSMutableArray *kk = [[NSMutableArray alloc] init];
    NSArray *keys = [CleObjectMap allKeys];
    NSUInteger length = [keys count];
    for (NSUInteger i = 0; i < length; i ++) {
        id key = [keys objectAtIndex:i];
        id obj = [CleObjectMap objectForKey:key];
        if( v == NULL || toPointer(obj) == v ){
            [kk addObject:key];
            if( StartsWithString(key, StarServicePrefix) ){
                class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer(obj);
                l_Service ->Release();
            }else if( StartsWithString(key, StarObjectPrefix) ){
                void *l_Object = toPointer(obj);
                SRPInterface->UnLockGC(l_Object);
            }else if( StartsWithString(key, StarParaPkgPrefix) ){
                class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer(obj);
                l_ParaPkg ->Release();
            }else if( StartsWithString(key, StarBinBufPrefix) ){
                class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer(obj);
                l_BinBuf ->Release();
            }else if( StartsWithString(key, StarSrvGroupPrefix) ){
                class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer(obj);
                if( l_SrvGroup != BasicSRPInterface )
                    l_SrvGroup ->Release();
            }
        }
    }
    for(NSUInteger i=0; i < [kk count]; i ++ ){
        [CleObjectMap removeObjectForKey:[kk objectAtIndex:i]];
    }
    return;
}

-(void) removeFromObjectMapByKey:(NSArray *)kk
{
    for (NSUInteger i = 0; i < [kk count]; i ++) {
        id key = [kk objectAtIndex:i];
        id obj = [CleObjectMap objectForKey:key];
        {
            if( StartsWithString(key, StarServicePrefix) ){
                class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer(obj);
                l_Service ->Release();
            }else if( StartsWithString(key, StarObjectPrefix) ){
                void *l_Object = toPointer(obj);
                SRPInterface->UnLockGC(l_Object);
            }else if( StartsWithString(key, StarParaPkgPrefix) ){
                class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer(obj);
                l_ParaPkg ->Release();
            }else if( StartsWithString(key, StarBinBufPrefix) ){
                class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer(obj);
                l_BinBuf ->Release();
            }else if( StartsWithString(key, StarSrvGroupPrefix) ){
                class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer(obj);
                if( l_SrvGroup != BasicSRPInterface )
                    l_SrvGroup ->Release();
            }
        }
    }
    for(NSUInteger i=0; i < [kk count]; i ++ ){
        [CleObjectMap removeObjectForKey:[kk objectAtIndex:i]];
    }
    return;
}

- (void)handleMethodCall_Common:(FlutterMethodCall*)call result:(FlutterResult)result {
    NSNumber *index = [starcoreCmdMap objectForKey:call.method];
    //if ([@"getPlatformVersion" isEqualToString:call.method]) {
    switch([index intValue]){
        case starcore_getDocumentPath :
        case starcore_isAndroid :
        case starcore_getResourcePath :
        case starcore_getPlatform :
        case starcore_loadLibrary :
        case starcore_setEnv :
        case starcore_getEnv :
        case starcore_getAssetsPath :

        case starcore_init :
        case starcore_sRPDispatch :
        case starcore_sRPLock :
        case starcore_sRPUnLock :
            [self handleMethodCall_Do_Direct:call result:result];
            break;
        default:
            /* in some case, for thread switching, starCoreThreadCallDeep may be not 0, so remove it */
            /*
            [starCoreThreadCallDeepSyncObject lock];
            if( starCoreThreadCallDeep != 0 ){
                NSLog(@"call starflut function [%@] failed, current is in starcore thread process",call.method);
            }
            [starCoreThreadCallDeepSyncObject unlock];
            */
          if ([index intValue] == starcore_moduleExit || [index intValue] == starcore_moduleClear) {
              StarflutMessage *message = [StarflutMessage initStarflutMessage:starcore_ThreadTick_MethodCall call:call result:result];
              sendStarMessage(StarThreadWorker[0],message);
          }
          else {
              struct StructOfStarThreadWorker* ThreadWorker = GetStarThreadWorker();
              if (ThreadWorker != NULL) {
                  StarflutMessage *message = [StarflutMessage initStarflutMessage:starcore_ThreadTick_MethodCall call:call result:result];
                  sendStarMessage(ThreadWorker,message);
              }
              else if (CanCreateStarThreadWorker() == VS_TRUE) {
                  /*--create a new worker*/
                  //VS_COND waitThreadCond;
                  vs_cond_init(&waitThreadCond);

                  vs_mutex_lock(&waitThreadCond.mutex);

                   /*--create worker thread--*/
                   dispatch_async(dispatch_get_global_queue(0, 0), ^{
                       VS_THREADID hThreadIDTemp = vs_thread_currentid();
                       CreateStarThreadWorker(hThreadIDTemp);
                       struct StructOfStarThreadWorker* ThreadWorker = GetStarThreadWorkerCurrent();

                       vs_mutex_lock(&waitThreadCond.mutex);
                       vs_cond_signal(&waitThreadCond);
                       vs_mutex_unlock(&waitThreadCond.mutex);

                        while(true) {
                            StarflutMessage *message = getStarMessage(ThreadWorker);
                            switch( message->messageID){
                            case starcore_ThreadTick_MethodCall :
                            {
                                /*SetStarThreadWorkerBusy(VS_TRUE);  need not, the caller will queue*/
                                NSNumber *index = [starcoreCmdMap objectForKey:call.method];
                                SRPControlInterface->SRPLock();
                                id value = [self handleMethodCall_Do:message->call];
                                if( [index intValue] != starcore_moduleExit)
                                    SRPControlInterface->SRPUnLock();
                                dispatch_async(dispatch_get_main_queue(), ^{
                                    message->result(value);  /*--run in ui thread--*/
                                });
                                /*SetStarThreadWorkerBusy(VS_FALSE);*/
                            }
                            break;
                            case starcore_ThreadTick_Exit :
                            {
                                SRPControlInterface->DetachCurrentThread();
                                vs_mutex_lock(&ThreadWorker->ThreadExitCond.mutex);
                                vs_cond_signal(&ThreadWorker->ThreadExitCond);
                                vs_mutex_unlock(&ThreadWorker->ThreadExitCond.mutex);
                                return;
                            }
                        }
                    }
                  });

                  vs_cond_wait_local(&waitThreadCond);
                  vs_mutex_unlock(&waitThreadCond.mutex);
                  vs_cond_destroy(&waitThreadCond);

                  struct StructOfStarThreadWorker* ThreadWorker_Local = GetStarThreadWorker();
                  StarflutMessage *message = [StarflutMessage initStarflutMessage:starcore_ThreadTick_MethodCall call:call result:result];
                  sendStarMessage(ThreadWorker_Local, message);
              }
              else {
                  NSLog(@"Can not create worker thread");
                  result(nil);
              }
           }
            break;
    }
}

static VS_HANDLE hDllInstance;
static VSCore_RegisterCallBackInfoProc RegisterCallBackInfoProc;
static VSCore_UnRegisterCallBackInfoProc UnRegisterCallBackInfoProc;
static VSCore_InitProc VSInitProc;
static VSCore_TermExProc VSTermExProc;
static VSCore_QueryControlInterfaceProc QueryControlInterfaceProc;
static VSCore_TermProc  VSTermProc;

- (void)handleMethodCall_Do_Direct:(FlutterMethodCall*)call result:(FlutterResult)result {
    NSNumber *index = [starcoreCmdMap objectForKey:call.method];
    //if ([@"getPlatformVersion" isEqualToString:call.method]) {
    switch([index intValue]){
        case starcore_getDocumentPath :
        {
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            NSString *documentsDirectory = [paths objectAtIndex:0];
            result(documentsDirectory);
        }
            break;
        case starcore_getResourcePath :
        {
            NSString *respaths = [[NSBundle mainBundle] resourcePath];
            result(respaths);
            break;
        }
        case starcore_getAssetsPath :
        {
            NSString *respaths = [[NSBundle mainBundle] resourcePath];
            const VS_CHAR *res_cpath = [respaths UTF8String];
            
            char LocalBuf[2048];
            sprintf(LocalBuf,"%s/../Frameworks/App.framework/Versions/A/Resources/flutter_assets",res_cpath);
            if( vs_file_exist(LocalBuf) == false )
                sprintf(LocalBuf,"%s/../Frameworks/App.framework",res_cpath);
            else
                sprintf(LocalBuf,"%s/../Frameworks/App.framework/Versions/A/Resources",res_cpath);
            result(fromString(LocalBuf));
            break;
        }
        case starcore_isAndroid :
        {
            result([NSNumber numberWithBool:false]);
        }
            break;
            case starcore_getPlatform :
            {
            result([NSNumber numberWithInteger:4]);
            }
            break;
         case starcore_loadLibrary :
          {
               NSArray *plist = (NSArray *)call.arguments;

               VS_HANDLE Handle = dlopen(toString([plist objectAtIndex:0]),0);
               if( Handle == 0 ){
                   VS_CHAR *LocalErrorInfo = (VS_CHAR *)dlerror();
                   if( LocalErrorInfo != NULL )
                       NSLog(@"%s",LocalErrorInfo);
                   result([NSNumber numberWithBool:false]);
               }else
                   result([NSNumber numberWithBool:true]);
          }
          break;
         case starcore_setEnv :
          {
              NSArray *plist = (NSArray *)call.arguments;
              VS_BOOL Result = vs_set_env(toString([plist objectAtIndex:0]),toString([plist objectAtIndex:1]));
              result([NSNumber numberWithBool:Result]);
          }
          break;
         case starcore_getEnv :
          {
              VS_CHAR LocalBuf[4096];
              LocalBuf[0] = 0;
              VS_BOOL Result = vs_get_env(toString(call.arguments),LocalBuf,4096);
              if( Result == VS_FALSE )
                  result(@"");
              else
                  result(fromString(LocalBuf));
          }
          break;
        case starcore_getPesudoExport :
            break;
        case starcore_init :
        {
            dispatch_async(dispatch_get_global_queue(0, 0), ^{
                //NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask, YES);
                //NSString *documentsDirectory = [paths objectAtIndex:0];
                //const char* destDir = [documentsDirectory UTF8String];

                NSString *respaths = [[NSBundle bundleForClass:[StarflutPluginCommon class]] resourcePath];
//[[NSBundle mainBundle] resourcePath];
                const VS_CHAR *res_cpath = [respaths UTF8String];
                                
#if 0 /*for test*/
                char ModuleName1[512];
                //sprintf(ModuleName1, "%s/libstar_python39.so",res_cpath);
                sprintf(ModuleName1, "%s/libpython3.9.dylib",res_cpath);
                VS_CHAR *ErrorInfo;
                VS_HANDLE Handle = dlopen(ModuleName1,0);
                VS_CHAR *LocalErrorInfo = (VS_CHAR *)dlerror();
                
                sprintf(ModuleName1, "%s/libstar_python39.so",res_cpath);
                Handle = dlopen(ModuleName1,0);
                LocalErrorInfo = (VS_CHAR *)dlerror();
#endif
                
                char ModuleName[512];
                sprintf(ModuleName, "%s/libstarcore.dylib",res_cpath);
                hDllInstance = vs_dll_open(ModuleName);
                if (hDllInstance == NULL) {
                #if defined(ENV_M64)
                    printf("load library [%s] 64bit version for %s error....\n", ModuleName, VS_OSALIAS);
                #else
                    printf("load library [%s] 32bit version for %s error....\n", ModuleName, VS_OSALIAS);
                #endif
                    dispatch_async(dispatch_get_main_queue(), ^{
                        result(@"");
                    });
                    return;
                }

                RegisterCallBackInfoProc = (VSCore_RegisterCallBackInfoProc)vs_dll_sym(hDllInstance, VSCORE_REGISTERCALLBACKINFO_NAME);
                UnRegisterCallBackInfoProc = (VSCore_UnRegisterCallBackInfoProc)vs_dll_sym(hDllInstance, VSCORE_UNREGISTERCALLBACKINFO_NAME);
                VSInitProc = (VSCore_InitProc)vs_dll_sym(hDllInstance, VSCORE_INIT_NAME);
                VSTermExProc = (VSCore_TermExProc)vs_dll_sym(hDllInstance, VSCORE_TERMEX_NAME);
                QueryControlInterfaceProc = (VSCore_QueryControlInterfaceProc)vs_dll_sym(hDllInstance, VSCORE_QUERYCONTROLINTERFACE_NAME);
                VSTermProc = (VSCore_TermProc)vs_dll_sym(hDllInstance, VSCORE_TERM_NAME);

                 VS_BOOL Result = VSInitProc(true, true, "", 0, "", 0, NULL);
                if( Result == VSINIT_ERROR ){
                    dispatch_async(dispatch_get_main_queue(), ^{
                        result(@"");
                    });
                    return;
                }

                SRPControlInterface = QueryControlInterfaceProc();
                {
                    NSString *respaths_library = [[NSBundle mainBundle] resourcePath];
                    const VS_CHAR *res_cpath_library = [respaths_library UTF8String];
                    SRPControlInterface->SetShareLibraryPath(res_cpath_library);  /*set library search path*/
                }                                

                SRPControlInterface->SetLocale("utf-8");
                SRPControlInterface->SRPUnLock();

                /*--return result--*/
                NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarCorePrefix,[[NSUUID UUID] UUIDString]];

                dispatch_async(dispatch_get_main_queue(), ^{
                    result(CleObjectID);
                });

                VS_THREADID hThreadIDTemp = vs_thread_currentid();
                CreateStarThreadWorker(hThreadIDTemp);
                struct StructOfStarThreadWorker* ThreadWorker = GetStarThreadWorkerCurrent();

                /*--create timer thread--*/
                dispatch_async(dispatch_get_global_queue(0, 0), ^{
                    while(true){
                        [mutexObject lock];
                        if (ExitAppFlag == true){
                            [mutexObject unlock];
                            for (VS_INT32 i = 0; i < MAX_STARTHREAD_NUMBER; i++) {
                                if (StarThreadWorker[i] != NULL) {
                                    StarflutMessage *message = [StarflutMessage initStarflutMessage:starcore_ThreadTick_Exit];
                                    sendStarMessage(StarThreadWorker[i], message);
                                }
                            }
                            break;
                         }
                         [mutexObject unlock];
                        sleep(10);
                        StarflutMessage *message = [StarflutMessage initStarflutMessage:starcore_ThreadTick_MessageID];
                        sendStarMessage(StarThreadWorker[0],message);
                    }
                });

                while(true) {
                    StarflutMessage *message = getStarMessage(ThreadWorker);
                    switch( message->messageID){
                        case starcore_ThreadTick_MessageID :
                        {
                            if( SRPControlInterface == NULL || ModuleInitFlag == false )
                                break;
                            SRPControlInterface->SRPLock();
                            while( SRPControlInterface->SRPDispatch(VS_FALSE) == VS_TRUE );
                            SRPControlInterface->SRPUnLock();
                        }
                            break;
                        case starcore_ThreadTick_MethodCall :
                        {
                            /*SetStarThreadWorkerBusy(VS_TRUE);  need not, the caller will queue*/
                            NSNumber *index = [starcoreCmdMap objectForKey:call.method];
                            SRPControlInterface->SRPLock();
                            id value = [self handleMethodCall_Do:message->call];
                            if( [index intValue] != starcore_moduleExit)
                                SRPControlInterface->SRPUnLock();
                            dispatch_async(dispatch_get_main_queue(), ^{
                                message->result(value);  /*--run in ui thread--*/
                            });
                            /*SetStarThreadWorkerBusy(VS_FALSE);*/
                        }
                            break;
                        case starcore_ThreadTick_Exit :
                        {
                            vs_mutex_lock(&ThreadWorker->ThreadExitCond.mutex);
                            vs_cond_signal(&ThreadWorker->ThreadExitCond);
                            vs_mutex_unlock(&ThreadWorker->ThreadExitCond.mutex);
                            return;
                        }
                    }
                }
            });

        }
            break;
        case starcore_sRPDispatch :
        {
            SRPControlInterface -> SRPLock();
            VS_BOOL Result = SRPControlInterface -> SRPDispatch(toBool(call.arguments));
            SRPControlInterface -> SRPUnLock();
            result(fromBool(Result));
        }
            break;
        case starcore_sRPLock :
        {
            SRPControlInterface -> SRPLock();
            result(nil);
        }
            break;
        case starcore_sRPUnLock :
        {
            SRPControlInterface -> SRPUnLock();
            result(nil);
        }
            break;
        default :
        {
            result(FlutterMethodNotImplemented);
        }
            break;
    }
}

- (id)handleMethodCall_Do:(FlutterMethodCall*)call {
    NSNumber *index = [starcoreCmdMap objectForKey:call.method];
    //if ([@"getPlatformVersion" isEqualToString:call.method]) {
    switch([index intValue]){
        case starcore_rubyInitExt :
        {
#if defined(ENV_WITHRUBY)
            ruby_init_ext("openssl.so",(void *)Init_openssl);
            ruby_init_ext("digest/md5.so",(void *)Init_md5);
            ruby_init_ext("digest/rmd160.so",(void *)Init_rmd160);
            ruby_init_ext("digest/sha1.so",(void *)Init_sha1);
            ruby_init_ext("digest/sha2.so",(void *)Init_sha2);
#endif
            return nil;
        }
        case starcore_poplocalframe :
        {
            NSArray *plist = (NSArray *)call.arguments;
            [self removeFromObjectMapByKey:plist];
            /*
            for(NSInteger i=0; i < [plist count]; i++ ){
                if( [CleObjectMap objectForKey:(NSString *)[plist objectAtIndex:i]] )
                    [CleObjectMap removeObjectForKey:(NSString *)[plist objectAtIndex:i]];
            }
             */
            return nil;
        }
        case starcore_InitCore:
        {
            NSArray *plist = (NSArray *)call.arguments;
            SRPControlInterface -> SRPLock();
            /*VS_BOOL ShowMenuFlag,VS_BOOL ShowOutWndFlag*/
            VS_INT32 Result = VSInitProc( toBool([plist objectAtIndex:0]), toBool([plist objectAtIndex:3]),toString([plist objectAtIndex:4]),toInt32([plist objectAtIndex:5]),toString([plist objectAtIndex:6]),toInt32([plist objectAtIndex:7]), NULL);
            if( Result >= 0 ){
                ModuleInitFlag = true;
                /*--register dispatch request--*/
                SRPControlInterface->RegDispatchRequest(Starflut_SRPDispatchRequestCallBack, 0);
            }
            SRPControlInterface -> SRPUnLock();
            return [NSNumber numberWithInt:Result];
        }
        case starcore_InitSimpleEx :
        {
            NSArray *plist = (NSArray *)call.arguments;
            SRPControlInterface -> SRPLock();
            VS_INT32 Result = VSInitProc(VS_TRUE, VS_TRUE, (VS_CHAR *)"", 0, (VS_CHAR *)"", toInt32([plist objectAtIndex:0]),NULL);
            if( Result < 0 ){
                SRPControlInterface -> SRPUnLock();
                return nil;
            }
            BasicSRPInterface = SRPControlInterface ->QueryBasicInterface(0);
            if( [plist count] > 2 ){
                for( NSInteger i=2; i < [plist count]; i++ ){
                    VS_CHAR *DependServiceName = toString([plist objectAtIndex:i]);
                    if( vs_string_strlen(DependServiceName) != 0 && BasicSRPInterface -> ImportService( DependServiceName, VS_TRUE ) == VS_FALSE ){
                        SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__,"import depend service [%s] fail",DependServiceName);
                        BasicSRPInterface ->Release();
                        BasicSRPInterface = NULL;
                        SRPControlInterface -> SRPUnLock();
                        return nil;
                    }
                }
            }
            VS_INT32 WebPortNumber = toInt32([plist objectAtIndex:1]);
            if( WebPortNumber != 0 )
                BasicSRPInterface -> SetWebServerPort("",WebPortNumber,100,2048);

            ModuleInitFlag = true;
            /*--register dispatch request--*/
            SRPControlInterface->RegDispatchRequest(Starflut_SRPDispatchRequestCallBack, 0);
            SRPControlInterface -> SRPUnLock();

            NSString *CleObjectID = [self getTagFromObjectMap:BasicSRPInterface];
            if( CleObjectID == nil ){
                CleObjectID = [NSString stringWithFormat:@"%@%@",StarSrvGroupPrefix,[[NSUUID UUID] UUIDString]];
                [CleObjectMap setObject:fromPointer(BasicSRPInterface) forKey:CleObjectID];
            }
            return CleObjectID;

        }
        case starcore_InitSimple :
        {
            NSArray *plist = (NSArray *)call.arguments;
            SRPControlInterface -> SRPLock();
            VS_INT32 Result = VSInitProc(VS_TRUE, VS_TRUE, (VS_CHAR *)"", 0, (VS_CHAR *)"", toInt32([plist objectAtIndex:2]),NULL);
            if( Result < 0 ){
                SRPControlInterface -> SRPUnLock();
                return nil;
            }
            VS_CHAR *ServiceName = toString([plist objectAtIndex:0]);
            VS_CHAR *ServicePass = toString([plist objectAtIndex:1]);
            BasicSRPInterface = SRPControlInterface ->QueryBasicInterface(0);
            if( [plist count] > 4 ){
                for( NSInteger i=4; i < [plist count]; i++ ){
                    VS_CHAR *DependServiceName = toString([plist objectAtIndex:i]);
                    if( vs_string_strlen(DependServiceName) != 0 && BasicSRPInterface -> ImportService( DependServiceName, VS_TRUE ) == VS_FALSE ){
                        SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__,"import depend service [%s] fail",DependServiceName);
                        BasicSRPInterface ->Release();
                        BasicSRPInterface = NULL;
                        SRPControlInterface -> SRPUnLock();
                        return nil;
                    }
                }
            }
            if( BasicSRPInterface -> CreateService( "", ServiceName, NULL, ServicePass,5, 10240,10240,10240,10240,10240 ) == VS_FALSE ){
                SRPControlInterface->ProcessError(VSFAULT_WARNING, __FILE__, __LINE__,"create service [%s] fail",toString([plist objectAtIndex:0]));
                BasicSRPInterface ->Release();
                BasicSRPInterface = NULL;
                SRPControlInterface -> SRPUnLock();
                return nil;
            }else{
                class ClassOfSRPInterface* l_SRPInterface;

                l_SRPInterface = BasicSRPInterface -> GetSRPInterface( ServiceName, "root", ServicePass );
                VS_INT32 WebPortNumber = toInt32([plist objectAtIndex:3]);
                if( WebPortNumber != 0 )
                    BasicSRPInterface -> SetWebServerPort("",WebPortNumber,100,2048);

                ModuleInitFlag = true;
                /*--register dispatch request--*/
                SRPControlInterface->RegDispatchRequest(Starflut_SRPDispatchRequestCallBack, 0);
                SRPControlInterface -> SRPUnLock();

                SRPInterface = l_SRPInterface;
                /*Star_ObjectCBridge_Init(SRPInterface,NULL,NULL);  for macos, need not call this*/

                VS_UUID ObjectID;
                l_SRPInterface->GetServiceID(&ObjectID);
                NSString *newServiceID = fromString(l_SRPInterface->UuidToString(&ObjectID));

                NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarServicePrefix,[[NSUUID UUID] UUIDString]];
                [CleObjectMap setObject:fromPointer(l_SRPInterface) forKey:CleObjectID];
                return [NSString stringWithFormat:@"%@+%@",CleObjectID,newServiceID];
            }
        }
        case starcore_GetSrvGroup :
        {
            NSArray *plist = (NSArray *)call.arguments;
            if ( [plist count] == 0){
                if( SRPControlInterface == NULL || BasicSRPInterface == NULL )
                    return nil;
                NSString *CleObjectID = [self getTagFromObjectMap:BasicSRPInterface];
                if( CleObjectID == nil ){
                    CleObjectID = [NSString stringWithFormat:@"%@%@",StarSrvGroupPrefix,[[NSUUID UUID] UUIDString]];
                    [CleObjectMap setObject:fromPointer(BasicSRPInterface) forKey:CleObjectID];
                }
                return CleObjectID;
            }else{
                if( SRPControlInterface == NULL )
                    return nil;
                id ServiceName_GroupID = [plist objectAtIndex:0];
                if( [ServiceName_GroupID isKindOfClass:[NSNumber class]] ){
                    int ServiceGroupID = toInt32(ServiceName_GroupID);
                    if( ServiceGroupID == 0 ){
                        if( BasicSRPInterface == NULL )
                            return nil;
                        NSString *CleObjectID = [self getTagFromObjectMap:BasicSRPInterface];
                        if( CleObjectID == nil ){
                            CleObjectID = [NSString stringWithFormat:@"%@%@",StarSrvGroupPrefix,[[NSUUID UUID] UUIDString]];
                            [CleObjectMap setObject:fromPointer(BasicSRPInterface) forKey:CleObjectID];
                        }
                        return CleObjectID;
                    }else{
                        SRPControlInterface -> SRPLock();
                        class ClassOfBasicSRPInterface *l_BasicSRPInterface = SRPControlInterface->QueryBasicInterface(ServiceGroupID);
                        if( l_BasicSRPInterface == NULL ){
                            SRPControlInterface -> SRPUnLock();
                            return nil;
                        }
                        if( l_BasicSRPInterface ->GetServiceGroupID() == 0 ){
                            if( BasicSRPInterface == NULL ){
                                BasicSRPInterface = l_BasicSRPInterface;
                            }else{
                                l_BasicSRPInterface ->Release();
                            }
                            SRPControlInterface -> SRPUnLock();
                            NSString *CleObjectID = [self getTagFromObjectMap:BasicSRPInterface];
                            if( CleObjectID == nil ){
                                CleObjectID = [NSString stringWithFormat:@"%@%@",StarSrvGroupPrefix,[[NSUUID UUID] UUIDString]];
                                [CleObjectMap setObject:fromPointer(BasicSRPInterface) forKey:CleObjectID];
                            }
                            return CleObjectID;
                        }else{
                            SRPControlInterface -> SRPUnLock();
                            NSString *CleObjectID = [self getTagFromObjectMap:l_BasicSRPInterface];
                            if( CleObjectID == nil ){
                                CleObjectID = [NSString stringWithFormat:@"%@%@",StarSrvGroupPrefix,[[NSUUID UUID] UUIDString]];
                                [CleObjectMap setObject:fromPointer(l_BasicSRPInterface) forKey:CleObjectID];
                            }
                            return CleObjectID;
                        }
                    }
                }else if( [ServiceName_GroupID isKindOfClass:[NSString class]] ){
                    VS_CHAR *ServiceName,*ActiveServiceName;
                    VS_UUID ServiceID;
                    VS_ULONG ServiceGroupID;

                    SRPControlInterface -> SRPLock();
                    ServiceName = toString(ServiceName_GroupID);
                    ServiceGroupID = SRPControlInterface->QueryFirstServiceGroup();
                    class ClassOfBasicSRPInterface *l_BasicSRPInterface = NULL;
                    while( ServiceGroupID != VS_INVALID_SERVICEGROUPID ){
                        l_BasicSRPInterface = SRPControlInterface->QueryBasicInterface(ServiceGroupID);
                        if( l_BasicSRPInterface == NULL ){
                            SRPControlInterface -> SRPUnLock();
                            return nil;
                        }
                        ActiveServiceName = l_BasicSRPInterface -> QueryActiveService(&ServiceID);
                        if( ActiveServiceName != NULL && vs_string_strcmp( ActiveServiceName, ServiceName ) == 0 )
                            break;
                        l_BasicSRPInterface -> Release();
                        ServiceGroupID = SRPControlInterface->QueryNextServiceGroup();
                    }
                    if( ServiceGroupID == VS_INVALID_SERVICEGROUPID ){
                        if( l_BasicSRPInterface != NULL )
                            l_BasicSRPInterface -> Release();
                        SRPControlInterface -> SRPUnLock();
                        return nil;
                    }
                    if( l_BasicSRPInterface ->GetServiceGroupID() == 0 ){
                        if( BasicSRPInterface == NULL ){
                            BasicSRPInterface = l_BasicSRPInterface;
                        }else{
                            l_BasicSRPInterface ->Release();
                        }
                        SRPControlInterface -> SRPUnLock();
                        NSString *CleObjectID = [self getTagFromObjectMap:BasicSRPInterface];
                        if( CleObjectID == nil ){
                            CleObjectID = [NSString stringWithFormat:@"%@%@",StarSrvGroupPrefix,[[NSUUID UUID] UUIDString]];
                            [CleObjectMap setObject:fromPointer(BasicSRPInterface) forKey:CleObjectID];
                        }
                        return CleObjectID;
                    }else{
                        SRPControlInterface -> SRPUnLock();
                        NSString *CleObjectID = [self getTagFromObjectMap:l_BasicSRPInterface];
                        if( CleObjectID == nil ){
                            CleObjectID = [NSString stringWithFormat:@"%@%@",StarSrvGroupPrefix,[[NSUUID UUID] UUIDString]];
                            [CleObjectMap setObject:fromPointer(l_BasicSRPInterface) forKey:CleObjectID];
                        }
                        return CleObjectID;
                    }
                }else{
                    return nil;
                }
            }
        }
        case starcore_moduleExit :
        {
            if( ModuleInitFlag == true ){
                for (VS_INT32 i = 1; i < MAX_STARTHREAD_NUMBER; i++) {
                    if (StarThreadWorker[i] != NULL) {
                        vs_mutex_lock(&StarThreadWorker[i]->ThreadExitCond.mutex);
                    }
                }

                [mutexObject lock];
                ExitAppFlag = true;
                [mutexObject unlock];

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

                [self removeFromObjectMap:NULL];

                BasicSRPInterface ->ClearService();
                BasicSRPInterface ->Release();
                BasicSRPInterface = NULL;

                SRPControlInterface->UnRegDispatchRequest(Starflut_SRPDispatchRequestCallBack, 0);

                UnRegisterCallBackInfoProc(GlobalMsgCallBack, 0);
                VSTermProc();
                ModuleInitFlag = false;
            }

            if (hDllInstance != NULL) {
                vs_dll_close(hDllInstance);
                hDllInstance = NULL;
            }
            return nil;
        }
        case starcore_moduleClear :
        {
            [self removeFromObjectMap:NULL];
            BasicSRPInterface ->ClearService();

            SRPControlInterface->UnRegDispatchRequest(Starflut_SRPDispatchRequestCallBack, 0);

            if( SRPControlInterface != NULL )
                while( SRPControlInterface -> SRPDispatch(VS_FALSE) == VS_TRUE );
            return nil;
        }
        case starcore_regMsgCallBackP :
        {
            if( toBool(call.arguments) == false ){
                UnRegisterCallBackInfoProc(GlobalMsgCallBack,0);
            }else{
                RegisterCallBackInfoProc(GlobalMsgCallBack,0);
            }
            return nil;
        }
        case starcore_setRegisterCode :
        {
            NSArray *plist = (NSArray *)call.arguments;
            VS_BOOL Result = SRPControlInterface ->SetRegisterCode(toString([plist objectAtIndex:0]),toBool([plist objectAtIndex:1]));
            return fromBool(Result);
        }
        case starcore_isRegistered :
        {
            VS_BOOL Result = SRPControlInterface ->IsRegistered();
            return fromBool(Result);
        }
        case starcore_setLocale :
        {
            SRPControlInterface->SetLocale(toString(call.arguments));
            return fromBool(VS_TRUE);
        }
        case starcore_getLocale :
        {
            VS_CHAR *Lang = SRPControlInterface->GetLocale();
            return fromString(Lang);
        }
        case starcore_version :
        {
            return [NSArray arrayWithObjects:[NSNumber numberWithInt:VS_MAINVERSION], [NSNumber numberWithInt:VS_SUBVERSION], [NSNumber numberWithInt:VS_BUILDVERSION], nil];
        }
        case starcore_getScriptIndex :
        {
            VS_INT32 Index = SRPControlInterface->GetScriptInterfaceIndex(toString(call.arguments));
            return fromInt32(Index);
        }
        case starcore_setScript :
        {
            NSArray *plist = (NSArray *)call.arguments;
            VS_BOOL Result = SRPControlInterface->SetScriptInterface(toString([plist objectAtIndex:0]),toString([plist objectAtIndex:1]),toString([plist objectAtIndex:2]));
            return fromBool(Result);
        }
        case starcore_detachCurrentThread :
        {
            SRPControlInterface->DetachCurrentThread();
            return nil;
        }
        case starcore_coreHandle :
        {
            return [NSNumber numberWithLongLong:0xFFFFFFFFFFFFFFFF];
        }
        case starcore_captureScriptGIL :
        {
            SRPControlInterface->CaptureScriptGIL(NULL, NULL);
            return nil;
        }
        case starcore_releaseScriptGIL :
        {
            SRPControlInterface->ReleaseScriptGIL(NULL, NULL);
            return nil;
        }
        case starcore_setShareLibraryPath :
        {
            SRPControlInterface->SetShareLibraryPath(toString(call.arguments));
            return nil;
        }
        /*-----------------------------------------------*/
        case StarSrvGroupClass_toString :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            if( l_SrvGroup ->QueryActiveService(NULL) == NULL ){
                return @"service not load";
            }else{
                return fromString(l_SrvGroup ->QueryActiveService(NULL));
            }
        }
        case StarSrvGroupClass_createService:
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            VS_UUID ObjectID;
            VS_CHAR *ServiceID = toString([plist objectAtIndex:10]);
            if( ServiceID != NULL )
                l_SrvGroup -> StringToUuid( ServiceID, &ObjectID );
            else{
                INIT_UUID( ObjectID );
            }
            VS_BOOL Result = l_SrvGroup -> CreateService(toString([plist objectAtIndex:1]),toString([plist objectAtIndex:2]),&ObjectID,toString([plist objectAtIndex:3]),toInt32([plist objectAtIndex:4]),toInt32([plist objectAtIndex:5]),toInt32([plist objectAtIndex:6]),toInt32([plist objectAtIndex:7]),toInt32([plist objectAtIndex:8]),toInt32([plist objectAtIndex:9]));
            if( Result == VS_FALSE ){
                return nil;
            }else{
                class ClassOfSRPInterface *l_SRPInterface = l_SrvGroup -> GetSRPInterface( toString([plist objectAtIndex:2]), "root", toString([plist objectAtIndex:3]) );
                SRPInterface = l_SRPInterface;

                /*Star_ObjectCBridge_Init(SRPInterface,NULL,NULL);  for macos, need not call this*/

                l_SRPInterface->GetServiceID(&ObjectID);
                NSString *newServiceID = fromString(l_SRPInterface->UuidToString(&ObjectID));

                NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarServicePrefix,[[NSUUID UUID] UUIDString]];
                [CleObjectMap setObject:fromPointer(l_SRPInterface) forKey:CleObjectID];
                return [NSString stringWithFormat:@"%@+%@",CleObjectID,newServiceID];
            }
        }
        case StarSrvGroupClass_getService :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            VS_UUID ServiceID;
            if( l_SrvGroup -> QueryActiveService( &ServiceID ) == NULL ){
                return nil;
            }
            class ClassOfSRPInterface *l_SRPInterface = l_SrvGroup -> GetSRPInterfaceEx( &ServiceID, toString([plist objectAtIndex:1]),toString([plist objectAtIndex:2]));
            if( l_SRPInterface == NULL )
                return nil;

            SRPInterface = l_SRPInterface;  /*--save it--*/

            NSString *newServiceID = fromString(l_SRPInterface->UuidToString(&ServiceID));

            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarServicePrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(l_SRPInterface) forKey:CleObjectID];
            return [NSString stringWithFormat:@"%@+%@",CleObjectID,newServiceID];
        }
        case StarSrvGroupClass_clearService :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            [self removeFromObjectMap:NULL];
            l_SrvGroup -> ClearService();
            return nil;
        }
        case StarSrvGroupClass_newParaPkg :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPParaPackageInterface *ParaPkg = l_SrvGroup ->GetParaPkgInterface();
            id vv = [plist objectAtIndex:1];
            if ([vv isKindOfClass:[NSArray class]]) {
                NSArray *parglist = vv;
                if( StarParaPkg_FromTuple_Sub(parglist,0,ParaPkg,0) == VS_FALSE ){
                    ParaPkg ->Release();
                    return nil;
                }
            }else if ([vv isKindOfClass:[NSDictionary class]]) {
                NSDictionary *nd = vv;
                NSMutableArray *parglist = [[NSMutableArray alloc] init];
                for (id key in nd) {
                    id value = nd[key];
                    [parglist addObject:key];
                    [parglist addObject:value];
                }
                if( StarParaPkg_FromTuple_Sub(parglist,0,ParaPkg,0) == VS_FALSE ){
                    ParaPkg ->Release();
                    return nil;
                }
                ParaPkg ->AsDict(VS_TRUE);
            }else{
            }
            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarParaPkgPrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(ParaPkg) forKey:CleObjectID];
            return CleObjectID;
        }
        case StarSrvGroupClass_newBinBuf :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPBinBufInterface *BinBuf = l_SrvGroup ->GetSRPBinBufInterface();
            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarBinBufPrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(BinBuf) forKey:CleObjectID];
            return CleObjectID;
        }
        case StarSrvGroupClass_getServicePath :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            VS_CHAR LocalBuf[512];
            l_SrvGroup -> GetDefaultPath(LocalBuf,512);
            return fromString(LocalBuf);
        }
        case StarSrvGroupClass_setServicePath :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            l_SrvGroup -> SetDefaultPath(toString([plist objectAtIndex:1]));
            return nil;
        }
        case StarSrvGroupClass_servicePathIsSet :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            return fromBool(l_SrvGroup -> DefaultPathIsSet());
        }
        case StarSrvGroupClass_getCurrentPath :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromString("");
            }
            VS_CHAR LocalBuf[512];
            vs_dir_getcwd( LocalBuf,512 );
            return fromString(LocalBuf);
        }
        case StarSrvGroupClass_importService :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            VS_BOOL Result =  l_SrvGroup -> ImportService( toString([plist objectAtIndex:1]), toBool([plist objectAtIndex:2]) );
            return fromBool(Result);
        }
        case StarSrvGroupClass_clearServiceEx :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            [self removeFromObjectMap:NULL];
            l_SrvGroup -> ClearServiceEx();
            return nil;
        }
        case StarSrvGroupClass_runScript :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            VS_CHAR *ScriptBuf = toString([plist objectAtIndex:2]);
            VS_BOOL RetResult = l_SrvGroup -> DoBuffer(toString([plist objectAtIndex:1]),ScriptBuf,(VS_INT32)vs_string_strlen(ScriptBuf),VS_FALSE,toString([plist objectAtIndex:3]));
            return fromBool(RetResult);
        }
        case StarSrvGroupClass_runScriptEx :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            class ClassOfSRPBinBufInterface *l_BinBuf= (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:2]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:2]);
                return fromBool(VS_FALSE);
            }
            VS_BOOL RetResult = l_SrvGroup -> DoBuffer(toString([plist objectAtIndex:1]),l_BinBuf -> GetBufPtr(0),l_BinBuf -> GetOffset(),VS_FALSE,toString([plist objectAtIndex:3]));
            return fromBool(RetResult);
        }
        case StarSrvGroupClass_doFile :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            VS_BOOL Result = l_SrvGroup -> DoFileEx( toString([plist objectAtIndex:1]),toString([plist objectAtIndex:2]),NULL,NULL,VS_FALSE,NULL );
            return fromBool(Result);
        }
        case StarSrvGroupClass_doFileEx :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            VS_BOOL Result = l_SrvGroup -> DoFileEx( toString([plist objectAtIndex:1]),toString([plist objectAtIndex:2]),NULL,NULL,VS_FALSE,toString([plist objectAtIndex:3]) );
            return fromBool(Result);
        }
        case StarSrvGroupClass_setClientPort :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            VS_BOOL RetResult = l_SrvGroup -> SetClientPort(toString([plist objectAtIndex:1]),toInt32([plist objectAtIndex:2]));
            return fromBool(RetResult);
        }
        case StarSrvGroupClass_setTelnetPort :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            VS_BOOL RetResult = l_SrvGroup -> SetTelnetPort(toInt32([plist objectAtIndex:1]));
            return fromBool(RetResult);
        }
        case StarSrvGroupClass_setOutputPort :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            VS_BOOL RetResult = l_SrvGroup -> SetOutputPort(toString([plist objectAtIndex:1]),toInt32([plist objectAtIndex:2]));
            return fromBool(RetResult);
        }
        case StarSrvGroupClass_setWebServerPort :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            VS_BOOL RetResult = l_SrvGroup -> SetWebServerPort(toString([plist objectAtIndex:1]),toInt32([plist objectAtIndex:2]),toInt32([plist objectAtIndex:3]),toInt32([plist objectAtIndex:4]));
            return fromBool(RetResult);
        }
        case StarSrvGroupClass_initRaw :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            class ClassOfSRPInterface *l_Service= (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:2]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:2]);
                return fromBool(VS_FALSE);
            }
            VS_BOOL Result = l_SrvGroup ->InitRaw(toString([plist objectAtIndex:1]),l_Service);
            return fromBool(Result);
        }
        case StarSrvGroupClass_loadRawModule :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            VS_BOOL Result = l_SrvGroup ->LoadRawModule(toString([plist objectAtIndex:1]),toString([plist objectAtIndex:2]),toString([plist objectAtIndex:3]),toBool([plist objectAtIndex:4]),NULL);
            return fromBool(Result);
        }
        case StarSrvGroupClass_getLastError :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromInt32(0);
            }
            return fromInt32(l_SrvGroup -> GetLastError());
        }
        case StarSrvGroupClass_getLastErrorInfo :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            VS_UINT32 LineIndex;
            VS_CHAR *SourceName,*TextBuf;
            VS_CHAR LocalBuf[512];
            TextBuf = l_SrvGroup -> GetLastErrorInfo(&LineIndex,&SourceName);
            vs_string_snprintf(LocalBuf,512,"[%s:%d]%s",SourceName,LineIndex,TextBuf);
            return fromString(TextBuf);
        }
        case StarSrvGroupClass_getCorePath :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            return fromString(l_SrvGroup -> GetCorePath());
        }
        case StarSrvGroupClass_getUserPath :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            return fromString(l_SrvGroup -> GetUserPath());
        }
        case StarSrvGroupClass_getLocalIP :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"127.0.0.1";
            }
            return fromString(l_SrvGroup -> GetLocalIP());
        }
        case StarSrvGroupClass_getLocalIPEx :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return [[NSArray alloc] initWithObjects:@"127.0.0.1",nil];
            }
            SOCKADDR_IN SockAddr[64];
            VS_CHAR Buf[64];
            VS_INT32 Number,i;
            NSMutableArray *Result = [[NSMutableArray alloc] init];
            Number = l_SrvGroup -> GetLocalIPEx(SockAddr,64);
            for( i=0; i < Number; i++ ){
                sprintf( Buf,"%d.%d.%d.%d",((struct _in_addr *)&SockAddr[i].sin_addr)->S_un.S_un_b.s_b1,
                        ((struct _in_addr *)&SockAddr[i].sin_addr)->S_un.S_un_b.s_b2,
                        ((struct _in_addr *)&SockAddr[i].sin_addr)->S_un.S_un_b.s_b3,
                        ((struct _in_addr *)&SockAddr[i].sin_addr)->S_un.S_un_b.s_b4);
                [Result addObject:fromString(Buf)];
            }
            return Result;
        }
        case StarSrvGroupClass_getObjectNum :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromInt32(0);
            }
            return fromInt32(l_SrvGroup -> GetObjectNum());
        }
        case StarSrvGroupClass_activeScriptInterface :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return [[NSArray alloc] initWithObjects:fromBool(VS_FALSE),fromBool(VS_FALSE),nil];
            }
            VS_BOOL Result,OnLineScriptFlag;
            Result = SRPControlInterface ->ActiveScriptInterface(toString([plist objectAtIndex:1]),&OnLineScriptFlag,NULL);
            return [[NSArray alloc] initWithObjects:fromBool(Result),fromBool(OnLineScriptFlag),nil];
        }
        case StarSrvGroupClass_preCompile :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfBasicSRPInterface *l_SrvGroup = (class ClassOfBasicSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_SrvGroup == NULL ){
                NSLog(@"starsrvgroup object[%@] can not be found..",[plist objectAtIndex:0]);
                return [[NSArray alloc] initWithObjects:fromBool(VS_FALSE),@"error...",nil];
            }
            VS_CHAR *In_ScriptBuf = toString([plist objectAtIndex:2]);
            VS_CHAR *ErrorInfo;
            VS_BOOL Result = l_SrvGroup ->PreCompile(toString([plist objectAtIndex:1]),In_ScriptBuf,(VS_INT32)vs_string_strlen(In_ScriptBuf),"",&ErrorInfo);
            return [[NSArray alloc] initWithObjects:fromBool(Result),fromString(ErrorInfo),nil];
        }

        /*-----------------------------------------------*/
        case StarServiceClass_toString :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            if( l_Service->GetServiceName() == NULL ){
                return @"service not load";
            }else{
                return fromString(l_Service ->GetServiceName());
            }
        }
        case StarServiceClass_get :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            VS_CHAR *ParaName = toString([plist objectAtIndex:1]);
            if( vs_string_strcmp( ParaName, "_Name") == 0 ){
                return fromString(l_Service -> GetServiceName());
            }else if( vs_string_strcmp( ParaName, "_ID") == 0 ){
                VS_UUID ServiceID;
                l_Service -> GetServiceID( &ServiceID );
                return fromString(l_Service -> UuidToString(&ServiceID));
            }else if( vs_string_strcmp( ParaName, "_ServiceGroup") == 0 ){
                class ClassOfBasicSRPInterface *l_BasicSRPInterface = l_Service ->GetBasicInterface();
                if( l_BasicSRPInterface ->GetServiceGroupID() == 0 ){
                    if( BasicSRPInterface == NULL ){
                        BasicSRPInterface = l_BasicSRPInterface;
                    }else{
                        l_BasicSRPInterface ->Release();
                    }
                    NSString *CleObjectID = [self getTagFromObjectMap:BasicSRPInterface];
                    if( CleObjectID == nil ){
                        CleObjectID = [NSString stringWithFormat:@"%@%@",StarSrvGroupPrefix,[[NSUUID UUID] UUIDString]];
                        [CleObjectMap setObject:fromPointer(BasicSRPInterface) forKey:CleObjectID];
                    }
                    return CleObjectID;
                }else{
                    NSString *CleObjectID = [self getTagFromObjectMap:l_BasicSRPInterface];
                    if( CleObjectID == nil ){
                        CleObjectID = [NSString stringWithFormat:@"%@%@",StarSrvGroupPrefix,[[NSUUID UUID] UUIDString]];
                        [CleObjectMap setObject:fromPointer(l_BasicSRPInterface) forKey:CleObjectID];
                    }
                    return CleObjectID;
                }
            }
            return nil;
        }
        case StarServiceClass_getObject :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            void *Object = l_Service ->GetObjectEx(NULL,toString([plist objectAtIndex:1]));
            if( Object == NULL ){
                return nil;
            }
            l_Service ->AddRefEx(Object);
            VS_UUID ObjectID;
            l_Service ->GetID(Object, &ObjectID);
            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarObjectPrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(Object) forKey:CleObjectID];
            return [NSString stringWithFormat:@"%@+%@",CleObjectID,fromString(l_Service->UuidToString(&ObjectID))];
        }
        case StarServiceClass_getObjectEx :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            VS_UUID LocalObjectID;
            l_Service -> StringToUuid(toString([plist objectAtIndex:1]),&LocalObjectID);
            void *Object = l_Service ->GetObject(&LocalObjectID);
            if( Object == NULL ){
                return nil;
            }
            l_Service ->AddRefEx(Object);
            VS_UUID ObjectID;
            l_Service ->GetID(Object, &ObjectID);
            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarObjectPrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(Object) forKey:CleObjectID];
            return [NSString stringWithFormat:@"%@+%@",CleObjectID,fromString(l_Service->UuidToString(&ObjectID))];
        }
        case StarServiceClass_newObject :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            NSArray *args = [plist objectAtIndex:1];
            VS_INT32 Index = 0;
            VS_INT32 argc = (VS_INT32)[args count];
            VS_ATTRIBUTEINFO AttributeInfo;
            VS_CHAR *QueueAttrName,*ObjectNameString,*AttributeChangeString;
            void *SRPObject,*SRPParentObject;
            id ObjectTemp;

            QueueAttrName = NULL;
            SRPParentObject = NULL;
            AttributeChangeString = NULL;
            ObjectNameString = NULL;
            ObjectTemp = NULL;

            ObjectTemp = SRPObject_GetArrayObject(argc,args,Index);
            if( ObjectTemp != nil && [ObjectTemp isKindOfClass:[NSString class]] ){
                if( !StartsWithString(ObjectTemp, StarObjectPrefix) ){
                    QueueAttrName = toString(ObjectTemp );
                    Index ++;
                    ObjectTemp = SRPObject_GetArrayObject(argc,args,Index);
                    if( ObjectTemp == nil ){  //no more parameter
                        ObjectNameString = QueueAttrName;
                        QueueAttrName = NULL;
                        SRPParentObject = NULL;
                    }else if([ObjectTemp isKindOfClass:[NSString class]]){
                        if( !StartsWithString(ObjectTemp, StarObjectPrefix) ){
                            ObjectNameString = QueueAttrName;
                            QueueAttrName = NULL;
                            SRPParentObject = NULL;
                            AttributeChangeString = toString(ObjectTemp);
                            Index ++;
                            ObjectTemp = nil;  //end
                        }else{
                            SRPParentObject = toPointer([CleObjectMap objectForKey:ObjectTemp]);
                            Index ++;
                            ObjectTemp = SRPObject_GetArrayObject(argc,args,Index);
                        }
                    }else{
                        return nil;  /*--input parameter error--*/
                    }
                }else{
                    /*--is starobject--*/
                    SRPParentObject = toPointer([CleObjectMap objectForKey:ObjectTemp]);
                    Index ++;
                    ObjectTemp = SRPObject_GetArrayObject(argc,args,Index);
                }
            }else if( ObjectTemp != nil ){
                return nil;  /*--input parameter error--*/
            }
            if( ObjectTemp != nil ){
                if([ObjectTemp isKindOfClass:[NSString class]]){
                    ObjectNameString = toString(ObjectTemp);
                    Index ++;
                    ObjectTemp = SRPObject_GetArrayObject(argc,args,Index);
                    if( ObjectTemp == nil ){ //no more parameter
                    }else if( [ObjectTemp isKindOfClass:[NSString class]] )
                        AttributeChangeString = toString(ObjectTemp );
                }
            }
            if( SRPParentObject == NULL )
                SRPObject = l_Service -> MallocObjectL(NULL,0,NULL);
            else{
                if( l_Service -> IsObject(SRPParentObject) ){
                    if( QueueAttrName != NULL ){
                        if( l_Service -> GetAttributeInfoEx( l_Service -> GetClass(SRPParentObject), QueueAttrName, &AttributeInfo ) == VS_FALSE ){
                            l_Service->ProcessError(VSFAULT_WARNING,__FILE__,__LINE__,"Get Parent Attribute [%s]",QueueAttrName);
                            return nil;
                        }
                    }else{
                        VS_INT32 AttributeNumber = l_Service -> GetAttributeNumber(l_Service -> GetClass(SRPParentObject));
                        VS_INT32 i;
                        for( i=0; i < AttributeNumber; i++ ){
                            l_Service -> GetAttributeInfo( l_Service -> GetClass(SRPParentObject), i, &AttributeInfo );
                            if( AttributeInfo.Type == VSTYPE_PTR && AttributeInfo.SyncType == VS_TRUE && UUID_ISINVALID(AttributeInfo.StructID) )
                                break;
                        }
                        if( i >= AttributeNumber ){
                            l_Service->ProcessError(VSFAULT_WARNING,__FILE__,__LINE__,"not Found Parent Sync Attribute Queue");
                            return nil;
                        }
                    }
                    SRPObject = l_Service -> MallocObject(SRPParentObject,AttributeInfo.AttributeIndex,NULL,0,NULL);
                }else{
                    SRPObject = l_Service -> MallocObject(SRPParentObject,0,NULL,0,NULL);
                }
            }
            if( SRPObject == NULL ){
                return nil;
            }
            l_Service ->SetSourceScript(SRPObject,VS_SOURCESCRIPT_C);
            if( ObjectNameString != NULL && vs_string_strlen(ObjectNameString) != 0 )
                l_Service -> SetName(SRPObject,ObjectNameString);
            if( AttributeChangeString != NULL && vs_string_strlen(AttributeChangeString) != 0 )
                l_Service -> LuaInitObject(SRPObject,AttributeChangeString);

            VS_UUID ObjectID;
            l_Service ->GetID(SRPObject, &ObjectID);
            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarObjectPrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(SRPObject) forKey:CleObjectID];
            return [NSString stringWithFormat:@"%@+%@",CleObjectID,fromString(l_Service->UuidToString(&ObjectID))];
        }

        case StarServiceClass_runScript :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return [[NSArray alloc] initWithObjects:@false,@"error...",nil];
            }
            VS_CHAR *ScriptBuf = toString([plist objectAtIndex:2]);
            VS_CHAR *ErrorInfo;
            VS_BOOL Result = l_Service -> DoBuffer( toString([plist objectAtIndex:1]),ScriptBuf, (VS_INT32)vs_string_strlen(ScriptBuf), toString([plist objectAtIndex:3]), &ErrorInfo, toString([plist objectAtIndex:4]), VS_FALSE );
            return [[NSArray alloc] initWithObjects:fromBool(Result),fromString(ErrorInfo),nil];
        }
        case StarServiceClass_runScriptEx :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return [[NSArray alloc] initWithObjects:@false,@"error...",nil];
            }
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:2]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return [[NSArray alloc] initWithObjects:@false,@"error...",nil];
            }
            VS_CHAR *ErrorInfo;
            VS_BOOL Result = l_Service -> DoBuffer( toString([plist objectAtIndex:1]),l_BinBuf -> GetBufPtr(0), l_BinBuf -> GetOffset(), toString([plist objectAtIndex:3]), &ErrorInfo, toString([plist objectAtIndex:4]), VS_FALSE );
            return [[NSArray alloc] initWithObjects:fromBool(Result),fromString(ErrorInfo),nil];
        }
        case StarServiceClass_doFile :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return [[NSArray alloc] initWithObjects:@false,@"error...",nil];
            }
            VS_CHAR *ErrorInfo;
            VS_BOOL Result = l_Service -> DoFileEx( toString([plist objectAtIndex:1]),toString([plist objectAtIndex:2]), &ErrorInfo, toString([plist objectAtIndex:3]), VS_FALSE,NULL );
            return [[NSArray alloc] initWithObjects:fromBool(Result),fromString(ErrorInfo),nil];
        }
        case StarServiceClass_doFileEx :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return [[NSArray alloc] initWithObjects:@false,@"error...",nil];
            }
            VS_CHAR *ErrorInfo;
            VS_BOOL Result = l_Service -> DoFileEx( toString([plist objectAtIndex:1]),toString([plist objectAtIndex:2]), &ErrorInfo, toString([plist objectAtIndex:3]), VS_FALSE,toString([plist objectAtIndex:4]) );
            return [[NSArray alloc] initWithObjects:fromBool(Result),fromString(ErrorInfo),nil];
        }
        case StarServiceClass_isServiceRegistered :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            return fromBool(l_Service ->IsRegistered( ));
        }
        case StarServiceClass_checkPassword :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            l_Service ->CheckPassword( toBool([plist objectAtIndex:1]) );
            return nil;
        }
        case StarServiceClass_newRawProxy :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            void *l_Object = toPointer([CleObjectMap objectForKey:[plist objectAtIndex:2]]);
            if( l_Object == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:2]);
                return nil;
            }
            void *Result = l_Service ->NewRawProxy(toString([plist objectAtIndex:1]),l_Object,toString([plist objectAtIndex:3]),toString([plist objectAtIndex:4]),toInt32([plist objectAtIndex:5]));
            if( Result == NULL)
                return nil;
            VS_UUID ObjectID;
            l_Service ->GetID(Result, &ObjectID);
            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarObjectPrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(Result) forKey:CleObjectID];
            return [NSString stringWithFormat:@"%@+%@",CleObjectID,fromString(l_Service->UuidToString(&ObjectID))];
        }
        case StarServiceClass_importRawContext :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            void *Result = l_Service ->ImportRawContext(toString([plist objectAtIndex:1]),toString([plist objectAtIndex:2]),toBool([plist objectAtIndex:3]),toString([plist objectAtIndex:4]));
            if( Result == NULL)
                return nil;
            VS_UUID ObjectID;
            l_Service ->GetID(Result, &ObjectID);
            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarObjectPrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(Result) forKey:CleObjectID];
            return [NSString stringWithFormat:@"%@+%@",CleObjectID,fromString(l_Service->UuidToString(&ObjectID))];
        }
        case StarServiceClass_getLastError :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromInt32(0);
            }
            return fromInt32(l_Service -> GetLastError());
        }
        case StarServiceClass_getLastErrorInfo :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            VS_UINT32 LineIndex;
            VS_CHAR *SourceName,*TextBuf;
            VS_CHAR LocalBuf[512];
            TextBuf = l_Service -> GetLastErrorInfo(&LineIndex,&SourceName);
            vs_string_snprintf(LocalBuf,512,"[%s:%d]%s",SourceName,LineIndex,TextBuf);
            return fromString(TextBuf);
        }

        case StarServiceClass_allObject :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                NSLog(@"service object[%@] can not be found..",[plist objectAtIndex:0]);
                return [[NSMutableArray alloc] init];
            }
            VS_PARAPKGPTR RetParaPkg;
            RetParaPkg = l_Service -> AllObject();
            if( RetParaPkg->GetNumber() == 0){
                RetParaPkg->Release();
                return [[NSMutableArray alloc] init];
            }else{
                NSMutableArray *res = [[NSMutableArray alloc] init];
                for( int i=0; i < RetParaPkg->GetNumber(); i++ ){
                    id vv = StarParaPkg_GetAtIndex(RetParaPkg,i);
                    if( vv != nil )
                        [res addObject:vv];
                    else
                        [res addObject:[NSNull null]];
                }
                return res;
            }
        }

        case StarServiceClass_restfulCall :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPInterface *l_Service = (class ClassOfSRPInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_Service == NULL ){
                return [[NSArray alloc] initWithObjects:fromInt32(400),fromString((VS_CHAR *)"{\"code\": -32600, \"message\": \"call _RestfulCall failed,input para error\"}"),nil];
            }
            VS_CHAR *Url = toString([plist objectAtIndex:1]);
            VS_CHAR *OpCode = toString([plist objectAtIndex:2]);
            VS_CHAR *JsonString = toString([plist objectAtIndex:3]);
            VS_INT32 ResultCode;
            VS_CHAR *Result;

            if( Url == NULL || OpCode == NULL ){
                return [[NSArray alloc] initWithObjects:fromInt32(400),fromString((VS_CHAR *)"{\"code\": -32600, \"message\": \"call _RestfulCall failed,input para error\"}"),nil];
            }
            if( JsonString == NULL ){
                Result = l_Service -> RestfulCall( Url, OpCode, NULL,&ResultCode);
                return [[NSArray alloc] initWithObjects:fromInt32(ResultCode),fromString(Result),nil];
              }else{
                Result = l_Service -> RestfulCall( Url, OpCode, JsonString,&ResultCode);
                return [[NSArray alloc] initWithObjects:fromInt32(ResultCode),fromString(Result),nil];
            }
        }

        /*-----------------------------------------------*/
        case StarParaPkgClass_toString :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            class ClassOfSRPParaPackageInterface* ResultParaPkg = l_ParaPkg ->GetDesc();
            VS_CHAR *JsonStr = ResultParaPkg->ToJSon();
            NSString *ResultObject;
            if( JsonStr == NULL )
                ResultObject = @"parapkg";
            else{
                ResultObject = fromString(JsonStr);
                ResultParaPkg->FreeBuf(JsonStr);
            }
            ResultParaPkg->Release();
            return ResultObject;
        }
        case StarParaPkgClass_GetNumber :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromInt32(0);
            }
            return fromInt32(l_ParaPkg -> GetNumber());
        }
        case StarParaPkgClass_get :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            id IndexOrList = [plist objectAtIndex:1];
            if( [IndexOrList isKindOfClass:[NSArray class]] ){
                NSMutableArray *res = [[NSMutableArray alloc] init];
                for( int i=0; i < [IndexOrList count]; i++ ){
                    id vv = StarParaPkg_GetAtIndex(l_ParaPkg,toInt32([IndexOrList objectAtIndex:i]));
                    if( vv != nil )
                        [res addObject:vv];
                    else
                        [res addObject:[NSNull null]];
                }
                return res;
            }else{
                return StarParaPkg_GetAtIndex(l_ParaPkg,toInt32(IndexOrList));
            }
        }
        case StarParaPkgClass_clear :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            l_ParaPkg ->Clear();
            return nil;
        }
        case StarParaPkgClass_appendFrom :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            class ClassOfSRPParaPackageInterface *s_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:1]]);
            if( s_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:1]);
                return fromBool(VS_FALSE);
            }
            return fromBool(l_ParaPkg -> AppendFrom(s_ParaPkg));
        }
        case StarParaPkgClass_equals :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            class ClassOfSRPParaPackageInterface *s_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:1]]);
            if( s_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:1]);
                return fromBool(VS_FALSE);
            }
            return fromBool(l_ParaPkg -> Equals(s_ParaPkg));
        }
        case StarParaPkgClass_V :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            VS_CHAR *CharValue = l_ParaPkg -> GetValueStr();
            if( CharValue == NULL )
                return @"";
            else{
                return fromString(CharValue);
            }
        }
        case StarParaPkgClass_set :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            id iv = [plist objectAtIndex:2];
            StarParaPkg_FromTuple_Sub([NSArray arrayWithObjects:iv, nil],0, l_ParaPkg, toInt32([plist objectAtIndex:1]));
            return nil;
        }
        case StarParaPkgClass_build :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            l_ParaPkg ->Clear();
            id vv = [plist objectAtIndex:1];
            if ([vv isKindOfClass:[NSArray class]]) {
                NSArray *parglist = vv;
                if( StarParaPkg_FromTuple_Sub(parglist,0,l_ParaPkg,0) == VS_FALSE ){
                    l_ParaPkg ->Clear();
                    return nil;
                }
            }else if ([vv isKindOfClass:[NSDictionary class]]) {
                NSDictionary *nd = vv;
                NSMutableArray *parglist = [[NSMutableArray alloc] init];
                for (id key in nd) {
                    id value = nd[key];
                    [parglist addObject:key];
                    [parglist addObject:value];
                }
                if( StarParaPkg_FromTuple_Sub(parglist,0,l_ParaPkg,0) == VS_FALSE ){
                    l_ParaPkg ->Clear();
                    return nil;
                }
                l_ParaPkg ->AsDict(VS_TRUE);
            }else{
                return nil;
            }
            return nil;
        }
        case StarParaPkgClass_free :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            [CleObjectMap removeObjectForKey:[plist objectAtIndex:0]];
            l_ParaPkg ->Release();
            return nil;
        }
        case StarParaPkgClass_dispose :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            [CleObjectMap removeObjectForKey:[plist objectAtIndex:0]];
            l_ParaPkg ->Release();
            return nil;
        }
        case StarParaPkgClass_releaseOwner :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            [CleObjectMap removeObjectForKey:[plist objectAtIndex:0]];
            l_ParaPkg ->ReleaseOwner();
            return nil;
        }
        case StarParaPkgClass_asDict :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            l_ParaPkg ->AsDict(toBool([plist objectAtIndex:1]));
            return nil;
        }
        case StarParaPkgClass_isDict :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            return fromBool(l_ParaPkg->IsDict());
        }
        case StarParaPkgClass_fromJSon :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            return fromBool(l_ParaPkg->FromJSon(toString([plist objectAtIndex:1])));
        }
        case StarParaPkgClass_toJSon :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            return fromString(l_ParaPkg->ToJSon());
        }
        case StarParaPkgClass_toTuple :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPParaPackageInterface *l_ParaPkg = (class ClassOfSRPParaPackageInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_ParaPkg == NULL ){
                NSLog(@"parapkg object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            return StarParaPkg_ToTuple(l_ParaPkg);
        }

        /*-----------------------------------------------*/
        case StarBinBufClass_toString :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            return fromString(l_BinBuf->GetName());
        }
        case StarBinBufClass_GetOffset :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromInt32(0);
            }
            return fromInt32(l_BinBuf->GetOffset());
        }
        case StarBinBufClass_init :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            l_BinBuf ->Init((VS_UINT32)toInt32([plist objectAtIndex:1]));
            return nil;
        }
        case StarBinBufClass_clear :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            l_BinBuf ->Clear();
            return nil;
        }
        case StarBinBufClass_saveToFile :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            VS_CHAR *FileName;
            FILE *hFile;
            VS_INT32 Length;
            VS_INT8 *Buf;

            Buf = l_BinBuf -> GetBuf( );
            Length = l_BinBuf -> GetOffset( );
            if( Buf == NULL || Length == 0 )
                return fromBool(VS_FALSE);
            FileName = toString([plist objectAtIndex:1]);
            VS_BOOL TxtFileFlag = toBool([plist objectAtIndex:2]);
            if( TxtFileFlag == VS_TRUE )
                hFile = vs_file_fopen( FileName, "wt" );
            else
                hFile = vs_file_fopen( FileName, "wb" );
            if( hFile == NULL ){
                return fromBool(VS_FALSE);
            }
            if( Length != 0 )
                fwrite( Buf, 1, Length, hFile );
            fclose( hFile );
            return fromBool(VS_TRUE);
        }
        case StarBinBufClass_loadFromFile :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            VS_CHAR *FileName;
            FILE *hFile;
            VS_INT32 Length;
            VS_INT8 *Buf;
            VS_BOOL Result;

            FileName = toString([plist objectAtIndex:1]);
            VS_BOOL TxtFileFlag = toBool([plist objectAtIndex:2]);
            if( TxtFileFlag == VS_TRUE )
                hFile = vs_file_fopen( FileName, "rt" );
            else
                hFile = vs_file_fopen( FileName, "rb" );
            if( hFile == NULL ){
                return fromBool(VS_FALSE);
            }
            fseek( hFile, 0, SEEK_END );
            Length = (VS_INT32)ftell( hFile );
            fseek( hFile, 0, SEEK_SET );
            Buf = (VS_INT8 *)malloc(Length);
            Length = (VS_INT32)fread( Buf,1,Length,hFile);
            fclose( hFile );
            l_BinBuf -> Clear();
            Result = l_BinBuf -> Set( 0, Length, Buf );
            free( Buf );
            return fromBool(Result);
        }
        case StarBinBufClass_read :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                NSData *adata = [[NSData alloc] initWithBytes:nil length:0];
                return [FlutterStandardTypedData typedDataWithBytes:adata];
            }
            int Length = toInt32([plist objectAtIndex:2]);
            if( Length <= 0 ){
                NSData *adata = [[NSData alloc] initWithBytes:nil length:0];
                return [FlutterStandardTypedData typedDataWithBytes:adata];
            }
            VS_UINT32 Offset = (VS_UINT32)toInt32([plist objectAtIndex:1]);
            VS_UINT32 Size;
            VS_INT8 *BufPtr;
            BufPtr = (VS_INT8 *)l_BinBuf -> GetBufPtr(0);
            Size = l_BinBuf -> GetOffset();
            if( BufPtr == NULL || Offset >= (VS_INT32)Size ){
                NSData *adata = [[NSData alloc] initWithBytes:nil length:0];
                return [FlutterStandardTypedData typedDataWithBytes:adata];
            }
            if( Offset + Length > (VS_INT32)Size )
                Length = Size - Offset;
            NSData *adata = [[NSData alloc] initWithBytes:&BufPtr[Offset] length:Length];
            return [FlutterStandardTypedData typedDataWithBytes:adata];
        }
        case StarBinBufClass_write :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromInt32(0);
            }
            VS_INT32 Length = toInt32([plist objectAtIndex:3]);
            VS_INT32 Offset = toInt32([plist objectAtIndex:1]);
            VS_INT8 *BufPtr;
            l_BinBuf -> FromRaw(VS_TRUE);
            if( Length == 0 )
                return fromInt32(0);
            FlutterStandardTypedData *Buf = [plist objectAtIndex:2];
            BufPtr = (VS_INT8 *)[[Buf data] bytes];
            if( BufPtr == NULL )
                return fromInt32(0);
            if( l_BinBuf -> Set(Offset,Length,BufPtr) == VS_FALSE ){
                return fromInt32(0);
            }
            return fromInt32(Length);
        }
        case StarBinBufClass_free :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            [CleObjectMap removeObjectForKey:[plist objectAtIndex:0]];
            l_BinBuf ->Release();
            return nil;
        }
        case StarBinBufClass_dispose :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            [CleObjectMap removeObjectForKey:[plist objectAtIndex:0]];
            l_BinBuf ->Release();
            return nil;
        }
        case StarBinBufClass_releaseOwner :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            [CleObjectMap removeObjectForKey:[plist objectAtIndex:0]];
            l_BinBuf ->ReleaseOwner();
            return nil;
        }

        case StarBinBufClass_setOffset :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            VS_INT32 Offset = toInt32([plist objectAtIndex:1]);
            if( Offset < 0 )
                Offset = 0;
            VS_BOOL Result = l_BinBuf -> SetOffset((VS_UINT32)Offset);
            return fromBool(Result);
        }
        case StarBinBufClass_print :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            VS_CHAR *Arg = toString([plist objectAtIndex:1]);
            l_BinBuf ->Print(0,"%s",Arg);
            return nil;
        }

       case StarBinBufClass_asString :
        {
            NSArray *plist = (NSArray *)call.arguments;
            class ClassOfSRPBinBufInterface *l_BinBuf = (class ClassOfSRPBinBufInterface *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_BinBuf == NULL ){
                NSLog(@"binbuf object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            VS_INT32 Length = l_BinBuf -> GetOffset();
            if( Length == 0 ){
                return @"";
            }else{
                VS_CHAR LocalBuf[10240+1];
                VS_CHAR *StringBufPtr;
                if( Length >= 10240 )
                    StringBufPtr = (VS_CHAR *)BasicSRPInterface -> Malloc(Length + 1);
                else
                    StringBufPtr = LocalBuf;
                vs_memcpy( StringBufPtr, l_BinBuf->GetBuf(), Length );
                StringBufPtr[Length] = 0;
                NSString* RetValue = fromString(StringBufPtr );
                if( Length >= 10240 )
                    BasicSRPInterface -> Free(StringBufPtr);
    			return RetValue;
	    	}
        }
        /*-----------------------------------------------*/
        case StarObjectClass_toString :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            VS_CHAR *CharPtr,CharBuf[10240],*InterfaceName;
            CharPtr = l_Service ->GetRawContextType( l_StarObject,&InterfaceName );
            if( CharPtr == NULL ){
                VS_CHAR *StrName = (VS_CHAR *)l_Service ->ScriptCall(l_StarObject,NULL,VS_OBJECTTOSTRINGFUNCNAME,"()s");
                if( StrName != NULL ){
                    return fromString(StrName);
                }
                return fromString(l_Service->GetName(l_StarObject));
            }else{
                VS_CHAR *StrName = (VS_CHAR *)l_Service ->ScriptCall(l_StarObject,NULL,VS_OBJECTTOSTRINGFUNCNAME,"()s");
                if( StrName != NULL ){
                    vs_string_snprintf( CharBuf, 10240, "%s[%s:%s]%s",l_Service->GetName(l_StarObject),InterfaceName,CharPtr,StrName);
                    return fromString(CharBuf );
                }
                vs_string_snprintf( CharBuf, 10240, "%s[%s:%s]",l_Service->GetName(l_StarObject),InterfaceName,CharPtr);
                return fromString(CharBuf );
            }
        }
        case StarObjectClass_get :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            id IndexOrList = [plist objectAtIndex:1];
            if( [IndexOrList isKindOfClass:[NSArray class]]){
                NSMutableArray *res = [[NSMutableArray alloc] init];
                for( int i=0; i < [IndexOrList count]; i++ ){
                    id vv = StarObject_getValue(l_Service,l_StarObject,[IndexOrList objectAtIndex:i]);
                    if( vv != nil )
                        [res addObject:vv];
                    else
                        [res addObject:[NSNull null]];
                }
                return res;
            }else{
                return StarObject_getValue(l_Service,l_StarObject,IndexOrList);
            }
        }

        case StarObjectClass_set :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            VS_CHAR LocalNameBuf[64];
            VS_CHAR * ParaName;
            id Name = [plist objectAtIndex:1];
            id Para = [plist objectAtIndex:2];

            if( [Name isKindOfClass:[NSString class]] ){
                ParaName=toString(Name);
            }else{
                if( [Name isKindOfClass:[NSNumber class]] ){
                    sprintf( LocalNameBuf, "\"%d\"", toInt32( Name) );
                    ParaName = LocalNameBuf;
                }else{
                    return nil;
                }
            }
            if( ParaName[0] == '_' && ParaName[1] == '_' && ParaName[2] == '_' ){
                //---process object namevalue
                if( [Para isKindOfClass:[NSNumber class]] ){
                    if (strcmp([Para objCType], @encode(char)) == 0 || strcmp([Para objCType], @encode(bool)) == 0 )
                        l_Service -> SetNameBoolValue( l_StarObject, &ParaName[3], toBool(Para), VS_FALSE );
                    else if(strcmp([Para objCType], @encode(int)) == 0)
                        l_Service -> SetNameIntValue( l_StarObject, &ParaName[3], toInt32(Para), VS_FALSE );
                    else if(strcmp([Para objCType], @encode(long)) == 0)
                        l_Service -> SetNameIntValue( l_StarObject, &ParaName[3], (VS_INT32)toInt64(Para), VS_FALSE );
                    else if(strcmp([Para objCType], @encode(double)) == 0)
                        l_Service -> SetNameFloatValue( l_StarObject, &ParaName[3], toDouble(Para), VS_FALSE );
                }else if( [Para isKindOfClass:[NSString class]] ){
                    l_Service -> SetNameStrValue( l_StarObject, &ParaName[3], toString(Para), VS_FALSE );
                }
                return nil;
            }
            if( vs_string_strcmp( ParaName, "_Name") == 0 ){
                l_Service -> SetName(l_StarObject,toString(Para));
                return nil;
            }
            //---is object atribute?
            VS_ATTRIBUTEINFO AttributeInfo;
            VS_CHAR Info[256];
            if( l_Service -> GetAttributeInfoEx(l_Service -> GetClass(l_StarObject),ParaName,&AttributeInfo) == VS_TRUE ){
                if( SRPObject_FlutterObjectToAttribute( l_Service,AttributeInfo.Type, AttributeInfo.Length,&AttributeInfo.StructID,Para,0,(VS_UINT8 *)ObjectCreate_AttachBuf ) == VS_FALSE ){
                    sprintf(Info,"Set Object[%s] Attribute [%s] Error",l_Service -> GetName(l_StarObject),AttributeInfo.Name);
                    l_Service->ProcessError(VSFAULT_WARNING, __FILE__,__LINE__,Info );
                    return nil;
                }
                l_Service -> ChangeObject( l_StarObject, AttributeInfo.AttributeIndex, ObjectCreate_AttachBuf );
                return nil;
            }else{
                VS_UUID ObjectID;

                l_Service->GetID(l_StarObject,&ObjectID);
                //---is script defined attribute?
                l_Service ->LuaPushObject(l_StarObject);
                l_Service ->LuaPushString(ParaName);
                l_Service ->LuaGetTable(-2);
                if( l_Service ->LuaIsNil(-1) == VS_FALSE ){
                    VS_UUID DefinedClassID;

                    l_Service ->LuaGetDefinedClass(l_StarObject,&DefinedClassID);
                    if( l_Service ->LuaIsFunction(-1) == VS_FALSE && UUID_ISEQUAL(ObjectID,DefinedClassID) ){
                        l_Service ->LuaPop(2);
                        l_Service ->LuaPushObject(l_StarObject);
                        l_Service ->LuaPushString(ParaName);

                        if( FlutterObjectToLua( l_Service, Para ) == VS_FALSE ){
                            sprintf(Info,"Set Object[%s] Attribute [%s] Error",l_Service -> GetName(l_StarObject),ParaName);
                            l_Service->ProcessError(VSFAULT_WARNING,__FILE__,__LINE__,Info );
                        }
                        l_Service ->LuaSetTable(-3);
                        l_Service ->LuaPop(1);
                        return nil;
                    }
                }
                l_Service ->LuaPop(2);
                if( l_Service ->GetRawLuaSetValueFunc(l_StarObject,ParaName,NULL) != NULL ){
                    l_Service ->LuaPushObject(l_StarObject);
                    l_Service ->LuaPushString(ParaName);

                    if( FlutterObjectToLua( l_Service, Para ) == VS_FALSE ){
                        sprintf(Info,"Set Object[%s] Attribute [%s] Error",l_Service -> GetName(l_StarObject),ParaName);
                        l_Service->ProcessError(VSFAULT_WARNING,__FILE__,__LINE__,Info );
                    }
                    l_Service ->LuaSetTable(-3);
                    l_Service ->LuaPop(1);
                    return nil;
                }
                return nil;
            }
        }
        case StarObjectClass_call :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *SRPObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( SRPObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(SRPObject);
            NSArray *Args = [plist objectAtIndex:2];
            VS_INT32 argc,i,m,n;
            id RetValue,localobject;
            VS_CHAR *FunctionName;
            VS_BOOL Result;

            argc = (VS_INT32)[Args count];
            n = l_Service -> LuaGetTop();
            for( i=0; i < argc; i++ ){
                localobject = [Args objectAtIndex:i];
                FlutterObjectToLua( l_Service, localobject);
            }
            FunctionName = toString([plist objectAtIndex:1]);
            if( l_Service -> LuaCall( SRPObject, FunctionName, argc,-1) ==VS_FALSE ){
                m = l_Service -> LuaGetTop();
                if( m > n )
                    l_Service -> LuaPop(m-n);
                return nil;
            }
            m = l_Service -> LuaGetTop();
            if( m == n ){
                return nil;
            }
            else if( m > n ){
                if( m - n == 1 ){
                    RetValue = LuaToFlutterObject(l_Service, -1, &Result );
                }else{
                    id objvalue;

                    RetValue = [NSMutableArray arrayWithCapacity:m - n];
                    for( i=0; i < m - n; i++ ){
                        objvalue = LuaToFlutterObject(l_Service, i - (m - n),&Result);
                        if( objvalue != nil )
                            RetValue[i] = objvalue;
                        else
                            RetValue[i] = [NSNull null];
                    }
                }
                l_Service -> LuaPop(m-n);
                return RetValue;
            }else{
                l_Service->ProcessError(VSFAULT_WARNING,__FILE__,__LINE__,"call function[%s] failed,lua stack corrupted",FunctionName);
                return nil;
            }
        }

        case StarObjectClass_newObject :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            NSArray *args = [plist objectAtIndex:1];
            VS_INT32 Index = 0;
            VS_INT32 argc = (VS_INT32)[args count];
            VS_ATTRIBUTEINFO AttributeInfo;
            VS_CHAR *QueueAttrName,*ObjectNameString,*AttributeChangeString;
            void *SRPParentObject;
            id ObjectTemp;
            VS_BOOL SkipObjectNameString = VS_FALSE;

            QueueAttrName = NULL;
            SRPParentObject = NULL;
            AttributeChangeString = NULL;
            ObjectNameString = NULL;
            ObjectTemp = NULL;

            ObjectTemp = SRPObject_GetArrayObject(argc,args,Index);
            if( ObjectTemp != nil && [ObjectTemp isKindOfClass:[NSString class]] ){
                if( !StartsWithString(ObjectTemp, StarObjectPrefix) ){
                    QueueAttrName = toString(ObjectTemp );
                    Index ++;
                    ObjectTemp = SRPObject_GetArrayObject(argc,args,Index);
                    if( ObjectTemp == nil ){  //no more parameter
                        ObjectNameString = QueueAttrName;
                        QueueAttrName = NULL;
                        SRPParentObject = NULL;
                        SkipObjectNameString = VS_TRUE;
                    }else if([ObjectTemp isKindOfClass:[NSString class]]){
                        if( !StartsWithString(ObjectTemp, StarObjectPrefix) ){
                            ObjectNameString = QueueAttrName;
                            QueueAttrName = NULL;
                            SRPParentObject = NULL;
                            AttributeChangeString = toString(ObjectTemp);
                            Index ++;
                            SkipObjectNameString = VS_TRUE;
                            ObjectTemp = SRPObject_GetArrayObject(argc,args,Index);  //end
                        }else{
                            SRPParentObject = toPointer([CleObjectMap objectForKey:ObjectTemp]);
                            Index ++;
                            ObjectTemp = SRPObject_GetArrayObject(argc,args,Index);
                        }
                    }else{
                        return nil;  /*--input parameter error--*/
                    }
                }else{
                    /*--is starobject--*/
                    SRPParentObject = toPointer([CleObjectMap objectForKey:ObjectTemp]);
                    Index ++;
                    ObjectTemp = SRPObject_GetArrayObject(argc,args,Index);
                }
            }else if( ObjectTemp != nil){
                return nil;  /*--input parameter error--*/
            }
            if( ObjectTemp != nil && SkipObjectNameString == VS_FALSE ){
                if([ObjectTemp isKindOfClass:[NSString class]]){
                    ObjectNameString = toString(ObjectTemp);
                    Index ++;
                    ObjectTemp = SRPObject_GetArrayObject(argc,args,Index);
                    if( ObjectTemp == nil ){ //no more parameter
                    }else if( [ObjectTemp isKindOfClass:[NSString class]] ){
                        AttributeChangeString = toString(ObjectTemp );
                        Index ++;
                        ObjectTemp = SRPObject_GetArrayObject(argc,args,Index);  //end
                    }
                }
            }
            VS_PARAPKGPTR InitBuf = NULL;
            if( Index < argc ){
                InitBuf = l_Service -> GetParaPkgInterface();
                StarParaPkg_FromTuple_Sub(args,Index,InitBuf,0);
            }

            VS_UUID ObjectClassID;
            VS_INT32 AttachBufSize,AttributeNumber,i;
            void *AttachBuf;
            l_Service -> GetID( l_StarObject, &ObjectClassID );
            AttachBufSize = 0;
            AttachBuf = NULL;
            if( SRPParentObject != NULL ){
                if( QueueAttrName != NULL ){
                    if( l_Service -> GetAttributeInfoEx( l_Service -> GetClass(SRPParentObject), QueueAttrName, &AttributeInfo ) == VS_FALSE ){
                        l_Service->ProcessError(VSFAULT_WARNING,__FILE__,__LINE__,"Get Parent Attribute [%s]",QueueAttrName);
                        if( InitBuf != NULL )
                            InitBuf ->Release();
                        return nil;
                    }
                }else{
                    if( l_Service -> IsObject( SRPParentObject ) == VS_TRUE ){
                        VS_CHAR *ClassName;

                        ClassName = l_Service -> GetName(l_StarObject);
                        AttributeNumber = l_Service -> GetAttributeNumber(l_Service -> GetClass(SRPParentObject));
                        for( i=0; i < AttributeNumber; i++ ){
                            l_Service -> GetAttributeInfo( l_Service -> GetClass(SRPParentObject), i, &AttributeInfo );
                            if( AttributeInfo.Type == VSTYPE_PTR && AttributeInfo.SyncType == VS_TRUE && ( UUID_ISEQUAL(AttributeInfo.StructID,ObjectClassID) || l_Service -> IsInst(&AttributeInfo.StructID,l_StarObject) == VS_TRUE ) )
                                break;
                        }
                        if( i >= AttributeNumber ){
                            for( i=0; i < AttributeNumber; i++ ){
                                l_Service -> GetAttributeInfo( l_Service -> GetClass(SRPParentObject), i, &AttributeInfo );
                                if( AttributeInfo.Type == VSTYPE_PTR && AttributeInfo.SyncType == VS_TRUE && UUID_ISINVALID(AttributeInfo.StructID) )
                                    break;
                            }
                            if( i >= AttributeNumber ){
                                l_Service->ProcessError(VSFAULT_WARNING,__FILE__,__LINE__,"not Found Parent Sync Attribute Queue");
                                if( InitBuf != NULL )
                                    InitBuf ->Release();
                                return nil;
                            }
                        }
                    }else{
                        AttributeInfo.AttributeIndex = 0;
                    }
                }
            }

            void *NewSRPObject = NULL;
            if( SRPParentObject != NULL )
                NewSRPObject = SRPInterface -> IMallocObjectEx( NULL, SRPParentObject, AttributeInfo.AttributeIndex, &ObjectClassID,InitBuf);
            else
                NewSRPObject = SRPInterface -> IMallocObjectLEx( NULL, &ObjectClassID,InitBuf);

            if( InitBuf != NULL )
                InitBuf ->Release();

            if( NewSRPObject == NULL ){
                return nil;
            }
            l_Service ->SetSourceScript(NewSRPObject,VS_SOURCESCRIPT_C);
            if( ObjectNameString != NULL && vs_string_strlen(ObjectNameString) != 0 )
                l_Service -> SetName(NewSRPObject,ObjectNameString);
            if( AttributeChangeString != NULL && vs_string_strlen(AttributeChangeString) != 0 )
                l_Service -> LuaInitObject(NewSRPObject,AttributeChangeString);

            VS_UUID ObjectID;
            l_Service ->GetID(NewSRPObject, &ObjectID);
            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarObjectPrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(NewSRPObject) forKey:CleObjectID];
            return [NSString stringWithFormat:@"%@+%@",CleObjectID,fromString(l_Service->UuidToString(&ObjectID))];
        }
        case StarObjectClass_free :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            [CleObjectMap removeObjectForKey:[plist objectAtIndex:0]];
            l_Service ->UnLockGC(l_StarObject);
            return nil;
        }
        case StarObjectClass_dispose :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            [CleObjectMap removeObjectForKey:[plist objectAtIndex:0]];
            l_Service ->UnLockGC(l_StarObject);
            return nil;
        }
        case StarObjectClass_hasRawContext :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            return fromBool(l_Service ->HasRawContext(l_StarObject));
        }
        case StarObjectClass_rawToParaPkg :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            class ClassOfSRPParaPackageInterface *l_ParaPkg = l_Service -> RawToParaPkg(l_StarObject);
            if( l_ParaPkg == NULL )
                return nil;
            return StarParaPkg_ToTuple(l_ParaPkg);
            /*
            NSString *CleObjectID = [NSString stringWithFormat:@"%@%@",StarParaPkgPrefix,[[NSUUID UUID] UUIDString]];
            [CleObjectMap setObject:fromPointer(ParaPkg) forKey:CleObjectID];
            return CleObjectID;
            */
        }
        case StarObjectClass_getSourceScript :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromInt32(0);
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            return fromInt32(l_Service -> GetSourceScript(l_StarObject));
        }
        case StarObjectClass_getLastError :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromInt32(0);
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            return fromInt32(l_Service -> GetLastError());
        }
        case StarObjectClass_getLastErrorInfo :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"";
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            VS_UINT32 LineIndex;
            VS_CHAR *SourceName,*TextBuf;
            VS_CHAR LocalBuf[512];
            TextBuf = l_Service -> GetLastErrorInfo(&LineIndex,&SourceName);
            vs_string_snprintf(LocalBuf,512,"[%s:%d]%s",SourceName,LineIndex,TextBuf);
            return fromString(TextBuf);
        }
        case StarObjectClass_releaseOwnerEx :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            [CleObjectMap removeObjectForKey:[plist objectAtIndex:0]];
            l_Service ->ReleaseOwnerEx(l_StarObject);
            return nil;
        }
        case StarObjectClass_regScriptProcP :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            if( toBool([plist objectAtIndex:2]) == VS_FALSE )
                l_Service->UnRegLuaFunc(l_StarObject, toString([plist objectAtIndex:1]), (void*)SRPObject_ScriptCallBack, 0);
            else{
                l_Service->RegLuaFunc(l_StarObject, toString([plist objectAtIndex:1]), (void*)SRPObject_ScriptCallBack, 0);
            }
            return nil;
        }

        case StarObjectClass_instNumber :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromInt32(0);
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            return fromInt32(l_Service -> InstNumberEx(l_StarObject));
        }
        case StarObjectClass_changeParent :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            NSString *parentObjectTag = [plist objectAtIndex:1];
            if( parentObjectTag == nil || [parentObjectTag isKindOfClass:[NSNull class]] ){
                l_Service->ChangeParent(l_StarObject,NULL,0);
                return nil;
            }
            void *l_ParentObject = (void *)toPointer([CleObjectMap objectForKey:parentObjectTag]);
            if( l_ParentObject == NULL ){
                NSLog(@"star object[%@] can not be found..",parentObjectTag);
                return nil;
            }
            VS_ATTRIBUTEINFO AttributeInfo;
            VS_INT32 i,AttributeNumber;
            VS_CHAR *QueueAttrName = toString([plist objectAtIndex:2]);
            if( QueueAttrName != NULL ){
                if( l_Service -> GetAttributeInfoEx( l_ParentObject, QueueAttrName, &AttributeInfo ) == VS_FALSE ){
                    return nil;
                }
            }else{
                AttributeNumber = l_Service -> GetAttributeNumber(SRPInterface -> GetClass(l_ParentObject));
                for( i=0; i < AttributeNumber; i++ ){
                  l_Service -> GetAttributeInfo( l_Service -> GetClass(l_ParentObject), i, &AttributeInfo );
                  if( AttributeInfo.Type == VSTYPE_PTR && AttributeInfo.SyncType == VS_TRUE && ( l_Service -> IsInst(&AttributeInfo.StructID,l_StarObject) == VS_TRUE || UUID_ISINVALID(AttributeInfo.StructID) ) )
                    break;
                }
                if( i >= AttributeNumber ){
                  return nil;
                }
            }
            l_Service -> ChangeParent( l_StarObject, l_ParentObject, AttributeInfo.AttributeIndex );
            return nil;
        }

        case StarObjectClass_jsonCall :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return @"{\"jsonrpc\": \"2.0\", \"error\": {\"code\": -32603, \"message\": \"call _JSonCall failed,Internal error\"}, \"id\": null}";
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            VS_CHAR *InputStr = toString([plist objectAtIndex:1]);
               if( InputStr == NULL){
                return @"{\"jsonrpc\": \"2.0\", \"error\": {\"code\": -32603, \"message\": \"call _JSonCall failed,Internal error\"}, \"id\": null}";
            }
            VS_CHAR *ResultStr = l_Service -> JSonCall( l_StarObject, InputStr);
            return fromString(ResultStr);
        }

        case StarObjectClass_active :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            return fromBool(l_Service ->Active(l_StarObject));
        }

        case StarObjectClass_deActive :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return nil;
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            l_Service ->Deactive(l_StarObject);
            return nil;
        }

        case StarObjectClass_isActive :
        {
            NSArray *plist = (NSArray *)call.arguments;
            void *l_StarObject = (void *)toPointer([CleObjectMap objectForKey:[plist objectAtIndex:0]]);
            if( l_StarObject == NULL ){
                NSLog(@"star object[%@] can not be found..",[plist objectAtIndex:0]);
                return fromBool(VS_FALSE);
            }
            class ClassOfSRPInterface *l_Service = SRPInterface->GetSRPInterface(l_StarObject);
            return fromBool(l_Service ->IsActive(l_StarObject));
        }

        default:
            return nil;
    }
}

@end


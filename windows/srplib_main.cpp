#define _INCLUDE_STARLIB
#if defined(ENV_WINDOWS) || defined(ENV_WP) || defined(ENV_WINRT) || defined(ENV_WIN10)
#undef UNICODE
#undef _UNICODE
#pragma warning( disable : 4057 ) 
#pragma warning( disable : 4312 ) 
#pragma warning( disable : 4047 ) 
#pragma warning( disable : 4024 ) 
#pragma warning( disable : 4244 ) 
#pragma warning( disable : 4245 ) 
#pragma warning( disable : 4311 ) 
#pragma warning( disable : 4996 ) 
#pragma warning( disable : 4152 ) 
#endif

#include "vsopencommtype.h"
#include "vsopensyseventdef.h"
#include "vsopenapi.h"
#include "vscoreshell.h"

extern VS_INT8* MDString (VS_INT8 *string);

static VS_INT8 SRPUserVerifyTable[128];
static VS_INT8 *SRPUserCharPtr;

VS_CHAR  g_ShareLibraryPath[512]={0};      /*/data/data/com.srplab.starcore/libs*/
VS_CHAR  g_CoreOperationPath[512]={0};     /*/data/data/com.srplab.starcore/files*/
VS_CHAR  g_CoreLibraryPath[512]={0};      /*/data/data/com.srplab.starcore/libs*/

VS_UUID VSSYSOBJ_OBJID = {0x76598bed,0x067d,0x4af5,{0xa1, 0x95, 0x10, 0x61, 0xcc, 0x04, 0x5b, 0xda}};
VS_UUID VSSYSOBJ_WNDADJUST = {0x6279d8e9,0xd4ea,0x4203,{0xbf, 0xf1, 0x6a, 0x47, 0xa2, 0x7d, 0xac, 0xac}};
VS_UUID VSSYSOBJ_WNDCANBERESIZE = {0xb79c8ef0,0xc502,0x4c78,{0x83, 0x94, 0xed, 0xb0, 0x67, 0xf9, 0x13, 0xfb}};
VS_UUID VSSYSOBJ_WNDRESIZE = {0xe53a770f,0x96eb,0x47c2,{0xb2, 0x98, 0xfb, 0x8e, 0x2d, 0x21, 0xda, 0x3b}};
VS_UUID VSSYSOBJ_EDITSELECT = { 0xea579333, 0x720, 0x4467, { 0xbf, 0x3e, 0xac, 0x6c, 0x5c, 0x50, 0xf7, 0x5e } };
VS_UUID VSSYSOBJ_SETFOCUS = { 0x486decc6, 0xf26e, 0x4f91, { 0x8c, 0x29, 0xe3, 0x19, 0xf7, 0x77, 0x8d, 0x18 } };
VS_UUID VSSYSOBJ_WNDMSG = { 0x3e19a7f5, 0x1424, 0x4238, { 0xa0, 0x24, 0xbe, 0x22, 0x8b, 0xb0, 0xdf, 0xc9 } };

//----
VS_UUID VSSYSDOC_CLASSID = {0x4bedae31, 0xa9c7, 0x433c, { 0x87, 0x5b, 0xf2, 0x16, 0xe, 0xba, 0xf4, 0x34} };
VS_UUID VSSYSDOC_ONGETTEXT = {0x9c1e82db, 0x5414, 0x40a3, { 0xb3, 0x53, 0xbd, 0x2d, 0x10, 0xf1, 0xc6, 0x1d } };
VS_UUID VSSYSDOC_ONSETTEXT = {0x44383716, 0xc4b6, 0x4704, { 0x82, 0x71, 0x78, 0x30, 0xcc, 0xac, 0x67, 0x63 } };
VS_UUID VSSYSDOC_LUA_GETTEXT = {0x1bb67764, 0xd5b2, 0x4631, { 0xa4, 0x9d, 0xdf, 0xde, 0xea, 0x89, 0xfc, 0x3f } };
VS_UUID VSSYSDOC_LUA_SETTEXT = {0x1aebbd25, 0x62c8, 0x4cf5, { 0x97, 0x74, 0x91, 0x58, 0xd0, 0xcc, 0x85, 0x23 } };
VS_UUID VSSYSDOC_ONTEXTCHANGE = {0xd3ed8f0c, 0xdf72, 0x4686, { 0x9d, 0xf1, 0x94, 0x48, 0xf3, 0xaa, 0xd7, 0x35 } };
VS_UUID VSSYSDOC_ONTEXTSELECT = {0x8d9fdd03, 0x8759, 0x457e, { 0xae, 0x1, 0x9e, 0x3b, 0xe8, 0xf4, 0x8c, 0xd6 } };

void SRPCreateUserInfoInit()
{
}

VS_INT8 *SRPCreateUserInfo(VS_CHAR *UserName,VS_INT8 *UserPassword,VS_CHAR *SRPUserName,VS_INT8 *SRPUserPassword)
{
    if( UserName == NULL || UserName[0] == 0 ){
        SRPUserName[0] = 0;
        SRPUserPassword[0] = 0;
        return NULL;
    }else
        strcpy( SRPUserName, UserName );
    if( UserPassword == NULL || UserPassword[0] == 0 ){
        SRPUserPassword[0] = 0;
        return NULL;
    }else{
        SRPUserCharPtr = UserPassword;
        vs_memcpy(SRPUserPassword,SRPUserCharPtr,32);  //---get md5 result
        SRPUserPassword[32] = 0;
        SRPUserVerifyTable[0] = 0;
        return SRPUserVerifyTable;
    }
}

void SRPCreateUserInfoTerm()
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void SRPAPI SRPObject_ReMallocNotifyProc(void *Object,VS_UWORD Para,void *NewObject)
{
    class ClassOfSRPObject *SRPObject;

    SRPObject = (class ClassOfSRPObject *)Para;
    SRPObject -> ThisSRPObject = NewObject;
    if( NewObject == NULL ){
        //---cancel association
        if( SRPObject -> AutoReleaseThis == VS_TRUE )
            SRPObject -> Release();  //---auto release
        else
            SRPObject -> UnWrapObject();
    }
}

ClassOfSRPObject::ClassOfSRPObject()
{
    ThisSRPClassLayer = 0;
    ThisSRPObject = NULL;
    ThisSRPInterface = NULL;
    AutoReleaseThis = VS_FALSE;
    AutoReleaseObject = VS_FALSE;
    WrapObjectFlag = VS_FALSE;
}

ClassOfSRPObject::~ClassOfSRPObject()
{
    if( ThisSRPObject != NULL && WrapObjectFlag == VS_TRUE ){
        ThisSRPInterface -> UnRegReMallocCallBack( ThisSRPObject, SRPObject_ReMallocNotifyProc, (VS_UWORD)this );
        ThisSRPInterface -> SetAppClass( ThisSRPObject, ThisSRPClassLayer, NULL );
        if( AutoReleaseObject == VS_TRUE )
            ThisSRPInterface -> FreeObject( ThisSRPObject );
    }
}

void ClassOfSRPObject::Release()
{
    delete this;
}

VS_CHAR *ClassOfSRPObject::GetSelfName()
{
    static VS_CHAR Buf[64];

    Buf[0] = '\0';
    return Buf;
}

void ClassOfSRPObject::ReleaseAttachClass( void *SRPObject,VS_ULONG ClassLayer)
{
    class ClassOfSRPObject *ClassObject;

    if( SRPObject == NULL )
        return;
    ClassObject = ThisSRPInterface -> GetAppClass( SRPObject, ClassLayer );
    if( ClassObject == NULL )
        return;
    if( ClassObject != this || ClassObject -> ThisSRPInterface != ThisSRPInterface ){
        ClassObject -> ThisSRPInterface -> SetAppClass( ClassObject -> ThisSRPObject, ClassLayer, NULL );
        if( ClassObject -> AutoReleaseObject == VS_TRUE )
            ClassObject -> AutoReleaseObject = VS_FALSE;
        ClassObject -> ThisSRPInterface -> UnRegReMallocCallBack( ClassObject -> ThisSRPObject, SRPObject_ReMallocNotifyProc, (VS_UWORD)ClassObject );
        ClassObject -> ThisSRPObject = NULL;
        ClassObject -> WrapObjectFlag = VS_FALSE;
        if( ClassObject -> AutoReleaseThis == VS_TRUE )
            ClassObject -> Release();
    }
}

class ClassOfSRPObject *ClassOfSRPObject::SRPObjectWrap(void *SRPObject)
{
    class ClassOfSRPObject *ClassObject;
    VS_ULONG In_ClassLayer;

    if( SRPObject == NULL )
        return NULL;
    In_ClassLayer = ThisSRPInterface -> GetLayer( SRPObject );
    ClassObject = ThisSRPInterface -> GetAppClass( SRPObject, In_ClassLayer );
    if( ClassObject != NULL )
        return ClassObject;
    ClassObject = new class ClassOfSRPObject();
    ClassObject -> WrapObject(ThisSRPInterface,VS_FALSE,VS_TRUE,SRPObject, In_ClassLayer);
    return ClassObject;
}

void ClassOfSRPObject::WrapObject(class ClassOfSRPInterface *In_SRPInterface,VS_BOOL In_AutoReleaseObject,VS_BOOL In_AutoReleaseThis,void *SRPObject,VS_ULONG In_ClassLayer)
{
    if( SRPObject != NULL && In_ClassLayer == 0xFFFFFFFF  )
        In_ClassLayer = In_SRPInterface -> GetLayer( SRPObject );
    if( SRPObject != NULL && In_ClassLayer == ThisSRPClassLayer && In_SRPInterface -> GetAppClass( SRPObject, ThisSRPClassLayer ) == this ){
        AutoReleaseObject = In_AutoReleaseObject;
        AutoReleaseThis = In_AutoReleaseThis;
        return;
    }
    if( ThisSRPObject != NULL && WrapObjectFlag == VS_TRUE){
        ThisSRPInterface -> UnRegReMallocCallBack( ThisSRPObject, SRPObject_ReMallocNotifyProc, (VS_UWORD)this );
        ThisSRPInterface -> SetAppClass( ThisSRPObject, ThisSRPClassLayer, NULL );
        if( AutoReleaseObject == VS_TRUE )
            ThisSRPInterface -> FreeObject( ThisSRPObject );
    }
    //---free object attach class
    ThisSRPInterface = In_SRPInterface;
    ReleaseAttachClass(SRPObject,In_ClassLayer);
    //---------
    ThisSRPClassLayer = In_ClassLayer;
    WrapObjectFlag = VS_TRUE;
    ThisSRPObject = SRPObject;
    AutoReleaseObject = In_AutoReleaseObject;
    AutoReleaseThis = In_AutoReleaseThis;
    if( ThisSRPObject != NULL ){
        ThisSRPInterface -> RegReMallocCallBack( ThisSRPObject, SRPObject_ReMallocNotifyProc, (VS_UWORD)this );
        ThisSRPInterface -> SetAppClass( ThisSRPObject, ThisSRPClassLayer, this );
    }
}

void ClassOfSRPObject::UnWrapObject()
{
    if( ThisSRPObject != NULL && WrapObjectFlag == VS_TRUE ){
        ThisSRPInterface -> UnRegReMallocCallBack( ThisSRPObject, SRPObject_ReMallocNotifyProc, (VS_UWORD)this );
        ThisSRPInterface -> SetAppClass( ThisSRPObject, ThisSRPClassLayer, NULL );
        if( AutoReleaseObject == VS_TRUE )
            ThisSRPInterface -> FreeObject( ThisSRPObject );
    }
    WrapObjectFlag = VS_FALSE;
    AutoReleaseObject = VS_FALSE;
    AutoReleaseThis = VS_FALSE;
	ThisSRPObject = NULL;
}

class ClassOfSRPObject *ClassOfSRPObject::GetSRPWrap( class ClassOfSRPInterface *In_SRPInterface,void *SRPObject,VS_ULONG In_ClassLayer)
{
    class ClassOfSRPObject *ClassObject;

    if( SRPObject == NULL || In_SRPInterface == NULL )
        return NULL;
    if( In_ClassLayer == 0xFFFFFFFF )
        In_ClassLayer = In_SRPInterface -> GetLayer( SRPObject );
    ClassObject = In_SRPInterface -> GetAppClass( SRPObject, In_ClassLayer );
    if( ClassObject != NULL )
        return ClassObject;
    ClassObject = new class ClassOfSRPObject();
    ClassObject -> WrapObject(In_SRPInterface,VS_FALSE,VS_TRUE,SRPObject, In_ClassLayer);
    return ClassObject;
}

void ClassOfSRPObject::ProcessObject( class ClassOfSRPInterface *In_SRPInterface,void *SRPObject )
{
    if( ThisSRPObject != NULL && WrapObjectFlag == VS_TRUE){
        WrapObjectFlag = VS_FALSE;
        ThisSRPInterface -> UnRegReMallocCallBack( ThisSRPObject, SRPObject_ReMallocNotifyProc, (VS_UWORD)this );
        ThisSRPInterface -> SetAppClass( ThisSRPObject, ThisSRPClassLayer, NULL );
        if( AutoReleaseObject == VS_TRUE )
            ThisSRPInterface -> FreeObject( ThisSRPObject );
    }
    ThisSRPInterface = In_SRPInterface;
    ThisSRPObject = SRPObject;
    AutoReleaseObject = VS_FALSE;
    AutoReleaseThis = VS_FALSE;
}

//-----------------------------------------------------------
VS_BOOL ClassOfSRPObject::StringToUuid(const VS_INT8 *String,VS_UUID *Uuid)
{
	return ThisSRPInterface -> StringToUuid( String, Uuid );
}
VS_INT8 *ClassOfSRPObject::UuidToString(VS_UUID *Uuid)
{
    return ThisSRPInterface -> UuidToString( Uuid );
}

class ClassOfSRPObject *ClassOfSRPObject::GetParent()
{
    return SRPObjectWrap(ThisSRPInterface -> GetParent( ThisSRPObject ));
}

class ClassOfSRPObject *ClassOfSRPObject::GetClass()
{
    return SRPObjectWrap( ThisSRPInterface -> GetClass( ThisSRPObject ) );
}

void ClassOfSRPObject::GetClassID(VS_UUID *UuidPtr)
{
    ThisSRPInterface -> GetClassID( ThisSRPObject, UuidPtr );
}

void ClassOfSRPObject::GetID(VS_UUID *UuidPtr)
{
    ThisSRPInterface -> GetID( ThisSRPObject, UuidPtr );
}

VS_UUID *ClassOfSRPObject::GetIDEx()
{
    return ThisSRPInterface -> GetIDEx( ThisSRPObject );
}

class ClassOfSRPObject *ClassOfSRPObject::GetPrevEx()
{
    return SRPObjectWrap(ThisSRPInterface -> GetPrevEx( ThisSRPObject ));
}

class ClassOfSRPObject *ClassOfSRPObject::GetNextEx()
{
    return SRPObjectWrap(ThisSRPInterface -> GetNextEx( ThisSRPObject ));
}

class ClassOfSRPObject *ClassOfSRPObject::QueryFirst(void *VSObject)
{
    return SRPObjectWrap(ThisSRPInterface -> QueryFirst( VSObject ));
}

class ClassOfSRPObject *ClassOfSRPObject::QueryNext()
{
    return SRPObjectWrap(ThisSRPInterface -> QueryNext( ThisSRPObject ));
}

class ClassOfSRPObject *ClassOfSRPObject::QueryPrev()
{
    return SRPObjectWrap(ThisSRPInterface -> QueryPrev( ThisSRPObject ));
}

class ClassOfSRPObject *ClassOfSRPObject::QueryFirstActiveChild(VS_UWORD *Context)
{
    return SRPObjectWrap(ThisSRPInterface -> QueryFirstActiveChild( ThisSRPObject, Context ));
}

class ClassOfSRPObject *ClassOfSRPObject::QueryNextActiveChild(VS_UWORD *Context)
{
    return SRPObjectWrap(ThisSRPInterface -> QueryNextActiveChild( Context ));
}

void ClassOfSRPObject::SetSaveFlag(VS_UINT8 SaveFlag)
{
    ThisSRPInterface -> SetSaveFlag( ThisSRPObject, SaveFlag );
}

VS_UINT8 ClassOfSRPObject::GetSaveFlag()
{
    return ThisSRPInterface -> GetSaveFlag( ThisSRPObject );
}

VS_INT8 *ClassOfSRPObject::SaveToBuf( VS_INT32 *ObjectSize, const VS_CHAR *Password, VS_UINT8 SaveFlag, VS_BOOL SaveNameValue )
{
    return ThisSRPInterface -> SaveToBuf( ThisSRPObject, ObjectSize, Password, SaveFlag, SaveNameValue );
}

VS_BOOL ClassOfSRPObject::SaveToFile( const VS_CHAR *FileName, const VS_CHAR *Password, VS_UINT8 SaveFlag, VS_BOOL SaveNameValue )
{
	return ThisSRPInterface -> SaveToFile( ThisSRPObject, FileName, Password, SaveFlag, SaveNameValue );
}

VS_BOOL ClassOfSRPObject::LoadFromBuf( const VS_INT8 *Buf, VS_INT32 BufSize, const VS_CHAR *Password, VS_BOOL LoadAsLocal,VS_BOOL LoadNameValue, VS_BOOL UpdateFlag )
{
    return ThisSRPInterface -> LoadFromBuf( ThisSRPObject, Buf, BufSize, Password, LoadAsLocal, LoadNameValue, UpdateFlag );
}

void ClassOfSRPObject::DeferLoadFromBuf( const VS_INT8 *Buf, VS_INT32 BufSize, const VS_CHAR *Password, VS_BOOL LoadAsLocal,VS_BOOL LoadNameValue, VS_BOOL UpdateFlag )
{
	ThisSRPInterface -> DeferLoadFromBuf( ThisSRPObject, Buf, BufSize, Password, LoadAsLocal, LoadNameValue, UpdateFlag );
}

VS_BOOL ClassOfSRPObject::LoadFromFile( const VS_CHAR *FileName, const VS_CHAR *Password, VS_BOOL LoadAsLocal,VS_BOOL LoadNameValue, VS_BOOL UpdateFlag, VS_BOOL StaticDataUseFile )
{
    return ThisSRPInterface -> LoadFromFile( ThisSRPObject, FileName, Password, LoadAsLocal, LoadNameValue, UpdateFlag, StaticDataUseFile );
}

void ClassOfSRPObject::DeferLoadFromFile( const VS_CHAR *FileName, const VS_CHAR *Password, VS_BOOL LoadAsLocal,VS_BOOL LoadNameValue, VS_BOOL UpdateFlag, VS_BOOL StaticDataUseFile )
{
    ThisSRPInterface -> DeferLoadFromFile( ThisSRPObject, FileName, Password, LoadAsLocal, LoadNameValue, UpdateFlag, StaticDataUseFile );
}

void ClassOfSRPObject::ResetLoad( )
{
    ThisSRPInterface -> ResetLoad( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::IsObjectInActiveSet()
{
    return ThisSRPInterface -> IsObjectInActiveSet( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::GetWebServiceFlag()
{
    return ThisSRPInterface -> GetWebServiceFlag( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::SetWebServiceFlag(VS_BOOL WebServiceFlag)
{
    return ThisSRPInterface -> SetWebServiceFlag( ThisSRPObject,WebServiceFlag );
}

class ClassOfSRPObject *ClassOfSRPObject::QueryFirstInst(VS_QUERYRECORD *QueryRecord)
{
    return SRPObjectWrap(ThisSRPInterface -> QueryFirstInst( QueryRecord, ThisSRPObject ));
}

class ClassOfSRPObject *ClassOfSRPObject::QueryNextInst(VS_QUERYRECORD *QueryRecord)
{
    return SRPObjectWrap(ThisSRPInterface -> QueryNextInst( QueryRecord, ThisSRPObject ));
}

void ClassOfSRPObject::QueryInstClose(VS_QUERYRECORD *QueryRecord)
{
    ThisSRPInterface -> QueryInstClose( QueryRecord );
}


VS_CHAR *ClassOfSRPObject::GetName()
{
    return ThisSRPInterface -> GetName( ThisSRPObject );
}

void ClassOfSRPObject::SetName(const VS_CHAR *Name)
{
    ThisSRPInterface -> SetName( ThisSRPObject, Name );
}

VS_BOOL ClassOfSRPObject::IsInst(VS_UUID *ObjectClassID)
{
    return ThisSRPInterface -> IsInst( ObjectClassID, ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::IsDirectInst(VS_UUID *ObjectClassID)
{
    return ThisSRPInterface -> IsDirectInst( ObjectClassID, ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::IsChild(class ClassOfSRPObject *ParentObject)
{
    return ThisSRPInterface -> IsChild( ThisSRPObject, ParentObject -> ThisSRPObject );
}

void ClassOfSRPObject::GetObjectSysRootItemID(VS_UUID *UuidPtr)
{
    ThisSRPInterface -> GetObjectSysRootItemID( ThisSRPObject, UuidPtr );
}

void *ClassOfSRPObject::GetObjectSysRootItem()
{
    return ThisSRPInterface -> GetObjectSysRootItem( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::IsThisService()
{
    return ThisSRPInterface -> IsThisService( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::IsActiveServiceObject()
{
    return ThisSRPInterface -> IsActiveServiceObject( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::IsThisClient()
{
    return ThisSRPInterface -> IsThisClient( ThisSRPObject );
}

VS_ULONG ClassOfSRPObject::GetClientID()
{
    return ThisSRPInterface -> GetClientID( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::ExportObjectHeader( const VS_CHAR *FileName )
{
    return ThisSRPInterface -> ExportObjectHeader( ThisSRPObject, FileName );
}

VS_BOOL ClassOfSRPObject::IsLocalControl( )
{
    return ThisSRPInterface -> IsLocalControl( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::IsRemoteCreate( )
{
    return ThisSRPInterface -> IsRemoteCreate( ThisSRPObject );
}

void ClassOfSRPObject::SetIDInParent( VS_UINT16 IDInParent )
{
    ThisSRPInterface -> SetIDInParent( ThisSRPObject, IDInParent );
}

VS_UINT16 ClassOfSRPObject::GetIDInParent(  )
{
    return ThisSRPInterface -> GetIDInParent( ThisSRPObject );
}

class ClassOfSRPObject *ClassOfSRPObject::GetChildByID( OBJECTATTRIBUTEINDEX AttributeIndex,VS_UINT16 IDInParent )
{
    return SRPObjectWrap(ThisSRPInterface -> GetChildByID( ThisSRPObject, AttributeIndex, IDInParent ));
}

//--------
VS_BOOL ClassOfSRPObject::GetFunctionID(const VS_CHAR *FuntionName,VS_UUID *FunctionID)
{
    return ThisSRPInterface -> GetFunctionID( ThisSRPObject,FuntionName,FunctionID );
}

VS_BOOL ClassOfSRPObject::GetInEventID(const VS_CHAR *InEventName,VS_UUID *InEventID)
{
    return ThisSRPInterface -> GetInEventID( ThisSRPObject,InEventName,InEventID );
}

VS_BOOL ClassOfSRPObject::GetOutEventID(const VS_CHAR *OutEventName,VS_UUID *OutEventID)
{
    return ThisSRPInterface -> GetOutEventID( ThisSRPObject,OutEventName,OutEventID );
}

VS_CHAR *ClassOfSRPObject::GetOutEventName(VS_UUID *OutEventID)
{
    return ThisSRPInterface -> GetOutEventName( OutEventID );
}

VS_BOOL ClassOfSRPObject::CreateOVLFunction(VS_UUID *OriginFunctionID,void *FuncAddress)
{
    return ThisSRPInterface -> CreateOVLFunction( ThisSRPObject, OriginFunctionID, FuncAddress, NULL  );
}

VS_BOOL ClassOfSRPObject::CreateOVLInEvent(VS_UUID *OutEventID,void *InEventAddress)
{
    return ThisSRPInterface -> CreateOVLInEvent(ThisSRPObject, OutEventID, InEventAddress, NULL  );
}

void *ClassOfSRPObject::GetFunction(VS_UUID *FunctionID,VS_BOOL *GlobalFunctionFlag)
{
    return ThisSRPInterface -> GetFunction(FunctionID,GlobalFunctionFlag );
}

void *ClassOfSRPObject::GetFunctionEx(VS_UUID *FunctionID,VS_BOOL *GlobalFunctionFlag)
{
    return ThisSRPInterface -> GetFunctionEx(ThisSRPObject,FunctionID,GlobalFunctionFlag  );
}

void *ClassOfSRPObject::GetOriFunction(VS_UUID *FunctionID,VS_BOOL *GlobalFunctionFlag)
{
    return ThisSRPInterface -> GetOriFunction(FunctionID,GlobalFunctionFlag  );
}

void ClassOfSRPObject::SetFunction(VS_UUID *FunctionID,void *FuncAddress)
{
    ThisSRPInterface -> SetFunction(FunctionID, FuncAddress  );
}

void ClassOfSRPObject::SetInEvent(VS_UUID *InEventID,void *InEventAddress)
{
    ThisSRPInterface -> SetInEvent(InEventID, InEventAddress  );
}

void *ClassOfSRPObject::GetSysEvent(VS_UWORD *Para)
{
    return ThisSRPInterface -> GetSysEvent(ThisSRPObject,Para  );
}

void ClassOfSRPObject::SetSysEvent(void *SysEventAddress,VS_UWORD Para)
{
    ThisSRPInterface -> SetSysEvent(ThisSRPObject,SysEventAddress,Para  );
}

void ClassOfSRPObject::SetSysEventMask(VS_ULONG EventMask, VSSystemEvent_EventProc EventProc )
{
    ThisSRPInterface -> SetSysEventMask(ThisSRPObject,EventMask,EventProc  );
}

VS_ULONG ClassOfSRPObject::GetSysEventMask(VSSystemEvent_EventProc EventProc )
{
    return ThisSRPInterface -> GetSysEventMask(ThisSRPObject,EventProc  );
}

void ClassOfSRPObject::SetChildEventMask(VS_ULONG ClassLayer,VS_ULONG EventMask )
{
    ThisSRPInterface -> SetChildEventMask(ThisSRPObject,ClassLayer,EventMask  );
}

VS_ULONG ClassOfSRPObject::GetChildEventMask(VS_ULONG ClassLayer )
{
    return ThisSRPInterface -> GetChildEventMask(ThisSRPObject,ClassLayer  );
}

VS_ULONG ClassOfSRPObject::EventIDToDWORD(VS_UUID *OutEventID)
{
    return ThisSRPInterface -> EventIDToDWORD(OutEventID  );
}

VS_ULONG ClassOfSRPObject::EventNameToDWORD(VS_UUID *OutEventID)
{
    return ThisSRPInterface -> EventNameToDWORD(OutEventID  );
}

VS_BOOL ClassOfSRPObject::CreateNameScript( const VS_CHAR *ScriptName,const VS_CHAR *ScriptBuf )
{
    return ThisSRPInterface -> CreateNameScript(ThisSRPObject, ScriptName, ScriptBuf );
}

VS_BOOL ClassOfSRPObject::CreateNameScriptEx( const VS_CHAR *ScriptName,const VS_CHAR *FileName )
{
    return ThisSRPInterface -> CreateNameScriptEx(ThisSRPObject, ScriptName, FileName );
}

void ClassOfSRPObject::DeleteNameScript( const VS_CHAR *ScriptName )
{
    ThisSRPInterface -> DeleteNameScript(ThisSRPObject, ScriptName );
}

void ClassOfSRPObject::ChangeScriptName( const VS_CHAR *ScriptName, const VS_CHAR *NewScriptName)
{
    ThisSRPInterface -> ChangeScriptName(ThisSRPObject, ScriptName, NewScriptName );
}

VS_CHAR *ClassOfSRPObject::GetNameScript( const VS_CHAR *ScriptName)
{
    return ThisSRPInterface -> GetNameScript(ThisSRPObject, ScriptName );
}

VS_CHAR *ClassOfSRPObject::QueryFirstNameScript( VS_CHAR **ScriptPtr )
{
    return ThisSRPInterface -> QueryFirstNameScript(ThisSRPObject, ScriptPtr );
}

VS_CHAR *ClassOfSRPObject::QueryNextNameScript( VS_CHAR **ScriptPtr )
{
    return ThisSRPInterface -> QueryNextNameScript( ScriptPtr );
}

VS_BOOL ClassOfSRPObject::ExecNameScript( const VS_CHAR *ScriptName, VS_INT32 nArgs,VS_INT32 nOutArgs )
{
    return ThisSRPInterface -> ExecNameScript(ThisSRPObject, ScriptName, nArgs, nOutArgs );
}

VS_BOOL ClassOfSRPObject::ExecNameScriptEx( const VS_CHAR *ScriptName, VS_INT32 nArgs,VS_INT32 nOutArgs )
{
    return ThisSRPInterface -> ExecNameScriptEx(ThisSRPObject, ScriptName, nArgs, nOutArgs );
}

VS_BOOL ClassOfSRPObject::ExecScript(const VS_CHAR *FuncName,const VS_INT8 *ScriptBuf,VS_INT32 ScriptBufSize,VS_INT32 nArgs,VS_INT32 nOutArgs)
{
    return ThisSRPInterface -> ExecScript(ThisSRPObject, FuncName, ScriptBuf, ScriptBufSize, nArgs, nOutArgs );
}

VS_BOOL ClassOfSRPObject::ExecScriptEx(const VS_CHAR *FuncName,const VS_CHAR *FileName,VS_INT32 nArgs,VS_INT32 nOutArgs)
{
    return ThisSRPInterface -> ExecScriptEx(ThisSRPObject, FuncName, FileName, nArgs, nOutArgs );
}

void ClassOfSRPObject::RemoteCall(VS_ULONG ClientID,VS_UUID *FunctionID,...)
{
	va_list argList;

    va_start( argList, FunctionID);
    ThisSRPInterface -> RemoteCallVar(ClientID,ThisSRPObject,FunctionID,argList );
    va_end(argList);
}

void ClassOfSRPObject::RemoteCallVar(VS_ULONG ClientID,VS_UUID *FunctionID,va_list argList)
{
    ThisSRPInterface -> RemoteCallVar(ClientID,ThisSRPObject,FunctionID,argList );
}

void ClassOfSRPObject::RemoteCallEx(VS_ULONG ExcludeClientID,VS_UUID *FunctionID,...)
{
	va_list argList;

    va_start( argList, FunctionID);
    ThisSRPInterface -> RemoteCallExVar(ExcludeClientID,ThisSRPObject,FunctionID,argList );
    va_end(argList);
}

void ClassOfSRPObject::RemoteCallExVar(VS_ULONG ExcludeClientID,VS_UUID *FunctionID,va_list argList)
{
    ThisSRPInterface -> RemoteCallExVar(ExcludeClientID,ThisSRPObject,FunctionID,argList );
}

VS_ULONG ClassOfSRPObject::GetRemoteID()
{
    return ThisSRPInterface -> GetRemoteID( ThisSRPObject );
}

VS_UINT16 ClassOfSRPObject::GetRemoteSourceTag()
{
    return ThisSRPInterface -> GetRemoteSourceTag( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::IsRemoteCall()
{
    return ThisSRPInterface -> IsRemoteCall( ThisSRPObject );
}

VS_ULONG ClassOfSRPObject::GetRemotePrivateTag()
{
    return ThisSRPInterface -> GetRemotePrivateTag( ThisSRPObject );
}

void *ClassOfSRPObject::GetRemoteAttach()
{
    return ThisSRPInterface -> GetRemoteAttach( ThisSRPObject );
}

VS_UWORD ClassOfSRPObject::SRemoteCall(VS_ULONG WaitTime,VS_ULONG ClientID,VS_ULONG *RetCode,VS_UUID *FunctionID,...)
{
	va_list argList;
    VS_UWORD Result;

    va_start( argList, FunctionID);
    Result = ThisSRPInterface -> SRemoteCallVar(WaitTime,ClientID,RetCode,ThisSRPObject,FunctionID,argList );
    va_end(argList);
    return Result;
}

VS_UWORD ClassOfSRPObject::SRemoteCallVar(VS_ULONG WaitTime,VS_ULONG ClientID,VS_ULONG *RetCode,VS_UUID *FunctionID,va_list argList)
{
    VS_UWORD Result;

    Result = ThisSRPInterface -> SRemoteCallVar(WaitTime,ClientID,RetCode,ThisSRPObject,FunctionID,argList );
    return Result;
}

VS_BOOL ClassOfSRPObject::ARemoteCall(VS_ULONG WaitTime,VS_ULONG ClientID,VS_RemoteCallResponseProc CallBackProc,VS_UWORD Para,VS_UUID *FunctionID,...)
{
	va_list argList;
    VS_BOOL Result;

    va_start( argList, FunctionID);
    Result = ThisSRPInterface -> ARemoteCallVar(WaitTime,ClientID,CallBackProc,Para,ThisSRPObject,FunctionID,argList );
    va_end(argList);
    return Result;
}

VS_BOOL ClassOfSRPObject::ARemoteCallVar(VS_ULONG WaitTime,VS_ULONG ClientID,VS_RemoteCallResponseProc CallBackProc,VS_UWORD Para,VS_UUID *FunctionID,va_list argList)
{
    VS_BOOL Result;

    Result = ThisSRPInterface -> ARemoteCallVar(WaitTime,ClientID,CallBackProc,Para,ThisSRPObject,FunctionID,argList );
    return Result;
}

VS_ULONG ClassOfSRPObject::GetRemoteCallID()
{
    return ThisSRPInterface -> GetRemoteCallID(ThisSRPObject);
}

VS_CHAR *ClassOfSRPObject::GetRemoteCallName()
{
    return ThisSRPInterface -> GetRemoteCallName(ThisSRPObject);
}

void ClassOfSRPObject::SetDeferRspFlag()
{
    ThisSRPInterface -> SetDeferRspFlag(ThisSRPObject);
}

void ClassOfSRPObject::SetRetCode(VS_ULONG RemoteRetCode)
{
    ThisSRPInterface -> SetRetCode(ThisSRPObject,RemoteRetCode);
}

void ClassOfSRPObject::SetRemoteRspAttach(void *RemoteAttach)
{
    ThisSRPInterface -> SetRemoteRspAttach(ThisSRPObject,RemoteAttach);
}

void ClassOfSRPObject::RemoteCallRsp(VS_ULONG ClientID,VS_ULONG RemoteCallID,const VS_CHAR *RemoteCallName,VS_UINT16 RemoteSourceTag,VS_ULONG RetCode,VS_UINT8 RetType,VS_UWORD RetValue, void *RemoteAttach)
{
    ThisSRPInterface -> RemoteCallRsp(ThisSRPObject,ClientID, RemoteCallID, RemoteCallName, RemoteSourceTag,RetCode,RetType,RetValue,RemoteAttach );
}

VS_UINT8 ClassOfSRPObject::GetSyncStatus( )
{
    return ThisSRPInterface -> GetSyncStatus( ThisSRPObject );
}

VS_UINT8 ClassOfSRPObject::GetSyncGroupStatus( VS_UUID *SysRootItemID,VS_SYNCGROUP GroupIndex )
{
    return ThisSRPInterface -> GetSyncGroupStatus( SysRootItemID, GroupIndex );
}

VS_UINT8 ClassOfSRPObject::GetSyncGroupStatusEx( VS_CHAR *SysRootItemName,VS_SYNCGROUP GroupIndex )
{
    return ThisSRPInterface -> GetSyncGroupStatusEx( SysRootItemName, GroupIndex );
}

void ClassOfSRPObject::SetSyncGroup(VS_SYNCGROUP GroupIndex)
{
    ThisSRPInterface -> SetSyncGroup( ThisSRPObject, GroupIndex );
}

void ClassOfSRPObject::GetSyncGroup(VS_SYNCGROUP *GroupIndex)
{
    ThisSRPInterface -> GetSyncGroup( ThisSRPObject, GroupIndex );
}

void ClassOfSRPObject::FreeObject()
{
    ThisSRPInterface -> FreeObject( ThisSRPObject );
}

void ClassOfSRPObject::DeferFreeObject()
{
    ThisSRPInterface -> DeferFreeObject( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::IsObjectInFree()
{
    return ThisSRPInterface -> IsObjectInFree( ThisSRPObject );
}

void ClassOfSRPObject::ChangeLocal(OBJECTATTRIBUTEINDEX AttributeIndex,const VS_INT8 *NewValue)
{
    ThisSRPInterface -> ChangeLocal( ThisSRPObject, AttributeIndex, NewValue );
}

VS_BOOL ClassOfSRPObject::ChangeParent(class ClassOfSRPObject *ParentObject)
{
    OBJECTATTRIBUTEINDEX AttributeIndex;

    if( ParentObject == NULL ){
        ThisSRPInterface -> ChangeParent( ThisSRPObject, NULL, 0 );
        return VS_TRUE;
    }
    AttributeIndex = ThisSRPInterface -> AllocQueue( ParentObject -> ThisSRPObject, ThisSRPObject );
    if( AttributeIndex == INVALID_OBJECTATTRIBUTEINDEX )
        return VS_FALSE;
    ThisSRPInterface -> ChangeParent( ThisSRPObject, ParentObject -> ThisSRPObject, AttributeIndex );
    return VS_TRUE;
}

void ClassOfSRPObject::ChangeParent(class ClassOfSRPObject *ParentObject,OBJECTATTRIBUTEINDEX AttributeIndex)
{
    if( ParentObject == NULL )
        ThisSRPInterface -> ChangeParent( ThisSRPObject, ParentObject, AttributeIndex );
    else
        ThisSRPInterface -> ChangeParent( ThisSRPObject, ParentObject -> ThisSRPObject, AttributeIndex );
}

void ClassOfSRPObject::MarkChange(OBJECTATTRIBUTEINDEX AttributeIndex)
{
    ThisSRPInterface -> MarkChange( ThisSRPObject, AttributeIndex );
}

VS_BOOL ClassOfSRPObject::RegBeforeChangeCallBack(VS_ObjectBeforeChangeNotifyProc ObjectBeforeChangeNotifyProc,VS_UWORD Para,VS_BOOL ChildNotify)
{
    return ThisSRPInterface -> RegBeforeChangeCallBack( ThisSRPObject, ObjectBeforeChangeNotifyProc, Para, ChildNotify );
}

VS_BOOL ClassOfSRPObject::RegChangeCallBack(VS_ObjectChangeNotifyProc ObjectChangeNotifyProc,VS_UWORD Para,VS_BOOL ChildNotify)
{
    return ThisSRPInterface -> RegChangeCallBack( ThisSRPObject, ObjectChangeNotifyProc, Para, ChildNotify );
}

VS_BOOL ClassOfSRPObject::RegReMallocCallBack(VS_ObjectReMallocNotifyProc ObjectReMallocNotifyProc,VS_UWORD Para)
{
    return ThisSRPInterface -> RegReMallocCallBack( ThisSRPObject, ObjectReMallocNotifyProc, Para );
}

void ClassOfSRPObject::UnRegBeforeChangeCallBack(VS_ObjectBeforeChangeNotifyProc ObjectBeforeChangeNotifyProc,VS_UWORD Para)
{
    ThisSRPInterface -> UnRegBeforeChangeCallBack( ThisSRPObject, ObjectBeforeChangeNotifyProc, Para );
}

void ClassOfSRPObject::UnRegChangeCallBack(VS_ObjectChangeNotifyProc ObjectChangeNotifyProc,VS_UWORD Para)
{
    ThisSRPInterface -> UnRegChangeCallBack( ThisSRPObject, ObjectChangeNotifyProc, Para );
}

void ClassOfSRPObject::UnRegReMallocCallBack(VS_ObjectReMallocNotifyProc ObjectReMallocNotifyProc,VS_UWORD Para)
{
    ThisSRPInterface -> UnRegReMallocCallBack( ThisSRPObject, ObjectReMallocNotifyProc, Para );
}

VS_BOOL ClassOfSRPObject::RegObjectIDChangeNotify(VS_ObjectIDChangeNotifyProc ChangeNotifyProc,VS_UWORD Para)
{
    return ThisSRPInterface -> RegObjectIDChangeNotify( ChangeNotifyProc, Para );
}

void ClassOfSRPObject::UnRegObjectIDChangeNotify(VS_ObjectIDChangeNotifyProc ChangeNotifyProc,VS_UWORD Para)
{
    ThisSRPInterface -> UnRegObjectIDChangeNotify( ChangeNotifyProc, Para );
}

VS_BOOL ClassOfSRPObject::RegObjectFreeNotify(VS_ObjectFreeNotifyProc FreeNotifyProc,VS_UWORD Para)
{
    return ThisSRPInterface -> RegObjectFreeNotify( FreeNotifyProc, Para );
}

void ClassOfSRPObject::UnRegObjectFreeNotify(VS_ObjectFreeNotifyProc FreeNotifyProc,VS_UWORD Para)
{
    ThisSRPInterface -> UnRegObjectFreeNotify( FreeNotifyProc, Para );
}

void ClassOfSRPObject::DupVString(VS_VSTRING *InVString,VS_VSTRING *OutVString)
{
    ThisSRPInterface -> DupVString( InVString, OutVString );
}

VS_ULONG ClassOfSRPObject::GetVStringBufSize(VS_VSTRING *VString)
{
    return ThisSRPInterface -> GetVStringBufSize( VString );
}

void ClassOfSRPObject::ExpandVStringBufSize(VS_VSTRING *VString,VS_ULONG Size)
{
    ThisSRPInterface -> ExpandVStringBufSize( VString, Size );
}

VS_BOOL ClassOfSRPObject::SetNameBoolValue( const VS_CHAR *Name, VS_BOOL Value, VS_BOOL LocalChange)
{
    return ThisSRPInterface -> SetNameBoolValue( ThisSRPObject, Name, Value, LocalChange );
}

VS_BOOL ClassOfSRPObject::SetNameIntValue( const VS_CHAR *Name, VS_INT32 Value, VS_BOOL LocalChange)
{
    return ThisSRPInterface -> SetNameIntValue( ThisSRPObject, Name, Value, LocalChange );
}

VS_BOOL ClassOfSRPObject::SetNameFloatValue( const VS_CHAR *Name, VS_DOUBLE Value, VS_BOOL LocalChange)
{
    return ThisSRPInterface -> SetNameFloatValue( ThisSRPObject, Name, Value, LocalChange );
}

VS_BOOL ClassOfSRPObject::SetNameBinValue( const VS_CHAR *Name, VS_INT8 *Value, VS_UINT16 ValueSize, VS_BOOL LocalChange)
{
    return ThisSRPInterface -> SetNameBinValue( ThisSRPObject, Name, Value, ValueSize, LocalChange );
}

VS_BOOL ClassOfSRPObject::SetNameStringValue( const VS_CHAR *Name, const VS_CHAR *Value, VS_BOOL LocalChange)
{
    return ThisSRPInterface -> SetNameStrValue( ThisSRPObject, Name, Value, LocalChange );
}

VS_BOOL ClassOfSRPObject::GetNameBoolValue( const VS_CHAR *Name, VS_BOOL DefaultValue )
{
    VS_BOOL Value;

    ThisSRPInterface -> GetNameBoolValue( ThisSRPObject, Name, &Value, DefaultValue );
    return Value;
}

VS_INT32 ClassOfSRPObject::GetNameIntValue( const VS_CHAR *Name, VS_INT32 DefaultValue )
{
    VS_INT32 Value;

    ThisSRPInterface -> GetNameIntValue( ThisSRPObject, Name, &Value, DefaultValue );
    return Value;
}

VS_DOUBLE ClassOfSRPObject::GetNameFloatValue( const VS_CHAR *Name, VS_DOUBLE DefaultValue )
{
    VS_DOUBLE Value;

    ThisSRPInterface -> GetNameFloatValue( ThisSRPObject, Name, &Value, DefaultValue );
    return Value;
}

VS_INT8 *ClassOfSRPObject::GetNameBinValue( const VS_CHAR *Name, VS_UINT16 *ValueSize)
{
    return ThisSRPInterface -> GetNameBinValue( ThisSRPObject, Name, ValueSize );
}

VS_CHAR *ClassOfSRPObject::GetNameStringValue( const VS_CHAR *Name, const VS_CHAR *DefaultValue )
{
    return ThisSRPInterface -> GetNameStrValue( ThisSRPObject, Name, DefaultValue );
}

VS_BOOL ClassOfSRPObject::RegNameValueChangeCallBack(VS_ObjectNameValueChangeNotifyProc ObjectNameValueChangeNotifyProc,VS_UWORD Para)
{
    return ThisSRPInterface -> RegNameValueChangeCallBack( ThisSRPObject, ObjectNameValueChangeNotifyProc, Para );
}

void ClassOfSRPObject::UnRegNameValueChangeCallBack(VS_ObjectNameValueChangeNotifyProc ObjectNameValueChangeNotifyProc,VS_UWORD Para)
{
    ThisSRPInterface -> UnRegNameValueChangeCallBack( ThisSRPObject, ObjectNameValueChangeNotifyProc, Para );
}

VS_ULONG ClassOfSRPObject::GetAllocType()
{
    return ThisSRPInterface -> GetAllocType( ThisSRPObject );
}

VS_ULONG ClassOfSRPObject::GetLayer( )
{
    return ThisSRPInterface -> GetLayer( ThisSRPObject );
}

void ClassOfSRPObject::SetPrivateValue( VS_ULONG ClassLayer,VS_ULONG Index, VS_UWORD Value  )
{
    ThisSRPInterface -> SetPrivateValue( ThisSRPObject, ClassLayer, Index, Value );
}

VS_BOOL ClassOfSRPObject::GetPrivateValue( VS_ULONG ClassLayer,VS_ULONG Index, VS_UWORD *Value, VS_UWORD DefaultValue  )
{
    return ThisSRPInterface -> GetPrivateValue( ThisSRPObject, ClassLayer, Index, Value, DefaultValue );
}

void *ClassOfSRPObject::MallocPrivateBuf( VS_ULONG ClassLayer,VS_ULONG Index, VS_INT32 BufSize  )
{
    return ThisSRPInterface -> MallocPrivateBuf( ThisSRPObject, ClassLayer, Index, BufSize );
}

void *ClassOfSRPObject::GetPrivateBuf( VS_ULONG ClassLayer,VS_ULONG Index, VS_INT32 *BufSize )
{
    return ThisSRPInterface -> GetPrivateBuf( ThisSRPObject, ClassLayer,Index, BufSize );
}

void ClassOfSRPObject::FreePrivate( VS_ULONG ClassLayer,VS_ULONG Index )
{
    ThisSRPInterface -> FreePrivate( ThisSRPObject, ClassLayer, Index );
}

VS_EVENTPARAM_RUNPARAM *ClassOfSRPObject::GetResponseBuf()
{
    return ThisSRPInterface -> GetResponseBuf( );
}

void ClassOfSRPObject::FreeResponseBuf(VS_EVENTPARAM_RUNPARAM *ResponseParam)
{
    ThisSRPInterface -> FreeResponseBuf( ResponseParam );
}

void ClassOfSRPObject::AttachResponseBuf(VS_EVENTPARAM *EventParam,VS_EVENTPARAM_RUNPARAM *ResponseParam)
{
    ThisSRPInterface -> AttachResponseBuf( EventParam, ResponseParam );
}

VS_BOOL ClassOfSRPObject::RegEventFunction(VS_UUID *EventID, void *FuncAddr)
{
    return ThisSRPInterface -> RegEventFunction( ThisSRPObject, EventID, ThisSRPObject, FuncAddr, 0 );
}

void ClassOfSRPObject::UnRegEventFunction(VS_UUID *EventID, void *FuncAddr )
{
    ThisSRPInterface -> UnRegEventFunction( ThisSRPObject, EventID, ThisSRPObject, FuncAddr, 0 );
}

VS_BOOL ClassOfSRPObject::RegSysEventFunction(VS_ULONG SysEventID, void *FuncAddr)
{
    return ThisSRPInterface -> RegSysEventFunction( ThisSRPObject, SysEventID, FuncAddr, 0 );
}

void ClassOfSRPObject::UnRegSysEventFunction(VS_ULONG SysEventID, void *FuncAddr )
{
    ThisSRPInterface -> UnRegSysEventFunction( ThisSRPObject, SysEventID, FuncAddr, 0 );
}

VS_BOOL ClassOfSRPObject::RegEventFunctionEx(class ClassOfSRPObject *SrcObject,VS_UUID *EventID, void *FuncAddr,VS_UWORD Para)
{
    return ThisSRPInterface -> RegEventFunction( SrcObject -> ThisSRPObject, EventID, ThisSRPObject, FuncAddr, Para );
}

void ClassOfSRPObject::UnRegEventFunctionEx(class ClassOfSRPObject *SrcObject,VS_UUID *EventID, void *FuncAddr,VS_UWORD Para )
{
    ThisSRPInterface -> UnRegEventFunction( SrcObject -> ThisSRPObject, EventID, ThisSRPObject, FuncAddr, Para );
}

VS_BOOL ClassOfSRPObject::RegSysEventFunctionEx(VS_ULONG SysEventID, void *FuncAddr,VS_UWORD Para)
{
    return ThisSRPInterface -> RegSysEventFunction( ThisSRPObject, SysEventID, FuncAddr, Para );
}

void ClassOfSRPObject::UnRegSysEventFunctionEx(VS_ULONG SysEventID, void *FuncAddr,VS_UWORD Para )
{
    ThisSRPInterface -> UnRegSysEventFunction( ThisSRPObject, SysEventID, FuncAddr, Para );
}

VS_BOOL ClassOfSRPObject::ActiveCmd(VS_UINT8 ActiveCmd)
{
    return ThisSRPInterface -> ActiveCmd( ThisSRPObject, ActiveCmd );
}

VS_UINT8 ClassOfSRPObject::GetActiveCmd()
{
    return ThisSRPInterface -> GetActiveCmd( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::ActiveClient(VS_ULONG ClientID)
{
    return ThisSRPInterface -> ActiveClient( ClientID, ThisSRPObject );
}

void ClassOfSRPObject::DeactiveClient(VS_ULONG ClientID)
{
    ThisSRPInterface -> DeactiveClient( ClientID, ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::Active()
{
    return ThisSRPInterface -> Active( ThisSRPObject );
}

void ClassOfSRPObject::Deactive()
{
    ThisSRPInterface -> Deactive( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::IsActive()
{
    return ThisSRPInterface -> IsActive( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::SetClientObject(VS_ULONG ClientID)
{
    return ThisSRPInterface -> SetClientObject( ClientID, ThisSRPObject );
}

class ClassOfSRPObject *ClassOfSRPObject::GetClientObject()
{
    return SRPObjectWrap(ThisSRPInterface -> GetClientObject( ));
}

VS_INT32 ClassOfSRPObject::GetDataStatus( VS_ULONG UniqueDataUnitID, VS_UPDOWNFILEMSG *FileInfo )
{
    return ThisSRPInterface -> GetDataStatus( ThisSRPObject, UniqueDataUnitID, FileInfo );
}

VS_INT8 *ClassOfSRPObject::GetStaticData( VS_ULONG UniqueDataUnitID,VS_STATICID *DataVersion,VS_ULONG *DataSize, VS_BOOL AutoDownLoad)
{
    return ThisSRPInterface -> GetStaticData( ThisSRPObject, UniqueDataUnitID, DataVersion, DataSize, AutoDownLoad );
}

VS_INT8 *ClassOfSRPObject::GetStaticDataEx( VS_ULONG UniqueDataUnitID,VS_STATICID *DataVersion,VS_ULONG *DataSize, VS_BOOL AutoDownLoad,VS_CHAR *Token)
{
    return ThisSRPInterface -> GetStaticDataEx( ThisSRPObject, UniqueDataUnitID, DataVersion, DataSize, AutoDownLoad, Token );
}

VS_BOOL ClassOfSRPObject::SetStaticData( VS_ULONG UniqueDataUnitID,VS_ULONG DataSize,VS_INT8 *DataBuf,VS_STATICID *RetDataVersion)
{
    return ThisSRPInterface -> SetStaticData( ThisSRPObject, UniqueDataUnitID, DataSize, DataBuf, RetDataVersion );
}

VS_BOOL ClassOfSRPObject::SetStaticDataEx( VS_ULONG UniqueDataUnitID,VS_ULONG *DataSize,VS_ULONG Offset,VS_CHAR *FileName,VS_STATICID *RetDataVersion)
{
    return ThisSRPInterface -> SetStaticDataEx( ThisSRPObject, UniqueDataUnitID, DataSize, Offset, FileName, RetDataVersion );
}

VS_ULONG ClassOfSRPObject::GetStaticAppCode( OBJECTATTRIBUTEINDEX StaticPersistentAttributeIndex)
{
    return ThisSRPInterface -> GetStaticAppCode( ThisSRPObject, StaticPersistentAttributeIndex );
}

VS_ULONG ClassOfSRPObject::SetupTimer(VS_INT32 Ticket,VS_TimerProc FunctionAddr,VS_UWORD Para1,VS_UWORD Para2,VS_UWORD Para3,VS_UWORD Para4 )
{
    return ThisSRPInterface -> SetupTimer( Ticket, FunctionAddr, ThisSRPObject, Para1, Para2, Para3, Para4 );
}

void ClassOfSRPObject::KillTimer(VS_ULONG TimerID)
{
    ThisSRPInterface -> KillTimer( TimerID );
}

VS_UWORD ClassOfSRPObject::GetTickCount()
{
    return ThisSRPInterface -> GetTickCount();
}

void ClassOfSRPObject::SetRunEnv_FromChildCallBack( VS_RunEnvCallBackProc CallBack,VS_UWORD Para)
{
    ThisSRPInterface ->SetRunEnv_FromChildCallBack( ThisSRPObject, CallBack, Para );
}

void ClassOfSRPObject::SetRunEnv_FromParentCallBack( VS_RunEnvCallBackProc CallBack,VS_UWORD Para)
{
    ThisSRPInterface ->SetRunEnv_FromParentCallBack( ThisSRPObject, CallBack, Para );
}

void ClassOfSRPObject::RegRunEnv_FromParentCallBack( void *ParentObject, VS_RunEnvCallBackProc CallBack,VS_UWORD Para)
{
    ThisSRPInterface ->RegRunEnv_FromParentCallBack( ThisSRPObject, ParentObject, CallBack, Para );
}

void ClassOfSRPObject::UnRegRunEnv_FromParentCallBack( void *ParentObject, VS_RunEnvCallBackProc CallBack,VS_UWORD Para)
{
    ThisSRPInterface ->UnRegRunEnv_FromParentCallBack( ThisSRPObject, ParentObject, CallBack, Para );
}

VS_BOOL ClassOfSRPObject::RunEnvToChild(void *DesObject, struct StructOfVSRunEnv *RunEnvInfo)
{
    return ThisSRPInterface ->RunEnvToChild( ThisSRPObject, DesObject, RunEnvInfo );
}

VS_BOOL ClassOfSRPObject::RunEnvToParent(struct StructOfVSRunEnv *RunEnvInfo)
{
    return ThisSRPInterface ->RunEnvToParent( ThisSRPObject, RunEnvInfo );
}

VS_BOOL ClassOfSRPObject::RegLuaFunc( const VS_CHAR *FuncName, void *FuncAddress, VS_UWORD Para)
{
    return ThisSRPInterface -> RegLuaFunc( ThisSRPObject, FuncName, FuncAddress, Para );
}

void ClassOfSRPObject::UnRegLuaFunc( const VS_CHAR *FuncName, void *FuncAddress, VS_UWORD Para )
{
    ThisSRPInterface ->UnRegLuaFunc( ThisSRPObject, FuncName, FuncAddress, Para );
}

void ClassOfSRPObject::UnRegLuaFuncEx( void *FuncAddress, VS_UWORD Para )
{
    ThisSRPInterface ->UnRegLuaFuncEx( ThisSRPObject, FuncAddress, Para );
}

void ClassOfSRPObject::ValidRegLuaFunc( const VS_CHAR *FuncName, void *FuncAddress, VS_UWORD Para )
{
    ThisSRPInterface ->ValidRegLuaFunc( ThisSRPObject, FuncName, FuncAddress, Para );
}

void ClassOfSRPObject::InValidRegLuaFunc( const VS_CHAR *FuncName, void *FuncAddress, VS_UWORD Para )
{
    ThisSRPInterface ->InValidRegLuaFunc( ThisSRPObject, FuncName, FuncAddress, Para );
}


VS_BOOL ClassOfSRPObject::RegLuaGetValueFunc( VS_LuaGetValueProc GetValueProc, VS_UWORD Para )
{
    return ThisSRPInterface -> RegLuaGetValueFunc( ThisSRPObject, GetValueProc, Para );
}

VS_BOOL ClassOfSRPObject::RegLuaSetValueFunc( VS_LuaSetValueProc SetValueProc, VS_UWORD Para )
{
    return ThisSRPInterface -> RegLuaSetValueFunc( ThisSRPObject, SetValueProc, Para );
}

VS_BOOL ClassOfSRPObject::UnRegLuaGetValueFunc( VS_LuaGetValueProc GetValueProc, VS_UWORD Para )
{
    return ThisSRPInterface -> UnRegLuaGetValueFunc( ThisSRPObject, GetValueProc, Para );
}

VS_BOOL ClassOfSRPObject::UnRegLuaSetValueFunc( VS_LuaSetValueProc SetValueProc, VS_UWORD Para )
{
    return ThisSRPInterface -> UnRegLuaSetValueFunc( ThisSRPObject, SetValueProc, Para );
}

void ClassOfSRPObject::ValidLuaGetValueFunc( VS_LuaGetValueProc GetValueProc, VS_UWORD Para )
{
    ThisSRPInterface ->ValidLuaGetValueFunc( ThisSRPObject, GetValueProc, Para );
}

void ClassOfSRPObject::ValidLuaSetValueFunc( VS_LuaSetValueProc SetValueProc, VS_UWORD Para )
{
    ThisSRPInterface ->ValidLuaSetValueFunc( ThisSRPObject, SetValueProc, Para );
}

void ClassOfSRPObject::InValidLuaGetValueFunc( VS_LuaGetValueProc GetValueProc, VS_UWORD Para )
{
    ThisSRPInterface ->InValidLuaGetValueFunc( ThisSRPObject, GetValueProc, Para );
}

void ClassOfSRPObject::InValidLuaSetValueFunc( VS_LuaSetValueProc SetValueProc, VS_UWORD Para )
{
    ThisSRPInterface ->InValidLuaSetValueFunc( ThisSRPObject, SetValueProc, Para );
}


void *ClassOfSRPObject::GetLua( )
{
    return ThisSRPInterface -> GetLua();
}

VS_BOOL ClassOfSRPObject::DoBuffer( const VS_CHAR *ScriptInterface,const VS_INT8 *ScriptBuf,VS_INT32 ScriptBufSize, const VS_CHAR *ModuleName, VS_CHAR **ErrorInfo, const VS_CHAR *WorkDirectory,VS_BOOL IsUTF8 )
{
	return ThisSRPInterface -> DoBuffer(ScriptInterface,ScriptBuf,ScriptBufSize, ModuleName, ErrorInfo, WorkDirectory, IsUTF8);
}

VS_BOOL ClassOfSRPObject::DoFile( const VS_CHAR *ScriptInterface,const VS_CHAR *FileName, VS_CHAR **ErrorInfo, const VS_CHAR *WorkDirectory,VS_BOOL IsUTF8 )
{
    return ThisSRPInterface -> DoFile(ScriptInterface,FileName, ErrorInfo, WorkDirectory,IsUTF8);
}

VS_BOOL ClassOfSRPObject::DoFileEx( const VS_CHAR *ScriptInterface,const VS_CHAR *FileName, VS_CHAR **ErrorInfo, const VS_CHAR *WorkDirectory,VS_BOOL IsUTF8,const VS_CHAR *ModuleName )
{
    return ThisSRPInterface -> DoFileEx(ScriptInterface,FileName, ErrorInfo, WorkDirectory,IsUTF8,ModuleName);
}


VS_BOOL ClassOfSRPObject::LuaPushObject( )
{
    return ThisSRPInterface -> LuaPushObject( ThisSRPObject );
}

class ClassOfSRPObject *ClassOfSRPObject::LuaToObject( VS_INT32 Index )
{
    return SRPObjectWrap(ThisSRPInterface -> LuaToObject( Index ));
}

void ClassOfSRPObject::LuaRCall(VS_ULONG ClientID,const VS_CHAR *ScriptName, VS_INT32 nArgs)
{
    ThisSRPInterface -> LuaRCall( ClientID, ThisSRPObject, ScriptName, nArgs );
}

void ClassOfSRPObject::LuaRCallEx(VS_ULONG ExcludeClientID,const VS_CHAR *ScriptName, VS_INT32 nArgs)
{
    ThisSRPInterface -> LuaRCallEx( ExcludeClientID, ThisSRPObject, ScriptName, nArgs );
}

VS_BOOL ClassOfSRPObject::LuaSRCall(VS_ULONG WaitTime,VS_ULONG ClientID,const VS_CHAR *ScriptName, VS_INT32 nArgs, VS_INT32 *OutArgs)
{
    return ThisSRPInterface -> LuaSRCall( WaitTime,ClientID, ThisSRPObject, ScriptName, nArgs, OutArgs );
}

VS_BOOL ClassOfSRPObject::LuaARCall(VS_ULONG WaitTime,VS_ULONG ClientID,void *CallBackProc, VS_UWORD Para, const VS_CHAR *ScriptName, VS_INT32 nArgs)
{
    return ThisSRPInterface -> LuaARCall( WaitTime,ClientID, ThisSRPObject, CallBackProc, Para, ScriptName, nArgs  );
}

VS_BOOL ClassOfSRPObject::DefLuaFunction( const VS_CHAR *ScriptName )
{
    return ThisSRPInterface -> DefLuaFunction( ThisSRPObject, ScriptName );
}

VS_BOOL ClassOfSRPObject::SaveToLuaFunc( const VS_CHAR *LuaFileName, const VS_CHAR *FuncName )
{
    return ThisSRPInterface -> SaveToLuaFunc( ThisSRPObject, LuaFileName, FuncName );
}

void ClassOfSRPObject::LuaInitObject(const VS_CHAR *InitScript)
{
    ThisSRPInterface -> LuaInitObject( ThisSRPObject, InitScript );
}

VS_CHAR *ClassOfSRPObject::GetAttributeLuaString()
{
    return ThisSRPInterface -> GetAttributeLuaString( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::LuaGetObjectValue(const VS_CHAR *Name)
{
    return ThisSRPInterface -> LuaGetObjectValue( ThisSRPObject,Name );
}

VS_BOOL ClassOfSRPObject::LuaSetObjectValue(const VS_CHAR *Name)
{
    return ThisSRPInterface -> LuaSetObjectValue( ThisSRPObject,Name );
}

VS_BOOL ClassOfSRPObject::LockGC()
{
    return ThisSRPInterface -> LockGC( ThisSRPObject );
}

VS_BOOL ClassOfSRPObject::UnLockGC()
{
    return ThisSRPInterface -> UnLockGC( ThisSRPObject );
}

//---Client/Send Message
VS_BOOL ClassOfSRPObject::RemoteSend( VS_ULONG ClientID, class ClassOfSRPParaPackageInterface *ParaPkg )
{
    return ThisSRPInterface -> RemoteSend( ThisSRPObject, ClientID, ParaPkg );
}

void ClassOfSRPObject::SetLog(VS_BOOL Flag)
{
    ThisSRPInterface -> SetLog( ThisSRPObject, Flag );
}
/*
VS_ULONG ClassOfSRPObject::Call(VS_CHAR *FunctionName,...)
{
	va_list argList;
    VS_ULONG Result;

    va_start( argList, FunctionName);
    Result = ThisSRPInterface -> CallVar(ThisSRPObject,FunctionName,argList );
    va_end(argList);
    return Result;
}

VS_ULONG ClassOfSRPObject::CallVar(VS_CHAR *FunctionName,va_list argList)
{
    return ThisSRPInterface -> CallVar(ThisSRPObject,FunctionName,argList );
}

VS_FLOAT ClassOfSRPObject::FCall(VS_CHAR *FunctionName,...)
{
	va_list argList;
    VS_FLOAT Result;

    va_start( argList, FunctionName);
    Result = ThisSRPInterface -> FCallVar(ThisSRPObject,FunctionName,argList );
    va_end(argList);
    return Result;
}

VS_FLOAT ClassOfSRPObject::FCallVar(VS_CHAR *FunctionName,va_list argList)
{
    return ThisSRPInterface -> FCallVar(ThisSRPObject,FunctionName,argList );
}

VS_BOOL ClassOfSRPObject::Set(VS_CHAR *AttributeName,...)
{
	va_list argList;
    VS_BOOL Result;

    va_start( argList, AttributeName);
    Result = ThisSRPInterface -> SetVar(ThisSRPObject,AttributeName,argList );
    va_end(argList);
    return Result;
}

VS_BOOL ClassOfSRPObject::SetEx(OBJECTATTRIBUTEINDEX AttributeIndex,...)
{
	va_list argList;
    VS_BOOL Result;

    va_start( argList, AttributeIndex);
    Result = ThisSRPInterface -> SetVarEx(ThisSRPObject,AttributeIndex,argList );
    va_end(argList);
    return Result;
}

VS_BOOL ClassOfSRPObject::SetVar(VS_CHAR *AttributeName,va_list argList)
{
    return ThisSRPInterface -> SetVar(ThisSRPObject,AttributeName,argList );
}

VS_BOOL ClassOfSRPObject::SetVarEx(OBJECTATTRIBUTEINDEX AttributeIndex,va_list argList)
{
    return ThisSRPInterface -> SetVarEx(ThisSRPObject,AttributeIndex,argList );
}

VS_ULONG ClassOfSRPObject::Get(VS_CHAR *AttributeName)
{
    return ThisSRPInterface -> Get(ThisSRPObject,AttributeName );
}

VS_ULONG ClassOfSRPObject::GetEx(OBJECTATTRIBUTEINDEX AttributeIndex)
{
    return ThisSRPInterface -> GetEx(ThisSRPObject,AttributeIndex );
}

VS_FLOAT ClassOfSRPObject::FGet(VS_CHAR *AttributeName)
{
    return ThisSRPInterface -> FGet(ThisSRPObject,AttributeName );
}

VS_FLOAT ClassOfSRPObject::FGetEx(OBJECTATTRIBUTEINDEX AttributeIndex)
{

    return ThisSRPInterface -> FGetEx(ThisSRPObject,AttributeIndex );
}

*/

//------------------------------------------------------------------------------
VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnFirstCreate()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnFirstCreate(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONFIRSTCREATE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnMalloc()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnMalloc(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONMALLOC,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnFree()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnFree(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONFREE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnCreate()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnCreate(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONCREATE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnBeforeDestory()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnBeforeDestory(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONBEFOREDESTORY,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnDestory()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnDestory(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONDESTORY,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnBeforeFirstCreate()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnBeforeFirstCreate(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONBEFOREFIRSTCREATE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnCreateChild()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnCreateChild(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONCREATECHILD,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnDestoryChild()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnDestoryChild(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONDESTORYCHILD,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnActivating()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnActivating(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONACTIVATING,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnDeactivating()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnDeactivating(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONDEACTIVATING,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnActivate()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnActivate(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONACTIVATE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnDeactivate()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnDeactivate(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONDEACTIVATE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnActivateChild()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnActivateChild(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONACTIVATECHILD,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnDeactivateChild()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnDeactivateChild(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONDEACTIVATECHILD,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnAttributeBeforeChange()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnAttributeBeforeChange(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONATTRIBUTEBEFORECHANGE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnAttributeChange()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnAttributeChange(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONATTRIBUTECHANGE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnParentBeforeChange()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnParentBeforeChange(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONPARENTBEFORECHANGE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnParentChange()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnParentChange(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONPARENTCHANGE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnStaticChange()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnStaticChange(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONSTATICCHANGE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnScriptChange()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnScriptChange(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONSCRIPTCHANGE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnBecomeSync()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnBecomeSync(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONBECOMESYNC,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnSyncGroupChange()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnSyncGroupChange(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONSYNCGROUPCHANGE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnActiveSetChange()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnActiveSetChange(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONACTIVESETCHANGE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnChildSyncGroupChange()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnChildSyncGroupChange(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONCHILDSYNCGROUPCHANGE,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnLoadMask()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnLoadMask(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONLOADMASK,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnLoadFinish()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnLoadFinish(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONLOADFINISH,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnRemoteSend()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnRemoteSend(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONREMOTESEND,(void *)In_Value,(VS_UWORD)this);
}

VSSystemEvent_EventProc ClassOfSRPObject::Get_E_OnCall()
{
    return NULL;
}

void ClassOfSRPObject::Put_E_OnCall(VSSystemEvent_EventProc In_Value)
{
    ThisSRPInterface -> RegSysEventFunction(ThisSRPObject,VSEVENT_SYSTEMEVENT_ONCALL,(void *)In_Value,(VS_UWORD)this);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

VS_BOOL VirtualSociety_ClassSkeleton_IsFileExist(VS_INT8 *FileName)
{
    return vs_file_exist(FileName);
}

void VirtualSociety_ClassSkeleton_CheckAndCreateDirectory(VS_INT8 *Path)
{
    vs_dir_create(Path);
}

void GetVirtualSocietyUserPath( VS_INT32 ProgramRunType,VS_INT8 *Filename, VS_UINT32 nSize, VS_BOOL CheckEnvPath )
{
    VS_INT8 LocalBuf[512];
    VS_BOOL AutoCreateDirectoryFlag;

    AutoCreateDirectoryFlag = VS_FALSE;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT  || VS_OS_TYPE == VS_OS_WIN10)
    if( CheckEnvPath == VS_TRUE && (vs_get_env("SRPHOME",LocalBuf,512) == VS_TRUE || strlen(g_CoreOperationPath) != 0 ) ){
        AutoCreateDirectoryFlag = VS_TRUE;
   		if( strlen(g_CoreOperationPath) != 0 )
            strcpy(LocalBuf,g_CoreOperationPath);  //优先使用g_CoreOperationPath
        switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
        case VS_CLIENT:
#ifdef _EXE
            _snprintf( Filename, nSize, "%s\\srplab\\client",LocalBuf);
#else
            _snprintf( Filename, nSize, "%s\\srplab\\clientapp",LocalBuf);
#endif
            break;
        case VS_SERVER:
#ifdef _EXE
            _snprintf( Filename, nSize, "%s\\srplab\\server",LocalBuf);
#else
            _snprintf( Filename, nSize, "%s\\srplab\\serverapp",LocalBuf);
#endif
            break;
        case VS_DEBUG:
            _snprintf( Filename, nSize, "%s\\srplab\\debug",LocalBuf);
            break;
        case VS_CORE :
            _snprintf( Filename, nSize, "%s\\srplab",LocalBuf);
            break;
        default:
            _snprintf( Filename, nSize, "%s\\srplab\\tools",LocalBuf);
            break;
        }
    }else{
		if( strlen(g_CoreOperationPath) != 0 ){  //add 2015/05/25
			AutoCreateDirectoryFlag = VS_TRUE;
			switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
			case VS_CLIENT:
#ifdef _EXE
				_snprintf( Filename, nSize, "%s\\srplab\\client",g_CoreOperationPath);
#else
				_snprintf( Filename, nSize, "%s\\srplab\\clientapp",g_CoreOperationPath);
#endif
				break;
			case VS_SERVER:
#ifdef _EXE
				_snprintf( Filename, nSize, "%s\\srplab\\server",g_CoreOperationPath);
#else
				_snprintf( Filename, nSize, "%s\\srplab\\serverapp",g_CoreOperationPath);
#endif
				break;
			case VS_DEBUG:
				_snprintf( Filename, nSize, "%s\\srplab\\debug",g_CoreOperationPath);
				break;
			case VS_CORE :
				_snprintf( Filename, nSize, "%s\\srplab",g_CoreOperationPath);
				break;
			default:
				_snprintf( Filename, nSize, "%s\\srplab\\tools",g_CoreOperationPath);
				break;
			}
		}else{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	        ::GetEnvironmentVariable("SystemDrive",LocalBuf,512);
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
		    vs_getlocal_dir(LocalBuf,512);
#endif
	  		switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
			case VS_CLIENT:
#ifdef _EXE
	   			_snprintf( Filename, nSize, "%s\\srplab\\client",LocalBuf);
#else
				_snprintf( Filename, nSize, "%s\\srplab\\clientapp",LocalBuf);
#endif
   				break;
	    	case VS_SERVER:
#ifdef _EXE
		    	_snprintf( Filename, nSize, "%s\\srplab\\server",LocalBuf);
#else
			    _snprintf( Filename, nSize, "%s\\srplab\\serverapp",LocalBuf);
#endif
   				break;
	    	case VS_DEBUG:
		    	_snprintf( Filename, nSize, "%s\\srplab\\debug",LocalBuf);
			    break;
	   		case VS_CORE:
				_snprintf( Filename, nSize, "%s\\srplab",LocalBuf);
				break;
	   		default:
				_snprintf( Filename, nSize, "%s\\srplab\\tools",LocalBuf);
				break;
   			}
		}
/*
		OSVERSIONINFO VersionInfo;
        VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        ::GetVersionEx( &VersionInfo );
        switch( VersionInfo.dwMajorVersion ){
        case 5 :  //---2000 xp 2003
            switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
            case VS_CLIENT:
#ifdef _EXE
                _snprintf( Filename, nSize, "%s\\Application Data\\srplab\\client",LocalBuf);
#else
                _snprintf( Filename, nSize, "%s\\Application Data\\srplab\\clientapp",LocalBuf);
#endif
                break;
            case VS_SERVER:
#ifdef _EXE
                _snprintf( Filename, nSize, "%s\\Application Data\\srplab\\server",LocalBuf);
#else
                _snprintf( Filename, nSize, "%s\\Application Data\\srplab\\serverapp",LocalBuf);
#endif
                break;
            case VS_DEBUG:
                _snprintf( Filename, nSize, "%s\\Application Data\\srplab\\debug",LocalBuf);
                break;
            case VS_CORE:
                _snprintf( Filename, nSize, "%s\\Application Data\\srplab",LocalBuf);
                break;
            default:
                _snprintf( Filename, nSize, "%s\\Application Data\\srplab\\tools",LocalBuf);
                break;
            }
            break;
    default :    //---vista
            switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
            case VS_CLIENT:
#ifdef _EXE
                _snprintf( Filename, nSize, "%s\\srplab\\client",LocalBuf);
#else
                _snprintf( Filename, nSize, "%s\\srplab\\clientapp",LocalBuf);
#endif
                break;
            case VS_SERVER:
#ifdef _EXE
            _snprintf( Filename, nSize, "%s\\srplab\\server",LocalBuf);
#else
            _snprintf( Filename, nSize, "%s\\srplab\\serverapp",LocalBuf);
#endif
                break;
            case VS_DEBUG:
                _snprintf( Filename, nSize, "%s\\srplab\\debug",LocalBuf);
                break;
            case VS_CORE:
                _snprintf( Filename, nSize, "%s\\srplab",LocalBuf);
                break;
            default:
                _snprintf( Filename, nSize, "%s\\srplab\\tools",LocalBuf);
                break;
            }
            break;
        }
*/
    }
#endif

#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_MACOS )
    if( CheckEnvPath == VS_TRUE && (vs_get_env("SRPHOME",LocalBuf,512) == VS_TRUE || strlen(g_CoreOperationPath) != 0 ) ){
        AutoCreateDirectoryFlag = VS_TRUE;
   		if( strlen(g_CoreOperationPath) != 0 )
            strcpy(LocalBuf,g_CoreOperationPath);  //优先使用g_CoreOperationPath
        switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
        case VS_CLIENT:
            vs_string_snprintf( Filename, nSize, "%s/srplab/clientapp",LocalBuf);
            break;
        case VS_SERVER:
            vs_string_snprintf( Filename, nSize, "%s/srplab/serverapp",LocalBuf);
            break;
        case VS_DEBUG:
            vs_string_snprintf( Filename, nSize, "%s/srplab/debug",LocalBuf);
            break;
        case VS_CORE:
            vs_string_snprintf( Filename, nSize, "%s/srplab",LocalBuf);
            break;
        default:
            vs_string_snprintf( Filename, nSize, "%s/srplab/tools",LocalBuf);
            break;
        }
    }else{
		if( strlen(g_CoreOperationPath) != 0 ){  //add 2015/05/25
			AutoCreateDirectoryFlag = VS_TRUE;
			switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
			case VS_CLIENT:
				vs_string_snprintf( Filename, nSize, "%s/srplab/clientapp",g_CoreOperationPath);
				break;
			case VS_SERVER:
				vs_string_snprintf( Filename, nSize, "%s/srplab/serverapp",g_CoreOperationPath);
				break;
			case VS_DEBUG:
				vs_string_snprintf( Filename, nSize, "%s/srplab/debug",g_CoreOperationPath);
				break;
			case VS_CORE:
				vs_string_snprintf( Filename, nSize, "%s/srplab",g_CoreOperationPath);
				break;
			default:
				vs_string_snprintf( Filename, nSize, "%s/srplab/tools",g_CoreOperationPath);
				break;
			}
		}else{
	        switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
		    case VS_CLIENT:
			    vs_string_snprintf( Filename, nSize, "/usr/local/srplab/clientapp");
				break;
	        case VS_SERVER:
		        vs_string_snprintf( Filename, nSize, "/usr/local/srplab/serverapp");
			    break;
	        case VS_DEBUG:
		        vs_string_snprintf( Filename, nSize, "/usr/local/srplab/debug");
			    break;
	        case VS_CORE:
		        vs_string_snprintf( Filename, nSize, "/usr/local/srplab");
			    break;
	        default:
		        vs_string_snprintf( Filename, nSize, "/usr/local/srplab/tools");
			    break;
			}
		}
    }
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
    if( CheckEnvPath == VS_TRUE && (vs_get_env("SRPHOME",LocalBuf,512) == VS_TRUE || strlen(g_CoreOperationPath) != 0 ) ){
        AutoCreateDirectoryFlag = VS_TRUE;
   		if( strlen(g_CoreOperationPath) != 0 )
            strcpy(LocalBuf,g_CoreOperationPath);  //use g_CoreOperationPath first
        switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
        case VS_CLIENT:
            vs_string_snprintf( Filename, nSize, "%s/srplab/clientapp",LocalBuf);
            break;
        case VS_SERVER:
            vs_string_snprintf( Filename, nSize, "%s/srplab/serverapp",LocalBuf);
            break;
        case VS_DEBUG:
            vs_string_snprintf( Filename, nSize, "%s/srplab/debug",LocalBuf);
            break;
        case VS_CORE:
            vs_string_snprintf( Filename, nSize, "%s/srplab",LocalBuf);
            break;
        default:
            vs_string_snprintf( Filename, nSize, "%s/srplab/tools",LocalBuf);
            break;
        }
    }else{
		if( strlen(g_CoreOperationPath) != 0 ){  //add 2015/05/25
			AutoCreateDirectoryFlag = VS_TRUE;
			switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
			case VS_CLIENT:
				vs_string_snprintf( Filename, nSize, "%s/srplab/clientapp",g_CoreOperationPath);
				break;
			case VS_SERVER:
				vs_string_snprintf( Filename, nSize, "%s/srplab/serverapp",g_CoreOperationPath);
				break;
			case VS_DEBUG:
				vs_string_snprintf( Filename, nSize, "%s/srplab/debug",g_CoreOperationPath);
				break;
			case VS_CORE:
				vs_string_snprintf( Filename, nSize, "%s/srplab",g_CoreOperationPath);
				break;
			default:
				vs_string_snprintf( Filename, nSize, "%s/srplab/tools",g_CoreOperationPath);
				break;
			}
		}else{
	        switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
		    case VS_CLIENT:
			    vs_string_snprintf( Filename, nSize, "/usr/local/srplab/clientapp");
				break;
	        case VS_SERVER:
		        vs_string_snprintf( Filename, nSize, "/usr/local/srplab/serverapp");
			    break;
	        case VS_DEBUG:
		        vs_string_snprintf( Filename, nSize, "/usr/local/srplab/debug");
			    break;
	        case VS_CORE:
		        vs_string_snprintf( Filename, nSize, "/usr/local/srplab");
			    break;
	        default:
		        vs_string_snprintf( Filename, nSize, "/usr/local/srplab/tools");
			    break;
			}
		}
    }
#endif
#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
    if( CheckEnvPath == VS_TRUE && (vs_get_env("SRPHOME",LocalBuf,512) == VS_TRUE || strlen(g_CoreOperationPath) != 0 ) ){
        AutoCreateDirectoryFlag = VS_TRUE;
   		if( strlen(g_CoreOperationPath) != 0 )
            strcpy(LocalBuf,g_CoreOperationPath);  //use g_CoreOperationPath first
        switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
        case VS_CLIENT:
            vs_string_snprintf( Filename, nSize, "%s/srplab/clientapp",LocalBuf);
            break;
        case VS_SERVER:
            vs_string_snprintf( Filename, nSize, "%s/srplab/serverapp",LocalBuf);
            break;
        case VS_DEBUG:
            vs_string_snprintf( Filename, nSize, "%s/srplab/debug",LocalBuf);
            break;
        case VS_CORE:
            vs_string_snprintf( Filename, nSize, "%s/srplab",LocalBuf);
            break;
        default:
            vs_string_snprintf( Filename, nSize, "%s/srplab/tools",LocalBuf);
            break;
        }
    }else{
		if( strlen(g_CoreOperationPath) != 0 ){  //add 2015/05/25
			AutoCreateDirectoryFlag = VS_TRUE;
			switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
			case VS_CLIENT:
				vs_string_snprintf( Filename, nSize, "%s/srplab/clientapp",g_CoreOperationPath);
				break;
			case VS_SERVER:
				vs_string_snprintf( Filename, nSize, "%s/srplab/serverapp",g_CoreOperationPath);
				break;
			case VS_DEBUG:
				vs_string_snprintf( Filename, nSize, "%s/srplab/debug",g_CoreOperationPath);
				break;
			case VS_CORE:
				vs_string_snprintf( Filename, nSize, "%s/srplab",g_CoreOperationPath);
				break;
			default:
				vs_string_snprintf( Filename, nSize, "%s/srplab/tools",g_CoreOperationPath);
				break;
			}
		}else{
	        switch(VS_BASIC_PROGRAMTYPE(ProgramRunType)){
		    case VS_CLIENT:
			    vs_string_snprintf( Filename, nSize, "/sdcard/srplab/clientapp");
				break;
	        case VS_SERVER:
		        vs_string_snprintf( Filename, nSize, "/sdcard/srplab/serverapp");
			    break;
	        case VS_DEBUG:
		        vs_string_snprintf( Filename, nSize, "/sdcard/srplab/debug");
			    break;
	        case VS_CORE:
		        vs_string_snprintf( Filename, nSize, "/sdcard/srplab");
			    break;
			default:
				vs_string_snprintf( Filename, nSize, "/sdcard/srplab/tools");
			    break;
			}
	    }
    }
#endif
    if( AutoCreateDirectoryFlag == VS_TRUE )   /* user temp path, we create by default */
        VirtualSociety_ClassSkeleton_CheckAndCreateDirectory( Filename );
}

VS_BOOL vs_dir_getcorepath(VS_CHAR *Buf,VS_ULONG Size)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
    if( vs_string_strlen(g_CoreLibraryPath) == 0 ){
        if( vs_string_strlen(g_ShareLibraryPath) == 0 ){
			VS_CHAR LocalBuf[512];
			
			::GetWindowsDirectory(LocalBuf,512);
			vs_string_snprintf(Buf,Size,"%s\\system32",LocalBuf);
			return VS_TRUE;
        }else
            vs_string_snprintf(Buf,Size,"%s",g_ShareLibraryPath);
    }else
        vs_string_snprintf(Buf,Size,"%s",g_CoreLibraryPath);
    return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    if( vs_string_strlen(g_CoreLibraryPath) == 0 ){
        if( vs_string_strlen(g_ShareLibraryPath) == 0 ){
			return vs_getinstall_dir(Buf,Size);
        }else
            vs_string_snprintf(Buf,Size,"%s",g_ShareLibraryPath);
    }else
        vs_string_snprintf(Buf,Size,"%s",g_CoreLibraryPath);
    return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX )
    if( vs_string_strlen(g_CoreLibraryPath) == 0 ){
        if( vs_string_strlen(g_ShareLibraryPath) == 0 ){
#if defined(ENV_M64)
            VS_CHAR FindBuf[512];
            vs_string_snprintf(FindBuf,512,"/usr/lib64/libstarcore.so");
            if( vs_file_exist(FindBuf) == VS_TRUE )     //--64 bit
    			vs_string_snprintf(Buf,Size,"/usr/lib64");
            else
    			vs_string_snprintf(Buf,Size,"/usr/lib");
#else
			vs_string_snprintf(Buf,Size,"/usr/lib");
#endif			
			return VS_TRUE;
        }else
            vs_string_snprintf(Buf,Size,"%s",g_ShareLibraryPath);
    }else
        vs_string_snprintf(Buf,Size,"%s",g_CoreLibraryPath);
    return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    if( vs_string_strlen(g_CoreLibraryPath) == 0 ){
        if( vs_string_strlen(g_ShareLibraryPath) == 0 ){
#if defined(ENV_M64)
            VS_CHAR FindBuf[512];
            vs_string_snprintf(FindBuf,512,"/usr/local/lib64/libstarcore.dylib");
            if( vs_file_exist(FindBuf) == VS_TRUE )     //--64 bit
    			vs_string_snprintf(Buf,Size,"/usr/local/lib64");
            else
    			vs_string_snprintf(Buf,Size,"/usr/local/lib");
#else
			vs_string_snprintf(Buf,Size,"/usr/local/lib");
#endif			
			return VS_TRUE;
        }else
            vs_string_snprintf(Buf,Size,"%s",g_ShareLibraryPath);
    }else
        vs_string_snprintf(Buf,Size,"%s",g_CoreLibraryPath);
    return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
    if( vs_string_strlen(g_CoreLibraryPath) == 0 ){
        if( vs_string_strlen(g_ShareLibraryPath) == 0 )
            vs_string_snprintf(Buf,Size,"/data/data/com.srplab.starcore/lib");
        else
            vs_string_snprintf(Buf,Size,"%s",g_ShareLibraryPath);
    }else
        vs_string_snprintf(Buf,Size,"%s",g_CoreLibraryPath);
    return VS_TRUE;
#endif
}

class ClassOfVSStarCore : public ClassOfStarCore{
public:
    class ClassOfVSStarCore *Up,*Down;

public:
    VS_INT32 InitResult;
    VS_HWND hWnd;
    VS_BOOL ActiveFlag;

public:
    VS_HANDLE hVSDllInstance;
    class ClassOfSRPControlInterface *VSControlInterface;
    class ClassOfBasicSRPInterface *BasicSRPInterface;

    VSCore_RegisterCallBackInfoProc    VSRegisterCallBackInfo;
	VSCore_UnRegisterCallBackInfoProc  VSUnRegisterCallBackInfo;
	VSCore_HasInitProc                 VSHasInit;
    VSCore_InitProc                    VSInit;
    VSCore_TermExProc                  VSVSTermEx;
    VSCore_QueryControlInterfaceProc   VSQueryControlInterface;

    class ClassOfSRPInterface *SRPInterface;  //Valid after Create Service

    VS_CHAR DumCoreModuleFileName[512];
    VS_BOOL LoadCoreModule();
    void    UnLoadCoreModule();

public:
    //--server as client
    ClassOfVSStarCore(VS_STARCONFIGEX *ConfigEx);
    ClassOfVSStarCore(VS_BOOL SRPPrintFlag, VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara,VS_STARCONFIGEX *ConfigEx);
    //--server
    ClassOfVSStarCore(VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient,VS_STARCONFIGEX *ConfigEx);
	ClassOfVSStarCore(VS_BOOL SRPPrintFlag, VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient, VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara, VS_STARCONFIGEX *ConfigEx);
    ClassOfVSStarCore(VS_BOOL ExternInit);
    ~ClassOfVSStarCore();
    void SRPAPI Release();

    VS_INT32 SRPAPI GetInitResult();
    void SRPAPI SetWnd(VS_HWND In_hWnd);
    void SRPAPI SetWndActive( VS_BOOL In_ActiveFlag );

	class ClassOfSRPInterface *  SRPAPI CreateService(const VS_CHAR *ServiceName,VSImportServiceDef ImportService[]);
	class ClassOfSRPInterface *  SRPAPI CreateService1(const VS_CHAR *ServicePath,const VS_CHAR *ServiceName,VS_UUID *ServiceID,const VS_CHAR *RootPass,VSImportServiceDef ImportService[]);
	class ClassOfSRPInterface *  SRPAPI CreateService2(const VS_CHAR *ServicePath,const VS_CHAR *ServiceName,VS_UUID *ServiceID,const VS_CHAR *RootPass,VS_INT32 FrameInterval,VS_INT32 NetPkgSize,VS_INT32 UploadPkgSize,VS_INT32 DownloadPkgSize,VS_INT32 DataUpPkgSize,VS_INT32 DataDownPkgSize,VSImportServiceDef ImportService[]);

    void SRPAPI Flush(VS_BOOL WaitFlag);
    void SRPAPI MsgLoop(VS_SRPMsgLoopExitProc SRPMsgLoopExitProc);

    class ClassOfSRPControlInterface * SRPAPI GetControlInterface(){return VSControlInterface;};
    class ClassOfBasicSRPInterface * SRPAPI GetBasicInterface(){return BasicSRPInterface;};
};

static class ClassOfVSStarCore *VSStarCoreQueueRoot;
static VS_MUTEX VSStarCoreQueueMutex;
static VS_BOOL VSStarCore_InitFlag = VS_FALSE;

void SRPAPI StarCore_Init()
{
	if( VSStarCore_InitFlag == VS_TRUE )
		return;
	VSStarCore_InitFlag = VS_TRUE;
    VSStarCoreQueueRoot = NULL;
    vs_mutex_init(&VSStarCoreQueueMutex);
}

void SRPAPI StarCore_Term()
{
	if( VSStarCore_InitFlag == VS_FALSE )
		return;
	VSStarCore_InitFlag = VS_FALSE;
    VSStarCoreQueueRoot = NULL;
    vs_mutex_destory(&VSStarCoreQueueMutex);
}

//--server as client
class ClassOfStarCore *SRPAPI StarCore_Init(VS_STARCONFIGEX *ConfigEx)
{
    class ClassOfVSStarCore *VSStarCore;

    StarCore_Init();
    VSStarCore = new class ClassOfVSStarCore(ConfigEx);
    if( VSStarCore -> GetInitResult() != VSINIT_ERROR ){
        vs_mutex_lock(&VSStarCoreQueueMutex);
        if(VSStarCoreQueueRoot == NULL )
            VSStarCoreQueueRoot = VSStarCore;
        else{
            VSStarCore -> Down = VSStarCoreQueueRoot;
            VSStarCoreQueueRoot -> Up = VSStarCore;
            VSStarCoreQueueRoot = VSStarCore;
        }
        vs_mutex_unlock(&VSStarCoreQueueMutex);
        return  VSStarCore;
    }
    delete VSStarCore;
    return NULL;
}

class ClassOfStarCore *SRPAPI StarCore_Init(VS_BOOL SRPPrintFlag, VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara, VS_STARCONFIGEX *ConfigEx)
{
    class ClassOfVSStarCore *VSStarCore;

    StarCore_Init();
    VSStarCore = new class ClassOfVSStarCore(SRPPrintFlag,MsgCallBackProc,MsgCallBackPara,ConfigEx);
    if( VSStarCore -> GetInitResult() != VSINIT_ERROR ){
        vs_mutex_lock(&VSStarCoreQueueMutex);
        if(VSStarCoreQueueRoot == NULL )
            VSStarCoreQueueRoot = VSStarCore;
        else{
            VSStarCore -> Down = VSStarCoreQueueRoot;
            VSStarCoreQueueRoot -> Up = VSStarCore;
            VSStarCoreQueueRoot = VSStarCore;
        }
        vs_mutex_unlock(&VSStarCoreQueueMutex);
        return  VSStarCore;
    }
    delete VSStarCore;
    return NULL;
}

//--server
class ClassOfStarCore *SRPAPI StarCore_Init(const VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, const VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient,VS_STARCONFIGEX *ConfigEx)
{
    class ClassOfVSStarCore *VSStarCore;

    StarCore_Init();
	VSStarCore = new class ClassOfVSStarCore((VS_CHAR *)DebugInterface,PortNumberForDebug,(VS_CHAR *)ClientInterface,PortNumberForDirectClient,ConfigEx);
    if( VSStarCore -> GetInitResult() != VSINIT_ERROR ){
        vs_mutex_lock(&VSStarCoreQueueMutex);
        if(VSStarCoreQueueRoot == NULL )
            VSStarCoreQueueRoot = VSStarCore;
        else{
            VSStarCore -> Down = VSStarCoreQueueRoot;
            VSStarCoreQueueRoot -> Up = VSStarCore;
            VSStarCoreQueueRoot = VSStarCore;
        }
        vs_mutex_unlock(&VSStarCoreQueueMutex);
        return  VSStarCore;
    }
    delete VSStarCore;
    return NULL;
}

class ClassOfStarCore *SRPAPI StarCore_Init(VS_BOOL SRPPrintFlag, const VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, const VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient, VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara, VS_STARCONFIGEX *ConfigEx)
{
    class ClassOfVSStarCore *VSStarCore;

    StarCore_Init();
    VSStarCore = new class ClassOfVSStarCore(SRPPrintFlag,(VS_CHAR *)DebugInterface,PortNumberForDebug,(VS_CHAR *)ClientInterface,PortNumberForDirectClient,MsgCallBackProc,MsgCallBackPara,ConfigEx);
    if( VSStarCore -> GetInitResult() != VSINIT_ERROR ){
        vs_mutex_lock(&VSStarCoreQueueMutex);
        if(VSStarCoreQueueRoot == NULL )
            VSStarCoreQueueRoot = VSStarCore;
        else{
            VSStarCore -> Down = VSStarCoreQueueRoot;
            VSStarCoreQueueRoot -> Up = VSStarCore;
            VSStarCoreQueueRoot = VSStarCore;
        }
        vs_mutex_unlock(&VSStarCoreQueueMutex);
        return  VSStarCore;
    }
    delete VSStarCore;
    return NULL;
}

class ClassOfStarCore *SRPAPI StarCore_Init( class ClassOfSRPControlInterface *In_VSControlInterface,class ClassOfBasicSRPInterface *In_BasicSRPInterface)
{
    class ClassOfVSStarCore *VSStarCore;

	StarCore_Init();
    VSStarCore = new class ClassOfVSStarCore(VS_TRUE);
    VSStarCore -> VSControlInterface = In_VSControlInterface;
    VSStarCore -> BasicSRPInterface = In_BasicSRPInterface;
    return VSStarCore;
}

static VS_UWORD ClassOfStarCore_MsgCallBack(VS_ULONG ServiceGroupID,VS_ULONG uMsg, VS_UWORD wParam, VS_UWORD lParam, VS_BOOL *IsProcessed, VS_UWORD Para )
{
    class ClassOfVSStarCore *starcore;

    starcore = (class ClassOfVSStarCore *)Para;
    if( starcore -> hWnd == NULL )
    	return 0;
    switch( uMsg ){
	case MSG_GETWNDHANDLE :
   		((VS_HWND *)wParam)[0] = (VS_HWND)starcore -> hWnd;
		(*IsProcessed) = true;
		break;
	case MSG_SETFOCUS :
		break;
    case MSG_ISAPPACTIVE :
        (*IsProcessed) = true;
        if( starcore -> ActiveFlag == VS_TRUE )
            return VS_TRUE;
        return VS_FALSE;
    }
    return 0;
}

#define Lib_SetUUIDString_ToChar(X) ( ( (VS_UINT8)X <= (VS_UINT8)9 ) ? X + '0' : X - 10 + 'a'  )

static void Lib_SetUUIDString(VS_UUID *ObjectUUID,VS_INT8 *Buf)
{
    VS_ULONG *DwordPtr;
    VS_UINT16  *WordPtr;
    VS_UINT8 *CharPtr,*OutBuf;

    DwordPtr = (VS_ULONG *)ObjectUUID;
    WordPtr = (VS_UINT16 *)ObjectUUID;
    CharPtr = (VS_UINT8 *)ObjectUUID;
    OutBuf = (VS_UINT8 *)Buf;

    OutBuf[0] = (VS_UINT8)Lib_SetUUIDString_ToChar( (DwordPtr[0] >> 28 & 0x0000000F) );
    OutBuf[1] = (VS_UINT8)Lib_SetUUIDString_ToChar( (DwordPtr[0] >> 24 & 0x0000000F) );
    OutBuf[2] = (VS_UINT8)Lib_SetUUIDString_ToChar( (DwordPtr[0] >> 20 & 0x0000000F) );
    OutBuf[3] = (VS_UINT8)Lib_SetUUIDString_ToChar( (DwordPtr[0] >> 16 & 0x0000000F) );
    OutBuf[4] = (VS_UINT8)Lib_SetUUIDString_ToChar( (DwordPtr[0] >> 12 & 0x0000000F) );
    OutBuf[5] = (VS_UINT8)Lib_SetUUIDString_ToChar( (DwordPtr[0] >> 8 & 0x0000000F) );
    OutBuf[6] = (VS_UINT8)Lib_SetUUIDString_ToChar( (DwordPtr[0] >> 4 & 0x0000000F) );
    OutBuf[7] = (VS_UINT8)Lib_SetUUIDString_ToChar( (DwordPtr[0] & 0x0000000F) );

    OutBuf[8] = '-';

    OutBuf[9] = (VS_UINT8)Lib_SetUUIDString_ToChar( (WordPtr[2] >> 12 & 0x000F) );
    OutBuf[10] = (VS_UINT8)Lib_SetUUIDString_ToChar( (WordPtr[2] >> 8 & 0x000F) );
    OutBuf[11] = (VS_UINT8)Lib_SetUUIDString_ToChar( (WordPtr[2] >> 4 & 0x000F) );
    OutBuf[12] = (VS_UINT8)Lib_SetUUIDString_ToChar( (WordPtr[2] & 0x000F) );

    OutBuf[13] = '-';

    OutBuf[14] = (VS_UINT8)Lib_SetUUIDString_ToChar( (WordPtr[3] >> 12 & 0x000F) );
    OutBuf[15] = (VS_UINT8)Lib_SetUUIDString_ToChar( (WordPtr[3] >> 8 & 0x000F) );
    OutBuf[16] = (VS_UINT8)Lib_SetUUIDString_ToChar( (WordPtr[3] >> 4 & 0x000F) );
    OutBuf[17] = (VS_UINT8)Lib_SetUUIDString_ToChar( (WordPtr[3] & 0x000F) );

    OutBuf[18] = '-';

    OutBuf[19] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[8] >> 4 & 0x0F) );
    OutBuf[20] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[8] & 0x0F) );
    OutBuf[21] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[9] >> 4 & 0x0F) );
    OutBuf[22] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[9] & 0x0F) );

    OutBuf[23] = '-';
    OutBuf[24] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[10] >> 4 & 0x0F) );
    OutBuf[25] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[10] & 0x0F) );
    OutBuf[26] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[11] >> 4 & 0x0F) );
    OutBuf[27] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[11] & 0x0F) );
    OutBuf[28] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[12] >> 4 & 0x0F) );
    OutBuf[29] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[12] & 0x0F) );
    OutBuf[30] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[13] >> 4 & 0x0F) );
    OutBuf[31] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[13] & 0x0F) );
    OutBuf[32] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[14] >> 4 & 0x0F) );
    OutBuf[33] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[14] & 0x0F) );
    OutBuf[34] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[15] >> 4 & 0x0F) );
    OutBuf[35] = (VS_UINT8)Lib_SetUUIDString_ToChar( (CharPtr[15] & 0x0F) );
    OutBuf[36] = 0;
//  sprintf(Buf,"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",DwordPtr[0],WordPtr[2],WordPtr[3],CharPtr[8],CharPtr[9],CharPtr[10],CharPtr[11],CharPtr[12],CharPtr[13],CharPtr[14],CharPtr[15]);
}

#define Lib_GetUUIDFromString_ToByte(X,Y_Buf,Z_Dword,I_Word,J_Byte) { \
    if( (*Y_Buf) >= '0' && (*Y_Buf) <= '9' )                      \
        X = X | ((*Y_Buf) - '0' );                                \
    else if( (*Y_Buf) >= 'a' && (*Y_Buf) <= 'f' )                 \
        X = X | ((*Y_Buf) - 'a' + 10 );                                \
    else if( (*Y_Buf) >= 'A' && (*Y_Buf) <= 'F' )                 \
        X = X | ((*Y_Buf) - 'A' + 10 );                                \
    else{                                                         \
        Z_Dword[0] = 0;                                           \
        I_Word[2] = 0;                                            \
        I_Word[3] = 0;                                            \
        J_Byte[8] = 0;                                            \
        J_Byte[9] = 0;                                            \
        J_Byte[10] = 0;                                           \
        J_Byte[11] = 0;                                           \
        J_Byte[12] = 0;                                           \
        J_Byte[13] = 0;                                           \
        J_Byte[14] = 0;                                           \
        J_Byte[15] = 0;                                           \
        return VS_FALSE;                                                   \
    }                                                             \
    }

#define Lib_GetUUIDFromString_Error(Z_Dword,I_Word,J_Byte) { \
        Z_Dword[0] = 0;                                           \
        I_Word[2] = 0;                                            \
        I_Word[3] = 0;                                            \
        J_Byte[8] = 0;                                            \
        J_Byte[9] = 0;                                            \
        J_Byte[10] = 0;                                           \
        J_Byte[11] = 0;                                           \
        J_Byte[12] = 0;                                           \
        J_Byte[13] = 0;                                           \
        J_Byte[14] = 0;                                           \
        J_Byte[15] = 0;                                           \
        return VS_FALSE;                                                   \
        }

VS_BOOL Lib_GetUUIDFromString(VS_UUID *ObjectUUID,VS_INT8 *Buf)
{
    VS_ULONG *DwordPtr;
    VS_UINT16  *WordPtr;
    VS_UINT8 *CharPtr;

    if( ObjectUUID != NULL )
        vs_memset(ObjectUUID,0,sizeof(VS_UUID));
    DwordPtr = (VS_ULONG *)ObjectUUID;
    WordPtr = (VS_UINT16 *)ObjectUUID;
    CharPtr = (VS_UINT8 *)ObjectUUID;
    if( Buf == NULL ){
        Lib_GetUUIDFromString_Error( DwordPtr, WordPtr,  CharPtr )
    }

    DwordPtr[0] = 0;
    Lib_GetUUIDFromString_ToByte( DwordPtr[0], Buf, DwordPtr, WordPtr,  CharPtr )
    DwordPtr[0] = DwordPtr[0] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( DwordPtr[0], Buf, DwordPtr, WordPtr,  CharPtr )
    DwordPtr[0] = DwordPtr[0] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( DwordPtr[0], Buf, DwordPtr, WordPtr,  CharPtr )
    DwordPtr[0] = DwordPtr[0] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( DwordPtr[0], Buf, DwordPtr, WordPtr,  CharPtr )
    DwordPtr[0] = DwordPtr[0] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( DwordPtr[0], Buf, DwordPtr, WordPtr,  CharPtr )
    DwordPtr[0] = DwordPtr[0] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( DwordPtr[0], Buf, DwordPtr, WordPtr,  CharPtr )
    DwordPtr[0] = DwordPtr[0] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( DwordPtr[0], Buf, DwordPtr, WordPtr,  CharPtr )
    DwordPtr[0] = DwordPtr[0] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( DwordPtr[0], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    if( (*Buf) != '-' ){
        Lib_GetUUIDFromString_Error( DwordPtr, WordPtr,  CharPtr )
    }
    Buf ++;

    WordPtr[2] = 0;
    Lib_GetUUIDFromString_ToByte( WordPtr[2], Buf, DwordPtr, WordPtr,  CharPtr )
    WordPtr[2] = WordPtr[2] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( WordPtr[2], Buf, DwordPtr, WordPtr,  CharPtr )
    WordPtr[2] = WordPtr[2] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( WordPtr[2], Buf, DwordPtr, WordPtr,  CharPtr )
    WordPtr[2] = WordPtr[2] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( WordPtr[2], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    if( (*Buf) != '-' ){
        Lib_GetUUIDFromString_Error( DwordPtr, WordPtr,  CharPtr )
    }
    Buf ++;

    WordPtr[3] = 0;
    Lib_GetUUIDFromString_ToByte( WordPtr[3], Buf, DwordPtr, WordPtr,  CharPtr )
    WordPtr[3] = WordPtr[3] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( WordPtr[3], Buf, DwordPtr, WordPtr,  CharPtr )
    WordPtr[3] = WordPtr[3] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( WordPtr[3], Buf, DwordPtr, WordPtr,  CharPtr )
    WordPtr[3] = WordPtr[3] << 4;
    Buf ++;
    Lib_GetUUIDFromString_ToByte( WordPtr[3], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;

    if( (*Buf) != '-' ){
        Lib_GetUUIDFromString_Error( DwordPtr, WordPtr,  CharPtr )
    }
    Buf ++;

    DwordPtr[2] = 0;
    DwordPtr[3] = 0;
    Lib_GetUUIDFromString_ToByte( CharPtr[8], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    CharPtr[8] = CharPtr[8] << 4;
    Lib_GetUUIDFromString_ToByte( CharPtr[8], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;

    Lib_GetUUIDFromString_ToByte( CharPtr[9], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    CharPtr[9] = CharPtr[9] << 4;
    Lib_GetUUIDFromString_ToByte( CharPtr[9], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    if( (*Buf) != '-' ){
        Lib_GetUUIDFromString_Error( DwordPtr, WordPtr,  CharPtr )
    }
    Buf ++;
    Lib_GetUUIDFromString_ToByte( CharPtr[10], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    CharPtr[10] = CharPtr[10] << 4;
    Lib_GetUUIDFromString_ToByte( CharPtr[10], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    Lib_GetUUIDFromString_ToByte( CharPtr[11], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    CharPtr[11] = CharPtr[11] << 4;
    Lib_GetUUIDFromString_ToByte( CharPtr[11], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    Lib_GetUUIDFromString_ToByte( CharPtr[12], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    CharPtr[12] = CharPtr[12] << 4;
    Lib_GetUUIDFromString_ToByte( CharPtr[12], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    Lib_GetUUIDFromString_ToByte( CharPtr[13], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    CharPtr[13] = CharPtr[13] << 4;
    Lib_GetUUIDFromString_ToByte( CharPtr[13], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    Lib_GetUUIDFromString_ToByte( CharPtr[14], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    CharPtr[14] = CharPtr[14] << 4;
    Lib_GetUUIDFromString_ToByte( CharPtr[14], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    Lib_GetUUIDFromString_ToByte( CharPtr[15], Buf, DwordPtr, WordPtr,  CharPtr )
    Buf ++;
    CharPtr[15] = CharPtr[15] << 4;
    Lib_GetUUIDFromString_ToByte( CharPtr[15], Buf, DwordPtr, WordPtr,  CharPtr )
 
    return VS_TRUE;
}

void starlib_uuidtostring(VS_UUID *ObjectUUID,VS_INT8 *Buf)
{
    Lib_SetUUIDString(ObjectUUID,Buf);
}
VS_BOOL starlib_stringtouuid(const VS_INT8 *Buf,VS_UUID *ObjectUUID)
{
	return Lib_GetUUIDFromString(ObjectUUID,(VS_INT8 *)Buf);
}

VS_UUID *starlib_stringtouuidptr(const VS_INT8 *Buf)
{
    static VS_UUID LocalBuf;
	if( Lib_GetUUIDFromString(&LocalBuf,(VS_INT8 *)Buf) == VS_FALSE )
        return NULL;
    return &LocalBuf;
}

VS_BOOL ClassOfVSStarCore::LoadCoreModule()
{
	VS_CHAR ModuleName[512];
    VS_CHAR ModuleName1[512];
    VS_INT8 VSUuidStringBuf[64];
    VS_UUID LocalDllID;
    VS_CHAR *ExistTempFile;
    VS_CHAR EnvModuleName[128];

    DumCoreModuleFileName[0] = 0;
    //-----------------------
	if( vs_get_env("SRPMODULE",EnvModuleName,128) == VS_FALSE )
		sprintf(EnvModuleName,"libstarcore");
    vs_dir_getcorepath( ModuleName, 512 );
	sprintf(ModuleName1,"\\%s%s",EnvModuleName,VS_MODULEEXT);
    strcat(ModuleName,ModuleName1);

    if( vs_dll_get( ModuleName ) != NULL  ){
        //---has load
        ExistTempFile = NULL;
        if( VSStarCoreQueueRoot != NULL )
            ExistTempFile = VSStarCoreQueueRoot -> GetBasicInterface() -> GetRegTempFile(ModuleName,ModuleName1,512);
        if( ExistTempFile == NULL ){ //not exist
            vs_uuid_create( &LocalDllID );
            Lib_SetUUIDString( &LocalDllID, VSUuidStringBuf );
            GetVirtualSocietyUserPath(VS_SERVER_SERVER,DumCoreModuleFileName,512,VS_TRUE);
            sprintf(ModuleName1,"\\%s.so",VSUuidStringBuf);
            if( vs_file_exist( ModuleName ) == VS_FALSE )
                return VS_FALSE;
            strcat(DumCoreModuleFileName,ModuleName1);
            vs_file_copy( ModuleName, DumCoreModuleFileName );
            VSStarCoreQueueRoot -> GetBasicInterface() -> RegTempFile(DumCoreModuleFileName,ModuleName);
            strcpy( ModuleName, DumCoreModuleFileName );
        }else{
            strcpy(DumCoreModuleFileName,ModuleName1);
            strcpy( ModuleName, DumCoreModuleFileName );
        }
    }
    hVSDllInstance = vs_dll_open( ModuleName );
    if( hVSDllInstance == NULL ){
        if( DumCoreModuleFileName[0] != 0 )
            VSStarCoreQueueRoot -> GetBasicInterface() -> UnRegTempFile(DumCoreModuleFileName);
	    return VS_FALSE;
    }
    VSRegisterCallBackInfo = (VSCore_RegisterCallBackInfoProc)vs_dll_sym( hVSDllInstance, VSCORE_REGISTERCALLBACKINFO_NAME );
	VSUnRegisterCallBackInfo = (VSCore_UnRegisterCallBackInfoProc)vs_dll_sym( hVSDllInstance, VSCORE_UNREGISTERCALLBACKINFO_NAME );
	VSHasInit = (VSCore_HasInitProc)vs_dll_sym( hVSDllInstance, VSCORE_HASINIT_NAME );
    VSInit = (VSCore_InitProc)vs_dll_sym( hVSDllInstance, VSCORE_INIT_NAME );
    VSVSTermEx = (VSCore_TermExProc)vs_dll_sym( hVSDllInstance, VSCORE_TERMEX_NAME );
    VSQueryControlInterface = (VSCore_QueryControlInterfaceProc)vs_dll_sym( hVSDllInstance, VSCORE_QUERYCONTROLINTERFACE_NAME );
    return VS_TRUE;
}

void ClassOfVSStarCore::UnLoadCoreModule()
{

    if( hVSDllInstance == NULL )
        return;
    vs_dll_close( hVSDllInstance );
    DumCoreModuleFileName[0] = 0;
    hVSDllInstance = NULL;
    return;
}

ClassOfVSStarCore::ClassOfVSStarCore(VS_STARCONFIGEX *ConfigEx)
{
    InitResult = VSINIT_ERROR;
    hWnd = NULL;
    ActiveFlag = VS_FALSE;

    hVSDllInstance = NULL;
    VSControlInterface = NULL;
    BasicSRPInterface = NULL;

    VSRegisterCallBackInfo = NULL;
    VSInit = NULL;
    VSVSTermEx = NULL;
    VSQueryControlInterface = NULL;
    SRPInterface = NULL;

    Up = NULL;
    Down = NULL;
    DumCoreModuleFileName[0] = 0;

    vs_mutex_lock(&VSStarCoreQueueMutex);
    if( LoadCoreModule() == VS_FALSE ){
        vs_mutex_unlock(&VSStarCoreQueueMutex);
        return;
    }
    VSRegisterCallBackInfo( ClassOfStarCore_MsgCallBack, (VS_UWORD)this );

    InitResult = VSInit( VS_FALSE, VS_FALSE, "", 0, "", 0, ConfigEx);
    if( InitResult == VSINIT_ERROR ){
        if( DumCoreModuleFileName[0] != 0 )
            VSStarCoreQueueRoot -> GetBasicInterface() -> UnRegTempFile(DumCoreModuleFileName);
        DumCoreModuleFileName[0] = 0;
        UnLoadCoreModule( );
        vs_mutex_unlock(&VSStarCoreQueueMutex);
        return;
    }
    vs_mutex_unlock(&VSStarCoreQueueMutex);
    VSControlInterface = VSQueryControlInterface();
    BasicSRPInterface = VSControlInterface -> QueryBasicInterface(0);
    return;
}

ClassOfVSStarCore::ClassOfVSStarCore(VS_BOOL SRPPrintFlag, VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara, VS_STARCONFIGEX *ConfigEx)
{
    InitResult = VSINIT_ERROR;
    hVSDllInstance = NULL;
    VSControlInterface = NULL;
    BasicSRPInterface = NULL;

    VSRegisterCallBackInfo = NULL;
    VSInit = NULL;
    VSVSTermEx = NULL;
    VSQueryControlInterface = NULL;
    SRPInterface = NULL;

    Up = NULL;
    Down = NULL;
    DumCoreModuleFileName[0] = 0;

    //-----------------------
    vs_mutex_lock(&VSStarCoreQueueMutex);
    if( LoadCoreModule() == VS_FALSE ){
        vs_mutex_unlock(&VSStarCoreQueueMutex);
        return;
    }
    VSRegisterCallBackInfo( ClassOfStarCore_MsgCallBack, (VS_UWORD)this );
    if( MsgCallBackProc != NULL )
        VSRegisterCallBackInfo( MsgCallBackProc, MsgCallBackPara );
    InitResult = VSInit( VS_FALSE, SRPPrintFlag, "", 0, "", 0, ConfigEx);
    if( InitResult == VSINIT_ERROR ){
        if( DumCoreModuleFileName[0] != 0 )
            VSStarCoreQueueRoot -> GetBasicInterface() -> UnRegTempFile(DumCoreModuleFileName);
        DumCoreModuleFileName[0] = 0;
        UnLoadCoreModule( );
        vs_mutex_unlock(&VSStarCoreQueueMutex);
        return;
    }
    vs_mutex_unlock(&VSStarCoreQueueMutex);
    VSControlInterface = VSQueryControlInterface();
    BasicSRPInterface = VSControlInterface -> QueryBasicInterface(0);
    return;
}

ClassOfVSStarCore::ClassOfVSStarCore(VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient, VS_STARCONFIGEX *ConfigEx)
{
    InitResult = VSINIT_ERROR;
    hVSDllInstance = NULL;
    VSControlInterface = NULL;
    BasicSRPInterface = NULL;

    VSRegisterCallBackInfo = NULL;
    VSInit = NULL;
    VSVSTermEx = NULL;
    VSQueryControlInterface = NULL;

    SRPInterface = NULL;

    Up = NULL;
    Down = NULL;
    DumCoreModuleFileName[0] = 0;

    //-----------------------
    vs_mutex_lock(&VSStarCoreQueueMutex);
    if( LoadCoreModule() == VS_FALSE ){
        vs_mutex_unlock(&VSStarCoreQueueMutex);
        return;
    }
    VSRegisterCallBackInfo( ClassOfStarCore_MsgCallBack, (VS_UWORD)this );
    InitResult = VSInit( VS_TRUE, VS_FALSE, DebugInterface, PortNumberForDebug, ClientInterface, PortNumberForDirectClient, ConfigEx);
    if( InitResult == VSINIT_ERROR ){
        if( DumCoreModuleFileName[0] != 0 )
            VSStarCoreQueueRoot -> GetBasicInterface() -> UnRegTempFile(DumCoreModuleFileName);
        DumCoreModuleFileName[0] = 0;
        vs_mutex_unlock(&VSStarCoreQueueMutex);
        return;
    }
    VSControlInterface = VSQueryControlInterface();
    BasicSRPInterface = VSControlInterface -> QueryBasicInterface(0);
    vs_mutex_unlock(&VSStarCoreQueueMutex);
    return;
}

ClassOfVSStarCore::ClassOfVSStarCore(VS_BOOL SRPPrintFlag, VS_CHAR *DebugInterface, VS_UINT16 PortNumberForDebug, VS_CHAR *ClientInterface, VS_UINT16 PortNumberForDirectClient, VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara, VS_STARCONFIGEX *ConfigEx)
{
    InitResult = VSINIT_ERROR;
    hVSDllInstance = NULL;
    VSControlInterface = NULL;
    BasicSRPInterface = NULL;

    VSRegisterCallBackInfo = NULL;
    VSInit = NULL;
    VSVSTermEx = NULL;
    VSQueryControlInterface = NULL;

    SRPInterface = NULL;

    Up = NULL;
    Down = NULL;
    DumCoreModuleFileName[0] = 0;

    //-----------------------
    vs_mutex_lock(&VSStarCoreQueueMutex);
    if( LoadCoreModule() == VS_FALSE ){
        vs_mutex_unlock(&VSStarCoreQueueMutex);
        return;
    }
    VSRegisterCallBackInfo( ClassOfStarCore_MsgCallBack, (VS_UWORD)this );
    if( MsgCallBackProc != NULL )
        VSRegisterCallBackInfo( MsgCallBackProc, MsgCallBackPara );

    InitResult = VSInit( VS_TRUE, SRPPrintFlag, DebugInterface, PortNumberForDebug, ClientInterface, PortNumberForDirectClient, ConfigEx);
    if( InitResult == VSINIT_ERROR ){
        if( DumCoreModuleFileName[0] != 0 )
            VSStarCoreQueueRoot -> GetBasicInterface() -> UnRegTempFile(DumCoreModuleFileName);
        DumCoreModuleFileName[0] = 0;
        vs_mutex_unlock(&VSStarCoreQueueMutex);
        return;
    }
    VSControlInterface = VSQueryControlInterface();
    BasicSRPInterface = VSControlInterface -> QueryBasicInterface(0);
    vs_mutex_unlock(&VSStarCoreQueueMutex);
    return;
}

ClassOfVSStarCore::ClassOfVSStarCore(VS_BOOL ExternInit)
{
    InitResult = VSINIT_OK;
    hVSDllInstance = NULL;
    VSControlInterface = NULL;
    BasicSRPInterface = NULL;

    VSRegisterCallBackInfo = NULL;
    VSInit = NULL;
    VSVSTermEx = NULL;
    VSQueryControlInterface = NULL;

    SRPInterface = NULL;
    Up = NULL;
    Down = NULL;
    DumCoreModuleFileName[0] = 0;

    vs_mutex_lock(&VSStarCoreQueueMutex);
    if(VSStarCoreQueueRoot == NULL )
        VSStarCoreQueueRoot = this;
    else{
        this -> Down = VSStarCoreQueueRoot;
        VSStarCoreQueueRoot -> Up = this;
        VSStarCoreQueueRoot = this;
    }
    vs_mutex_unlock(&VSStarCoreQueueMutex);
}

void ClassOfVSStarCore::Release()
{
    vs_mutex_lock(&VSStarCoreQueueMutex);
    if( Up == NULL )
        VSStarCoreQueueRoot = Down;
    else
        Up -> Down = Down;
    if( Down != NULL )
        Down -> Up = Up;
    delete this;
    vs_mutex_unlock(&VSStarCoreQueueMutex);
}

ClassOfVSStarCore::~ClassOfVSStarCore()
{
    if( InitResult == VSINIT_ERROR )
        return;
    if( hVSDllInstance == NULL )
        return;
	VSUnRegisterCallBackInfo( ClassOfStarCore_MsgCallBack, (VS_UWORD)this );
    if( DumCoreModuleFileName[0] != 0 )
        VSControlInterface -> UnRegTempFile(DumCoreModuleFileName);
    if( BasicSRPInterface != NULL )
        BasicSRPInterface -> Release();
    if( VSControlInterface != NULL )
        VSControlInterface -> Release();
    VSVSTermEx();
    UnLoadCoreModule( );
}

VS_INT32 ClassOfVSStarCore::GetInitResult()
{
    return InitResult;
}

void ClassOfVSStarCore::SetWnd(VS_HWND In_hWnd)
{
    hWnd = In_hWnd;
}

void ClassOfVSStarCore::SetWndActive( VS_BOOL In_ActiveFlag )
{
    ActiveFlag = In_ActiveFlag;
}

class ClassOfSRPInterface * ClassOfVSStarCore::CreateService(const VS_CHAR *ServiceName,VSImportServiceDef ImportService[])
{
    VSImportServiceDef *ImportServicePtr;
    VS_UUID ServiceID;
    
    if( BasicSRPInterface == NULL )
        return NULL;
    if( ImportService != NULL ){
        ImportServicePtr = &ImportService[0];
        while( ImportServicePtr -> ServiceName != NULL && ImportServicePtr -> ServiceName[0] != 0 ){
            if( BasicSRPInterface -> ImportService( (VS_CHAR *)ImportServicePtr -> ServiceName, VS_TRUE ) == VS_FALSE )
                return NULL;
            ImportServicePtr ++;
        }
    }
    INIT_UUID( ServiceID );
    if( BasicSRPInterface -> CreateService( "",ServiceName,&ServiceID,"123",0,0,0,0,0,0 ) == VS_FALSE )
        return NULL;
    SRPInterface = BasicSRPInterface ->GetSRPInterface(ServiceName,"root","123");
    if( SRPInterface == NULL )
        return NULL;
    return SRPInterface;
}

class ClassOfSRPInterface * ClassOfVSStarCore::CreateService1(const VS_CHAR *ServicePath,const VS_CHAR *ServiceName,VS_UUID *ServiceID,const VS_CHAR *RootPass,VSImportServiceDef ImportService[])
{
    VSImportServiceDef *ImportServicePtr;

    if( BasicSRPInterface == NULL )
        return NULL;
    if( ImportService != NULL ){
        ImportServicePtr = &ImportService[0];
        while( ImportServicePtr -> ServiceName != NULL && ImportServicePtr -> ServiceName[0] != 0 ){
            if( BasicSRPInterface -> ImportService( (VS_CHAR *)ImportServicePtr -> ServiceName, VS_TRUE ) == VS_FALSE )
                return NULL;
            ImportServicePtr ++;
        }
    }
	if( BasicSRPInterface -> CreateService( ServicePath,ServiceName,ServiceID,RootPass,0,0,0,0,0,0 ) == VS_FALSE )
        return NULL;
    SRPInterface = BasicSRPInterface ->GetSRPInterface(ServiceName,"root",RootPass);
    if( SRPInterface == NULL )
        return NULL;
    return SRPInterface;
}

class ClassOfSRPInterface * ClassOfVSStarCore::CreateService2(const VS_CHAR *ServicePath,const VS_CHAR *ServiceName,VS_UUID *ServiceID,const VS_CHAR *RootPass,VS_INT32 FrameInterval,VS_INT32 NetPkgSize,VS_INT32 UploadPkgSize,VS_INT32 DownloadPkgSize,VS_INT32 DataUpPkgSize,VS_INT32 DataDownPkgSize,VSImportServiceDef ImportService[])
{
    VSImportServiceDef *ImportServicePtr;

    if( BasicSRPInterface == NULL )
        return NULL;
    if( ImportService != NULL ){
        ImportServicePtr = &ImportService[0];
        while( ImportServicePtr -> ServiceName != NULL && ImportServicePtr -> ServiceName[0] != 0 ){
            if( BasicSRPInterface -> ImportService( (VS_CHAR *)ImportServicePtr -> ServiceName, VS_TRUE ) == VS_FALSE )
                return NULL;
            ImportServicePtr ++;
        }
    }
    if( BasicSRPInterface -> CreateService( ServicePath,ServiceName,ServiceID,RootPass,FrameInterval,NetPkgSize,UploadPkgSize,DownloadPkgSize,DataUpPkgSize,DataDownPkgSize ) == VS_FALSE )
        return NULL;
    SRPInterface = BasicSRPInterface ->GetSRPInterface(ServiceName,"root",RootPass);
    if( SRPInterface == NULL )
        return NULL;
    return SRPInterface;
}

void ClassOfVSStarCore::Flush(VS_BOOL WaitFlag)
{
    if( InitResult == VSINIT_ERROR )
        return;
    while( VSControlInterface -> SRPDispatch(VS_FALSE) == VS_TRUE ){
    }
    //if( VSControlInterface -> SRPDispatch(VS_FALSE) == VS_FALSE )
    VSControlInterface -> SRPIdle();
    if( WaitFlag == VS_TRUE )
        VSControlInterface -> SRPDispatch(VS_TRUE);
}

void ClassOfVSStarCore::MsgLoop(VS_SRPMsgLoopExitProc SRPMsgLoopExitProc)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	VS_BOOL QuitFlag;
#endif

    if( InitResult == VSINIT_ERROR )
        return;
	while(SRPMsgLoopExitProc() == VS_FALSE)
	{
        if( BasicSRPInterface -> QueryActiveService(NULL) == NULL )
            break;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
        if( VSControlInterface -> WinMsgLoop(&QuitFlag) == VS_TRUE ){
			if(QuitFlag == VS_TRUE)
				break;
		}else{
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
        {
#endif                
			if( VSControlInterface -> IsAppActive() == VS_TRUE ){
				if( VSControlInterface -> SRPDispatch(false) == false )
					VSControlInterface -> SRPIdle();
			}else{
				VSControlInterface -> SRPDispatch(true);
				VSControlInterface -> SRPIdle();
			}
        }
    }
}

	class ClassOfSRPInterface *VSCore_InitSimple(VS_CORESIMPLECONTEXT *Context, const VS_CHAR *ServiceName, const VS_CHAR *RootPass, VS_UINT16 ClientPort, VS_UINT16 WebPort, VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara, ...)
{
	VS_CHAR ModuleName[512],EnvModuleName[128];
    va_list tArgList;
    VS_CHAR *CharPtr;

    if( Context == NULL || ServiceName == NULL || RootPass == NULL )
        return NULL;
	if( vs_get_env("SRPMODULE",EnvModuleName,128) == VS_FALSE )
		sprintf(EnvModuleName,"libstarcore");
    if( vs_string_strlen(g_CoreLibraryPath) == 0 ){
        if( vs_string_strlen(g_ShareLibraryPath) == 0 )
        	sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
        else
            sprintf(ModuleName,"%s/%s%s",g_ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
    }else{
        sprintf(ModuleName,"%s/%s%s",g_CoreLibraryPath,EnvModuleName,VS_MODULEEXT);
        if( vs_file_exist( ModuleName ) == VS_FALSE ){
            if( vs_string_strlen(g_ShareLibraryPath) == 0 )
            	sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
            else
                sprintf(ModuleName,"%s/%s%s",g_ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
        }
    }
    memset(Context,0,sizeof(VS_CORESIMPLECONTEXT));
    Context -> hVSDllInstance = vs_dll_open( ModuleName );
	if( Context -> hVSDllInstance == NULL )
		return NULL;
    Context -> VSRegisterCallBackInfo = (VSCore_RegisterCallBackInfoProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_REGISTERCALLBACKINFO_NAME );
    Context -> VSInit = (VSCore_InitProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_INIT_NAME );
    Context -> VSTermEx = (VSCore_TermExProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_TERMEX_NAME );
    Context -> VSQueryControlInterface = (VSCore_QueryControlInterfaceProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_QUERYCONTROLINTERFACE_NAME );
    if( Context -> VSRegisterCallBackInfo == NULL || Context -> VSInit == NULL || Context -> VSTermEx == NULL || Context -> VSQueryControlInterface == NULL )
        return NULL;
    if( Context -> VSInit( VS_TRUE, VS_TRUE, "", 0, "", ClientPort,NULL) == VSINIT_ERROR ){
		vs_dll_close(Context -> hVSDllInstance);
        Context -> hVSDllInstance = NULL;
        return NULL;
    }
    if( MsgCallBackProc != NULL )
        Context -> VSRegisterCallBackInfo( MsgCallBackProc, MsgCallBackPara );
    Context -> VSControlInterface = Context -> VSQueryControlInterface();
    Context -> VSBasicSRPInterface = Context -> VSControlInterface -> QueryBasicInterface(0);
    //--import service
    va_start(tArgList, MsgCallBackPara);
    CharPtr = va_arg( tArgList, VS_CHAR * );
    while( CharPtr != NULL ){
        if( Context -> VSBasicSRPInterface -> ImportService(CharPtr,VS_TRUE) == VS_FALSE ){
            Context -> VSControlInterface -> Release();
            Context -> VSBasicSRPInterface -> Release();
            Context -> VSTermEx();
    		vs_dll_close(Context -> hVSDllInstance);
            Context -> hVSDllInstance = NULL;
            return NULL;
        }
        CharPtr = va_arg( tArgList, VS_CHAR * );
    }
    va_end(tArgList);
    //--Create Service
    if( Context -> VSBasicSRPInterface -> CreateService( "",ServiceName,NULL,RootPass,5,10240,10240,10240,10240,10240 ) == VS_FALSE ){
        Context -> VSControlInterface -> Release();
        Context -> VSBasicSRPInterface -> Release();
        Context -> VSTermEx();
 		vs_dll_close(Context -> hVSDllInstance);
        Context -> hVSDllInstance = NULL;
        return NULL;
    }
    if( WebPort != 0 )
        Context -> VSBasicSRPInterface ->SetWebServerPort("",WebPort,100,2048);  //2M字节
    return Context -> VSBasicSRPInterface ->GetSRPInterface(ServiceName,"root",RootPass);
}

	class ClassOfSRPInterface *VSCore_InitSimple1(VS_CORESIMPLECONTEXT *Context, const VS_CHAR *ServiceName, VS_UUID *ServiceID, const VS_CHAR *RootPass, VS_UINT16 ClientPort, VS_UINT16 WebPort, VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara, ...)
{
	VS_CHAR ModuleName[512],EnvModuleName[128];
    va_list tArgList;
    VS_CHAR *CharPtr;

    if( Context == NULL || ServiceName == NULL || RootPass == NULL )
        return NULL;
	if( vs_get_env("SRPMODULE",EnvModuleName,128) == VS_FALSE )
		sprintf(EnvModuleName,"libstarcore");
    if( vs_string_strlen(g_CoreLibraryPath) == 0 ){
        if( vs_string_strlen(g_ShareLibraryPath) == 0 )
        	sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
        else
            sprintf(ModuleName,"%s/%s%s",g_ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
    }else{
        sprintf(ModuleName,"%s/%s%s",g_CoreLibraryPath,EnvModuleName,VS_MODULEEXT);
        if( vs_file_exist( ModuleName ) == VS_FALSE ){
            if( vs_string_strlen(g_ShareLibraryPath) == 0 )
            	sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
            else
                sprintf(ModuleName,"%s/%s%s",g_ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
        }
    }
    memset(Context,0,sizeof(VS_CORESIMPLECONTEXT));
    Context -> hVSDllInstance = vs_dll_open( ModuleName );
	if( Context -> hVSDllInstance == NULL )
		return NULL;
    Context -> VSRegisterCallBackInfo = (VSCore_RegisterCallBackInfoProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_REGISTERCALLBACKINFO_NAME );
    Context -> VSInit = (VSCore_InitProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_INIT_NAME );
    Context -> VSTermEx = (VSCore_TermExProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_TERMEX_NAME );
    Context -> VSQueryControlInterface = (VSCore_QueryControlInterfaceProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_QUERYCONTROLINTERFACE_NAME );
    if( Context -> VSRegisterCallBackInfo == NULL || Context -> VSInit == NULL || Context -> VSTermEx == NULL || Context -> VSQueryControlInterface == NULL )
        return NULL;
    if( Context -> VSInit( VS_TRUE, VS_TRUE, "", 0, "", ClientPort,NULL) == VSINIT_ERROR ){
		vs_dll_close(Context -> hVSDllInstance);
        Context -> hVSDllInstance = NULL;
        return NULL;
    }
    if( MsgCallBackProc != NULL )
        Context -> VSRegisterCallBackInfo( MsgCallBackProc, MsgCallBackPara );
    Context -> VSControlInterface = Context -> VSQueryControlInterface();
    Context -> VSBasicSRPInterface = Context -> VSControlInterface -> QueryBasicInterface(0);
    //--import service
    va_start(tArgList, MsgCallBackPara);
    CharPtr = va_arg( tArgList, VS_CHAR * );
    while( CharPtr != NULL ){
        if( Context -> VSBasicSRPInterface -> ImportService(CharPtr,VS_TRUE) == VS_FALSE ){
            Context -> VSControlInterface -> Release();
            Context -> VSBasicSRPInterface -> Release();
            Context -> VSTermEx();
    		vs_dll_close(Context -> hVSDllInstance);
            Context -> hVSDllInstance = NULL;
            return NULL;
        }
        CharPtr = va_arg( tArgList, VS_CHAR * );
    }
    va_end(tArgList);
    //--Create Service
    if( Context -> VSBasicSRPInterface -> CreateService( "",ServiceName,ServiceID,RootPass,5,10240,10240,10240,10240,10240 ) == VS_FALSE ){
        Context -> VSControlInterface -> Release();
        Context -> VSBasicSRPInterface -> Release();
        Context -> VSTermEx();
 		vs_dll_close(Context -> hVSDllInstance);
        Context -> hVSDllInstance = NULL;
        return NULL;
    }
    if( WebPort != 0 )
        Context -> VSBasicSRPInterface ->SetWebServerPort("",WebPort,100,2048);  //2M字节
    return Context -> VSBasicSRPInterface ->GetSRPInterface(ServiceName,"root",RootPass);
}

	class ClassOfBasicSRPInterface *VSCore_InitSimpleEx(VS_CORESIMPLECONTEXT *Context, VS_UINT16 ClientPort, VS_UINT16 WebPort, VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara, ...)
{
	VS_CHAR ModuleName[512],EnvModuleName[128];
    va_list tArgList;
    VS_CHAR *CharPtr;

    if( Context == NULL )
        return NULL;
	if( vs_get_env("SRPMODULE",EnvModuleName,128) == VS_FALSE )
		sprintf(EnvModuleName,"libstarcore");
    if( vs_string_strlen(g_CoreLibraryPath) == 0 ){
        if( vs_string_strlen(g_ShareLibraryPath) == 0 )
        	sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
        else
            sprintf(ModuleName,"%s/%s%s",g_ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
    }else{
        sprintf(ModuleName,"%s/%s%s",g_CoreLibraryPath,EnvModuleName,VS_MODULEEXT);
        if( vs_file_exist( ModuleName ) == VS_FALSE ){
            if( vs_string_strlen(g_ShareLibraryPath) == 0 )
            	sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
            else
                sprintf(ModuleName,"%s/%s%s",g_ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
        }
    }
    memset(Context,0,sizeof(VS_CORESIMPLECONTEXT));    
    Context -> hVSDllInstance = vs_dll_open( ModuleName );
	if( Context -> hVSDllInstance == NULL )
		return NULL;
    Context -> VSRegisterCallBackInfo = (VSCore_RegisterCallBackInfoProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_REGISTERCALLBACKINFO_NAME );
    Context -> VSInit = (VSCore_InitProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_INIT_NAME );
    Context -> VSTermEx = (VSCore_TermExProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_TERMEX_NAME );
    Context -> VSQueryControlInterface = (VSCore_QueryControlInterfaceProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_QUERYCONTROLINTERFACE_NAME );
    if( Context -> VSRegisterCallBackInfo == NULL || Context -> VSInit == NULL || Context -> VSTermEx == NULL || Context -> VSQueryControlInterface == NULL )
        return NULL;
    if( Context -> VSInit( VS_TRUE, VS_TRUE, "", 0, "", ClientPort,NULL) == VSINIT_ERROR ){
		vs_dll_close(Context -> hVSDllInstance);
        Context -> hVSDllInstance = NULL;
        return NULL;
    }
    if( MsgCallBackProc != NULL )
        Context -> VSRegisterCallBackInfo( MsgCallBackProc, MsgCallBackPara );
    Context -> VSControlInterface = Context -> VSQueryControlInterface();
    Context -> VSBasicSRPInterface = Context -> VSControlInterface -> QueryBasicInterface(0);
    //--import service
    va_start(tArgList, MsgCallBackPara);
    CharPtr = va_arg( tArgList, VS_CHAR * );
    while( CharPtr != NULL ){
        if( Context -> VSBasicSRPInterface -> ImportService(CharPtr,VS_TRUE) == VS_FALSE ){
            Context -> VSControlInterface -> Release();
            Context -> VSBasicSRPInterface -> Release();
            Context -> VSTermEx();
    		vs_dll_close(Context -> hVSDllInstance);
            Context -> hVSDllInstance = NULL;
            return NULL;
        }
        CharPtr = va_arg( tArgList, VS_CHAR * );
    }
    va_end(tArgList);
    if( WebPort != 0 )
        Context -> VSBasicSRPInterface ->SetWebServerPort("",WebPort,100,2048);  //2M字节
    return Context -> VSBasicSRPInterface ->Dup();
}

	class ClassOfSRPInterface *VSCore_InitSimpleWithCfg(VS_CORESIMPLECONTEXT *Context, VS_STARCONFIGEX *ConfigEx, const VS_CHAR *ServiceName, const VS_CHAR *RootPass, VS_UINT16 ClientPort, VS_UINT16 WebPort, VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara, ...)
{
	VS_CHAR ModuleName[512],EnvModuleName[128];
    va_list tArgList;
    VS_CHAR *CharPtr;

    if( Context == NULL || ServiceName == NULL || RootPass == NULL )
        return NULL;
	if( vs_get_env("SRPMODULE",EnvModuleName,128) == VS_FALSE )
		sprintf(EnvModuleName,"libstarcore");
    if( ConfigEx == NULL ){
        if( vs_string_strlen(g_CoreLibraryPath) == 0 ){
            if( vs_string_strlen(g_ShareLibraryPath) == 0 )
            	sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
            else
                sprintf(ModuleName,"%s/%s%s",g_ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
        }else{
            sprintf(ModuleName,"%s/%s%s",g_CoreLibraryPath,EnvModuleName,VS_MODULEEXT);
            if( vs_file_exist( ModuleName ) == VS_FALSE ){
                if( vs_string_strlen(g_ShareLibraryPath) == 0 )
            	    sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
                else
                    sprintf(ModuleName,"%s/%s%s",g_ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
            }
        }
    }else{
        if( vs_string_strlen(ConfigEx->CoreLibraryPath) == 0 ){
            if( vs_string_strlen(ConfigEx->ShareLibraryPath) == 0 )
            	sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
            else
                sprintf(ModuleName,"%s/%s%s",ConfigEx->ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
        }else{
            sprintf(ModuleName,"%s/%s%s",ConfigEx->CoreLibraryPath,EnvModuleName,VS_MODULEEXT);
            if( vs_file_exist( ModuleName ) == VS_FALSE ){
                if( vs_string_strlen(ConfigEx->ShareLibraryPath) == 0 )
            	    sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
                else
                    sprintf(ModuleName,"%s/%s%s",ConfigEx->ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
            }
        }
    }
    memset(Context,0,sizeof(VS_CORESIMPLECONTEXT));
    Context -> hVSDllInstance = vs_dll_open( ModuleName );
	if( Context -> hVSDllInstance == NULL )
		return NULL;
    Context -> VSRegisterCallBackInfo = (VSCore_RegisterCallBackInfoProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_REGISTERCALLBACKINFO_NAME );
    Context -> VSInit = (VSCore_InitProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_INIT_NAME );
    Context -> VSTermEx = (VSCore_TermExProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_TERMEX_NAME );
    Context -> VSQueryControlInterface = (VSCore_QueryControlInterfaceProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_QUERYCONTROLINTERFACE_NAME );
    if( Context -> VSRegisterCallBackInfo == NULL || Context -> VSInit == NULL || Context -> VSTermEx == NULL || Context -> VSQueryControlInterface == NULL )
        return NULL;
    if( Context -> VSInit( VS_TRUE, VS_TRUE, "", 0, "", ClientPort,ConfigEx) == VSINIT_ERROR ){
		vs_dll_close(Context -> hVSDllInstance);
        Context -> hVSDllInstance = NULL;
        return NULL;
    }
    if( MsgCallBackProc != NULL )
        Context -> VSRegisterCallBackInfo( MsgCallBackProc, MsgCallBackPara );
    Context -> VSControlInterface = Context -> VSQueryControlInterface();
    Context -> VSBasicSRPInterface = Context -> VSControlInterface -> QueryBasicInterface(0);
    //--import service
    va_start(tArgList, MsgCallBackPara);
    CharPtr = va_arg( tArgList, VS_CHAR * );
    while( CharPtr != NULL ){
        if( Context -> VSBasicSRPInterface -> ImportService(CharPtr,VS_TRUE) == VS_FALSE ){
            Context -> VSControlInterface -> Release();
            Context -> VSBasicSRPInterface -> Release();
            Context -> VSTermEx();
    		vs_dll_close(Context -> hVSDllInstance);
            Context -> hVSDllInstance = NULL;
            return NULL;
        }
        CharPtr = va_arg( tArgList, VS_CHAR * );
    }
    va_end(tArgList);
    //--Create Service
    if( Context -> VSBasicSRPInterface -> CreateService( "",ServiceName,NULL,RootPass,5,10240,10240,10240,10240,10240 ) == VS_FALSE ){
        Context -> VSControlInterface -> Release();
        Context -> VSBasicSRPInterface -> Release();
        Context -> VSTermEx();
 		vs_dll_close(Context -> hVSDllInstance);
        Context -> hVSDllInstance = NULL;
        return NULL;
    }
    if( WebPort != 0 )
        Context -> VSBasicSRPInterface ->SetWebServerPort("",WebPort,100,2048);  //2M字节
    return Context -> VSBasicSRPInterface ->GetSRPInterface(ServiceName,"root",RootPass);
}

	class ClassOfSRPInterface *VSCore_InitSimpleWithCfg1(VS_CORESIMPLECONTEXT *Context, VS_STARCONFIGEX *ConfigEx, const VS_CHAR *ServiceName, VS_UUID *ServiceID, const VS_CHAR *RootPass, VS_UINT16 ClientPort, VS_UINT16 WebPort, VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara, ...)
{
	VS_CHAR ModuleName[512],EnvModuleName[128];
    va_list tArgList;
    VS_CHAR *CharPtr;

    if( Context == NULL || ServiceName == NULL || RootPass == NULL )
        return NULL;
	if( vs_get_env("SRPMODULE",EnvModuleName,128) == VS_FALSE )
		sprintf(EnvModuleName,"libstarcore");
    if( ConfigEx == NULL ){
        if( vs_string_strlen(g_CoreLibraryPath) == 0 ){
            if( vs_string_strlen(g_ShareLibraryPath) == 0 )
            	sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
            else
                sprintf(ModuleName,"%s/%s%s",g_ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
        }else{
            sprintf(ModuleName,"%s/%s%s",g_CoreLibraryPath,EnvModuleName,VS_MODULEEXT);
            if( vs_file_exist( ModuleName ) == VS_FALSE ){
                if( vs_string_strlen(g_ShareLibraryPath) == 0 )
            	    sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
                else
                    sprintf(ModuleName,"%s/%s%s",g_ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
            }
        }
    }else{
        if( vs_string_strlen(ConfigEx->CoreLibraryPath) == 0 ){
            if( vs_string_strlen(ConfigEx->ShareLibraryPath) == 0 )
            	sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
            else
                sprintf(ModuleName,"%s/%s%s",ConfigEx->ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
        }else{
            sprintf(ModuleName,"%s/%s%s",ConfigEx->CoreLibraryPath,EnvModuleName,VS_MODULEEXT);
            if( vs_file_exist( ModuleName ) == VS_FALSE ){
                if( vs_string_strlen(ConfigEx->ShareLibraryPath) == 0 )
            	    sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
                else
                    sprintf(ModuleName,"%s/%s%s",ConfigEx->ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
            }
        }
    }
    memset(Context,0,sizeof(VS_CORESIMPLECONTEXT));
    Context -> hVSDllInstance = vs_dll_open( ModuleName );
	if( Context -> hVSDllInstance == NULL )
		return NULL;
    Context -> VSRegisterCallBackInfo = (VSCore_RegisterCallBackInfoProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_REGISTERCALLBACKINFO_NAME );
    Context -> VSInit = (VSCore_InitProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_INIT_NAME );
    Context -> VSTermEx = (VSCore_TermExProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_TERMEX_NAME );
    Context -> VSQueryControlInterface = (VSCore_QueryControlInterfaceProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_QUERYCONTROLINTERFACE_NAME );
    if( Context -> VSRegisterCallBackInfo == NULL || Context -> VSInit == NULL || Context -> VSTermEx == NULL || Context -> VSQueryControlInterface == NULL )
        return NULL;
    if( Context -> VSInit( VS_TRUE, VS_TRUE, "", 0, "", ClientPort,ConfigEx) == VSINIT_ERROR ){
		vs_dll_close(Context -> hVSDllInstance);
        Context -> hVSDllInstance = NULL;
        return NULL;
    }
    if( MsgCallBackProc != NULL )
        Context -> VSRegisterCallBackInfo( MsgCallBackProc, MsgCallBackPara );
    Context -> VSControlInterface = Context -> VSQueryControlInterface();
    Context -> VSBasicSRPInterface = Context -> VSControlInterface -> QueryBasicInterface(0);
    //--import service
    va_start(tArgList, MsgCallBackPara);
    CharPtr = va_arg( tArgList, VS_CHAR * );
    while( CharPtr != NULL ){
        if( Context -> VSBasicSRPInterface -> ImportService(CharPtr,VS_TRUE) == VS_FALSE ){
            Context -> VSControlInterface -> Release();
            Context -> VSBasicSRPInterface -> Release();
            Context -> VSTermEx();
    		vs_dll_close(Context -> hVSDllInstance);
            Context -> hVSDllInstance = NULL;
            return NULL;
        }
        CharPtr = va_arg( tArgList, VS_CHAR * );
    }
    va_end(tArgList);
    //--Create Service
    if( Context -> VSBasicSRPInterface -> CreateService( "",ServiceName,ServiceID,RootPass,5,10240,10240,10240,10240,10240 ) == VS_FALSE ){
        Context -> VSControlInterface -> Release();
        Context -> VSBasicSRPInterface -> Release();
        Context -> VSTermEx();
 		vs_dll_close(Context -> hVSDllInstance);
        Context -> hVSDllInstance = NULL;
        return NULL;
    }
    if( WebPort != 0 )
        Context -> VSBasicSRPInterface ->SetWebServerPort("",WebPort,100,2048);  //2M字节
    return Context -> VSBasicSRPInterface ->GetSRPInterface(ServiceName,"root",RootPass);
}

	class ClassOfBasicSRPInterface *VSCore_InitSimpleWithCfgEx(VS_CORESIMPLECONTEXT *Context, VS_STARCONFIGEX *ConfigEx, VS_UINT16 ClientPort, VS_UINT16 WebPort, VS_MsgCallBackProc MsgCallBackProc, VS_UWORD MsgCallBackPara, ...)
{
	VS_CHAR ModuleName[512],EnvModuleName[128];
    va_list tArgList;
    VS_CHAR *CharPtr;

    if( Context == NULL )
        return NULL;
	if( vs_get_env("SRPMODULE",EnvModuleName,128) == VS_FALSE )
		sprintf(EnvModuleName,"libstarcore");
    if( ConfigEx == NULL ){
        if( vs_string_strlen(g_CoreLibraryPath) == 0 ){
            if( vs_string_strlen(g_ShareLibraryPath) == 0 )
            	sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
            else
                sprintf(ModuleName,"%s/%s%s",g_ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
        }else{
            sprintf(ModuleName,"%s/%s%s",g_CoreLibraryPath,EnvModuleName,VS_MODULEEXT);
            if( vs_file_exist( ModuleName ) == VS_FALSE ){
                if( vs_string_strlen(g_ShareLibraryPath) == 0 )
            	    sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
                else
                    sprintf(ModuleName,"%s/%s%s",g_ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
            }
        }
    }else{
        if( vs_string_strlen(ConfigEx->CoreLibraryPath) == 0 ){
            if( vs_string_strlen(ConfigEx->ShareLibraryPath) == 0 )
            	sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
            else
                sprintf(ModuleName,"%s/%s%s",ConfigEx->ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
        }else{
            sprintf(ModuleName,"%s/%s%s",ConfigEx->CoreLibraryPath,EnvModuleName,VS_MODULEEXT);
            if( vs_file_exist( ModuleName ) == VS_FALSE ){
                if( vs_string_strlen(ConfigEx->ShareLibraryPath) == 0 )
            	    sprintf(ModuleName,"%s%s%s",VS_COREPATH,EnvModuleName,VS_MODULEEXT);
                else
                    sprintf(ModuleName,"%s/%s%s",ConfigEx->ShareLibraryPath,EnvModuleName,VS_MODULEEXT);
            }
        }
    }
    memset(Context,0,sizeof(VS_CORESIMPLECONTEXT));    
    Context -> hVSDllInstance = vs_dll_open( ModuleName );
	if( Context -> hVSDllInstance == NULL )
		return NULL;
    Context -> VSRegisterCallBackInfo = (VSCore_RegisterCallBackInfoProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_REGISTERCALLBACKINFO_NAME );
    Context -> VSInit = (VSCore_InitProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_INIT_NAME );
    Context -> VSTermEx = (VSCore_TermExProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_TERMEX_NAME );
    Context -> VSQueryControlInterface = (VSCore_QueryControlInterfaceProc)vs_dll_sym( Context -> hVSDllInstance, VSCORE_QUERYCONTROLINTERFACE_NAME );
    if( Context -> VSRegisterCallBackInfo == NULL || Context -> VSInit == NULL || Context -> VSTermEx == NULL || Context -> VSQueryControlInterface == NULL )
        return NULL;
    if( Context -> VSInit( VS_TRUE, VS_TRUE, "", 0, "", ClientPort,ConfigEx) == VSINIT_ERROR ){
		vs_dll_close(Context -> hVSDllInstance);
        Context -> hVSDllInstance = NULL;
        return NULL;
    }
    if( MsgCallBackProc != NULL )
        Context -> VSRegisterCallBackInfo( MsgCallBackProc, MsgCallBackPara );
    Context -> VSControlInterface = Context -> VSQueryControlInterface();
    Context -> VSBasicSRPInterface = Context -> VSControlInterface -> QueryBasicInterface(0);
    //--import service
    va_start(tArgList, MsgCallBackPara);
    CharPtr = va_arg( tArgList, VS_CHAR * );
    while( CharPtr != NULL ){
        if( Context -> VSBasicSRPInterface -> ImportService(CharPtr,VS_TRUE) == VS_FALSE ){
            Context -> VSControlInterface -> Release();
            Context -> VSBasicSRPInterface -> Release();
            Context -> VSTermEx();
    		vs_dll_close(Context -> hVSDllInstance);
            Context -> hVSDllInstance = NULL;
            return NULL;
        }
        CharPtr = va_arg( tArgList, VS_CHAR * );
    }
    va_end(tArgList);
    if( WebPort != 0 )
        Context -> VSBasicSRPInterface ->SetWebServerPort("",WebPort,100,2048);  //2M字节
    return Context -> VSBasicSRPInterface ->Dup();
}

class ClassOfSRPInterface *VSCore_InitService(class ClassOfBasicSRPInterface *BasicSRPInterface,const VS_CHAR *ServiceName,VS_UUID *ServiceID,const VS_CHAR *RootPass,VS_INT32 FrameInterval,VS_INT32 NetPkgSize,VS_INT32 UploadPkgSize,VS_INT32 DownloadPkgSize,VS_INT32 DataUpPkgSize,VS_INT32 DataDownPkgSize,...)
{
    class ClassOfSRPInterface *SRPInterface;
    va_list tArgList;
    VS_CHAR *CharPtr;

    //--Create Service
    if( BasicSRPInterface -> CreateService( "",ServiceName,ServiceID,RootPass,FrameInterval,NetPkgSize,UploadPkgSize,DownloadPkgSize,DataUpPkgSize,DataDownPkgSize ) == VS_FALSE )
        return NULL;
    SRPInterface = BasicSRPInterface ->GetSRPInterface(ServiceName,"root",RootPass);
    if( SRPInterface == NULL )
        return NULL;
    va_start(tArgList, DataDownPkgSize);
    CharPtr = va_arg( tArgList, VS_CHAR * );
    while( CharPtr != NULL ){
        SRPInterface -> CreateSysRootItem( CharPtr, NULL, NULL, NULL );
        CharPtr = va_arg( tArgList, VS_CHAR * );
    }
    va_end(tArgList);
    return SRPInterface;
}

void VSCore_TermSimple(VS_CORESIMPLECONTEXT *Context)
{
    if( Context == NULL )
        return;
    if( Context -> hVSDllInstance != NULL ){
        Context -> VSControlInterface -> Release();
        Context -> VSBasicSRPInterface -> Release();
        Context -> VSTermEx();
 		vs_dll_close(Context -> hVSDllInstance);
        Context -> hVSDllInstance = NULL;
    }
}

void VSCore_RunSimple(VS_CORESIMPLECONTEXT *Context,VS_SRPMsgLoopExitProc SRPMsgLoopExitProc)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	VS_BOOL QuitFlag;
#endif

    if( Context == NULL )
        return;
	while(SRPMsgLoopExitProc == NULL || SRPMsgLoopExitProc() == VS_FALSE)
	{
        if( Context -> VSBasicSRPInterface -> QueryActiveService(NULL) == NULL )
            break;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
        if( Context -> VSControlInterface -> WinMsgLoop(&QuitFlag) == VS_TRUE ){
			if(QuitFlag == VS_TRUE)
				break;
		}else{
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
        {
#endif
			Context -> VSControlInterface -> SRPDispatch(VS_TRUE);
			Context -> VSControlInterface -> SRPIdle();
        }
    }
    VSCore_TermSimple(Context);
}

void VSCore_ReleaseStarCore(class ClassOfStarCore *core)
{
    if( core == NULL )
        return;
    ((class ClassOfVSStarCore *)core) -> VSControlInterface -> Release();
    ((class ClassOfVSStarCore *)core) -> BasicSRPInterface -> Release();
    core -> Release();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

VS_HANDLE starlib_dll_open_starcore(VS_CHAR *TempFileName,VS_INT32 TempFileNameSize)
{
    VSCore_QueryControlInterfaceProc VSQueryControlInterface;
    class ClassOfSRPControlInterface *VSControlInterface;
    class ClassOfBasicSRPInterface *BasicSRPInterface;
	VS_CHAR ModuleName[512];
    VS_CHAR ModuleName1[512];
    VS_CHAR DumCoreModuleFileName[512],EnvModuleName[128];
    VS_INT8 VSUuidStringBuf[64];
    VS_UUID LocalDllID;
    VS_CHAR *ExistTempFile;
    VS_HANDLE hVSDllInstance;

    vs_dir_getcorepath( ModuleName, 512 );
	if( vs_get_env("SRPMODULE",EnvModuleName,128) == VS_FALSE )
		sprintf(EnvModuleName,"libstarcore");
	sprintf(ModuleName1,"\\%s%s",EnvModuleName,VS_MODULEEXT);
    strcat(ModuleName,ModuleName1);

    DumCoreModuleFileName[0] = 0;
    hVSDllInstance = vs_dll_get(ModuleName);
    if( hVSDllInstance != NULL  ){
        VSQueryControlInterface = (VSCore_QueryControlInterfaceProc)vs_dll_sym( hVSDllInstance, VSCORE_QUERYCONTROLINTERFACE_NAME );
        VSControlInterface = VSQueryControlInterface();
        BasicSRPInterface = VSControlInterface -> QueryBasicInterface(0);
        //---已经装载
        ExistTempFile = BasicSRPInterface -> GetRegTempFile(ModuleName,ModuleName1,512);
        if( ExistTempFile == NULL ){ //not exist
            vs_uuid_create( &LocalDllID );
            Lib_SetUUIDString( &LocalDllID, VSUuidStringBuf );
            GetVirtualSocietyUserPath(VS_SERVER_SERVER,DumCoreModuleFileName,512,VS_TRUE);
            sprintf(ModuleName1,"\\%s.so",VSUuidStringBuf);
            if( vs_file_exist( ModuleName ) == VS_FALSE ){
                VSControlInterface -> Release();
                BasicSRPInterface -> Release();
                return NULL;
            }
            strcat(DumCoreModuleFileName,ModuleName1);
            vs_file_copy( ModuleName, DumCoreModuleFileName );
            BasicSRPInterface -> RegTempFile(DumCoreModuleFileName,ModuleName);
            strcpy( ModuleName, DumCoreModuleFileName );
        }else{
            strcpy(DumCoreModuleFileName,ModuleName1);
            strcpy( ModuleName, DumCoreModuleFileName );
        }
        hVSDllInstance = vs_dll_open( ModuleName );
        if( hVSDllInstance == NULL ){
            if( DumCoreModuleFileName[0] != 0 )
                BasicSRPInterface -> UnRegTempFile(DumCoreModuleFileName);
            VSControlInterface -> Release();
            BasicSRPInterface -> Release();
    	    return NULL;
        }
        if( TempFileName != NULL ){
            VS_STRNCPY(TempFileName,ModuleName,TempFileNameSize);
        }
        VSControlInterface -> Release();
        BasicSRPInterface -> Release();
        return hVSDllInstance;
    }
    hVSDllInstance = vs_dll_open( ModuleName );
    if( TempFileName != NULL )
        TempFileName[0] = 0;
    return hVSDllInstance;
}

VS_HANDLE starlib_dll_open(class ClassOfSRPControlInterface *ControlInterface,const VS_CHAR *ModuleName)
{
    class ClassOfBasicSRPInterface *BasicSRPInterface;
    VS_CHAR OriginModuleName[512];
    VS_CHAR ModuleName1[512];
    VS_CHAR DumCoreModuleFileName[512];
    VS_INT8 VSUuidStringBuf[64];
    VS_UUID LocalDllID;
    VS_CHAR *ExistTempFile;
    VS_HANDLE hVSDllInstance;

    //---get full name
    DumCoreModuleFileName[0] = 0;
    hVSDllInstance = vs_dll_getex(ModuleName,OriginModuleName,512);
    if( hVSDllInstance != NULL  ){  //--has load
        BasicSRPInterface = ControlInterface -> QueryBasicInterface(0);
        //---has load
        ExistTempFile = BasicSRPInterface -> GetRegTempFile(OriginModuleName,ModuleName1,512);
        if( ExistTempFile == NULL ){ //not exist
            vs_uuid_create( &LocalDllID );
            Lib_SetUUIDString( &LocalDllID, VSUuidStringBuf );
            GetVirtualSocietyUserPath(VS_SERVER_SERVER,DumCoreModuleFileName,512,VS_TRUE);
            sprintf(ModuleName1,"\\%s.so",VSUuidStringBuf);
            if( vs_file_exist( OriginModuleName ) == VS_FALSE ){
                BasicSRPInterface -> Release();
                return NULL;
            }
            strcat(DumCoreModuleFileName,ModuleName1);
            vs_file_copy( OriginModuleName, DumCoreModuleFileName );
            BasicSRPInterface -> RegTempFile(DumCoreModuleFileName,OriginModuleName);
            strcpy( ModuleName1, DumCoreModuleFileName );
        }else{
            strcpy(DumCoreModuleFileName,ModuleName1);
        }
        hVSDllInstance = vs_dll_open( ModuleName1 );
        if( hVSDllInstance == NULL ){
            if( DumCoreModuleFileName[0] != 0 )
                BasicSRPInterface -> UnRegTempFile(DumCoreModuleFileName);
            BasicSRPInterface -> Release();
    	    return NULL;
        }
        BasicSRPInterface -> Release();
        return hVSDllInstance;
    }
    hVSDllInstance = vs_dll_open( ModuleName );
    return hVSDllInstance;
}

#ifndef VS_OPENSYSEVENTDEF
#define VS_OPENSYSEVENTDEF

#include "vsopencommtype.h"

/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/*---------------------------------define sysevent---------------------------------*/
#define VSEVENT_SYSTEMEVENT_ONBEFOREFIRSTCREATE       0x00000001
/*--IN   EventPara.LParam = ParentAttributeIndex*/
/*--IN   EventPara.SParam = *AttachBuf  may be NULL*/
/*--IN   EventPara.TParam = *NewObjectClassID (VS_UUID)*/
/*--IN   EventPara.FParam = 0, 1*/
/*--IN   EventPara.DesObject = ParentObject(FParam = 0) / Class(FParam = 1)   SrcObject = NULL*/
/*--OUT  EventPara.LParam == 0 permit, else forbid and is errorcode*/

#define VSEVENT_SYSTEMEVENT_ONFIRSTCREATE             0x00000002
/*--IN   EventPara.LParam = *AttachBuf    */ /* object init data*/
/*--IN   EventPara.SParam = AttachBufSize */ /* object init data size*/
/*--IN   EventPara.TParam = ParentObject  */ /* parent object*/
/*--OUT  None*/

#define VSEVENT_SYSTEMEVENT_ONMALLOC                  0x00000003      /*--alloc object memory*/
/*--OUT  None*/

#define VSEVENT_SYSTEMEVENT_ONFREE                    0x00000004      /*--free object memory*/
/*--OUT  None*/

#define VSEVENT_SYSTEMEVENT_ONCREATE                  0x00000005
/*--OUT  None*/

#define VSEVENT_SYSTEMEVENT_ONDESTORY                 0x00000006
#define VSEVENT_SYSTEMEVENT_ONDESTROY                 0x00000006
/*--OUT  None */

#define VSEVENT_SYSTEMEVENT_ONBEFOREDESTORY           0x0000001B    /* triggered when free object, before free child object, before VSEVENT_SYSTEMEVENT_ONDESTORY */
#define VSEVENT_SYSTEMEVENT_ONBEFOREDESTROY           0x0000001B
/*--OUT  None */

#define VSEVENT_SYSTEMEVENT_ONCREATECHILD             0x00000007   /*---triggered after child's OnCreate event */
/*--IN   EventPara.LParam = ChildObject*/
/*--IN   EventPara.DesObject = Object   SrcObject = NULL*/
/*--OUT  None*/

#define VSEVENT_SYSTEMEVENT_ONDESTORYCHILD            0x00000008
#define VSEVENT_SYSTEMEVENT_ONDESTROYCHILD            0x00000008
/*--IN   EventPara.LParam = ChildObject*/
/*--IN   EventPara.DesObject = Object   SrcObject = NULL */
/*--OUT  None */

#define VSEVENT_SYSTEMEVENT_ONACTIVATING                0x00000009
/*--IN   EventPara.LParam = 0  normal activate  1 activate for object is load  */
/*--OUT  EventPara.LParam == 0 success, else fail, (if no response, default is success) */

#define VSEVENT_SYSTEMEVENT_ONDEACTIVATING              0x00000010
/*--IN   EventPara.LParam = 0  normal deactivate  1 deactivate for object is load */
/*--OUT  None                                                                       */

#define VSEVENT_SYSTEMEVENT_ONACTIVATE                0x00000011
/*--IN   EventPara.LParam = 0  normal activate  1 activate for object is load     */
/*--OUT  None                                                                       */

#define VSEVENT_SYSTEMEVENT_ONDEACTIVATE              0x00000012
/*--IN   EventPara.LParam = 0  normal activate  1 activate for object is load     */
/*--OUT  None                                                                       */

#define VSEVENT_SYSTEMEVENT_ONACTIVATECHILD           0x00000013
/*--IN   EventPara.LParam = ChildObject                                           */
/*--IN   EventPara.DesObject = Object   SrcObject = NULL                          */
/*--OUT  None                                                                       */

#define VSEVENT_SYSTEMEVENT_ONDEACTIVATECHILD         0x00000014
/*--IN   EventPara.LParam = ChildObject                                           */
/*--IN   EventPara.DesObject = Object   SrcObject = NULL                          */
/*--OUT  None                                                                       */

#define VSEVENT_SYSTEMEVENT_ONATTRIBUTEBEFORECHANGE   0x00000015   /*---only send to class which defines the attribute */
/*--IN   EventPara.LParam = AttributeIndex                                         */
/*--IN   EventPara.SParam = 0 reserved                                             */
/*--IN   EventPara.TParam = NewValue       */  /*---address for new value. for global pointer attribute, it is the address of ClassID of the new created object */
/*--IN   EventPara.FParam = DebugEditFlag; */  /*---==0 normal  ==1 triggered by debug edit   */
/*--IN   EventPara.DesObject = Object   SrcObject = NULL   */
/*--OUT  EventPara.LParam == 0 permit change, else forbid and is the errorcode( take effect when object is local control)   */
/*--the corresponding attribute must be declared to accept this event, which can be defined using service description file or function "CreateAtomicAttribute" */

#define VSEVENT_SYSTEMEVENT_ONATTRIBUTECHANGE         0x00000016    /*---only send to class which defines the attribute  */
/*--IN   EventPara.LParam = AttributeIndex      */
/*--IN   EventPara.SParam = (VS_ATTRIBUTEINDEXMAP *)AttributeIndexMap   */
/*--OUT  None         */
/*--the corresponding attribute must be declared to accept this event, which can be defined using service description file or function "CreateAtomicAttribute"   */

#define VSEVENT_SYSTEMEVENT_ONPARENTBEFORECHANGE     0x00000017
/*--IN   EventPara.LParam = (VS_UWORD)*ParentObject             */
/*--IN   EventPara.DesObject = Object   SrcObject = NULL        */
/*--OUT  EventPara.LParam == 0 permit change, else forbid and is the errorcode( take effect when object is local control)     */

#define VSEVENT_SYSTEMEVENT_ONPARENTCHANGE           0x00000018
/*--IN   EventPara.DesObject = Object   SrcObject = NULL        */
/*--OUT  None                                                     */

#define VSEVENT_SYSTEMEVENT_ONSTATICCHANGE           0x00000019  /*---only send to class which defines the attribute    */
/*--IN   EventPara.LParam = AttributeIndex  */
/*--should process instantly                 */

#define VSEVENT_SYSTEMEVENT_ONSCRIPTCHANGE           0x0000001A
/*--IN   EventPara.LParam = ScriptName                             */
/*--IN   EventPara.SParam = Operation:  0 - Change  1  Create      */
/*--should process instantly,do not record status such as script exists, not exists, or how to prrocess */

/*----about sync statsus  */
#define VSEVENT_SYSTEMEVENT_ONBECOMESYNC             0x00000030  /*----object changes to sync, may preform some function which only valid under sync status                   */
                                                                 /*----only create once, may be together with VSEVENT_SYSTEMEVENT_ONMODULEINIT event to init the object       */
                                                                 /*----the event only create for object itself, does not include its instance                                 */
                                                                 /*----only create once, when object change to sync                                                           */
                                                                 /*----the event does not create to script functions                                                          */
/*--OUT  none */

/*----About activate sets and synchronous group events */
#define VSEVENT_SYSTEMEVENT_ONSYNCGROUPCHANGE        0x00000040  /*----Object changes synchronous group */
/*--OUT  none  */
#define VSEVENT_SYSTEMEVENT_ONCHILDSYNCGROUPCHANGE   0x00000041  /*----Subobject synchronization group change  */
/*--IN   EventPara.LParam = ChildObject  */
/*--OUT  none                            */
#define VSEVENT_SYSTEMEVENT_ONACTIVESETCHANGE        0x00000042  /*----Serviceitem activation set to change    */
/*--IN  EventPara.LParam = SysRootItem               Serviceitem object address   */
/*--OUT  none                                                                     */

/*------------------------------------------------------------------------------  */
/*----save or load on objects, only create when object save or load methods is called   */
#define VSEVENT_SYSTEMEVENT_ONSAVE                   0x00000050
/*--IN  none */
/*--OUT EventPara.LParam = Buf               must alloc memory using Malloc function of interface   */
/*--OUT EventPara.SParam = BufSize                                                                  */
/*--OUT EventPara.TParam = 0 success, otherwise fail                                                */

#define VSEVENT_SYSTEMEVENT_ONLOAD                   0x00000051  /*----load object   */
/*--IN  EventPara.LParam = Buf   */
/*--IN  EventPara.SParam = BufSize */
/*--OUT EventPara.LParam = 0 success, otherwise fail  */

#define VSEVENT_SYSTEMEVENT_ONLOADMASK               0x00000052  /*----Get loaded object's mask         */
/*--IN  EventPara.LParam = &VS_ATTRIBUTEINDEXMAP  if bit is set,then does not cover at the loading time  */
/*--OUT none                                                                                             */

#define VSEVENT_SYSTEMEVENT_ONLOADFINISH             0x00000053
/*--IN  none  */
/*--OUT none  */

/*------------------------------------------------------------------------------ */
/*----send message between client and server                                     */
#define VSEVENT_SYSTEMEVENT_ONREMOTESEND             0x00000060  /*----receive peer message  */
/*--IN EventPara.LParam = VS_PARAPKGPTR  message  */
/*--OUT none                                      */
/*--message is free by starcore, may be call GetRemotePrivateTag, GetRemoteID to get client tag and client id    */

#define VSEVENT_SYSTEMEVENT_ONCALL                   0x00000061  /*----function call  */
/*--IN EventPara.LParam = ClassOfSRPFunctionParaInterface *       */
/*--IN EventPara.SParam = FunctionName                            */
/*--IN EventPara.TParam = FunctionID(VS_UUID *)                    */
/*--OUT EventPara.LParam = Result   if return type is VS_FLOAT, should uses ((VS_FLOAT *)&LParam)[0]           */
/*--OUT EventPara.SParam = RetType  if no return value, RetType should set yo VSTYPE_IGNORE, LParam set to 0   */
/*--ClassOfSRPFunctionParaInterface is freed by starcore                                                       */

/********************************************************************************/
/*----timer pulse event*/
#define VSEVENT_SYSTEMEVENT_ONTICKET                  0x00000080
/*--IN   EventPara.LParam = CurrentTicket*/
/*--Must be handled immediately*/

#define VSEVENT_SYSTEMEVENT_ONFRAMETICKET             0x00000081
/*--IN   EventPara.LParam = CurrentTicket*/
/*--IN   EventPara.SParam = FrameTimer counter*/
/*--Must be handled immediately*/

/*----main program is idle,does not process any window event*/
#define VSEVENT_SYSTEMEVENT_ONIDLE                    0x00000082
/*--IN   EventPara.LParam = CurrentTicket*/
/*--OUT  EventPara.LParam = 1  continue to create IDLE event */
/*--Must be handled immediately*/

#define VSEVENT_SYSTEMEVENT_ONAPPACTIVE               0x00000083
/*--Must be handled immediately */

#define VSEVENT_SYSTEMEVENT_ONAPPDEACTIVE             0x00000084
/*--Must be handled immediately  */

#define VSEVENT_SYSTEMEVENT_ONSERVICEACTIVE           0x00000085
/*--Must be handled immediately  */

#define VSEVENT_SYSTEMEVENT_ONSERVICEDEACTIVE         0x00000086
/*--Must be handled immediately  */

/********************************************************************************/
/*----module manager event*/
#define VSEVENT_SYSTEMEVENT_ONMODULEINIT              0x00000090
/*--IN  none                         */
/*--Must be handled immediately      */

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
#define VSEVENT_SYSTEMEVENT_ONMODULETERM              0x00000091
/*--IN  none*/
/*--Must be handled immediately*/
#define VSEVENT_SYSTEMEVENT_ONMODULECANBEUNLOAD       0x00000092
/*--IN  none*/
/*--OUT EventPara.LParam = result, ==0 may be unload  ==1 should wait*/
/*--Must be handled immediately*/

/********************************************************************************/
/*----lua parameter translate event*/
#define VSEVENT_SYSTEMEVENT_ONVSTOSCRIPTINPUTPARA            0x00000100  /*----translate to script parameter*/
/*--IN   EventPara.LParam = (VS_UWORD)&EventUUID*/
/*--IN   EventPara.SParam = (VS_UWORD)&EventParam*/
/*--OUT  EventPara.LParam = result,==1 success  ==0 no change */

#define VSEVENT_SYSTEMEVENT_ONSCRIPTTOVSINPUTPARA            0x00000101  /*----translate to c parameter*/
/*--IN   EventPara.LParam = (VS_UWORD)&EventUUID*/
/*--IN   EventPara.SParam = (VS_UWORD)&EventParam*/
/*--OUT  EventPara.LParam = result,==1 success  ==0 no change*/

#define VSEVENT_SYSTEMEVENT_ONVSTOSCRIPTOUTPUTPARA           0x00000102  /*----translate to script parameter*/
/*--IN   EventPara.LParam = (VS_UWORD)&EventUUID*/
/*--IN   EventPara.SParam = (VS_UWORDG)&EventParam*/
/*--OUT  EventPara.LParam = result,==1 success  ==0 no change*/

#define VSEVENT_SYSTEMEVENT_ONSCRIPTTOVSOUTPUTPARA           0x00000103  /*----translate to c parameter*/
/*--IN   EventPara.LParam = (VS_UWORD)&EventUUID*/
/*--IN   EventPara.SParam = (VS_UWORD)&EventParam*/
/*--OUT  EventPara.LParam = result,==1 success  ==0 no change*/


/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/*---------edit event, valid at debugserver                                     */
/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
#define VSEVENT_SYSTEMEDIEEVENT_START    0x00001000

#define VSEVENT_SYSTEMEDIEEVENT_ONQUERYEDIT      VSEVENT_SYSTEMEDIEEVENT_START + 1
/*--IN   DesObject = Object SrcObject = NULL; */
/*--OUT  EventPara.LParam = 0edit function not exist,==1 edit function exist*/

/*----Add an instance of the class, the event will fill the additional parameters needed to increase*/
#define VSEVENT_SYSTEMEDIEEVENT_ONEDITFILLADDBUF VSEVENT_SYSTEMEDIEEVENT_START + 2
/*--IN   EventPara.LParam = *AttachBuf*/
/*--OUT  EventPara.LParam = 0not process, == returns the size of the struct filled*/

/*----Have chosen the time when the parent object is editing state, the child object does not trigger the event*/
#define VSEVENT_SYSTEMEDIEEVENT_ONEDIT           VSEVENT_SYSTEMEDIEEVENT_START + 3
/*--IN   EventPara.LParam = *EditObject*/
/*--IN   EventPara.SParam = AppData Self-definition of application layer, used to initialize the editor state; debugger initialization editor, use 0 */
/*--IN   EventPara.TParam = AppData1 Self-definition of application layer, used to initialize the editor state; debugger initialization editor, use 0 */
/*--IN  DesObject = Object SrcObject = NULL;   */

#define VSEVENT_SYSTEMEDIEEVENT_ONTERMEDIT       VSEVENT_SYSTEMEDIEEVENT_START + 4
/*--IN   EventPara.LParam = *EditObject        */
/*--IN  DesObject = Object SrcObject = NULL;   */

#define VSEVENT_SYSTEMEDIEEVENT_ONEDITSELECT     VSEVENT_SYSTEMEDIEEVENT_START + 5
/*--IN   EventPara.LParam = (VS_UUID *)SelectObject                              */
/*--IN  DesObject = Object SrcObject = NULL; */ /*--DesObject is parent object     */

#define VSEVENT_SYSTEMEDIEEVENT_ONEDITEBUTTON    VSEVENT_SYSTEMEDIEEVENT_START + 6
/*--IN  EventPara.LParam = (VS_INT8 *)Buf,  store result   */
/*--IN  EventPara.SParam = *OBJECTATTRIBUTEINDEX;           */
/*--IN  EventPara.TParam = AttributeDeepth;  */
/*--IN  DesObject = Object SrcObject = NULL; */

#define VSEVENT_SYSTEMEDIEEVENT_ONEDITSHOW      VSEVENT_SYSTEMEDIEEVENT_START + 7
/*--IN  EventPara.LParam = (VS_INT8 *)Buf,  Storage buffer to adjust the display string    */
/*--IN  EventPara.SParam = *OBJECTATTRIBUTEINDEX; */ /*--Pointer type, the attribute index  */
/*--IN  EventPara.TParam = AttributeDeepth;  */
/*--IN  DesObject = Object SrcObject = NULL; */

#define VSEVENT_SYSTEMEDIEEVENT_ONEDITPREVIEW   VSEVENT_SYSTEMEDIEEVENT_START + 8
/*--IN  EventPara.LParam = HDC   */
/*--IN  EventPara.SParam = VS_RECT * */ /*--display region,Left,Top; Right and Bottom */
/*--IN  DesObject = Object SrcObject = NULL;  */

#define VSEVENT_SYSTEMEDIEEVENT_ONATTRIBUTEEDITFLAG     VSEVENT_SYSTEMEDIEEVENT_START + 10
/*--IN  EventPara.LParam = AttriubteIndex  */
/*--IN  DesObject = Object SrcObject = NULL;  */
/*--OUT  EventPara.LParam = 0 none 1 invisible and can not edit  2 visible and can not edit 3 may edit  */
#define VSSYSTEMEDIT_ATTRIBUTEEDITFLAG_NONE       0
#define VSSYSTEMEDIT_ATTRIBUTEEDITFLAG_INVISIBLE  1
#define VSSYSTEMEDIT_ATTRIBUTEEDITFLAG_READONLY   2
#define VSSYSTEMEDIT_ATTRIBUTEEDITFLAG_EDIT       3

/*------------------------------------------------------------------------------ */
#define VSEVENT_SYSTEMEDIEEVENT_ONBEFOREFIRSTCREATE       VSEVENT_SYSTEMEDIEEVENT_START + 11
/*--IN   EventPara.LParam = ParentAttributeIndex  */
/*--IN   EventPara.SParam = *AttachBuf */ /* May be empty, if valid, you need to check the legality of parameters  */
/*--IN   EventPara.TParam = *NewObjectClassID (VS_UUID)                */
/*--IN   EventPara.FParam = ==0 : global create  ==1 : load create     */
/*--IN   EventPara.DesObject = ParentObject   SrcObject = NULL         */
/*--OUT  EventPara.LParam == 0 permit,otherwise not, and is the error code  */

/*------------------------------------------------------------------------------ */

#endif

package com.srplab.starflut;

import io.flutter.plugin.common.MethodCall;
import io.flutter.plugin.common.MethodChannel;
import io.flutter.plugin.common.MethodChannel.MethodCallHandler;
import io.flutter.plugin.common.MethodChannel.Result;
import io.flutter.plugin.common.PluginRegistry.Registrar;
import io.flutter.plugin.common.BasicMessageChannel;
import io.flutter.plugin.common.StandardMessageCodec;

import java.util.UUID;
import com.srplab.www.starcore.*;

import android.app.Activity;
import android.os.Bundle;
import android.content.res.AssetManager;
import android.os.Handler;
import android.os.Message;
import android.os.Looper;

import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.concurrent.locks.*;

import java.io.ByteArrayOutputStream;   
import java.io.IOException;   
import java.io.InputStream;
import java.io.OutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.Enumeration;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import java.util.Iterator;
import java.util.List;
import java.io.FileNotFoundException;

/*
    dart                                 starcore(cle)                                          dart
    (ui thread)                          (thread)                                               (ui thread)

    script call(ui thread) -> send msg ->
                                          run
                                              -> call back -> send msg and wait result ->       run(call invokeMethod, with Result callback)
                                                   return callback result              <-       set wait result
                                                                                                
*/

/** StarflutPlugin */
@SuppressWarnings("unchecked")
public class StarflutPlugin implements MethodCallHandler {
  private Activity activity;
  private MethodChannel channel;
  private BasicMessageChannel messagechannel;
  HashMap<String,Object> CleObjectMap;
  private Handler mainHandler;

  private Object starCoreThreadCallDeepSyncObject;
  private int starCoreThreadCallDeep;

  private Thread starCoreHandlerThread;
  private Handler starCoreHandler;
  private Looper starCoreHandlerLooper;


  boolean StarCoreInitFlag;
  StarCoreFactory starcore;

  final String StarCorePrefix =     "@s_s_c";
  final String StarSrvGroupPrefix = "@s_s_g";  /*--with group index: not supported */
  final String StarServicePrefix =  "@s_s_e";  /*--with service id--*/
  final String StarParaPkgPrefix =  "@s_s_p";
  final String StarBinBufPrefix  =  "@s_s_b";
  //final String StarSXmlPrefix  =  "@s_s_x";
  final String StarObjectPrefix = "@s_s_o";    /*--with object id--*/
  final String StarValueBooleanPrefix_TRUE = "@s_s_t_bool_true";  /*-fix bug for return boolean in arraylist or hashmap-*/
  final String StarValueBooleanPrefix_FALSE = "@s_s_t_bool_false"; 

  /*--objectag:  prefix + uuid + '+' + (objectid/serviceid/groupindex)

  /*--not used, may be in future --*/
  class StarFlutWaitResult
  {
      boolean FinishFlag;
      Object Result;
      Object Tag;
      private ReentrantLock reentrantLock;

      public StarFlutWaitResult()
      {
          Result = null;
          FinishFlag = false;
          reentrantLock = new ReentrantLock();
      }
      public Object WaitResult()
      {
          reentrantLock.lock();
          while( FinishFlag == false ){
              reentrantLock.unlock();
              try{
                  Thread.sleep(10);
              }
              catch(Exception ex)
              {

              }
              reentrantLock.lock();
          }
          reentrantLock.unlock();
          return Result;
      }
      public void SetResult(Object result)
      {
          reentrantLock.lock();
          Result = result;
          FinishFlag = true;
          reentrantLock.unlock();
      }
  }

  boolean unzip(InputStream zipFileName, String outputDirectory,Boolean OverWriteFlag ) {
    try {
        ZipInputStream in = new ZipInputStream(zipFileName);
        ZipEntry entry = in.getNextEntry();
        byte[] buffer = new byte[1024];
        while (entry != null) {
            File file = new File(outputDirectory);
            file.mkdir();
            if (entry.isDirectory()) {
                String name = entry.getName();
                name = name.substring(0, name.length() - 1);
                if( CreatePath(outputDirectory + File.separator + name) == false )
                    return false;
            } else {
                String name = outputDirectory + File.separator + entry.getName();
                int Index = name.lastIndexOf(File.separator.charAt(0));
                if( Index < 0 ){
                    file = new File(outputDirectory + File.separator + entry.getName());
                }else{
                    String ParentPath = name.substring(0, Index);
                    if( CreatePath(ParentPath) == false )
                        return false;
                    file = new File(outputDirectory + File.separator + entry.getName());
                }
                if( !file.exists() || OverWriteFlag == true){
                    file.createNewFile();
                    FileOutputStream out = new FileOutputStream(file);
                    int readLen = 0;
                    while((readLen = in.read(buffer)) != -1){
                        out.write(buffer, 0, readLen);
                    }
                    out.close();
                }
            }
            entry = in.getNextEntry();
        }
        in.close();
        return true;
    } catch (FileNotFoundException e) {
        e.printStackTrace();
        return false;
    } catch (IOException e) {
        e.printStackTrace();
        return false;
    }
  }  

  void copyFile(Activity c, String Name,String srcPath,String desPath) throws IOException {
    File outfile = null;
    outfile = new File(desPath+"/"+Name);
    //if (!outfile.exists()) {
    {
        outfile.createNewFile();
        FileOutputStream out = new FileOutputStream(outfile);
        byte[] buffer = new byte[1024];
        InputStream in;
        int readLen = 0;
        if( srcPath != null )
            in = c.getAssets().open(srcPath+"/"+Name);
        else
            in = c.getAssets().open(Name);
        while((readLen = in.read(buffer)) != -1){
            out.write(buffer, 0, readLen);
        }
        out.flush();
        in.close();
        out.close();
    }
  }

  boolean CreatePath(String Path){
    File destCardDir = new File(Path);
    if(!destCardDir.exists()){
        int Index = Path.lastIndexOf(File.separator.charAt(0));
        if( Index < 0 ){
            if( destCardDir.mkdirs() == false )
                return false;
        }else{
            String ParentPath = Path.substring(0, Index);
            if( CreatePath(ParentPath) == false )
                return false;
            if( destCardDir.mkdirs() == false )
                return false;
        }
    }
    return true;
  }  

  long mainThreadID;

  Object g_WaitResultMap_mutexObject;
  Integer g_WaitResultMap_Index;
  HashMap<Integer,StarFlutWaitResult> g_WaitResultMap;

  Object mutexObject;
  boolean ExitAppFlag;  

  final int starcore_ThreadTick_Init = 0;
  final int starcore_ThreadTick_MessageID = 1;
  final int starcore_ThreadTick_MethodCall = 2;


  final int starcore_sendResult_MessageID = 0;
  final int starcore_msgCallBack_MessageID = 1;
  final int starobjecrclass_ScriptCallBack_MessageID = 2;

  public StarflutPlugin(Activity activity,MethodChannel in_channel,BasicMessageChannel in_messagechannel) {
      this.activity = activity;
      this.channel = in_channel;
      CleObjectMap = new HashMap<String,Object>();
      g_WaitResultMap = new HashMap<Integer,StarFlutWaitResult>();
      g_WaitResultMap_mutexObject = new Object();
      g_WaitResultMap_Index = 1;      

      mainThreadID = Thread.currentThread().getId();
      starCoreThreadCallDeep = 0;
      starCoreThreadCallDeepSyncObject = new Object();

      mainHandler = new android.os.Handler(Looper.getMainLooper()){
        //--this will be run in mainthread, and process callback from native to dart
        public void handleMessage(Message msg) {
          switch( msg.what ){
          case starcore_sendResult_MessageID :
          {
            Object[] plist = (Object[])msg.obj;
            Result result = (Result)plist[0];
            result.success(plist[1]);
            break;
          }
          case starcore_msgCallBack_MessageID:
            channel.invokeMethod("starcore_msgCallBack" ,msg.obj, new Result() {
              @Override
              public void success(Object res_o) {
                ArrayList<Object> plist = (ArrayList<Object>)res_o;
                Object o = (Object)plist.get(1);
                Integer w_tag = (Integer)plist.get(0);
                StarFlutWaitResult t_WaitResult = null;
                if( w_tag != 0 )
                  t_WaitResult = get_WaitResult(w_tag);
                else
                  t_WaitResult = null;
                if( o == null || (!(o instanceof ArrayList<?>)) ){
                    if( t_WaitResult != null )
                      t_WaitResult.SetResult(null); 
                    return;
                }
                if( t_WaitResult != null ){
                  ArrayList<Object> lr = (ArrayList<Object>)o;
                  if( lr.size() > 2 )
                    t_WaitResult.SetResult(new Object[]{lr.get(0),lr.get(1)});
                  else
                    t_WaitResult.SetResult(new Object[]{lr.get(0)});
                }else{
                  System.out.println(String.format("MsgCallBack can not return value"));
                  t_WaitResult.SetResult(new Object[]{null});
                }
                return;
              }                  
              @Override
              public void error(String s, String s1, Object o) {
                System.out.println(s + "    "+s1);
              }
            
              @Override
              public void notImplemented() {}
            }); 
            break;

          case starobjecrclass_ScriptCallBack_MessageID :
            channel.invokeMethod("starobjectclass_scriptproc" ,msg.obj, new Result() {
              @Override
              public void success(Object res_o) {
                ArrayList<Object> plist = (ArrayList<Object>)res_o;
                Object o = (Object)plist.get(2);
                Integer w_tag = (Integer)plist.get(0);
                if( w_tag != 0 ){
                  StarFlutWaitResult t_WaitResult = get_WaitResult(w_tag);
                  t_WaitResult.SetResult(o);
                }else{
                  if( o != null ){
                    System.out.println(String.format("Object function [%s] can not return value, for it is called in ui thread",(String)plist.get(1)));
                  }
                }
                return;
              }                  
              @Override
              public void error(String s, String s1, Object o) {
                System.out.println(s + "    "+s1);
              }
            
              @Override
              public void notImplemented() {}
            });           
            break;

          }
        }
    };

      StarCoreInitFlag = false;
      mutexObject = new Object();
      ExitAppFlag = false;      
  }  

  Integer get_WaitResultIndex()
  {
    synchronized (g_WaitResultMap_mutexObject)
    {
        Integer Index = g_WaitResultMap_Index;
        g_WaitResultMap_Index = g_WaitResultMap_Index + 1;
        if( g_WaitResultMap_Index == 0x7FFFFFFF)
            g_WaitResultMap_Index = 1;
        return Index;
    }      
  }

  StarFlutWaitResult new_WaitResult(Integer Index)
  {
    synchronized (g_WaitResultMap_mutexObject)
    {
      StarFlutWaitResult m_result = new StarFlutWaitResult();
      g_WaitResultMap.put(Index,m_result);
      return m_result;
    }  
  }

  StarFlutWaitResult get_WaitResult(Integer Index)
  {
    synchronized (g_WaitResultMap_mutexObject)
    {
      StarFlutWaitResult m_result = g_WaitResultMap.get(Index);
      return m_result;
    }  
  }  

  void remove_WaitResult(Integer Index)
  {
    synchronized (g_WaitResultMap_mutexObject)
    {
      g_WaitResultMap.remove(Index);
    }  
  }    

  /** Plugin registration. */
  public static void registerWith(Registrar registrar) {
    final MethodChannel channel = new MethodChannel(registrar.messenger(), "starflut");
    final BasicMessageChannel messagechannel = new BasicMessageChannel(registrar.messenger(),"starflut_message",new StandardMessageCodec());
    channel.setMethodCallHandler(new StarflutPlugin(registrar.activity(),channel,messagechannel));
  }

  Object[] processInputArgs(ArrayList<Object> parglist)
  {
    Object[] out = new Object[parglist.size()];
    for( int i=0; i < parglist.size(); i++ ){
      Object vv = parglist.get(i);
      if( vv instanceof String ){
        String starObjTag = (String)vv;
        if( starObjTag.startsWith(StarBinBufPrefix) || starObjTag.startsWith(StarParaPkgPrefix) || starObjTag.startsWith(StarObjectPrefix) || starObjTag.startsWith(StarServicePrefix) ){
          Object srcv = (Object)CleObjectMap.get(starObjTag);
          if( srcv == null ){
            System.out.println(String.format("object[%s] can not be found..",(String)starObjTag));
            return null;   
          } 
          out[i] = srcv;
        }else{             
          out[i] = vv;
        }   
      }else if(vv instanceof ArrayList<?> ){ 
        ArrayList<Object> s_l = (ArrayList<Object>)vv;
        Object[] s_o = processInputArgs(s_l);
        StarSrvGroupClass SrvGroup = starcore._GetSrvGroup(0);
        StarParaPkgClass ParaPkg = SrvGroup._NewParaPkg(); 
        for( int ii=0; ii < s_o.length; ii++ ){
          ParaPkg._Set(ii,s_o[ii]);
        }      
        out[i] = ParaPkg;     
      }else if(vv instanceof HashMap<?,?>){
        /*--we change this to parapkg, for set dict flag--*/
        HashMap<?,?> s_m = ( HashMap<?,?>)vv;
        ArrayList<Object> s_l = new ArrayList<Object>();
        for(Object key:s_m.keySet()){    
          s_l.add(key);
          s_l.add(s_m.get(key)); 
        }
        Object[] s_o = processInputArgs(s_l);
        StarSrvGroupClass SrvGroup = starcore._GetSrvGroup(0);
        StarParaPkgClass ParaPkg = SrvGroup._NewParaPkg(); 
        for( int ii=0; ii < s_o.length; ii++ ){
          ParaPkg._Set(ii,s_o[ii]);
        }      
        ParaPkg._AsDict(true);
        out[i] = ParaPkg;
      }else{
        out[i] = vv;              
      }      
    }
    return out;
  }

  ArrayList<Object> processOutputArgs(Object[] parglist)
  {
    ArrayList<Object> out = new ArrayList<Object>();
    for( int i=0; i < parglist.length; i++ ){
      Object vv = parglist[i];
      if( vv instanceof StarObjectClass ){
        /*
        starcore._SRPLock();       
        StarParaPkgClass Result = ((StarObjectClass)vv)._RawToParaPkg();
        starcore._SRPUnLock();
        */
        StarParaPkgClass Result = null;
        if( Result != null ){
          ArrayList<Object> ll = processOutputArgs(new Object[]{Result}); 
          out.add(ll.get(0));
        }else{
          starcore._SRPLock(); 
          String StarObjectID = (String)((StarObjectClass)vv)._Get("_ID");
          starcore._SRPUnLock();
          String CleObjectID = StarObjectPrefix+UUID.randomUUID().toString();
          CleObjectMap.put(CleObjectID,vv);
          out.add(CleObjectID + "+" + StarObjectID);
        }
      }else if( vv instanceof StarParaPkgClass ){
        StarParaPkgClass ParaPkg = (StarParaPkgClass)vv;
        starcore._SRPLock();
        int Number = ParaPkg._GetInt("_Number");
        Object[] res = new Object[Number];
        for( int ii=0; ii < Number; ii++ ){
          res[ii] = ParaPkg._Get(ii);
        }
        if( ParaPkg._IsDict() == false || (Number % 2) != 0 ){
          ArrayList<Object> al = processOutputArgs(res);
          out.add(al);
        }else{
          ArrayList<Object> al = processOutputArgs(res);
          HashMap<Object,Object> hm = new HashMap<Object,Object>();
          for( int ii=0; ii < Number / 2 ; ii++ ){
            hm.put(al.get(2*ii),al.get(2*ii+1));
          }
          out.add(hm);
        }
        starcore._SRPUnLock();
      }else if( vv instanceof StarBinBufClass ){
        String CleObjectID = StarBinBufPrefix+UUID.randomUUID().toString();
        CleObjectMap.put(CleObjectID,vv);
        out.add(CleObjectID);  
      }else if( vv instanceof StarServiceClass ){  
        starcore._SRPLock();
        String ServiceID = (String)((StarServiceClass)vv)._Get("_ID");
        starcore._SRPUnLock();
        String CleObjectID = StarServicePrefix+UUID.randomUUID().toString();
        CleObjectMap.put(CleObjectID,vv);       
        out.add(CleObjectID + "+" + ServiceID);              
      }else if( vv instanceof Object[] ){
        Object[] s_l = (Object[])vv;
        ArrayList<Object> s_o = processOutputArgs(s_l);
        out.add(s_o); 
      }else{      
        if( vv instanceof Boolean){
          if( (Boolean)vv == true )
            out.add(StarValueBooleanPrefix_TRUE);
          else
            out.add(StarValueBooleanPrefix_FALSE);  /*fix the bug for return boolean value in arraylist or hashmap-*/
        } else{   
          out.add(vv);
        }
      }         
    }
    return out;
  }

  Object processInputListToArray(Object val)
  {
    if( val instanceof ArrayList<?> ){
      ArrayList<Object> s_l = (ArrayList<Object>)val;
      Object[] s_o = processInputArgs(s_l);
      StarSrvGroupClass SrvGroup = starcore._GetSrvGroup(0);
      StarParaPkgClass ParaPkg = SrvGroup._NewParaPkg(); 
      for( int ii=0; ii < s_o.length; ii++ ){
        ParaPkg._Set(ii,s_o[ii]);
      }      
      return ParaPkg;
    }else if( val instanceof HashMap<?,?> ){
      /*--we change this to parapkg, for set dict flag--*/
        HashMap<?,?> s_m = ( HashMap<?,?>)val;
        ArrayList<Object> s_l = new ArrayList<Object>();
        for(Object key:s_m.keySet()){    
          s_l.add(key);
          s_l.add(s_m.get(key)); 
        }
        Object[] s_o = processInputArgs(s_l);
        StarSrvGroupClass SrvGroup = starcore._GetSrvGroup(0);
        StarParaPkgClass ParaPkg = SrvGroup._NewParaPkg(); 
        for( int ii=0; ii < s_o.length; ii++ ){
          ParaPkg._Set(ii,s_o[ii]);
        }      
        ParaPkg._AsDict(true);
        return ParaPkg;
    }else{
      return val;
    }
  }

  String getTagFromObjectMap(Object v)
  {
    Iterator entries = CleObjectMap.entrySet().iterator(); 
    while (entries.hasNext()) { 
      Map.Entry entry = (Map.Entry) entries.next(); 
      if( entry.getValue().equals(v)){
        return (String)entry.getKey();
      }
    }
    return null;
  }

  void removeFromObjectMap(Object v)
  {
    ArrayList<String> kk = new ArrayList<String>();
    Iterator entries = CleObjectMap.entrySet().iterator(); 
    while (entries.hasNext()) { 
      Map.Entry entry = (Map.Entry) entries.next(); 
      if( entry.getValue().equals(v)){
        kk.add((String)entry.getKey());
      }
    }
    for(int i=0; i < kk.size(); i ++ ){
      CleObjectMap.remove(kk.get(i));
    }
    return;
  }  

  /*https://docs.flutter.io/flutter/services/MethodCall-class.html */

  @Override
  public void onMethodCall(MethodCall call, Result result) {
    switch(call.method){
    /*--direct call--*/
    case "starcore_nativeLibraryDir" :
    case "starcore_getDocumentPath" :
    case "starcore_getResourcePath" :
    case "starcore_getPackageName" :
    case "starcore_unzipFromAssets" :
    case "starcore_copyFileFromAssets" :
    case "starcore_loadLibrary" :
    case "starcore_isAndroid" :

    case "starcore_init" :
    case "starcore_sRPDispatch":
    case "starcore_sRPLock" :
    case "starcore_sRPUnLock" : 
      onMethodCall_Do_Direct(call,result);
      break;
    /*--send message to main starcore thread--*/
    default:
      synchronized(starCoreThreadCallDeepSyncObject){
        if( starCoreThreadCallDeep != 0 ){
          System.out.println("call starflut function ["+call.method+"] failed, current is in starcore thread process");
        }
      }    
      Message message1 = starCoreHandler.obtainMessage();
      message1.what = starcore_ThreadTick_MethodCall;
      message1.obj = new Object[]{call,result};
      starCoreHandler.sendMessage(message1);   
      break;   
    }
  }

  public void onMethodCall_Do_Direct(MethodCall call, Result result) {
    switch(call.method){
    case "starcore_nativeLibraryDir" :{
      result.success(activity.getApplicationInfo().nativeLibraryDir);
      }
      break;
    case "starcore_getDocumentPath" :{
      result.success("/data/data/"+activity.getPackageName()+"/files");
      }
      break;      
    case "starcore_getResourcePath" :{
      result.success("/data/data/"+activity.getPackageName()+"/files");
      }
      break; 
    case "starcore_getPackageName" :{
      result.success(activity.getPackageName());
      }
      break;   
    case "starcore_unzipFromAssets" :{
        ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
        try {
          AssetManager assetManager = activity.getAssets();
          InputStream dataSource = assetManager.open((String)plist.get(0));
          boolean Result = unzip(dataSource,(String)plist.get(1),(boolean)plist.get(2) );
          dataSource.close();
          result.success(Result);
        }
        catch(Exception e)
        {
          System.out.println(e.toString());
          result.success(false);
        }                  
      }
      break;       
    case "starcore_copyFileFromAssets" :{
        ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
        try {
          String SrcPath = (String)plist.get(1);
          String DesPath = (String)plist.get(2);
          if( DesPath != null && DesPath.length() != 0 ){
            DesPath = "/data/data/"+activity.getPackageName()+"/files/"+DesPath;
            CreatePath(DesPath);
          }else{
            DesPath = "/data/data/"+activity.getPackageName()+"/files";
            CreatePath(DesPath);
          }
          copyFile(activity,(String)plist.get(0),SrcPath,DesPath);
          result.success(true);
        }
        catch(Exception e)
        {
          System.out.println(e.toString());
          result.success(false);
        }                  
      }
      break; 
    case "starcore_loadLibrary" :{
        ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
        try{
          System.load(activity.getApplicationInfo().nativeLibraryDir+"/"+(String)plist.get(0));
          result.success(true);
        }
        catch(UnsatisfiedLinkError ex)
        {
          System.out.println(ex.toString());
          result.success(false);
        }                
      }
      break;    
    case "starcore_isAndroid" :{
        result.success(true);
      }
      break;                      
    case "starcore_init" :{
        //----create new thread and init starcore----
        final Integer w_tag = get_WaitResultIndex();
        StarFlutWaitResult m_WaitResult = new_WaitResult(w_tag);

        starCoreHandlerThread = new Thread(new Runnable(){
          @Override
          public void run() {
            /*--thread message loop--*/
            Looper.prepare();
            starCoreHandlerLooper = Looper.myLooper();
            starCoreHandler = new android.os.Handler(){
              //--this will be run in mainthread, and process callback from native to dart
              public void handleMessage(Message msg) {
                switch( msg.what ){
                case starcore_ThreadTick_MessageID:
                  if( starcore == null ){
                    break; /*--starcore not init--*/
                  }
                  starcore._SRPLock();  
                  while (starcore._SRPDispatch(false) == true) ;
                  starcore._SRPUnLock();
                  break;
                case starcore_ThreadTick_MethodCall :
                  {
                    /*--object : [MethodCall,Result]*/
                    Object[] args = (Object[])msg.obj;
                    synchronized(starCoreThreadCallDeepSyncObject){
                      starCoreThreadCallDeep = starCoreThreadCallDeep + 1;  
                    }
                    Object result = onMethodCall_Do((MethodCall)args[0]);
                    synchronized(starCoreThreadCallDeepSyncObject){
                      starCoreThreadCallDeep = starCoreThreadCallDeep - 1;  
                    }                    
                    Message message1 = mainHandler.obtainMessage();
                    message1.what = starcore_sendResult_MessageID;
                    message1.obj = new Object[]{args[1],result};
                    mainHandler.sendMessage(message1);
                  }
                  break;                                                                
                }
              }
            };
            /*--create tick thread--*/
            new Thread(new Runnable(){
              @Override
              public void run() {
                while (true)
                {
                  synchronized (mutexObject)
                  {
                      if (ExitAppFlag == true){
                        starCoreHandlerLooper.quit();
                        break;
                      }
                  }                  
                  try {
                    Thread.sleep(5);
                    Message message1 = starCoreHandler.obtainMessage();
                    message1.what = starcore_ThreadTick_MessageID;
                    message1.obj = null;
                    starCoreHandler.sendMessage(message1);                    
                  }
                  catch(Exception ex)
                  {
                  }
                }
              }
            }).start(); 
            
            StarFlutWaitResult t_WaitResult = get_WaitResult(w_tag);
            t_WaitResult.SetResult(true);  

            Looper.loop();           
          }
        });
        starCoreHandlerThread.start();     

        m_WaitResult.WaitResult();
        remove_WaitResult(w_tag);

        Message message1 = starCoreHandler.obtainMessage();
        message1.what = starcore_ThreadTick_MethodCall;
        message1.obj = new Object[]{call,result};
        starCoreHandler.sendMessage(message1);              
      }
      break;

    case "starcore_sRPDispatch" : /*starcore_sRPDispatch*/
          {
            starcore._SRPLock();
            boolean Result = starcore._SRPDispatch((boolean)call.arguments);
            starcore._SRPUnLock();
            result.success(Result);                   
          }   
          break;
    case "starcore_sRPLock" : /*starcore_sRPLock*/
          {
            starcore._SRPLock();
            result.success(true);                   
          }
          break;
    case "starcore_sRPUnLock" : /*starcore_sRPUnLock*/
          {
            starcore._SRPUnLock();
            result.success(true);                   
          }        
          break;
    }
  }

  public Object onMethodCall_Do(MethodCall call) {
    switch(call.method){
    case "starcore_init" :{  
        StarCoreFactoryPath.StarCoreCoreLibraryPath = activity.getApplicationInfo().nativeLibraryDir;
        StarCoreFactoryPath.StarCoreShareLibraryPath = activity.getApplicationInfo().nativeLibraryDir;
        StarCoreFactoryPath.StarCoreOperationPath = "/data/data/"+activity.getPackageName()+"/files";

        starcore = StarCoreFactory.GetFactory();
        starcore._SRPUnLock(); //--release global lock--

        String CleObjectID = UUID.randomUUID().toString();
        CleObjectMap.put(CleObjectID,starcore);
        return CleObjectID;        
      }        

    case "starcore_getActivity" :{
        ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
        StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
        if( l_Service == null ){
          System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
          return null;    
        }      
        starcore._SRPLock();
        StarObjectClass l_StarObject = l_Service._New();
        l_StarObject._AttachRawObject(activity,false);
        String StarObjectID = (String)l_StarObject._Get("_ID");
        starcore._SRPUnLock();        
        String CleObjectID = StarObjectPrefix+UUID.randomUUID().toString();
        CleObjectMap.put(CleObjectID,l_StarObject);              
        return CleObjectID + "+" + StarObjectID;   
      }     
    case "starcore_poplocalframe" :{
        ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
        for(int i=0; i < plist.size(); i++ ){
          if( CleObjectMap.containsKey((String)plist.get(i)))
              CleObjectMap.remove((String)plist.get(i));
        }
        return null;
      }
    case "starcore_InitCore" :{
        ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
        starcore._SRPLock();
        int Result = starcore._InitCore((boolean)plist.get(0),(boolean)plist.get(1),(boolean)plist.get(2),(boolean)plist.get(3),(String)plist.get(4),(int)plist.get(5),(String)plist.get(6),(int)plist.get(7));
        starcore._SRPUnLock();
        if( Result >= 0 ){
          StarCoreInitFlag = true;
           /*--register dispatch request--*/
          starcore._RegDispatchRequest_P(new StarDispatchRequestCallBackInterface(){    
            public void Invoke()
            {        
              Message message1 = starCoreHandler.obtainMessage();
              message1.what = starcore_ThreadTick_MessageID;
              message1.obj = null;
              starCoreHandler.sendMessage(message1);     
              return;
            }
          });
        }
        return Result;
      }
    case "starcore_InitSimpleEx" :{
        ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
        StarSrvGroupClass l_SrvGroup = null;
        starcore._SRPLock();
        switch(plist.size() - 2){
        case 0: l_SrvGroup = starcore._InitSimpleEx((int)plist.get(0),(int)plist.get(1)); break;
        case 1: l_SrvGroup = starcore._InitSimpleEx((int)plist.get(0),(int)plist.get(1),(String)plist.get(2)); break;        
        case 2: l_SrvGroup = starcore._InitSimpleEx((int)plist.get(0),(int)plist.get(1),(String)plist.get(2),(String)plist.get(3)); break;        
        case 3: l_SrvGroup = starcore._InitSimpleEx((int)plist.get(0),(int)plist.get(1),(String)plist.get(2),(String)plist.get(3),(String)plist.get(4)); break;        
        case 4: l_SrvGroup = starcore._InitSimpleEx((int)plist.get(0),(int)plist.get(1),(String)plist.get(2),(String)plist.get(3),(String)plist.get(4),(String)plist.get(5)); break;        
        case 5: l_SrvGroup = starcore._InitSimpleEx((int)plist.get(0),(int)plist.get(1),(String)plist.get(2),(String)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6)); break;        
        case 6: l_SrvGroup = starcore._InitSimpleEx((int)plist.get(0),(int)plist.get(1),(String)plist.get(2),(String)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6),(String)plist.get(7)); break;        
        case 7: l_SrvGroup = starcore._InitSimpleEx((int)plist.get(0),(int)plist.get(1),(String)plist.get(2),(String)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6),(String)plist.get(7),(String)plist.get(8)); break;        
        case 8: l_SrvGroup = starcore._InitSimpleEx((int)plist.get(0),(int)plist.get(1),(String)plist.get(2),(String)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6),(String)plist.get(7),(String)plist.get(8),(String)plist.get(9)); break;        
        case 9: l_SrvGroup = starcore._InitSimpleEx((int)plist.get(0),(int)plist.get(1),(String)plist.get(2),(String)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6),(String)plist.get(7),(String)plist.get(8),(String)plist.get(9),(String)plist.get(10)); break;        
        case 10: l_SrvGroup = starcore._InitSimpleEx((int)plist.get(0),(int)plist.get(1),(String)plist.get(2),(String)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6),(String)plist.get(7),(String)plist.get(8),(String)plist.get(9),(String)plist.get(10),(String)plist.get(11)); break;        
        default: break;
        }        
        starcore._SRPUnLock();       
        if( l_SrvGroup != null ){
          StarCoreInitFlag = true;
          //--register dispatch request--
          starcore._RegDispatchRequest_P(new StarDispatchRequestCallBackInterface(){    
            public void Invoke()
            {        
              Message message1 = starCoreHandler.obtainMessage();
              message1.what = starcore_ThreadTick_MessageID;
              message1.obj = null;
              starCoreHandler.sendMessage(message1);     
              return;
            }
          });          
          String CleObjectID = getTagFromObjectMap(l_SrvGroup);
          if( CleObjectID == null ){
            CleObjectID = StarSrvGroupPrefix+UUID.randomUUID().toString();
            CleObjectMap.put(CleObjectID,l_SrvGroup);
          }
          return CleObjectID;          
        }else
          return null;      
      }
    case "starcore_InitSimple" : {
        ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
        StarServiceClass l_Service = null;
        starcore._SRPLock();
        switch(plist.size() - 4){
        case 0: l_Service = starcore._InitSimple((String)plist.get(0),(String)plist.get(1),(int)plist.get(2),(int)plist.get(3)); break;
        case 1: l_Service = starcore._InitSimple((String)plist.get(0),(String)plist.get(1),(int)plist.get(2),(int)plist.get(3),(String)plist.get(4)); break;        
        case 2: l_Service = starcore._InitSimple((String)plist.get(0),(String)plist.get(1),(int)plist.get(2),(int)plist.get(3),(String)plist.get(4),(String)plist.get(5)); break;        
        case 3: l_Service = starcore._InitSimple((String)plist.get(0),(String)plist.get(1),(int)plist.get(2),(int)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6)); break;        
        case 4: l_Service = starcore._InitSimple((String)plist.get(0),(String)plist.get(1),(int)plist.get(2),(int)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6),(String)plist.get(7)); break;        
        case 5: l_Service = starcore._InitSimple((String)plist.get(0),(String)plist.get(1),(int)plist.get(2),(int)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6),(String)plist.get(7),(String)plist.get(8)); break;        
        case 6: l_Service = starcore._InitSimple((String)plist.get(0),(String)plist.get(1),(int)plist.get(2),(int)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6),(String)plist.get(7),(String)plist.get(8),(String)plist.get(9)); break;        
        case 7: l_Service = starcore._InitSimple((String)plist.get(0),(String)plist.get(1),(int)plist.get(2),(int)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6),(String)plist.get(7),(String)plist.get(8),(String)plist.get(9),(String)plist.get(10)); break;        
        case 8: l_Service = starcore._InitSimple((String)plist.get(0),(String)plist.get(1),(int)plist.get(2),(int)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6),(String)plist.get(7),(String)plist.get(8),(String)plist.get(9),(String)plist.get(10),(String)plist.get(11)); break;        
        case 9: l_Service = starcore._InitSimple((String)plist.get(0),(String)plist.get(1),(int)plist.get(2),(int)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6),(String)plist.get(7),(String)plist.get(8),(String)plist.get(9),(String)plist.get(10),(String)plist.get(11),(String)plist.get(12)); break;        
        case 10: l_Service = starcore._InitSimple((String)plist.get(0),(String)plist.get(1),(int)plist.get(2),(int)plist.get(3),(String)plist.get(4),(String)plist.get(5),(String)plist.get(6),(String)plist.get(7),(String)plist.get(8),(String)plist.get(9),(String)plist.get(10),(String)plist.get(11),(String)plist.get(12),(String)plist.get(13)); break;        
        default: break;
        }               
        if( l_Service != null ){
          String ServiceID = (String)l_Service._Get("_ID");
          starcore._SRPUnLock();
          StarCoreInitFlag = true;
          /*--register dispatch request--*/
          starcore._RegDispatchRequest_P(new StarDispatchRequestCallBackInterface(){    
            public void Invoke()
            {        
              Message message1 = starCoreHandler.obtainMessage();
              message1.what = starcore_ThreadTick_MessageID;
              message1.obj = null;
              starCoreHandler.sendMessage(message1);     
              return;
            }
          });          
          String CleObjectID = StarServicePrefix+UUID.randomUUID().toString();
          CleObjectMap.put(CleObjectID,l_Service);
          return CleObjectID + "+" + ServiceID;          
        }else{
          starcore._SRPUnLock();
          return null; 
        }              
      }
    case "starcore_GetSrvGroup" : /*starcore_GetSrvGroup*/
          {
            StarSrvGroupClass l_SrvGroup = null; 
            starcore._SRPLock();
            l_SrvGroup = starcore._GetSrvGroup(call.arguments);
            starcore._SRPUnLock();
            if( l_SrvGroup != null ){
              String CleObjectID = getTagFromObjectMap(l_SrvGroup);
              if( CleObjectID == null ){
                CleObjectID = StarSrvGroupPrefix+UUID.randomUUID().toString();
                CleObjectMap.put(CleObjectID,l_SrvGroup);
              }              
              return CleObjectID;          
            }else
              return null;             
          }
    case "starcore_moduleExit" : /*starcore_moduleExit*/
          {
            StarCoreInitFlag = false;
            synchronized (mutexObject)
            {
                ExitAppFlag = true;
            }             
            starcore._SRPLock();
            starcore._ModuleExit();  
            CleObjectMap.clear();          
            return true;                         
          }  
    case "starcore_moduleClear" : /*starcore_moduleClear*/
          {
            StarCoreInitFlag = false;
            starcore._SRPLock();
            starcore._ModuleClear();
            starcore._SRPUnLock();
            CleObjectMap.clear();
            return true;                                  
          }
    case "starcore_regMsgCallBackP" : /*starcore_regMsgCallBackP*/
          {
            starcore._SRPLock();
            if( (boolean)call.arguments == false ){
              starcore._RegMsgCallBack_P(null);
            }else{
              starcore._RegMsgCallBack_P(new StarMsgCallBackInterface(){    
                public Object Invoke( int ServiceGroupID, final int uMes, Object wParam, Object lParam)
                {        
                  final Integer w_tag = get_WaitResultIndex();
                  StarFlutWaitResult m_WaitResult = new_WaitResult(w_tag);

                  ArrayList<Object> cP = new ArrayList<Object>();
                  cP.add(ServiceGroupID);
                  cP.add(uMes);
                  cP.add(wParam);
                  cP.add(lParam);   
                  cP.add(w_tag);
                  
                  Message message1 = mainHandler.obtainMessage();
                  message1.what = starcore_msgCallBack_MessageID;
                  message1.obj = cP;
                  mainHandler.sendMessage(message1);

                  starcore._SRPUnLock();
                  Object result = m_WaitResult.WaitResult();
                  remove_WaitResult(w_tag);
                  starcore._SRPLock();
                  return result;       
                }
              }); 
            }
            starcore._SRPUnLock();
            return 0;                   
          }    
/*
    case "starcore_regDispatchRequestP" : //starcore_regDispatchRequestP
          {
            starcore._SRPLock();
            if( (boolean)call.arguments == false ){
              starcore._RegDispatchRequest_P(null);
            }else{
              starcore._RegDispatchRequest_P(new StarDispatchRequestCallBackInterface(){    
                public void Invoke()
                {        
                  channel.invokeMethod("starcore_dispatchRequestCallBack" ,null);
                  return;
                }
              }); 
            }
            starcore._SRPUnLock();
            return 0;                   
          }    
*/          
    case "starcore_setRegisterCode" : /*starcore_setRegisterCode*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            starcore._SRPLock();
            boolean Result = starcore._SetRegisterCode((String)plist.get(0),(boolean)plist.get(1));
            starcore._SRPUnLock();
            return Result;                   
          }            
    case "starcore_isRegistered" : /*starcore_isRegistered*/
          {
            starcore._SRPLock();
            boolean Result = starcore._IsRegistered();
            starcore._SRPUnLock();
            return Result;                   
          }   
    case "starcore_setLocale" : /*starcore_setLocale*/
          {
            starcore._SRPLock();
            starcore._SetLocale((String)call.arguments);
            starcore._SRPUnLock();
            return true;                   
          }           
    case "starcore_getLocale" : /*starcore_getLocale*/
          {
            starcore._SRPLock();
            String Result = starcore._GetLocale();
            starcore._SRPUnLock();
            return Result;                   
          }
    case "starcore_version" : /*starcore_version*/
          {
            starcore._SRPLock();
            int[] Result = starcore._Version();
            starcore._SRPUnLock();
            return  processOutputArgs(new Object[]{Result[0],Result[1],Result[2]});                   
          }                                                                         
    case "starcore_getScriptIndex" : /*starcore_getScriptIndex*/
          {
            starcore._SRPLock();
            int Result = starcore._GetScriptIndex((String)call.arguments);
            starcore._SRPUnLock();
            return Result;                   
          }  
    case "starcore_setScript" : /*starcore_setScript*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            starcore._SRPLock();
            boolean Result = starcore._SetScript((String)plist.get(0),(String)plist.get(1),(String)plist.get(2));
            starcore._SRPUnLock();
            return Result;                   
          }
    case "starcore_detachCurrentThread" : /*starcore_detachCurrentThread*/
          {
            starcore._SRPLock();
            starcore._DetachCurrentThread();
            starcore._SRPUnLock();
            return true;                   
          }
    case "starcore_coreHandle" : /*starcore_coreHandle*/
          {
            starcore._SRPLock();
            long Result = starcore._CoreHandle();
            starcore._SRPUnLock();
            return Result;                   
          }
          
    /*-----------------------------------------------*/
    case "StarSrvGroupClass_toString" : /*StarSrvGroupClass_toString*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return "";   
            }
            return ""+l_SrvGroup;          
          }

    case "StarSrvGroupClass_createService" : /*StarSrvGroupClass_createService*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return null;   
            }
            StarServiceClass l_Service = null;
            starcore._SRPLock();
            //[this.starTag,servicePath,serviceName,rootPass,frameInterval,netPkgSize,uploadPkgSize,downloadPkgSize,dataUpPkgSize,dataDownPkgSize,derviceID]);          
            l_Service = l_SrvGroup._CreateService((String)plist.get(1),(String)plist.get(2),(String)plist.get(3),(int)plist.get(4),(int)plist.get(5),(int)plist.get(6),(int)plist.get(7),(int)plist.get(8),(int)plist.get(9),(String)plist.get(10));            
            if( l_Service != null ){
              String ServiceID = (String)l_Service._Get("_ID");
              starcore._SRPUnLock();
              String CleObjectID = StarServicePrefix+UUID.randomUUID().toString();
              CleObjectMap.put(CleObjectID,l_Service);
              return CleObjectID + "+" + ServiceID;          
            }else{
              starcore._SRPUnLock();
              return null;
            }                 
          }
    case "StarSrvGroupClass_getService" : /*StarSrvGroupClass_getService*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            StarServiceClass l_Service = null;
            starcore._SRPLock();
            l_Service = l_SrvGroup._GetService((String)plist.get(1),(String)plist.get(2));            
            if( l_Service != null ){
              String ServiceID = (String)l_Service._Get("_ID");
              starcore._SRPUnLock();
              String CleObjectID = StarServicePrefix+UUID.randomUUID().toString();
              CleObjectMap.put(CleObjectID,l_Service);
              return CleObjectID+"+"+ServiceID;          
            }else{
              starcore._SRPUnLock();
              return null; 
            }                
          }  
    case "StarSrvGroupClass_clearService" : /*StarSrvGroupClass_clearService*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            starcore._SRPLock();
            l_SrvGroup._ClearService();
            starcore._SRPUnLock();
            return null;                 
          }
    case "StarSrvGroupClass_newParaPkg" : /*StarSrvGroupClass_newParaPkg*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return null;
            }
            Object vv = plist.get(1);
            StarParaPkgClass l_ParaPkg = null;
            if( vv instanceof ArrayList<?> ){
              ArrayList<Object> parglist = (ArrayList<Object>)plist.get(1);
              starcore._SRPLock();
              Object[] pp = processInputArgs(parglist);              
              l_ParaPkg = l_SrvGroup._NewParaPkg(pp);
              starcore._SRPUnLock();
            }else if( vv instanceof HashMap<?,?> ){
              HashMap<?,?> s_m = ( HashMap<?,?>)vv;
              ArrayList<Object> parglist = new ArrayList<Object>();
              for(Object key:s_m.keySet()){    
                parglist.add(key);
                parglist.add(s_m.get(key)); 
              }
              starcore._SRPLock();
              Object[] pp = processInputArgs(parglist);                            
              l_ParaPkg = l_SrvGroup._NewParaPkg(pp);
              if( l_ParaPkg != null )
                l_ParaPkg._AsDict(true);
              starcore._SRPUnLock();
            }
            if( l_ParaPkg != null ){
              String CleObjectID = StarParaPkgPrefix+UUID.randomUUID().toString();
              CleObjectMap.put(CleObjectID,l_ParaPkg);
              return CleObjectID;          
            }else
              return null;                 
          } 
          
    case "StarSrvGroupClass_newBinBuf" : /*StarSrvGroupClass_newBinBuf*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return null; 
            }
            StarBinBufClass l_BinBuf = null;
            starcore._SRPLock();
            l_BinBuf = l_SrvGroup._NewBinBuf();
            starcore._SRPUnLock();
            if( l_BinBuf != null ){
              String CleObjectID = StarBinBufPrefix+UUID.randomUUID().toString();
              CleObjectMap.put(CleObjectID,l_BinBuf);
              return CleObjectID;          
            }else
              return null;                 
          }    
/*          
    case "StarSrvGroupClass_newSXml" : //StarSrvGroupClass_newSXml
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return null;  
            }
            StarSXmlClass l_SXml = null;
            starcore._SRPLock();
            l_SXml = l_SrvGroup._NewSXml();
            starcore._SRPUnLock();
            if( l_SXml != null ){
              String CleObjectID = StarSXmlPrefix+UUID.randomUUID().toString();
              CleObjectMap.put(CleObjectID,l_SXml);
              return CleObjectID;          
            }else
              return null;                 
          }
*/          
    case "StarSrvGroupClass_getServicePath" : /*StarSrvGroupClass_getServicePath*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return "";  
            }
            starcore._SRPLock();
            String Result = l_SrvGroup._GetServicePath();
            starcore._SRPUnLock();
            return Result;                 
          } 
    case "StarSrvGroupClass_setServicePath" : /*StarSrvGroupClass_setServicePath*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return null;   
            }
            starcore._SRPLock();
            l_SrvGroup._SetServicePath((String)plist.get(1));
            starcore._SRPUnLock();
            return true;                 
          }
    case "StarSrvGroupClass_servicePathIsSet" : /*StarSrvGroupClass_servicePathIsSet*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return false;    
            }
            starcore._SRPLock();
            boolean Result = l_SrvGroup._ServicePathIsSet();
            starcore._SRPUnLock();
            return Result;                 
          } 
    case "StarSrvGroupClass_getCurrentPath" : /*StarSrvGroupClass_getCurrentPath*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return "";
            }
            starcore._SRPLock();
            String Result = l_SrvGroup._GetCurrentPath();
            starcore._SRPUnLock();
            return Result;                 
          }     
          
    case "StarSrvGroupClass_importService" : /*StarSrvGroupClass_importService*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return false;
            }
            starcore._SRPLock();
            boolean Result = l_SrvGroup._ImportService((String)plist.get(1),(boolean)plist.get(2));
            starcore._SRPUnLock();
            return Result;                 
          }  
          
    case "StarSrvGroupClass_clearServiceEx" : /*StarSrvGroupClass_clearServiceEx*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return null;
            }
            starcore._SRPLock();
            l_SrvGroup._ClearServiceEx();
            starcore._SRPUnLock();
            return null;                 
          }
          
    case "StarSrvGroupClass_runScript" : /*StarSrvGroupClass_runScript*/
          {
              ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
              StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
              if( l_SrvGroup == null ){
                System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
                return false; 
              }
              starcore._SRPLock();
              boolean Result = l_SrvGroup._RunScript((String)plist.get(1),(String)plist.get(2),(String)plist.get(3));
              starcore._SRPUnLock();
              if( Result == false ){
                System.out.println(String.format("call runScript failed, [%s]",plist.get(1)));
              }                  
              return Result;              
          }     
    case "StarSrvGroupClass_runScriptEx" : /*StarSrvGroupClass_runScriptEx*/
          {           
              ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
              StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
              if( l_SrvGroup == null ){
                System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
                return false;  
              }
              StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(2));
              if( l_BinBuf == null ){
                System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(2)));
                return false;
              }               
              starcore._SRPLock();
              boolean Result = l_SrvGroup._RunScriptEx((String)plist.get(1),l_BinBuf,(String)plist.get(3));
              starcore._SRPUnLock();
              if( Result == false ){
                System.out.println(String.format("call runScriptEx failed, [%s]",plist.get(1)));
              }                  
              return Result;                 
          }
    case "StarSrvGroupClass_doFile" : /*StarSrvGroupClass_doFile*/
          {           
              ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
              StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
              if( l_SrvGroup == null ){
                System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
                return false;  
              }           
              starcore._SRPLock();
              boolean Result = l_SrvGroup._DoFile((String)plist.get(1),(String)plist.get(2));
              starcore._SRPUnLock();
              if( Result == false ){
                System.out.println(String.format("call doFile failed, [%s:%s]",plist.get(1),(String)plist.get(2)));
              } 
              return Result;                 
          }  
    case "StarSrvGroupClass_doFileEx" : /*StarSrvGroupClass_doFileEx*/
          {           
              ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
              StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
              if( l_SrvGroup == null ){
                System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
                return false;  
              }           
              starcore._SRPLock();
              boolean Result = l_SrvGroup._DoFileEx((String)plist.get(1),(String)plist.get(2),(String)plist.get(3));
              starcore._SRPUnLock();
              if( Result == false ){
                System.out.println(String.format("call doFileEx failed, [%s:%s]",plist.get(1),(String)plist.get(2)));
              }               
              return Result;                 
          }                            
    case "StarSrvGroupClass_setClientPort" : /*StarSrvGroupClass_setClientPort*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return false; 
            }
            starcore._SRPLock();
            boolean Result = l_SrvGroup._SetClientPort((String)plist.get(1),(int)plist.get(2));
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarSrvGroupClass_setTelnetPort" : /*StarSrvGroupClass_setTelnetPort*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return false;   
            }
            starcore._SRPLock();
            boolean Result = l_SrvGroup._SetTelnetPort((int)plist.get(1));
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarSrvGroupClass_setOutputPort" : /*StarSrvGroupClass_setOutputPort*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return false;
            }
            starcore._SRPLock();
            boolean Result = l_SrvGroup._SetOutputPort((String)plist.get(1),(int)plist.get(2));
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarSrvGroupClass_setWebServerPort" : /*StarSrvGroupClass_setWebServerPort*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return false; 
            }
            starcore._SRPLock();
            boolean Result = l_SrvGroup._SetWebServerPort((String)plist.get(1),(int)plist.get(2),(int)plist.get(3),(int)plist.get(4));
            starcore._SRPUnLock();
            return Result;                 
          }                     
    case "StarSrvGroupClass_initRaw" : /*StarSrvGroupClass_initRaw*/
          {           
              ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
              StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
              if( l_SrvGroup == null ){
                System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
                return false;  
              }           
              StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(2));
              if( l_Service == null ){
                System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(2)));
                return false;  
              }               
              starcore._SRPLock();
              boolean Result = l_SrvGroup._InitRaw((String)plist.get(1),l_Service);
              starcore._SRPUnLock();              
              return Result;                 
          }
    case "StarSrvGroupClass_loadRawModule" : /*StarSrvGroupClass_loadRawModule*/
          {           
              ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
              StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
              if( l_SrvGroup == null ){
                System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
                return false;  
              }                         
              starcore._SRPLock();
              boolean Result = l_SrvGroup._LoadRawModule((String)plist.get(1),(String)plist.get(2),(String)plist.get(3),(boolean)plist.get(4));
              starcore._SRPUnLock();             
              return Result;                 
          }
    case "StarSrvGroupClass_getLastError" : /*StarSrvGroupClass_getLastError*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return 0;
            }
            starcore._SRPLock();
            int Result = l_SrvGroup._GetLastError();
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarSrvGroupClass_getLastErrorInfo" : /*StarSrvGroupClass_getLastErrorInfo*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return "";
            }
            starcore._SRPLock();
            String Result = l_SrvGroup._GetLastErrorInfo();
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarSrvGroupClass_getCorePath" : /*StarSrvGroupClass_getCorePath*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return "";
            }
            starcore._SRPLock();
            String Result = l_SrvGroup._GetCorePath();
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarSrvGroupClass_getUserPath" : /*StarSrvGroupClass_getUserPath*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return "";
            }
            starcore._SRPLock();
            String Result = l_SrvGroup._GetUserPath();
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarSrvGroupClass_getLocalIP" : /*StarSrvGroupClass_getLocalIP*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return "127.0.0.1";
            }
            starcore._SRPLock();
            String Result = l_SrvGroup._GetLocalIP();
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarSrvGroupClass_getLocalIPEx" : /*StarSrvGroupClass_getLocalIPEx*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return processOutputArgs(new Object[]{"127.0.0.1"});
            }
            starcore._SRPLock();
            String[] Result = l_SrvGroup._GetLocalIPEx();
            starcore._SRPUnLock();
            return processOutputArgs(Result);                 
          }         
    case "StarSrvGroupClass_getObjectNum" : /*StarSrvGroupClass_getObjectNum*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
            if( l_SrvGroup == null ){
              System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
              return 0;
            }
            starcore._SRPLock();
            int Result = l_SrvGroup._GetObjectNum();
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarSrvGroupClass_activeScriptInterface" : /*StarSrvGroupClass_activeScriptInterface*/
          {           
              ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
              StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
              if( l_SrvGroup == null ){
                System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
                return processOutputArgs(new Object[]{false,false});  
              }                         
              starcore._SRPLock();
              boolean[] Result = l_SrvGroup._ActiveScriptInterface((String)plist.get(1));
              starcore._SRPUnLock();           
              return processOutputArgs(new Object[]{Result[0],Result[1]});                               
          }
    case "StarSrvGroupClass_preCompile" : /*StarSrvGroupClass_preCompile*/
          {           
              ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
              StarSrvGroupClass l_SrvGroup = (StarSrvGroupClass)CleObjectMap.get((String)plist.get(0));
              if( l_SrvGroup == null ){
                System.out.println(String.format("starsrvgroup object[%s] can not be found..",(String)plist.get(0)));
                return processOutputArgs(new Object[]{false,"error..."});  
              }                         
              starcore._SRPLock();
              Object[] Result = l_SrvGroup._PreCompile((String)plist.get(1),(String)plist.get(2));
              starcore._SRPUnLock();           
              return processOutputArgs(Result);                 
          }       
          
    /*-----------------------------------------------*/
    case "StarServiceClass_toString" : /*StarServiceClass_toString*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
            if( l_Service == null ){
              System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
              return "";    
            }
            return ""+l_Service;          
          }

    case "StarServiceClass_get" : /*StarServiceClass_get*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
            if( l_Service == null ){
              System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            starcore._SRPLock();
            Object Result = l_Service._Get((String)plist.get(1));
            starcore._SRPUnLock();
            if( Result instanceof StarSrvGroupClass ){
              String CleObjectID = getTagFromObjectMap(Result);
              if( CleObjectID == null ){
                CleObjectID = StarSrvGroupPrefix+UUID.randomUUID().toString();
                CleObjectMap.put(CleObjectID,Result);
              }  
              return CleObjectID;       
            }else 
              return Result;                 
          }
    case "StarServiceClass_getObject" : /*StarServiceClass_getObject*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
            if( l_Service == null ){
              System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            starcore._SRPLock();
            StarObjectClass Result = l_Service._GetObject((String)plist.get(1));
            if( Result != null ){
              String StarObjectID = (String)((StarObjectClass)Result)._Get("_ID");
              starcore._SRPUnLock();
              String CleObjectID = StarObjectPrefix+UUID.randomUUID().toString();
              CleObjectMap.put(CleObjectID,Result);              
              return CleObjectID + "+" + StarObjectID;               
            }else{
              starcore._SRPUnLock();
              return null; 
            }                   
          }
    case "StarServiceClass_getObjectEx" : /*StarServiceClass_getObjectEx*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
            if( l_Service == null ){
              System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            starcore._SRPLock();
            StarObjectClass Result = l_Service._GetObjectEx((String)plist.get(1));
            if( Result != null ){
              String StarObjectID = (String)((StarObjectClass)Result)._Get("_ID");
              starcore._SRPUnLock();
              String CleObjectID = StarObjectPrefix+UUID.randomUUID().toString();
              CleObjectMap.put(CleObjectID,Result);              
              return CleObjectID + "+" + StarObjectID;               
            }else{
              starcore._SRPUnLock();
              return null; 
            }                  
          }
    case "StarServiceClass_newObject" : /*StarServiceClass_newObject*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
            if( l_Service == null ){
              System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            ArrayList<Object> parglist = (ArrayList<Object>)plist.get(1); 
            starcore._SRPLock();  
            Object[] in_args = processInputArgs(parglist);
            if( in_args == null ){
              starcore._SRPUnLock();
              return null;               
            }                
            StarObjectClass Result = l_Service._New(in_args);
            if( Result != null ){
              String StarObjectID = (String)((StarObjectClass)Result)._Get("_ID");
              starcore._SRPUnLock();
              String CleObjectID = StarObjectPrefix+UUID.randomUUID().toString();
              CleObjectMap.put(CleObjectID,Result);              
              return CleObjectID + "+" + StarObjectID;               
            }else{
              starcore._SRPUnLock();
              return null; 
            }                  
          }
    case "StarServiceClass_runScript" : /*StarServiceClass_runScript*/
          {
              ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
              StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
              if( l_Service == null ){
                System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
                return processOutputArgs(new Object[]{false,"error..."});    
              }
              starcore._SRPLock();
              Object[] Result = l_Service._RunScript((String)plist.get(1),(String)plist.get(2),(String)plist.get(3),(String)plist.get(4));
              starcore._SRPUnLock();               
              return processOutputArgs(Result);               
          }
    case "StarServiceClass_runScriptEx" : /*StarServiceClass_runScriptEx*/
          {           
              ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
              StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
              if( l_Service == null ){
                System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
                return processOutputArgs(new Object[]{false,"error..."});  
              }
              StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(2));
              if( l_BinBuf == null ){
                System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(2)));
                return processOutputArgs(new Object[]{false,"error..."}); 
              }               
              starcore._SRPLock();
              Object[] Result = l_Service._RunScriptEx((String)plist.get(1),l_BinBuf,(String)plist.get(3),(String)plist.get(4));
              starcore._SRPUnLock();             
              return processOutputArgs(Result);                 
          }
    case "StarServiceClass_doFile" : /*StarServiceClass_doFile*/
          {           
              ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
              StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
              if( l_Service == null ){
                System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
                return processOutputArgs(new Object[]{false,"error..."});    
              }     
              starcore._SRPLock();
              Object[] Result = l_Service._DoFile((String)plist.get(1),(String)plist.get(2),(String)plist.get(3));
              starcore._SRPUnLock();
              return processOutputArgs(Result);                 
          }
    case "StarServiceClass_doFileEx" : /*StarServiceClass_doFileEx*/
          {           
              ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
              StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
              if( l_Service == null ){
                System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
                return processOutputArgs(new Object[]{false,"error..."});    
              }             
              starcore._SRPLock();
              Object[] Result = l_Service._DoFileEx((String)plist.get(1),(String)plist.get(2),(String)plist.get(3),(String)plist.get(4));
              starcore._SRPUnLock();              
              return processOutputArgs(Result);                 
          }
    case "StarServiceClass_isServiceRegistered" : /*StarServiceClass_isServiceRegistered*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
            if( l_Service == null ){
              System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
              return false;    
            }
            starcore._SRPLock();
            boolean Result = l_Service._IsServiceRegistered();
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarServiceClass_checkPassword" : /*StarServiceClass_checkPassword*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
            if( l_Service == null ){
              System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            starcore._SRPLock();
            l_Service._CheckPassword((boolean)plist.get(1));
            starcore._SRPUnLock();
            return null;                 
          }
    case "StarServiceClass_newRawProxy" : /*StarServiceClass_newRawProxy*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
            if( l_Service == null ){
              System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            StarObjectClass l_Object = (StarObjectClass)CleObjectMap.get((String)plist.get(2));
            if( l_Object == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(2)));
              return null;    
            }            
            starcore._SRPLock();
            StarObjectClass Result = l_Service._NewRawProxy((String)plist.get(1),l_Object,(String)plist.get(3),(String)plist.get(4),(int)plist.get(5));            
            if( Result != null ){
              String StarObjectID = (String)((StarObjectClass)Result)._Get("_ID");
              starcore._SRPUnLock();
              String CleObjectID = StarObjectPrefix+UUID.randomUUID().toString();
              CleObjectMap.put(CleObjectID,Result);              
              return CleObjectID + "+" + StarObjectID;               
            }else{
              starcore._SRPUnLock();
              return null; 
            }                
          }
    case "StarServiceClass_importRawContext" : /*StarServiceClass_importRawContext*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
            if( l_Service == null ){
              System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }        
            starcore._SRPLock();
            StarObjectClass Result = l_Service._ImportRawContext((String)plist.get(1),(String)plist.get(2),(boolean)plist.get(3),(String)plist.get(4));
            if( Result != null ){
              String StarObjectID = (String)((StarObjectClass)Result)._Get("_ID");
              starcore._SRPUnLock();
              String CleObjectID = StarObjectPrefix+UUID.randomUUID().toString();
              CleObjectMap.put(CleObjectID,Result);
              return CleObjectID + "+" + StarObjectID;               
            }else{
              starcore._SRPUnLock();
              return null; 
            }                
          }
    case "StarServiceClass_getLastError" : /*StarServiceClass_getLastError*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
            if( l_Service == null ){
              System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
              return 0;    
            }        
            starcore._SRPLock();
            int Result = l_Service._GetLastError();
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarServiceClass_getLastErrorInfo" : /*StarServiceClass_getLastErrorInfo*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarServiceClass l_Service = (StarServiceClass)CleObjectMap.get((String)plist.get(0));
            if( l_Service == null ){
              System.out.println(String.format("service object[%s] can not be found..",(String)plist.get(0)));
              return "";    
            } 
            starcore._SRPLock();
            String Result = l_Service._GetLastErrorInfo();
            starcore._SRPUnLock();
            return Result;                 
          }
          
    /*-----------------------------------------------*/
    case "StarParaPkgClass_toString" : /*StarParaPkgClass_toString*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return "";    
            }
            starcore._SRPLock();
            String val = ""+l_ParaPkg;
            starcore._SRPUnLock();
            return val;          
          }

    case "StarParaPkgClass_GetNumber" : /*StarParaPkgClass_GetNumber*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return 0;    
            }
            starcore._SRPLock();
            int ii = l_ParaPkg._GetInt("_Number");
            starcore._SRPUnLock();
            return ii;          
          }
    case "StarParaPkgClass_get" : /*StarParaPkgClass_get*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            starcore._SRPLock();
            Object IndexOrList = plist.get(1);
            if( IndexOrList instanceof ArrayList<?>){
              ArrayList<Object> ParaList = (ArrayList<Object>)IndexOrList;
              Object[] res = new Object[ParaList.size()];
              for( int i=0; i < ParaList.size(); i++ ){
                res[i] = l_ParaPkg._Get((int)ParaList.get(i));
              }
              ArrayList<Object> al = processOutputArgs(res);
              starcore._SRPUnLock();
              return al;
            }else{
              Object[] res = new Object[1];
              Object Result = l_ParaPkg._Get((int)plist.get(1));    
              res[0] = Result;        
              ArrayList<Object> al = processOutputArgs(res);
              starcore._SRPUnLock();
              return al.get(0);
            }       
          }
    case "StarParaPkgClass_clear" : /*StarParaPkgClass_clear*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            starcore._SRPLock();
            l_ParaPkg._Clear();    
            starcore._SRPUnLock();      
            return null;
          }
    case "StarParaPkgClass_appendFrom" : /*StarParaPkgClass_appendFrom*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return false;    
            }
            StarParaPkgClass s_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(1));
            if( s_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(1)));
              return false;    
            }            
            starcore._SRPLock();
            boolean val = l_ParaPkg._AppendFrom(s_ParaPkg);
            starcore._SRPUnLock();  
            return val;          
          }
    case "StarParaPkgClass_set" : /*StarParaPkgClass_set*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            if( plist.get(2) instanceof String ){
              String starObjTag = (String)plist.get(2);
              if( starObjTag.startsWith(StarBinBufPrefix) || starObjTag.startsWith(StarParaPkgPrefix) || starObjTag.startsWith(StarObjectPrefix) ){
                Object srcv = (Object)CleObjectMap.get(starObjTag);
                if( srcv == null ){
                  System.out.println(String.format("object[%s] can not be found..",starObjTag));
                  return null;   
                } 
                starcore._SRPLock();
                l_ParaPkg._Set((int)plist.get(1),srcv);
                starcore._SRPUnLock();
              }else{             
                starcore._SRPLock();
                l_ParaPkg._Set((int)plist.get(1),plist.get(2));
                starcore._SRPUnLock();
              }   
            }else{
              starcore._SRPLock();
              l_ParaPkg._Set((int)plist.get(1),processInputListToArray(plist.get(2)));
              starcore._SRPUnLock();              
            }
            return null;      
          }
    case "StarParaPkgClass_build" : /*StarParaPkgClass_build*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }     
            Object vv = plist.get(1); 
            if( vv instanceof ArrayList<?> ){
              ArrayList<Object> parglist = (ArrayList<Object>)vv;
              starcore._SRPLock(); 
              Object[] in_args = processInputArgs(parglist);
              if( in_args == null ){
                starcore._SRPUnLock();
                return null;    
              }      
              l_ParaPkg._Build(in_args);
              starcore._SRPUnLock();
            }else if( vv instanceof HashMap<?,?> ){
              HashMap<?,?> s_m = ( HashMap<?,?>)vv;
              ArrayList<Object> parglist = new ArrayList<Object>();
              for(Object key:s_m.keySet()){    
                parglist.add(key);
                parglist.add(s_m.get(key)); 
              }
              starcore._SRPLock();
              Object[] in_args = processInputArgs(parglist);              
              l_ParaPkg._Build(in_args);
              l_ParaPkg._AsDict(true);
              starcore._SRPUnLock();
            }
            return null;           
          }
    case "StarParaPkgClass_free" : /*StarParaPkgClass_free*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }              
            
            removeFromObjectMap(l_ParaPkg);

            starcore._SRPLock();  
            l_ParaPkg._Free();
            starcore._SRPUnLock();
            return null;           
          }   
    case "StarParaPkgClass_dispose" : /*StarParaPkgClass_dispose*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }               

            removeFromObjectMap(l_ParaPkg);

            starcore._SRPLock();  
            l_ParaPkg._Dispose();
            starcore._SRPUnLock();
            CleObjectMap.remove((String)plist.get(0));
            return null;           
          }
    case "StarParaPkgClass_releaseOwner" : /*StarParaPkgClass_releaseOwner*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }               

            removeFromObjectMap(l_ParaPkg);

            starcore._SRPLock();  
            l_ParaPkg._ReleaseOwner();
            starcore._SRPUnLock();
            CleObjectMap.remove((String)plist.get(0));
            return null;           
          }
    case "StarParaPkgClass_asDict" : /*StarParaPkgClass_asDict*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }               
            starcore._SRPLock();  
            l_ParaPkg._AsDict((boolean)plist.get(1));
            starcore._SRPUnLock();
            return null;           
          }
    case "StarParaPkgClass_isDict" : /*StarParaPkgClass_isDict*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return false;    
            }               
            starcore._SRPLock();  
            boolean Result = l_ParaPkg._IsDict();
            starcore._SRPUnLock();
            return Result;           
          }
    case "StarParaPkgClass_fromJSon" : /*StarParaPkgClass_fromJSon*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return false;    
            }               
            starcore._SRPLock();  
            boolean Result = l_ParaPkg._FromJSon((String)plist.get(1));
            starcore._SRPUnLock();
            return Result;           
          }
    case "StarParaPkgClass_toJSon" : /*StarParaPkgClass_toJSon*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return "";    
            }               
            starcore._SRPLock();  
            String Result = l_ParaPkg._ToJSon();
            starcore._SRPUnLock();
            return Result;           
          }
    case "StarParaPkgClass_toTuple" : /*StarParaPkgClass_toTuple*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarParaPkgClass l_ParaPkg = (StarParaPkgClass)CleObjectMap.get((String)plist.get(0));
            if( l_ParaPkg == null ){
              System.out.println(String.format("parapkg object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }               
            starcore._SRPLock();  
            int Number = l_ParaPkg._GetInt("_Number");
            Object[] oo = new Object[Number];
            for( int i=0; i < Number ; i++ ){              
              oo[i] = l_ParaPkg._Get(i);
            }
            ArrayList<Object> al = processOutputArgs(oo);      
            if( l_ParaPkg._IsDict() == true && (Number % 2) == 0 ){
              HashMap<Object,Object> hm = new HashMap<Object,Object>();
              for( int i=0; i < Number / 2 ; i++ ){
                hm.put(al.get(2*i),al.get(2*i+1));
              }
              starcore._SRPUnLock();
              return hm;
            }else{
              starcore._SRPUnLock();
              return al;  
            }         
          } 
          
    /*-----------------------------------------------*/
    case "StarBinBufClass_toString" : /*StarBinBufClass_toString*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(0));
            if( l_BinBuf == null ){
              System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(0)));
              return "";    
            }
            starcore._SRPLock();
            String val = ""+l_BinBuf;
            starcore._SRPUnLock();
            return val;          
          }

    case "StarBinBufClass_GetOffset" : /*StarBinBufClass_GetOffset*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(0));
            if( l_BinBuf == null ){
              System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(0)));
              return 0;    
            }
            starcore._SRPLock();
            int ii = l_BinBuf._GetInt("_Offset");
            starcore._SRPUnLock();
            return ii;          
          }
    case "StarBinBufClass_init" : /*StarBinBufClass_init*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(0));
            if( l_BinBuf == null ){
              System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            starcore._SRPLock();
            l_BinBuf._Init((int)plist.get(1));
            starcore._SRPUnLock();
            return null;          
          }
    case "StarBinBufClass_clear" : /*StarBinBufClass_clear*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(0));
            if( l_BinBuf == null ){
              System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            starcore._SRPLock();
            l_BinBuf._Clear();
            starcore._SRPUnLock();
            return null;          
          }
    case "StarBinBufClass_saveToFile" : /*StarBinBufClass_saveToFile*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(0));
            if( l_BinBuf == null ){
              System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(0)));
              return false;    
            }
            starcore._SRPLock();
            boolean Result = l_BinBuf._SaveToFile((String)plist.get(1),(boolean)plist.get(2));
            starcore._SRPUnLock();
            return Result;          
          }
    case "StarBinBufClass_loadFromFile" : /*StarBinBufClass_loadFromFile*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(0));
            if( l_BinBuf == null ){
              System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(0)));
              return false;    
            }
            starcore._SRPLock();
            boolean Result = l_BinBuf._LoadFromFile((String)plist.get(1),(boolean)plist.get(2));
            starcore._SRPUnLock();
            return Result;          
          }
    case "StarBinBufClass_read" : /*StarBinBufClass_read*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(0));
            if( l_BinBuf == null ){
              System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(0)));
              return new byte[]{};    
            }
            int Length = (int)plist.get(2);
            if( Length <= 0 ){
              return new byte[]{};                
            }
            byte[] Buf = new byte[Length];
            starcore._SRPLock();
            int Result = l_BinBuf._Read(Buf,(int)plist.get(1),Length);
            starcore._SRPUnLock();
            if( Result == 0 ){
              return new byte[]{};                
            }else{
              byte[] RetBuf = new byte[Result];
              System.arraycopy(Buf, 0, RetBuf, 0, Result);
              return RetBuf; 
            }                     
          }
    case "StarBinBufClass_write" : /*StarBinBufClass_write*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(0));
            if( l_BinBuf == null ){
              System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(0)));
              return 0;    
            }
            starcore._SRPLock();
            int Result = l_BinBuf._Write((int)plist.get(1),(byte[])plist.get(2),(int)plist.get(3));
            starcore._SRPUnLock();
            return Result;          
          }                      
    case "StarBinBufClass_free" : /*StarBinBufClass_free*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(0));
            if( l_BinBuf == null ){
              System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }              

            removeFromObjectMap(l_BinBuf);

            starcore._SRPLock();  
            l_BinBuf._Free();
            starcore._SRPUnLock();
            CleObjectMap.remove((String)plist.get(0));
            return null;           
          }
    case "StarBinBufClass_dispose" : /*StarBinBufClass_dispose*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(0));
            if( l_BinBuf == null ){
              System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }              

            removeFromObjectMap(l_BinBuf);

            starcore._SRPLock();  
            l_BinBuf._Dispose();
            starcore._SRPUnLock();
            CleObjectMap.remove((String)plist.get(0));
            return null;           
          }
    case "StarBinBufClass_releaseOwner" : /*StarBinBufClass_releaseOwner*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarBinBufClass l_BinBuf = (StarBinBufClass)CleObjectMap.get((String)plist.get(0));
            if( l_BinBuf == null ){
              System.out.println(String.format("binbuf object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }              

            removeFromObjectMap(l_BinBuf);

            starcore._SRPLock();  
            l_BinBuf._ReleaseOwner();
            starcore._SRPUnLock();
            CleObjectMap.remove((String)plist.get(0));
            return null;           
          }

    /*-----------------------------------------------*/
    case "StarObjectClass_toString" : /*StarObjectClass_toString*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return "";    
            }
            starcore._SRPLock();
            String val = ""+l_StarObject;
            starcore._SRPUnLock();
            return val;          
          }
    case "StarObjectClass_get" : /*StarObjectClass_get*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            starcore._SRPLock();
            Object IndexOrList = plist.get(1);
            if( IndexOrList instanceof ArrayList<?>){
              ArrayList<Object> ParaList = (ArrayList<Object>)IndexOrList;
              Object[] res = new Object[ParaList.size()];
              for( int i=0; i < ParaList.size(); i++ ){
                res[i] = l_StarObject._Get(ParaList.get(i));
              }
              ArrayList<Object> al = processOutputArgs(res);
              starcore._SRPUnLock();
              return al;
            }else{
              Object Result = l_StarObject._Get(plist.get(1));  
              Object[] res = new Object[]{Result};   
              ArrayList<Object> al = processOutputArgs(res);
              starcore._SRPUnLock();
              return al.get(0);        
            }
          }
    case "StarObjectClass_set" : /*StarObjectClass_set*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }
            if( plist.get(2) instanceof String ){
              String starObjTag = (String)plist.get(2);
              if( starObjTag.startsWith(StarBinBufPrefix) || starObjTag.startsWith(StarParaPkgPrefix) || starObjTag.startsWith(StarObjectPrefix) ){
                Object srcv = (Object)CleObjectMap.get(starObjTag);
                if( srcv == null ){
                  System.out.println(String.format("object[%s] can not be found..",starObjTag));
                  return null;   
                } 
                starcore._SRPLock();
                l_StarObject._Set(plist.get(1),srcv);
                starcore._SRPUnLock();
              }else{             
                starcore._SRPLock();
                l_StarObject._Set(plist.get(1),processInputListToArray(plist.get(2)));
                starcore._SRPUnLock();
              }   
            }else{
              starcore._SRPLock();
              l_StarObject._Set(plist.get(1),plist.get(2));
              starcore._SRPUnLock();              
            }              
            return null;      
          }
          
    case "StarObjectClass_call" : /*StarObjectClass_call*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            } 
            starcore._SRPLock();       
            ArrayList<Object> parglist = (ArrayList<Object>)plist.get(2);   
            Object[] in_args = processInputArgs(parglist);
            if( in_args == null ){
              starcore._SRPUnLock();
              return null;               
            }                 
            Object Result = l_StarObject._Call((String)plist.get(1),in_args); 
            ArrayList<Object> out = processOutputArgs(new Object[]{Result});
            starcore._SRPUnLock();
            return out.get(0);      
          }
    case "StarObjectClass_newObject" : /*StarObjectClass_newObject*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            } 
            ArrayList<Object> parglist = (ArrayList<Object>)plist.get(1);   
            starcore._SRPLock();
            Object[] in_args = processInputArgs(parglist);
            if( in_args == null ){
              starcore._SRPUnLock();
              return null;               
            }               
            StarObjectClass Result = l_StarObject._New(in_args);
            if( Result != null ){
              String StarObjectID = (String)((StarObjectClass)Result)._Get("_ID");
              starcore._SRPUnLock();
              String CleObjectID = StarObjectPrefix+UUID.randomUUID().toString();
              CleObjectMap.put(CleObjectID,Result);              
              return CleObjectID+"+"+StarObjectID;               
            }else{
              starcore._SRPUnLock();
              return null; 
            }                  
          }
    case "StarObjectClass_free" : /*StarObjectClass_free*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }             

            removeFromObjectMap(l_StarObject);

            starcore._SRPLock();  
            l_StarObject._Free();
            starcore._SRPUnLock();
            CleObjectMap.remove((String)plist.get(0));
            return null;           
          }
    case "StarObjectClass_dispose" : /*StarObjectClass_dispose*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }               

            removeFromObjectMap(l_StarObject);
            
            starcore._SRPLock();  
            l_StarObject._Dispose();
            starcore._SRPUnLock();
            CleObjectMap.remove((String)plist.get(0));
            return null;           
          }
    case "StarObjectClass_hasRawContext" : /*StarObjectClass_hasRawContext*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return false;    
            }                
            starcore._SRPLock();       
            boolean Result = l_StarObject._HasRawContext();
            starcore._SRPUnLock();      
            return Result;                 
          }
    case "StarObjectClass_rawToParaPkg" : /*StarObjectClass_rawToParaPkg*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }                
            starcore._SRPLock();       
            StarParaPkgClass Result = l_StarObject._RawToParaPkg();
            starcore._SRPUnLock();
            if( Result == null ){
              return null; 
            }
            ArrayList<Object> out = processOutputArgs(new Object[]{Result});
            return out.get(0);
            /*
            String CleObjectID = StarParaPkgPrefix+UUID.randomUUID().toString();
            CleObjectMap.put(CleObjectID,Result);      
            return CleObjectID;   
            */              
          }
    case "StarObjectClass_getSourceScript" : /*StarObjectClass_getSourceScript*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return 0;    
            }                
            starcore._SRPLock();       
            int Result = l_StarObject._GetSourceScript();
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarObjectClass_getLastError" : /*StarObjectClass_getLastError*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return 0;    
            }    
            starcore._SRPLock();
            int Result = l_StarObject._GetLastError();
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarObjectClass_getLastErrorInfo" : /*StarSrvGroupClass_getLastErrorInfo*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return "";    
            }    
            starcore._SRPLock();
            String Result = l_StarObject._GetLastErrorInfo();
            starcore._SRPUnLock();
            return Result;                 
          }
    case "StarObjectClass_releaseOwnerEx" : /*StarObjectClass_releaseOwnerEx*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }               

            removeFromObjectMap(l_StarObject);

            starcore._SRPLock();  
            l_StarObject._ReleaseOwnerEx();
            starcore._SRPUnLock();
            CleObjectMap.remove((String)plist.get(0));
            return null;           
          }
    case "StarObjectClass_regScriptProcP" : /*StarObjectClass_regScriptProcP*/
          {
            ArrayList<Object> plist = (ArrayList<Object>)call.arguments;
            StarObjectClass l_StarObject = (StarObjectClass)CleObjectMap.get((String)plist.get(0));
            if( l_StarObject == null ){
              System.out.println(String.format("star object[%s] can not be found..",(String)plist.get(0)));
              return null;    
            }               
            starcore._SRPLock();
            if( (boolean)plist.get(2) == false ){
              l_StarObject._RegScriptProc_P((String)plist.get(1),null);
            }else{
              final String FuncName = (String)plist.get(1);
              l_StarObject._RegScriptProc_P((String)plist.get(1),new StarObjectScriptProcInterface(){    
                public Object Invoke(Object CleObject,Object[] Paras)
                {        
                  final StarObjectClass i_StarObject = (StarObjectClass)CleObject;
                  String StarObjectID = (String)((StarObjectClass)i_StarObject)._Get("_ID");
                  String CleObjectID = StarObjectPrefix+UUID.randomUUID().toString();
                  CleObjectMap.put(CleObjectID,i_StarObject); 

                  ArrayList<Object> out = processOutputArgs(Paras);

                  ArrayList<Object> cP = new ArrayList<Object>();
                  cP.add(CleObjectID+"+"+StarObjectID);
                  cP.add(FuncName);
                  cP.add(out);                      

                  final Integer w_tag = get_WaitResultIndex();
                  StarFlutWaitResult m_WaitResult = new_WaitResult(w_tag);
                  cP.add(w_tag);
                 
                  Message message1 = mainHandler.obtainMessage();
                  message1.what = starobjecrclass_ScriptCallBack_MessageID;
                  message1.obj = cP;
                  mainHandler.sendMessage(message1);

                  starcore._SRPUnLock();
                  Object result = m_WaitResult.WaitResult();
                  remove_WaitResult(w_tag);
                  starcore._SRPLock();

                  //--process output result
                  ArrayList<Object> b_Result = new ArrayList<Object>();
                  b_Result.add(result);
                  Object[] pResult = processInputArgs(b_Result);
                  if( pResult[0] instanceof ArrayList<?> ){
                    ArrayList<Object> ll = (ArrayList<Object>)pResult[0];
                    return ll.toArray(); 
                  }else{
                    return pResult[0];     
                  }
                }
              }); 
            }
            starcore._SRPUnLock();
            return 0;                   
          }                                           
    default:
          return null;
    } 
  }
}

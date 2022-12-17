/*--------------------------------------------------------------------
   does not functions in ansi c
   for this function support on all platforms
----------------------------------------------------------------------*/
#ifndef _VS_SHELL
#define _VS_SHELL

#include "vsopendatatype.h"
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#include <sys\stat.h>
#include <direct.h>
#include <winbase.h>
#include <conio.h>
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>
#include <dirent.h>
#if( VS_OS_TYPE != VS_OS_ANDROID &&  VS_OS_TYPE != VS_OS_ANDROIDV7A &&  VS_OS_TYPE != VS_OS_ANDROIDX86 && VS_OS_TYPE != VS_OS_IOS && VS_OS_TYPE != VS_OS_LINUX && VS_OS_TYPE != VS_OS_MACOS )
#include <curses.h>
#endif
#include <termios.h>

#define _MULTI_THREADED
#include <pthread.h>
#include <sched.h>
#if( VS_OS_TYPE != VS_OS_ANDROID &&  VS_OS_TYPE != VS_OS_ANDROIDV7A &&  VS_OS_TYPE != VS_OS_ANDROIDX86 )
#include <sys/sem.h>
#endif
#include <sys/ipc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <alloca.h>
#include <utime.h>
#endif


#define VS_STRNCPY(x,y,n) {strncpy(x,y,n);((VS_CHAR *)x)[n-1]=0;}

/*--------------------------------------------------------------------
part 1 : file system functions
the following list use ansi c
FILE * / fopen / fclose / feof / fflush / fgetc / fprintf / fputc / fread / fscanf,
fseek / ftell / fwrite / vfprintf / vsprintf
----------------------------------------------------------------------*/
#define VS_FILE_NAMELENGTH 512

typedef struct
{    
    VS_UINT16     usMode;          /* reserved.file mode,Bit mask for file-mode information. */       
    VS_UINT16     usReserved;
    VS_ULONG      ulSize;          /* size of file, in bytes */
    time_t        stAccessTime;    /* time of last access */
    time_t        stModifyTime;    /* time of last modification */
    time_t        stChangTime;     /* time of last change of file status */
}VS_FILE_STAT_T;

#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
typedef WIN32_FIND_DATAA VS_FILE_FIND_T;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
typedef struct
{
	VS_BOOL    FirstMask;
	VS_BOOL    ExtendMask;
	VS_UINT8   Reserve[2];
	VS_CHAR    cPatternFirst[VS_FILE_NAMELENGTH]; 
	VS_CHAR    cPatternExtend[VS_FILE_NAMELENGTH]; 

	VS_CHAR    cDirName[VS_FILE_NAMELENGTH];
	VS_ULONG   dwFileAttributes;
    VS_CHAR    cFileName[VS_FILE_NAMELENGTH]; 
}VS_FILE_FIND_T ;   
typedef struct
{
	VS_FILE_FIND_T  FindData;
	DIR *dirptr;
}VS_DIR_T ;  
#endif

#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
typedef VS_ULONG VS_THREADID;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
typedef pthread_t VS_THREADID;
#endif

/*-------------------functions-------------------*/
void vs_file_namechange(VS_CHAR *FileName,VS_CHAR Ch,VS_CHAR NewCh);
FILE *vs_file_fopen(const VS_CHAR *FileName,const VS_CHAR *mode );  /*--opmode = 0777--*/
FILE *vs_file_fopenex(const VS_CHAR *FileName,const VS_CHAR *mode,VS_UINT16 opmode );

#define vs_file_fclose fclose
#define vs_file_feof feof
#define vs_file_fflush fflush
#define vs_file_fgetc fgetc
#define vs_file_ungetc ungetc
#define vs_file_fgets fgets
#define vs_file_fprintf fprintf
#define vs_file_fputc fputc
#define vs_file_fputs fputs
#define vs_file_fread fread
#define vs_file_fscanf fscanf
#define vs_file_fseek fseek
#define vs_file_ftell ftell
#define vs_file_fwrite fwrite
#define vs_file_vfprintf vfprintf
#define vs_file_vsprintf vsprintf

VS_INT32 vs_file_delete(const VS_CHAR *FileName);   /*--VS_OK/VS_FAIL--*/
VS_BOOL vs_file_exist(const VS_CHAR *FileName);
VS_INT32 vs_file_stat(const VS_CHAR *FileName, VS_FILE_STAT_T *pstStat); /*--VS_OK/VS_FAIL--*/
VS_INT32 vs_file_copy( const VS_CHAR *SrcFileName,const VS_CHAR *DestFileName ); /*--VS_OK/VS_FAIL--*/
VS_ULONG vs_file_size(const VS_CHAR *szFileName);
VS_INT32 vs_file_rename(const VS_CHAR *szOldFileName,const VS_CHAR *szNewFileName); /*--VS_OK/VS_FAIL--*/
VS_BOOL vs_file_isdir(const VS_CHAR *szFileName);
VS_BOOL vs_file_chmod(const VS_CHAR *szFileName,VS_UINT16 mode);
VS_INT32 vs_file_settime(const VS_CHAR *szFileName,VS_ULONG AccessTime,VS_ULONG ModifyTime);  /*--VS_OK/VS_FAIL--*/

/*--find functions support * / *.*   all files 
and *.XXX and XXX.*---*/
VS_HANDLE vs_file_findfirst(const VS_CHAR *szFileName,VS_FILE_FIND_T *pstFind);    /*VS_INVALID_HANDLE*/
VS_BOOL vs_file_findnext(const VS_HANDLE FindHandle,VS_FILE_FIND_T *pstFind);
VS_BOOL vs_file_findclose(const VS_HANDLE FindHandle);
VS_BOOL vs_file_isfinddir(const VS_FILE_FIND_T *pstFind);

VS_BOOL vs_dir_create(const VS_CHAR *szDirName);     /*--opmode = 0777--*/
VS_BOOL vs_dir_createex(const VS_CHAR *szDirName,VS_UINT16 opmode);
VS_BOOL vs_dir_delete(const VS_CHAR *szDirName);
VS_BOOL vs_dir_getcwd(VS_CHAR *Buf,VS_ULONG Size);
VS_BOOL vs_dir_chdir(const VS_CHAR *szDirName);

VS_CHAR *vs_file_strchr(const VS_CHAR *szFileName,VS_CHAR Pattern);  /*--look for \\ or / */
VS_CHAR *vs_file_strrchr(const VS_CHAR *szFileName,VS_CHAR Pattern);  /*--look for \\ or / */

VS_BOOL vs_dir_isfullname(const VS_CHAR *szDirName);  /* has ':' in szDirName windows  or the first is '/' linux */
void vs_dir_toroot(VS_CHAR *szDirName); /* input is full path, output: for windows set szDirName[2]=0  for linux is set szDirName[1]=0 */
void vs_dir_tofullname(VS_CHAR *szDirName,VS_ULONG Size);
void vs_dir_tofullnameex(const VS_CHAR *RootPath,VS_CHAR *szDirName,VS_ULONG Size);

void vs_file_log(const VS_CHAR *FileName,const VS_CHAR *format,...);

/*--------------------------------------------------------------------
part 2 : time functions
----------------------------------------------------------------------*/
typedef struct
{
    time_t lSecond; /*VS_LONG lSecond;   */        /* second */
    VS_UINT16  usMilliseconds;         /* million second */
    VS_UINT8   ucPad[2];
}VS_TM_SECOND_T;       

/*VS_INT32 vs_tm_gmt2clock(VS_UINT32 ulGmtSeconds,VS_TIME_T *pstClock);*/
VS_INT32 vs_tm_gmt2clock(time_t ulGmtSeconds,VS_TIME_T *pstClock);
VS_INT32 vs_tm_time2clock(VS_TM_SECOND_T *pstSecMillisec,VS_TIME_T  *ptClock);
VS_INT32 vs_tm_clock2time(VS_TIME_T  *ptClock,VS_TM_SECOND_T *pstSecMillisec);
VS_UWORD vs_tm_gettickcount(void);    /*ms*/
VS_INT64 vs_tm_gettickcount64(void);  /*ms*/
VS_INT64 vs_tm_gettickcount_us(void); /*us*/
void vs_tm_getlocaltime(VS_TIME_T *Time);

/*--------------------------------------------------------------------
part 3 : dll functions
----------------------------------------------------------------------*/
typedef struct{
	VS_CHAR FunctionName[128];
	void *FunctionAddr;
}VS_FUNCTION_TABLE;  /* add v2.50.0 */
#define VS_DLL_PSEUDOHANDLE   ((VS_HANDLE)-1)   /* for ios : RTLD_MAIN_ONLY */
VS_HANDLE vs_dll_open(const VS_CHAR *ModuleName);
VS_HANDLE vs_dll_openex(const VS_CHAR *ModuleName,VS_BOOL ExportGlobal,VS_CHAR **ErrorInfo); /* ExportGlobal is valid for linux */
/* vs_dll_get return sharelibrary handle : not support on ios, wp*/
VS_HANDLE vs_dll_get(const VS_CHAR *ModuleName);   /* get module handle */
/* vs_dll_getex return sharelibrary handle, and full file name : not support on ios, wp*/
VS_HANDLE vs_dll_getex(const VS_CHAR *ModuleName,VS_CHAR *FullModuleName,VS_INT32 FullModuleSize);   /* get module handle */
void vs_dll_close(const VS_HANDLE Handle);
void *vs_dll_sym(const VS_HANDLE Handle,const VS_CHAR *ProcName);
void vs_dll_settemppath(const VS_CHAR *Path);  /*used for android*/

void *vs_dll_registerproc(const VS_CHAR *ProcName,void *ProcAddr);  /*--add v2.50.0, the maximum leng of ProcName is 127, the maximum function entry is 4096--*/
                                                                    /*after register function address, you can use vs_dll_sym to get the address, in this case, Handle is ignored*/
                                                                    /*the return value is old registered value, if exist. Or else, returns NULL*/
/*--------------------------------------------------------------------
part 4 : string functions
----------------------------------------------------------------------*/
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#ifndef CBUILDER_FLAG
#define vs_string_stricmp _stricmp
#define vs_string_strnicmp _strnicmp
#else
#define vs_string_stricmp stricmp
#define vs_string_strnicmp strnicmp
#endif
//#define vs_string_snprintf _snprintf
//#define vs_string_vsnprintf _vsnprintf
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
#define vs_string_stricmp strcasecmp
#define vs_string_strnicmp strncasecmp
//#define vs_string_snprintf snprintf
//#define vs_string_vsnprintf vsnprintf
#endif

VS_INT32 vs_string_snprintf( VS_CHAR *buffer, size_t count, const VS_CHAR *format, ... );
VS_INT32 vs_string_vsnprintf( VS_CHAR *buffer, size_t count, const VS_CHAR *format, va_list argptr );

VS_CHAR *vs_string_stristr( const VS_CHAR *string, const VS_CHAR *strCharSet );
void vs_memcpy(void *DesBuf,const void *SrcBuf,VS_WORD Size);
void vs_memset(void *DesBuf,VS_INT8 c,VS_WORD Size);

size_t vs_string_strlen( const VS_CHAR *string );
VS_INT32 vs_string_strcmp( const VS_CHAR *string1, const VS_CHAR *string2 );

/*--------------------------------------------------------------------
part 5 : thread functions
----------------------------------------------------------------------*/
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#define SCHED_OTHER  0
#define SCHED_FIFO   0
#define SCHED_RR     0
#define VSTHREADAPI  WINAPI
#define vs_thread_routineproc LPTHREAD_START_ROUTINE
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
#define VSTHREADAPI
typedef void *(*vs_thread_routineproc)(void *arg);
#endif

VS_HANDLE vs_thread_create(vs_thread_routineproc proc,void *arg,VS_THREADID *ThreadID);    /* ThreadID changed from VS_ULONG to VS_THREADID  2.5.2*/
void vs_thread_exit(void *status);
VS_HANDLE vs_thread_current();
VS_THREADID vs_thread_currentid();                                                         /* ThreadID changed from VS_ULONG to VS_THREADID 2.5.2*/
void vs_thread_yield();
void vs_thread_detach(VS_HANDLE hThread);
void vs_thread_join(VS_HANDLE hThread);

/*---set thread priority-----
  priority = priority + RelativePriority;
  if( RelativePriority < 0 )  THREAD_PRIORITY_ABOVE_NORMAL
  if( RelativePriority > 0 )  THREAD_PRIORITY_BELOW_NORMAL
----------------------------*/
void vs_thread_priority(VS_HANDLE hThread,VS_INT32 Policy,VS_INT32 RelativePriority);
void vs_thread_sleep( VS_ULONG wMilliseconds);

/*----recursive mutex in process */
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
typedef CRITICAL_SECTION VS_MUTEX;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
typedef pthread_mutex_t VS_MUTEX;
#endif

VS_BOOL vs_mutex_init( VS_MUTEX *mutex);
VS_BOOL vs_mutex_lock( VS_MUTEX *mutex);
void vs_mutex_unlock( VS_MUTEX *mutex);
void vs_mutex_destory( VS_MUTEX *mutex);

/*----mutex between process*/
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
typedef VS_HANDLE VS_PMUTEX;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
typedef VS_INT32 VS_PMUTEX;
#endif
#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
typedef struct{
	VS_CHAR Name[128];
	VS_INT32 LockCount;
}VS_PMUTEX_T;
typedef VS_PMUTEX_T *VS_PMUTEX; 
#endif
/*The following four funcion does not supported for ios and android, vs_pmutex_init only simply returns -1*/
VS_PMUTEX vs_pmutex_init( const VS_CHAR *Path,const VS_CHAR *name );  /* name is first four chars for linux, path ignore on windows and name < 128 bytes*/
                                                          /* for android does not support sem. so we try to use a file to realize, but it should not be called frequently*/
VS_BOOL vs_pmutex_lock( VS_PMUTEX mutex);
void vs_pmutex_unlock( VS_PMUTEX mutex);
void vs_pmutex_destory( VS_PMUTEX mutex);

/*----condition variable in process*/
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
typedef VS_HANDLE VS_COND;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
typedef struct{
    pthread_cond_t cond;
	VS_MUTEX mutex;
}VS_COND;
#endif
VS_BOOL vs_cond_init( VS_COND *cond);
void vs_cond_wait( VS_COND *cond);
VS_BOOL vs_cond_timewait( VS_COND *cond, VS_ULONG TimeMs);  /* if TimeMs == 0, then this function is same as vs_cond_wait */
void vs_cond_signal( VS_COND *cond);  /*--trigger one thread, the condition will be reset after trigered--*/
void vs_cond_destroy( VS_COND *cond);

/*--------------------------------------------------------------------
part 6 : atomic functions
----------------------------------------------------------------------*/
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
typedef VS_LONG vs_atomic;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
typedef struct{
	VS_LONG val;
	VS_MUTEX mutex;
}vs_atomic;
#endif

void vs_atomic_init(vs_atomic *atomic);
void vs_atomic_set(vs_atomic *atomic,VS_LONG i);
VS_LONG vs_atomic_compare_set(vs_atomic *atomic,VS_LONG i,VS_LONG comparevalue);
VS_LONG vs_atomic_get(vs_atomic *atomic);
void vs_atomic_destory(vs_atomic *atomic);

/*--------------------------------------------------------------------
part 7 : system app functions
----------------------------------------------------------------------*/
/*-----sys/resource.h>   getrusage() */
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#endif

VS_BOOL vs_get_module(VS_CHAR *Buf,VS_INT32 BufSize);
VS_HANDLE vs_process_current();
VS_ULONG vs_process_currentid();
VS_BOOL vs_process_exist(VS_ULONG ProcessID); /* vs_process_exist always returns true for ios/wp */
void vs_process_exit(VS_INT32 Status);  /*vs_process_exit not supported for wp*/

VS_BOOL vs_get_env(const VS_CHAR *Name,VS_CHAR *Buf,VS_INT32 BufSize);
VS_BOOL vs_set_env(const VS_CHAR *Name,const VS_CHAR *Value);

void vs_uuid_create(VS_UUID *Uuid);
VS_INT32 vs_kbhit(void);          /*return -1; non key enter*/

/*--------------------------------------------------------------------
part 8 : socket functions
----------------------------------------------------------------------*/
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS || VS_OS_TYPE == VS_OS_WIN10 )
VS_BOOL vs_socket_init();
void vs_socket_term();
VS_INT32 vs_socket_close(SOCKET s);
void vs_socket_setnonblock(SOCKET s);
void vs_socket_setreuseaddr(SOCKET s);

VS_INT32 vs_socket_getpeername(SOCKET s,SOCKADDR *name,VS_INT32 *namelen);
VS_INT32 vs_socket_getsockname(SOCKET s,SOCKADDR *name,VS_INT32 *namelen);
SOCKET vs_socket_accept(SOCKET s,SOCKADDR *addr,VS_INT32 *addrlen);
VS_INT32 vs_socket_recvfrom(SOCKET s,VS_CHAR* buf,VS_INT32 len,VS_INT32 flags,SOCKADDR* from,VS_INT32* fromlen);
VS_INT32 vs_socket_geterrno();

VS_BOOL vs_socket_isconnect(SOCKET s);  /*only valid at linux, on win32 always return true*/

#define vs_ntohs ntohs
#define vs_ntohl ntohl
#define vs_htons htons
#define vs_htonl htonl

#endif

#if( VS_OS_TYPE == VS_OS_WINRT )
#if defined(USE_SRPSOCKET)
VS_INT16 vs_htons(VS_INT16 x);
VS_LONG vs_htonl(VS_LONG x);
#define vs_ntohs vs_htons
#define vs_ntohl vs_htonl
#else
VS_BOOL vs_socket_init();
void vs_socket_term();
VS_INT32 vs_socket_close(SOCKET s);
void vs_socket_setnonblock(SOCKET s);
void vs_socket_setreuseaddr(SOCKET s);

VS_INT32 vs_socket_getpeername(SOCKET s, SOCKADDR *name, VS_INT32 *namelen);
VS_INT32 vs_socket_getsockname(SOCKET s, SOCKADDR *name, VS_INT32 *namelen);
SOCKET vs_socket_accept(SOCKET s, SOCKADDR *addr, VS_INT32 *addrlen);
VS_INT32 vs_socket_recvfrom(SOCKET s, VS_CHAR* buf, VS_INT32 len, VS_INT32 flags, SOCKADDR* from, VS_INT32* fromlen);
VS_INT32 vs_socket_geterrno();

VS_BOOL vs_socket_isconnect(SOCKET s);  /*only valid at linux, on win32 always return true*/

#define vs_ntohs ntohs
#define vs_ntohl ntohl
#define vs_htons htons
#define vs_htonl htonl
#endif
#endif


VS_INT64 vs_ntohl64(VS_INT64 Value);
VS_INT64 vs_htonl64(VS_INT64 Value);

/*--------------------------------------------------------------------
part 9 : float, double <-> uword, int64
----------------------------------------------------------------------*/

VS_UWORD Float2UWord(VS_FLOAT val);
VS_FLOAT UWord2Float(VS_UWORD val);
VS_UWORD Double2UWord(VS_DOUBLE val);  /* Double will be changed to float to compatiable with 32bit*/
VS_DOUBLE UWord2Double(VS_UWORD val);
VS_INT64 Float2Int64(VS_FLOAT val);
VS_FLOAT Int642Float(VS_INT64 val);
VS_INT64 Double2Int64(VS_DOUBLE val);
VS_DOUBLE Int642Double(VS_INT64 val);

#define _F2U(X) Float2UWord(X)
#define _U2F(X) UWord2Float(X)
#define _D2U(X) Double2UWord(X)
#define _U2D(X) UWord2Double(X)
#define _F2I(X) Float2Int64(X)
#define _I2F(X) Int642Float(X)
#define _D2I(X) Double2Int64(X)
#define _I2D(X) Int642Double(X)
#define _U2I(X) (((VS_INT64)((VS_UINT64)((VS_UWORD)X))))
#define _I2U(X) (((VS_INT64)((VS_UINT64)((VS_INT64)X))))

#if defined(ENV_M64)
#define TO_VS_FLOAT_F(X)    Float2UWord(X)  /*--VS_FLOAT to VS_FLOAT_F */
#define FROM_VS_FLOAT_F(X)  UWord2Float(X)  /*--VS_FLOAT_F to VS_FLOAT */
#define TO_VS_DOUBLE_F(X)   Double2Int64(X)  /*--VS_DOUBLE to VS_DOUBLE_F */
#define FROM_VS_DOUBLE_F(X) Int642Double(X)  /*--VS_DOUBLE_F to VS_DOUBLE */
#else
#define TO_VS_FLOAT_F(X)    X  /*--VS_FLOAT to VS_FLOAT_F */
#define FROM_VS_FLOAT_F(X)  X  /*--VS_FLOAT_F to VS_FLOAT */
#define TO_VS_DOUBLE_F(X)   X  /*--VS_DOUBLE to VS_DOUBLE_F */
#define FROM_VS_DOUBLE_F(X) X  /*--VS_DOUBLE_F to VS_DOUBLE */
#endif

/*--------------------------------------------------------------------
part 10 : atoi, process overflow of integer  
----------------------------------------------------------------------*/
VS_INT32 vs_atoi(const VS_CHAR *str);

#if( VS_OS_TYPE == VS_OS_WINDOWS )
#if defined(_MSC_VER)
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"rpcrt4.lib")
#endif
#endif

#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 )
#pragma comment(lib,"Ws2_32.lib")
#endif

#if( VS_OS_TYPE == VS_OS_WINRT )
#if !defined(USE_SRPSOCKET)
#pragma comment(lib,"Ws2_32.lib")
#endif
#endif

#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
VS_BOOL vs_getinstall_dir(VS_CHAR *Buf,VS_INT32 BufSize);
VS_BOOL vs_getlocal_dir(VS_CHAR *Buf,VS_INT32 BufSize);
void vs_outputdebugstring(VS_CHAR *Buf,VS_UINT32 CodePage);
#endif

#endif

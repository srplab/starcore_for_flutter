#ifndef _VSOPENDATATYPE
#define _VSOPENDATATYPE

#define  VS_OS_WINDOWS      0
#define  VS_OS_WINDOW       0 
#define  VS_OS_LINUX        1 
#define  VS_OS_ANDROID      2
#define  VS_OS_ANDROIDV7A   3
#define  VS_OS_IOS          4
#define  VS_OS_WP           5
#define  VS_OS_ANDROIDX86   6
#define  VS_OS_WINRT        7
#define  VS_OS_MACOS        8
#define  VS_OS_WIN10        9   /* for windows 10 */
#define  VS_OS_UNKOWN       255

#if !defined(ENV_WINDOWS) && !defined(ENV_LINUX) && !defined(ENV_ANDROID) && !defined(ENV_ANDROIDV7A) && !defined(ENV_ANDROIDX86) && !defined(ENV_IOS)  && !defined(ENV_WP)  && !defined(ENV_WINRT) && !defined(ENV_MACOS) && !defined(ENV_WIN10)
    #define VS_OS_TYPE          VS_OS_WINDOW
#else
    #ifdef ENV_WINDOWS
        #define VS_OS_TYPE          VS_OS_WINDOW
    #endif
    #ifdef ENV_LINUX
        #define VS_OS_TYPE          VS_OS_LINUX
    #endif
    #ifdef ENV_ANDROID
        #define VS_OS_TYPE          VS_OS_ANDROID
    #endif
    #ifdef ENV_ANDROIDV7A
        #define VS_OS_TYPE          VS_OS_ANDROIDV7A
    #endif
    #ifdef ENV_ANDROIDX86
        #define VS_OS_TYPE          VS_OS_ANDROIDX86
    #endif
    #ifdef ENV_IOS
        #define VS_OS_TYPE          VS_OS_IOS
    #endif    
    #ifdef ENV_WP
        #define VS_OS_TYPE          VS_OS_WP
    #endif  
    #ifdef ENV_WINRT
        #define VS_OS_TYPE          VS_OS_WINRT
    #endif  
    #ifdef ENV_MACOS
        #define VS_OS_TYPE          VS_OS_MACOS
    #endif      
    #ifdef ENV_WIN10
        #define VS_OS_TYPE          VS_OS_WIN10
    #endif      
#endif

#if( VS_OS_TYPE == VS_OS_WINDOW || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    #include <stdio.h>
    #include <stdlib.h>
    #include <malloc.h>
    #include <string.h>
    #include <stdarg.h>
    #include <stddef.h>
    #include <time.h>
    #include <rpc.h>
    #include <windows.h>
    #include <assert.h>
    #include <tchar.h>
    #include <sys\stat.h>
    #include <direct.h>
    #include <winbase.h>
    #include <conio.h>
    #include <ctype.h>
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 )
	#include <winsock2.h>
    #define HAS_SOCKADDR
#endif
#if( VS_OS_TYPE == VS_OS_WINRT )
#if !defined(USE_SRPSOCKET)
	#include <winsock2.h>
    #define HAS_SOCKADDR
#endif    
#endif

#if( VS_OS_TYPE == VS_OS_WINDOW )
#ifdef WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
	#include <winsock2.h>
#endif
#endif
    #define HAS_SOCKADDR
#endif

#endif

#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    #include <stdio.h>
    #include <stdlib.h>
#if( VS_OS_TYPE != VS_OS_IOS && VS_OS_TYPE != VS_OS_MACOS )    
    #include <malloc.h>
#endif    
    #include <string.h>
    #include <stdarg.h>
    #include <stddef.h>
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86)
    #include <time.h>
#endif
#if( VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    #import <sys/time.h> 
    #import <time.h>
#endif    
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <assert.h>
    #include <wchar.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <dlfcn.h>
    #include <dirent.h>
#if( VS_OS_TYPE != VS_OS_ANDROID && VS_OS_TYPE != VS_OS_ANDROIDV7A  && VS_OS_TYPE != VS_OS_ANDROIDX86 && VS_OS_TYPE != VS_OS_IOS && VS_OS_TYPE != VS_OS_LINUX && VS_OS_TYPE != VS_OS_MACOS)
    #include <curses.h>
#endif    
    #include <termios.h>
    #include <ctype.h>
    
    #define _MULTI_THREADED
    #include <pthread.h>
    #include <sched.h>
#if( VS_OS_TYPE != VS_OS_ANDROID && VS_OS_TYPE != VS_OS_ANDROIDV7A && VS_OS_TYPE != VS_OS_ANDROIDX86 )
    #include <sys/sem.h>
#else
    #include <semaphore.h>
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
    #include <signal.h>
#if( VS_OS_TYPE != VS_OS_ANDROID && VS_OS_TYPE != VS_OS_ANDROIDV7A && VS_OS_TYPE != VS_OS_ANDROIDX86 && VS_OS_TYPE != VS_OS_IOS && VS_OS_TYPE != VS_OS_MACOS )
    #include <wait.h>
#endif 
#if( VS_OS_TYPE == VS_OS_LINUX )
	#include <ifaddrs.h>
    #include <net/if.h>		
#endif
#if( VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	#include <ifaddrs.h>
    #include <net/if.h>		
#endif
    #define HAS_SOCKADDR
#endif


#if !defined(ENV_M64)

#if( VS_OS_TYPE == VS_OS_WINDOW || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )

#if defined(_WIN64)
#define ENV_M64 1
#endif
#endif

#if( VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
#if defined(__LP64__) && __LP64__
#define ENV_M64 1
#endif
#endif

#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86)
#if defined(__LP64__) || defined(__LP64)
#define ENV_M64 1
#endif
#endif

#endif


/*--------------OS TYPE And Module Extension-----------*/
/*  Max Number Is 32  */
#define VS_OSTYPE_NUM   10
#define VSOS_WIN32      ((VS_UINT32)0x01)    /*---need search---*/
#define VSOS_LINUX      ((VS_UINT32)0x02)
#define VSOS_ANDROID    ((VS_UINT32)0x04)
#define VSOS_ANDROIDV7A  ((VS_UINT32)0x08)
#define VSOS_IOS        ((VS_UINT32)0x10)
#define VSOS_WP         ((VS_UINT32)0x20)
#define VSOS_ANDROIDX86  ((VS_UINT32)0x40)
#define VSOS_WINRT      ((VS_UINT32)0x80)
#define VSOS_MACOS      ((VS_UINT32)0x100)
#define VSOS_WIN10      ((VS_UINT32)0x200)

#define VSMODULEEXT_WIN32   ".dll"
#define VSMODULEEXT_LINUX   ".so"
#define VSMODULEEXT_ANDROID ".so"
#define VSMODULEEXT_ANDROIDV7A ".so"
#define VSMODULEEXT_ANDROIDX86 ".so"
#define VSMODULEEXT_IOS     ".dylib"
#define VSMODULEEXT_WP     ".dll"
#define VSMODULEEXT_WINRT  ".dll"
#define VSMODULEEXT_MACOS     ".dylib"
#define VSMODULEEXT_WIN10  ".dll"

#if defined(ENV_M64)

#define VSSRVLIBEXT_WIN32   "_x64.dll"
#define VSSRVLIBEXT_LINUX   "_x64.so"
#define VSSRVLIBEXT_ANDROID "_android_x64.so"
#define VSSRVLIBEXT_ANDROIDV7A "_androidv7a_x64.so"
#define VSSRVLIBEXT_ANDROIDX86 "_androidx86_x64.so"
#define VSSRVLIBEXT_IOS     "_x64.dylib"
#if !defined(_M_ARM)
#define VSSRVLIBEXT_WP      "_wpx86_x64.dll"
#define VSSRVLIBEXT_WINRT   "_winrtx86_x64.dll"
#define VSSRVLIBEXT_WIN10   "_win10x86_x64.dll"
#else
#define VSSRVLIBEXT_WP      "_wparm_x64.dll"
#define VSSRVLIBEXT_WINRT   "_winrtarm_x64.dll"
#define VSSRVLIBEXT_WIN10   "_win10arm_x64.dll"
#endif
#define VSSRVLIBEXT_MACOS     "_x64.dylib"

#else

#define VSSRVLIBEXT_WIN32   ".dll"
#define VSSRVLIBEXT_LINUX   ".so"
#define VSSRVLIBEXT_ANDROID "_android.so"
#define VSSRVLIBEXT_ANDROIDV7A "_androidv7a.so"
#define VSSRVLIBEXT_ANDROIDX86 "_androidx86.so"
#define VSSRVLIBEXT_IOS     ".dylib"
#if !defined(_M_ARM)
#define VSSRVLIBEXT_WP      "_wpx86.dll"
#define VSSRVLIBEXT_WINRT   "_winrtx86.dll"
#define VSSRVLIBEXT_WIN10   "_win10x86.dll"
#else
#define VSSRVLIBEXT_WP      "_wparm.dll"
#define VSSRVLIBEXT_WINRT   "_winrtarm.dll"
#define VSSRVLIBEXT_WIN10   "_win10arm.dll"
#endif
#define VSSRVLIBEXT_MACOS     ".dylib"

#endif

#if !defined(ENV_M64)
#define VSOSALIAS_WIN32     "windows_x86"
#define VSOSALIAS_LINUX     "linux_x86"
#define VSOSALIAS_ANDROID   "android_armeabi"
#define VSOSALIAS_ANDROIDV7A   "android_armeabiv7a"
#define VSOSALIAS_ANDROIDX86   "android_x86"
#define VSOSALIAS_IOS       "ios"
#define VSOSALIAS_WP       "wp"
#define VSOSALIAS_WINRT    "winrt"
#define VSOSALIAS_MACOS    "macos"
#define VSOSALIAS_WIN10    "windows10"
#else
#define VSOSALIAS_WIN32     "windows_x64"
#define VSOSALIAS_LINUX     "linux_x64"
#define VSOSALIAS_ANDROID   "android_arm64"
#define VSOSALIAS_ANDROIDV7A   "android_armv7a_64"
#define VSOSALIAS_ANDROIDX86   "android_x86_64"
#define VSOSALIAS_IOS       "ios"
#define VSOSALIAS_WP       "wp"
#define VSOSALIAS_WINRT    "winrt"
#define VSOSALIAS_MACOS    "macos"
#define VSOSALIAS_WIN10    "windows10"
#endif

/*-----for current os----*/
#if( VS_OS_TYPE == VS_OS_WINDOW )
#define VS_OSTYPE       VSOS_WIN32
#define VS_MODULEEXT    VSMODULEEXT_WIN32
#define VS_SRVLIBEXT    VSSRVLIBEXT_WIN32
#define VS_OSALIAS      VSOSALIAS_WIN32
#define VS_COREPATH     ""
#endif
#if( VS_OS_TYPE == VS_OS_WP )
#define VS_OSTYPE       VSOS_WP
#define VS_MODULEEXT    VSMODULEEXT_WP
#define VS_SRVLIBEXT    VSSRVLIBEXT_WP
#define VS_OSALIAS      VSOSALIAS_WP
#define VS_COREPATH     ""
#endif
#if( VS_OS_TYPE == VS_OS_WINRT )
#define VS_OSTYPE       VSOS_WINRT
#define VS_MODULEEXT    VSMODULEEXT_WINRT
#define VS_SRVLIBEXT    VSSRVLIBEXT_WINRT
#define VS_OSALIAS      VSOSALIAS_WINRT
#define VS_COREPATH     ""
#endif
#if( VS_OS_TYPE == VS_OS_WIN10 )
#define VS_OSTYPE       VSOS_WIN10
#define VS_MODULEEXT    VSMODULEEXT_WIN10
#define VS_SRVLIBEXT    VSSRVLIBEXT_WIN10
#define VS_OSALIAS      VSOSALIAS_WIN10
#define VS_COREPATH     ""
#endif
#if( VS_OS_TYPE == VS_OS_LINUX )
#define VS_OSTYPE       VSOS_LINUX
#define VS_MODULEEXT    VSMODULEEXT_LINUX
#define VS_SRVLIBEXT    VSSRVLIBEXT_LINUX
#define VS_OSALIAS      VSOSALIAS_LINUX
#define VS_COREPATH      ""
#endif
#if( VS_OS_TYPE == VS_OS_ANDROID )
#define VS_OSTYPE       VSOS_ANDROID
#define VS_MODULEEXT    VSMODULEEXT_ANDROID
#define VS_SRVLIBEXT    VSSRVLIBEXT_ANDROID
#define VS_OSALIAS      VSOSALIAS_ANDROID
#define VS_COREPATH      "/data/data/com.srplab.starcore/lib/"
#endif
#if( VS_OS_TYPE == VS_OS_ANDROIDV7A )
#define VS_OSTYPE       VSOS_ANDROIDV7A
#define VS_MODULEEXT    VSMODULEEXT_ANDROIDV7A
#define VS_SRVLIBEXT    VSSRVLIBEXT_ANDROIDV7A
#define VS_OSALIAS      VSOSALIAS_ANDROIDV7A
#define VS_COREPATH      "/data/data/com.srplab.starcore/lib/"
#endif
#if( VS_OS_TYPE == VS_OS_ANDROIDX86 )
#define VS_OSTYPE       VSOS_ANDROIDX86
#define VS_MODULEEXT    VSMODULEEXT_ANDROIDX86
#define VS_SRVLIBEXT    VSSRVLIBEXT_ANDROIDX86
#define VS_OSALIAS      VSOSALIAS_ANDROIDX86
#define VS_COREPATH      "/data/data/com.srplab.starcore/lib/"
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
#define VS_OSTYPE       VSOS_IOS
#define VS_MODULEEXT    VSMODULEEXT_IOS
#define VS_SRVLIBEXT    VSSRVLIBEXT_IOS
#define VS_OSALIAS      VSOSALIAS_IOS
#define VS_COREPATH      ""
#endif
#if( VS_OS_TYPE == VS_OS_MACOS )
#define VS_OSTYPE       VSOS_MACOS
#define VS_MODULEEXT    VSMODULEEXT_MACOS
#define VS_SRVLIBEXT    VSSRVLIBEXT_MACOS
#define VS_OSALIAS      VSOSALIAS_MACOS
#define VS_COREPATH     "/usr/local/lib/"  /*note: for macos, libstarcore.dylib is installed at /usr/local/lib*/
#endif

typedef char                VS_BOOL;
typedef char                VS_INT8;
typedef unsigned char       VS_UINT8;
typedef short int           VS_INT16;
typedef unsigned short int  VS_UINT16;
typedef int                 VS_INT32;
typedef unsigned int        VS_UINT32;
typedef float               VS_FLOAT;
typedef double              VS_DOUBLE;

/*#ifdef _UNICODE*/
/*typedef unsigned short int  VS_CHAR;*/
/*#else*/
typedef char                VS_CHAR;
/*#endif*/

typedef int                 VS_LONG;  
typedef unsigned int        VS_ULONG;  

#if defined(ENV_M64)
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
typedef __int64             VS_WORD;
typedef unsigned __int64    VS_UWORD;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS  )
typedef long long int             VS_WORD;
typedef unsigned long long int    VS_UWORD;
#endif
#define VSPLAT_64
#else
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
typedef int                 VS_WORD;
typedef unsigned int        VS_UWORD;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS  )
typedef int                 VS_WORD;
typedef unsigned int        VS_UWORD;
#endif
#define VSPLAT_32
#endif

#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
typedef __int64             VS_INT64;
typedef unsigned __int64    VS_UINT64;
#define INT64_LOWWORD           ((VS_INT64)0xFFFFFFFF00000000)
#define INT64_HIGHWORD          ((VS_INT64)0x00000000FFFFFFFF)
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS  )
typedef long long int           VS_INT64;
typedef unsigned long long int  VS_UINT64;
#define INT64_LOWWORD           ((VS_INT64)0xFFFFFFFF00000000ll)
#define INT64_HIGHWORD          ((VS_INT64)0x00000000FFFFFFFFll)
#endif

typedef void *              VS_HWND;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
typedef UUID                VS_UUID;
typedef WCHAR               VS_WCHAR;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
/*typedef uuid_t              VS_UUID;*/
typedef struct{
    VS_ULONG  Data1;
    VS_UINT16 Data2;
    VS_UINT16 Data3;
    VS_UINT8  Data4[8];
}VS_UUID;
typedef wchar_t             VS_WCHAR;
#endif

typedef void *              VS_HANDLE;
#define VS_INVALID_HANDLE   ((VS_HANDLE)-1)

#if defined(ENV_M64)
typedef VS_UWORD            VS_FLOAT_F;
typedef VS_UWORD            VS_DOUBLE_F;
#else
typedef VS_FLOAT            VS_FLOAT_F;
typedef VS_DOUBLE           VS_DOUBLE_F;
#endif

#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
typedef SYSTEMTIME VS_TIME_T;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
typedef struct{
    VS_UINT16 wYear; 
    VS_UINT16 wMonth; 
    VS_UINT16 wDayOfWeek; 
    VS_UINT16 wDay; 
    VS_UINT16 wHour;
    VS_UINT16 wMinute; 
    VS_UINT16 wSecond; 
    VS_UINT16 wMilliseconds; 
}VS_TIME_T;
#endif

/*--for winrt, there are no definition for socket, for native socket is not supported--*/
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#define WSAEWOULDBLOCK_CONNECT  WSAEWOULDBLOCK
struct _in_addr {
        union {
                struct { VS_UINT8 s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { VS_UINT16 s_w1,s_w2; } S_un_w;
                VS_UINT32 S_addr;
        } S_un;
};

#if !defined(HAS_SOCKADDR)
struct sockaddr_in {
	VS_INT16   sin_family;
	VS_UINT16  sin_port;
	struct  _in_addr sin_addr;
	VS_CHAR    sin_zero[8];
};
struct sockaddr {
	VS_UINT16 sa_family;              /* address family */
	VS_CHAR   sa_data[14];            /* up to 14 bytes of direct address */
};
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
#endif

#define VSSOCKADDR_IN SOCKADDR_IN
#define VSSOCKADDR SOCKADDR
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
struct _in_addr {
        union {
                struct { VS_UINT8  s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { VS_UINT16 s_w1,s_w2; } S_un_w;
                VS_ULONG S_addr;
        } S_un;
};
typedef struct sockaddr SOCKADDR;
typedef struct{
    VS_INT16         sin_family;
    VS_UINT16        sin_port;
    struct in_addr   sin_addr;
    VS_CHAR          sin_zero[8];
}SOCKADDR_IN;
typedef VS_INT32 SOCKET;
typedef struct hostent *LPHOSTENT;
#define VSSOCKADDR_IN SOCKADDR_IN
#define VSSOCKADDR SOCKADDR
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#define WSAEWOULDBLOCK EAGAIN
#define WSAEINVAL      EINVAL
#define WSAEWOULDBLOCK_CONNECT  EINPROGRESS
#endif

#define  VS_TRUE            ((VS_BOOL)1)
#define  VS_FALSE           ((VS_BOOL)0)

#define  VS_OK              ((VS_INT32)0)
#define  VS_FAIL            ((VS_INT32)-1)

#define  VS_MOD(x,y)        ((x)%(y))
#define  VS_DIV(x,y)        ((x)/(y))

/*-------------function call format */
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#define SRPAPI __cdecl
#define SRPSTDCALL __stdcall
#define SRPCALLBACK __cdecl
#define SRPDLLEXPORT __declspec(dllexport)
#define SRPDLLIMPORT __declspec(dllimport)
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
#define SRPAPI
#define SRPSTDCALL
#define SRPCALLBACK
#define SRPDLLEXPORT
#define SRPDLLIMPORT
#endif

#if defined(ENV_M64)
#define UWORD2ULONG(X) ( ((VS_ULONG *)X)[0] + ((VS_ULONG *)X)[1] )
#else
#define UWORD2ULONG(X) ( (VS_ULONG)X )
#endif

#endif
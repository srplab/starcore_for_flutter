/*--
  macro define,
  USE_SRPSOCKET : valid for winrt, for compiler than vs2013 update3, this flaf should be defined
  USR_SRPTHREAD : valid for winrt, wp;  for compiler than vs2013 update3, this flaf should be defined
--*/
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

#include "vs_shell.h"
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#if defined(CBUILDER_FLAG)
#include <utime.h>
#else
#include <sys\utime.h>
#endif
#endif
#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
#include <md5proc_c.h>
#include <android/log.h>
#endif

#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
#if( VS_OS_TYPE != VS_OS_ANDROID && VS_OS_TYPE != VS_OS_ANDROIDV7A && VS_OS_TYPE != VS_OS_ANDROIDX86 && VS_OS_TYPE != VS_OS_LINUX )
    #include <uuid/uuid.h>
#endif
#endif

#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#if defined(USR_SRPTHREAD)
#include "ThreadEmulation.h"
#else
#include <Processthreadsapi.h>
#endif
#include "windows8_shell.h"
#endif

/*--------------------------------------------------------------------
part 1 : file system functions
---------------------------------------------------------------------*/
/*判断文件属性的宏定义*/
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
#define    VS_FILE_ISDIR(mode)    ((mode & S_IFMT) == S_IFDIR)     /* directory */
#define    VS_FILE_ISCHR(mode)    ((mode & S_IFMT) == S_IFCHR)     /* character special */
#define    VS_FILE_ISREG(mode)    ((mode & S_IFMT) == S_IFREG)     /* regular file ,not device file*/
#define    VS_FILE_ISFIFO(mode)   ((mode & S_IFMT) == S_IFIFO)     /* fifo special */
#endif
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#define    VS_FILE_ISDIR(mode)    ((mode & _S_IFMT) == _S_IFDIR)    /* directory */
#define    VS_FILE_ISCHR(mode)    ((mode & _S_IFMT) == _S_IFCHR)    /* character special */
#define    VS_FILE_ISREG(mode)    ((mode & _S_IFMT) == _S_IFREG)    /* regular file ,not device file*/
#define    VS_FILE_ISFIFO(mode)   ((mode & _S_IFMT) == _S_IFIFO)    /* fifo special */
#endif

void vs_file_namechange(VS_CHAR *FileName,VS_CHAR Ch,VS_CHAR NewCh)
{
    VS_CHAR *CharPtr;

	CharPtr = FileName;
	while( (*CharPtr) != 0 ){
		if( (*CharPtr) == Ch )
			(*CharPtr) = NewCh;
		CharPtr++;
	}
	return;
}

VS_INT32 vs_fs_errno_get(void)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    VS_UINT32 ulErrNo;

    ulErrNo = GetLastError() ;
#else
    VS_UINT32 ulErrNo = 0 ;
#endif
    return (ulErrNo | 0x80000000) ;
}

FILE *vs_file_fopenex(const VS_CHAR *FileName,const VS_CHAR *mode,VS_UINT16 opmode )
{
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH] ;
    
    if(FileName == NULL)
        return NULL;
	while( (*FileName) != 0 && (*FileName) == ' ' ) FileName ++;
    VS_STRNCPY(acFileName,FileName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acFileName, '/', '\\' );
#endif
	#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acFileName, '\\', '/' );
    if( vs_file_exist(acFileName) == VS_FALSE ){
        VS_CHAR *CharPtr;

        /*--if create file, we need set file mode--*/
        CharPtr = (VS_CHAR *)mode;
        while( (*CharPtr) != 0 ){
            if( (*CharPtr) == 'a' || (*CharPtr) == 'w' ){
                FILE *hFile;

                hFile = fopen(acFileName,mode);
                if( hFile != NULL ){
                    fclose(hFile);
                    vs_file_chmod(acFileName,opmode);
                }
                break;
            }
            CharPtr ++;
        }
    }
#endif
	return fopen(acFileName,mode);
}

FILE *vs_file_fopen(const VS_CHAR *FileName,const char *mode)
{
    return vs_file_fopenex(FileName,mode,0777);
}

/*----------------------------------------------------------------*/
VS_INT32 _vs_file_stat(const VS_CHAR *FileName, VS_FILE_STAT_T *pstStat)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    {    
        struct _stat stStat ;
        VS_INT32 lStat;
        
        lStat = _stat(FileName,&stStat);
        if ( -1 == lStat )   
            return vs_fs_errno_get() ;
        pstStat->usMode   = stStat.st_mode;
        pstStat->ulSize   = stStat.st_size;

        pstStat->stAccessTime = stStat.st_atime;
        pstStat->stModifyTime = stStat.st_mtime;
        pstStat->stChangTime  = stStat.st_ctime;
/*
        vs_tm_gmt2clock(stStat.st_atime,&pstStat->stAccessTime) ;
        vs_tm_gmt2clock(stStat.st_mtime,&pstStat->stModifyTime) ;
        vs_tm_gmt2clock(stStat.st_ctime,&pstStat->stChangTime) ;
*/
    }
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    {
        struct stat stStat;        

        if ( 0 != stat(FileName, &stStat))
            return vs_fs_errno_get() ;
        pstStat->usMode  = stStat.st_mode;        
        pstStat->ulSize  = (VS_ULONG)stStat.st_size;

        pstStat->stAccessTime = stStat.st_atime;
        pstStat->stModifyTime = stStat.st_mtime;
        pstStat->stChangTime  = stStat.st_ctime;
/*
        vs_tm_gmt2clock(stStat.st_atime,&pstStat->stAccessTime) ;
        vs_tm_gmt2clock(stStat.st_mtime,&pstStat->stModifyTime) ;
        vs_tm_gmt2clock(stStat.st_ctime,&pstStat->stChangTime) ;
*/        
    }    
#endif                         
    return VS_OK;
}    

VS_INT32 vs_file_stat(const VS_CHAR *FileName, VS_FILE_STAT_T *pstStat)
{
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH] ;
    
    if(FileName == NULL)
        return VS_FALSE;
    VS_STRNCPY(acFileName,FileName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acFileName, '/', '\\' );
#endif
	#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acFileName, '\\', '/' );
#endif
	return _vs_file_stat(acFileName,pstStat);
}

/*----------------------------------------------------------------*/
VS_BOOL _vs_file_exist(const VS_CHAR *FileName)
{
	VS_INT32 lFileStat = 0;
	VS_FILE_STAT_T stStat;

	lFileStat = _vs_file_stat(FileName,&stStat);
	if( VS_OK != lFileStat )
		return VS_FALSE;
	return VS_TRUE;
}

VS_BOOL vs_file_exist(const VS_CHAR *FileName)
{
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH] ;
    
    if(FileName == NULL)
        return VS_FALSE;
    VS_STRNCPY(acFileName,FileName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acFileName, '/', '\\' );
#endif
	#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acFileName, '\\', '/' );
#endif
	return _vs_file_exist(acFileName);
}
/*----------------------------------------------------------------*/
VS_INT32 vs_file_delete(const VS_CHAR *FileName)
{
    VS_CHAR   acFileName[ VS_FILE_NAMELENGTH] ;
    VS_BOOL bFileExist;
    VS_INT32 lUnlink = 0;

    if(FileName == NULL)
        return VS_FAIL;
    VS_STRNCPY(acFileName,FileName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acFileName, '/', '\\' );
#endif
	#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acFileName, '\\', '/' );
#endif
	
    bFileExist = _vs_file_exist(acFileName);
    if( VS_FALSE == bFileExist )
        return VS_FAIL;
    /*unlink delete file*/
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    lUnlink = _unlink(acFileName);
    if ( -1 == lUnlink )
		return vs_fs_errno_get() ;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    lUnlink = unlink(acFileName);
    if ( 0 == lUnlink )
		return vs_fs_errno_get() ;
#endif
    return VS_OK ;
}

VS_INT32 vs_file_copy( const VS_CHAR *SrcFileName,const VS_CHAR *DestFileName )
{
    VS_CHAR acSrcFileName[ VS_FILE_NAMELENGTH] ;
    VS_CHAR acDstFileName[ VS_FILE_NAMELENGTH] ;
    
    if((SrcFileName == NULL)||(DestFileName == NULL))
        return VS_FAIL ;
    VS_STRNCPY(acSrcFileName,SrcFileName, VS_FILE_NAMELENGTH) ;
    VS_STRNCPY(acDstFileName,DestFileName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	vs_file_namechange( acSrcFileName, '/', '\\' );
	vs_file_namechange( acDstFileName, '/', '\\' );
	if( CopyFile(acSrcFileName,acDstFileName,VS_FALSE) == VS_FALSE )
		return VS_FAIL;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_MACOS || VS_OS_TYPE == VS_OS_WIN10)
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acSrcFileName, '/', '\\' );
	vs_file_namechange( acDstFileName, '/', '\\' );
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acSrcFileName, '\\', '/' );
	vs_file_namechange( acDstFileName, '\\', '/' );
#endif
	{
		FILE *hSrcFile,*hDesFile;
		VS_UINT8 FileBuf[1024];
		VS_ULONG ReadLength;
		VS_FILE_STAT_T stStat;

		hSrcFile = vs_file_fopen(acSrcFileName,"rb");
		if( hSrcFile == NULL )
			return VS_FAIL;
		hDesFile = vs_file_fopen(acDstFileName,"wb");
		if( hDesFile == NULL )
			return VS_FAIL;
		ReadLength = (VS_ULONG)vs_file_fread(FileBuf,1,1024,hSrcFile);
		while( ReadLength != 0 ){
			vs_file_fwrite(FileBuf,1,ReadLength,hDesFile);
			ReadLength = (VS_ULONG)vs_file_fread(FileBuf,1,1024,hSrcFile);
		}
		vs_file_fclose(hSrcFile);
		vs_file_fclose(hDesFile);

		_vs_file_stat(acSrcFileName, &stStat);
		chmod(acDstFileName,stStat.usMode);
	}
#endif 
    return VS_OK;    
}

VS_ULONG vs_file_size(const VS_CHAR *szFileName)
{
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH] ;
    VS_FILE_STAT_T stStat;
    
    if(szFileName == NULL)
        return VS_FAIL;
    VS_STRNCPY(acFileName,szFileName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acFileName, '/', '\\' );
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acFileName, '\\', '/' );
#endif
    if( _vs_file_stat(acFileName,&stStat) != VS_OK )
        return 0;   
    return stStat.ulSize ;
}

VS_INT32 vs_file_rename(const VS_CHAR *szOldFileName,const VS_CHAR *szNewFileName)
{
    VS_CHAR acOldFileName[ VS_FILE_NAMELENGTH] ;
    VS_CHAR acNewFileName[ VS_FILE_NAMELENGTH] ;
    
    if((szOldFileName==NULL)||(szNewFileName==NULL))
		return VS_FAIL;
    VS_STRNCPY(acOldFileName,szOldFileName, VS_FILE_NAMELENGTH) ;
    VS_STRNCPY(acNewFileName,szNewFileName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acOldFileName, '/', '\\' );
	vs_file_namechange( acNewFileName, '/', '\\' );
	if( rename(acOldFileName,acNewFileName) != 0 )
		return VS_FAIL;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acOldFileName, '\\', '/' );
	vs_file_namechange( acNewFileName, '\\', '/' );
	if( rename(acOldFileName,acNewFileName) != 0 )
		return VS_FAIL;
#endif
    return VS_OK ;
}

VS_BOOL vs_file_chmod(const VS_CHAR *szFileName,VS_UINT16 mode)
{
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH] ;
    
    if(szFileName == NULL)
        return VS_FALSE;
    VS_STRNCPY(acFileName,szFileName, VS_FILE_NAMELENGTH) ;
	vs_file_namechange( acFileName, '\\', '/' );
	if( chmod( acFileName, mode ) == -1 )
		return VS_FALSE;
#endif
	return VS_TRUE;
}

VS_INT32 vs_file_settime(const VS_CHAR *szFileName,VS_ULONG AccessTime,VS_ULONG ModifyTime)
{
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH] ;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	struct utimbuf TimeBuf;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	struct utimbuf TimeBuf;
#endif
    if(szFileName == NULL)
        return VS_FAIL;
    VS_STRNCPY(acFileName,szFileName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    TimeBuf.actime = AccessTime;
    TimeBuf.modtime = ModifyTime;
	vs_file_namechange( acFileName, '/', '\\' );
    if( utime( acFileName, &TimeBuf ) != 0 )
        return VS_FAIL;
    return VS_OK;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    TimeBuf.actime = AccessTime;
    TimeBuf.modtime = ModifyTime;
	vs_file_namechange( acFileName, '\\', '/' );
    if( utime( acFileName, &TimeBuf ) != 0 )
        return VS_FAIL;
    return VS_OK;
#endif
}

VS_BOOL vs_file_isdir(const VS_CHAR *szFileName)
{
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH] ;
    VS_FILE_STAT_T stStat;

    if(szFileName == NULL)
        return VS_FALSE;
    VS_STRNCPY(acFileName,szFileName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acFileName, '/', '\\' );
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acFileName, '\\', '/' );
#endif
    if( _vs_file_stat(acFileName,&stStat) != VS_OK )
        return VS_FALSE; 
	return VS_FILE_ISDIR(stStat.usMode)?VS_TRUE:VS_FALSE;
}

VS_BOOL vs_dir_createex(const VS_CHAR *szDirName,VS_UINT16 opmode)
{
    VS_CHAR acDirName[ VS_FILE_NAMELENGTH] ;
	VS_CHAR acDirName1[ VS_FILE_NAMELENGTH] ;
    
    if(szDirName == NULL)
        return VS_FALSE;
    VS_STRNCPY(acDirName,szDirName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    if( acDirName[1] == ':' && acDirName[2] == 0 )
        return VS_FALSE;
	vs_file_namechange( acDirName, '/', '\\' );
    if( _vs_file_exist(acDirName) == VS_TRUE )
        return VS_TRUE; /*--Exist--*/
	strcpy(acDirName1,acDirName);
	if( _mkdir(acDirName) == -1 ){
		VS_CHAR *CharPtr;
        
		CharPtr = strrchr(acDirName1,'\\');
		if( CharPtr == NULL )
			return VS_FALSE;
		(*CharPtr) = 0;
		if( _vs_file_exist(acDirName1) == VS_FALSE ){
			if( vs_dir_createex(acDirName1,opmode) == VS_FALSE )
				return VS_FALSE;
			if( _mkdir(acDirName) == -1 )
				return VS_FALSE;
		}else
			return  VS_FALSE;
	}
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acDirName, '\\', '/' );
    if( acDirName[0] == '/' && acDirName[1] == 0 )
        return VS_FALSE;
    if( _vs_file_exist(acDirName) == VS_TRUE )
        return VS_TRUE; /*--Exist--*/
	strcpy(acDirName1,acDirName);
	if( mkdir(acDirName,opmode) == -1 ){
		VS_CHAR *CharPtr;
        
		CharPtr = strrchr(acDirName1,'/');
		if( CharPtr == NULL )
			return VS_FALSE;
		(*CharPtr) = 0;
		if( _vs_file_exist(acDirName1) == VS_FALSE ){
			if( vs_dir_createex(acDirName1,opmode) == VS_FALSE )
				return VS_FALSE;
			if( mkdir(acDirName,opmode) == -1 )
				return VS_FALSE;
            else
                vs_file_chmod(acDirName,opmode);
		}else
			return  VS_FALSE;
	}else
        vs_file_chmod(acDirName,opmode);
#endif
	return VS_TRUE;
}

VS_BOOL vs_dir_create(const VS_CHAR *szDirName)
{
    return vs_dir_createex(szDirName,0777);
}

VS_BOOL vs_dir_delete(const VS_CHAR *szDirName)
{
    VS_CHAR acDirName[ VS_FILE_NAMELENGTH] ;
    
    if(szDirName == NULL)
        return VS_FALSE;
    VS_STRNCPY(acDirName,szDirName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acDirName, '/', '\\' );
	if( _rmdir(acDirName) == -1 )
		return VS_FALSE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acDirName, '\\', '/' );
	if( rmdir(acDirName) == -1 )
		return VS_FALSE;
#endif
	return VS_TRUE;
}

VS_BOOL vs_dir_getcwd(VS_CHAR *Buf,VS_ULONG Size)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	if( GetCurrentDirectory(Size,Buf) == 0 )
		return VS_FALSE;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	if( GetCurrentDirectory_win8((int)Size,(char *)Buf) == 0 )
		return VS_FALSE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	if( getcwd( Buf, Size ) == NULL )
		return VS_FALSE;
#endif
	return VS_TRUE;
}

VS_BOOL vs_dir_chdir(const VS_CHAR *szDirName)
{
    VS_CHAR acDirName[ VS_FILE_NAMELENGTH] ;
    
    if(szDirName == NULL)
        return VS_FAIL;
    VS_STRNCPY(acDirName,szDirName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	vs_file_namechange( acDirName, '/', '\\' );
	if( SetCurrentDirectory(acDirName) == 0 )
		return VS_FALSE;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acDirName, '/', '\\' );
	if( SetCurrentDirectory_win8((char *)acDirName) == 0 )
		return VS_FALSE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acDirName, '\\', '/' );
	if( chdir(acDirName) == -1 )
		return VS_FALSE;
#endif
	return VS_TRUE;
}

VS_CHAR *vs_file_strchr(const VS_CHAR *szFileName,VS_CHAR Pattern)
{
    VS_CHAR *CharPtr;

    if( Pattern != '\\' && Pattern != '/' )
        return strchr(szFileName,Pattern);
    if( szFileName == NULL )
        return NULL;
    CharPtr = (VS_CHAR *)szFileName;
    while( (*CharPtr) != 0 ){
        if( (*CharPtr) == '\\' || (*CharPtr) == '/' )
            return CharPtr;
        CharPtr ++;
    }
    return NULL;
}

VS_CHAR *vs_file_strrchr(const VS_CHAR *szFileName,VS_CHAR Pattern)
{
    VS_CHAR *CharPtr;
    VS_WORD Len;

    if( Pattern != '\\' && Pattern != '/' )
        return strrchr(szFileName,Pattern);
    if( szFileName == NULL )
        return NULL;
    Len = strlen(szFileName);
    if( Len == 0 )
        return NULL;
    CharPtr = (VS_CHAR *)&szFileName[Len - 1];
    while( CharPtr >= szFileName ){
        if( (*CharPtr) == '\\' || (*CharPtr) == '/' )
            return CharPtr;
        CharPtr --;
    }
    return NULL;
}

VS_BOOL vs_dir_isfullname(const VS_CHAR *szDirName)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    if( strchr(szDirName,':') != NULL )
        return VS_TRUE;
    return VS_FALSE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    if( szDirName[0] == '\\' || szDirName[0] == '/' )
        return VS_TRUE;
    if( strchr(szDirName,':') != NULL )
        return VS_TRUE;
    return VS_FALSE;
#endif
}

void vs_dir_toroot(VS_CHAR *szDirName)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    szDirName[2] = 0;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    szDirName[1] = 0;
#endif
}

/*--local function--*/
static void _vs_dir_tofullname(VS_CHAR *Root,VS_CHAR *szDirName,VS_CHAR *szRetBuf,VS_ULONG Size)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH],*CharPtr;
	VS_WORD Len;
	
	strcpy(acFileName,Root);
	if( szDirName[0] == '.' ){
		if( szDirName[1] == '.' ){
			CharPtr = vs_file_strrchr(acFileName,'\\');
			if( CharPtr != NULL )
				(*CharPtr) = 0;
			_vs_dir_tofullname(acFileName,&szDirName[3],szRetBuf,Size);
		}else{
			if( szDirName[1] != '/' && szDirName[1] != '\\' ){
        		Len = strlen(acFileName);
	        	if( Len <= 0 || (acFileName[Len-1] != '\\' && acFileName[Len-1] != '/') ){
		        	acFileName[Len] = '\\';
			        acFileName[Len+1] = 0;
        		}
				strncpy(szRetBuf,acFileName,Size);
                strncat(szRetBuf,szDirName,VS_FILE_NAMELENGTH);
				szRetBuf[Size-1] = 0;
			}else{
				Len = strlen(acFileName);
				if( Len > 0 && ( acFileName[Len-1] == '\\' || acFileName[Len-1] == '/' ) )
			        acFileName[Len-1] = 0;
				_vs_dir_tofullname(acFileName,&szDirName[2],szRetBuf,Size);
			}
		}
    }else if( szDirName[0] == '\\' || szDirName[0] == '/' ){
        acFileName[2] = 0;  /* X: */
		strncat(acFileName,szDirName,VS_FILE_NAMELENGTH);
		strncpy(szRetBuf,acFileName,Size);
		szRetBuf[Size-1] = 0;
	}else{
		Len = strlen(acFileName);
		if( Len <= 0 || (acFileName[Len-1] != '\\' && acFileName[Len-1] != '/') ){
			acFileName[Len] = '\\';
			acFileName[Len+1] = 0;
		}
		strncat(acFileName,szDirName,VS_FILE_NAMELENGTH);
		strncpy(szRetBuf,acFileName,Size);
		szRetBuf[Size-1] = 0;
	}
	return;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH],*CharPtr;
	VS_INT32 Len;
	
	strcpy(acFileName,Root);
	if( szDirName[0] == '.' ){
		if( szDirName[1] == '.' ){
			CharPtr = vs_file_strrchr(acFileName,'/');
			if( CharPtr != NULL )
				(*CharPtr) = 0;
			_vs_dir_tofullname(acFileName,&szDirName[3],szRetBuf,Size);
		}else{
			if( szDirName[1] != '/' && szDirName[1] != '\\' ){
        		Len = (VS_INT32)strlen(acFileName);
		        if( Len <= 0 || (acFileName[Len-1] != '/' && acFileName[Len-1] != '\\') ){
        			acFileName[Len] = '/';
		        	acFileName[Len+1] = 0;
        		}
		        strncat(acFileName,szDirName,VS_FILE_NAMELENGTH);
        		strncpy(szRetBuf,acFileName,Size);
        		szRetBuf[Size-1] = 0;
			}else{
				Len = (VS_INT32)strlen(acFileName);
				if( Len > 0 && ( acFileName[Len-1] == '/' || acFileName[Len-1] == '\\' ) )
			        acFileName[Len-1] = 0;
				_vs_dir_tofullname(acFileName,&szDirName[2],szRetBuf,Size);
			}
		}
    }else if( szDirName[0] == '\\' || szDirName[0] == '/' ){
        /* not exist this case under unix */        
	}else{
		Len = (VS_INT32)strlen(acFileName);
		if( Len <= 0 || (acFileName[Len-1] != '/' && acFileName[Len-1] != '\\') ){
			acFileName[Len] = '/';
			acFileName[Len+1] = 0;
		}
		strncat(acFileName,szDirName,VS_FILE_NAMELENGTH);
		strncpy(szRetBuf,acFileName,Size);
		szRetBuf[Size-1] = 0;
	}
	return;
#endif
}

void vs_dir_tofullname(VS_CHAR *szDirName,VS_ULONG Size)
{
	VS_CHAR acFileName[ VS_FILE_NAMELENGTH];

    if( vs_dir_isfullname(szDirName) == VS_TRUE || Size == 0 )
		return;
	vs_dir_getcwd(acFileName,VS_FILE_NAMELENGTH);
	if( vs_string_strcmp(szDirName,".") == 0 ){
		VS_STRNCPY( szDirName, acFileName, Size );
		return;
	}
	_vs_dir_tofullname(acFileName,szDirName,szDirName,Size);
	return;
}

void vs_dir_tofullnameex(const VS_CHAR *RootPath,VS_CHAR *szDirName,VS_ULONG Size)
{
	VS_CHAR acFileName[ VS_FILE_NAMELENGTH];

    if( vs_dir_isfullname(szDirName) == VS_TRUE || Size == 0 )
		return;
    if( RootPath == NULL || strlen(RootPath) == 0 )
    	vs_dir_getcwd(acFileName,VS_FILE_NAMELENGTH);
    else{
        VS_STRNCPY( acFileName, RootPath, VS_FILE_NAMELENGTH );
    }
	if( vs_string_strcmp(szDirName,".") == 0 ){
		VS_STRNCPY( szDirName, acFileName, Size );
		return;
	}
	_vs_dir_tofullname(acFileName,szDirName,szDirName,Size);
	return;
}

/*
void vs_dir_tofullname(VS_CHAR *szDirName,VS_ULONG Size)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH],*CharPtr;
	VS_INT32 Len;

    if( vs_dir_isfullname(szDirName) == VS_TRUE || Size == 0 )
		return;
	if( szDirName[0] == '.' ){
		if( szDirName[1] == '.' ){
			vs_dir_getcwd(acFileName,VS_FILE_NAMELENGTH);
			CharPtr = vs_file_strrchr(acFileName,'\\');
			if( CharPtr != NULL )
				(*CharPtr) = 0;
			strncat(acFileName,&szDirName[2],VS_FILE_NAMELENGTH);
			strncpy(szDirName,acFileName,Size);
			szDirName[Size-1] = 0;
		}else{
			vs_dir_getcwd(acFileName,VS_FILE_NAMELENGTH);
			Len = strlen(acFileName);
			if( acFileName[Len-1] == '\\' )
                acFileName[Len-1] = 0;
			strncat(acFileName,&szDirName[1],VS_FILE_NAMELENGTH);
			strncpy(szDirName,acFileName,Size);
			szDirName[Size-1] = 0;
		}
	}else{
		vs_dir_getcwd(acFileName,VS_FILE_NAMELENGTH);
		Len = strlen(acFileName);
		if( acFileName[Len-1] != '\\' ){
			acFileName[Len] = '\\';
			acFileName[Len+1] = 0;
		}
		strncat(acFileName,szDirName,VS_FILE_NAMELENGTH);
		strncpy(szDirName,acFileName,Size);
		szDirName[Size-1] = 0;
	}
	return;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH],*CharPtr;
	VS_INT32 Len;
	
    if( vs_dir_isfullname(szDirName) == VS_TRUE || Size == 0 )
		return;
	if( szDirName[0] == '.' ){
		if( szDirName[1] == '.' ){
			vs_dir_getcwd(acFileName,VS_FILE_NAMELENGTH);
			CharPtr = vs_file_strrchr(acFileName,'/');
			if( CharPtr != NULL )
				(*CharPtr) = 0;
			strncat(acFileName,&szDirName[2],VS_FILE_NAMELENGTH);
			strncpy(szDirName,acFileName,Size);
			szDirName[Size-1] = 0;
		}else{
			vs_dir_getcwd(acFileName,VS_FILE_NAMELENGTH);
			Len = strlen(acFileName);
			if( acFileName[Len-1] == '/' )
                acFileName[Len-1] = 0;
			strncat(acFileName,&szDirName[1],VS_FILE_NAMELENGTH);
			strncpy(szDirName,acFileName,Size);
			szDirName[Size-1] = 0;
		}
	}else{
		vs_dir_getcwd(acFileName,VS_FILE_NAMELENGTH);
		Len = strlen(acFileName);
		if( acFileName[Len-1] != '/' ){
			acFileName[Len] = '/';
			acFileName[Len+1] = 0;
		}
		strncat(acFileName,szDirName,VS_FILE_NAMELENGTH);
		strncpy(szDirName,acFileName,Size);
		szDirName[Size-1] = 0;
	}
	return;
#endif
}
*/

void vs_file_log(const VS_CHAR *FileName,const VS_CHAR *format,...)
{
    va_list tArgList;
	FILE *hFile;

    va_start(tArgList, format);
    if( vs_file_exist(FileName) == VS_TRUE ){
    	hFile = vs_file_fopen(FileName,"a");
    }else{
        hFile = vs_file_fopen(FileName,"wt");
    }
    if( hFile != NULL ){
    	vs_file_vfprintf(hFile,format,tArgList);
	    vs_file_fclose(hFile);
    }
    va_end(tArgList);
}


static VS_BOOL vs_file_findfirst_firstmatch(VS_CHAR *Pattern,VS_CHAR *FileName,VS_INT32 Length)
{
	VS_CHAR *CharPtr,*DesCharPtr;
	VS_INT32 i;

	//Length is max search for FileName
	CharPtr = Pattern;
	DesCharPtr = FileName;
	i = 0;
	while( i < Length ){
		if( (*CharPtr) == '*' )
			return VS_TRUE;
		if( (*CharPtr) == 0 )
			return VS_FALSE;  // FileName is long
		if( (*CharPtr) != (*DesCharPtr) )
			return VS_FALSE;
		CharPtr ++;
		DesCharPtr ++;
		i++;
	}
	if( (*CharPtr) == 0 )
		return VS_TRUE;  //same
	return VS_FALSE;
}

VS_HANDLE vs_file_findfirst(const VS_CHAR *szFileName,VS_FILE_FIND_T *pstFind)
{
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH] ;
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	VS_WCHAR wacFileName[ VS_FILE_NAMELENGTH] ;
	WIN32_FIND_DATAW wFindData;
	HANDLE result;
#endif

    if(szFileName == NULL)
        return VS_INVALID_HANDLE;
    VS_STRNCPY(acFileName,szFileName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS)
	vs_file_namechange( acFileName, '/', '\\' );
	return FindFirstFile(acFileName,pstFind);
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acFileName, '/', '\\' );
	MultiByteToWideChar( CP_ACP, MB_ERR_INVALID_CHARS, acFileName, -1,wacFileName, VS_FILE_NAMELENGTH );
	result = FindFirstFileExW(wacFileName,FindExInfoStandard,&wFindData,FindExSearchNameMatch,NULL, 0);
	if( result == INVALID_HANDLE_VALUE )
		return INVALID_HANDLE_VALUE ;
	pstFind ->dwFileAttributes = wFindData.dwFileAttributes;
	pstFind ->ftCreationTime = wFindData.ftCreationTime;
	pstFind ->ftLastAccessTime = wFindData.ftLastAccessTime;
	pstFind ->ftLastWriteTime = wFindData.ftLastWriteTime;
	pstFind ->nFileSizeHigh = wFindData.nFileSizeHigh;
	pstFind ->nFileSizeLow = wFindData.nFileSizeLow;
	pstFind ->dwReserved0 = wFindData.dwReserved0;
	pstFind ->dwReserved1 = wFindData.dwReserved1;
	WideCharToMultiByte( CP_ACP, 0, wFindData.cFileName, -1, pstFind ->cFileName, MAX_PATH, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, wFindData.cAlternateFileName, -1, pstFind ->cAlternateFileName, 14, NULL, NULL );
	return result;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acFileName, '\\', '/' );
	{
		VS_CHAR acFileName1[VS_FILE_NAMELENGTH],*CharPtr,*CharPtr1;
		struct dirent * ptr;
		DIR *dirptr;
		VS_DIR_T *vs_dir;
        VS_INT32 PatternFirstLength;

		strcpy(acFileName1,acFileName);
		CharPtr = strrchr(acFileName1,'/');
		if( CharPtr == NULL ){
			getcwd(acFileName1,VS_FILE_NAMELENGTH);
			CharPtr = acFileName;
		}else{
			(*CharPtr) = 0;
			CharPtr ++;
		}
		dirptr = opendir(acFileName1);
		if( dirptr == NULL )
			return VS_INVALID_HANDLE;  /*--fail--*/
		strcpy( pstFind -> cDirName, acFileName1 );
		CharPtr1 = strrchr(CharPtr,'.');
		pstFind -> FirstMask = VS_FALSE;
		pstFind -> ExtendMask = VS_FALSE;
		if( CharPtr1 == NULL ){
			strcpy(pstFind -> cPatternFirst,CharPtr);
			//pstFind -> ExtendMask = VS_TRUE;  //2014/08/31注释掉，否则会出现查找不正确
			if( strcmp(pstFind -> cPatternFirst,"*") == 0 )
				pstFind -> FirstMask = VS_TRUE;
		}else{
			(*CharPtr1) = 0;
			strcpy(pstFind -> cPatternFirst,CharPtr);
			CharPtr1 ++;
			strcpy(pstFind -> cPatternExtend,CharPtr1);
			if( strcmp(pstFind -> cPatternFirst,"*") == 0 )
				pstFind -> FirstMask = VS_TRUE;
			if( strcmp(pstFind -> cPatternExtend,"*") == 0 )
				pstFind -> ExtendMask = VS_TRUE;
		}
        PatternFirstLength = (VS_INT32)strlen(pstFind -> cPatternFirst);
		/*--read first file--*/
		vs_dir = (VS_DIR_T *)malloc(sizeof(VS_DIR_T));
		vs_dir ->FindData = (*pstFind);
		ptr = readdir(dirptr);
		while( ptr != NULL ){
			if( pstFind -> FirstMask == VS_TRUE ){
				if( pstFind -> ExtendMask == VS_TRUE ){
					VS_STRNCPY(pstFind ->cFileName,ptr -> d_name, VS_FILE_NAMELENGTH) ;
					vs_dir ->dirptr = dirptr;
					return (VS_HANDLE)vs_dir;
				}else{
					CharPtr = strrchr(ptr -> d_name,'.');
					if( CharPtr == NULL ){
						if (strlen(pstFind->cPatternExtend) == 0){  //2014/08/31增加，都没有扩展名，则表示匹配
							VS_STRNCPY(pstFind ->cFileName,ptr -> d_name, VS_FILE_NAMELENGTH) ;
							vs_dir ->dirptr = dirptr;
							return (VS_HANDLE)vs_dir;
						}
						//---find next;
						ptr = readdir(dirptr);
						continue;
					}	
					CharPtr ++;
					if( vs_string_stricmp(pstFind -> cPatternExtend,CharPtr) == 0 ){
						VS_STRNCPY(pstFind ->cFileName,ptr -> d_name, VS_FILE_NAMELENGTH) ;
						vs_dir ->dirptr = dirptr;
						return (VS_HANDLE)vs_dir;
					}				
				}
			}else{
				CharPtr = strrchr(ptr -> d_name,'.');
				if( CharPtr == NULL ){
					if( pstFind -> ExtendMask == VS_TRUE )
						CharPtr = &ptr -> d_name[strlen(ptr -> d_name)];
					else{
						if (strlen(pstFind->cPatternExtend) == 0){  //2014/08/31增加，都没有扩展名，则表示匹配
							CharPtr =  &ptr -> d_name[strlen(ptr -> d_name)];
						}
						else{
							//---find next;
							ptr = readdir(dirptr);
							continue;
						}
					}
				}
				{
					//if( PatternFirstLength != CharPtr-ptr -> d_name || vs_string_strnicmp(pstFind -> cPatternFirst,ptr -> d_name,CharPtr-ptr -> d_name) != 0 ){
					if( vs_file_findfirst_firstmatch(pstFind -> cPatternFirst,ptr -> d_name,(VS_INT32)(CharPtr-ptr -> d_name)) == VS_FALSE ){
						//---find next;
						ptr = readdir(dirptr);
						continue;
					}else{	
						if( pstFind -> ExtendMask == VS_TRUE ){
							VS_STRNCPY(pstFind ->cFileName,ptr -> d_name, VS_FILE_NAMELENGTH) ;
							vs_dir ->dirptr = dirptr;
							return (VS_HANDLE)vs_dir;
						}else{
							if ( strlen(pstFind->cPatternExtend) == 0 ){  //2014/08/31增加，都没有扩展名，则表示匹配
								if (strrchr(ptr -> d_name, '.') == NULL){
									VS_STRNCPY(pstFind ->cFileName,ptr -> d_name, VS_FILE_NAMELENGTH) ;
									vs_dir ->dirptr = dirptr;
									return (VS_HANDLE)vs_dir;
								}
								else{
									//---find next;
									ptr = readdir(dirptr);
									continue;
								}
							}
							CharPtr ++;
							if( vs_string_stricmp(pstFind -> cPatternExtend,CharPtr) == 0 ){
								VS_STRNCPY(pstFind ->cFileName,ptr -> d_name, VS_FILE_NAMELENGTH) ;
								vs_dir ->dirptr = dirptr;
								return (VS_HANDLE)vs_dir;
							}				
						}
					}
				}
			}
			ptr = readdir(dirptr);
		}
		closedir(dirptr);
		free(vs_dir);
		return VS_INVALID_HANDLE;
	}
#endif
}

VS_BOOL vs_file_findnext(const VS_HANDLE FindHandle,VS_FILE_FIND_T *pstFind)
{
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	WIN32_FIND_DATAW wFindData;
#endif

#if( VS_OS_TYPE == VS_OS_WINDOWS )
	if( FindHandle == NULL )
		return VS_FALSE;
	if( FindNextFile(FindHandle,pstFind) != 0 )
		return VS_TRUE;
	return VS_FALSE;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	if( FindHandle == NULL )
		return VS_FALSE;
	if( FindNextFileW(FindHandle,&wFindData) == 0 )
		return VS_FALSE;
	pstFind ->dwFileAttributes = wFindData.dwFileAttributes;
	pstFind ->ftCreationTime = wFindData.ftCreationTime;
	pstFind ->ftLastAccessTime = wFindData.ftLastAccessTime;
	pstFind ->ftLastWriteTime = wFindData.ftLastWriteTime;
	pstFind ->nFileSizeHigh = wFindData.nFileSizeHigh;
	pstFind ->nFileSizeLow = wFindData.nFileSizeLow;
	pstFind ->dwReserved0 = wFindData.dwReserved0;
	pstFind ->dwReserved1 = wFindData.dwReserved1;
	WideCharToMultiByte( CP_ACP, 0, wFindData.cFileName, -1, pstFind ->cFileName, MAX_PATH, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, wFindData.cAlternateFileName, -1, pstFind ->cAlternateFileName, 14, NULL, NULL );
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	{
		VS_CHAR *CharPtr;
		struct dirent * ptr;
		VS_DIR_T *vs_dir;
        VS_INT32 PatternFirstLength;

		if( FindHandle == NULL )
			return VS_FALSE;
		vs_dir = (VS_DIR_T *)FindHandle;
        PatternFirstLength = (VS_INT32)strlen(vs_dir->FindData.cPatternFirst);
		/*--read first file--*/
		ptr = readdir((DIR *)vs_dir ->dirptr);
		while( ptr != NULL ){
			if( vs_dir->FindData.FirstMask == VS_TRUE ){
				if( vs_dir->FindData.ExtendMask == VS_TRUE ){
					VS_STRNCPY(pstFind ->cFileName,ptr -> d_name, VS_FILE_NAMELENGTH) ;
					return VS_TRUE;
				}else{
					CharPtr = strrchr(ptr -> d_name,'.');
					if( CharPtr == NULL ){
						if (strlen(vs_dir->FindData.cPatternExtend) == 0){  //2014/08/31增加，都没有扩展名，则表示匹配
							VS_STRNCPY(pstFind ->cFileName,ptr -> d_name, VS_FILE_NAMELENGTH) ;
							return VS_TRUE;
						}
						//---find next;
						ptr = readdir((DIR *)vs_dir ->dirptr);
						continue;
					}	
					CharPtr ++;
					if( vs_string_stricmp(vs_dir->FindData.cPatternExtend,CharPtr) == 0 ){
						VS_STRNCPY(pstFind ->cFileName,ptr -> d_name, VS_FILE_NAMELENGTH) ;
						return VS_TRUE;
					}				
				}
			}else{
				CharPtr = strrchr(ptr -> d_name,'.');
				if( CharPtr == NULL ){
					if( pstFind -> ExtendMask == VS_TRUE )
						CharPtr = &ptr -> d_name[strlen(ptr -> d_name)];
					else{
						if (strlen(vs_dir->FindData.cPatternExtend) == 0){  //2014/08/31增加，都没有扩展名，则表示匹配
							CharPtr =  &ptr -> d_name[strlen(ptr -> d_name)];
						}
						else{
							//---find next;
							ptr = readdir((DIR *)vs_dir ->dirptr);
							continue;
						}
					}					
				}
				{
					//if( PatternFirstLength != CharPtr-ptr -> d_name || vs_string_strnicmp(vs_dir->FindData.cPatternFirst,ptr -> d_name,CharPtr-ptr -> d_name) != 0 ){
					if( vs_file_findfirst_firstmatch(vs_dir->FindData.cPatternFirst,ptr -> d_name,(VS_INT32)(CharPtr-ptr -> d_name)) == VS_FALSE ){
						//---find next;
						ptr = readdir((DIR *)vs_dir ->dirptr);
						continue;
					}else{	
						if( vs_dir->FindData.ExtendMask == VS_TRUE ){
							VS_STRNCPY(pstFind ->cFileName,ptr -> d_name, VS_FILE_NAMELENGTH) ;
							return VS_TRUE;
						}else{
							if ( strlen(vs_dir->FindData.cPatternExtend) == 0 ){  //2014/08/31增加，都没有扩展名，则表示匹配
								if (strrchr(ptr -> d_name, '.') == NULL){
									VS_STRNCPY(pstFind ->cFileName,ptr -> d_name, VS_FILE_NAMELENGTH) ;
									return VS_TRUE;
								}
								else{
									//---find next;
									ptr = readdir((DIR *)vs_dir ->dirptr);
									continue;
								}
							}
							CharPtr ++;
							if( vs_string_stricmp(vs_dir->FindData.cPatternExtend,CharPtr) == 0 ){
								VS_STRNCPY(pstFind ->cFileName,ptr -> d_name, VS_FILE_NAMELENGTH) ;
								return VS_TRUE;
							}				
						}
					}
				}
			}
			ptr = readdir((DIR *)vs_dir ->dirptr);
		}
		return VS_FALSE;
	}
#endif
}

VS_BOOL vs_file_findclose(const VS_HANDLE FindHandle)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	if( FindHandle == NULL )
		return VS_FALSE;
	if( FindClose(FindHandle) != 0 )
		return VS_TRUE;
	return VS_FALSE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	VS_DIR_T *vs_dir;

	if( FindHandle == NULL )
		return VS_FALSE;
	vs_dir = (VS_DIR_T *)FindHandle;
	closedir((DIR *)vs_dir ->dirptr);
	free(vs_dir);
	return VS_TRUE;
#endif
}

VS_BOOL vs_file_isfinddir(const VS_FILE_FIND_T *pstFind)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	if( (pstFind -> dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
		return VS_TRUE;
	return VS_FALSE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    VS_CHAR acFileName[ VS_FILE_NAMELENGTH] ;
	VS_FILE_STAT_T stStat;

	strcpy(acFileName,pstFind -> cDirName);
	strcat(acFileName,"/");
	strcat(acFileName,pstFind -> cFileName);
    if( _vs_file_stat(acFileName,&stStat) != VS_OK )
        return VS_FALSE; 
	return VS_FILE_ISDIR(stStat.usMode)?VS_TRUE:VS_FALSE; 
#endif
}

/*--------------------------------------------------------------------
part 2 : time functions
----------------------------------------------------------------------*/
VS_INT32 vs_tm_time2clock(VS_TM_SECOND_T *pstSecMillisec,VS_TIME_T  *ptClock)
{
	struct tm *newtime;

	newtime = localtime( &pstSecMillisec ->lSecond );
	if( newtime == NULL ){
		ptClock -> wYear = 0;
		ptClock -> wMonth = 0;
		ptClock -> wDayOfWeek = 0;
		ptClock -> wDay = 0;
		ptClock -> wHour = 0;
		ptClock -> wMinute = 0;
		ptClock -> wSecond = 0;
		ptClock -> wMilliseconds = 0;
		return VS_FAIL;
	}
	ptClock -> wYear = newtime -> tm_year + 1900;
	ptClock -> wMonth = newtime -> tm_mon + 1;
	ptClock -> wDayOfWeek = newtime -> tm_wday + 1;
	ptClock -> wDay = newtime -> tm_mday;
	ptClock -> wHour = newtime -> tm_hour;
	ptClock -> wMinute = newtime -> tm_min;
	ptClock -> wSecond = newtime -> tm_sec;
	ptClock -> wMilliseconds = pstSecMillisec -> usMilliseconds;
    return VS_OK;
}

VS_INT32 vs_tm_clock2time(VS_TIME_T  *ptClock,VS_TM_SECOND_T *pstSecMillisec)
{
	struct tm newtime;
	time_t Value;

	newtime.tm_year = ptClock -> wYear - 1900;
	newtime.tm_mon = ptClock -> wMonth - 1;
	newtime.tm_wday = ptClock -> wDayOfWeek - 1;
	newtime.tm_mday = ptClock -> wDay;
	newtime.tm_hour = ptClock -> wHour;
	newtime.tm_min = ptClock -> wMinute;
	newtime.tm_sec = ptClock -> wSecond;
	Value = mktime(&newtime);
	pstSecMillisec ->lSecond = Value;
	pstSecMillisec -> usMilliseconds = ptClock -> wMilliseconds;
    return VS_OK;
}

VS_INT32 vs_tm_gmt2clock(time_t ulGmtSeconds,VS_TIME_T *pstClock)
{
	struct tm *newtime;
	time_t temp;

	newtime = gmtime( (time_t *)&ulGmtSeconds );
	if( newtime == NULL ){
		pstClock -> wYear = 0;
		pstClock -> wMonth = 0;
		pstClock -> wDayOfWeek = 0;
		pstClock -> wDay = 0;
		pstClock -> wHour = 0;
		pstClock -> wMinute = 0;
		pstClock -> wSecond = 0;
		pstClock -> wMilliseconds = 0;
		return VS_FAIL;
	}
	temp = mktime(newtime);
	ulGmtSeconds = ulGmtSeconds + (VS_LONG)((VS_ULONG)ulGmtSeconds-temp);
	newtime = localtime( (time_t *)&ulGmtSeconds );
	if( newtime == NULL ){
		pstClock -> wYear = 0;
		pstClock -> wMonth = 0;
		pstClock -> wDayOfWeek = 0;
		pstClock -> wDay = 0;
		pstClock -> wHour = 0;
		pstClock -> wMinute = 0;
		pstClock -> wSecond = 0;
		pstClock -> wMilliseconds = 0;
		return VS_FAIL;
	}
	pstClock -> wYear = newtime -> tm_year + 1900;
	pstClock -> wMonth = newtime -> tm_mon + 1;
	pstClock -> wDayOfWeek = newtime -> tm_wday + 1;
	pstClock -> wDay = newtime -> tm_mday;
	pstClock -> wHour = newtime -> tm_hour;
	pstClock -> wMinute = newtime -> tm_min;
	pstClock -> wSecond = newtime -> tm_sec;
	pstClock -> wMilliseconds = 0;
    return VS_OK;
}

static VS_UWORD in_vs_tm_gettickcount(void)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS)
    return GetTickCount();
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	return (VS_UWORD)GetTickCount64();
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
    struct timespec tp;
    VS_ULONG Count;

/*    clock_gettime(CLOCK_MONOTONIC, &tp);  CLOCK_PROCESS_CPUTIME_ID CLOCK_REALTIME,  using CLOCK_MONOTONIC may error on vnware linux, by srplab*/
    clock_gettime(CLOCK_REALTIME, &tp);
    Count = (VS_ULONG)(tp.tv_sec*1000) + (VS_ULONG)(tp.tv_nsec/1000000);
    return Count;
#endif    
#if( VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    struct timeval muh2;
    struct timespec tp;
    VS_ULONG Count;
    gettimeofday(&muh2, NULL);
    tp.tv_sec = muh2.tv_sec; tp.tv_nsec = muh2.tv_usec * 1000; 
    Count = (VS_ULONG)(tp.tv_sec*1000) + (VS_ULONG)(tp.tv_nsec/1000000);
    return Count;    
#endif

}

VS_INT64 vs_tm_gettickcount64(void)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    LARGE_INTEGER LocalLargeIntFreq;
	LARGE_INTEGER LocalLargeInt;
	VS_UINT64 Count;
    if( QueryPerformanceFrequency( &LocalLargeIntFreq ) != 0 ){
		QueryPerformanceCounter( &LocalLargeInt );
		Count = (VS_UINT64)LocalLargeInt.QuadPart * 1000 / (VS_UINT64)LocalLargeIntFreq.QuadPart;
		return (VS_INT64)Count;
    }else{
		/*--does not support high resolution timer--*/
#if( VS_OS_TYPE == VS_OS_WINDOWS)
        return (VS_INT64)(VS_ULONG)GetTickCount();
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	    return (VS_INT64)(VS_UWORD)GetTickCount64();
#endif
    }
#endif  
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
    struct timespec tp;
    VS_UINT64 Count;

/*    clock_gettime(CLOCK_MONOTONIC, &tp);  CLOCK_PROCESS_CPUTIME_ID CLOCK_REALTIME,  using CLOCK_MONOTONIC may error on vnware linux, by srplab*/
    clock_gettime(CLOCK_REALTIME, &tp);
	Count = (VS_UINT64)(VS_ULONG)tp.tv_sec;
	Count = Count * 1000 + (tp.tv_nsec/1000000);
    return (VS_INT64)Count;
#endif    
#if( VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    struct timeval muh2;
    VS_ULONG Count;
    gettimeofday(&muh2, NULL);
	Count = (VS_UINT64)(VS_ULONG)muh2.tv_sec;
	Count = Count * 1000 + (muh2.tv_usec/1000);
    return (VS_INT64)Count;  
#endif
}

VS_UWORD vs_tm_gettickcount(void)
{
#if defined(ENV_M64)
	return (VS_UWORD)vs_tm_gettickcount64();
#else
	return (VS_ULONG)(((VS_UINT64)vs_tm_gettickcount64()) & (VS_UINT64)0x00000000FFFFFFFF);
#endif
}

VS_INT64 vs_tm_gettickcount_us(void)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    LARGE_INTEGER LocalLargeIntFreq;
	LARGE_INTEGER LocalLargeInt;
	VS_UINT64 Count;
    if( QueryPerformanceFrequency( &LocalLargeIntFreq ) != 0 ){
		QueryPerformanceCounter( &LocalLargeInt );
		Count = (VS_UINT64)LocalLargeInt.QuadPart * 1000000 / (VS_UINT64)LocalLargeIntFreq.QuadPart;
		return (VS_INT64)Count;
    }else{
		/*--does not support high resolution timer--*/
#if( VS_OS_TYPE == VS_OS_WINDOWS)
        return (VS_INT64)((VS_ULONG)GetTickCount() * 1000);
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	    return (VS_INT64)((VS_UWORD)GetTickCount64() * 1000);
#endif
    }
#endif  
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
    struct timespec tp;
    VS_UINT64 Count;

/*    clock_gettime(CLOCK_MONOTONIC, &tp);  CLOCK_PROCESS_CPUTIME_ID CLOCK_REALTIME,  using CLOCK_MONOTONIC may error on vnware linux, by srplab*/
    clock_gettime(CLOCK_REALTIME, &tp);
	Count = (VS_UINT64)(VS_ULONG)tp.tv_sec;
	Count = Count * 1000000 + (tp.tv_nsec/1000);
    return (VS_INT64)Count;
#endif    
#if( VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    struct timeval muh2;
    VS_ULONG Count;
    gettimeofday(&muh2, NULL);
	Count = (VS_UINT64)(VS_ULONG)muh2.tv_sec;
	Count = Count * 1000000 + muh2.tv_usec;
    return (VS_INT64)Count;  
#endif
}

void vs_tm_getlocaltime(VS_TIME_T *Time)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    GetLocalTime(Time);
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    time_t timep;
    struct tm *newtime;
    time(&timep);
    newtime=localtime(&timep);
	Time -> wYear = newtime -> tm_year + 1900;
	Time -> wMonth = newtime -> tm_mon + 1;
	Time -> wDayOfWeek = newtime -> tm_wday + 1;
	Time -> wDay = newtime -> tm_mday;
	Time -> wHour = newtime -> tm_hour;
	Time -> wMinute = newtime -> tm_min;
	Time -> wSecond = newtime -> tm_sec;
	Time -> wMilliseconds = 0;
#endif
}
/*--------------------------------------------------------------------
part 3 : string
----------------------------------------------------------------------*/
VS_INT32 vs_string_snprintf( VS_CHAR *buffer, size_t count, const VS_CHAR *format, ... )
{
	va_list argList;
	VS_INT32 result;

	va_start( argList, format);
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	result = _vsnprintf(buffer, count, format, argList );
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	result = vsnprintf(buffer, count, format, argList );
#endif
	if( count > 0 )
		buffer[count-1] = 0;
	va_end(argList);
	return result;
}

VS_INT32 vs_string_vsnprintf( VS_CHAR *buffer, size_t count, const VS_CHAR *format, va_list argptr )
{
	VS_INT32 result;

#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	result = _vsnprintf(buffer, count, format, argptr );
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	result = vsnprintf(buffer, count, format, argptr );
#endif
	if( count > 0 )
		buffer[count-1] = 0;
	return result;
}

size_t vs_string_strlen( const VS_CHAR *string )
{
	if( string == NULL )
		return 0;
	return strlen(string);
}

VS_INT32 vs_string_strcmp( const VS_CHAR *string1, const VS_CHAR *string2 )
{
	if( string1 == NULL )
		return -1;
	if( string2 == NULL )
		return 1;
	return strcmp(string1,string2);
}


/*--------------------------------------------------------------------
part 3 : dll functions
----------------------------------------------------------------------*/
static VS_BOOL ShareLibraryTempPathFlag = VS_FALSE;
static VS_CHAR ShareLibraryTempPath[512];
void vs_dll_settemppath(const VS_CHAR *Path)
{
    if( Path == NULL )
        ShareLibraryTempPath[0] = 0;
    else{
        VS_STRNCPY(ShareLibraryTempPath,Path,512);
    }
    ShareLibraryTempPathFlag = VS_TRUE;
}

static VS_HANDLE _vs_dll_open(const VS_CHAR *ModuleName,VS_INT32 Flag,VS_CHAR **ErrorInfo)
{
    VS_CHAR acModuleName[ VS_FILE_NAMELENGTH] ;
    VS_HANDLE Handle;
    static VS_CHAR ErrorBuf[1024];
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	VS_WCHAR wacModuleName[ VS_FILE_NAMELENGTH] ;
	VS_CHAR PackagePath[ VS_FILE_NAMELENGTH] ;
#endif
#if( VS_OS_TYPE == VS_OS_WINDOWS )
    UINT oldErrorMode;
#endif

    ErrorBuf[0] = 0;
    if( ErrorInfo != NULL )
        (*ErrorInfo) = ErrorBuf;
    if(ModuleName==NULL)
		return NULL;
    VS_STRNCPY(acModuleName,ModuleName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	vs_file_namechange( acModuleName, '/', '\\' );
    oldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    SetErrorMode(oldErrorMode | SEM_FAILCRITICALERRORS);
	Handle = LoadLibrary(ModuleName);  /*changed at 2018/09/14 */
	if( Handle == NULL )
	    Handle = LoadLibraryEx(ModuleName,NULL,LOAD_WITH_ALTERED_SEARCH_PATH);
    SetErrorMode(oldErrorMode);
	if( Handle == NULL )
		vs_string_snprintf(ErrorBuf,1024,"error code = %u\n",(VS_UINT32)GetLastError());
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acModuleName, '/', '\\' );
	GetInstall_Dir_win8(PackagePath,VS_FILE_NAMELENGTH);
	if( vs_string_strnicmp(PackagePath,acModuleName,strlen(PackagePath)) == 0 )
		MultiByteToWideChar( CP_ACP, MB_ERR_INVALID_CHARS, &acModuleName[strlen(PackagePath)+1], -1,wacModuleName, VS_FILE_NAMELENGTH );
	else
		MultiByteToWideChar( CP_ACP, MB_ERR_INVALID_CHARS, acModuleName, -1,wacModuleName, VS_FILE_NAMELENGTH );
	Handle = LoadPackagedLibrary(wacModuleName,0);
	if( Handle == NULL )
        vs_string_snprintf(ErrorBuf,1024,"error code = %u\n",GetLastError());
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_MACOS )
	if( vs_file_strchr(ModuleName,'/') == NULL ){
		//--search current path
		vs_dir_getcwd(acModuleName,VS_FILE_NAMELENGTH);
		strcat(acModuleName,"/");
		strcat(acModuleName,ModuleName);
		if( vs_file_exist(acModuleName) == VS_FALSE ){
			VS_STRNCPY(acModuleName,ModuleName, VS_FILE_NAMELENGTH) ;
			vs_file_namechange( acModuleName, '\\', '/' );
		}
	}else{
		vs_file_namechange( acModuleName, '\\', '/' );
	}
	Handle = dlopen(acModuleName,Flag);
	if( Handle == NULL ){
		VS_CHAR *LocalErrorInfo = (VS_CHAR *)dlerror();
		if( LocalErrorInfo != NULL){
/*
            if( vs_file_exist(acModuleName) == VS_TRUE ){
    			printf("%s\n",LocalErrorInfo);
            }
*/            
#if defined(DEBUG) || defined(_DEBUG)
			printf("%s\n",LocalErrorInfo);
#endif
            VS_STRNCPY(ErrorBuf,LocalErrorInfo,1024);
        }
	}
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
	vs_file_namechange( acModuleName, '\\', '/' );
	Handle = dlopen(acModuleName,Flag);
	if( Handle == NULL ){
        /*--for ios, if dll is not load ok, we return pseudohandle--*/
        return VS_DLL_PSEUDOHANDLE;
	}
#endif
#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
    /*for android, sharelib can not load from sdcard*/
	vs_file_namechange( acModuleName, '\\', '/' );

    if( vs_string_strnicmp(acModuleName,"/sdcard",strlen("/sdcard")) != 0 ){
    	Handle = dlopen(acModuleName,Flag);
    }else{
        VS_CHAR acLocalModuleName[ VS_FILE_NAMELENGTH] ;
        VS_CHAR acLocalPathName[ VS_FILE_NAMELENGTH] ;
        VS_CHAR *CharPtr;

        if( ShareLibraryTempPathFlag == VS_FALSE || strlen(ShareLibraryTempPath) == 0 ){
             /* strcpy(acLocalModuleName,"/data/data/com.srplab.starcore/files/sdcard"); */
             /*--- removed at 2013/02/17, for not create file by default */
#if defined(DEBUG) || defined(_DEBUG)
    		printf("%s\n","CoreOperationPath is not set, can not load share library from sdcard");
#endif
            VS_STRNCPY(ErrorBuf,"CoreOperationPath is not set, can not load share library from sdcard",1024);
            return NULL;
        }else
            sprintf(acLocalModuleName,"%s/sdcard",ShareLibraryTempPath);
        vs_dir_create(acLocalModuleName);
        strcat(acLocalModuleName,&acModuleName[strlen("/sdcard")]);
        if( vs_file_exist( acLocalModuleName ) == VS_FALSE ){
            /*--check directory is exist or not*/
            strcpy(acLocalPathName,acLocalModuleName);
            CharPtr = vs_file_strrchr(acLocalPathName,'/');
            if( CharPtr != NULL )
                (*CharPtr) = 0;
            vs_dir_create(acLocalPathName);
            /*--copy file--*/
            vs_file_copy(acModuleName,acLocalModuleName);
            chmod(acLocalModuleName, 0777);
        	Handle = dlopen(acLocalModuleName,Flag);
            strcpy(acModuleName,acLocalModuleName);
        }else{
            /*--load and check MD5--*/
            FILE *hFile;
            VS_INT32 SrcLength,DesLength;
            VS_INT8 *SrcBuf,*DesBuf;

            hFile = vs_file_fopen(acModuleName,"rb" );
            if( hFile == NULL )
                return NULL;
            fseek( hFile, 0, SEEK_END );
            SrcLength = ftell( hFile );
            fseek( hFile, 0, SEEK_SET );
            if( SrcLength == 0 ){
                fclose(hFile);
                return NULL;
            }
            SrcBuf = NULL;
            DesBuf = NULL;
            SrcBuf = (VS_INT8 *)malloc(SrcLength);
            fread(SrcBuf,1,SrcLength,hFile);
            fclose(hFile);
            //===
            hFile = vs_file_fopen(acLocalModuleName,"rb" );
            if( hFile != NULL ){
                fseek( hFile, 0, SEEK_END );
                DesLength = ftell( hFile );
                fseek( hFile, 0, SEEK_SET );
                if( DesLength != 0 ){
                    DesBuf = (VS_INT8 *)malloc(DesLength);
                    fread(DesBuf,1,DesLength,hFile);
                    fclose(hFile);

                    VS_CHAR SrcMD5Buf[64],DesMD5Buf[64];

                    strcpy( SrcMD5Buf, C_MDDataBuf(SrcBuf,SrcLength));
                    strcpy( DesMD5Buf, C_MDDataBuf(DesBuf,DesLength));
                    if( strcmp( SrcMD5Buf, DesMD5Buf ) == 0){
                        if( SrcBuf != NULL )
                            free(SrcBuf);
                        if( DesBuf != NULL )
                            free(DesBuf);
                        Handle = dlopen(acLocalModuleName,Flag);
                        return Handle;
                    }
                }else
                    fclose(hFile);
            }
            if( SrcBuf != NULL )
                free(SrcBuf);
            if( DesBuf != NULL )
                free(DesBuf);
            /*--copy file--*/
            vs_file_copy(acModuleName,acLocalModuleName);
            chmod(acLocalModuleName, 0777);
        	Handle = dlopen(acLocalModuleName,Flag);
            strcpy(acModuleName,acLocalModuleName);
        }
    }
	if( Handle == NULL ){
		VS_CHAR *LocalErrorInfo = (VS_CHAR *)dlerror();
		if( LocalErrorInfo != NULL ){
/*
            if( vs_file_exist(acModuleName) == VS_TRUE ){
                VS_CHAR ErrBuf[512];

                vs_string_snprintf(ErrBuf,512,"load library[%s] failed [%s]",acModuleName,LocalErrorInfo);
		        __android_log_write(ANDROID_LOG_DEBUG,"starcore",ErrBuf);
            }
*/
#if defined(DEBUG) || defined(_DEBUG)
			printf("%s\n",LocalErrorInfo);
#endif			
            VS_STRNCPY(ErrorBuf,LocalErrorInfo,1024);
        }
	}    
#endif
    return Handle;
}

VS_HANDLE vs_dll_open(const VS_CHAR *ModuleName)
{
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_MACOS )
    return _vs_dll_open(ModuleName,RTLD_NOW | RTLD_GLOBAL,NULL);
#endif
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    return _vs_dll_open(ModuleName,0,NULL);
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
    return _vs_dll_open(ModuleName,0,NULL);
#endif
}

VS_HANDLE vs_dll_openex(const VS_CHAR *ModuleName,VS_BOOL ExportGlobal,VS_CHAR **ErrorInfo)
{
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_MACOS )
	if( ExportGlobal == VS_FALSE )
		return _vs_dll_open(ModuleName,RTLD_NOW,ErrorInfo);
	else
		return _vs_dll_open(ModuleName,RTLD_NOW | RTLD_GLOBAL,ErrorInfo);
#endif
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    return _vs_dll_open(ModuleName,0,ErrorInfo);
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
    if( ExportGlobal == VS_FALSE )
        return _vs_dll_open(ModuleName,RTLD_NOW,ErrorInfo);
    else
        return _vs_dll_open(ModuleName,RTLD_NOW | RTLD_GLOBAL,ErrorInfo);
#endif
}

#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
static VS_BOOL lookup_sharelib(VS_HANDLE ProcessID,VS_CHAR *name,VS_CHAR *fullname,VS_INT32 fullnamelen)
{
#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS|| VS_OS_TYPE == VS_OS_MACOS )
    return VS_FALSE;  /* popen may be no response for android 4.0 */
#endif
#if( VS_OS_TYPE == VS_OS_LINUX )
    VS_CHAR CmdBuf[64],*CharPtr;
    FILE *fp;
    VS_CHAR buf[512], cmd[512], s[4][32],NameBuf[512];
    VS_ULONG Address;
    VS_BOOL Result,AbsoluteFile;
    VS_INT32 namelen;

    CharPtr = vs_file_strrchr(name,'/');
    if( CharPtr != NULL ){  // get absolute file name
        vs_memset(NameBuf,0,512);
	    if( readlink(name,NameBuf,512) == -1 )
		    strcpy(NameBuf,name);
        namelen = strlen(NameBuf);
        AbsoluteFile = VS_TRUE;
    }else{
        namelen = strlen(name);
        AbsoluteFile = VS_FALSE;
    }
	/*printf("Link File:   %s\n",NameBuf);*/
    sprintf(CmdBuf,"cat /proc/%u/maps",ProcessID);
    if ((fp = popen(CmdBuf, "r")) == NULL)
        return VS_FALSE;
    Result = VS_FALSE;
    while(fgets(buf, sizeof(buf), fp)){
        if( Result == VS_FALSE ){
        	if( sscanf(buf,"%s %s %s %s %d %s",s[0],s[1],s[2],s[3],&Address,cmd) == 6 ){
                if( AbsoluteFile == VS_TRUE ){  // absolute file name
            	    if( vs_string_strnicmp(cmd,NameBuf,namelen) == 0 ){
                        Result = VS_TRUE;
                        if( fullname != NULL ){
                            VS_STRNCPY( fullname, cmd, fullnamelen );
                        }
                    }
                }else{
                    CharPtr = vs_file_strrchr(cmd,'/');
            	    if( CharPtr != NULL && vs_string_strnicmp(CharPtr+1,name,namelen) == 0 ){
                        Result = VS_TRUE;
                        if( fullname != NULL ){
                            VS_STRNCPY( fullname, cmd, fullnamelen );
                        }
                    }
                }
        	}
        }
    }
    pclose(fp);
    return Result;
#endif    
}
#endif

VS_HANDLE vs_dll_get(const VS_CHAR *ModuleName)
{
    VS_CHAR acModuleName[VS_FILE_NAMELENGTH];
    VS_HANDLE Handle;

    if(ModuleName==NULL )
		return NULL;
    VS_STRNCPY(acModuleName,ModuleName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	vs_file_namechange( acModuleName, '/', '\\' );
    Handle = GetModuleHandle( acModuleName );
	return Handle;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acModuleName, '\\', '/' );
    if( lookup_sharelib( vs_process_current(), acModuleName, NULL, 0 ) == VS_FALSE )
        return NULL;
    Handle = vs_dll_open(acModuleName);
    vs_dll_close(Handle);
	return Handle;
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
	vs_file_namechange( acModuleName, '\\', '/' );
    if( lookup_sharelib( vs_process_current(), acModuleName, NULL, 0 ) == VS_FALSE )
        return NULL;
    Handle = vs_dll_open(acModuleName);
    vs_dll_close(Handle);
	return Handle;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	return NULL;
#endif
}

VS_HANDLE vs_dll_getex(const VS_CHAR *ModuleName,VS_CHAR *FullModuleName,VS_INT32 FullModuleSize)
{
    VS_CHAR acModuleName[VS_FILE_NAMELENGTH];
    VS_HANDLE Handle;
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	VS_CHAR LocalBuf[VS_FILE_NAMELENGTH];
#endif

    if(ModuleName==NULL )
		return NULL;
    VS_STRNCPY(acModuleName,ModuleName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	vs_file_namechange( acModuleName, '/', '\\' );
    Handle = GetModuleHandle( acModuleName );
    if( Handle != NULL && FullModuleName != NULL && FullModuleSize != 0 )
        GetModuleFileName(Handle,FullModuleName,FullModuleSize);
	return Handle;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acModuleName, '\\', '/' );
    if( lookup_sharelib( vs_process_current(), acModuleName, LocalBuf, VS_FILE_NAMELENGTH ) == VS_FALSE )
        return NULL;
    Handle = vs_dll_open(LocalBuf);
    vs_dll_close(Handle);
    if( Handle != NULL && FullModuleName != NULL && FullModuleSize != 0 ){
        VS_STRNCPY( FullModuleName, LocalBuf, FullModuleSize );
    }
	return Handle;
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
	vs_file_namechange( acModuleName, '\\', '/' );
    if( lookup_sharelib( vs_process_current(), acModuleName, LocalBuf, VS_FILE_NAMELENGTH ) == VS_FALSE )
        return NULL;
    Handle = vs_dll_open(LocalBuf);
    vs_dll_close(Handle);
    if( Handle != NULL && FullModuleName != NULL && FullModuleSize != 0 ){
        VS_STRNCPY( FullModuleName, LocalBuf, FullModuleSize );
    }
	return Handle;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    return NULL;
#endif
}

#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
static VS_BOOL vs_dll_close_check(VS_CHAR *Path)
{
    VS_CHAR acModuleName[VS_FILE_NAMELENGTH],FileName[VS_FILE_NAMELENGTH];
    VS_FILE_FIND_T FindData;
    VS_HANDLE Handle;

    sprintf(acModuleName,"%s/*.*",Path);
    Handle = vs_file_findfirst( acModuleName, &FindData );
    if( Handle != VS_INVALID_HANDLE ){
        if( vs_file_isfinddir( &FindData ) == VS_TRUE ){
            if( strcmp(FindData.cFileName,".") != 0 && strcmp(FindData.cFileName,"..") != 0 ){
                sprintf(FileName,"%s/%s",Path,FindData.cFileName);
                if( vs_dll_close_check(FileName) == VS_TRUE ){
                    vs_file_findclose(Handle);
                    return VS_TRUE;
                }
            }
        }else{
            if( ShareLibraryTempPathFlag == VS_FALSE || strlen(ShareLibraryTempPath) == 0 )
                sprintf(FileName,"%s/%s",&Path[strlen("/data/data/com.srplab.starcore/files")],FindData.cFileName);
            else
                sprintf(FileName,"%s/%s",&Path[strlen(ShareLibraryTempPath)],FindData.cFileName);
            if( vs_file_exist( FileName ) == VS_FALSE ){
                sprintf(FileName,"%s/%s",Path,FindData.cFileName);
                vs_file_delete(FileName);
                vs_file_findclose(Handle);
                return VS_TRUE;
            }
        }
        while( vs_file_findnext( Handle, &FindData ) == VS_TRUE ){
            if( vs_file_isfinddir( &FindData ) == VS_TRUE ){
                if( strcmp(FindData.cFileName,".") != 0 && strcmp(FindData.cFileName,"..") != 0 ){
                    sprintf(FileName,"%s/%s",Path,FindData.cFileName);
                    if( vs_dll_close_check(FileName) == VS_TRUE ){
                        vs_file_findclose(Handle);
                        return VS_TRUE;
                    }
                }
            }else{
                if( ShareLibraryTempPathFlag == VS_FALSE || strlen(ShareLibraryTempPath) == 0 )
                    sprintf(FileName,"%s/%s",&Path[strlen("/data/data/com.srplab.starcore/files")],FindData.cFileName);
                else
                    sprintf(FileName,"%s/%s",&Path[strlen(ShareLibraryTempPath)],FindData.cFileName);
                if( vs_file_exist( FileName ) == VS_FALSE ){
                    sprintf(FileName,"%s/%s",Path,FindData.cFileName);
                    vs_file_delete(FileName);
                    vs_file_findclose(Handle);
                    return VS_TRUE;
                }
            }
        }
        vs_file_findclose(Handle);
    }
    return VS_FALSE;
}
#endif

void vs_dll_close(const VS_HANDLE Handle)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	FreeLibrary(Handle);
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_MACOS )
    if( Handle == VS_DLL_PSEUDOHANDLE )
        return;
	dlclose(Handle);
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
    if( Handle == VS_DLL_PSEUDOHANDLE )
        return;
	dlclose(Handle);
#endif

#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
    if( Handle == VS_DLL_PSEUDOHANDLE )
        return;
	dlclose(Handle);
    /*--check temp files on data/data/com.srplab.starcore/file/sdcard--*/
    if( ShareLibraryTempPathFlag == VS_FALSE || strlen(ShareLibraryTempPath) == 0 ){
        /*---vs_dll_close_check("/data/data/com.srplab.starcore/files/sdcard");  */
        /*--- removed at 2013/02/17, for not create file by default */
    }else{
        VS_CHAR TempPath[512];
        sprintf(TempPath,"%s/sdcard",ShareLibraryTempPath);
        vs_dll_close_check(TempPath);
    }
#endif
}

struct StructOfVSDllRegisterProcItem
{
	void *ProcAddress;
	VS_CHAR Name[128];
};

static VS_ULONG VSDllRegisterProcItemBufCount = 0;
static struct StructOfVSDllRegisterProcItem VSDllRegisterProcItemBuf[4096];  /*--max value is 1024 entry */

void *vs_dll_registerproc(const VS_CHAR *ProcName,void *ProcAddr)
{
	VS_ULONG i,j;
	void *OldProcAddr;

	if( VSDllRegisterProcItemBufCount >= 4096 || ProcName == NULL || vs_string_strlen(ProcName) >= 128 || vs_string_strlen(ProcName) == 0 )
		return NULL;
	if( ProcAddr == NULL ){
		//--cancel the register
		for( i=0; i < VSDllRegisterProcItemBufCount; i++ ){
			if( VSDllRegisterProcItemBuf[i].Name[0] == ProcName[0] && vs_string_strcmp(VSDllRegisterProcItemBuf[i].Name,ProcName) == 0 ){
				OldProcAddr = VSDllRegisterProcItemBuf[i].ProcAddress;
				for( j=i; j < VSDllRegisterProcItemBufCount - 1; j ++ )
					VSDllRegisterProcItemBuf[j] = VSDllRegisterProcItemBuf[j+1];
				VSDllRegisterProcItemBufCount --;
				return OldProcAddr;
			}
		}
		return NULL;
	}else{
		for( i=0; i < VSDllRegisterProcItemBufCount; i++ ){
			if( VSDllRegisterProcItemBuf[i].Name[0] == ProcName[0] && vs_string_strcmp(VSDllRegisterProcItemBuf[i].Name,ProcName) == 0 ){
				OldProcAddr = VSDllRegisterProcItemBuf[i].ProcAddress;
				VSDllRegisterProcItemBuf[i].ProcAddress = ProcAddr;
				return OldProcAddr;
			}
		}
		strcpy(VSDllRegisterProcItemBuf[i].Name,ProcName);
		VSDllRegisterProcItemBuf[i].ProcAddress = ProcAddr;
		VSDllRegisterProcItemBufCount ++;
		return NULL;
	}
}

void *vs_dll_sym(const VS_HANDLE Handle,const VS_CHAR *ProcName)
{
    VS_CHAR acProcName[ VS_FILE_NAMELENGTH] ;
    void *RetHandle;
	VS_ULONG i;

    if(ProcName==NULL)
		return NULL;
	/* the register function is high priority */
	for( i=0; i < VSDllRegisterProcItemBufCount; i++){
		if( VSDllRegisterProcItemBuf[i].Name[0] == ProcName[0] && vs_string_strcmp(VSDllRegisterProcItemBuf[i].Name,ProcName) == 0 )
			return VSDllRegisterProcItemBuf[i].ProcAddress;
	}

    VS_STRNCPY(acProcName,ProcName, VS_FILE_NAMELENGTH) ;
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	vs_file_namechange( acProcName, '/', '\\' );
    if( Handle == VS_DLL_PSEUDOHANDLE ){
		HMODULE hmodule = GetModuleHandle(NULL);
        RetHandle = GetProcAddress(hmodule,acProcName);
    }else
		RetHandle = GetProcAddress(Handle,acProcName);
    return RetHandle;
#endif	
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	vs_file_namechange( acProcName, '/', '\\' );
    if( Handle == VS_DLL_PSEUDOHANDLE )
        RetHandle = NULL;
    else
		RetHandle = GetProcAddress(Handle,acProcName);
    return RetHandle;
#endif	
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
	vs_file_namechange( acProcName, '\\', '/' );
    if( Handle == VS_DLL_PSEUDOHANDLE ){
        RetHandle = NULL; //dlsym(RTLD_DEFAULT,acProcName);
#if defined(DEBUG) || defined(_DEBUG)			        
		printf("VS_DLL_PSEUDOHANDLE not supported for linux & android\n");
#endif		
    }else
		RetHandle = dlsym(Handle,acProcName);
    if( RetHandle == NULL ){
#if defined(DEBUG) || defined(_DEBUG)			    	
        VS_CHAR *ErrorInfo = (VS_CHAR *)dlerror();
        if(ErrorInfo!=NULL)
            printf("%s\n",ErrorInfo);
#endif            
    }
    return RetHandle;
#endif
#if( VS_OS_TYPE == VS_OS_MACOS )
	vs_file_namechange( acProcName, '\\', '/' );
    if( Handle == VS_DLL_PSEUDOHANDLE ){
        RetHandle = dlsym(RTLD_DEFAULT,acProcName);	
    }else
		RetHandle = dlsym(Handle,acProcName);
    if( RetHandle == NULL ){
#if defined(DEBUG) || defined(_DEBUG)			    	
        VS_CHAR *ErrorInfo = (VS_CHAR *)dlerror();
        if(ErrorInfo!=NULL)
            printf("%s\n",ErrorInfo);
#endif            
    }
    return RetHandle;
#endif
#if( VS_OS_TYPE == VS_OS_IOS )    
	vs_file_namechange( acProcName, '\\', '/' );
    if( Handle == VS_DLL_PSEUDOHANDLE )
        //RetHandle = dlsym(RTLD_MAIN_ONLY,acProcName);
		RetHandle = dlsym(RTLD_DEFAULT,acProcName);
    else
	    RetHandle = dlsym(Handle,acProcName);
    if( RetHandle == NULL ){
#if defined(DEBUG) || defined(_DEBUG)			    	
        VS_CHAR *ErrorInfo = (VS_CHAR *)dlerror();
        if(ErrorInfo!=NULL)
            printf("%s\n",ErrorInfo);
#endif            
    }
    return RetHandle;
#endif
}

/*--------------------------------------------------------------------
part 4 : string functions
----------------------------------------------------------------------*/

VS_CHAR *vs_string_stristr( const VS_CHAR *string, const VS_CHAR *strCharSet )
{
    VS_CHAR *pCompareStart = (VS_CHAR *)string;
    VS_CHAR *pCursor_S1, *pCursor_S2;
    VS_CHAR cSrc,cDst;

    if(string == NULL || strCharSet == NULL)
        return NULL;
    while (*pCompareStart){
        pCursor_S1 = pCompareStart;
        pCursor_S2 = (VS_CHAR *)strCharSet;
        while( (*pCursor_S1) != 0 && (*pCursor_S2) != 0 ){
            cSrc = *pCursor_S1;
            cDst = *pCursor_S2;
            if ((cSrc >= 'A') && (cSrc <= 'Z'))
                cSrc -= ('A' - 'a');
            if ((cDst >= 'A') && (cDst <= 'Z'))
                cDst -= ('A' - 'a');
            if (cSrc != cDst)
                break;
            pCursor_S1++;
            pCursor_S2++;
        }
        if((*pCursor_S2) == 0)
            return(pCompareStart);
        pCompareStart++;
    }
     return NULL;
}

void vs_memcpy(void *DesBuf,const void *SrcBuf,VS_WORD Size)
{
    VS_WORD i;

    if( (((VS_UWORD)DesBuf) % sizeof(VS_UWORD) ) == 0 && (((VS_UWORD)SrcBuf) % sizeof(VS_UWORD) ) == 0 ){
        memcpy(DesBuf,SrcBuf,Size);
        return;
    }
    for( i=0; i< Size; i++ )
        ((VS_INT8 *)DesBuf)[i] = ((VS_INT8 *)SrcBuf)[i];
    return;
}

void vs_memset(void *DesBuf,VS_INT8 c,VS_WORD Size)
{
    VS_WORD i;

    if( (((VS_UWORD)DesBuf) % sizeof(VS_UWORD) ) == 0 ){
        memset(DesBuf,c,Size);
        return;
    }
    for( i=0; i< Size; i++ )
        ((VS_INT8 *)DesBuf)[i] = c;
    return;
}

/*--------------------------------------------------------------------
part 5 : thread functions
----------------------------------------------------------------------*/
VS_HANDLE vs_thread_create(vs_thread_routineproc proc,void *arg,VS_THREADID *ThreadID)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	return CreateThread(NULL,0,proc,arg,0,ThreadID);
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#if defined(USR_SRPTHREAD)
	VS_HANDLE thread;
	thread = CreateThread_Win8(NULL,0,proc,arg,0,NULL);
	if( thread == NULL )
		return NULL;
    if( ThreadID != NULL )
		(*ThreadID) = (VS_THREADID)thread;
	return thread;
#else
	return CreateThread(NULL,0,proc,arg,0,ThreadID);
#endif	
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    pthread_t  thread;

    if( pthread_create(&thread, NULL, proc, arg) != 0 )
		return NULL;
    if( ThreadID != NULL )
		(*ThreadID) = (VS_THREADID)thread;
	return (VS_HANDLE)thread;
#endif
}

void vs_thread_exit(void *status)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	ExitThread((VS_ULONG)status);  /* ok for 64 bits */
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#if defined(USR_SRPTHREAD)
	ExitThread_Win8((VS_ULONG)status);
#else	
	ExitThread((VS_ULONG)status);  /* ok for 64 bits */
#endif	
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	pthread_exit(status);
#endif
}

VS_HANDLE vs_thread_current()
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	return GetCurrentThread();
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	return (VS_HANDLE)pthread_self();
#endif
}

VS_THREADID vs_thread_currentid()
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	return GetCurrentThreadId();
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	return (VS_THREADID)pthread_self();
#endif
}

void vs_thread_yield()
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	Sleep(0);
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#if defined(USR_SRPTHREAD)
	Sleep_Win8(0);
#else
	Sleep(0);
#endif	
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	sched_yield();
#endif
}

void vs_thread_detach(VS_HANDLE hThread)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	pthread_detach( (pthread_t)hThread );
#endif
}

void vs_thread_join(VS_HANDLE hThread)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	VS_ULONG ExitCode;

	while( GetExitCodeThread( hThread, &ExitCode ) != 0 ){
		if( ExitCode != STILL_ACTIVE )  //one second
			break;
		Sleep(10);
	}
/*
	VS_ULONG ExitCode,RepeatTimes;

	RepeatTimes = 0;
	while( GetExitCodeThread( hThread, &ExitCode ) != 0 ){
        if( ExitCode != STILL_ACTIVE || RepeatTimes > 100 )  //one second
            break;
		RepeatTimes ++;
        Sleep(10);
    }
*/
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#if defined(USR_SRPTHREAD)
	VS_ULONG ExitCode;
	GetExitCodeThread_Win8(hThread, &ExitCode);
/*
	VS_ULONG ExitCode,RepeatTimes;

	RepeatTimes = 0;
	while( GetExitCodeThread_Win8( hThread, &ExitCode ) != 0 ){
        if( RepeatTimes > 100 )  //one second
            break;
		RepeatTimes ++;
        Sleep_Win8(10);
    }
*/
#else
	VS_ULONG ExitCode;

	while( GetExitCodeThread( hThread, &ExitCode ) != 0 ){
		if( ExitCode != STILL_ACTIVE )  //one second
			break;
		Sleep(10);
	}
#endif	
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	pthread_join( (pthread_t)hThread, NULL );
#endif
}

void vs_thread_priority(VS_HANDLE hThread,VS_INT32 Policy,VS_INT32 RelativePriority)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	if( RelativePriority < 0 )
		SetThreadPriority( hThread, THREAD_PRIORITY_ABOVE_NORMAL );
	if( RelativePriority == 0 )
		SetThreadPriority( hThread, THREAD_PRIORITY_NORMAL );
	if( RelativePriority > 0 )
		SetThreadPriority( hThread, THREAD_PRIORITY_BELOW_NORMAL );
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#if defined(USR_SRPTHREAD)
	if( RelativePriority < 0 )
		SetThreadPriority_Win8( hThread, THREAD_PRIORITY_ABOVE_NORMAL );
	if( RelativePriority == 0 )
		SetThreadPriority_Win8( hThread, THREAD_PRIORITY_NORMAL );
	if( RelativePriority > 0 )
		SetThreadPriority_Win8( hThread, THREAD_PRIORITY_BELOW_NORMAL );
#else
	if( RelativePriority < 0 )
		SetThreadPriority( hThread, THREAD_PRIORITY_ABOVE_NORMAL );
	if( RelativePriority == 0 )
		SetThreadPriority( hThread, THREAD_PRIORITY_NORMAL );
	if( RelativePriority > 0 )
		SetThreadPriority( hThread, THREAD_PRIORITY_BELOW_NORMAL );
#endif		
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	VS_INT32 LocalPolicy;
	struct sched_param   param;

	pthread_getschedparam( (pthread_t)hThread, &LocalPolicy, &param);
	param.sched_priority = param.sched_priority + RelativePriority;
	pthread_setschedparam((pthread_t)hThread, Policy, &param);
#endif
}

void vs_thread_sleep( VS_ULONG wMilliseconds)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	Sleep(wMilliseconds);
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#if defined(USR_SRPTHREAD)
	Sleep_Win8(wMilliseconds);
#else
	Sleep(wMilliseconds);
#endif	
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
/*	usleep(wMilliseconds * 1000); */
	struct timeval t_timeval;

	t_timeval.tv_sec = VS_DIV(wMilliseconds,1000);
	t_timeval.tv_usec = VS_MOD(wMilliseconds,1000) * 1000;
	select(0,NULL,NULL,NULL,&t_timeval);
#endif
}

/*---mutex---*/
VS_BOOL vs_mutex_init( VS_MUTEX *mutex)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
#ifdef CBUILDER_FLAG
    InitializeCriticalSectionAndSpinCount( mutex,(VS_ULONG)4000 );
#else
	InitializeCriticalSection( mutex );
#endif
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	InitializeCriticalSectionEx( mutex,1000,0 );
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	pthread_mutexattr_t   mta;

	if( pthread_mutexattr_init(&mta) != 0 )
		return VS_FALSE;
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
#if defined(_CYGWIN)
    pthread_mutexattr_settype(&mta,PTHREAD_MUTEX_RECURSIVE);
#else
    pthread_mutexattr_settype(&mta,PTHREAD_MUTEX_RECURSIVE_NP);
#endif
#endif    
#if( VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )    
    pthread_mutexattr_settype(&mta,PTHREAD_MUTEX_RECURSIVE);
#endif
/*	mta.__mutexkind = PTHREAD_MUTEX_RECURSIVE_NP; 2011-01-22 may compile error on redhat as5*/
	if( pthread_mutex_init(mutex,&mta) != 0 )
		return VS_FALSE;
	return VS_TRUE;
#endif
}

VS_BOOL vs_mutex_lock( VS_MUTEX *mutex)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	EnterCriticalSection(mutex);
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_MACOS )
	if( pthread_mutex_lock(mutex) != 0 )
		return VS_FALSE;
	return VS_TRUE;
#endif
}

void vs_mutex_unlock( VS_MUTEX *mutex)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	LeaveCriticalSection(mutex);
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	pthread_mutex_unlock(mutex);
#endif
}


void vs_mutex_destory( VS_MUTEX *mutex)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	DeleteCriticalSection( mutex );
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	pthread_mutex_destroy( mutex );
#endif
}

#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
union senum{
    int val;
	struct semid_ds *buf;
	VS_UINT16 *array;
};
#endif

VS_PMUTEX vs_pmutex_init( const VS_CHAR *Path,const VS_CHAR *name )
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	return CreateMutex(NULL,VS_FALSE,name);
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	VS_WCHAR LocalNameBuf[512];

	if( name != NULL && name[0] != 0 )
		MultiByteToWideChar( CP_ACP, MB_ERR_INVALID_CHARS, name, -1,LocalNameBuf, 512 );
	else
		LocalNameBuf[0] = 0;
	return CreateMutexExW(NULL,LocalNameBuf,0,SYNCHRONIZE | MUTEX_ALL_ACCESS );
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_MACOS )
#if defined(_CYGWIN)
	return (VS_PMUTEX)-1;
#else
	key_t key;
	VS_INT32 semid;
	union senum sem;

    key = ftok(Path,((VS_INT32 *)name)[0]);
	if( key == -1 ){
		if( vs_string_stricmp("/usr/local/srplab",Path) == 0 ){
			VS_CHAR LocalPath[512];
			vs_dir_getcwd(LocalPath,512);
			return vs_pmutex_init(LocalPath,name);
		}else
			printf("important : vs_pmutex_init failed.....[%s][%s]\n",Path,name);
	}
	semid = semget(key,1,IPC_CREAT|IPC_EXCL|0666);
	if( semid != -1 ){
    	sem.val = 1;
	    semctl(semid,0,SETVAL,sem);
    }else{
        if( errno != EEXIST )
            return VS_FALSE;
        else{
            semid = semget(key,1,IPC_CREAT|0666);
            if( semid == -1 )
                return VS_FALSE;
        }
    }
	return semid;
#endif
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
    return (VS_PMUTEX)-1;
#endif    
#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
    return (VS_PMUTEX)-1;
/*
    VS_PMUTEX sem;

    sem = (VS_PMUTEX)malloc(sizeof(VS_PMUTEX_T));
    if( strlen( name ) > 128 )
    	return NULL;
    strcpy(sem ->Name,name);
    sem -> LockCount = 0;

    if( ShareLibraryTempPathFlag == VS_FALSE || strlen(ShareLibraryTempPath) == 0 )
        vs_dir_create("/data/data/com.srplab.starcore/files/srplab/sem");
    else{
        VS_CHAR TempPath[512];
        sprintf(TempPath,"%s/srplab/sem",ShareLibraryTempPath);
        vs_dir_create(TempPath);
    }
	return sem;
*/
    /*--- removed at 2013/02/17, for not create file by default */
#endif
}

VS_BOOL vs_pmutex_lock( VS_PMUTEX mutex)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	WaitForSingleObject(mutex,INFINITE);
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	WaitForSingleObjectEx(mutex,INFINITE,FALSE);
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_MACOS )
#if defined(_CYGWIN)
	return VS_TRUE;
#else
	struct sembuf sops={0,-1,SEM_UNDO}; /*IPC_NOWAIT};*/

	semop(mutex,&sops,1);
	return VS_TRUE;
#endif
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
    return VS_TRUE;
#endif    
#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
/*
    VS_CHAR Buf[512];
    VS_INT32 PrivateHandle;
	VS_CHAR PrivateBuf[32];
	VS_INT32 MaxWaitTime;
    
    if( mutex -> LockCount > 0 ){
    	mutex -> LockCount ++;
    	return VS_TRUE;
    }
    if( ShareLibraryTempPathFlag == VS_FALSE || strlen(ShareLibraryTempPath) == 0 )
        sprintf(Buf,"/data/data/com.srplab.starcore/files/srplab/sem/%s",mutex -> Name);
    else
        sprintf(Buf,"%s/srplab/sem/%s",ShareLibraryTempPath,mutex -> Name);
    PrivateHandle = open(Buf,O_RDWR | O_CREAT | O_EXCL );
	MaxWaitTime = 0;
	while( PrivateHandle == -1 ){
		MaxWaitTime = MaxWaitTime + 100;		
		//--has exist?
		PrivateHandle = open(Buf, O_RDONLY );
		if( PrivateHandle != -1 ){
			if( read( PrivateHandle, PrivateBuf, 16 ) != 16 ){
				if( MaxWaitTime > 1000 ){
					PrivateHandle = open(Buf,O_CREAT | O_RDWR);
					break;
				}
				close( PrivateHandle );
			}else{
				VS_ULONG hProcessID;

				close( PrivateHandle );
				hProcessID = atoi(PrivateBuf);
				if( vs_process_exist(hProcessID) == VS_FALSE ){
					//---process has been killed.
					PrivateHandle = open(Buf,O_CREAT | O_RDWR);
					break;
				}else{
					//---wait
					MaxWaitTime = 0;
				}
			}
		}
		vs_thread_sleep(100);
		if( MaxWaitTime > 1000 ){
			mutex -> LockCount = 0;
			return VS_FALSE;
		}			
		PrivateHandle = open(Buf,O_RDWR | O_CREAT | O_EXCL );
	}
    sprintf(Buf,"%16d",vs_process_currentid());
	write(PrivateHandle,Buf,16);
	close( PrivateHandle );
	mutex -> LockCount = 1;
*/
    /*--- removed at 2013/02/17, for not create file by default */
	return VS_TRUE;
#endif
}

void vs_pmutex_unlock( VS_PMUTEX mutex)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	ReleaseMutex(mutex);
	return;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_MACOS )
#if defined(_CYGWIN)
	return;
#else
	struct sembuf sops={0,+1,SEM_UNDO | IPC_NOWAIT }; /*IPC_NOWAIT};*/

	semop(mutex,&sops,1);
#endif
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
    return;
#endif    
#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
/*
    VS_CHAR Buf[512];
    
    if( mutex -> LockCount <= 0 )
    	return;
    mutex -> LockCount = mutex -> LockCount - 1;
    if( mutex -> LockCount == 0 ){
        if( ShareLibraryTempPathFlag == VS_FALSE || strlen(ShareLibraryTempPath) == 0 )
            sprintf(Buf,"/data/data/com.srplab.starcore/files/srplab/sem/%s",mutex -> Name);
        else
            sprintf(Buf,"%s/srplab/sem/%s",ShareLibraryTempPath,mutex -> Name);
    	vs_file_delete(Buf);
    }
*/
    /*--- removed at 2013/02/17, for not create file by default */
#endif
}

void vs_pmutex_destory( VS_PMUTEX mutex)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	CloseHandle(mutex);
	return;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_MACOS )
#if defined(_CYGWIN)
	return;
#else
	union senum sem;

	sem.val = 0;
	semctl(mutex,0,IPC_RMID,sem);
#endif
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
    return;
#endif    
#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
/*    free( mutex ); */
    /*--- removed at 2013/02/17, for not create file by default */
#endif
}

VS_BOOL vs_cond_init( VS_COND *cond)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	(*cond) = CreateEvent(NULL,VS_TRUE,VS_FALSE,NULL);
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	(*cond) = CreateEventExW(NULL,NULL,CREATE_EVENT_MANUAL_RESET,SYNCHRONIZE | EVENT_ALL_ACCESS);
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	if( pthread_cond_init(&cond ->cond, NULL) != 0 )
		return VS_FALSE;
	if( vs_mutex_init(&cond->mutex) == VS_FALSE ){
		pthread_cond_destroy(&cond ->cond);
		return VS_FALSE;
	}
	return VS_TRUE;
#endif
}
void vs_cond_wait( VS_COND *cond)
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
	vs_mutex_lock(&cond ->mutex);
	pthread_cond_wait(&cond ->cond,&cond ->mutex);
	vs_mutex_unlock(&cond ->mutex);
#endif
}

VS_BOOL vs_cond_timewait( VS_COND *cond, VS_ULONG TimeMs)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	if( TimeMs == 0 ){
		vs_cond_wait(cond);
		return VS_TRUE;
	}
	if( WaitForSingleObject((*cond),TimeMs) == WAIT_OBJECT_0 ){
		ResetEvent((*cond));
		return VS_TRUE;
	}else{
#if defined(DEBUG) || defined(_DEBUG)
		printf("call vs_cond_timewait overtime...\n");
#endif
		return VS_FALSE;
	}
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	if( TimeMs == 0 ){
		vs_cond_wait(cond);
		return VS_TRUE;
	}
	if( WaitForSingleObjectEx((*cond),TimeMs,VS_FALSE) == WAIT_OBJECT_0 ){
		ResetEvent((*cond));
		return VS_TRUE;
	}else{
#if defined(DEBUG) || defined(_DEBUG)		
		vs_outputdebugstring("call vs_cond_timewait overtime...",CP_ACP);
#endif
		return VS_FALSE;
	}
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	VS_BOOL Result;
	struct timespec to;

	if( TimeMs == 0 ){
		vs_cond_wait(cond);
		return VS_TRUE;
	}
	to.tv_sec = time(NULL) + TimeMs / 1000;
	to.tv_nsec = TimeMs % 1000;
	vs_mutex_lock(&cond ->mutex);
	if( pthread_cond_timedwait(&cond ->cond,&cond ->mutex,&to) == 0 )
		Result = VS_TRUE;
	else{
#if defined(DEBUG) || defined(_DEBUG)
		printf("call vs_cond_timewait overtime...\n");
#endif
		Result = VS_FALSE;
	}
	vs_mutex_unlock(&cond ->mutex);
	return Result;
#endif
}

void vs_cond_signal( VS_COND *cond)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	SetEvent((*cond));
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	pthread_cond_signal(&cond ->cond);
#endif
}

void vs_cond_destroy( VS_COND *cond)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	CloseHandle((*cond));
	return;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	pthread_cond_destroy(&cond ->cond);
	vs_mutex_destory(&cond->mutex);
#endif
}

void vs_atomic_init(vs_atomic *atomic)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	atomic->val = 0;
	vs_mutex_init(&atomic->mutex);
#endif
}

void vs_atomic_set(vs_atomic *atomic,VS_LONG i)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	VS_LONG l;

	l = i;
	InterlockedExchange(atomic,l);
	return;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_mutex_lock(&atomic->mutex);
	atomic->val = i;
	vs_mutex_unlock(&atomic->mutex);
#endif
}

VS_LONG vs_atomic_compare_set(vs_atomic *atomic,VS_LONG i,VS_LONG comparevalue)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    VS_LONG DwordTemp;
#ifdef CBUILDER_FLAG
    DwordTemp = (VS_LONG)InterlockedCompareExchange((VS_LONG *)atomic,i,comparevalue);
#else
    DwordTemp = (VS_LONG)InterlockedCompareExchange((void *)atomic,(void *)i,(void *)comparevalue);
#endif
	return DwordTemp;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    VS_LONG l;
	vs_mutex_lock(&atomic->mutex);
    l = atomic->val;
    if( atomic->val == comparevalue )
    	atomic->val = i;
	vs_mutex_unlock(&atomic->mutex);
    return l;
#endif
}

VS_LONG vs_atomic_get(vs_atomic *atomic)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	VS_LONG l;

	l = InterlockedExchangeAdd (atomic,0);
	return (VS_INT32)l;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	VS_LONG l;
	vs_mutex_lock(&atomic->mutex);
	l = atomic->val;
	vs_mutex_unlock(&atomic->mutex);
	return l;
#endif
}

void vs_atomic_destory(vs_atomic *atomic)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	vs_mutex_destory(&atomic->mutex);
#endif
}

/*--------------------------------------------------------------------
part 7 : system app functions
----------------------------------------------------------------------*/
VS_BOOL vs_get_module(VS_CHAR *Buf,VS_INT32 BufSize)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	if( GetModuleFileName(NULL,Buf,BufSize) == 0 ){
		Buf[0] = 0;
		return VS_FALSE;
	}
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	if( GetModuleFileName_win8(Buf,BufSize) == 0 ){
		Buf[0] = 0;
		return VS_FALSE;
	}
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_MACOS )
	VS_CHAR proc[256],*p;

	sprintf(proc,"/proc/%d/exe",getpid());
	vs_memset(Buf,0,BufSize);
	if( readlink(proc,Buf,BufSize) == -1 ){
		Buf[0] = 0;
		return VS_FALSE;
	}
	Buf[BufSize-1] = 0;
	p = strchr(Buf,'(');
	if( p != NULL ){
		p --;
		*p = 0;
	}
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS )
    Buf[0] = 0;
	return VS_FALSE;
#endif
} 

VS_BOOL vs_get_env(const VS_CHAR *Name,VS_CHAR *Buf,VS_INT32 BufSize)
{
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	VS_CHAR *CharPtr;
#endif

	if( Buf != NULL )
		Buf[0] = 0;
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	if( GetEnvironmentVariable(Name,Buf,BufSize) == 0 )
		return VS_FALSE;
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	if( GetEnvironmentVariable_win8((char *)Name,(char *)Buf,(int)BufSize) == 0 )
		return VS_FALSE;
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	VS_CHAR *CharPtr;

	CharPtr = getenv(Name);
	if( CharPtr == NULL )
		return VS_FALSE;
	VS_STRNCPY(Buf,CharPtr,BufSize);
	return VS_TRUE;
#endif
}

VS_BOOL vs_set_env(const VS_CHAR *Name,const VS_CHAR *Value)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
	if( SetEnvironmentVariable(Name,Value) == VS_FALSE )
		return VS_FALSE;
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	if( SetEnvironmentVariable_win8((char *)Name,(char *)Value) == 0 )
		return VS_FALSE;
	return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	if( setenv(Name,Value,1) == 0 )
		return VS_TRUE;
	return VS_FALSE;
#endif
}

#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_MACOS || VS_OS_TYPE == VS_OS_WIN10 )
extern void uuid_create_internal(VS_UUID *in_uuid);
#endif

void vs_uuid_create(VS_UUID *Uuid)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
    UuidCreate(Uuid);
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	uuid_create_internal( Uuid );
#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if( VS_OS_TYPE == VS_OS_LINUX )
    uuid_create_internal( Uuid );
#endif    
#if( VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    uuid_t uu;

    uuid_generate( uu );
    vs_memcpy(Uuid,uu,sizeof(VS_UUID));
#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if( VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
	uuid_create_internal( Uuid );
#endif  
}

VS_INT32 vs_kbhit(void)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
#ifdef VC_VERSION
#if(VC_VERSION >= 12)
	if( _kbhit() != 0 )
		return _getch();
	else
		return -1;
#else
	if (kbhit() != 0)
		return getch();
	else
		return -1;
#endif
#else
    if( kbhit() != 0 )
        return getch();
    else
        return -1;
#endif
#endif
#if( VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 )
	return -1;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 )
    struct termios oldt, newt;
    int ch;

    if( tcgetattr( STDIN_FILENO, &oldt ) != 0 )
        return -1;
    newt = oldt;

    newt.c_cc[VMIN] = 0;
    newt.c_cc[VTIME] = 1;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );

    ch = getchar();

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );

    return ch;
#endif
#if( VS_OS_TYPE == VS_OS_MACOS || VS_OS_TYPE == VS_OS_IOS )
    struct termios oldt, newt;
	unsigned char ch;
	ssize_t size;

    if( tcgetattr( STDIN_FILENO, &oldt ) != 0 )
        return -1;
    newt = oldt;

    newt.c_cc[VMIN] = 0;
    newt.c_cc[VTIME] = 1;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );

	size = read (STDIN_FILENO, &ch, 1);
	
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	
	if (size == 0)
		return -1;
	else
		return ch;
#endif
}

VS_HANDLE vs_process_current()
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	return GetCurrentProcess();
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    return (VS_HANDLE)(VS_UWORD)getpid();
#endif
}

VS_ULONG vs_process_currentid()
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	return GetCurrentProcessId();
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    return (VS_ULONG)getpid();
#endif
}

VS_BOOL vs_process_exist(VS_ULONG ProcessID)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
    VS_HANDLE Handle;
	VS_ULONG ExitCode;

    Handle = OpenProcess(PROCESS_QUERY_INFORMATION,0,ProcessID);
    if( Handle == NULL )
        return VS_FALSE;
	GetExitCodeProcess(Handle,&ExitCode);
    CloseHandle(Handle);
	if( ExitCode == STILL_ACTIVE )
		return VS_TRUE;
	return VS_FALSE;
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_MACOS )
	VS_CHAR proc[256];

	sprintf(proc,"/proc/%u",(VS_UINT32)ProcessID);
    return vs_file_exist(proc);
#endif
#if( VS_OS_TYPE == VS_OS_IOS )
    return VS_TRUE;
#endif    
}

void vs_process_exit(VS_INT32 Status)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS )
    ExitProcess(Status);
#endif
#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
    return;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    exit(Status);
#endif    
}

/*--------------------------------------------------------------------
part 8 : socket functions
----------------------------------------------------------------------*/
#if( ( VS_OS_TYPE != VS_OS_WINRT ) || ( (VS_OS_TYPE == VS_OS_WINRT) && (!defined(USE_SRPSOCKET)) ) )

VS_BOOL vs_socket_init()
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 || VS_OS_TYPE == VS_OS_WINRT )
    VS_UINT16 wVersionRequested = MAKEWORD( 2, 0 );
    WSADATA wsaData;
	VS_INT32 nRet;

    nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet){
		WSACleanup();
		return VS_FALSE;
	 }
	 if (wsaData.wVersion != wVersionRequested){
		 WSACleanup();
		 return VS_FALSE;
	 }
	 return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	 return VS_TRUE;
#endif
}

void vs_socket_term()
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 || VS_OS_TYPE == VS_OS_WINRT )
    WSACleanup();
#endif
}

VS_INT32 vs_socket_close(SOCKET s)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 || VS_OS_TYPE == VS_OS_WINRT )
    return closesocket(s);
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	 return close(s);
#endif
}

void vs_socket_setnonblock(SOCKET s)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 || VS_OS_TYPE == VS_OS_WINRT )
    VS_ULONG nNoBlock = 1;
    ioctlsocket( s, FIONBIO, &nNoBlock );
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    VS_INT32 flags=fcntl(s,F_GETFL,0);
    fcntl(s,F_SETFL,flags | O_NONBLOCK );
#endif
}

void vs_socket_setreuseaddr(SOCKET s)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 || VS_OS_TYPE == VS_OS_WINRT )
//	BOOL bReuseaddr = TRUE;
//	setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(const VS_CHAR*)&bReuseaddr,sizeof(BOOL)); 
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	VS_INT32 bReuseaddr = 1;
	setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(const VS_CHAR*)&bReuseaddr,sizeof(VS_INT32)); 
#endif
}

VS_INT32 vs_socket_getpeername(SOCKET s,SOCKADDR *name,VS_INT32 *namelen)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 || VS_OS_TYPE == VS_OS_WINRT )
    return getpeername(s,name,namelen);
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    return getpeername(s,name,(socklen_t *)namelen);
#endif
}

VS_INT32 vs_socket_getsockname(SOCKET s,SOCKADDR *name,VS_INT32 *namelen)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 || VS_OS_TYPE == VS_OS_WINRT )
    return getsockname(s,name,namelen);
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    return getsockname(s,name,(socklen_t *)namelen);
#endif
}

SOCKET vs_socket_accept(SOCKET s,SOCKADDR *addr,VS_INT32 *addrlen)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 || VS_OS_TYPE == VS_OS_WINRT )
    return accept(s,addr,addrlen);
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    return accept(s,addr,(socklen_t *)addrlen);
#endif
}

VS_INT32 vs_socket_geterrno()
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 || VS_OS_TYPE == VS_OS_WINRT )
    return WSAGetLastError();
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    return errno;
#endif
}

VS_BOOL vs_socket_isconnect(SOCKET s)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 || VS_OS_TYPE == VS_OS_WINRT )
    return VS_TRUE;
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	VS_INT32 localErrorNo;
    socklen_t Len;

    Len = sizeof(VS_INT32);
	if( getsockopt(s,SOL_SOCKET,SO_ERROR,&localErrorNo,&Len) != 0 || localErrorNo != 0 )
		return VS_FALSE;
	return VS_TRUE;
#endif
}

VS_INT32 vs_socket_recvfrom(SOCKET s,VS_CHAR* buf,VS_INT32 len,VS_INT32 flags,SOCKADDR* from,VS_INT32* fromlen)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WIN10 || VS_OS_TYPE == VS_OS_WINRT )
    return recvfrom(s,buf,len,flags,from,fromlen);
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A  || VS_OS_TYPE == VS_OS_ANDROIDX86|| VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
    return (VS_INT32)recvfrom(s,buf,len,flags,from,(socklen_t *)fromlen);
#endif
}

#endif /*------------------------socket-----------------*/

#if( VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
VS_BOOL vs_getinstall_dir(VS_CHAR *Buf,VS_INT32 BufSize)
{
    return GetInstall_Dir_win8(Buf,BufSize);
}

VS_BOOL vs_getlocal_dir(VS_CHAR *Buf,VS_INT32 BufSize)
{
	return GetLocal_Dir_win8(Buf,BufSize);
}

void vs_outputdebugstring(VS_CHAR *Buf,VS_UINT32 CodePage)
{
	OutputDebugString_win8(Buf,CodePage);
}
#endif

#if( VS_OS_TYPE == VS_OS_WINRT )
#if defined(USE_SRPSOCKET)
static VS_BOOL hton_init_flag = VS_FALSE;
static VS_BOOL m_hton_need_convert = VS_FALSE;

static VS_BOOL  hton_need_convert()
{
	VS_UINT16 x = 0x0102;
	VS_UINT8 *y = (VS_UINT8 *)&x;
	if (hton_init_flag == VS_TRUE)
		return m_hton_need_convert;
	hton_init_flag = VS_TRUE;
	if (y[0] == 0x01)
		m_hton_need_convert = VS_FALSE;
	else
		m_hton_need_convert = VS_TRUE;
	return m_hton_need_convert;
}

#define local_htons(x) ((VS_UINT16)((((VS_UINT16)(x) & 0x00ff)<<8)|(((VS_UINT16)(x)& 0xff00) >> 8)))
#define local_htonl(x) ((VS_ULONG)((((VS_ULONG)(x) & 0x000000ff)<<24)|(((VS_ULONG)(x)& 0x0000ff00) << 8) | (((VS_ULONG)(x)& 0x00ff0000) >> 8) | (((VS_ULONG)(x)& 0xff000000) >> 24)))

VS_INT16 vs_htons(VS_INT16 x)
{
	if (hton_need_convert() == VS_FALSE)
		return x;
	return local_htons(x);
/*	VS_INT16 y;
	VS_UINT8 *s1 = (VS_UINT8 *)&x;
	VS_UINT8 *s2 = (VS_UINT8 *)&y;
	s2[0] = s1[1];
	s2[1] = s1[0];
	return y;
*/
}

VS_LONG vs_htonl(VS_LONG x)
{
	if (hton_need_convert() == VS_FALSE)
		return x;
	return local_htonl(x);
/*	VS_LONG y;
	VS_UINT8 *s1 = (VS_UINT8 *)&x;
	VS_UINT8 *s2 = (VS_UINT8 *)&y;
	s2[0] = s1[3];
	s2[1] = s1[2];
	s2[2] = s1[1];
	s2[3] = s1[0];
	return y;
*/
}
#endif
#endif

VS_INT64 vs_htonl64(VS_INT64 Val)
{
    union{
        VS_INT64 Val;
        VS_INT32 SVal[2];
    }w;
    w.Val = Val;
    w.SVal[0] = vs_htonl(w.SVal[0]);
    w.SVal[1] = vs_htonl(w.SVal[1]);
    return w.Val;
}
VS_INT64 vs_ntohl64(VS_INT64 Val)
{
    union{
        VS_INT64 Val;
        VS_INT32 SVal[2];
    }w;
    w.Val = Val;
    w.SVal[0] = vs_ntohl(w.SVal[0]);
    w.SVal[1] = vs_ntohl(w.SVal[1]);
    return w.Val;
}

VS_UWORD Float2UWord(VS_FLOAT val)
{
	VS_UWORD Temp;
	((VS_FLOAT *)&Temp)[0] = val;
	return Temp;
}

VS_FLOAT UWord2Float(VS_UWORD val)
{
	return ((VS_FLOAT *)&val)[0];
}

VS_UWORD Double2UWord(VS_DOUBLE val)  /* Double will be changed to float to compatiable with 32bit*/
{
	VS_UWORD Temp;
	((VS_FLOAT *)&Temp)[0] = (VS_FLOAT)val;
	return Temp;
}

VS_DOUBLE UWord2Double(VS_UWORD val)
{
	return (VS_DOUBLE)((VS_FLOAT *)&val)[0];
}


VS_INT64 Float2Int64(VS_FLOAT val)
{
	VS_INT64 Temp;
	((VS_FLOAT *)&Temp)[0] = val;
	return Temp;
}

VS_FLOAT Int642Float(VS_INT64 val)
{
	return ((VS_FLOAT *)&val)[0];
}

VS_INT64 Double2Int64(VS_DOUBLE val)
{
	VS_INT64 Temp;
	((VS_DOUBLE *)&Temp)[0] = (VS_DOUBLE)val;
	return Temp;
}

VS_DOUBLE Int642Double(VS_INT64 val)
{
	return ((VS_DOUBLE *)&val)[0];
}

VS_INT32 vs_atoi(const VS_CHAR *str)
{
#if( VS_OS_TYPE == VS_OS_WINDOWS || VS_OS_TYPE == VS_OS_WP || VS_OS_TYPE == VS_OS_WINRT || VS_OS_TYPE == VS_OS_WIN10 )
	return (VS_INT32)(VS_UINT32)(VS_UINT64)_atoi64(str);
#endif
#if( VS_OS_TYPE == VS_OS_LINUX || VS_OS_TYPE == VS_OS_ANDROID || VS_OS_TYPE == VS_OS_ANDROIDV7A || VS_OS_TYPE == VS_OS_ANDROIDX86 || VS_OS_TYPE == VS_OS_IOS || VS_OS_TYPE == VS_OS_MACOS )
	return (VS_INT32)(VS_UINT32)(VS_UINT64)atoll(str);
#endif
}
/*
 * libjingle
 * Copyright 2004--2006, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <errno.h>
#include <cassert>

#include "talk/base/basicdefs.h"
#include "talk/base/convert.h"
#include "talk/base/pathutils.h"
#include "talk/base/fileutils.h"
#include "talk/base/stringutils.h"
#include "talk/base/stream.h"

#include "talk/base/win32filesystem.h"

static wchar_t * GetWPath( wchar_t * wszPath, size_t dwWLen, const char * szPath, size_t dwLen )
{
    wcscpy( wszPath,  L"\\\\?\\" );
    wchar_t * p = wszPath + 4;

    int nCount = MultiByteToWideChar( CP_ACP, 0, szPath, dwLen, p, dwWLen - 1 - 4 );
    p[nCount] = L'\0';

    return wszPath;
}

static wchar_t * GetWStr( wchar_t * wszStr, size_t dwWLen, const char * szStr, size_t dwLen )
{
    int nCount = MultiByteToWideChar( CP_ACP, 0, szStr, dwLen, wszStr, dwWLen - 1);
    wszStr[nCount] = L'\0';

    return wszStr;
}

static char * GetStr( char * szStr, size_t dwLen, const wchar_t * wszStr, size_t dwWLen )
{
    int nCount = WideCharToMultiByte( CP_ACP, 0, wszStr, dwWLen, szStr, dwLen - 1, 0, 0 );
    szStr[nCount] = '\0';
    
    return szStr;
}



namespace talk_base {

bool Win32Filesystem::CreateFolderI(const Pathname &pathname) {
  int len = pathname.pathname().length();

  if ((len <= 0) || (pathname.pathname().c_str()[len-1] != '\\')) {
    return false;
  }

  // modified by shijie
  wchar_t wszPath[512];
  GetWPath( wszPath, 512, pathname.pathname().c_str(), pathname.pathname().length() );

  // DWORD res = ::GetFileAttributes(Utf16(pathname.pathname()).AsWz());
  DWORD res = ::GetFileAttributes(wszPath);
  // end of modifying

  if (res != INVALID_FILE_ATTRIBUTES) {
    // Something exists at this location, check if it is a directory
    return ((res & FILE_ATTRIBUTE_DIRECTORY) != 0);
  } else if ((GetLastError() != ERROR_FILE_NOT_FOUND)
              && (GetLastError() != ERROR_PATH_NOT_FOUND)) {
    // Unexpected error
    return false;
  }
  // Directory doesn't exist, look up one directory level
  do {
    --len;
  } while ((len > 0) && (pathname.pathname().c_str()[len-1] != '\\'));

  if (!CreateFolder(std::string(pathname.pathname().c_str(),len)))
    return false;

  // modified by shijie
  //if (pathname.pathname().c_str()[0] != '\\') {
	 // std::string long_path = std::string("\\\\?\\") + pathname.pathname();
	 // return (::CreateDirectory(Utf16(long_path).AsWz(), NULL) != 0);
  //} else {
  //  return (::CreateDirectory(Utf16(pathname.pathname()).AsWz(), NULL) != 0);
  //}

  return (::CreateDirectory( wszPath, NULL ) != 0 );
  // end of modifying
}

FileStream *Win32Filesystem::OpenFileI(const Pathname &filename, 
			    const std::string &mode) {
  talk_base::FileStream *fs = new talk_base::FileStream();
  if (fs)
    fs->Open(filename.pathname().c_str(), mode.c_str());
  return fs;
}

bool Win32Filesystem::DeleteFileI(const Pathname &filename) {
  LOG(LS_INFO) << "Deleting " << filename.pathname();

  wchar_t   wszPath[512];;
  GetWPath( wszPath, 512, filename.pathname().c_str(), filename.pathname().length() );

   if (IsFolder(filename)) {
     Pathname dir;
     dir.SetFolder(filename.pathname());

     {
         DirectoryIterator di;
         di.Iterate(dir.pathname());
         while(di.Next()) {
             if (di.Name() == "." || di.Name() == "..")
                 continue;
             Pathname subdir;
             subdir.SetFolder(filename.pathname());
             subdir.SetFilename(di.Name());

             if (!DeleteFile(subdir.pathname()))
                 return false;
         }
     }
     


     ////////////////// modified by shijie
     //std::string no_slash(filename.pathname(), 0, filename.pathname().length()-1);
     // return ::RemoveDirectory(Utf16(no_slash).AsWz()) == 0;

     return ::RemoveDirectoryW( wszPath );
     ////////////////// end of modifying
   } 

   //// modified by shijie
   // return ::DeleteFile(Utf16(filename.pathname()).AsWz()) == 0;
   return ::DeleteFile(wszPath) == 0;
   //// end of modifying
}

bool Win32Filesystem::GetTemporaryFolderI(Pathname &pathname, bool create,
				    const std::string *append) {
 ASSERT(!g_application_name_.empty());
  wchar_t buffer[MAX_PATH + 1];
  if (!::GetTempPath(ARRAY_SIZE(buffer), buffer))
    return false;
  if (!::GetLongPathName(buffer, buffer, ARRAY_SIZE(buffer)))
    return false;
  size_t len = strlen(buffer);
  if ((len > 0) && (buffer[len-1] != '\\')) {
    len += talk_base::strcpyn(buffer + len, ARRAY_SIZE(buffer) - len,
                              L"\\");
  }
  if ((len > 0) && (buffer[len-1] != '\\')) {
    len += talk_base::strcpyn(buffer + len, ARRAY_SIZE(buffer) - len,
                              L"\\");
  }
  if (len >= ARRAY_SIZE(buffer) - 1)
    return false;
  pathname.clear();

  // modified by shijie
  // pathname.SetFolder(Utf8(buffer).AsSz());
  char buf[512];
  GetStr( buf, 512, buffer, wcslen(buffer) );
  pathname.SetFolder( buf );
  // end of modifying

  if (append != NULL)
    pathname.AppendFolder(*append);
  if (create)
    CreateFolderI(pathname);
  return true;
}

std::string Win32Filesystem::TempFilenameI(const Pathname &dir, const std::string &prefix) {
	wchar_t filename[MAX_PATH];

    // modified by shijie
	// if (::GetTempFileName(Utf16(dir.pathname()).AsWz(), Utf16(prefix).AsWz(), 0, filename) == 0)
	//	return Utf8(filename).AsString();

    wchar_t wszPath[512];
    GetWPath( wszPath, 512, dir.pathname().c_str(), dir.pathname().length() );

    wchar_t wszPrefix[512];
    int nCount = MultiByteToWideChar( CP_ACP, 0, prefix.c_str(), prefix.length(), wszPrefix, 512 - 1 - 4 );
    wszPrefix[nCount] = L'\0';

    if (::GetTempFileName( wszPath, wszPrefix, 0, filename ) == 0)
    {
        //return Utf8(filename).AsString();
        char buf[512];
        GetStr( buf, 512, filename, wcslen(filename) );
        return buf;
    }
    // end of modifying

	return "";
}

bool Win32Filesystem::MoveFileI(const Pathname &old_path, const Pathname &new_path) 
{
  LOG(LS_INFO) << "Moving " << old_path.pathname() << " to " << new_path.pathname();

  /////////////// modified by shijie
  //if (_wrename(Utf16(old_path.pathname()).AsWz(), Utf16(new_path.pathname()).AsWz()) != 0) {
  //  if (errno != EXDEV) {
  //    printf("errno: %d\n", errno);
  //    return false;
  //  }
  //  if (!CopyFile(old_path, new_path))
  //    return false;
  //  if (!DeleteFile(old_path))
  //    return false;
  //}

  int nRet = -1;
  if ( IsFolder(old_path) )
  {
      Pathname  ensureFolder( new_path.folder() );
      if ( !CreateFolder( ensureFolder ) )
      {
          return false;
      }

      wchar_t wszFrom[512];
      wchar_t wszTo[512];

      memset( wszFrom, 0, sizeof(wszFrom) );
      memset( wszTo,   0, sizeof(wszTo) );

      GetWStr( wszFrom, 512, old_path.pathname().c_str(), old_path.pathname().length() );
      GetWStr( wszTo,   512, new_path.pathname().c_str(), new_path.pathname().length() );

      size_t dwFromLen = wcslen( wszFrom );
      if ( wszFrom[dwFromLen - 1] = L'\\' )
      {
          wszFrom[dwFromLen - 1] = L'\0';
      }


      SHFILEOPSTRUCT   sfo; 
      sfo.hwnd    =   NULL; 
      sfo.wFunc   =   FO_MOVE; 
      sfo.pFrom   =   wszFrom;
      sfo.pTo     =   wszTo;
      sfo.fFlags  =   FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR; 

      nRet = SHFileOperation(&sfo);

      if ( 0 != nRet )
      {
          if (!CopyFile(old_path, new_path))
              return false;
          if (!DeleteFile(old_path))
              return false;
      }
      else
      {
          //wchar_t * pSlach = wcsrchr( wszFrom, L'\\' );
          //if ( pSlach )
          //{
          //    *pSlach     = L'\0';
          //}

          //char szFrom[512];
          //GetStr( szFrom, 512, wszFrom, wcslen( wszFrom) );

          //Pathname tmpPath( szFrom ) ;
          //
          //if (!DeleteFile(tmpPath))
          //    return false;
      }

  }
  else
  {
      Pathname  ensureFolder( new_path.folder() );
      if ( !CreateFolder( ensureFolder ) )
      {
          return false;
      }

      // modified by shijie
      wchar_t wszFrom[512];
      wchar_t wszTo[512];

      wcscpy( wszFrom, L"\\\\?\\" );
      wchar_t * p = wszFrom + 4;
      int nCount = MultiByteToWideChar( CP_ACP, 0, old_path.pathname().c_str(), old_path.pathname().length(), 
                                        p, 512 - 1 - 4 );
      p[nCount] = L'\0';

      wcscpy( wszTo, L"\\\\?\\" );
      p = wszTo + 4;
      nCount = MultiByteToWideChar( CP_ACP, 0, new_path.pathname().c_str(), new_path.pathname().length(), 
          p, 512 - 1 - 4 );
      p[nCount] = L'\0';

      // nRet = _wrename(Utf16(old_path.pathname()).AsWz(), Utf16(new_path.pathname()).AsWz());
      nRet = _wrename( wszFrom, wszTo );
      // end of modifying
      if ( 0 != nRet )
      {
          if (errno != EXDEV) {
              printf("errno: %d\n", errno);
              return false;
          }
          if (!CopyFile(old_path, new_path))
              return false;
          if (!DeleteFile(old_path))
              return false;
      }
  }

  //////////////// end of modifying
  return true;
}

bool Win32Filesystem::IsFolderI(const Pathname &path)
{
  WIN32_FILE_ATTRIBUTE_DATA data = {0};

  // modified by shijie
  wchar_t wszPath[512];
  GetWPath( wszPath, 512, path.pathname().c_str(), path.pathname().length() );

  // if (0 == ::GetFileAttributesEx(Utf16(path.pathname()).AsWz(), GetFileExInfoStandard, &data))
  if (0 == ::GetFileAttributesEx( wszPath, GetFileExInfoStandard, &data))
  // end of mosidyfing
    return false;
  return (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}

bool Win32Filesystem::FileExistsI(const Pathname &path)
{
  // modified by shijie
  wchar_t wszPath[512];
  GetWPath( wszPath, 512, path.pathname().c_str(), path.pathname().length() );

  // DWORD res = ::GetFileAttributes(Utf16(path.pathname()).AsWz());
  DWORD res = ::GetFileAttributes( wszPath );
  // end of modifying
  return res != INVALID_FILE_ATTRIBUTES;
}

bool Win32Filesystem::CopyFileI(const Pathname &old_path, const Pathname &new_path) 
{
    // modified by shijie
    wchar_t wszOld[512];
    GetWPath( wszOld, 512, old_path.pathname().c_str(), old_path.pathname().length() );

    wchar_t wszNew[512];
    GetWPath( wszNew, 512, new_path.pathname().c_str(), new_path.pathname().length() );

  // return ::CopyFile(Utf16(old_path.pathname()).AsWz(), Utf16(new_path.pathname()).AsWz(), TRUE) == 0;
    return ::CopyFile( wszOld, wszNew, TRUE) == 0;
  // end of modifying
}

bool Win32Filesystem::IsTemporaryPathI(const Pathname& pathname)
{
  TCHAR buffer[MAX_PATH + 1];
  if (!::GetTempPath(ARRAY_SIZE(buffer), buffer))
    return false;
  if (!::GetLongPathName(buffer, buffer, ARRAY_SIZE(buffer)))
    return false;

  // modified by shijie
  wchar_t wszPath[512];
  int nCount = MultiByteToWideChar( CP_ACP, 0, pathname.pathname().c_str(), pathname.pathname().length(), 
                                    wszPath, 512 - 1 );
  wszPath[nCount] = L'\0';

  // return (::strnicmp(Utf16(pathname.pathname()).AsWz(), buffer, strlen(buffer)) == 0);
  return (::strnicmp( wszPath, buffer, strlen(buffer)) == 0);
  // end of modifying
}

bool Win32Filesystem::GetFileSizeI(const Pathname &pathname, size_t *size)
{
  // modified by shijie
  wchar_t buf[512];
  wcscpy( buf, L"\\\\?\\" );
  wchar_t * p = buf + 4;
  int nCount = MultiByteToWideChar( CP_ACP, 0, pathname.pathname().c_str(), pathname.pathname().length(), 
                                    p, 512 - 1- 4 );
  p[nCount] = L'\0';


  WIN32_FILE_ATTRIBUTE_DATA data = {0};
  //if (::GetFileAttributesEx( Utf16(pathname.pathname()).AsWz(), GetFileExInfoStandard, &data) == 0)
  // end of modifying

  // 因为对于网络邻居上的文件名，前加\\?\会导致失败
  // if (::GetFileAttributesEx( buf, GetFileExInfoStandard, &data) == 0)
  if (::GetFileAttributesEx( p, GetFileExInfoStandard, &data) == 0)
  {
	  return false;
  }
  *size = data.nFileSizeLow;
  return true;
}

}

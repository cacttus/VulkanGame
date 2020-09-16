/**
*  @file FileSystem.h
*  @date August 24, 2013
*  @author MetalMario971
*/
#pragma once
#ifndef __FILESYSTEM_14848814891654953164_H__
#define __FILESYSTEM_14848814891654953164_H__

#include "../base/BaseHeader.h"

namespace BR2 {
/**
*  @class FileInfo
*  @brief Boiled down unix stat() structure.
*/
class FileInfo : public VirtualMemory {
public:
  bool _exists;
  std::string _originalPath;
  std::string _rootedPath;
  time_t _accssed;     //Last read time
  time_t _modified;    //Last write time
  time_t _status;      //Last status change time
  mode_t _protection;  // should be mode_t
  int64_t _size;       //size in bytes
  FileInfo() {}
  virtual ~FileInfo() {}
};
/**
*  @class FileSystem
*  @brief Filesystem functions.
*     In the future a lot of this can be refactored with C++17's std::filesystem
*/
class FileSystem : public VirtualMemory {
public:
  FileSystem();
  virtual ~FileSystem() override;
  static void init(const string_t& exePath);
  static string_t getExecutableFullPath();
  static string_t getExecutableName();
  static void setExecutablePath(const string_t& st);
  static string_t getExecutableDirectory();
  static string_t combinePath(const string_t& a, const string_t& b);
  static string_t formatPath(const string_t& p);
  static bool isUNC(const string_t& p);
  static string_t getRoot(const string_t& p);
  static string_t getRootedPath(const string_t& p);
  static bool pathIsAbsolute(const string_t& p);
  static bool createFile(const string_t& filename, bool truncate = false, bool bLog = true);
  static void createFileIfFileDoesNotExist(const string_t& filename, bool bAlsoCreateDirectoryPath = true);
  static bool createDirectorySingle(const string_t& dirName, int permissions = 755);  //Note: this usually fails. windwos is dubm
  static bool createDirectoryRecursive(const string_t& dirName);
  static bool directoryExists(const string_t& dirName);
  static bool fileExists(const string_t& filename);
  static bool getAllFiles(const string_t& dir, std::vector<string_t>& __out_ files);
  static bool getAllDirs(const string_t& dir, std::vector<string_t>& __out_ dirs);

  static time_t getLastModifyTime(const string_t& location);
  static string_t getCurrentDirectory();
  static void setCurrentDirectory(const string_t& str);

  static bool deleteFile(const string_t& filename);
  static bool deleteAllFiles(const string_t& dir, std::vector<string_t>& vecFileExts);
  static bool deleteDirectory(const string_t& dirname, std::vector<string_t>& vecFileExts);
  static bool deleteDirectoryRecursive(const string_t& dirName, std::vector<string_t>& vecFileExts);

  static string_t getExtensionPartOfFileName(const string_t& fileName);
  static string_t getFilePartOfFileName(const string_t& fileName);
  static string_t getDirectoryNameFromPath(const string_t& name);  //Returns the second-level directory in the path (regardless of whether it is a file or not, see C# GetDirectoryName)
  static string_t getFileNameFromPath(const string_t& name);       //Returns top level file OR directory. See C# GetFileName()

  static bool isFile(const string_t& fileOrDirPath);
  static bool isDir(const string_t& fileOrDirPath);

  static string_t getScreenshotFilename();
  static string_t replaceInvalidCharsFromFilename(const string_t& __in_ fnIn, char replaceChar = '_', bool bIgnoreSlashes = false);

  static void SDLFileFree(char*& pOutData);
  static int SDLFileRead(const string_t& fname, char*& pOutData, int64_t& _iOutSizeBytes, bool addNull = false);
  static int SDLFileWrite(const string_t& fname, char* pData, size_t _iDataSizeBytes);
  static string_t getFilePath(const string_t& name);

  static FileInfo getFileInfo(const string_t& loc);

private:
  static string_t _strExePath;
  static bool getAllFilesOrDirs(const string_t& dir, std::vector<string_t>& __out_ dirs, bool bFiles);
};

}  // namespace BR2

#endif

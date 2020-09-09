#include "../base/FileSystem.h"
#include "../base/ApplicationPackage.h"
#include "../base/Exception.h"
#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../base/StringUtil.h"

#include "../base/DateTime.h"
#include <fstream>
#include <unistd.h>

#include <filesystem>

#if defined(BR2_OS_WINDOWS)
#include "../ext/dirent.h"
#include <direct.h>
#elif defined(BR2_OS_LINUX)
// Linux
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#endif

namespace BR2 {
string_t FileSystem::_strExePath = "";
FileSystem::FileSystem() {}
FileSystem::~FileSystem() {}
void FileSystem::init(string_t executablePath) {
  FileSystem::setExecutablePath(executablePath);
  string_t a = FileSystem::getCurrentDirectory();
  FileSystem::setCurrentDirectory(FileSystem::getExecutableDirectory());
  string_t b = FileSystem::getCurrentDirectory();
}
void FileSystem::setExecutablePath(string_t st) {
  _strExePath = st;
}
string_t FileSystem::getExecutableFullPath() {
  //  t_string ret;
  //  char* p = SDL_GetBasePath();
  //  if (p == nullptr) {
  //      BroLogError("SDL Doesn't implement SDL_GetBasePath. Using program
  //      argument '"+FileSystem::_strExePath+"'  This will result in errors.");
  string_t ret = FileSystem::_strExePath;
  // }
  //  else {
  //      ret = t_string(p);
  //   }
  return ret;
}
string_t FileSystem::getExecutableName() {
  string_t ret;
  ret = FileSystem::getFileNameFromPath(getExecutableFullPath());
  return ret;
}
string_t FileSystem::getExecutableDirectory() {
  string_t ret;
  ret = FileSystem::getDirectoryNameFromPath(getExecutableFullPath());
  return ret;
}
string_t FileSystem::combinePath(string_t a, string_t b) {
#if defined(BR2_OS_LINUX)
  string_t st = std::filesystem::path(a) / b;
  return st;
#else
  if ((a.length() == 0) || (b.length() == 0)) {
    return a + b;
  }

  // - Make sure path is a / path not a \\ path
  a = formatPath(a);
  b = formatPath(b);

  // Remove / from before and after strings
  a = StringUtil::trim(a, '/');
  b = StringUtil::trim(b, '/');

  return a + string_t("/") + b;
#endif
}
bool FileSystem::createFile(const string_t &filename, bool trunc, bool bLog) {
  std::fstream pStream;
  bool exists = true;

  // TODO: specify in or out in case of read / write access problems.
  pStream.open(filename.c_str(), std::ios::out | (trunc ? (std::ios::app) : (std::ios::trunc)));

  if (!pStream.good()) {
    exists = false;
    string_t err = Stz " [FileSystem] Output file " + filename +
                   " could not be created with trunc=" + (int32_t)trunc;
    if (bLog == true) {  // For logger initializtion
      BRLogError(err);
    }
    else {
      Gu::print(err);
    }
  }

  pStream.close();

  return exists;
}

bool FileSystem::createDirectoryRecursive(string_t dirName) {
  bool bRet = true;
#if defined(BR2_OS_LINUX)
  //Note: this creates directories with full permissions.
  std::error_code code;
  bRet = std::filesystem::create_directories(dirName, code);
  int a = code.value();
  int b = code.default_error_condition().value();
  if (a != b) {
    BRLogError(code.message());
  }
#else
  string_t dirCpy;
  dirCpy.assign(dirName);
  dirCpy = formatPath(dirCpy);

  if (directoryExists(dirName) == true) {
    return true;
  }

  std::vector<string_t> dirs = StringUtil::split(dirCpy, '/');
  string_t strDir = dirs[0];
  for (size_t i = 1; i < dirs.size(); ++i) {
    strDir = FileSystem::combinePath(strDir, dirs[i]);
    bRet = bRet && FileSystem::createDirectorySingle(strDir);
  }

#endif
  return bRet;
}
bool FileSystem::directoryExists(string_t dirName) {
  bool ret;
  DIR *dir = NULL;

  dir = opendir(dirName.c_str());

  if (dir == NULL) {
    ret = false;
  }
  else {
    ret = true;
  }

  closedir(dir);

  return ret;
}
bool FileSystem::createDirectorySingle(string_t &dirName, int permissions) {
  int err = 0;
#if defined(BR2_OS_LINUX)
  int ret = mkdir(dirName.c_str(), permissions);
  if (ret == -1) {
    err = errno;
  }
#elif defined(BR2_OS_WINDOWS)
  //TODO: windows permissions ..
  //#ifdef BR2_OS_WINDOWS
  //    //SECURITY_ATTRIBUTES sc;
  //    //sc.nLength = sizeof(sc);
  //    //sc.bInheritHandle = false;
  //    //sc.lpSecurityDescriptor = NULL;
  //
  //    BOOL b = CreateDirectory(dir.c_str(),NULL);
  //#else
  // - Open / Make directory
  //  if (opendir(dir.c_str()) == NULL)
  //  {
  //      if (_mkdir(dir.c_str()) == -1)
  //          throw new Exception("Could not make log
  //          directory.",__LINE__,__FILE__);
  //  }
  // use mkdir
  //#endif

  //I think windows uses the return type as the error TODO: check this.
  err = mkdir(dirName.c_str());
#else
  OS_NOT_SUPPORTED_ERROR
#endif

  //https://man7.org/linux/man-pages/man2/mkdir.2.html
  if (err != 0) {
    if (err == EEXIST) {
      //pathname already exists (not necessarily as a directory). This includes the case where pathname is a symbolic link, dangling or not.
    }
    else if (err == ENOENT) {
      //A directory component in pathname does not exist or is a dangling symbolic link.
      //Failure.
      return false;
    }
    else {
    }
  }

  return true;
}
string_t FileSystem::getFileNameFromPath(const string_t &name) {
  //Returns the TLD for the path. Either filename, or directory.
  // Does not include the / in the directory.
  // ex ~/files/dir would return dir
  // - If formatPath is true then we'll convert the path to a / path.
  string_t fn = "";
#if defined(BR2_OS_LINUX)
  fn = std::filesystem::path(name).filename();
#else

  DiskLoc l2;

  l2 = formatPath(name);

  //Index of TLD
  size_t tld_off = l2.rfind('/');

  if (tld_off != string_t::npos) {
    fn = l2.substr(tld_off + 1, l2.length() - tld_off + 1);
  }
  else {
    fn = name;
  }
#endif
  return fn;
}

string_t FileSystem::getDirectoryNameFromPath(const string_t &pathName) {
  // Returns the path without the filename OR top level directory.
  // See C# GetDirectoryName()
  string_t ret = "";
  DiskLoc formattedPath;
  DiskLoc tmpPath = pathName;
  string_t fn;

  formattedPath = formatPath(pathName);
  fn = getFileNameFromPath(formattedPath);
  size_t x = formattedPath.rfind('/');
  if (x != string_t::npos) {
    ret = formattedPath.substr(0, x);
  }
  else {
    ret = formattedPath;
  }

  return ret;
}
bool FileSystem::fileExists(string_t filename) {
  //**NOTE: a portable option would be to use this code from SDL.
  // However the code below is supposed to be "faster"
  /*
  SDL_RWops *rw = SDL_RWFromFile(filename.c_str(), "rb");
  if (rw == nullptr)
  return false;

  SDL_RWclose(rw);
  return true;
  */

  //**Alternate version
  bool exists;
  struct stat buffer;
  exists = (stat(filename.c_str(), &buffer) == 0);

#if defined(BR2_OS_WINDOWS)
  // We get errors when calling stat in Windows for some reason.
  SetLastError(0);
#endif

  return exists;
}
void FileSystem::createFileIfFileDoesNotExist(string_t &filename, bool bAlsoCreateDirectoryPath) {
  if (bAlsoCreateDirectoryPath == true) {
    createDirectoryRecursive(getDirectoryNameFromPath(filename));
  }

  if (FileSystem::fileExists(filename) == false) {
    createFile(filename, false);
  }
}
time_t FileSystem::getLastModifyTime(string_t &location) {
  // Gets the last time the file was modified as a time_t
  struct stat fileInfo;

  if (FileSystem::fileExists(location) == false) {
    BRThrowException("File '" + location + "' does not exist");
  }

  stat(location.c_str(), &fileInfo);
  return fileInfo.st_mtime;
}
string_t FileSystem::getCurrentDirectory() {
  // DO NOT LOG HERE
  char buf[BRO_MAX_PATH];
#if defined(BR2_OS_WINDOWS)
  _getcwd(buf, BRO_MAX_PATH);
#elif defined(BR2_OS_LINUX)
  getcwd(buf, BRO_MAX_PATH);
#endif

  string_t cwd = string_t(buf);
  return cwd;
}
void FileSystem::setCurrentDirectory(string_t str) {
// DO NOT LOG HERE
#if defined(BR2_OR_WINDOWS)
  _chdir(str.c_str());
#elif defined(BR2_OS_LINUX)
  chdir(str.c_str());
#endif
}

bool FileSystem::isFile(string_t fileOrDirPath) {
  // https://stackoverflow.com/questions/146924/how-can-i-tell-if-a-given-path-is-a-directory-or-a-file-c-c
  struct stat s;
  if (stat(fileOrDirPath.c_str(), &s) == 0) {
    if (s.st_mode & S_IFREG) {
      return true;
    }
  }
  else {
    // DO NOT LOG HERE
    // BroLogError("stat - failed");
  }
  return false;
}
bool FileSystem::isDir(string_t fileOrDirPath) {
  // https://stackoverflow.com/questions/146924/how-can-i-tell-if-a-given-path-is-a-directory-or-a-file-c-c
  struct stat s;
  if (stat(fileOrDirPath.c_str(), &s) == 0) {
    if (s.st_mode & S_IFDIR) {
      return true;
    }
  }
  else {
    // DO NOT LOG HERE
    // BroLogError("stat - failed");
  }
  return false;
}
bool FileSystem::getAllFilesOrDirs(string_t dir,
                                   std::vector<string_t> &__out_ dirs,
                                   bool bFiles) {
  dirs.resize(0);

  dirent *ep;
  DIR *dp;
  dp = opendir(dir.c_str());

  if (dp != NULL) {
    while ((ep = readdir(dp))) {
      string_t name(ep->d_name);
      if (StringUtil::doesNotEqual(name, "..") &&
          StringUtil::doesNotEqual(name, ".")) {
        string_t nameFull = FileSystem::combinePath(dir, name);
        if (!bFiles && FileSystem::isDir(nameFull)) {
          dirs.push_back(nameFull);
        }
        else if (bFiles && FileSystem::isFile(nameFull)) {
          dirs.push_back(nameFull);
        }
      }
    }
    closedir(dp);
  }
  else {
    BRLogError("Couldn't open the directory '" + dir + "'");
    return false;
  }
  return true;
}
bool FileSystem::getAllDirs(string_t dir, std::vector<string_t> &__out_ dirs) {
  return getAllFilesOrDirs(dir, dirs, false);
}
bool FileSystem::getAllFiles(string_t dir,
                             std::vector<string_t> &__out_ files) {
  return getAllFilesOrDirs(dir, files, true);
}
bool FileSystem::deleteDirectoryRecursive(string_t dir,
                                          std::vector<string_t> &vecFileExts) {
  std::vector<string_t> dirs;
  if (getAllDirs(dir, dirs) == false) {
    return false;
  }
  for (string_t dir : dirs) {
    if (deleteDirectoryRecursive(dir, vecFileExts) == false) {
      return false;
    }
  }

  if (deleteAllFiles(dir, vecFileExts) == false) {
    return false;
  }

  int ret = rmdir(dir.c_str());
  if (ret != 0) {
    BRLogError("Failed to delete directory.");
    return false;
  }
  return true;
}
bool FileSystem::deleteDirectory(string_t dir,
                                 std::vector<string_t> &vecFileExts) {
  int ret = rmdir(dir.c_str());
  if (ret != 0) {
    BRLogError("Failed to delete directory.");
    return false;
  }
  return true;
}
bool FileSystem::deleteAllFiles(string_t dir,
                                std::vector<string_t> &vecFileExts) {
  // vecFileExts - list of file extensions with '.' like '.exe', '.dat'

  std::vector<string_t> files;
  if (getAllFiles(dir, files) == false) {
    BRLogError("Failed to get files from '" + dir + "'");
    return false;
  }

  for (string_t file : files) {
    if (FileSystem::fileExists(file)) {
      bool bMatch = false;
      for (string_t ext : vecFileExts) {
        if (StringUtil::equalsi(FileSystem::getExtensionPartOfFileName(file),
                                ext)) {
          bMatch = true;
          break;
        }
      }
      if (bMatch) {
        if (FileSystem::deleteFile(file) == false) {
          BRLogError("Failed to delte file '" + file + "'");
          return false;
        }
      }
    }
  }

  return true;
}
bool FileSystem::deleteFile(string_t filename) {
  int32_t ret = std::remove(filename.c_str());
  // Equivalent POSIX function
  // unlink(filename.c_str())
  if (ret != 0) {
    BRLogError("Failed to delete file '" + filename + "'");
    return false;
  }
  return true;
}

string_t FileSystem::getFilePartOfFileName(const string_t &fileName) {
  size_t off = fileName.rfind(".");

  if (off >= fileName.size() || off == string_t::npos) {
    BRThrowException(
        "Incorrect file name, no extension given or incorrect extension.");
  }

  return fileName.substr(0, off);
}
string_t FileSystem::getExtensionPartOfFileName(const string_t &name) {
  // 2017 12 22 - **Changed this to return the DOT just like .NET ( .exe, .dat
  // etc..)
  size_t x;
  string_t ext = "";
  string_t fileName;

  fileName = FileSystem::getFileNameFromPath(name);

  if ((x = fileName.rfind('.')) != string_t::npos) {
    ext = fileName.substr(x, fileName.length() - x);
  }

  return ext;
}
string_t FileSystem::formatPath(string_t p) {
  return StringUtil::replaceAll(p, '\\', '/');
}
string_t FileSystem::getScreenshotFilename() {
  string_t fname = DateTime::getAsString() + "_" + FileSystem::getExecutableName() + "_frame.png";
  fname = FileSystem::replaceInvalidCharsFromFilename(fname);
  fname = StringUtil::replaceAll(fname, " ", "_");
  fname = FileSystem::combinePath(Gu::getPackage()->getCacheFolder(), fname);

  return fname;
}
string_t FileSystem::replaceInvalidCharsFromFilename(const string_t &__in_ fnIn,
                                                     char replaceChar,
                                                     bool bIgnoreSlashes) {
  // Remove invalid WINDOWS characters from path
  string_t fname = fnIn;
  fname = StringUtil::replaceAll(fname, "<", Stz(replaceChar));
  fname = StringUtil::replaceAll(fname, ">", Stz(replaceChar));
  fname = StringUtil::replaceAll(fname, ":", Stz(replaceChar));
  fname = StringUtil::replaceAll(fname, "\"", Stz(replaceChar));
  if (bIgnoreSlashes == false) {
    fname = StringUtil::replaceAll(fname, "/", Stz(replaceChar));
    fname = StringUtil::replaceAll(fname, "\\", Stz(replaceChar));
  }
  fname = StringUtil::replaceAll(fname, "|", Stz(replaceChar));
  fname = StringUtil::replaceAll(fname, "?", Stz(replaceChar));
  fname = StringUtil::replaceAll(fname, "*", Stz(replaceChar));

  return fname;
}
void FileSystem::SDLFileFree(char *&pOutData) {
  delete[] pOutData;
  pOutData = nullptr;
}
int FileSystem::SDLFileRead(std::string fname, char *&pOutData,
                            int64_t &_iOutSizeBytes, bool addNull) {
  fname = getFilePath(fname);

  SDL_RWops *rw = SDL_RWFromFile(fname.c_str(), "rb");
  if (rw == NULL)
    return 1;

  _iOutSizeBytes = SDL_RWsize(rw);
  if (addNull) {
    pOutData = new char[_iOutSizeBytes + 1];
  }
  else {
    pOutData = new char[_iOutSizeBytes];
  }

  Sint64 nb_read_total = 0, nb_read = 1;
  char *buf = pOutData;
  while (nb_read_total < _iOutSizeBytes && nb_read != 0) {
    nb_read = SDL_RWread(rw, buf, 1, _iOutSizeBytes - nb_read_total);
    nb_read_total += nb_read;
    buf += nb_read;
  }
  SDL_RWclose(rw);
  if (nb_read_total != _iOutSizeBytes) {
    delete[] pOutData;
    return 1;
  }

  // So yeah sometimes you just gotta..
  if (addNull) {
    pOutData[nb_read_total] = '\0';
  }
  return 0;
}
int FileSystem::SDLFileWrite(std::string fname, char *pData,
                             size_t _iDataSizeBytes) {
  fname = getFilePath(fname);

  SDL_RWops *rw = SDL_RWFromFile(fname.c_str(), "wb");
  if (rw == NULL)
    return 1;

  SDL_RWwrite(rw, pData, 1, _iDataSizeBytes);
  SDL_RWclose(rw);

  return 0;
}

std::string FileSystem::getFilePath(std::string name) {
  std::string str;
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
  // Gosh napoleon
  int pos1 = name.rfind("/");
  int pos2 = name.rfind("\\");

  int pos3;
  if (pos1 != std::string::npos && pos2 != std::string::npos) {
    if (pos1 > pos2) {
      pos3 = pos1;
    }
    else {
      pos3 = pos2;
    }
  }
  else if (pos1 != std::string::npos) {
    pos3 = pos1;
  }
  else if (pos2 != std::string::npos) {
    pos3 = pos2;
  }
  else {
    // Error
    throw 0;
    return "";
  }

  std::string fname = name.substr(pos3 + 1, name.length() - (pos3 + 1));
  std::string fdir = name.substr(0, pos3 + 1);

  CFBundleRef mainBundle = CFBundleGetMainBundle();
  CFStringRef rName = Gu::stdStrToCFStr(fname);
  CFStringRef rDir =
      CFSTR("");  // stdStrToCFStr(fdir);//CFSTR("");
                  // It appears that the bundle doesn't use directories we supply
                  // If it does we should change this in the futrue.

  // CFStringFileNa
  // NULL file extension - indicates extension is in the name
  CFURLRef rUrl = CFBundleCopyResourceURL(mainBundle, rName, NULL, rDir);

  if (rUrl == NULL) {
    throw 0;
    return "";
  }
  // stackoverflow.com/questions/21933548/how-to-read-a-xml-file-using-fstream-in-ios
  CFStringRef filePath = CFURLCopyFileSystemPath(rUrl, kCFURLPOSIXPathStyle);
  CFStringEncoding encodingMethod = CFStringGetSystemEncoding();
  const char *path = CFStringGetCStringPtr(filePath, encodingMethod);
  str = std::string(path);

#endif
#else
  // WIN / ANDROID
  str = name;
#endif
  return str;
}

FileInfo FileSystem::getFileInfo(std::string loc) {
  FileInfo inf;
  inf._originalPath = loc;
  inf._exists = FileSystem::fileExists(loc);
  if (!inf._exists) {
    return inf;
  }

  // Gets the last time the file was modified as a time_t
  struct stat fi;
  stat(loc.c_str(), &fi);
  inf._rootedPath = getRootedPath(loc);
  inf._modified = fi.st_mtime;
  inf._status = fi.st_ctime;
  inf._accssed = fi.st_atime;
  inf._protection = fi.st_mode;
  inf._size = fi.st_size;

  return inf;
}
string_t FileSystem::getRootedPath(string_t loc) {
  // C++17 has std::filesystem::path path(loc);
  // currently not supported so we need our own.
  string_t path = "";

#if defined(BR2_OS_LINUX)
  path = std::filesystem::absolute(loc);
#else

  // Get root of current.
  if (!pathIsAbsolute(loc)) {
    path = combinePath(FileSystem::getCurrentDirectory(), loc);
  }
#endif
  return path;
}
string_t FileSystem::getRoot(string_t in_loc) {
  // C++17 has std::filesystem::path path(loc);
  // currently not supported so we need our own.

  // returns the root of the path (c:, e:, or \\,// for UNC) of the current
  // path. if the path is relative, then it is assumed to be relative to the
  // current directory set with setCurrentDirectory()
  string_t s = "";
  string_t path = "";

  if (pathIsAbsolute(in_loc)) {
    path = in_loc;
  }
  else {
    path = FileSystem::getCurrentDirectory();
  }

  if (path.length() > 0) {
    if (pathIsAbsolute(path)) {
      if (isUNC(path)) {
        s = Stz path[0] + path[1];
      }
      else {
        char c = path[0];
        for (int i = 0; (i < path.length()) && (c != ':'); ++i) {
          s += c;
        }
      }
    }
  }

  return s;
}
bool FileSystem::isUNC(string_t path) {
  // Returns true if the path is a valid UNC root
  if (path.length() == 0) {
    return false;
  }
  bool uncFs = path[0] == '/' && path[1] == '/' && path[2] != '/';
  bool uncBs = path[0] == '\\' && path[1] == '\\' && path[2] != '\\';

  return (uncFs || uncBs);
}
bool FileSystem::pathIsAbsolute(string_t path) {
#if defined(BR2_OS_WINDOWS)
  // C++14 has std::filesystem::path path(loc);
  // This wasn't supported (last windows build) so this legacy may be removed.
  Gu::debugBreak();  //TEST THIS
  if (isUNC(path)) {
    return true;
  }
  if (path.length() == 0) {
    return false;
  }
  if (path[0] == '.') {
    return false;
  }
  if (path[0] == '/') {
    return false;
  }
  if (path[0] == '\\') {
    return false;
  }
  return true;
#elif defined(BR2_OS_LINUX)
  std::filesystem::path p(path);
  bool isabs = p.is_absolute();
  return isabs;
#else
  OS_NOT_SUPPORTED_ERROR
#endif
}

}  // namespace BR2



#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/OperatingSystem.h"
#include "../base/WindowsIncludes.h"

#include <iostream>

#include <SDL2/SDL.h>

#ifndef BRO_OS_WINDOWS
//This gets the OS name
#include <sys/utsname.h>
//Sort of similar to windows GetLastError
#include <sys/errno.h>
#endif

namespace BR2 {

string_t OperatingSystem::getOperatingSystemName() {
  string_t res;
#if defined(BR2_OS_WINDOWS)
  OSVERSIONINFOEX vex;
  vex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  GetVersionExW((OSVERSIONINFO*)&vex);

  if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 3) {
    res.append(" Windows 8.1");
  }
  else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 2) {
    res.append(" Windows 8");
  }
  else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 1) {
    res.append(" Windows 7");
  }
  else if (vex.dwMajorVersion == 6 && vex.dwMinorVersion == 0) {
    res.append(" Windows Vista");
  }
  else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 2) {
    res.append(" Windows XP Pro 64 bit");
  }
  else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 1) {
    res.append(" Windows XP");
  }
  else if (vex.dwMajorVersion == 5 && vex.dwMinorVersion == 0) {
    res.append(" Windows 2000");
  }
  else {
    res.append(" OS Unknown.  Note: Mac / Linux are not supported.");
  }

  if (vex.wServicePackMajor != 0) {
    res += Stz ", Service Pack " + vex.wServicePackMajor + "." + vex.wServicePackMinor;
  }
  else {
    res += Stz ", No service pack";
  }

  if (vex.wProductType == VER_NT_DOMAIN_CONTROLLER) {
    res.append(", Domain Controller, note operating system may be incorrect as this is not supported");
  }
  else if (vex.wProductType == VER_NT_SERVER) {
    res.append(", Server, note operating system may be incorrect as this is not supported");
  }
  else if (vex.wProductType == VER_NT_WORKSTATION) {
    res.append(", Workstation");
  }

#else
  struct utsname uts;
  uname(&uts);
  res = uts.sysname;
#endif
  return res;
}
string_t OperatingSystem::getUserFolderPath() {
  //returns "My Documents" on windows.
  //Linux ~ or /home/(user)/
  string_t ret;
#if defined(BR2_OS_WINDOWS)
  char lpstrPath[BRO_MAX_PATH];
  //TODO: Vista and beyond
  //SHGetKnownFolderPath
  HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, lpstrPath);

  ret = string_t(lpstrPath);
  OperatingSystem::suppressError(OSErrorCode::ErrorNoToken, false);

#else
  ret = std::string("~/");
#endif
  return ret;
}
string_t OperatingSystem::getGamesFolderPath() {
  // * Apparently, FOLDERID_GAMES doesn't work.
  string_t ret = "";
  /*
 // CheckOsErrorsDbg();
#ifdef BR2_OS_WINDOWS
    PWSTR lpwstrPath = NULL;
    HRESULT result;

    // Win Vista and beyond ONLY
    result = SHGetKnownFolderPath(FOLDERID_SavedGames,KF_FLAG_DONT_UNEXPAND|KF_FLAG_CREATE,NULL,&lpwstrPath);
    if(result == E_FAIL)
        BroThrowException("Folder could not be found.");
    ret = wstrToStr(lpwstrPath);

    CoTaskMemFree(static_cast<void*>(lpwstrPath));

#elif BRO_OS_UNIX

    ret = t_string("/usr/games/");
#endif
    CheckOsErrorsDbg();*/
  return ret;
}
void OperatingSystem::suppressError(OSErrorCode ec, bool bWriteMessage) {
  string_t strMsg = "";
#if defined(BR2_OS_WINDOWS)
  switch (ec) {
    case OSErrorCode::FileNotFound:
      strMsg = "File Not Found";
    case OSErrorCode::PathNotFound:
      strMsg = "Path Not Found";
    case OSErrorCode::NoGuidTranslation:
      strMsg = "No GUID Translation for Security Audit";
    case OSErrorCode::ProcNotFound:
      strMsg = "Proc Not Found";
    default:
      strMsg = "Failed to register operating system error code. Code was not defined in the engine.";
  };
#else
  //TODO: Linux

#endif
  if (getError() == (int)ec) {
    clearAllErrors();
    if (bWriteMessage) {
      BRLogInfo(strMsg);
    }
  }
}
void OperatingSystem::showErrorDialog(const string_t& str, const string_t& title) {
  //Sweet. Thanks SDL.
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), str.c_str(), NULL);
}
void OperatingSystem::clearAllErrors() {
#if defined(BR2_OS_WINDOWS)
  SetLastError(0);
#else
  throw new NotImplementedException();
#endif
}
int32_t OperatingSystem::getError() {
#if defined(BR2_OS_WINDOWS)
  return GetLastError();
#else
  throw new NotImplementedException();
#endif
}

//size_t OperatingSystem::getAvailableMemory()
//{
//#ifdef BR2_OS_WINDOWS
//    MEMORYSTATUSEX mmex;
//    mmex.dwLength = sizeof(mmex);
//
//    GlobalMemoryStatusEx(&mmex);
//  //  CheckOsErrorsDbg();
//    return mmex.ullAvailPhys;
//#else
//   OS_METHOD_NOT_IMPLEMENTED
//#endif
//}
//size_t OperatingSystem::getProcessMemoryUsage()
//{
//
//#ifdef BR2_OS_WINDOWS
//    PROCESS_MEMORY_COUNTERS pmex;
//    GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmex, sizeof(PROCESS_MEMORY_COUNTERS) );
//
//    return pmex.WorkingSetSize;
//#else
//  OS_METHOD_NOT_IMPLEMENTED
//#endif
//}

string_t OperatingSystem::getRuntimeEnvironmentStr() {
#if defined(_WIN32)
  return string_t("Windows 32 Bit");
#elif defined(_WIN64)
  return string_t("Windows 64 Bit");
#elif defined(__LP32__) || defined(_LP32)
  return string_t("Linux 32 Bit");
#elif defined(__LP64__) || defined(_LP64)
  return string_t("Linux 64 Bit");
#else
  throw new NotImplementedException();
#endif
}
int OperatingSystem::strCaseCmp(const string_t& str1, const string_t& str2) {
#if defined(BR2_OS_WINDOWS)
  //win32 only
  return _stricmp(str1.c_str(), str2.c_str());
#elif defined(BR2_OS_LINUX)
  //POSIX standard
  return strcasecmp(str1.c_str(), str2.c_str());
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
}
void OperatingSystem::showConsole() {
#if defined(BR2_OS_WINDOWS)
  ShowWindow(GetConsoleWindow(), SW_SHOW);
#elif defined(BR2_OS_LINUX)
  //No idea.
  BRLogWarn("Can't call 'show console' for linux.");
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
}
void OperatingSystem::hideConsole() {
#if defined(BR2_OS_WINDOWS)
  ShowWindow(GetConsoleWindow(), SW_HIDE);
#elif defined(BR2_OS_LINUX)
  //No Idea.
  BRLogWarn("Can't call 'hide console' for linux.");
//OS_METHOD_NOT_IMPLEMENTED
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
}

#if defined BR2_OS_LINUX
string_t zenityGetResult(FILE* result) {
  //Returns the result of a Zenity file selection dialog.
  string_t ret = "";
  if (result) {
    //TODO: This needs to be a much larger buffer if we support multiple files.
    char filename[BRO_MAX_PATH];
    std::memset((void*)filename, 0, BRO_MAX_PATH);
    fgets(filename, BRO_MAX_PATH, result);
    string_t unsplit(filename);
  }
  return ret;
}
#endif

string_t OperatingSystem::showOpenFolderDialog(const string_t& saved_path) {
  string_t res = "";
#if defined(BR2_OS_WINDOWS)
  WCHAR path[MAX_PATH];

  const char* path_param = saved_path.c_str();

  BROWSEINFO bi = {0};
  bi.lpszTitle = L"Browse for folder...";
  bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
  bi.lpfn = [](HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
    if (uMsg == BFFM_INITIALIZED) {
      std::string tmp = (const char*)lpData;
      std::cout << "path: " << tmp << std::endl;
      SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }
    return 0;
  };
  bi.lParam = (LPARAM)path_param;

  LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

  if (pidl != 0) {
    //get the name of the folder and put it in path
    SHGetPathFromIDList(pidl, path);

    //free memory used
    IMalloc* imalloc = 0;
    if (SUCCEEDED(SHGetMalloc(&imalloc))) {
      imalloc->Free(pidl);
      imalloc->Release();
    }

    res = StringUtil::wStrToStr(std::wstring(path));
  }
#elif defined(BR2_OS_LINUX)
  //Must test this.
  Gu::debugBreak();

  try {
    FILE* f = popen("zenity --file-selection --directory", "r");
    res = zenityGetResult(f);
  }
  catch (...) {
    BRLogError("Error during zenity folder selection.");
  }
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif

  return res;
}
std::vector<string_t> OperatingSystem::showOpenFileDialog(const string_t& title, const string_t& filter,
                                                          const string_t& defaultext, const string_t& basePath, bool multiple) {
  //Note Zenity supports returning multiple strings. If windows supports this we can easily allow this to return multiple.
  std::vector<string_t> ret;
#if defined(BR2_OS_WINDOWS)
  OPENFILENAMEW ofn = {0};
  WCHAR openFileNameReturnString[MAX_PATH];  // the filename will go here from the openfile dialog
  ZeroMemory(openFileNameReturnString, MAX_PATH);

  ofn.lStructSize = sizeof(OPENFILENAMEA);
  ofn.lpstrFileTitle = openFileNameReturnString;
  ofn.nMaxFileTitle = MAX_PATH;
  ofn.lpstrTitle = title.length() ? StringUtil::strToWStr(title).c_str() : 0;
  ofn.lpstrFilter = filter.length() ? StringUtil::strToWStr(filter).c_str() : 0;  //"COLLADA Files (*.DAE)\0*.DAE\0\0";
  ofn.lpstrInitialDir = basePath.length() ? StringUtil::strToWStr(basePath).c_str() : 0;
  ofn.lpstrDefExt = defaultext.length() ? StringUtil::strToWStr(defaultext).c_str() : 0;  //"DAE";
  ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

  if (!GetOpenFileNameW(&ofn)) {
    return "";
  }
  file = StringUtil::wStrToStr(openFileNameReturnString);
#elif defined(BR2_OS_LINUX)

  std::string args = "zenity --file-selection --separator=\"|\" ";
  //TODO: for multiple files add --multiple
  if (StringUtil::isNotEmpty(basePath)) {
    args += " --filename \"" + basePath + "\" ";
  }
  if (multiple) {
    args += " --multiple ";
  }
  string_t res = "";
  try {
    FILE* f = popen(args.c_str(), "r");

    res = zenityGetResult(f);
    ret = StringUtil::split(res, '|');
  }
  catch (...) {
    BRLogError("Error during zenity file selection.");
  }
  //TODO: multiple files.

#else
  OS_METHOD_NOT_IMPLEMENTED
#endif

  return ret;
}

string_t OperatingSystem::getEnvironmentVariable(const string_t& var) {
  //Get a Windows/Linux environment variable.
  string_t ret = "";
#if defined(BR2_OS_WINDOWS)
  OS_METHOD_NOT_IMPLEMENTED

#elif defined(BR2_OS_LINUX)
  const char* val = std::getenv(var.c_str());
  if (val == nullptr) {  // invalid to assign nullptr to std::string
    ret = "";
  }
  else {
    ret = string_t(val);
  }
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
  return ret;
}
string_t OperatingSystem::newline() {
  string_t ret = "";
#if defined(BR2_OS_WINDOWS)
  ret = "\r\n";
#elif defined(BR2_OS_LINUX)
  ret = "\n";
#else
  OS_METHOD_NOT_IMPLEMENTED
#endif
  return ret;
}

string_t OperatingSystem::executeReadOutput(const string_t& cmd) {
  //Execute the given command and return the output in a std::string.
  //https://www.jeremymorgan.com/tutorials/c-programming/how-to-capture-the-output-of-a-linux-command-in-c/
  string_t data = "";
  FILE* stream;
  const int max_buffer = 256;
  char buffer[max_buffer];
  string_t cmd_mod = cmd + " 2>&1";

  try {
    stream = popen(cmd_mod.c_str(), "r");
    if (stream) {
      while (!feof(stream))
        if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
      pclose(stream);
    }
    return data;
  }
  catch (...) {
    BRLogError("Failed to execute command: '" + cmd + "'");
  }
}

}  // namespace BR2

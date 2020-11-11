/**
*  @file BaseHeader.h
*  @date November 12, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __BASEHEADER_1478973884162863251_H__
#define __BASEHEADER_1478973884162863251_H__

#include "../base/BaseDefines.h"

namespace VG {

/************************************************************************/
/* String                                                               */
/************************************************************************/
//String
#define USE_STD_STRING
#ifdef USE_STD_STRING
typedef std::string string_t;
#else
#include "../base/StringWrapper.h"
#endif
typedef std::wstring wstring_t;

//Helper string since we can't cast char* to std::string automatically.
#define Stz std::string("") +
//String Extensions  See MathHeader for math string extensions.
std::string operator+(const std::string& str, const char& rhs);
std::string operator+(const std::string& str, const int8_t& rhs);
std::string operator+(const std::string& str, const int16_t& rhs);
std::string operator+(const std::string& str, const int32_t& rhs);
std::string operator+(const std::string& str, const int64_t& rhs);
std::string operator+(const std::string& str, const uint8_t& rhs);
std::string operator+(const std::string& str, const uint16_t& rhs);
std::string operator+(const std::string& str, const uint32_t& rhs);
std::string operator+(const std::string& str, const uint64_t& rhs);
std::string operator+(const std::string& str, const double& rhs);
std::string operator+(const std::string& str, const float& rhs);

/************************************************************************/
/* Exceptions & Errors                                                  */
/************************************************************************/
void staticDebugBreak(const string_t& str);
void runtimeAssertion(const string_t& str);
#define TIME_T_MIN (0)
#ifdef _DEBUG
#define AssertOrThrow2(x)                                                \
  do {                                                                   \
    if (!(x)) {                                                          \
      VG::staticDebugBreak(Stz "Debug Time Assertion: '" + #x + "'. "); \
    }                                                                    \
  } while (0)
#else
#define AssertOrThrow2(x)                                             \
  do {                                                                \
    if (!(x)) {                                                       \
      VG::runtimeAssertion(Stz "Runtime Assertion: '" + #x + "'. "); \
    }                                                                 \
  } while (0)
#endif

//GCC NOTE: GCC says allowing the use of an undeclared name is deprecated which would make us have to move TStr up
//above BaseHeader.
//if you use '-fpermissive', G++ will accept your code, but allowing the use of an undeclared name is deprecated
//https://stackoverflow.com/questions/10948316/throw-new-stdexception-vs-throw-stdexception
#define BRTestFL(x) (Stz x + BR_SRC_FLINE + BR_SRC_FNAME)
#define BRThrowException(x) throw VG::Exception(Stz x, BR_SRC_FLINE, BR_SRC_FNAME, true)
#define BRThrowNotImplementedException() BRThrowException("The method is not implemented.")
#define BRThrowNotImplementedExceptionMsg(sz) \
  do {                                        \
    BRLogError(sz);                           \
    BRThrowNotImplementedException();         \
  } while (0);
#define BRThrowDeprecatedException() BRThrowException("The method has been deprecated and removed.  Please see the method notes to use the correct replacement.")
#define VerifyOrThrow(expr, x)        \
  do {                                \
    if (!(expr)) BRThrowException(x); \
  } while (0)
#define CheckGpuErrorsDbg() VG::Base::checkErrors()
#define ShowMessageBoxOnce(msg)           \
  {                                       \
    static bool __show = false;           \
    if (__show == false) {                \
      do {                                \
        Base::showMessageBox(msg, "Error"); \
      } while (0);                        \
    };                                    \
    __show = true;                        \
  }
//Logassert is a shorthand for logging an assert, but letting the code continue.
#define LogAssert(x)                                             \
  do {                                                           \
    if (!(x)) {                                                  \
      BRLogError("Log assertion failed '" + string_t(#x) + "'"); \
      Base::debugBreak();                                          \
    }                                                            \
  } while (0)

/************************************************************************/
/* Casting                                                              */
/************************************************************************/

template <typename Tx, typename Ty>
inline Tx brSafeCast(Ty pb) {
  //Note:
  // If this is giving compile errors make sure you
  // included both the .h definition files of the casted
  // object types.
  Tx ret = dynamic_cast<Tx>(pb);
  if (pb != nullptr && ret == nullptr) {
    throw 0;
  }
  //    AssertOrThrow2(ret!=NULL);
  return ret;
}
#define SafeCast(pObj, pType) BRSafeCast<pType>(pObj)
#define UnsafeCast(pObj, pType) reinterpret_cast<pType>(pObj)

/************************************************************************/
/* FW Class                                                             */
/************************************************************************/

/**
*  @class GLFramework
*  @brief Inherit to be part of the OpenGL rendering system.
*/
// class GLFramework : public VirtualMemoryShared<GLFramework> {
// public:
//   GLFramework(std::shared_ptr<GLContext> ct) {
//     _pContext = ct;
//   }
//   virtual ~GLFramework() override { _pContext = nullptr; }
//   std::shared_ptr<GLContext> getContext() { return _pContext; }
// 
// protected:
//   void setContext(std::shared_ptr<GLContext> ct) { _pContext = ct; }
// 
// private:
//   std::shared_ptr<GLContext> _pContext = nullptr;
// };

template <class Tx>
class ISerializable : public VirtualMemoryShared<Tx> {
public:
  ISerializable() {}
  virtual ~ISerializable() override {}
  virtual void deserialize(std::shared_ptr<BinaryFile> fb) = 0;
  virtual void serialize(std::shared_ptr<BinaryFile> fb) = 0;
};
class SoundPlayInfo : public VirtualMemory {
public:
  bool _bLoop = false;
  float _fLoopPos = 0.0f;  // [0,1]
  float _fVolume = 1.0f;   // [0,1]
  SoundPlayInfo() {}
  SoundPlayInfo(bool loop, float fLoopPos, float fVolume) {
    _bLoop = loop;\
    _fLoopPos = fLoopPos;
    _fVolume = fVolume;
  }
  virtual ~SoundPlayInfo() override {}
};
class App : public VirtualMemoryShared<App> {
public:
    App(){}
    virtual ~App() override {}
    virtual void checkErrors(bool, bool) = 0;
    virtual std::shared_ptr<ApplicationPackage> package() = 0;
    virtual void freeFile(char*& pOutData) = 0;
    virtual int readFile(const string_t& fname, char*& pOutData, int64_t& _iOutSizeBytes, bool addNull = false) = 0;
    virtual int writeFile(const string_t& fname, char* pData, size_t _iDataSizeBytes) = 0;

};


}  // namespace VG

/************************************************************************/
/* SDL Defines                                                          */
/************************************************************************/
// struct SDL_Window;
// union SDL_Event;

#endif

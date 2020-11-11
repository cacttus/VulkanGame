#include "../base/BaseHeader.h"
#include "../core/opengl/OglErr.h"
#include "../base/Logger.h"
#include "../core/opengl/GLContext.h"
#include "../base/DebugHelper.h"
#include "../base/EngineConfig.h"
#include "../core/SDLUtils.h"
#include "../base/OperatingSystem.h"
#include "../gfx/RenderUtils.h"
#include "../core/opengl/OpenGLUtils.h"
#include <atomic>

namespace VG {
class OglErr_Internal {
public:
  enum class GpuLogLevel { Err_,
                           Wrn_,
                           Inf_,
                           Dbg_ };
  static string_t glErrToStr(GLenum err) {
    switch (err) {
      case 0:
        return "GL_NO_ERROR         ";
      case 0x0500:
        return "GL_INVALID_ENUM     ";
      case 0x0501:
        return "GL_INVALID_VALUE    ";
      case 0x0502:
        return "GL_INVALID_OPERATION";
      case 0x0503:
        return "GL_STACK_OVERFLOW   ";
      case 0x0504:
        return "GL_STACK_UNDERFLOW  ";
      case 0x0505:
        return "GL_OUT_OF_MEMORY    ";
    }
    return " *GL Error code not recognized.";
  }
  bool _bPrintingGPULog = false;
  GLint _maxMsgLen = -1;

  bool handleErrors(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak, bool doNotLog, const string_t& shaderName, bool clearOnly) {
    SDLUtils::Base::checkErrors()(doNotLog || !clearOnly, clearOnly);

    printAndFlushGpuLog(ctx, true, bDoNotBreak, doNotLog, shaderName, clearOnly);

    return checkOglErr(ctx, bShowNote, bDoNotBreak || clearOnly, doNotLog || !clearOnly, shaderName);
  }
  bool checkOglErr(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak, bool doNotLog, const string_t& shaderName) {
    bool bError = false;

    //GPU Log -
    // This isn't the Application log it's the hardware log on the card.
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      if (doNotLog == false) {
        string_t errmsg = "GL Error: " + glErrToStr(err) + " (" + (int)err + ")";
        if (StringUtil::isNotEmpty(shaderName)) {
          errmsg += OperatingSystem::newline() + " -> shader: " + shaderName;
        }
        errmsg += ctx->debugGetRenderState();
        BRLogError(errmsg);
      }

      if (Base::config()->getBreakOnGraphicsError() == true) {
        if (bDoNotBreak == false) {
          Base::debugBreak();
        }
      }
      bError = true;
    }

    return bError;
  }
  void clearGPULog(std::shared_ptr<GLContext> ctx){
    printAndFlushGpuLog(ctx, true, true, true, "", true);
  }
  void printAndFlushGpuLog(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak, bool doNotLog,
                           const string_t& shaderName, bool clearOnly) {
    if (_bPrintingGPULog) {
      //Prevent recursion.
      return;
    }
    _bPrintingGPULog = true;
    {
      printAndFlushGpuLog_Notrap(ctx, bShowNote, bDoNotBreak, doNotLog, shaderName, clearOnly);
    }
    _bPrintingGPULog = false;
  }
  void printAndFlushGpuLog_Notrap(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak, bool doNotLog,
                                  const string_t& shaderName, bool clearOnly) {
    //Enable this in engine.cpp glEnable(GL_DEBUG_OUTPUT);
    if (ctx == nullptr) {
      BRLogWarn("Context not initialized (context isseu");
      return;
    }
    if (!ctx->glGetDebugMessageLog) {
      BRLogWarn("Opengl log not initialized (context isseu");
      return;
    }

    GLuint numMsgs = 64;
    GLuint numFound;

    if (_maxMsgLen == -1) {
      glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &_maxMsgLen);
    }
    if (_maxMsgLen <= 0) {
      BRLogError("GL_MAX_DEBUG_MESSAGE_LENGTH returned 0.");
      _maxMsgLen = -2;
      return;
    }

    bool graphicsLogHigh = Base::config()->getGraphicsErrorLogging_High();
    bool graphicsLogMed = Base::config()->getGraphicsErrorLogging_Medium();
    bool graphicsLogLow = Base::config()->getGraphicsErrorLogging_Low();
    bool graphicsLogInfo = Base::config()->getGraphicsErrorLogging_Info();

    do {
      std::vector<GLchar> msgData(numMsgs * _maxMsgLen);
      std::vector<GLenum> sources(numMsgs);
      std::vector<GLenum> types(numMsgs);
      std::vector<GLenum> severities(numMsgs);
      std::vector<GLuint> ids(numMsgs);
      std::vector<GLsizei> lengths(numMsgs);

      numFound = ctx->glGetDebugMessageLog(numMsgs, (GLsizei)msgData.size(), &sources[0], &types[0],
                                           &ids[0], &severities[0], &lengths[0], &msgData[0]);
      if (numFound == 0) {
        return;
      }
      if (clearOnly) {
        continue;  //clear messages.
      }

      sources.resize(numFound);
      types.resize(numFound);
      severities.resize(numFound);
      ids.resize(numFound);
      lengths.resize(numFound);

      std::vector<string_t> messages;
      messages.reserve(numFound);

      std::vector<GLchar>::iterator currPos = msgData.begin();
      for (size_t iMsg = 0; iMsg < lengths.size(); ++iMsg) {
        int id = ids[iMsg];
        if (!skipNVIDIA(id) && !skipATI(id)) {
          string_t strMsg = std::string(currPos, currPos + lengths[iMsg] - 1);
          GLenum severity = severities[iMsg];
          GLenum type = types[iMsg];
          GLenum source = sources[iMsg];
          logGPUMessageText(strMsg, id, shaderName, doNotLog, severity, type, source, graphicsLogHigh, graphicsLogMed, graphicsLogLow, graphicsLogInfo, ctx);
        }
        currPos = currPos + lengths[iMsg];
      }
    } while (numFound > 0);
  }
  void logGPUMessageText(const string_t& cstrMsg, int msgId, const string_t& shaderName, bool doNotLog, GLenum severity,
                         GLenum type, GLenum source, bool graphicsLogHigh, bool graphicsLogMed, bool graphicsLogLow, 
                         bool graphicsLogInfo, std::shared_ptr<GLContext> ctx) {
    string_t msg = "";
    string_t shaderMsg = "";

    if (StringUtil::isNotEmpty(shaderName)) {
      shaderMsg = " -> shader: " + shaderName;
    }
    if (doNotLog == false) {
      string_t strId = Stz "[id=" + StringUtil::toHex(msgId, true) + "]";

      //Skip if the config.xml has turned off this kind of logging.
      if (severity == GL_DEBUG_SEVERITY_HIGH && graphicsLogHigh == false) {
        return;
      }
      else if (severity == GL_DEBUG_SEVERITY_MEDIUM && graphicsLogMed == false) {
        return;
      }
      else if (severity == GL_DEBUG_SEVERITY_LOW && graphicsLogLow == false) {
        return;
      }
      else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION && graphicsLogInfo == false) {
        return;
      }

      string_t strSev = "";
      string_t strType = "";
      string_t strSource = "";
      GpuLogLevel level = GpuLogLevel::Dbg_;
      getTypeSevSourceLevel(type, severity, source, strType, strSev, strSource, level);

      //Prevent infinite recursion to dump the rendering state.
      string_t strStackInfo = "";
      string_t strRenderState = "";
      static bool _bPrintingGPULog = false;
      if (_bPrintingGPULog == false) {
        _bPrintingGPULog = true;
        //This isn't necessary. We can just add it above. what's happening is calling renderstate() resets the glError.
        // Also the GL Error automatically resets.
        strRenderState = (type == GL_DEBUG_SEVERITY_NOTIFICATION) ? "" : ctx->debugGetRenderState(true, false, false);
        strStackInfo = "";//(type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_SEVERITY_NOTIFICATION) ? "" : DebugHelper::getStackTrace();  //error prints stack.
        _bPrintingGPULog = false;
      }
      else {
        strRenderState = " RenderState: Gpu Log is currently in recursive call, no information can be displayed.";
        strStackInfo = " Stack: Gpu Log is currently in recursive call, no information can be displayed.";
      }

      msg = Stz "GPU_LOG_MSG" + strId + strType + strSev + strSource + OperatingSystem::newline() +
            shaderMsg + OperatingSystem::newline() +
            " MSG ID: " + strId + OperatingSystem::newline() +
            " Msg: " + cstrMsg + OperatingSystem::newline() +
            " Render: " + OperatingSystem::newline() + strStackInfo + OperatingSystem::newline() +
            strRenderState;

      if (type == GL_DEBUG_TYPE_ERROR) {
        BRLogError(msg);
      }
      else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        BRLogInfo(msg);
      }
      else {
        BRLogWarn(msg);
      }
    }
  }
  FORCE_INLINE bool skipNVIDIA(int id) {
    //NVidia - redundant messages / infos
    return id == 0x00020071     // GL_DYANMIC_DRAW or GL_STATIC_DRAW memory usgae
           || id == 0x00020084  // Texture state usage warning: Texture 0 is base level inconsistent. Check texture size.
                                // else if (id == 0x00020061) {
                                //   return true;
                                // }  // Framebuffer detailed info: The driver allocated storage for renderbuffer 1.
                                // else if (id == 0x00020004) {
                                //   return true;
                                // }  // Usage warning: Generic vertex attribute array ... uses a pointer with a small value (...). Is this intended to be used as an offset into a buffer object?
                                // else if (id == 0x00020072) {
                                //   return true;
                                // }  // Buffer performance warning: Buffer object ... (bound to ..., usage hint is GL_STATIC_DRAW) is being copied/moved from VIDEO memory to HOST memory.
                                // else if (id == 0x00020074) {
                                //   return true;
                                // }  // Buffer usage warning: Analysis of buffer object ... (bound to ...) usage indicates that the GPU is the primary producer and consumer of data for this buffer object.  The usage hint s upplied with this buffer object, GL_STATIC_DRAW, is inconsistent with this usage pattern.  Try using GL_STREAM_COPY_ARB, GL_STATIC_COPY_ARB, or GL_DYNAMIC_COPY_ARB instead.
                                // else if (id == 0x00020070) {
                                //   return true;
                                // }  // Total VBO Usage in the system... (Useful information)
                                // else if (id == 0x00020043) {
                                //   return true;
                                // }  // A non-Fullscreen clear caused a fallback from CSAA to MSAA; - probolem in clearing cube shadow buffers
                                //Other (mom's house) driver
                                // else if (id == 0x07) {
                                //   return true;
                                // }  // glLineWidth Deprecated (other driver)

        ;

    return false;
  }
  static bool skipATI(int id) {
    return false;
  }
  static void getTypeSevSourceLevel(GLenum type, GLenum severity, GLenum source, string_t& strType, string_t& strSev, string_t& strSource, GpuLogLevel& level) {
    if (type == GL_DEBUG_TYPE_ERROR) {
      strType = "[type=ERROR]";
    }
    else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) {
      strType = "[type=DEPRECATED_BEHAVIOR]";
    }
    else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) {
      strType = "[type=UNDEFINED_BEHAVIOR]";
    }
    else if (type == GL_DEBUG_TYPE_PORTABILITY) {
      strType = "[type=PORTABILITY]";
    }
    else if (type == GL_DEBUG_TYPE_PERFORMANCE) {
      strType = "[type=PERFORMANCE]";
    }
    else if (type == GL_DEBUG_TYPE_OTHER) {
      strType = "[type=OTHER]";
    }
    else {
      strType = "[type=(undefined(" + TypeConv::intToStr(type) + "))]";
    }

    if (severity == GL_DEBUG_SEVERITY_HIGH) {
      strSev = "[severity=HIGH]";
      level = GpuLogLevel::Err_;
    }
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
      strSev = "[severity=MEDIUM]";
      level = GpuLogLevel::Wrn_;
    }
    else if (severity == GL_DEBUG_SEVERITY_LOW) {
      strSev = "[severity=LOW]";
      level = GpuLogLevel::Inf_;
    }
    else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
      strSev = "[severity=NOTIFICATION]";
      level = GpuLogLevel::Inf_;
    }
    else {
      strSev = "[severity=(undefined(" + TypeConv::intToStr(severity) + ")))]";
      level = GpuLogLevel::Inf_;
    }

    if (source == GL_DEBUG_SOURCE_API) {
      strSource = "[source=API]";
    }
    else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM) {
      strSource = "[source=WINDOW_SYSTEM]";
    }
    else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER) {
      strSource = "[source=SHADER_COMPILER]";
    }
    else if (source == GL_DEBUG_SOURCE_THIRD_PARTY) {
      strSource = "[source=THIRD_PARTY]";
    }
    else if (source == GL_DEBUG_SOURCE_APPLICATION) {
      strSource = "[source=APPLICATION]";
    }
    else if (source == GL_DEBUG_SOURCE_OTHER) {
      strSource = "[source=OTHER]";
    }
  }
};

///
///
///
///
void OglErr::clearGPULog(std::shared_ptr<GLContext> ctx){
  _pint->clearGPULog(ctx);
}
bool OglErr::chkErrRt(std::shared_ptr<GLContext> ctx, bool bDoNotBreak, bool doNotLog, const string_t& shaderName, bool clearOnly) {
  if (Base::config()->getEnableRuntimeErrorChecking() == true) {
    return _pint->handleErrors(ctx, true, bDoNotBreak, doNotLog, shaderName, clearOnly);
  }
  return false;
}
bool OglErr::chkErrDbg(std::shared_ptr<GLContext> ctx, bool bDoNotBreak, bool doNotLog, const string_t& shaderName, bool clearOnly) {
  if (Base::config()->getEnableDebugErrorChecking() == true) {
    return _pint->handleErrors(ctx, true, bDoNotBreak, doNotLog, shaderName, clearOnly);
  }
  return false;
}
OglErr::OglErr() {
  _pint = std::make_unique<OglErr_Internal>();
}
OglErr::~OglErr() {
  _pint = nullptr;
}

}  // namespace VG

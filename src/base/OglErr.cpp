#include "../base/BaseHeader.h"
#include "../base/OglErr.h"
#include "../base/Logger.h"
#include "../base/GLContext.h"
#include "../base/DebugHelper.h"
#include "../base/EngineConfig.h"
#include "../base/SDLUtils.h"
#include "../base/OperatingSystem.h"

namespace BR2 {
class OglErr_Internal {
public:
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
  static bool handleErrors(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak, bool doNotLog, const string_t& shaderName) {
    SDLUtils::checkSDLErr(doNotLog);

    printAndFlushGpuLog(ctx, true, bDoNotBreak, doNotLog, shaderName);

    return checkOglErr(ctx, bShowNote, bDoNotBreak, doNotLog, shaderName);
  }
  static bool checkOglErr(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak, bool doNotLog, const string_t& shaderName) {
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
        BRLogError(errmsg);
      }

      if (Gu::getEngineConfig()->getBreakOnGraphicsError() == true) {
        if (bDoNotBreak == false) {
          Gu::debugBreak();
        }
      }
      bError = true;
    }

    return bError;
  }
  static void printAndFlushGpuLog(std::shared_ptr<GLContext> ctx, bool bShowNote, bool bDoNotBreak, bool doNotLog, const string_t& shaderName) {
    //Enable this in engine.cpp
    //   glEnable(GL_DEBUG_OUTPUT);
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

    static GLint maxMsgLen = -1;
    if (maxMsgLen == -1) {
      //avoid doing this every call.
      glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxMsgLen);
    }

    if (maxMsgLen <= 0) {
      BRLogError("GL_MAX_DEBUG_MESSAGE_LENGTH returned 0.");
      maxMsgLen = -2;
      return;
    }

    bool graphicsLogHigh = Gu::getEngineConfig()->getGraphicsErrorLogging_High();
    bool graphicsLogMed = Gu::getEngineConfig()->getGraphicsErrorLogging_Medium();
    bool graphicsLogLow = Gu::getEngineConfig()->getGraphicsErrorLogging_Low();
    bool graphicsLogInfo = Gu::getEngineConfig()->getGraphicsErrorLogging_Info();

    do {
      std::vector<GLchar> msgData(numMsgs * maxMsgLen);
      std::vector<GLenum> sources(numMsgs);
      std::vector<GLenum> types(numMsgs);
      std::vector<GLenum> severities(numMsgs);
      std::vector<GLuint> ids(numMsgs);
      std::vector<GLsizei> lengths(numMsgs);

      numFound = ctx->glGetDebugMessageLog(
          numMsgs, (GLsizei)msgData.size(), &sources[0], &types[0], &ids[0], &severities[0], &lengths[0], &msgData[0]);
      if (numFound == 0) {
        return;
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
        //GTX670 Driver
        //NVidia - invalid messages / infos
        if (id == 0x00020071) {
          return;
        }  // memory usage
        else if (id == 0x00020084) {
          return;
        }  // Texture state usage warning: Texture 0 is base level inconsistent. Check texture size.
        else if (id == 0x00020061) {
          return;
        }  // Framebuffer detailed info: The driver allocated storage for renderbuffer 1.
        else if (id == 0x00020004) {
          return;
        }  // Usage warning: Generic vertex attribute array ... uses a pointer with a small value (...). Is this intended to be used as an offset into a buffer object?
        else if (id == 0x00020072) {
          return;
        }  // Buffer performance warning: Buffer object ... (bound to ..., usage hint is GL_STATIC_DRAW) is being copied/moved from VIDEO memory to HOST memory.
        else if (id == 0x00020074) {
          return;
        }  // Buffer usage warning: Analysis of buffer object ... (bound to ...) usage indicates that the GPU is the primary producer and consumer of data for this buffer object.  The usage hint s upplied with this buffer object, GL_STATIC_DRAW, is inconsistent with this usage pattern.  Try using GL_STREAM_COPY_ARB, GL_STATIC_COPY_ARB, or GL_DYNAMIC_COPY_ARB instead.
        else if (id == 0x00020070) {
          return;
        }  // Total VBO Usage in the system... (Useful information)
        else if (id == 0x00020043) {
          return;
        }  // A non-Fullscreen clear caused a fallback from CSAA to MSAA; - probolem in clearing cube shadow buffers
        //Other (mom's house) driver
        else if (id == 0x07) {
          return;
        }  // glLineWidth Deprecated (other driver)

        //We don't need these.
        //string_t strSrc = glDebugGetErrorSource(sources[iMsg]);
        //string_t strType = glDebugGetMessageType(types[iMsg]);
        //string_t strSev = glDebugGetMessageSeverity(severities[iMsg]);

        string_t strMsg = std::string(currPos, currPos + lengths[iMsg] - 1);
        string_t shaderMsg = "";

        if (StringUtil::isNotEmpty(shaderName)) {
          shaderMsg = " -> shader: " + shaderName;
        }
        if (doNotLog == false) {
          GLenum severity = severities[iMsg];
          GLenum type = types[iMsg];
          string_t strType = "";
          string_t strId = Stz "[" + StringUtil::toHex(id, true) + "]";
          string_t strSev = "";

          //Skip if the config.xml has turned off this kind of logging.
          if (severity == GL_DEBUG_SEVERITY_HIGH && graphicsLogHigh == false) {
            continue;
          }
          else if (severity == GL_DEBUG_SEVERITY_MEDIUM && graphicsLogMed == false) {
            continue;
          }
          else if (severity == GL_DEBUG_SEVERITY_LOW && graphicsLogLow == false) {
            continue;
          }
          else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION && graphicsLogInfo == false) {
            continue;
          }



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

          enum class GpuLogLevel { Err_,
                                   Wrn_,
                                   Inf_,
                                   Dbg_ };
          GpuLogLevel level = GpuLogLevel::Dbg_;
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

          string_t strStackInfo = (type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_SEVERITY_NOTIFICATION) ? "" : DebugHelper::getStackTrace();  //error prints stack.

          strMsg = "GPU_LOG_MSG" + strId + strType + strSev + OperatingSystem::newline() +
                   shaderMsg + OperatingSystem::newline() +
                   " MSG ID: " + strId + OperatingSystem::newline() +
                   " Msg: " + strMsg + OperatingSystem::newline() +
                   strStackInfo;
          if (type == GL_DEBUG_TYPE_ERROR) {
            BRLogError(strMsg);
          }
          else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
            BRLogInfo(strMsg);
          }
          else {
            BRLogWarn(strMsg);
          }
        }

        currPos = currPos + lengths[iMsg];
      }
    } while (numFound > 0);
  }
  static string_t glDebugGetErrorSource(int eCode) {
    switch (eCode) {
      case GL_DEBUG_SOURCE_API:
        return " SOURCE API";
        break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return (" WINDOW SYSTEM");
        break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return (" SHADER COMPILER");
        break;
      case GL_DEBUG_SOURCE_THIRD_PARTY:
        return (" THIRD PARTY");
        break;
      case GL_DEBUG_SOURCE_APPLICATION:
        return (" APPLICATION");
        break;
      case GL_DEBUG_SOURCE_OTHER:
        return (" OTHER");
        break;
    }
    return ("*No Enum*");
  }
  static string_t glDebugGetMessageType(int eCode) {
    switch (eCode) {
      case GL_DEBUG_TYPE_ERROR:
        return (" ERROR");
        break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return (" DEPRECATED");
        break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return (" UNDEFINED BEHAVIOR");
        break;
      case GL_DEBUG_TYPE_PORTABILITY:
        return (" PORTABILITY");
        break;
      case GL_DEBUG_TYPE_PERFORMANCE:
        return (" PERFORMANCE");
        break;
      case GL_DEBUG_TYPE_MARKER:
        return (" MARKER");
        break;
      case GL_DEBUG_TYPE_PUSH_GROUP:
        return (" PUSH GRP");
        break;
      case GL_DEBUG_TYPE_POP_GROUP:
        return (" POP GRP");
        break;
      case GL_DEBUG_TYPE_OTHER:
        return (" OTHER");
        break;
    }
    return ("*No Enum*");
  }
  static string_t glDebugGetMessageSeverity(int eCode) {
    switch (eCode) {
      case GL_DEBUG_SEVERITY_HIGH:
        return (" HIGH");
        break;
      case GL_DEBUG_SEVERITY_MEDIUM:
        return (" MEDIUM");
        break;
      case GL_DEBUG_SEVERITY_LOW:
        return (" LOW");
        break;
      case GL_DEBUG_SEVERITY_NOTIFICATION:
        return (" NOTE");
        break;
    }
    return ("*No Enum*");
  }
};

bool OglErr::chkErrRt(std::shared_ptr<GLContext> ctx, bool bDoNotBreak, bool doNotLog, const string_t& shaderName) {
  if (Gu::getEngineConfig()->getEnableRuntimeErrorChecking() == true) {
    return OglErr_Internal::handleErrors(ctx, true, bDoNotBreak, doNotLog, shaderName);
  }
  return false;
}
bool OglErr::chkErrDbg(std::shared_ptr<GLContext> ctx, bool bDoNotBreak, bool doNotLog, const string_t& shaderName) {
  if (Gu::getEngineConfig()->getEnableDebugErrorChecking() == true) {
    return OglErr_Internal::handleErrors(ctx, true, bDoNotBreak, doNotLog, shaderName);
  }
  return false;
}

}  // namespace BR2

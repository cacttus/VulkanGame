#include "../base/TextConfigFile.h"
#include "../base/Logger.h"
#include "../base/Exception.h"
#include "../base/OperatingSystem.h"
#include "../base/StringUtil.h"

namespace VG {
TextConfigFile::TextConfigFile(const string_t& loc) : TextDataFile(loc) {
}
TextConfigFile::~TextConfigFile() {
}
string_t TextConfigFile::getCleanToken(std::vector<string_t>& tokens, int& iind) {
  //**NOTE be care ful with this function when calling in constructors  C++ constructors don't guarantee parameter order, thus
  //you may end up with invalid parameters.
  string_t ret = getCleanTokenInd(tokens, iind);
  iind++;
  return ret;
}
string_t TextConfigFile::getCleanTokenInd(std::vector<string_t>& tokens, int iind) {
  string_t ret;
  if ((size_t)iind >= tokens.size()) {
    parseErr(Stz "Token index '" + iind + "' out of range.", true, true);  //throw
  }
  ret = StringUtil::stripDoubleQuotes(tokens[iind]);
  ret = StringUtil::trim(ret);

  return ret;
}
void TextConfigFile::parseErr(const string_t& str, bool bDebugBreak, bool bFatal) {
  string_t strhead = Stz "Error: '" + _fileLoc + "': line " + _iCurrentParseLine + OperatingSystem::newline() + "  ";
  string_t str2 = strhead + str;
  //Throw this if you wnt to have an error in your file.
  if (bFatal) {
    BRThrowException(str2);
  }
  else {
    BRLogError(str2);
    if (bDebugBreak) {
      Base::debugBreak();
    }
  }
}
bool TextConfigFile::validateArguments(const string_t& arg, int32_t count) {
  if (_vecTokens.size() != count) {
    displayError("While parsing '" + _fileLoc + "' invalid number of arguments for '" + arg + "'. Got " + _vecTokens.size() + " expected " + count + ".");
  }
  return true;
}

void TextConfigFile::displayError(const string_t& errMsg, bool bThrow) {
  string_t strErr = Stz "Data File Parse Error:" + OperatingSystem::newline() + "  " + errMsg + OperatingSystem::newline() + "   File: " + _fileLoc + OperatingSystem::newline() + "   Line:" + _iCurrentParseLine + OperatingSystem::newline();
  if (bThrow) {
    BRThrowException(strErr);
  }
}
bool TextConfigFile::lcmp(const string_t& tok0, const std::string& ch, int32_t validateArgCount) {
  return lcmp(tok0, ch.c_str(), validateArgCount);
}
bool TextConfigFile::lcmp(const string_t& tok0, const char* ch, int32_t validateArgCount) {
  bool ret;

  ret = StringUtil::equalsi(tok0, string_t(ch));  //(StringUtil::lowercase(tok0) == StringUtil::lowercase(t_string(ch)));

  if (validateArgCount >= 0) {
    if (ret == true) {
      validateArguments(tok0, validateArgCount);
    }
  }

  return ret;
}
bool TextConfigFile::cmp(const string_t& tok0, const char* ch, int32_t validateArgCount) {
  bool ret = tok0 == string_t(ch);

  if (validateArgCount >= 0) {
    if (ret == true) {
      validateArguments(tok0, validateArgCount);
    }
  }

  return ret;
}

}  // namespace VG

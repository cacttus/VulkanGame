//#ifdef _DEBUG
#include "../base/TypeConv.h"
#include "../base/StringUtil.h"

#include "../base/Logger.h"
//#endif
namespace VG {
//Explicit versions
bool TypeConv::strToBool(const string_t& s1, bool bThrowIfInvalid) {
  string_t s = StringUtil::lowercase(s1);

  if (StringUtil::equals(s, "1")) {
    return true;
  }
  else if (StringUtil::equals(s, "0")) {
    return false;
  }
  else if (StringUtil::equals(s, "true")) {
    return true;
  }
  else if (StringUtil::equals(s, "false")) {
    return false;
  }
  else if (StringUtil::equals(s, "t")) {
    return true;
  }
  else if (StringUtil::equals(s, "f")) {
    return false;
  }
  else if (StringUtil::equals(s, "y")) {
    return true;
  }
  else if (StringUtil::equals(s, "n")) {
    return false;
  }
  else if (StringUtil::equals(s, "yes")) {
    return true;
  }
  else if (StringUtil::equals(s, "no")) {
    return false;
  }
  if (bThrowIfInvalid) {
    BRThrowException("Invalid boolean cast, tried to cast " + s1 + " to boolean.");
  }

  return false;
}
long TypeConv::strToLong(const string_t& s) {
  long ret;
  try {
    ret = std::stol(s);
  } catch (const std::invalid_argument& ia) {
    BRLogError("strToLong: Invalid parameter" + s + ". ex: " + ia.what());

    throw ia;
  }
  return ret;
}
bool TypeConv::strToInt(const string_t& s, int32_t& out) {
  try {
    out = std::stoi(s);
  } catch (const std::invalid_argument& ia) {
    BRLogError("strToInt: Invalid parameter" + s + ". ex: " + ia.what());
    return false;
  }
  return true;
}
int_fast32_t TypeConv::strToInt(const string_t& s) {
  int_fast32_t ret;
  try {
    ret = std::stoi(s);
  } catch (const std::invalid_argument& ia) {
    BRLogError("strToInt: Invalid argument '" + s + "'. ex: " + ia.what());

    throw ia;
  }
  return ret;
}
float TypeConv::strToFloat(const string_t& s) {
  float ret;
  try {
    ret = std::stof(s);
  } catch (const std::invalid_argument& ia) {
    BRLogError("strToFloat: Invalid argument '" + s + "'. ex: " + ia.what());

    throw ia;
  }
  return ret;
}
uint_fast32_t TypeConv::strToUint(const string_t& s) {
  uint_fast32_t ret;
  try {
    ret = (uint_fast32_t)std::stoi(s);
  } catch (const std::invalid_argument& ia) {
    BRLogError("strToUint: Invalid argument " + s + "'. ex: " + ia.what());

    throw ia;
  }
  return ret;
}
double TypeConv::strToDouble(const string_t& s) {
  double ret;
  try {
    ret = std::stod(s);
  } catch (const std::invalid_argument& ia) {
    BRLogError("strToDouble: Invalid argument '" + s + "'.");

    throw ia;
    return 0;
  }
  return ret;
}

// int8_t TypeConv::strToByte(const string_t& s) {
//   int32_t i = strToInt(s);
//   return (t_byte)i;
// }

//int32_t strToInt( t_string& s, bool bFailed ){
//    char* endPtr;
//    int32_t ret = (int32_t)strtol(s.c_str(), &endPtr, 10);
//    bFailed = (bool)(*endPtr!='\0');
//    return ret;
//}

//////////////////////////////////////////////////////////////////////////
//Silent Fail Versions
float TypeConv::strToFloat(const string_t& s, bool bFailed) {
  char* pend;
  float ret = (float)strtod(s.c_str(), &pend);
  bFailed = (bool)(*pend != '\0');
  return ret;
}
double TypeConv::strToDouble(const string_t& s, bool bFailed) {
  char* pend;
  double ret = strtod(s.c_str(), &pend);
  bFailed = (bool)(*pend != '\0');
  return ret;
}

//////////////////////////////////////////////////////////////////////////
//ToStr
// string_t TypeConv::int32ToStr(const int_fast32_t i) {
//   string_t ret = string_t(std::to_string(i));
//   return ret;
// }
// string_t TypeConv::uint32ToStr(const uint_fast32_t i) {
//   string_t ret = string_t(std::to_string(i));
//   return ret;
// }
string_t TypeConv::intToStr(int_fast64_t i) {
  string_t ret = string_t(std::to_string(i));
  return ret;
}
string_t TypeConv::uintToStr(uint_fast64_t i) {
  string_t ret = string_t(std::to_string(i));
  return ret;
}
string_t TypeConv::TypeConv::floatToStr(const float i) {
  string_t ret = string_t(std::to_string(i));
  return ret;
}
string_t TypeConv::dblToStr(const double i) {
  string_t ret = string_t(std::to_string(i));
  return ret;
}
string_t TypeConv::sizetToStr(const size_t i) {
  string_t ret = string_t(std::to_string(i));
  return ret;
}
// string_t TypeConv::longToStr(const long& i) {
//   string_t ret = string_t(std::to_string(i));
//   return ret;
// }
// string_t TypeConv::ulongToStr(const unsigned long& i) {
//   string_t ret = string_t(std::to_string(i));
//   return ret;
// }
// string_t TypeConv::int64ToStr(const int_fast64_t& i) {
//   string_t ret = string_t(std::to_string(i));
//   return ret;
// }
// string_t TypeConv::uint64ToStr(const uint_fast64_t& i) {
//   string_t ret = string_t(std::to_string(i));
//   return ret;
// }

string_t TypeConv::wstrToStr(wchar_t* wstr) {
  string_t ret;

  char str[BRO_MAX_PATH];
  wcstombs(str, wstr, BRO_MAX_PATH);
  ret.assign(str);

  return ret;
}

//////////////////////////////////////////////////////////////////////////
//Format Versions
//t_string TypeConv::uintToStr(const  uint32_t i) {
//    uint32_t cpy = i;
//    int8_t buf[32];
//    //_itoa_s( cpy, (char*)buf, 32, 10 );
//    snprintf((char*)buf, 32, "%u", i);
//    return t_string((char*)buf);
//}

string_t TypeConv::intToStr(const int_fast64_t i, const char* const fmt) {
  int32_t cpy = i;
  char buf[32];
  snprintf((char*)buf, 32, fmt, i);

  return string_t((char*)buf);
}

}  // namespace VG
#include "../base/DateTime.h"

#include "../base/StringUtil.h"
#include "../base/Logger.h"

#ifdef BR2_OS_LINUX
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#endif
#include <chrono>

namespace BR2 {
DateTime::DateTime(tm *t, int ms) {
  this->_seconds = t->tm_sec;
  this->_minutes = t->tm_min;
  this->_hours = t->tm_hour;
  this->_day = t->tm_mday;
  this->_month = t->tm_mon;
  this->_year = 1900 + t->tm_year;  //tm is from 1900
  this->_milliseconds = ms;
}
string_t DateTime::getAsString() {
  string_t ret = DateTime::getDateTime().toString();
  return ret;
}
DateTime DateTime::fromMilliseconds(uint_fast64_t in_ms) {
  //Test
  Gu::debugBreak();

  uint_fast32_t hh = in_ms % (60 * 60 * 60 * 1000) / (60 * 60 * 1000);
  uint_fast32_t mm = in_ms % (60 * 60 * 1000) / (60 * 1000);
  uint_fast32_t ss = in_ms % (60 * 1000) / (1000);
  uint_fast32_t ms = in_ms % (1000);

  tm tt;
  tt.tm_hour = hh;
  tt.tm_min = mm;
  tt.tm_sec = ss;
  DateTime dt(&tt, ms);
  return dt;
}
DateTime DateTime::getDateTime() {
  time_t t = time(NULL);
  tm *tm_struct = localtime(&t);

  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();

  ms = (ms % 1000);
  DateTime dt(tm_struct, (int)ms);

  return dt;
}
string_t DateTime::toString() {
  //returns "dd/mm/yyyy hh:mm:ss" Todo: localize this (of course).
  string_t dt = dateToStr("/");
  string_t tm = timeToStr(":");
  string_t ret = Stz dt + " " + tm;
  return ret;
}
string_t DateTime::dateToStr(const string_t &delim) {
  string_t ret;
  if (this->_month >= 0) {
    ret += TypeConv::intToStr(this->_month);
    ret += delim;
  }
  if (this->_day >= 0) {
    ret += TypeConv::intToStr(this->_day);
    ret += delim;
  }
  if (this->_year >= 0) {
    ret += TypeConv::intToStr(this->_year);
  }

  return ret;
}
string_t DateTime::timeToStr(const string_t &delim, bool hour, bool minute, bool second, bool millisecond) {
  string_t ret = "";

  string_t h, m, s, ms, del;

  h = m = s = ms = del = "";

  if (hour) {
    if (this->_hours >= 0) {
      h = TypeConv::intToStr(this->_hours, "%02i");
      del = delim;
      ret += h + del;
    }
  }
  if (minute) {
    if (this->_minutes >= 0) {
      m = TypeConv::intToStr(this->_minutes, "%02i");
      del = delim;
      ret += m + del;
    }
  }
  if (second) {
    if (this->_seconds >= 0) {
      s = TypeConv::intToStr(this->_seconds, "%02i");
      del = delim;
      ret += s + del;
    }
  }
  if (millisecond) {
    if (this->_milliseconds >= 0) {
      ms = TypeConv::intToStr(this->_milliseconds, "%03i");
      ret += ms;
    }
  }

  return ret;
}

int DateTime::hour() {
#if defined(BR2_OS_WINDOWS)
  SYSTEMTIME st;
  GetLocalTime(&st);
  return (int)st.wHour;
#elif defined(BR2_OS_LINUX)
  time_t t = time(NULL);
  struct tm *tm_struct = localtime(&t);
  int hour = tm_struct->tm_hour;
  return hour;
#endif
}
int DateTime::minute() {
#if defined(BR2_OS_WINDOWS)
  SYSTEMTIME st;
  GetLocalTime(&st);
  return (int)st.wMinute;
#elif defined(BR2_OS_LINUX)
  time_t t = time(NULL);
  struct tm *tm_struct = localtime(&t);
  int min = tm_struct->tm_min;
  return min;
#endif
}
int DateTime::second() {
#if defined(BR2_OS_WINDOWS)
  SYSTEMTIME st;
  GetLocalTime(&st);
  return (int)st.wSecond;
#elif defined(BR2_OS_LINUX)
  time_t t = time(NULL);
  struct tm *tm_struct = localtime(&t);
  int sec = tm_struct->tm_sec;
  return sec;
#endif
}

}  // namespace BR2

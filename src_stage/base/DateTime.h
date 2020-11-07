/**
*  @file Date.h
*  @date August 23, 2010
*  @author MetalMario971
*/
#pragma once
#ifndef __DATE_2689426881206782656826687_H__
#define __DATE_2689426881206782656826687_H__

#include "../base/BaseHeader.h"

struct tm;

namespace BR2 {
// #define TIME_HH_RSH (0)
// #define TIME_MM_RSH (6)
// #define TIME_SS_RSH (13)
// #define TIME_MS_RSH (20)
//
// #define TIME_HH_LSH (25)
// #define TIME_MM_LSH (18)
// #define TIME_SS_LSH (11)
// #define TIME_MS_LSH (2)
//
// //masks for the given time components if their least significant bit is at bit 0
// #define TIME_HH_CH_MASK (64+32+16+8+4+2+1)
// #define TIME_MM_CH_MASK (64+32+16+8+4+2+1)
// #define TIME_SS_CH_MASK (64+32+16+8+4+2+1)
// #define TIME_MS_CH_MASK (256+128+64+32+16+8+4+2+1)
// #define TIME_UNUSED_CH_MASK (2+1)
//
// // - counts of time bits in the integer
// #define TIME_HH_BIT_CT (7)
// #define TIME_MM_BIT_CT (7)
// #define TIME_SS_BIT_CT (7)
// #define TIME_MS_BIT_CT (9)
// #define TIME_UNUSED_BIT_CT (2)

//Alias for the time structure.
class DateTime : public VirtualMemory {
  int _seconds = -1; /* Seconds.	[0-60] (1 leap second) */
  int _minutes = -1; /* Minutes.	[0-59] */
  int _hours = -1;   /* Hours.	[0-23] */
  int _day = -1;     /* Day.		[1-31] */
  int _month = -1;   /* Month.	[0-11] */
  int _year = -1;    /* Year	- 1900.  */
  int _milliseconds = -1;
  //int tm_wday;			/* Day of week.	[0-6] */
  //int tm_yday;			/* Days in year.[0-365]	*/
  //int tm_isdst;			/* DST.		[-1/0/1]*/

public:
  DateTime(tm* t, int ms);

  string_t toString();
  string_t dateToStr(const string_t& delim = std::string("/"));
  string_t timeToStr(const string_t& delim = std::string(":"), bool hour = true, bool minute = true, bool second = true, bool millisecond = true);

  static string_t getAsString();
  static DateTime getDateTime();
  static int hour();                                         // - Get the current hour.
  static int minute();                                       // - Get the current minute.
  static int second();                                       // - Get the current second.
  static string_t getTimeString(int_fast64_t milliseconds = -1);  //time as string. hh:mm:ss:uuuu
  static DateTime fromMilliseconds(uint_fast64_t milliseconds);   //time as string. hh:mm:ss:uuuu
};

}  // namespace BR2

#endif

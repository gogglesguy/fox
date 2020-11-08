/********************************************************************************
*                                                                               *
*                                T i m e   S t u f f                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 2019,2020 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or modify          *
* it under the terms of the GNU Lesser General Public License as published by   *
* the Free Software Foundation; either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
* GNU Lesser General Public License for more details.                           *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public License      *
* along with this program.  If not, see <http://www.gnu.org/licenses/>          *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxmath.h"
#include "fxascii.h"
#include "FXAtomic.h"
#include "FXElement.h"
#include "FXString.h"
#include "FXSystem.h"


/*
  Notes:
  - Handy functions for manipulating calendar time.
*/


using namespace FX;

/*******************************************************************************/

namespace FX {

// Default formatting string used for time formatting
const FXchar FXSystem::defaultTimeFormat[]="%m/%d/%Y %H:%M:%S";


// ISO 8601 time format (yyyy-mm-ddThh:mm:ss+hhmm) formatting string
const FXchar FXSystem::isoTimeFormat[]="%FT%T%z";


// Time zone variables, set only once
static volatile FXuint local_zone_offset_set=0;  // Called already?
static FXTime          local_zone_offset=0;      // Offset local to UTC (ns)
static FXTime          local_dst_offset=0;       // Offset daylight savings to UTC (ns)
static FXchar          local_zone_name[2][32];   // Local zone name (regular and daylight savings)

// Cumulative days of the year, for non-leap-years and leap-years
static const FXint days_of_year[2][13]={
  {0,31,59,90,120,151,181,212,243,273,304,334,365},
  {0,31,60,91,121,152,182,213,244,274,305,335,366}
  };


// Is year a leap-year
static FXint is_leap(FXint year){
  return !(year%4) && ((year%100) || !(year%400));
  }


// Returns day of week in civil calendar [0, 6] -> [Sun, Sat],
// from z is number of days since 1970-01-01.
static FXuint weekday_from_days(FXlong z){
  return (FXuint)(z>=-4 ? (z+4)%7 : (z+5)%7+6);
  }


// From year, month (1..12), day (1..31) return year-day (1..366).
static FXint yearday_from_date(FXint y,FXint m,FXint d){
  return days_of_year[is_leap(y)][m-1]+d;
  }

/*******************************************************************************/

// Returns number of days since civil 1970-01-01.  Negative values indicate
// days prior to 1970-01-01.
// y is year, m is month of year (1..12), d is day of month (1..31).
FXlong FXSystem::daysFromCivil(FXint y,FXint m,FXint d){
  y-=(m<=2);                                            // March
  FXlong era=(y>=0?y:y-399)/400;                        // Era
  FXuint yoe=(FXuint)(y-era*400);                       // [0, 399]
  FXuint doy=(153*(m+(m>2?-3:9))+2)/5+d-1;              // [0, 365]
  FXuint doe=yoe*365+yoe/4-yoe/100+doy;                 // [0, 146096]
  return era*146097+doe-719468;                         // Relative to epoch
  }


// Returns year/month/day in civil calendar.
// z is number of days since 1970-01-01. Negative values indicate
// days prior to 1970-01-01.
// y is year, m is month of year (1..12), d is day of month (1..31).
void FXSystem::civilFromDays(FXint& y,FXint& m,FXint& d,FXlong z){
  z+=719468;                                            // Relative to era
  FXlong era=(z>=0?z:z-146096)/146097;                  // Era
  FXuint doe=(FXuint)(z-era*146097);                    // [0, 146096]
  FXuint yoe=(doe-doe/1460+doe/36524-doe/146096)/365;   // [0, 399]
  FXuint doy=doe-(365*yoe+yoe/4-yoe/100);               // [0, 365]
  FXuint mp=(5*doy+2)/153;                              // [0, 11]
  y=(FXint)(era*400+yoe);
  d=doy-(153*mp+2)/5+1;                                 // [1, 31]
  m=mp+(mp<10?3:-9);                                    // [1, 12]
  y+=(m<=2);                                            // March
  }

/*******************************************************************************/

// Compute nanoseconds since Unix Epoch from struct tm
FXTime FXSystem::timeFromSystemTime(const Time& st){
  const FXTime seconds=1000000000;
  FXint year=st.year;
  FXint month=st.month;
  FXint day=st.mday;
  FXint hour=st.hour;
  FXint min=st.min;
  FXint sec=st.sec;
  FXint nano=st.nano;
  FXint leap;

  // Validate nanoseconds
  if(nano>seconds){
    sec+=nano/seconds;
    nano%=seconds;
    }

  // Validate seconds
  if(sec>=60){
    min+=sec/60;
    sec%=60;
    }

  // Validate minutes
  if(min>=60){
    hour+=min/60;
    min%=60;
    }

  // Validate days
  if(hour>=24){
    day+=hour/24;
    hour%=24;
    }

  // Validate month
  if(month>=13){
    year+=(month-1)/12;
    month=(month-1)%12+1;
    }

  // Is leap year
  leap=is_leap(year);

  // Validate day of month
  while(day>days_of_year[leap][month]){
    day-=days_of_year[leap][month];
    month+=1;
    if(13<=month){
      month=1;
      year+=1;
      leap=is_leap(year);
      }
    }

  // Return nanoseconds since Epoch
  return (((daysFromCivil(year,month,day)*24+hour)*60+min)*60+sec)*seconds+nano;
  }


// Return system time from number of nanoseconds since Epoch
void FXSystem::systemTimeFromTime(Time& st,FXTime ns){
  const FXTime seconds=1000000000;
  const FXTime minutes=60*seconds;
  const FXTime hours=60*minutes;
  const FXTime days=24*hours;

  // Compute days from nanoseconds, rounding down
  FXlong zz=(0<=ns ? ns : ns-(days-1))/days;

  // Compute date from seconds
  civilFromDays(st.year,st.month,st.mday,zz);

  // Compute day of year
  st.yday=yearday_from_date(st.year,st.month,st.mday);

  // Compute day of week
  st.wday=weekday_from_days(zz);

  // Hours
  ns=ns-zz*days;
  st.hour=(FXint)(ns/hours);

  // Minutes
  ns=ns-st.hour*hours;
  st.min=(FXint)(ns/minutes);

  // Seconds
  ns=ns-st.min*minutes;
  st.sec=(FXint)(ns/seconds);

  // Nanoseconds
  ns=ns-st.sec*seconds;
  st.nano=(FXint)ns;

  // Offset utc
  st.offset=0;
  }

/*******************************************************************************/

//BOOL SystemTimeToTzSpecificLocalTime(const TIME_ZONE_INFORMATION *lpTimeZoneInformation,const SYSTEMTIME *lpUniversalTime,LPSYSTEMTIME lpLocalTime);
//BOOL TzSpecificLocalTimeToSystemTime(const TIME_ZONE_INFORMATION *lpTimeZoneInformation,const SYSTEMTIME *lpLocalTime,LPSYSTEMTIME lpUniversalTime);

// Setup timezone information
static void setuplocaltimezone(){
#if defined(_WIN32)
  const FXTime seconds=1000000000;
  const FXTime minutes=60*seconds;
  const FXTime hours=60*minutes;
  if(atomicSet(&local_zone_offset_set,1)==0){
    TIME_ZONE_INFORMATION tzi;
    clearElms(&tzi,1);
    GetTimeZoneInformation(&tzi);
    local_zone_offset=minutes*tzi.Bias;
    local_dst_offset=0;
    strncpy(local_zone_name[0],tzi.StandardName,ARRAYNUMBER(local_zone_name[0])-1);
    strncpy(local_zone_name[1],tzi.DaylightName,ARRAYNUMBER(local_zone_name[1])-1);
    if(tzi.StandardDate.wMonth!=0 && tzi.DaylightDate.wMonth!=0){
      local_dst_offset=minutes*tzi.DaylightBias;
      }
    }
#else
  const FXTime seconds=1000000000;
  const FXTime minutes=60*seconds;
  const FXTime hours=60*minutes;
  if(atomicSet(&local_zone_offset_set,1)==0){
    tzset();
    local_zone_offset=seconds*timezone;
    local_dst_offset=-hours*daylight;
    strncpy(local_zone_name[0],tzname[0],ARRAYNUMBER(local_zone_name[0])-1);
    strncpy(local_zone_name[1],tzname[1],ARRAYNUMBER(local_zone_name[1])-1);
    }
#endif
  }


// Return offset between standard local time zone to UTC, in nanoseconds
FXTime FXSystem::localTimeZoneOffset(){
  setuplocaltimezone();
  return local_zone_offset;
  }


// Return time zone name (or daylight savings time time zone name)
FXString FXSystem::localTimeZoneName(FXbool dst){
  setuplocaltimezone();
  return local_zone_name[dst];
  }


// Return offset daylight savings time to standard time, in nanoseconds
FXTime FXSystem::daylightSavingsOffset(){
  setuplocaltimezone();
  return local_dst_offset;
  }

/*******************************************************************************/

// Format UTC nanoseconds since Unix Epoch to date-time string using given format
FXString FXSystem::universalTime(FXTime ns,const FXchar *format){
  FXSystem::Time st={0,0,0,0,0,0,0,0,0,0};
  FXSystem::systemTimeFromTime(st,ns);
  return FXSystem::systemTimeFormat(st,format);
  }


// Parse utc date-time string to UTC nanoseconds since Unix Epoch using given format
// Assume string is time in UTC, unless a time zone offset was parsed; in that 
// case, adjust the time according to the zone offset.
FXTime FXSystem::universalTime(const FXchar* string,const FXchar* format){
  FXSystem::Time st={0,0,0,0,0,0,0,0,0,0};
  if(FXSystem::systemTimeParse(st,string,format)){
    return FXSystem::timeFromSystemTime(st)+st.offset*FXLONG(1000000000);
    }
  return forever;
  }


// Parse date-time string to UTC nanoseconds since Unix Epoch using given format
FXTime FXSystem::universalTime(const FXString& string,const FXchar* format){
  return FXSystem::universalTime(string.text(),format);
  }

/*******************************************************************************/

// Format UTC nanoseconds since Unix Epoch to local date-time string using given format
FXString FXSystem::localTime(FXTime ns,const FXchar *format){
  FXTime zoneoffset=FXSystem::localTimeZoneOffset()+FXSystem::daylightSavingsOffset(); 
  FXSystem::Time st={0,0,0,0,0,0,0,0,0,zoneoffset/FXLONG(1000000000)};
  FXSystem::systemTimeFromTime(st,ns-zoneoffset); 
  return FXSystem::systemTimeFormat(st,format);
  }


// Parse local date-time string to UTC nanoseconds since Unix Epoch using given format
// Assume string is time in local time, unless a time zone offset was parsed; in that 
// case, adjust the time according to the zone offset.
FXTime FXSystem::localTime(const FXchar* string,const FXchar* format){
  FXTime zoneoffset=FXSystem::localTimeZoneOffset()+FXSystem::daylightSavingsOffset(); 
  FXSystem::Time st={0,0,0,0,0,0,0,0,0,zoneoffset/FXLONG(1000000000)};
  if(FXSystem::systemTimeParse(st,string,format)){
    return FXSystem::timeFromSystemTime(st)+st.offset*FXLONG(1000000000);
    }
  return forever;
  }


// Parse local date-time string to UTC nanoseconds since Unix Epoch using given format
FXTime FXSystem::localTime(const FXString& string,const FXchar* format){
  return FXSystem::localTime(string.text(),format);
  }

}


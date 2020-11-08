/********************************************************************************
*                                                                               *
*         M i s c e l l a n e o u s   S y s t e m   F u n c t i o n s           *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2019 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#ifndef FXSYSTEM_H
#define FXSYSTEM_H

namespace FX {

namespace FXSystem {

  /// System Time in parts
  struct Time {
    FXint year;         /// Year (e.g. 1970)
    FXint month;        /// Month 1..12
    FXint mday;         /// Day of the month 1..31
    FXint yday;         /// Day in the year 1..366
    FXint wday;         /// Day of the week 0..6
    FXint hour;         /// Hours 0..23
    FXint min;          /// Minutes 0..59
    FXint sec;          /// Seconds 0..60
    FXint nano;         /// Nanoseconds 0..999999999
    FXint offset;       /// Seconds east of utc
    };

  /// Compute nanoseconds since Unix Epoch from system time
  extern FXAPI FXTime timeFromSystemTime(const Time& st);

  /// Return system time from number of nanoseconds since Epoch
  extern FXAPI void systemTimeFromTime(Time& st,FXTime ns);

  /// Format system time to string, returning the number of characters that would have been written.
  extern FXAPI FXint systemTimeFormat(FXchar* string,FXint length,const FXchar* format,const Time& st);
  
  /// Format system time to string, returning the number of characters.
  extern FXAPI FXint systemTimeFormat(FXString& string,const FXchar* format,const Time& st);

  /// Parse system time from string, returning number of characters parsed
  extern FXAPI FXint systemTimeParse(Time& st,const FXchar* string,FXint length,const FXchar* format);

  /// Parse system time from string, returning number of characters parsed
  extern FXAPI FXint systemTimeParse(Time& st,const FXchar* string,const FXchar* format);

  /// Parse system time from string, returning number of characters parsed
  extern FXAPI FXint systemTimeParse(Time& st,const FXString& string,const FXchar* format);

  /**
  * Returns number of days since civil 1970-01-01.  Negative values indicate
  * days prior to 1970-01-01.
  * y is year, m is month of year (1..12), d is day of month (1..31).
  */
  extern FXAPI FXlong daysFromCivil(FXint y,FXint m,FXint d);

  /**
  * Returns year/month/day in civil calendar.
  * z is number of days since 1970-01-01. Negative values indicate
  * days prior to 1970-01-01.
  * y is year, m is month of year (1..12), d is day of month (1..31).
  */
  extern FXAPI void civilFromDays(FXint& y,FXint& m,FXint& d,FXlong z);

  /// Convert NTP format (ssss:ffff) to nanoseconds since Unix Epoch
  extern FXAPI FXTime timeFromNTPTime(FXulong ntptime);

  /// Convert nanoseconds since Unix Epoch to NTP (ssss:ffff)
  extern FXAPI FXulong ntpTimeFromTime(FXTime ns);

  /// Return leap seconds since since Unix Epoch time
  extern FXAPI FXival leapSeconds(FXTime ns);

  /// Default formatting string used for time formatting
  extern FXAPI const FXchar defaultTimeFormat[];

  /// ISO 8601 time format (yyyy-mm-ddThh:mm:ss+hh:mm) formatting string
  extern FXAPI const FXchar isoTimeFormat[];

  /// Convert time in nanoseconds since 1/1/1970 to local date string
  extern FXAPI FXString localTime(FXTime value);

  /// Convert time in nanoseconds since 1/1/1970 to universal date string
  extern FXAPI FXString universalTime(FXTime value);

  /**
  * Convert time in nanoseconds since 1/1/1970 to local date string as per strftime(3).
  * Format characters supported by most systems are:
  *
  *  %a %A %b %B %c %d %H %I %j %m %M %p %S %U %w %W %x %X %y %Y %Z %%
  *
  * Some systems support additional conversions.
  */
  extern FXAPI FXString localTime(const FXchar *format,FXTime value);

  /**
  * Convert time in nanoseconds since 1/1/1970 to universal date string as per strftime(3).
  */
  extern FXAPI FXString universalTime(const FXchar *format,FXTime value);

  /// Convert date string to time in nanoseconds since 1/1/1970.
  extern FXAPI FXTime localTime(const FXchar* string);
  extern FXAPI FXTime localTime(const FXString& string);

  /**
  * Convert date string to time in nanoseconds since 1/1/1970, as per strptime.
  * Format characters supported by most systems are:
  *
  *  %a %A %b %B %c %d %H %I %j %m %M %p %S %U %w %W %x %X %y %Y %Z %%
  *
  * Some systems support additional conversions.
  */
  extern FXAPI FXTime localTime(const FXchar* string,const FXchar* format);
  extern FXAPI FXTime localTime(const FXString& string,const FXchar* format);


  /// Get effective user id
  extern FXAPI FXuint user();

  /// Get effective group id
  extern FXAPI FXuint group();

  /// Return owner name from uid if available
  extern FXAPI FXString userName(FXuint uid);

  /// Return group name from gid if available
  extern FXAPI FXString groupName(FXuint gid);

  /// Get current effective user name
  extern FXAPI FXString currentUserName();

  /// Get current effective group name
  extern FXAPI FXString currentGroupName();


  /// Get permissions string
  extern FXAPI FXString modeString(FXuint mode);



  /// Return value of environment variable name
  extern FXAPI FXString getEnvironment(const FXString& name);

  /// Change value of environment variable name, return true if success
  extern FXAPI FXbool setEnvironment(const FXString& name,const FXString& value);



  /// Get the current working directory
  extern FXAPI FXString getCurrentDirectory();

  /// Set the current working directory
  extern FXAPI FXbool setCurrentDirectory(const FXString& path);

  /// Return the current drive (for Win32 systems)
  extern FXAPI FXString getCurrentDrive();

  /// Set the current drive (for Win32 systems)
  extern FXAPI FXbool setCurrentDrive(const FXString& prefix);



  /// Get executable path
  extern FXAPI FXString getExecPath();

  /// Return known executable file extensions (Windows)
  extern FXAPI FXString getExecExtensions();

  /// Return the home directory for the current user
  extern FXAPI FXString getHomeDirectory();

  /// Return the home directory for a given user
  extern FXAPI FXString getUserDirectory(const FXString& user);

  /// Return temporary directory
  extern FXAPI FXString getTempDirectory();


  /// Return host name
  extern FXAPI FXString getHostName();


  /// Determine if UTF8 locale in effect
  extern FXAPI FXbool localeIsUTF8();

  /// Get name of calling executable
  extern FXAPI FXString getExecFilename();

  /**
  * Get DLL name for given base name; for example "png"
  * becomes "libpng.so" on Linux, and "png.dll" on Windows.
  */
  extern FXAPI FXString dllName(const FXString& name);
  }

}

#endif

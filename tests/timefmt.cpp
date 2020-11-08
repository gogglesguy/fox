/********************************************************************************
*                                                                               *
*                             String Format I/O Test                            *
*                                                                               *
*********************************************************************************
* Copyright (C) 2007,2020 by Jeroen van der Zijp.   All Rights Reserved.        *
********************************************************************************/
#include "fx.h"
#include "fx3d.h"
#include <time.h>


/*
  Notes:
  - Test battery for time formatting and parsing.
*/


/*******************************************************************************/


// Time print formats
const FXchar *timeformat[]={
  "%H:%M:%S.%fm",
  "%l:%M:%S %P",
  "%A %B %d %Y %I:%M:%S.%fu %p %z (UTC)",
  "%Y/%m/%d",
  "%G-W%V-%u",
  "%Y %j",
  "%Y %U %w",
  "%Y %W %u",
  "%Y %j %w",
  "%Y %U %w",
  "%T",
  "%Z",
  "%FT%T%z",
  "%s"
  };


// Time values to print
const FXTime timevalues[]={
  FXLONG(-885744000000000000),
  FXLONG(-192799800000000000),
  FXLONG(                  0),
  FXLONG( 798282120000000000),
  FXLONG(1000197960000000000),
  FXLONG(1571388300000000000),
  FXLONG(1588006865264666745)
  };


// Time parse formats
const FXchar *timeparse[]={
  "%Z",                 // Zone offset
  "%Y",                 // Year
  "%Y %j",              // Year, day of year
  "%Y/%m/%d",           // Year, month, day
  "%b %d %Y",           // Month, day, year
  "%Y %U %w",           // Year, week, day of week
  "%Y %U",              // Year, week
  "%H%M%S",             // Hours, minutes, seconds
  "%s",                 // Seconds since Epoch
  "%z",                 // Zone offset
  "%z",                 // Zone offset
  "%F",                 // ISO date format
  };


// Time strings to parse
const FXchar* timestrings[]={
  "CST",
  "2020",
  "2020 38",
  "2020/2/7",
  "Feb  7 2020",
  "2020 3 1",
  "2020 5",
  "112233",
  "1579508820",
  "+0300",
  "-03",
  "2020-2-7",
  };


// ISO week/year checks
const FXint week_number_cases[][3]={
  {2005, 1, 1},     // Sat  1 Jan 2005      2005-01-01      2004-W53-6
  {2005, 1, 2},     // Sun  2 Jan 2005      2005-01-02      2004-W53-7
  {2005,12,31},     // Sat 31 Dec 2005      2005-12-31      2005-W52-6
  {2006, 1, 1},     // Sun  1 Jan 2006      2006-01-01      2005-W52-7
  {2006, 1, 2},     // Mon  2 Jan 2006      2006-01-02      2006-W01-1
  {2006,12,31},     // Sun 31 Dec 2006      2006-12-31      2006-W52-7
  {2007, 1, 1},     // Mon  1 Jan 2007      2007-01-01      2007-W01-1
  {2007,12,30},     // Sun 30 Dec 2007      2007-12-30      2007-W52-7
  {2007,12,31},     // Mon 31 Dec 2007      2007-12-31      2008-W01-1
  {2008, 1, 1},     // Tue  1 Jan 2008      2008-01-01      2008-W01-2
  {2008,12,28},     // Sun 28 Dec 2008      2008-12-28      2008-W52-7
  {2008,12,29},     // Mon 29 Dec 2008      2008-12-29      2009-W01-1
  {2008,12,30},     // Tue 30 Dec 2008      2008-12-30      2009-W01-2
  {2008,12,31},     // Wed 31 Dec 2008      2008-12-31      2009-W01-3
  {2009, 1, 1},     // Thu  1 Jan 2009      2009-01-01      2009-W01-4
  {2009,12,31},     // Thu 31 Dec 2009      2009-12-31      2009-W53-4
  {2010, 1, 1},     // Fri  1 Jan 2010      2010-01-01      2009-W53-5
  {2010, 1, 2},     // Sat  2 Jan 2010      2010-01-02      2009-W53-6
  {2010, 1, 3},     // Sun  3 Jan 2010      2010-01-03      2009-W53-7 
  {2020, 4,27}      // Mon 27 Apr 2020      2020-04-27      2020-W18-1
  };


// ISO week/year checks
const FXchar* isoweek_date_strings[]={
  "2004-W53-6",     // Sat  1 Jan 2005      2005-01-01      2004-W53-6
  "2004-W53-7",     // Sun  2 Jan 2005      2005-01-02      2004-W53-7
  "2005-W52-6",     // Sat 31 Dec 2005      2005-12-31      2005-W52-6
  "2005-W52-7",     // Sun  1 Jan 2006      2006-01-01      2005-W52-7
  "2006-W01-1",     // Mon  2 Jan 2006      2006-01-02      2006-W01-1
  "2006-W52-7",     // Sun 31 Dec 2006      2006-12-31      2006-W52-7
  "2007-W01-1",     // Mon  1 Jan 2007      2007-01-01      2007-W01-1
  "2007-W52-7",     // Sun 30 Dec 2007      2007-12-30      2007-W52-7
  "2008-W01-1",     // Mon 31 Dec 2007      2007-12-31      2008-W01-1
  "2008-W01-2",     // Tue  1 Jan 2008      2008-01-01      2008-W01-2
  "2008-W52-7",     // Sun 28 Dec 2008      2008-12-28      2008-W52-7
  "2009-W01-1",     // Mon 29 Dec 2008      2008-12-29      2009-W01-1
  "2009-W01-2",     // Tue 30 Dec 2008      2008-12-30      2009-W01-2
  "2009-W01-3",     // Wed 31 Dec 2008      2008-12-31      2009-W01-3
  "2009-W01-4",     // Thu  1 Jan 2009      2009-01-01      2009-W01-4
  "2009-W53-4",     // Thu 31 Dec 2009      2009-12-31      2009-W53-4
  "2009-W53-5",     // Fri  1 Jan 2010      2010-01-01      2009-W53-5
  "2009-W53-6",     // Sat  2 Jan 2010      2010-01-02      2009-W53-6
  "2009-W53-7",     // Sun  3 Jan 2010      2010-01-03      2009-W53-7 
  "2020-W18-1"      // Mon 27 Apr 2020      2020-04-27      2020-W18-1
  };
  

// Populate time struct, then print with time format
void format_time(FXchar* buffer,FXint length,const FXchar* format,FXlong time){
  FXSystem::Time st;
  FXSystem::systemTimeFromTime(st,time);
  st.offset=-3600*5;
  FXSystem::systemTimeFormat(buffer,length,format,st);  
  }


// Test week number stuff
void weeknumber(FXchar* buffer,FXint length,FXint year,FXint month,FXint day){
  const FXTime seconds=1000000000;
  const FXTime DAY=86400*seconds;
  FXSystem::Time st;
  FXSystem::systemTimeFromTime(st,FXSystem::daysFromCivil(year,month,day)*DAY);
  systemTimeFormat(buffer,length,"%a %e %b %Y    %Y-%m-%d    %G-W%V-%u (doy=%j)",st);  
  }
  

// Print options
void printusage(const char* prog){
  fxmessage("%s options:\n",prog);
  fxmessage("  -h, --help               Print help.\n");
  fxmessage("  --tracelevel <level>     Set trace level.\n");
  fxmessage("  --year <year>            Set the year.\n");
  fxmessage("  --month <month>          Set the month (1..12).\n");
  fxmessage("  --mday <mday>            Set the day of month (1..31).\n");
  fxmessage("  --hour <hour>            Set the hour (0..23).\n");
  fxmessage("  --min <min>              Set the minute (0..59).\n");
  fxmessage("  --sec <sec>              Set the second (0..59).\n");
  fxmessage("  --nano <nano>            Set the nano second (0..999999999).\n");
  fxmessage("  --format <format>        Set format.\n");
  fxmessage("  --to-iso                 Test conversion to iso-week/day.\n");
  fxmessage("  --from-iso               Test converstion from iso-week/day.\n");
  }


// Start
int main(int argc,char* argv[]){
  const FXTime seconds=1000000000;
  const FXTime DAY=86400*seconds;
  FXint  year,month,day,hour,min,sec,days;
  FXString string;
  FXchar buffer[1024];
  FXTime z,ct0,ct1;
  FXuval x,y;
  
  // System Time in parts
  FXSystem::Time st;

  // Current time
  ct0=ct1=FXThread::time();
  
  // Init structure
  FXSystem::systemTimeFromTime(st,ct0);
  
  // Grab a few arguments
  for(FXint arg=1; arg<argc; ++arg){
    if(strcmp(argv[arg],"-h")==0 || strcmp(argv[arg],"--help")==0){
      printusage(argv[0]);
      return 0;
      }
    else if(strcmp(argv[arg],"--tracelevel")==0){
      if(++arg>=argc){ fxmessage("Missing --tracelevel argument.\n"); exit(1); }
      fxTraceLevel=strtoul(argv[arg],NULL,0);
      }
    else if(strcmp(argv[arg],"--year")==0){
      if(++arg>=argc){ fxmessage("Missing --year argument.\n"); exit(1); }
      st.year=strtoul(argv[arg],NULL,0);
      }
    else if(strcmp(argv[arg],"--month")==0){
      if(++arg>=argc){ fxmessage("Missing --month argument.\n"); exit(1); }
      st.month=strtoul(argv[arg],NULL,0);
      }
    else if(strcmp(argv[arg],"--mday")==0){
      if(++arg>=argc){ fxmessage("Missing --mday argument.\n"); exit(1); }
      st.mday=strtoul(argv[arg],NULL,0);
      }
    else if(strcmp(argv[arg],"--yday")==0){
      if(++arg>=argc){ fxmessage("Missing --mday argument.\n"); exit(1); }
      st.yday=strtoul(argv[arg],NULL,0);
      }
    else if(strcmp(argv[arg],"--hour")==0){
      if(++arg>=argc){ fxmessage("Missing --mday argument.\n"); exit(1); }
      st.hour=strtoul(argv[arg],NULL,0);
      }
    else if(strcmp(argv[arg],"--min")==0){
      if(++arg>=argc){ fxmessage("Missing --mday argument.\n"); exit(1); }
      st.min=strtoul(argv[arg],NULL,0);
      }
    else if(strcmp(argv[arg],"--sec")==0){
      if(++arg>=argc){ fxmessage("Missing --mday argument.\n"); exit(1); }
      st.sec=strtoul(argv[arg],NULL,0);
      }
    else if(strcmp(argv[arg],"--nano")==0){
      if(++arg>=argc){ fxmessage("Missing --mday argument.\n"); exit(1); }
      st.nano=strtoul(argv[arg],NULL,0);
      }
    else if(strcmp(argv[arg],"--offset")==0){
      if(++arg>=argc){ fxmessage("Missing --mday argument.\n"); exit(1); }
      st.offset=strtoul(argv[arg],NULL,0);
      }
    else if(strcmp(argv[arg],"--string")==0){
      if(++arg>=argc){ fxmessage("Missing --string argument.\n"); exit(1); }
      string=argv[arg];
      }
    else if(strcmp(argv[arg],"--format")==0){
      if(++arg>=argc){ fxmessage("Missing --format argument.\n"); exit(1); }
      FXSystem::systemTimeFormat(string,argv[arg],st);
      fxmessage("format=%-16s output=%s\n",argv[arg],string.text());
      }
    else if(strcmp(argv[arg],"--to-iso")==0){           // Test ISO week and year
      for(y=0; y<ARRAYNUMBER(week_number_cases); y++){
        weeknumber(buffer,sizeof(buffer),week_number_cases[y][0],week_number_cases[y][1],week_number_cases[y][2]);
        fxmessage("%s\n",buffer);
        }
      fxmessage("\n");
      return 0;
      }
    else if(strcmp(argv[arg],"--from-iso")==0){         // Test ISO week and year
      for(x=0; x<ARRAYNUMBER(isoweek_date_strings); x++){
        clearElms(&st,1);
        FXSystem::systemTimeParse(st,isoweek_date_strings[x],"%G-W%V-%u");
        z=FXSystem::timeFromSystemTime(st);
        fxmessage("%-16s -> ",isoweek_date_strings[x]);
        fxmessage("%04d/%02d/%02d %02d:%02d:%02d  (wday=%1d  yday=%03d)\n",st.year,st.month,st.mday,st.hour,st.min,st.sec,st.wday,st.yday);
        }
      return 0;
      }
    else{
      printusage(argv[0]);
      return 0;
      }
    }

#if 1
  // Calculate nanoseconds
  ct1=FXSystem::timeFromSystemTime(st);

  // Dump original and updated
  fxmessage("ct0=%'-16lld\n",ct0);
  fxmessage("ct1=%'-16lld\n",ct1);

  // Format every which way
  for(x=0; x<ARRAYNUMBER(timeformat); x++){
    format_time(buffer,sizeof(buffer),timeformat[x],ct1);
    fxmessage("format=%-16s output=%s\n",timeformat[x],buffer);
    }
  fxmessage("\n");
#endif

#if 1
  // Test new civilFromDays() and daysFromCivil()
  for(y=0; y<ARRAYNUMBER(timevalues); y++){
    days=(timevalues[y]>=0 ? timevalues[y] : (timevalues[y]-(DAY-1)))/DAY;
    FXSystem::civilFromDays(year,month,day,days);
    fxmessage("%12ld - > %04d/%02d/%02d %02d:%02d:%02d  ",days,year,month,day,hour,min,sec);
    days=FXSystem::daysFromCivil(year,month,day);
    fxmessage("%04d/%02d/%02d %02d:%02d:%02d - > %12ld\n",year,month,day,hour,min,sec,days);
    }
  fxmessage("\n");
#endif

#if 1
  // Test ISO week and year
  for(y=0; y<ARRAYNUMBER(week_number_cases); y++){
    weeknumber(buffer,sizeof(buffer),week_number_cases[y][0],week_number_cases[y][1],week_number_cases[y][2]);
    fxmessage("%s\n",buffer);
    }
  fxmessage("\n");
#endif

#if 1
  // Testing time formats
  for(y=0; y<ARRAYNUMBER(timevalues); y++){
    fxmessage("time=%'-16lld\n",timevalues[y]);
    for(x=0; x<ARRAYNUMBER(timeformat); x++){
      format_time(buffer,sizeof(buffer),timeformat[x],timevalues[y]);
      fxmessage("format=%-16s output=%s\n",timeformat[x],buffer);
      }
    fxmessage("\n");
    }
#endif

#if 1
  for(x=0; x<ARRAYNUMBER(timeparse); x++){
    FXSystem::systemTimeParse(st,timestrings[x],timeparse[x]);
    z=FXSystem::timeFromSystemTime(st);
    fxmessage("format=%-16s input=%-16s -> ",timeparse[x],timestrings[x]);
    format_time(buffer,sizeof(buffer),"date: %Y/%m/%d time: %H:%M:%S wday: %w yday: %j week: %U epoch: %s",z);
    fxmessage("%s \n",buffer);
    }
  fxmessage("\n");
#endif

#if 1
  for(x=0; x<ARRAYNUMBER(timeparse); x++){
    clearElms(&st,1);
    FXSystem::systemTimeParse(st,timestrings[x],timeparse[x]);
    z=FXSystem::timeFromSystemTime(st);
    fxmessage("format=%-16s input=%-16s -> ",timeparse[x],timestrings[x]);
    fxmessage("%04d/%02d/%02d %02d:%02d:%02d  (wday=%1d  yday=%03d gmt=%6d z=%lld)\n",st.year,st.month,st.mday,st.hour,st.min,st.sec,st.wday,st.yday,st.offset,z);
    }
  fxmessage("\n");
#endif

  return 0;
  }


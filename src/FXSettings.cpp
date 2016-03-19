/********************************************************************************
*                                                                               *
*                           S e t t i n g s   C l a s s                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************
* $Id: FXSettings.cpp,v 1.71 2007/06/03 05:30:38 fox Exp $                      *
********************************************************************************/
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxascii.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXStringDict.h"
#include "FXFile.h"
#include "FXSettings.h"

/*
  Notes:

  - Format for settings database file:

    [Section Key]
    EntryKey=string-with-no-spaces
    EntryKey="string\nwith a\nnewline in it\n"
    EntryKey=" string with leading and trailing spaces and \"embedded\" in it  "
    EntryKey=string with no leading or trailing spaces

  - EntryKey may is of the form "ali baba", "ali-baba", "ali_baba", or "ali.baba".

  - Leading/trailing spaces are NOT part of the EntryKey.

  - FXSectionDict should go; FXSettings should simply derive from FXDict.

  - Escape sequences now allow octal (\377) as well as hex (\xff) codes.

  - EntryKey format should be like values.

  - Extensive error checking in unparseFile() to ensure no settings data is
    lost when disk is full.

*/

#define MAXBUFFER 2000
#define MAXNAME   200
#define MAXVALUE  2000

using namespace FX;

/*******************************************************************************/

namespace FX {


// Furnish our own version
extern FXAPI FXint __sscanf(const FXchar* string,const FXchar* format,...);
extern FXAPI FXint __vsscanf(const FXchar* string,const FXchar* format,va_list arg_ptr);
extern FXAPI FXint __snprintf(FXchar* string,FXint length,const FXchar* format,...);
extern FXAPI FXint __vsnprintf(FXchar* string,FXint length,const FXchar* format,va_list args);


// Object implementation
FXIMPLEMENT(FXSettings,FXDict,NULL,0)


// Construct settings database
FXSettings::FXSettings(){
  modified=false;
  }


// Construct copy of existing database
FXSettings::FXSettings(const FXSettings& orig):FXDict(orig){
  register FXint i;
  modified=orig.modified;
  for(i=0; i<orig.total; i++){
    if(0<=dict[i].hash){
      dict[i].data=new FXStringDict(*((FXStringDict*)orig.dict[i].data));
      }
    }
  }


// Assignment operator
FXSettings& FXSettings::operator=(const FXSettings& orig){
  register FXint i;
  if(&orig!=this){
    FXDict::operator=(orig);
    for(i=0; i<orig.total; i++){
      if(0<=orig.dict[i].hash){
        dict[i].data=new FXStringDict(*((FXStringDict*)orig.dict[i].data));
        }
      }
    }
  return *this;
  }


// Create data
void *FXSettings::createData(void*){
  return new FXStringDict;
  }


// Delete data
void FXSettings::deleteData(void* ptr){
  delete ((FXStringDict*)ptr);
  }


// Read string
static FXbool readString(FXFile& file,FXchar *buffer,FXint& bol,FXint& eol,FXint& end){
  register FXint n;
  while(eol<end && buffer[eol++]!='\n');
  if(eol>=end){
    if(bol<end){ memmove(buffer,buffer+bol,end-bol); }
    end=end-bol;
    bol=0;
    eol=end;
    n=file.readBlock(buffer+end,MAXBUFFER-end);
    if(n<0) return false;
    end+=n;
    while(eol<end && buffer[eol++]!='\n');
    }
  return bol<eol;
  }


// Parse filename
FXbool FXSettings::parseFile(const FXString& filename,FXbool mrk){
  FXFile file(filename,FXIO::Reading);
  if(file.isOpen()){
    FXchar line[MAXBUFFER];
    FXint bol,eol,end,section,name,value,p,lineno;
    FXStringDict *group=NULL;

    lineno=bol=eol=end=0;

    // Read lines
    while(readString(file,line,bol,eol,end)){
      lineno++;

      // Skip BOM, if any
      if(lineno==1 && line[bol]==(FXchar)0xef && line[bol+1]==(FXchar)0xbb && line[bol+2]==(FXchar)0xbf) bol+=3;

      // Skip leading spaces
      while(bol<eol && Ascii::isBlank(line[bol])) bol++;

      // Skip comment lines and empty lines
      if(bol>=eol || line[bol]=='#' || line[bol]==';' || line[bol]=='\n' || line[bol]=='\r') goto next;

      // Parse section name
      if(line[bol]=='['){

        // Scan section name
        for(section=++bol; bol<eol && line[bol]!=']' && !Ascii::isControl(line[bol]); bol++);

        // Check errors
        if(bol>=eol || line[bol]!=']'){ fxwarning("%s:%d: illegal section name.\n",filename.text(),lineno); goto next; }

        // Terminate name
        line[bol]='\0';

        // Add new section dict
        group=insert(line+section);
        }

      // Parse name-value pair
      else{

        // Should have a group
        if(!group){ fxwarning("%s:%d: settings entry should follow a section.\n",filename.text(),lineno); goto next; }

        // Scan key name
        for(name=bol; bol<eol && line[bol]!='=' && !Ascii::isControl(line[bol]); bol++);

        // Check errors
        if(bol>=eol || line[bol]!='='){ fxwarning("%s:%d: expected '=' to follow key.\n",filename.text(),lineno); goto next; }

        // Remove trailing spaces after name
        for(p=bol; name<p && Ascii::isBlank(line[p-1]); p--);

        // Terminate name
        line[p]='\0';

        // Skip leading spaces
        for(bol++; bol<eol && Ascii::isBlank(line[bol]); bol++);

        // Scan value
        for(value=bol; bol<eol && !Ascii::isControl(line[bol]); bol++);

        // Remove trailing spaces after value
        for(p=bol; value<p && Ascii::isBlank(line[p-1]); p--);

        // Terminate value
        line[p]='\0';

        // Add entry to current section
        group->replace(line+name,dequote(line+value),mrk);
        }
next: bol=eol;
      }

    // Done
    return true;
    }
  return false;
  }


// Write string
static FXbool writeString(FXFile& file,const FXchar* string){
  register FXint len=strlen(string);
  return file.writeBlock(string,len)==len;
  }


// Unparse registry file
FXbool FXSettings::unparseFile(const FXString& filename){
  FXFile file(filename,FXIO::Writing);
  FXchar line[MAXVALUE];
  if(file.isOpen()){

    // Loop over all sections
    for(FXint s=first(); s<size(); s=next(s)){

      // Get group
      FXStringDict* group=data(s);
      FXbool sec=false;

      // Loop over all entries
      for(FXint e=group->first(); e<group->size(); e=group->next(e)){

        // Is key-value pair marked?
        if(group->mark(e)){

          // Write section name if not written yet
          if(!sec){
            if(!writeString(file,"[")) goto x;
            if(!writeString(file,key(s))) goto x;
            if(!writeString(file,"]" ENDLINE)) goto x;
            sec=true;
            }

          // Write marked key-value pairs only
          if(!writeString(file,group->key(e))) goto x;
          if(!writeString(file,"=")) goto x;
          if(!writeString(file,enquote(line,group->data(e)))) goto x;
          if(!writeString(file,ENDLINE)) goto x;
          }
        }

      // Blank line after end
      if(sec){
        if(!writeString(file,ENDLINE)) goto x;
        }
      }
    return true;
    }
x:return false;
  }


// Dequote a value, in situ
FXchar* FXSettings::dequote(FXchar* text) const {
  register FXchar *result=text;
  register FXchar *ptr=text;
  register FXuint v;
  if(*text=='"'){
    text++;
    while((v=*text++)!='\0' && v!='\n' && v!='"'){
      if(v=='\\'){
        v=*text++;
        switch(v){
          case 'n':
            v='\n';
            break;
          case 'r':
            v='\r';
            break;
          case 'b':
            v='\b';
            break;
          case 'v':
            v='\v';
            break;
          case 'a':
            v='\a';
            break;
          case 'f':
            v='\f';
            break;
          case 't':
            v='\t';
            break;
          case '\\':
            v='\\';
            break;
          case '"':
            v='"';
            break;
          case '\'':
            v='\'';
            break;
          case 'x':
            v='x';
            if(Ascii::isHexDigit(*text)){
              v=Ascii::digitValue(*text++);
              if(Ascii::isHexDigit(*text)){
                v=(v<<4)+Ascii::digitValue(*text++);
                }
              }
            break;
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
            v=v-'0';
            if('0'<=*text && *text<='7'){
              v=(v<<3)+*text++-'0';
              if('0'<=*text && *text<='7'){
                v=(v<<3)+*text++-'0';
                }
              }
            break;
          }
        }
      *ptr++=v;
      }
    *ptr='\0';
    }
  return result;
  }


// Check if quotes are needed
static FXbool needquotes(const FXchar* text){
  register const FXchar *ptr=text;
  register FXuchar c;
  while((c=*ptr++)!='\0'){
    if(0x7f<=c || c<0x20 || c=='"' || c=='\'' || c=='\\' || (c==' ' && (ptr==(text+1) || *ptr=='\0'))) return true;
    }
  return false;
  }


// Enquote a value
FXchar* FXSettings::enquote(FXchar* result,const FXchar* text){
  register FXchar *end=result+MAXVALUE-6;
  register FXchar *ptr=result;
  register FXuchar c;
  if(needquotes(text)){
    *ptr++='"';
    while((c=*text++)!='\0' && ptr<end){
      switch(c){
        case '\n':
          *ptr++='\\';
          *ptr++='n';
          break;
        case '\r':
          *ptr++='\\';
          *ptr++='r';
          break;
        case '\b':
          *ptr++='\\';
          *ptr++='b';
          break;
        case '\v':
          *ptr++='\\';
          *ptr++='v';
          break;
        case '\a':
          *ptr++='\\';
          *ptr++='a';
          break;
        case '\f':
          *ptr++='\\';
          *ptr++='f';
          break;
        case '\t':
          *ptr++='\\';
          *ptr++='t';
          break;
        case '\\':
          *ptr++='\\';
          *ptr++='\\';
          break;
        case '"':
          *ptr++='\\';
          *ptr++='"';
          break;
        case '\'':
          *ptr++='\\';
          *ptr++='\'';
          break;
        default:
          if(c<0x20 || 0x7f<c){
            *ptr++='\\';
            *ptr++='x';
            *ptr++=FXString::value2Digit[c>>4];
            *ptr++=FXString::value2Digit[c&15];
            }
          else{
            *ptr++=c;
            }
          break;
        }
      }
    *ptr++='"';
    }
  else{
    while((c=*text++)!='\0' && ptr<end){
      *ptr++=c;
      }
    }
  *ptr='\0';
  return result;
  }


// Read a formatted registry entry
FXint FXSettings::readFormatEntry(const FXchar *section,const FXchar *name,const FXchar *fmt,...){
  if(!section || !section[0]){ fxerror("FXSettings::readFormatEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readFormatEntry: bad name argument.\n"); }
  if(!fmt){ fxerror("FXSettings::readFormatEntry: bad fmt argument.\n"); }
  FXStringDict *group=find(section);
  FXint result=0;
  va_list args;
  va_start(args,fmt);
  if(group){
    const char *value=group->find(name);
    if(value){
      result=__vsscanf(value,fmt,args);
      }
    }
  va_end(args);
  return result;
  }


// Read a string-valued registry entry
const FXchar *FXSettings::readStringEntry(const FXchar *section,const FXchar *name,const FXchar *def){
  if(!section || !section[0]){ fxerror("FXSettings::readStringEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readStringEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const char *value=group->find(name);
    if(value) return value;
    }
  return def;
  }


// Read a int-valued registry entry
FXint FXSettings::readIntEntry(const FXchar *section,const FXchar *name,FXint def){
  if(!section || !section[0]){ fxerror("FXSettings::readIntEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readIntEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const char *value=group->find(name);
    if(value){
      FXint ivalue;
      if(__sscanf(value,"%d",&ivalue)==1) return ivalue;
      }
    }
  return def;
  }


// Read a unsigned int-valued registry entry
FXuint FXSettings::readUIntEntry(const FXchar *section,const FXchar *name,FXuint def){
  if(!section || !section[0]){ fxerror("FXSettings::readUnsignedEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readUnsignedEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const char *value=group->find(name);
    if(value){
      FXuint ivalue;
      if(__sscanf(value,"%u",&ivalue)==1) return ivalue;
      }
    }
  return def;
  }


// Read a 64-bit long integer registry entry
FXlong FXSettings::readLongEntry(const FXchar *section,const FXchar *name,FXlong def){
  if(!section || !section[0]){ fxerror("FXSettings::readLongEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readLongEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const char *value=group->find(name);
    if(value){
      FXlong lvalue;
      if(__sscanf(value,"%lld",&lvalue)==1) return lvalue;
      }
    }
  return def;
  }


// Read a 64-bit unsigned long integer registry entry
FXulong FXSettings::readULongEntry(const FXchar *section,const FXchar *name,FXulong def){
  if(!section || !section[0]){ fxerror("FXSettings::readULongEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readULongEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const char *value=group->find(name);
    if(value){
      FXulong lvalue;
      if(value[0]=='0' && (value[1]=='x' || value[1]=='X')){
        if(__sscanf(value,"%llu",&lvalue)==1) return lvalue;
        }
      }
    }
  return def;
  }


// Read a double-valued registry entry
FXdouble FXSettings::readRealEntry(const FXchar *section,const FXchar *name,FXdouble def){
  if(!section || !section[0]){ fxerror("FXSettings::readRealEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readRealEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const char *value=group->find(name);
    if(value){
      FXdouble dvalue;
      if(__sscanf(value,"%lf",&dvalue)==1) return dvalue;
      }
    }
  return def;
  }


// Read a color registry entry
FXColor FXSettings::readColorEntry(const FXchar *section,const FXchar *name,FXColor def){
  if(!section || !section[0]){ fxerror("FXSettings::readColorEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readColorEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const char *value=group->find(name);
    if(value){
      return fxcolorfromname(value);
      }
    }
  return def;
  }


// Read a boolean registry entry
FXbool FXSettings::readBoolEntry(const FXchar *section,const FXchar *name,FXbool def){
  if(!section || !section[0]){ fxerror("FXSettings::readBoolEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readBoolEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const char *value=group->find(name);
    if(value){
      if(comparecase(value,"true")==0) return true;
      else if(comparecase(value,"false")==0) return false;
      else if(comparecase(value,"yes")==0) return true;
      else if(comparecase(value,"no")==0) return false;
      else if(comparecase(value,"on")==0) return true;
      else if(comparecase(value,"off")==0) return false;
      else if(comparecase(value,"1")==0) return true;
      else if(comparecase(value,"0")==0) return false;
      }
    }
  return def;
  }


// Write a formatted registry entry
FXint FXSettings::writeFormatEntry(const FXchar *section,const FXchar *name,const FXchar *fmt,...){
  if(!section || !section[0]){ fxerror("FXSettings::writeFormatEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::writeFormatEntry: bad name argument.\n"); }
  if(!fmt){ fxerror("FXSettings::writeFormatEntry: bad fmt argument.\n"); }
  FXStringDict *group=insert(section);
  FXint result=0;
  va_list args;
  va_start(args,fmt);
  if(group){
    FXchar buffer[2048];
    result=__vsnprintf(buffer,sizeof(buffer),fmt,args);
    group->replace(name,buffer,true);
    modified=true;
    }
  va_end(args);
  return result;
  }


// Write a string-valued registry entry
FXbool FXSettings::writeStringEntry(const FXchar *section,const FXchar *name,const FXchar *val){
  if(!section || !section[0]){ fxerror("FXSettings::writeStringEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::writeStringEntry: bad name argument.\n"); }
  FXStringDict *group=insert(section);
  if(group){
    group->replace(name,val,true);
    modified=true;
    return true;
    }
  return false;
  }


// Write a int-valued registry entry
FXbool FXSettings::writeIntEntry(const FXchar *section,const FXchar *name,FXint val){
  if(!section || !section[0]){ fxerror("FXSettings::writeIntEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::writeIntEntry: bad name argument.\n"); }
  FXStringDict *group=insert(section);
  if(group){
    FXchar buffer[32];
    __snprintf(buffer,sizeof(buffer),"%d",val);
    group->replace(name,buffer,true);
    modified=true;
    return true;
    }
  return false;
  }


// Write a unsigned int-valued registry entry
FXbool FXSettings::writeUIntEntry(const FXchar *section,const FXchar *name,FXuint val){
  if(!section || !section[0]){ fxerror("FXSettings::writeUnsignedEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::writeUnsignedEntry: bad name argument.\n"); }
  FXStringDict *group=insert(section);
  if(group){
    FXchar buffer[32];
    __snprintf(buffer,sizeof(buffer),"%u",val);
    group->replace(name,buffer,true);
    modified=true;
    return true;
    }
  return false;
  }


// Write a 64-bit long integer registry entry
FXbool FXSettings::writeLongEntry(const FXchar *section,const FXchar *name,FXlong val){
  if(!section || !section[0]){ fxerror("FXSettings::writeLongEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::writeLongEntry: bad name argument.\n"); }
  FXStringDict *group=insert(section);
  if(group){
    FXchar buffer[64];
    __snprintf(buffer,sizeof(buffer),"%lld",val);
    group->replace(name,buffer,true);
    modified=true;
    return true;
    }
  return false;
  }


// Write a 64-bit long integer registry entry
FXbool FXSettings::writeULongEntry(const FXchar *section,const FXchar *name,FXulong val){
  if(!section || !section[0]){ fxerror("FXSettings::writeULongEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::writeULongEntry: bad name argument.\n"); }
  FXStringDict *group=insert(section);
  if(group){
    FXchar buffer[64];
    __snprintf(buffer,sizeof(buffer),"%llu",val);
    group->replace(name,buffer,true);
    modified=true;
    return true;
    }
  return false;
  }


// Write a double-valued registry entry
FXbool FXSettings::writeRealEntry(const FXchar *section,const FXchar *name,FXdouble val){
  if(!section || !section[0]){ fxerror("FXSettings::writeRealEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::writeRealEntry: bad name argument.\n"); }
  FXStringDict *group=insert(section);
  if(group){
    FXchar buffer[64];
    __snprintf(buffer,sizeof(buffer),"%.16g",val);
    group->replace(name,buffer,true);
    modified=true;
    return true;
    }
  return false;
  }


// Write a color registry entry
FXbool FXSettings::writeColorEntry(const FXchar *section,const FXchar *name,FXColor val){
  if(!section || !section[0]){ fxerror("FXSettings::writeColorEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::writeColorEntry: bad name argument.\n"); }
  FXStringDict *group=insert(section);
  if(group){
    FXchar buffer[64];
    group->replace(name,fxnamefromcolor(buffer,val),true);
    modified=true;
    return true;
    }
  return false;
  }


// Write a boolean registry entry
FXbool FXSettings::writeBoolEntry(const FXchar *section,const FXchar *name,FXbool val){
  if(!section || !section[0]){ fxerror("FXSettings::writeBoolEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::writeBoolEntry: bad name argument.\n"); }
  FXStringDict *group=insert(section);
  if(group){
    group->replace(name,val?"true":"false",true);
    modified=true;
    return true;
    }
  return false;
  }


// Delete a registry entry
FXbool FXSettings::deleteEntry(const FXchar *section,const FXchar *name){
  if(!section || !section[0]){ fxerror("FXSettings::deleteEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::deleteEntry: bad name argument.\n"); }
  FXStringDict *group=insert(section);
  if(group){
    group->remove(name);
    modified=true;
    return true;
    }
  return false;
  }


// Delete section
FXbool FXSettings::deleteSection(const FXchar *section){
  if(!section || !section[0]){ fxerror("FXSettings::deleteSection: bad section argument.\n"); }
  remove(section);
  modified=true;
  return true;
  }


// Clear all sections
FXbool FXSettings::clear(){
  FXDict::clear();
  modified=true;
  return true;
  }


// See if section exists
FXbool FXSettings::existingSection(const FXchar *section){
  if(!section || !section[0]){ fxerror("FXSettings::existingSection: bad section argument.\n"); }
  return find(section)!=NULL;
  }


// See if entry exists
FXbool FXSettings::existingEntry(const FXchar *section,const FXchar *name){
  if(!section || !section[0]){ fxerror("FXSettings::existingEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::existingEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  return group && group->find(name)!=NULL;
  }


// Clean up
FXSettings::~FXSettings(){
  clear();
  }

}

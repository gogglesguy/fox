/********************************************************************************
*                                                                               *
*                           S e t t i n g s   C l a s s                         *
*                                                                               *
*********************************************************************************
* Copyright (C) 1998,2009 by Jeroen van der Zijp.   All Rights Reserved.        *
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
#include "fxascii.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXColors.h"
#include "FXStringDict.h"
#include "FXFile.h"
#include "FXSettings.h"

/*
  Notes:

  - Format for settings database file:

    [SectionKey]
    EntryKey=string-with-no-spaces
    EntryKey="string\nwith a\nnewline in it\n"
    EntryKey=" string with leading and trailing spaces and \"embedded\" in it  "
    EntryKey=string with no leading or trailing spaces

  - EntryKey may is of the form "ali baba", "ali-baba", "ali_baba", or "ali.baba".

  - Leading/trailing spaces are NOT part of the EntryKey.

  - Escape sequences now allow octal (\377) as well as hex (\xff) codes.

  - EntryKey format should be like values.

  - Extensive error checking in unparseFile() to ensure no settings data is
    lost when disk is full.
    
  - FIXME only writeFormatEntry() still has arbitrary limits.
*/


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
  modified=orig.modified;
  for(FXint i=0; i<orig.total; i++){
    if(0<=dict[i].hash){
      dict[i].data=new FXStringDict(*((FXStringDict*)orig.dict[i].data));
      }
    }
  }


// Assignment operator
FXSettings& FXSettings::operator=(const FXSettings& orig){
  if(&orig!=this){
    FXDict::operator=(orig);
    for(FXint i=0; i<orig.total; i++){
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


// Parse filename
FXbool FXSettings::parseFile(const FXString& filename,FXbool mrk){
  FXFile file(filename,FXIO::Reading);
  if(file.isOpen()){

    // Prepare buffer string
    FXString string('\0',file.size());

    // Load file
    if(file.readBlock(string.text(),string.length())==string.length()){
      FXStringDict *group=NULL;
      FXint lineno=1,p=0,b,e;
      FXString name;
      FXString value;

      // Skip BOM, if any
      if(string[p]=='\xef' && string[p+1]=='\xbb' && string[p+2]=='\xbf') p+=3;

      // Parse one line at a time
      while(string[p]){

        // Skip leading blanks
        while(Ascii::isBlank(string[p])) p++;

        // Non-comment
        if(string[p] && string[p]!='\n' && string[p]!='\r' && string[p]!='#' && string[p]!=';'){

          // Parse section name
          if(string[p]=='['){

            b=++p;

            // Scan section name
            while(string[p] && string[p]!=']' && string[p]!='\n' && string[p]!='\r' && !Ascii::isControl(string[p])) p++;

            // Check errors
            if(string[p]!=']'){ fxwarning("%s:%d: illegal section name.\n",filename.text(),lineno); goto next; }

            e=p++;

            // Grab name
            name=string.mid(b,e-b);

            // Add new section dict
            group=insert(name.text());
            }

          // Parse name-value pair
          else{

            // Should have seen section prior to this
            if(!group){ fxwarning("%s:%d: settings entry should follow a section.\n",filename.text(),lineno); goto next; }

            b=p;

            // Scan key name
            while(string[p] && string[p]!='=' && string[p]!='\n' && string[p]!='\r' && !Ascii::isControl(string[p])) p++;

            // Check errors
            if(string[p]!='='){ fxwarning("%s:%d: expected '=' to follow key.\n",filename.text(),lineno); goto next; }

            e=p++;

            // Remove trailing spaces after name
            while(b<e && Ascii::isBlank(string[e-1])) e--;

            // Grab name
            name=string.mid(b,e-b);

            // Skip leading spaces
            while(Ascii::isBlank(string[p])) p++;

            // Mark value
            b=p;

            // Scan value
            while(string[p] && string[p]!='\n' && string[p]!='\r' && !Ascii::isControl(string[p])) p++;

            e=p;

            // Remove trailing spaces after value
            while(b<e && Ascii::isBlank(string[e-1])) e--;

            // Grab the unescaped value
            value=string.mid(b,e-b).unescape('"','"');

            // Add entry to current section
            group->replace(name.text(),value.text(),mrk);
            }
          }

        // Skip to end of line
next:   while(string[p] && string[p]!='\n') p++;

        // End of line
        if(string[p]=='\n'){
          lineno++;
          p++;
          }
        }
      return true;
      }
    }
  return false;
  }


// Unparse registry file
FXbool FXSettings::unparseFile(const FXString& filename){
  FXFile file(filename,FXIO::Writing);
  if(file.isOpen()){
    FXString string;

    // Loop over all sections
    for(FXint s=first(); s<size(); s=next(s)){

      // Get group
      FXStringDict* group=data(s);
      FXbool sec=false;

      // Loop over all entries
      for(FXint e=group->first(); e<group->size(); e=group->next(e)){

        // Is key-value pair marked?
        if(group->mark(e)){
          FXString value=group->data(e);

          // Write section name if not written yet
          if(!sec){
            string.append("[");
            string.append(key(s));
            string.append("]" ENDLINE);
            sec=true;
            }

          // Write marked key-value pairs only
          string.append(group->key(e));
          string.append("=");
          string.append(value.shouldEscape('"','"') ? value.escape('"','"') : value);
          string.append(ENDLINE);
          }
        }

      // Blank line after end
      if(sec){
        string.append(ENDLINE);
        }
      }

    // Save file
    if(file.writeBlock(string.text(),string.length())==string.length()){
      return true;
      }
    }
  return false;
  }


// Read a formatted registry entry
FXint FXSettings::readFormatEntry(const FXchar *section,const FXchar *name,const FXchar *fmt,...) const {
  FXStringDict *group=find(section);
  FXint result=0;
  if(group){
    const FXchar *value=group->find(name);
    if(value){
      va_list args;
      va_start(args,fmt);
      result=__vsscanf(value,fmt,args);
      va_end(args);
      }
    }
  return result;
  }


// Read a formatted registry entry
FXint FXSettings::readFormatEntry(const FXString& section,const FXchar *name,const FXchar *fmt,...) const {
  FXStringDict *group=find(section.text());
  FXint result=0;
  if(group){
    const FXchar *value=group->find(name);
    if(value){
      va_list args;
      va_start(args,fmt);
      result=__vsscanf(value,fmt,args);
      va_end(args);
      }
    }
  return result;
  }


// Read a formatted registry entry
FXint FXSettings::readFormatEntry(const FXString& section,const FXString& name,const FXchar *fmt,...) const {
  FXStringDict *group=find(section.text());
  FXint result=0;
  if(group){
    const FXchar *value=group->find(name.text());
    if(value){
      va_list args;
      va_start(args,fmt);
      result=__vsscanf(value,fmt,args);
      va_end(args);
      }
    }
  return result;
  }


// Write a formatted registry entry
FXint FXSettings::writeFormatEntry(const FXchar *section,const FXchar *name,const FXchar *fmt,...){
  FXStringDict *group=insert(section);
  FXint result=0;
  if(group){
    va_list args;
    va_start(args,fmt);
    FXchar buffer[2048];                // FIXME
    result=__vsnprintf(buffer,sizeof(buffer),fmt,args);
    group->replace(name,buffer,true);
    modified=true;
    va_end(args);
    }
  return result;
  }


// Write a formatted registry entry
FXint FXSettings::writeFormatEntry(const FXString& section,const FXchar *name,const FXchar *fmt,...){
  FXStringDict *group=insert(section.text());
  FXint result=0;
  if(group){
    va_list args;
    va_start(args,fmt);
    FXchar buffer[2048];                // FIXME
    result=__vsnprintf(buffer,sizeof(buffer),fmt,args);
    group->replace(name,buffer,true);
    modified=true;
    va_end(args);
    }
  return result;
  }


// Write a formatted registry entry
FXint FXSettings::writeFormatEntry(const FXString& section,const FXString& name,const FXchar *fmt,...){
  FXStringDict *group=insert(section.text());
  FXint result=0;
  if(group){
    va_list args;
    va_start(args,fmt);
    FXchar buffer[2048];                // FIXME
    result=__vsnprintf(buffer,sizeof(buffer),fmt,args);
    group->replace(name.text(),buffer,true);
    modified=true;
    va_end(args);
    }
  return result;
  }


// Read a string-valued registry entry
const FXchar *FXSettings::readStringEntry(const FXchar *section,const FXchar *name,const FXchar *def) const {
  if(!section || !section[0]){ fxerror("FXSettings::readStringEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readStringEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const FXchar *value=group->find(name);
    if(value) return value;
    }
  return def;
  }


// Read a string-valued registry entry
const FXchar *FXSettings::readStringEntry(const FXString& section,const FXchar *name,const FXchar *def) const {
  return readStringEntry(section.text(),name,def);
  }


// Read a string-valued registry entry
const FXchar *FXSettings::readStringEntry(const FXString& section,const FXString& name,const FXchar *def) const {
  return readStringEntry(section.text(),name.text(),def);
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


// Write a string-valued registry entry
FXbool FXSettings::writeStringEntry(const FXString& section,const FXchar *name,const FXchar *val){
  return writeStringEntry(section.text(),name,val);
  }


// Write a string-valued registry entry
FXbool FXSettings::writeStringEntry(const FXString& section,const FXString& name,const FXchar *val){
  return writeStringEntry(section.text(),name.text(),val);
  }


// Read a int-valued registry entry
FXint FXSettings::readIntEntry(const FXchar *section,const FXchar *name,FXint def) const {
  if(!section || !section[0]){ fxerror("FXSettings::readIntEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readIntEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const FXchar *value=group->find(name);
    if(value){
      FXint ivalue;
      if(__sscanf(value,"%d",&ivalue)==1) return ivalue;
      }
    }
  return def;
  }


// Read a int-valued registry entry
FXint FXSettings::readIntEntry(const FXString& section,const FXchar *name,FXint def) const {
  return readIntEntry(section.text(),name,def);
  }


// Read a int-valued registry entry
FXint FXSettings::readIntEntry(const FXString& section,const FXString& name,FXint def) const {
  return readIntEntry(section.text(),name.text(),def);
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


// Write a int-valued registry entry
FXbool FXSettings::writeIntEntry(const FXString& section,const FXchar *name,FXint val){
  return writeIntEntry(section.text(),name,val);
  }


// Write a int-valued registry entry
FXbool FXSettings::writeIntEntry(const FXString& section,const FXString& name,FXint val){
  return writeIntEntry(section.text(),name.text(),val);
  }


// Read a unsigned int-valued registry entry
FXuint FXSettings::readUIntEntry(const FXchar *section,const FXchar *name,FXuint def) const {
  if(!section || !section[0]){ fxerror("FXSettings::readUnsignedEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readUnsignedEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const FXchar *value=group->find(name);
    if(value){
      FXuint ivalue;
      if(__sscanf(value,"%u",&ivalue)==1) return ivalue;
      }
    }
  return def;
  }


// Read a unsigned int-valued registry entry
FXuint FXSettings::readUIntEntry(const FXString& section,const FXchar *name,FXuint def) const {
  return readUIntEntry(section.text(),name,def);
  }


// Read a unsigned int-valued registry entry
FXuint FXSettings::readUIntEntry(const FXString& section,const FXString& name,FXuint def) const {
  return readUIntEntry(section.text(),name.text(),def);
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


// Write a unsigned int-valued registry entry
FXbool FXSettings::writeUIntEntry(const FXString& section,const FXchar *name,FXuint val){
  return writeUIntEntry(section.text(),name,val);
  }


// Write a unsigned int-valued registry entry
FXbool FXSettings::writeUIntEntry(const FXString& section,const FXString& name,FXuint val){
  return writeUIntEntry(section.text(),name.text(),val);
  }


// Read a 64-bit long integer registry entry
FXlong FXSettings::readLongEntry(const FXchar *section,const FXchar *name,FXlong def) const {
  if(!section || !section[0]){ fxerror("FXSettings::readLongEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readLongEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const FXchar *value=group->find(name);
    if(value){
      FXlong lvalue;
      if(__sscanf(value,"%lld",&lvalue)==1) return lvalue;
      }
    }
  return def;
  }


// Read a 64-bit long integer registry entry
FXlong FXSettings::readLongEntry(const FXString& section,const FXchar *name,FXlong def) const {
  return readLongEntry(section.text(),name,def);
  }


// Read a 64-bit long integer registry entry
FXlong FXSettings::readLongEntry(const FXString& section,const FXString& name,FXlong def) const {
  return readLongEntry(section.text(),name.text(),def);
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
FXbool FXSettings::writeLongEntry(const FXString& section,const FXchar *name,FXlong val){
  return writeLongEntry(section.text(),name,val);
  }


// Write a 64-bit long integer registry entry
FXbool FXSettings::writeLongEntry(const FXString& section,const FXString& name,FXlong val){
  return writeLongEntry(section.text(),name.text(),val);
  }


// Read a 64-bit unsigned long integer registry entry
FXulong FXSettings::readULongEntry(const FXchar *section,const FXchar *name,FXulong def) const {
  if(!section || !section[0]){ fxerror("FXSettings::readULongEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readULongEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const FXchar *value=group->find(name);
    if(value){
      FXulong lvalue;
      if(value[0]=='0' && (value[1]=='x' || value[1]=='X')){
        if(__sscanf(value,"%llu",&lvalue)==1) return lvalue;
        }
      }
    }
  return def;
  }


// Read a 64-bit unsigned long integer registry entry
FXulong FXSettings::readULongEntry(const FXString& section,const FXchar *name,FXulong def) const {
  return readULongEntry(section.text(),name,def);
  }


// Read a 64-bit unsigned long integer registry entry
FXulong FXSettings::readULongEntry(const FXString& section,const FXString& name,FXulong def) const {
  return readULongEntry(section.text(),name.text(),def);
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


// Write a 64-bit long integer registry entry
FXbool FXSettings::writeULongEntry(const FXString& section,const FXchar *name,FXulong val){
  return writeULongEntry(section.text(),name,val);
  }


// Write a 64-bit long integer registry entry
FXbool FXSettings::writeULongEntry(const FXString& section,const FXString& name,FXulong val){
  return writeULongEntry(section.text(),name.text(),val);
  }


// Read a double-valued registry entry
FXdouble FXSettings::readRealEntry(const FXchar *section,const FXchar *name,FXdouble def) const {
  if(!section || !section[0]){ fxerror("FXSettings::readRealEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readRealEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const FXchar *value=group->find(name);
    if(value){
      FXdouble dvalue;
      if(__sscanf(value,"%lf",&dvalue)==1) return dvalue;
      }
    }
  return def;
  }


// Read a double-valued registry entry
FXdouble FXSettings::readRealEntry(const FXString& section,const FXchar *name,FXdouble def) const {
  return readRealEntry(section.text(),name,def);
  }


// Read a double-valued registry entry
FXdouble FXSettings::readRealEntry(const FXString& section,const FXString& name,FXdouble def) const {
  return readRealEntry(section.text(),name.text(),def);
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


// Write a double-valued registry entry
FXbool FXSettings::writeRealEntry(const FXString& section,const FXchar *name,FXdouble val){
  return writeRealEntry(section.text(),name,val);
  }


// Write a double-valued registry entry
FXbool FXSettings::writeRealEntry(const FXString& section,const FXString& name,FXdouble val){
  return writeRealEntry(section.text(),name.text(),val);
  }


// Read a color registry entry
FXColor FXSettings::readColorEntry(const FXchar *section,const FXchar *name,FXColor def) const {
  if(!section || !section[0]){ fxerror("FXSettings::readColorEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readColorEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const FXchar *value=group->find(name);
    if(value){
      return colorFromName(value);
      }
    }
  return def;
  }


// Read a color registry entry
FXColor FXSettings::readColorEntry(const FXString& section,const FXchar *name,FXColor def) const {
  return readColorEntry(section.text(),name,def);
  }


// Read a color registry entry
FXColor FXSettings::readColorEntry(const FXString& section,const FXString& name,FXColor def) const {
  return readColorEntry(section.text(),name.text(),def);
  }


// Write a color registry entry
FXbool FXSettings::writeColorEntry(const FXchar *section,const FXchar *name,FXColor val){
  if(!section || !section[0]){ fxerror("FXSettings::writeColorEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::writeColorEntry: bad name argument.\n"); }
  FXStringDict *group=insert(section);
  if(group){
    FXchar buffer[64];
    group->replace(name,nameFromColor(buffer,val),true);
    modified=true;
    return true;
    }
  return false;
  }


// Write a color registry entry
FXbool FXSettings::writeColorEntry(const FXString& section,const FXchar *name,FXColor val){
  return writeColorEntry(section.text(),name,val);
  }


// Write a color registry entry
FXbool FXSettings::writeColorEntry(const FXString& section,const FXString& name,FXColor val){
  return writeColorEntry(section.text(),name.text(),val);
  }


// Read a boolean registry entry
FXbool FXSettings::readBoolEntry(const FXchar *section,const FXchar *name,FXbool def) const {
  if(!section || !section[0]){ fxerror("FXSettings::readBoolEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::readBoolEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    const FXchar *value=group->find(name);
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


// Read a boolean registry entry
FXbool FXSettings::readBoolEntry(const FXString& section,const FXchar *name,FXbool def) const {
  return readBoolEntry(section.text(),name,def);
  }


// Read a boolean registry entry
FXbool FXSettings::readBoolEntry(const FXString& section,const FXString& name,FXbool def) const {
  return readBoolEntry(section.text(),name.text(),def);
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


// Write a boolean registry entry
FXbool FXSettings::writeBoolEntry(const FXString& section,const FXchar *name,FXbool val){
  return writeBoolEntry(section.text(),name,val);
  }


// Write a boolean registry entry
FXbool FXSettings::writeBoolEntry(const FXString& section,const FXString& name,FXbool val){
  return writeBoolEntry(section.text(),name.text(),val);
  }


// See if entry exists
FXbool FXSettings::existingEntry(const FXchar *section,const FXchar *name) const {
  if(!section || !section[0]){ fxerror("FXSettings::existingEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::existingEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  return group && group->find(name)!=NULL;
  }


// See if entry exists
FXbool FXSettings::existingEntry(const FXString& section,const FXchar *name) const {
  return existingEntry(section.text(),name);
  }


// See if entry exists
FXbool FXSettings::existingEntry(const FXString& section,const FXString& name) const {
  return existingEntry(section.text(),name.text());
  }


// Delete a registry entry
FXbool FXSettings::deleteEntry(const FXchar *section,const FXchar *name){
  if(!section || !section[0]){ fxerror("FXSettings::deleteEntry: bad section argument.\n"); }
  if(!name || !name[0]){ fxerror("FXSettings::deleteEntry: bad name argument.\n"); }
  FXStringDict *group=find(section);
  if(group){
    group->remove(name);
    modified=true;
    return true;
    }
  return false;
  }


// Delete a registry entry
FXbool FXSettings::deleteEntry(const FXString& section,const FXchar *name){
  return deleteEntry(section.text(),name);
  }


// Delete a registry entry
FXbool FXSettings::deleteEntry(const FXString& section,const FXString& name){
  return deleteEntry(section.text(),name.text());
  }


// See if section exists
FXbool FXSettings::existingSection(const FXchar *section) const {
  if(!section || !section[0]){ fxerror("FXSettings::existingSection: bad section argument.\n"); }
  return find(section)!=NULL;
  }


// See if section exists
FXbool FXSettings::existingSection(const FXString& section) const {
  return existingSection(section.text());
  }


// Delete section
FXbool FXSettings::deleteSection(const FXchar *section){
  if(!section || !section[0]){ fxerror("FXSettings::deleteSection: bad section argument.\n"); }
  remove(section);
  modified=true;
  return true;
  }


// Delete section
FXbool FXSettings::deleteSection(const FXString& section){
  return deleteSection(section.text());
  }


// Clear all sections
FXbool FXSettings::clear(){
  FXDict::clear();
  modified=true;
  return true;
  }


// Clean up
FXSettings::~FXSettings(){
  clear();
  }

}

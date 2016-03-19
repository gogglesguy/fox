/********************************************************************************
*                                                                               *
*                L o a d   I c o n   F r o m   E x e c u t a b l e              *
*                                                                               *
*********************************************************************************
* Copyright (C) 2014 by Jeroen van der Zijp.   All Rights Reserved.             *
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
#include "FXArray.h"
#include "FXHash.h"
#include "FXElement.h"
#include "FXStream.h"


/*
  Notes:
  - Load icon from Windows Executable (or EXE or DLL).
  - Only works on Windows.
*/


using namespace FX;

/*******************************************************************************/

namespace FX {


#if defined(WIN32)
extern FXAPI FXbool fxcheckEXE(FXStream& store);
extern FXAPI FXbool fxloadEXE(FXStream& store,FXColor*& data,FXint& width,FXint& height);
#endif


#if defined(WIN32)

/*******************************************************************************/
#if 0  

#pragma pack( push )
#pragma pack( 2 )

typedef struct {
  BYTE bWidth;
  BYTE bHeight;
  BYTE bColorCount;
  BYTE bReserved;
  WORD wPlanes;
  WORD wBitCount;
  DWORD dwBytesInRes;
  WORD nID;
  } MEMICONDIRENTRY, *LPMEMICONDIRENTRY;

typedef struct {
  WORD idReserved;
  WORD idType;
  WORD idCount;
  MEMICONDIRENTRY idEntries[1];
  } MEMICONDIR, *LPMEMICONDIR;

#pragma pack( pop )

typedef struct {
  BYTE bWidth;
  BYTE bHeight;
  BYTE bColorCount;
  BYTE bReserved;
  WORD wPlanes;
  WORD wBitCount;
  DWORD dwBytesInRes;
  DWORD dwImageOffset;
  } ICONDIRENTRY, *LPICONDIRENTRY;
  
  
static char *arg_output;
static int arg_verbose = FALSE;
static int count;

#define prgname "extrico"

char *
basename_no_extension(filename)
  char *filename;
{
	char *str, *str2;
	int len;

	str = strrchr(filename, '\\');
	if (str && str[1]) {
		str = &str[1];
	} else {
		str = strrchr(filename, ':');
		if (str && str[1])
			str = &str[1];
		else
			str = filename;
	}

	str2 = strrchr(str, '.');
	if (str2)
		len = str2 - str + 1;
	else
		len = strlen(str) + 1;

	str2 = (char *) malloc(len);
	if (str2) {
		memcpy(str2, str, len-1);
		str2[len] = 0;
		return str2;
	}

	return str;
}

int
xwrite(file, data, size)
  HANDLE file;
  void *data;
  DWORD size;
{
	DWORD written_count;

	if (!WriteFile(file, data, size, &written_count, NULL)) {
		fprintf(stderr, "%s: %s\n", prgname, errorstr());
        return FALSE;
    }

    if(written_count != size) {
		fprintf(stderr, "%s: Could not write all data.\n", prgname);
        return FALSE;
	}

	return TRUE;
}

int
write_icon_to_file (library, icon, filename)
  HANDLE library;
  LPMEMICONDIR icon;
  LPCTSTR filename;
{
	HANDLE file;
	WORD output;
	int c;

	/* open file */
	file = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
	  FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "%s: %s: %s\n", prgname, filename, errorstr());
		return FALSE;
	}

	/* write icon file header */
	output = 0;	/* reserved */
	if (!xwrite(file, &output, sizeof(WORD))) {
		CloseHandle(file);
		return FALSE;
	}
    output = 1;	/* type */
	if (!xwrite(file, &output, sizeof(WORD))) {
		CloseHandle(file);
		return FALSE;
	}
	output = icon->idCount;	/* number of images */
	if (!xwrite(file, &output, sizeof(WORD))) {
		CloseHandle(file);
		return FALSE;
	}

 	/* write ICONDIRENTRY for each image */
	for (c=0; c < icon->idCount; c++) {
        int d;
        ICONDIRENTRY icon_dir_entry;

		/* convert to ICONDIRENTRY */
		icon_dir_entry.bWidth = icon->idEntries[c].bWidth;
		icon_dir_entry.bHeight = icon->idEntries[c].bHeight;
		icon_dir_entry.bColorCount = icon->idEntries[c].bColorCount;
		icon_dir_entry.bReserved = 0;
		icon_dir_entry.wPlanes = icon->idEntries[c].wPlanes;
		icon_dir_entry.wBitCount = icon->idEntries[c].wBitCount;
		icon_dir_entry.dwBytesInRes = icon->idEntries[c].dwBytesInRes;

		/* calculate image offset */
		icon_dir_entry.dwImageOffset = 3*sizeof(WORD) + icon->idCount*sizeof(ICONDIRENTRY);
		for (d=0; d < c; d++)
			icon_dir_entry.dwImageOffset += icon->idEntries[d].dwBytesInRes;

		/* output the ICONDIRENTRY */
		if (!xwrite(file, &icon_dir_entry, sizeof(ICONDIRENTRY))) {
			CloseHandle(file);
			return FALSE;
		}
	}

	/* write image data bits for each image */
	for (c=0; c < icon->idCount; c++) {
		HRSRC resource;
		HGLOBAL memory;

		/* find the separate icon resource */
		resource = FindResource (library, MAKEINTRESOURCE(icon->idEntries[c].nID), RT_ICON);
		if (!resource) {
			fprintf(stderr, "%s: %s\n", prgname, errorstr());
			CloseHandle(file);
			return FALSE;
		}
		/* load the resource */
		memory = LoadResource (library, resource);
		if (!memory) {
			fprintf(stderr, "%s: %s\n", prgname, errorstr());
			CloseHandle(file);
			return FALSE;
		}

		/* output the image data */
		if (!xwrite(file, LockResource(memory), SizeofResource(library, resource))) {
			CloseHandle(file);
			return FALSE;
		}
	}

	CloseHandle(file);

	return TRUE;
}

BOOL CALLBACK resource_enum (module, type, name, param)
  HANDLE module;
  LPCTSTR type;
  LPTSTR name;
  LONG param;
{
	char *filename;
	HRSRC resource;
	HGLOBAL memory;
	LPMEMICONDIR icon;
	char dest_filename[1000];

	filename = (char *) param;

    /* if high word of name is zero, name is from MAKEINTRESOURCE() */
	if (!HIWORD(name))
		sprintf(dest_filename, "%s%s-%d.ico", (arg_output ? arg_output : ""), basename_no_extension(filename), name);
    else
		sprintf(dest_filename, "%s%s-%s.ico", (arg_output ? arg_output : ""), basename_no_extension(filename), name);

	/* find this resource */
	resource = FindResource(module, name, type);
	if (!resource) {
		fprintf(stderr, "%s: %s\n", prgname, errorstr());
		return TRUE;
	}
	/* load the resource */
	memory = LoadResource(module, resource);
	if (!resource) {
		fprintf(stderr, "%s: %s\n", prgname, errorstr());
		return TRUE;
	}
	/* lock the resource */
	icon = LockResource(memory);
	if (!icon) {
		fprintf(stderr, "%s: %s\n", prgname, errorstr());
		return TRUE;
	}

	if (arg_verbose)
		printf("%s [%d] => %s\n", filename, count, dest_filename);
	count++;

	/* write icon to file */
	if (!write_icon_to_file(module, icon, dest_filename)) {
		return TRUE;
	}

	return TRUE;
}

int
extract_icons_from_library(filename)
  char *filename;
{
	HINSTANCE library;

	/* load the DLL/EXE file */
	library = LoadLibraryEx(filename, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if (!library) {
		fprintf(stderr, "%s: %s: %s\n", prgname, filename, errorstr());
		return 1;
	}

	/* list all icons in file */
	if (!EnumResourceNames(library, RT_GROUP_ICON, resource_enum, (LONG) filename)) {
		fprintf(stderr, "%s: %s\n", prgname, errorstr());
		FreeLibrary (library);
		return 1;
	}

	/* free library */
	if (!FreeLibrary (library)) {
		fprintf(stderr, "%s: %s\n", prgname, errorstr());
	}
}

#endif

/*******************************************************************************/


// Check if stream contains a JPG
FXbool fxcheckEXE(FXStream& store){
  FXuchar ss[12];
  store.load(ss,12);
  store.position(-12,FXFromCurrent);
  // FIXME
  return false;
  }


// Load resource from executable or dll
FXbool fxloadEXE(FXStream& store,FXColor*& data,FXint& width,FXint& height){

  // Null out
  data=NULL;
  width=0;
  height=0;

  return false;
  }

/*******************************************************************************/

#else

// Check if stream contains EXE or DLL
FXbool fxcheckEXE(FXStream&){
  return false;
  }


// Stub routine returns placeholder bitmap
FXbool fxloadEXE(FXStream&,FXColor*& data,FXint& width,FXint& height){
  static const FXuchar image_bits[]={
   0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x80, 0xfd, 0xff, 0xff, 0xbf,
   0x05, 0x00, 0x00, 0xa0, 0x05, 0x00, 0x00, 0xa0, 0x05, 0x00, 0x00, 0xa0,
   0x05, 0x00, 0x00, 0xa0, 0x05, 0x00, 0x00, 0xa0, 0x05, 0x00, 0x00, 0xa0,
   0x05, 0x00, 0x00, 0xa0, 0x05, 0x00, 0x00, 0xa0, 0x05, 0x00, 0x00, 0xa0,
   0x05, 0x00, 0x00, 0xa0, 0x05, 0x00, 0x00, 0xa0, 0x05, 0x00, 0x00, 0xa0,
   0x05, 0x00, 0x00, 0xa0, 0x05, 0x00, 0x00, 0xa0, 0xf5, 0x33, 0xe6, 0xa7,
   0x25, 0x22, 0x42, 0xa4, 0x25, 0x40, 0x41, 0xa0, 0xa5, 0x40, 0x41, 0xa1,
   0xe5, 0x80, 0xc0, 0xa1, 0xa5, 0x40, 0x41, 0xa1, 0x25, 0x40, 0x41, 0xa0,
   0x25, 0x22, 0x42, 0xa4, 0xf5, 0x33, 0xe6, 0xa7, 0x05, 0x00, 0x00, 0xa0,
   0x05, 0x00, 0x00, 0xa0, 0x05, 0x00, 0x00, 0xa0, 0xfd, 0xff, 0xff, 0xbf,
   0x01, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff};
  register FXint p;
  allocElms(data,32*32);
  for(p=0; p<32*32; p++){
    data[p]=(image_bits[p>>3]&(1<<(p&7))) ? FXRGB(0,0,0) : FXRGB(255,255,255);
    }
  width=32;
  height=32;
  return true;
  }


#endif

}

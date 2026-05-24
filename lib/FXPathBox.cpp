/********************************************************************************
*                                                                               *
*                       P a t h   B o x   W i d g e t                           *
*                                                                               *
*********************************************************************************
* Copyright (C) 2025 by Jeroen van der Zijp.   All Rights Reserved.             *
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
#include "fxkeys.h"
#include "FXMutex.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXElement.h"
#include "FXMetaClass.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXPath.h"
#include "FXIO.h"
#include "FXStat.h"
#include "FXFile.h"
#include "FXDir.h"
#include "FXStringDictionary.h"
#include "FXSettings.h"
#include "FXRegistry.h"
#include "FXAccelTable.h"
#include "FXObjectList.h"
#include "FXFont.h"
#include "FXEvent.h"
#include "FXWindow.h"
#include "FXDCWindow.h"
#include "FXApp.h"
#include "FXImage.h"
#include "FXIcon.h"
#include "FXGIFIcon.h"
#include "FXBMPIcon.h"
#include "FXFrame.h"
#include "FXComposite.h"
#include "FXPathBox.h"
#include "FXDictionary.h"
#include "FXDictionaryOf.h"
#include "FXIconCache.h"
#include "FXFileAssociations.h"
#include "FXShell.h"
#include "FXPopup.h"
#include "FXMenuCaption.h"
#include "FXMenuCommand.h"
#include "icons.h"

/*
  Notes:
  - When setting path, it adds all directories from the top down to
    the lowest directory.
  - When switching from one path to the next, keep as much of the old path
    as possible; remove and create minimum number of items.
  - Thus, if path is same as old path, there's no change in the items.
  - Share icons with other widgets; upgrade icons to some nicer ones.
  - Should some of these icons move to FXFileAssociations?
  - Need to support ":" directory list separator so we can path not just
    a single path but a list of paths.
*/

#define TOPIC_CONSTRUCT 1000
#define TOPIC_CREATION  1001
#define TOPIC_DETAIL    1002
#define TOPIC_DEBUG     1003


// Default locations and names
#if defined(WIN32)
#define INITAILPATH "C:\\"
#else
#define INITAILPATH "/"
#endif


using namespace FX;

/*******************************************************************************/

namespace FX {


// Spacing between icon and path components
enum { ICON_SPACING = 4, PATH_SPACING = 4};


// Map
FXDEFMAP(FXPathBox) FXPathBoxMap[]={
  FXMAPFUNC(SEL_PAINT,0,FXPathBox::onPaint),
  FXMAPFUNC(SEL_ENTER,0,FXPathBox::onEnter),
  FXMAPFUNC(SEL_LEAVE,0,FXPathBox::onLeave),
  FXMAPFUNC(SEL_MOTION,0,FXPathBox::onMotion),
  FXMAPFUNC(SEL_QUERY_TIP,0,FXPathBox::onQueryTip),
  FXMAPFUNC(SEL_QUERY_HELP,0,FXPathBox::onQueryHelp),
  FXMAPFUNC(SEL_LEFTBUTTONPRESS,0,FXPathBox::onLeftBtnPress),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,0,FXPathBox::onLeftBtnRelease),
  FXMAPFUNC(SEL_MIDDLEBUTTONPRESS,0,FXPathBox::onMiddleBtnPress),
  FXMAPFUNC(SEL_MIDDLEBUTTONRELEASE,0,FXPathBox::onMiddleBtnRelease),
  FXMAPFUNC(SEL_RIGHTBUTTONPRESS,0,FXPathBox::onRightBtnPress),
  FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,0,FXPathBox::onRightBtnRelease),
  FXMAPFUNC(SEL_TIMEOUT,FXPathBox::ID_TIPTIMER,FXPathBox::onTipTimer),
  FXMAPFUNC(SEL_COMMAND,FXPathBox::ID_SETVALUE,FXPathBox::onCmdSetValue),
  FXMAPFUNC(SEL_COMMAND,FXPathBox::ID_SETSTRINGVALUE,FXPathBox::onCmdSetStringValue),
  FXMAPFUNC(SEL_COMMAND,FXPathBox::ID_GETSTRINGVALUE,FXPathBox::onCmdGetStringValue),
  };


// Implementation
FXIMPLEMENT(FXPathBox,FXFrame,FXPathBoxMap,ARRAYNUMBER(FXPathBoxMap))


// Path box
FXPathBox::FXPathBox(){
  flags|=FLAG_ENABLED;
  font=(FXFont*)-1L;
  icon=(FXIcon*)-1L;
  associations=(FXFileAssociations*)-1L;
  foldericon=(FXIcon*)-1L;
  cdromicon=(FXIcon*)-1L;
  harddiskicon=(FXIcon*)-1L;
  netdriveicon=(FXIcon*)-1L;
  floppyicon=(FXIcon*)-1L;
  nethoodicon=(FXIcon*)-1L;
  zipdiskicon=(FXIcon*)-1L;
  hiliteBeg=0;
  hiliteEnd=0;
  selectBeg=0;
  selectEnd=0;
  textColor=0;
  selbackColor=0;
  seltextColor=0;
  columns=0;
  mode=MOUSE_NONE;
  }


// Path box
FXPathBox::FXPathBox(FXComposite *p,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h,FXint pl,FXint pr,FXint pt,FXint pb):FXFrame(p,opts,x,y,w,h,pl,pr,pt,pb),path(INITAILPATH){
  FXTRACE((TOPIC_CONSTRUCT,"FXPathBox::FXPathBox %p\n",this));
  flags|=FLAG_ENABLED;
  textColor=getApp()->getForeColor();
  backColor=getApp()->getBackColor();
  selbackColor=getApp()->getSelbackColor();
  seltextColor=getApp()->getSelforeColor();
  font=getApp()->getNormalFont();
  associations=nullptr;
  if(!(options&PATHBOX_NO_OWN_ASSOC)) associations=new FXFileAssociations(getApp());
  foldericon=new FXGIFIcon(getApp(),minifolder);
  cdromicon=new FXGIFIcon(getApp(),minicdrom);
  harddiskicon=new FXGIFIcon(getApp(),miniharddisk);
  netdriveicon=new FXGIFIcon(getApp(),mininetdrive);
  floppyicon=new FXGIFIcon(getApp(),minifloppy);
  nethoodicon=new FXGIFIcon(getApp(),mininethood);
  zipdiskicon=new FXGIFIcon(getApp(),minizipdrive);
  icon=foldericon;
  hiliteBeg=0;
  hiliteEnd=0;
  selectBeg=0;
  selectEnd=0;
  columns=1;
  target=tgt;
  message=sel;
  mode=MOUSE_NONE;
  }


// Create window
void FXPathBox::create(){
  FXTRACE((TOPIC_CREATION,"FXPathBox::create %p\n",this));
  FXFrame::create();
  font->create();
  FXASSERT(icon);
  icon->create();
  foldericon->create();
  cdromicon->create();
  harddiskicon->create();
  netdriveicon->create();
  floppyicon->create();
  nethoodicon->create();
  zipdiskicon->create();
  }


// Detach window
void FXPathBox::detach(){
  FXTRACE((TOPIC_CREATION,"FXPathBox::detach %p\n",this));
  FXFrame::detach();
  font->detach();
  foldericon->detach();
  cdromicon->detach();
  harddiskicon->detach();
  netdriveicon->detach();
  floppyicon->detach();
  nethoodicon->detach();
  zipdiskicon->detach();
  }


// Destroy window
void FXPathBox::destroy(){
  FXTRACE((TOPIC_CREATION,"FXPathBox::destroy %p\n",this));
  FXFrame::destroy();
  foldericon->destroy();
  cdromicon->destroy();
  harddiskicon->destroy();
  netdriveicon->destroy();
  floppyicon->destroy();
  nethoodicon->destroy();
  zipdiskicon->destroy();
  }


// Enable the window
void FXPathBox::enable(){
  if(!(flags&FLAG_ENABLED)){
    FXFrame::enable();
    update();
    }
  }


// Disable the window
void FXPathBox::disable(){
  if(flags&FLAG_ENABLED){
    FXFrame::disable();
    update();
    }
  }


// Get default width
FXint FXPathBox::getDefaultWidth(){
  return padleft+padright+(border<<1)+columns*font->getTextWidth("8",1)+icon->getWidth()+ICON_SPACING;
  }


// Get default height
FXint FXPathBox::getDefaultHeight(){
  return padtop+padbottom+(border<<1)+Math::imax(font->getFontHeight(),icon->getHeight());
  }


// Draw arrow
void FXPathBox::drawArrow(FXDCWindow& dc,FXint ix,FXint iy,FXint s) const {
  FXPoint points[3]={{(FXshort)ix,(FXshort)iy},{(FXshort)ix,(FXshort)(iy-s-s)},{(FXshort)(ix+s),(FXshort)(iy-s)}};
  dc.fillPolygon(points,3);
  }


// Find extent of the path component f..t
// Return code: 0=none, 1=text, 2=arrow, 3=icon, 4=dots
FXint FXPathBox::getPathComponent(FXint mx,FXint my,FXint& f,FXint& t) const {
  FXint ll=padleft+border;
  FXint rr=width-border-padright;
  FXint tt=border+padtop;
  FXint bb=height-border-padbottom;
  f=t=0;
  if(ll<=mx && mx<rr && tt<=my && my<bb){
    FXint dw=font->getTextWidth("...",3);
    FXint th=font->getFontHeight();
    FXint iw=icon->getWidth();
    FXint aw=(th-6)>>1;
    FXint tw,b,e;

    // Head of path
#if defined(WIN32)
    if(Ascii::isLetter(path[t]) && path[t+1]==':'){
      t+=2;
      if(ISPATHSEP(path[t])) t++;
      }
    else if(ISPATHSEP(path[t])){
      t++;
      if(ISPATHSEP(path[t])) t++;
      }
#else
    if(ISPATHSEP(path[t])) t++;
#endif

    // Got root?
    if(0<t){

      // Hit the icon?
      if(mx<ll+iw) return 3;
      ll+=iw+ICON_SPACING;
      if(mx<ll) return 0;

      // Hit path head?
      tw=font->getTextWidth(&path[0],t);
      if(mx<ll+tw) return 1;
      ll+=tw+PATH_SPACING;
      if(mx<ll) return 0;

      // Scan from back, we want to see the tail
      f=t;
      rr+=PATH_SPACING;
      for(t=b=e=path.length(); f<=b; e=--b){
        while(!ISPATHSEP(path[b-1])) --b;
        tw=font->getTextWidth(&path[b],e-b)+aw+PATH_SPACING+PATH_SPACING;
        if(rr-tw<=ll+dw+aw+PATH_SPACING+PATH_SPACING && (f<b || rr-tw<=ll)) break;
        rr-=tw;
        t=b;
        }

      // We need the dots
      if(f<t){
        if(mx<ll+aw) return 2;
        ll+=aw+PATH_SPACING;
        if(mx<ll) return 0;
        if(mx<ll+dw) return 4;
        ll+=dw+PATH_SPACING;
        if(mx<ll) return 0;
        }

      // Scan forward to find component
      while(t<path.length()){
        for(f=t; t<path.length() && !ISPATHSEP(path[t]); ++t){ }
        if(mx<ll+aw) return 2;
        ll+=aw+PATH_SPACING;
        if(mx<ll) return 0;
        tw=font->getTextWidth(&path[f],t-f);
        if(mx<ll+tw) return 1;
        ll+=tw+PATH_SPACING;
        if(ISPATHSEP(path[t])) t++;
        if(mx<ll) return 0;
        }
      f=t;
      }
    }
  return 0;
  }


// Select component in base from path
FXbool FXPathBox::selectPathComponent(const FXString& base,const FXString& path,FXint& f,FXint& t){
  if(!base.empty() && !path.empty()){
    FXint p=0;
    f=t=0;
#if defined(WIN32)

    // Drive letter same?
    if(Ascii::isLetter(path[0]) && path[1]==':'){
      if(Ascii::toUpper(path[0])!=Ascii::toUpper(base[0])) return false;
      if(base[1]!=':') return false;
      p=2;
      }

    // Server and share same?
    else if(ISPATHSEP(path[0]) && ISPATHSEP(path[1])){
      if(!ISPATHSEP(base[0])) return false;
      if(!ISPATHSEP(base[1])) return false;
      p=2;
      while(ISPATHCOMP(path[p])){
        if(Ascii::toLower(path[p])!=Ascii::toLower(base[p])) return false;
        p++;
        }
      if(ISPATHSEP(path[p])){
        if(!ISPATHSEP(base[p])) return false;
        p++;
        while(ISPATHCOMP(path[p])){
          if((Ascii::toLower(path[p])!=Ascii::toLower(base[p]))) return false;
          p++;
          }
        }
      }

    // Process case-insensitive path
    if(ISPATHSEP(path[p])){
      if(!ISPATHSEP(base[p])) return false;
      t=++p;
      }
    while(path[p]){
      f=p;
      while(ISPATHCOMP(path[p])){
        if(Ascii::toUpper(path[p])!=Ascii::toUpper(base[p])) return false;
        p++;
        }
      t=p;
      if(ISPATHSEP(path[p])){
        if(!ISPATHSEP(base[p])) return false;
        p++;
        }
      }
    return true;

#else

    // Process path
    if(ISPATHSEP(path[p])){
      if(!ISPATHSEP(base[p])) return false;
      t=++p;
      }
    while(path[p]){
      f=p;
      while(ISPATHCOMP(path[p])){
        if(path[p]!=base[p]) return false;
        p++;
        }
      t=p;
      if(ISPATHSEP(path[p])){
        if(!ISPATHSEP(base[p])) return false;
        p++;
        }
      }
    return true;
#endif
    }
  return false;
  }



// Handle repaint
long FXPathBox::onPaint(FXObject*,FXSelector,void* ptr){
  FXDCWindow dc(this,(FXEvent*)ptr);
  FXint rr=width-border-padright;
  FXint bb=height-border-padbottom;
  FXint ll=padleft+border;
  FXint tt=border+padtop;
  FXint th=font->getFontHeight();
  FXint dw=font->getTextWidth("...",3);
  FXint ih=icon->getHeight();
  FXint iw=icon->getWidth();
  FXint aw=(th-6)/2;
  FXint f,t,b,e,iy,ty,ww;

  // Draw frame
  drawFrame(dc,0,0,width,height);

  // Gray background if disabled
  dc.setForeground(isEnabled() ? backColor : baseColor);

  // Draw background
  dc.fillRectangle(border,border,width-(border<<1),height-(border<<1));

  // Head of path
  f=t=0;
#if defined(WIN32)
  if(Ascii::isLetter(path[t]) && path[t+1]==':'){
    t+=2;
    if(ISPATHSEP(path[t])) t++;
    }
  else if(ISPATHSEP(path[t])){
    t++;
    if(ISPATHSEP(path[t])) t++;
    }
#else
  if(ISPATHSEP(path[t])) t++;
#endif

  // Got root?
  if(0<t){

    // Set font
    dc.setFont(font);

    // Text color
    dc.setForeground(textColor);

    // Draw text, clipped against frame interior
    dc.setClipRectangle(border,border,width-(border<<1),height-(border<<1));

    // Observe vertical justification
    if(options&JUSTIFY_TOP){
      ty=tt;
      iy=tt;
      }
    else if(options&JUSTIFY_BOTTOM){
      ty=bb-th;
      iy=bb-ih;
      }
    else{
      ty=tt+(bb-tt-th)/2;
      iy=tt+(bb-tt-ih)/2;
      }
    ty+=font->getFontAscent();

    // Draw icon
    dc.drawIcon(icon,ll,iy);
    ll+=iw+ICON_SPACING;

    // Root '/'
    ww=font->getTextWidth(&path[0],t);
    if(selectBeg==f && selectEnd==t){
      dc.setForeground(selbackColor);
      dc.fillRectangle(ll,tt,ww,bb-tt);
      dc.setForeground(seltextColor);
      dc.drawText(ll,ty,&path[0],t);
      dc.setForeground(textColor);
      }
    else{
      dc.drawText(ll,ty,&path[0],t);
      }
    if(hiliteBeg==f && hiliteEnd==t){
      dc.drawFocusRectangle(ll-1,border+1,ww+2,height-border-border-2);
      }
    ll+=ww+PATH_SPACING;

    // Scan from back, we want to see the tail
    f=t;
    rr+=PATH_SPACING;
    for(t=b=e=path.length(); f<=b; e=--b){
      while(!ISPATHSEP(path[b-1])) --b;
      ww=font->getTextWidth(&path[b],e-b)+aw+PATH_SPACING+PATH_SPACING;
      if(rr-ww<=ll+dw+aw+PATH_SPACING+PATH_SPACING && (f<b || rr-ww<=ll)) break;
      rr-=ww;
      t=b;
      }

    // Dots if parts got dropped
    if(f<t){

      // Draw arrow
      drawArrow(dc,ll,ty,aw);
      ll+=aw+PATH_SPACING;

      // Draw path component
      if(selectBeg==f && selectEnd==t){
        dc.setForeground(selbackColor);
        dc.fillRectangle(ll,tt,dw,bb-tt);
        dc.setForeground(seltextColor);
        dc.drawText(ll,ty,"...",3);
        dc.setForeground(textColor);
        }
      else{
        dc.drawText(ll,ty,"...",3);
        }
      if(hiliteBeg==f && hiliteEnd==t){
        dc.drawFocusRectangle(ll-1,border+1,dw+2,height-border-border-2);
        }
      ll+=dw+PATH_SPACING;
      }

    // Draw visible components
    while(t<path.length()){

      // Scan to '/' or tail
      for(f=t; t<path.length() && !ISPATHSEP(path[t]); ++t){ }

      // Draw arrow
      drawArrow(dc,ll,ty,aw);
      ll+=aw+PATH_SPACING;

      // Draw path component
      ww=font->getTextWidth(&path[f],t-f);
      if(selectBeg==f && selectEnd==t){
        dc.setForeground(selbackColor);
        dc.fillRectangle(ll,tt,ww,bb-tt);
        dc.setForeground(seltextColor);
        dc.drawText(ll,ty,&path[f],t-f);
        dc.setForeground(textColor);
        }
      else{
        dc.drawText(ll,ty,&path[f],t-f);
        }
      if(hiliteBeg==f && hiliteEnd==t){
        dc.drawFocusRectangle(ll-1,border+1,ww+2,height-border-border-2);
        }
      ll+=ww+PATH_SPACING;

      // More after this
      if(ISPATHSEP(path[t])) t++;
      }
    }
  return 1;
  }


// Enter window, kick off tip timer
long FXPathBox::onEnter(FXObject* sender,FXSelector sel,void* ptr){
  FXFrame::onEnter(sender,sel,ptr);
  getApp()->addTimeout(this,ID_TIPTIMER,getApp()->getMenuPause());
  hiliteBeg=hiliteEnd=0;
  update();
  return 1;
  }


// Leave window, kill tip timer
long FXPathBox::onLeave(FXObject* sender,FXSelector sel,void* ptr){
  FXFrame::onLeave(sender,sel,ptr);
  getApp()->removeTimeout(this,ID_TIPTIMER);
  hiliteBeg=hiliteEnd=0;
  update();
  return 1;
  }


// Pressed mouse button
long FXPathBox::onLeftBtnPress(FXObject*,FXSelector,void* ptr){
  FXEvent* event=(FXEvent*)ptr;
  handle(this,FXSEL(SEL_FOCUS_SELF,0),ptr);
  flags&=~FLAG_TIP;
  if(isEnabled() && !(flags&FLAG_PRESSED)){
    grab();
    if(target && target->tryHandle(this,FXSEL(SEL_LEFTBUTTONPRESS,message),ptr)) return 1;
    flags|=FLAG_PRESSED;
    flags&=~FLAG_UPDATE;
    return 1;
    }
  return 0;
  }


// Released mouse button
// A click on arrow should bring up popup with sub-directories.
// A click on icon should bring up alternate drive letters [windows]
// A click on "..." should drop some trailing path-components to reveal at least one
// more item currently represented by the dots.
long FXPathBox::onLeftBtnRelease(FXObject*,FXSelector,void* ptr){
  FXEvent* event=(FXEvent*)ptr;
  if(isEnabled() && (flags&FLAG_PRESSED)){
    FXint code,beg,end;
    ungrab();
    flags|=FLAG_UPDATE;
    flags&=~FLAG_PRESSED;
    if(target && target->tryHandle(this,FXSEL(SEL_LEFTBUTTONRELEASE,message),ptr)) return 1;
    code=getPathComponent(event->win_x,event->win_y,beg,end);
    FXTRACE((TOPIC_DEBUG,"FXPathBox::onLeftBtnRelease: \"%.*s\" code: %d beg: %d end: %d\n",end-beg,&path[beg],code,beg,end));
    selectBeg=beg;
    selectEnd=end;
    update();
    if(target){ target->tryHandle(this,FXSEL(SEL_COMMAND,message),(void*)path.left(end).text()); }
    return 1;
    }
  return 0;
  }


// Pressed middle mouse button
long FXPathBox::onMiddleBtnPress(FXObject*,FXSelector,void* ptr){
  flags&=~FLAG_TIP;
  handle(this,FXSEL(SEL_FOCUS_SELF,0),ptr);
  if(isEnabled()){
    grab();
    flags&=~FLAG_UPDATE;
    if(target && target->handle(this,FXSEL(SEL_MIDDLEBUTTONPRESS,message),ptr)) return 1;
    return 1;
    }
  return 0;
  }


// Released middle mouse button
long FXPathBox::onMiddleBtnRelease(FXObject*,FXSelector,void* ptr){
  if(isEnabled()){
    ungrab();
    flags|=FLAG_UPDATE;
    if(target && target->handle(this,FXSEL(SEL_MIDDLEBUTTONRELEASE,message),ptr)) return 1;

//  pane=new FXPopup(this,FRAME_LINE);
//    FXMenuPane filemenu(this);
//    new FXMenuCommand(&filemenu,tr("Up one level"),updiricon,this,ID_DIRECTORY_UP);
/*
    FXPopup pop(this,FRAME_LINE);
    new FXMenuCommand(&pop,"A File",nullptr);
    pop.create();
    pop.popup(nullptr,((FXEvent*)ptr)->root_x,((FXEvent*)ptr)->root_y);
    getApp()->runModalWhileShown(&pop);
*/
    return 1;
    }
  return 0;
  }


// Pressed right button
long FXPathBox::onRightBtnPress(FXObject*,FXSelector,void* ptr){
  flags&=~FLAG_TIP;
  handle(this,FXSEL(SEL_FOCUS_SELF,0),ptr);
  if(isEnabled()){
    grab();
    flags&=~FLAG_UPDATE;
    if(target && target->handle(this,FXSEL(SEL_RIGHTBUTTONPRESS,message),ptr)) return 1;
    return 1;
    }
  return 0;
  }


// Released right button
long FXPathBox::onRightBtnRelease(FXObject*,FXSelector,void* ptr){
  if(isEnabled()){
    ungrab();
    flags|=FLAG_UPDATE;
    if(target && target->handle(this,FXSEL(SEL_RIGHTBUTTONRELEASE,message),ptr)) return 1;
    return 1;
    }
  return 0;
  }


// Mouse moved
long FXPathBox::onMotion(FXObject*,FXSelector,void* ptr){
  FXEvent* event=(FXEvent*)ptr;
  FXuint flg=flags;
  FXint code,beg,end;

  // Kill the tip
  flags&=~FLAG_TIP;

  // Kill the tip timer
  getApp()->removeTimeout(this,ID_TIPTIMER);


  code=getPathComponent(event->win_x,event->win_y,beg,end);
//  FXTRACE((TOPIC_DETAIL,"FXPathBox::onMotion: %.*s code: %d beg: %d end: %d xx: %d ww: %d\n",end-beg,&path[beg],code,beg,end,xx,ww));
  if((code==1 || code==4) && (hiliteBeg!=beg || hiliteEnd!=end)){
    //update(padleft+border,border+padtop,width-border-border-padright-padright,border+padtop,height-border--border-padbottom-padtop);
    hiliteBeg=beg;
    hiliteEnd=end;
    update();
    }

/*
  if(flags&FLAG_PRESSED){
    }

  // FIXME //
  // Highlight (or greyout) tail of path
  // So we can see what will be path if it were clicked.
*/
  // Reset tip timer if nothing's going on
  getApp()->addTimeout(this,ID_TIPTIMER,getApp()->getMenuPause());

  // Force GUI update only when needed
  return (flg&FLAG_TIP);
  }


// We timed out, i.e. the user didn't move for a while
long FXPathBox::onTipTimer(FXObject*,FXSelector,void*){
  flags|=FLAG_TIP;
  return 1;
  }


// Set the current item's text from the message
long FXPathBox::onCmdSetValue(FXObject*,FXSelector,void* ptr){
  setDirectory((char*)ptr);
  return 1;
  }


// Change value
long FXPathBox::onCmdSetStringValue(FXObject*,FXSelector,void* ptr){
  setDirectory(*((FXString*)ptr));
  return 1;
  }


// Obtain value
long FXPathBox::onCmdGetStringValue(FXObject*,FXSelector,void* ptr){
  *((FXString*)ptr)=getDirectory();
  return 1;
  }


// Set help using a message
long FXPathBox::onCmdSetHelp(FXObject*,FXSelector,void* ptr){
  setHelpText(*((FXString*)ptr));
  return 1;
  }


// Get help using a message
long FXPathBox::onCmdGetHelp(FXObject*,FXSelector,void* ptr){
  *((FXString*)ptr)=getHelpText();
  return 1;
  }


// Set tip using a message
long FXPathBox::onCmdSetTip(FXObject*,FXSelector,void* ptr){
  setTipText(*((FXString*)ptr));
  return 1;
  }


// Get tip using a message
long FXPathBox::onCmdGetTip(FXObject*,FXSelector,void* ptr){
  *((FXString*)ptr)=getTipText();
  return 1;
  }


// We were asked about tip text
long FXPathBox::onQueryTip(FXObject* sender,FXSelector sel,void* ptr){
  if(FXFrame::onQueryTip(sender,sel,ptr)) return 1;
//FIXME//
//FXTRACE((1,"FXDir::hasSubDirectories(%s) : %s\n",path.text(),FXDir::hasSubDirectories(path)?"true":"false"));
  if(flags&FLAG_TIP){
    if(tip.empty()){
      sender->handle(this,FXSEL(SEL_COMMAND,ID_SETSTRINGVALUE),(void*)&path);
      return 1;
      }
    sender->handle(this,FXSEL(SEL_COMMAND,ID_SETSTRINGVALUE),(void*)&tip);
    return 1;
    }
  return 0;
  }


// We were asked about status text
long FXPathBox::onQueryHelp(FXObject* sender,FXSelector sel,void* ptr){
  if(FXFrame::onQueryHelp(sender,sel,ptr)) return 1;
  if((flags&FLAG_HELP) && !help.empty()){
    sender->handle(this,FXSEL(SEL_COMMAND,ID_SETSTRINGVALUE),(void*)&help);
    return 1;
    }
  return 0;
  }


// Set directory
void FXPathBox::setDirectory(const FXString& newpath,FXbool notify){
  FXTRACE((TOPIC_DETAIL,"FXDirBox::setDirectory(%s,%d)\n",newpath.text(),notify));
  FXString abspath(FXPath::absolute(newpath));
  if(!selectPathComponent(path,abspath,selectBeg,selectEnd)){
    path=abspath;
    icon=foldericon;
    selectPathComponent(path,abspath,selectBeg,selectEnd);
    if(associations){
      FXFileAssoc *assoc=associations->findDirBinding(path);
      if(assoc && assoc->miniicon){
        icon=assoc->miniicon;
        if(id()) icon->create();
        }
      }
    recalc();
    update();
    }
  }


/*
// Set directory
void FXPathBox::setDirectory(const FXString& newpath,FXbool notify){
  FXTRACE((TOPIC_DETAIL,"FXDirBox::setDirectory(%s,%d)\n",newpath.text(),notify));
  FXString abspath(FXPath::absolute(newpath));
  if(path!=abspath){
    path=abspath;
    icon=foldericon;
    if(associations){
      FXFileAssoc *assoc=associations->findDirBinding(path);
      if(assoc && assoc->miniicon){
        icon=assoc->miniicon;
        if(id()) icon->create();
        }
      }
    recalc();
    update();
    }
  }
*/

// Change number of columns
void FXPathBox::setNumColumns(FXint ncols){
  if(ncols<0) ncols=0;
  if(columns!=ncols){
    columns=ncols;
    layout();
    recalc();
    update();
    }
  }


// Change the font
void FXPathBox::setFont(FXFont *fnt){
  if(!fnt){ fxerror("%s::setFont: NULL font specified.\n",getClassName()); }
  if(font!=fnt){
    font=fnt;
    recalc();
    update();
    }
  }


// Change file associations; delete the old one unless it was shared
void FXPathBox::setAssociations(FXFileAssociations* assocs,FXbool owned,FXbool notify){
  FXuint opts=options;
  options^=((owned-1)^options)&PATHBOX_NO_OWN_ASSOC;
  if(associations!=assocs){
    if(!(opts&PATHBOX_NO_OWN_ASSOC)) delete associations;
    associations=assocs;
    setDirectory(path,notify);
    }
  }


// Set text color
void FXPathBox::setTextColor(FXColor clr){
  if(clr!=textColor){
    textColor=clr;
    update();
    }
  }


// Set select background color
void FXPathBox::setSelBackColor(FXColor clr){
  if(selbackColor!=clr){
    selbackColor=clr;
    update();
    }
  }


// Set selected text color
void FXPathBox::setSelTextColor(FXColor clr){
  if(seltextColor!=clr){
    seltextColor=clr;
    update();
    }
  }


// Save object to stream
void FXPathBox::save(FXStream& store) const {
  FXFrame::save(store);
  store << path;
  store << font;
  store << icon;
  store << associations;
  store << textColor;
  store << help;
  store << tip;
  }


// Load object from stream
void FXPathBox::load(FXStream& store){
  FXFrame::load(store);
  store >> path;
  store >> font;
  store >> icon;
  store >> associations;
  store >> textColor;
  store >> help;
  store >> tip;
  }


// Delete it
FXPathBox::~FXPathBox(){
  FXTRACE((TOPIC_CONSTRUCT,"FXPathBox::~FXPathBox %p\n",this));
  getApp()->removeTimeout(this,ID_TIPTIMER);
  if(!(options&PATHBOX_NO_OWN_ASSOC)) delete associations;
  font=(FXFont*)-1L;
  icon=(FXIcon*)-1L;
  associations=(FXFileAssociations*)-1L;
  foldericon=(FXIcon*)-1L;
  cdromicon=(FXIcon*)-1L;
  harddiskicon=(FXIcon*)-1L;
  netdriveicon=(FXIcon*)-1L;
  floppyicon=(FXIcon*)-1L;
  nethoodicon=(FXIcon*)-1L;
  zipdiskicon=(FXIcon*)-1L;
  }

}

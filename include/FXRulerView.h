/********************************************************************************
*                                                                               *
*                         R u l e r V i e w   W i d g e t                       *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2006 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXRulerView.h,v 1.16 2006/03/31 07:33:03 fox Exp $                       *
********************************************************************************/
#ifndef FXRULERVIEW_H
#define FXRULERVIEW_H

#ifndef FXSCROLLAREA_H
#include "FXScrollArea.h"
#endif


namespace FX {


class FXRuler;
class FXFrame;


/**
* The Ruler View provides viewing of a document with rulers.
* It is intended to be subclassed in order to draw actual contents
* and provide editing behavior for the document.
* The ruler view itself simply manages the geometry of the document
* being edited, and coordinates the movement of the ruler displays
* as the document is being scrolled.
*/
class FXRulerView : public FXScrollArea {
  FXDECLARE(FXRulerView)
protected:
  FXRuler *hruler;      // Horizontal ruler
  FXRuler *vruler;      // Vertical ruler
  FXFrame *filler;      // Corner thingy
  FXColor  docColor;    // Color of document
  FXString tip;         // Tooltip text
  FXString help;        // Help text
protected:
  FXRulerView();
  virtual void moveContents(FXint x,FXint y);
  virtual void drawBackground(FXDCWindow& dc);
  virtual void drawContents(FXDCWindow& dc);
private:
  FXRulerView(const FXRulerView&);
  FXRulerView &operator=(const FXRulerView&);
public:
  long onPaint(FXObject*,FXSelector,void*);
  long onMotion(FXObject*,FXSelector,void*);
  long onCmdSetHelp(FXObject*,FXSelector,void*);
  long onCmdGetHelp(FXObject*,FXSelector,void*);
  long onCmdSetTip(FXObject*,FXSelector,void*);
  long onCmdGetTip(FXObject*,FXSelector,void*);
  long onQueryHelp(FXObject*,FXSelector,void*);
  long onQueryTip(FXObject*,FXSelector,void*);
  long onDocChanged(FXObject*,FXSelector,void*);
public:
  enum {
    ID_HRULER=FXScrollArea::ID_LAST,
    ID_VRULER,
    ID_LAST
    };
public:

  /// Construct a rulerview window
  FXRulerView(FXComposite* p,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0);

  /// Return viewport height
  virtual FXint getViewportHeight();

  /// Return viewport width
  virtual FXint getViewportWidth();

  /// Return content width
  virtual FXint getContentWidth();

  /// Return content height
  virtual FXint getContentHeight();

  /// Perform layout
  virtual void layout();

  /// Return a pointer to the horizontal ruler
  FXRuler* horizontalRuler() const { return hruler; }

  /// Return a pointer to the vertical ruler
  FXRuler* verticalRuler() const { return vruler; }

  /// Get document position X
  FXint getDocumentX() const;

  /// Get document position Y
  FXint getDocumentY() const;

  /// Set document width
  void setDocumentWidth(FXint w,bool notify=false);

  /// Get document width
  FXint getDocumentWidth() const;

  /// Set document height
  void setDocumentHeight(FXint h,bool notify=false);

  /// Get document height
  FXint getDocumentHeight() const;

  /// Set the current document color
  void setDocumentColor(FXColor clr);

  /// Get the current document color
  FXColor getDocumentColor() const { return docColor; }

  /// Change edge spacing around document
  void setHEdgeSpacing(FXint es,bool notify=false);
  void setVEdgeSpacing(FXint es,bool notify=false);

  /// Return edge spacing
  FXint getHEdgeSpacing() const;
  FXint getVEdgeSpacing() const;

  /// Change horizontal lower margin
  void setHMarginLower(FXint marg,bool notify=false);

  /// Change horizontal upper margin
  void setHMarginUpper(FXint marg,bool notify=false);

  /// Get horizontal lower margin
  FXint getHMarginLower() const;

  /// Get horizontal upper margin
  FXint getHMarginUpper() const;

  /// Change vertical lower margin
  void setVMarginLower(FXint marg,bool notify=false);

  /// Change vertical upper margin
  void setVMarginUpper(FXint marg,bool notify=false);

  /// Get vertical lower margin
  FXint getVMarginLower() const;

  /// Get vertical upper margin
  FXint getVMarginUpper() const;

  /// Set horizontal alignment; the default is RULER_ALIGN_NORMAL
  void setHAlignment(FXuint align,bool notify=false);

  /// Return horizontal alignment
  FXuint getHAlignment() const;

  /// Set vertical alignment; the default is RULER_ALIGN_NORMAL
  void setVAlignment(FXuint align,bool notify=false);

  /// Return vertical alignment
  FXuint getVAlignment() const;

  /// Set X arrow position, relative to document position
  void setArrowPosX(FXint x);

  /// Set X arrow position, relative to document position
  void setArrowPosY(FXint y);

  /// Get X arrow position in document
  FXint getArrowPosX() const;

  /// Get Y arrow position in document
  FXint getArrowPosY() const;

  /// Set the horizontal ruler font
  void setHRulerFont(FXFont *fnt,bool notify=false);

  /// Get the horizontal ruler font
  FXFont* getHRulerFont() const;

  /// Set the vertical ruler font
  void setVRulerFont(FXFont *fnt,bool notify=false);

  /// Get the vertical ruler font
  FXFont* getVRulerFont() const;

  /// Change document number placement
  void setHNumberTicks(FXint ticks,bool notify=false);
  void setVNumberTicks(FXint ticks,bool notify=false);

  /// Return document number placement
  FXint getHNumberTicks() const;
  FXint getVNumberTicks() const;

  /// Change document major ticks
  void setHMajorTicks(FXint ticks,bool notify=false);
  void setVMajorTicks(FXint ticks,bool notify=false);

  /// Return document major ticks
  FXint getHMajorTicks() const;
  FXint getVMajorTicks() const;

  /// Change document medium ticks
  void setHMediumTicks(FXint ticks,bool notify=false);
  void setVMediumTicks(FXint ticks,bool notify=false);

  /// Return document medium ticks
  FXint getHMediumTicks() const;
  FXint getVMediumTicks() const;

  /// Change document tiny ticks
  void setHTinyTicks(FXint ticks,bool notify=false);
  void setVTinyTicks(FXint ticks,bool notify=false);

  /// Return document tiny ticks
  FXint getHTinyTicks() const;
  FXint getVTinyTicks() const;

  /// Change pixel per tick spacing
  void setHPixelPerTick(FXdouble space,bool notify=false);
  void setVPixelPerTick(FXdouble space,bool notify=false);

  /// Return pixel per tick spacing
  FXdouble getHPixelPerTick() const;
  FXdouble getVPixelPerTick() const;

  /// Set ruler style
  void setHRulerStyle(FXuint style);
  void setVRulerStyle(FXuint style);

  /// Get ruler style
  FXuint getHRulerStyle() const;
  FXuint getVRulerStyle() const;

  /// Set the status line help text for the ruler view
  void setHelpText(const FXString& text){ help=text; }

  /// Get the status line help text for the ruler view
  const FXString& getHelpText() const { return help; }

  /// Set the tool tip message for the ruler view
  void setTipText(const FXString& text){ tip=text; }

  /// Get the tool tip message for the ruler view
  const FXString& getTipText() const { return tip; }

  /// Save list to a stream
  virtual void save(FXStream& store) const;

  /// Load list from a stream
  virtual void load(FXStream& store);

  /// Destroy
  virtual ~FXRulerView();
  };

}

#endif

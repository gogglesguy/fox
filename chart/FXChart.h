/********************************************************************************
*                                                                               *
*                        C h a r t   B a s e   W i d g e t                      *
*                                                                               *
*********************************************************************************
* Copyright (C) 2003,2007 by Jeroen van der Zijp.   All Rights Reserved.        *
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
* $Id: FXChart.h,v 1.23 2007/02/07 20:21:51 fox Exp $                           *
********************************************************************************/
#ifndef FXCHART_H
#define FXCHART_H


//////////////////////////////  UNDER DEVELOPMENT  //////////////////////////////


// Define FXCHARTAPI for DLL builds
#ifdef FOXDLL
#ifdef CHARTDLL_EXPORTS
#define FXCHARTAPI FXEXPORT
#else
#define FXCHARTAPI FXIMPORT
#endif
#else
#define FXCHARTAPI
#endif


namespace FX {


/// Tickmark placement styles
enum {
  TICKS_OFF     = 0,            /// No tickmarks
  TICKS_MAJOR   = 0x01,         /// Display major ticks
  TICKS_MINOR   = 0x02,         /// Display minor ticks
  TICKS_INSIDE  = 0x04,         /// Tickmarks inside box
  TICKS_OUTSIDE = 0x08,         /// Tickmarks outside box
  TICKS_CROSS   = 0x0C          /// Tickmarks inside and outside box
  };


/// Tickmark definition
struct TickStyle {
  FXdouble majorspace;          /// Major tick spacing
  FXdouble minorspace;          /// Minor tick spacing
  FXColor  majorcolor;          /// Major tick color
  FXColor  minorcolor;          /// Minor tick color
  FXuchar  majorlength;         /// Major tick length
  FXuchar  minorlength;         /// Minor tick length
  FXuchar  majorweight;         /// Major tick line weight
  FXuchar  minorweight;         /// Minor tick line weight
  FXuchar  style;               /// Style flags
  };


/// Line styles
enum {
  LINESTYLE_NONE,               /// No line
  LINESTYLE_SOLID,              /// Solid line
  LINESTYLE_DOTTED,             /// Dotted line
  LINESTYLE_SHORTDASHED,        /// Short dashed line
  LINESTYLE_LONGDASHED,         /// Long dashed line
  LINESTYLE_DOTDASHED           /// Dot-dashed line
  };


/// Line style definition
struct LineStyle {
  FXColor  forecolor;           /// Line color
  FXColor  backcolor;           /// Back color when stippling (may be clear)
  FXuchar  width;               /// Line width
  FXuchar  cap;                 /// End cap style
  FXuchar  join;                /// Join style
  FXuchar  style;               /// Line style flags
  };


/// Fill styles
enum {
  FILLSTYLE_OFF,                /// Not filled
  FILLSTYLE_SOLID,              /// Solid color
  FILLSTYLE_HATCH,              /// Hatch pattern
  FILLSTYLE_TEXTURE,            /// Repeating texture
  FILLSTYLE_IMAGE,              /// Fill with an image
  FILLSTYLE_HORIZONTAL,         /// Horizontal gradient
  FILLSTYLE_VERTICAL
  };


/// Fill style definition
struct FillStyle {
  FXImage  *image;              /// Image used for texture or image fill
  FXColor   forecolor;          /// Fill color
  FXColor   backcolor;          /// Back color when hatching (may be clear)
  FXColor   lower;              /// Lower gradient color
  FXColor   upper;              /// Upper gradient color
  FXuchar   hatch;              /// Hatch pattern if hatch style
  FXuchar   style;              /// Fill style
  };


/// Text alignment styles
enum {
  HOR_ALIGN_CENTER = 0,         /// Horizontally centered
  HOR_ALIGN_LEFT   = 1,         /// Left aligned
  HOR_ALIGN_RIGHT  = 2,         /// Right aligned
  VER_ALIGN_CENTER = 0,         /// Vertically centered
  VER_ALIGN_TOP    = 4,         /// Top aligned
  VER_ALIGN_BOTTOM = 8          /// Bottom aligned
  };


/// Text definition
struct TextStyle {
  FXFont   *font;               /// Text font
  FXColor   color;              /// Text color
  FXColor   shadowcolor;        /// Text shadow color (may be clear)
  FXuchar   shadowx;            /// X shadow offset
  FXuchar   shadowy;            /// Y shadow offset
  FXuchar   style;              /// Text Style
  };


/// Marker styles
enum {
  MARKER_NONE          = 0,     /// Draw nothing
  MARKER_SQUARE        = 1,     /// Draw (solid) square
  MARKER_CIRCLE        = 2,     /// Draw (solid) circle
  MARKER_DIAMOND       = 3,     /// Draw (solid) diamond
  MARKER_TRIANGLE_UP   = 4,     /// Draw (solid) upward triangle
  MARKER_TRIANGLE_DN   = 5,     /// Draw (solid) downward triangle
  MARKER_TRIANGLE_LT   = 6,     /// Draw (solid) leftward triangle
  MARKER_TRIANGLE_RT   = 7,     /// Draw (solid) rightward triangle
  MARKER_SOLID         = 8      /// Fill shape
  };


/// Marker definition
struct Marker {
  FXColor color;                /// Color of markers
  FXuchar size;                 /// How big to draw markers
  FXuchar style;                /// Marker style
  };


/// Grid styles
enum {
  GRID_OFF   = 0,               /// No grid displayed
  GRID_MAJOR = 1,               /// Draw grid lines at major ticks
  GRID_MINOR = 2                /// Draw grid lines at minor ticks
  };


/// Grid defintion
struct GridStyle {
  LineStyle major;              /// Major grid line styles
  LineStyle minor;              /// Minor grid line styles
  FXuchar   style;              /// Grid draw style
  };


/// Range description
struct Range {
  FXdouble minimum;             /// Minumum value
  FXdouble maximum;             /// Maximum value
  };


/// Axis styles
enum {
  AXIS_OFF       = 0,           /// Nothing drawn on axis
  AXIS_CAPTION   = 0x0001,      /// Axis label drawn
  AXIS_NUMBERS   = 0x0002,      /// Draw numbers on major ticks
  AXIS_UNITS     = 0x0004,      /// Units display (. . .  N/m^2)
  AXIS_GRID      = 0x0008,      /// Grid lines drawn on major ticks
  AXIS_TICKS     = 0x0010,      /// Grid lines drawn on major ticks
  AXIS_EXPONENT  = 0x0020,      /// Exponent near end of axis ( . . .  x 10^5 N/m^2)
  AXIS_REVERSED  = 0x0040,      /// Numbers increase to left
  AXIS_ROUND     = 0x0080,      /// Round range to nearest nice number
  AXIS_LOG       = 0x0100,      /// Logarithmic scale
  AXIS_GRIDFRONT = 0x0200       /// Grid in front of data
  };


/// Axis definition
struct Axis {
  Range       axisrange;        /// Range of displayed part
  Range       datarange;        /// Range of data
  TickStyle   tickstyle;        /// Tick drawing style
  GridStyle   gridstyle;        /// Grid settings
  TextStyle   labelstyle;       /// Style for axis caption
  TextStyle   unitstyle;        /// Style for units
  TextStyle   numberstyle;      /// Style for numbers
  LineStyle   linestyle;        /// Line style of axis
  FXuint      style;            /// Axis style flags
  };


class FXImage;


/// Base class for the various chart widgets
class FXCHARTAPI FXChart : public FXComposite {
  FXDECLARE(FXChart)
protected:
  FXImage  *chart;              // Chart image
  FXString  caption;            // Caption over plot
  FXString  tip;                // Tooltip value
  FXString  help;               // Help value
  FillStyle fillstyle;          // Plot background fill style
  TextStyle captionstyle;       // Plot caption style
protected:
  FXChart();
  FXint textWidth(const TextStyle& ts,const FXString& string) const;
  FXint textHeight(const TextStyle& ts,const FXString& string) const;
  void drawText(FXDC& dc,const TextStyle& ts,FXint x,FXint y,const FXString& string) const;
  void drawMarker(FXDC& dc,const Marker& ms,FXint x,FXint y) const;
  void drawRectangle(FXDC& dc,const FillStyle& fs,FXint x,FXint y,FXint w,FXint h) const;
private:
  FXChart(const FXChart&);
  FXChart &operator=(const FXChart&);
public:
  long onPaint(FXObject*,FXSelector,void*);
  long onQueryHelp(FXObject*,FXSelector,void*);
  long onQueryTip(FXObject*,FXSelector,void*);
  long onClipboardLost(FXObject*,FXSelector,void*);
  long onClipboardRequest(FXObject*,FXSelector,void*);
public:
  static FXDragType bmpType;
  static FXDragType gifType;
  static FXDragType jpgType;
  static FXDragType pngType;
  static FXDragType tifType;
  static FXDragType csvType;
public:

  /// Construct color well with initial color clr
  FXChart(FXComposite* p,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=FRAME_NORMAL,FXint x=0,FXint y=0,FXint w=0,FXint h=0);

  /// Create server-side resources
  virtual void create();

  /// Detach server-side resources
  virtual void detach();

  /// Perform layout
  virtual void layout();

  /// Set fill style
  void setFillStyle(const FillStyle& fs);

  /// Get fill style
  FillStyle getFillStyle() const { return fillstyle; }

  /// Set status line help text for this chart
  void setHelpText(const FXString& text);

  /// Get status line help text for this chart
  FXString getHelpText() const { return help; }

  /// Set tool tip message for this chart
  void setTipText(const FXString& text);

  /// Get tool tip message for this chart
  FXString getTipText() const { return tip; }

  /// Save chart to a stream
  virtual void save(FXStream& store) const;

  /// Load chart from a stream
  virtual void load(FXStream& store);

  /// Destructor
  virtual ~FXChart();
  };

}

#endif

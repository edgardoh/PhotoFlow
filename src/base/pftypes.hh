/* 
 */

/*

    Copyright (C) 2014 Ferrero Andrea

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.


 */

/*

    These files are distributed with PhotoFlow - http://aferrero2707.github.io/PhotoFlow/

 */

#ifndef PF_TYPES_H
#define PF_TYPES_H

#include <lcms2.h>
#include <vips/vips.h>

#include "property.hh"

typedef cmsInt8Number int8_t;
typedef cmsUInt8Number uint8_t;

typedef cmsInt16Number int16_t;
typedef cmsUInt16Number uint16_t;

typedef cmsInt32Number int32_t;
typedef cmsUInt32Number uint32_t;

// 64-bit base types
#if !defined(__APPLE__) && !defined(__MACH__)
#ifndef CMS_DONT_USE_INT64
typedef cmsInt64Number int64_t;
typedef cmsUInt64Number uint64_t;
#endif
#endif

typedef cmsFloat32Number float32_t;
typedef cmsFloat64Number float64_t;


namespace PF
{

  enum BandFormat {
    PF_BANDFMT_UCHAR = IM_BANDFMT_UCHAR,
    PF_BANDFMT_CHAR = IM_BANDFMT_CHAR,
    PF_BANDFMT_USHORT = IM_BANDFMT_USHORT,
    PF_BANDFMT_SHORT = IM_BANDFMT_SHORT,
    PF_BANDFMT_UINT = IM_BANDFMT_UINT,
    PF_BANDFMT_INT = IM_BANDFMT_INT,
    PF_BANDFMT_FLOAT = IM_BANDFMT_FLOAT,
    PF_BANDFMT_DOUBLE = IM_BANDFMT_DOUBLE,
    PF_BANDFMT_UNKNOWN
  };

  enum colorspace_t {
    PF_COLORSPACE_RAW,
    PF_COLORSPACE_GRAYSCALE,
    PF_COLORSPACE_RGB,
    PF_COLORSPACE_LAB,
    PF_COLORSPACE_CMYK,
    PF_COLORSPACE_UNKNOWN
  };

  template<colorspace_t CS>
  struct ColorspaceInfo
  {
    static int NCH;
  };

  enum blendmode_t {
    PF_BLEND_PASSTHROUGH,
    PF_BLEND_NORMAL,
    PF_BLEND_GRAIN_EXTRACT,
    PF_BLEND_GRAIN_MERGE,
    PF_BLEND_OVERLAY,
    PF_BLEND_OVERLAY_GIMP,
    PF_BLEND_SOFT_LIGHT,
    PF_BLEND_HARD_LIGHT,
    PF_BLEND_VIVID_LIGHT,
    PF_BLEND_MULTIPLY,
    PF_BLEND_SCREEN,
    PF_BLEND_LIGHTEN,
    PF_BLEND_DARKEN,
    PF_BLEND_LUMI,
    PF_BLEND_COLOR,
    PF_BLEND_SEP1=1001,
    PF_BLEND_SEP2=1002,
    PF_BLEND_SEP3=1003,
    PF_BLEND_SEP4=1004,
    PF_BLEND_SEP5=1005,
    PF_BLEND_SEP6=1006,
    PF_BLEND_SEP7=1007,
    PF_BLEND_SEP8=1008,
    PF_BLEND_SEP9=1009,
    PF_BLEND_UNKNOWN
  };

  enum rendermode_t {
    PF_RENDER_NORMAL,
    PF_RENDER_PREVIEW,
    PF_RENDER_EDITING
  };


  colorspace_t convert_colorspace(VipsInterpretation interpretation);


  enum mod_key_t {
    MOD_KEY_NONE = 0,
    MOD_KEY_CTRL = 1,
    MOD_KEY_ALT = 2,
    MOD_KEY_SHIFT = 4
  };

  enum direction_key_t {
    DIRECTION_KEY_NONE = 0,
    DIRECTION_KEY_UP = 1,
    DIRECTION_KEY_DOWN = 2,
    DIRECTION_KEY_LEFT = 4,
    DIRECTION_KEY_RIGHT = 8,
    DIRECTION_KEY_SMOOTH = 16
  };


  template<>
  class Property<blendmode_t>: public PropertyBase
  {
  public:
    Property(std::string name, OpParBase* par);
  };


}

#endif

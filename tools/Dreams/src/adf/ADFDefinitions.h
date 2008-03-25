// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 

/**
  * @file ADFDefinitions.h
  */

#ifndef _ADFDEFINITIONS_H
#define _ADFDEFINITIONS_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/aux/FValueVector.h"
#include "dDB/DRALTag.h"
#include "EventShape.h"
#include "DreamsDefs.h"

// Qt includes.
#include <qstring.h>
#include <qcolor.h>
#include <qvaluelist.h>
#include <qintdict.h>

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// -- ADF-Related Defines
// ------------------------------------------------------------------
// ------------------------------------------------------------------

// ------------------------------
// --- XML ADF TAGS & Attributes
// ------------------------------

// -- Main XML-Tags
#define ADF_2DREAMS    "dreams2-adf"
#define ADF_TAGDESC    "tagdescriptor"
#define ADF_SHADE      "shade"
#define ADF_EDGE       "edge"
#define ADF_NODE       "node"
#define ADF_SET        "set"
#define ADF_IF         "if"
#define ADF_BLANKROWS  "blank"
#define ADF_SHADETABLE "shadetable"
#define ADF_SETEDGEBW  "setedgebw"
#define ADF_ITEMFONT   "itemfont"
#define ADF_ITEMWINDOW "itemwindow"
#define ADF_TAB        "tab"
#define ADF_SHOW       "show"
// ADF 2.0
#define ADF_ENTERNODE  "enternode"
#define ADF_EXITNODE   "exitnode"
#define ADF_GROUP      "group"
#define ADF_SHADING    "shading"
// ADF 2.5
#define ADF_DREAMS      "dreams-adf"
#define ADF_DREAMS2     "dreams2"
#define ADF_DREAMS3     "dreams3"
#define ADF_WATERFALL   "waterfall"
#define ADF_WATCHWINDOW "watchwindow"
#define ADF_TABLE       "table"
#define ADF_TR          "tr"
#define ADF_TD          "td"
#define ADF_RECTANGLE   "rectangle"
#define ADF_LAYOUT      "layout"
#define ADF_DIMENSION   "dimension"
#define ADF_DEFAULT     "default"
#define ADF_OVERRIDE    "override"
#define ADF_POINTER     "pointer"
#define ADF_RULE        "rule"
#define ADF_MOVEITEM    "moveitem"
// ADF 2.7
#define ADF_PHASE_HIGH "phaseHigh"
#define ADF_PHASE_LOW  "phaseLow"
#define ADF_PHASE_EXP  "expanded"
#define ADF_PHASE_PH   "phase"

// -- XML-Attributes
#define ADF_COLOR_ATTRIB     "color"
#define ADF_LCOLOR_ATTRIB    "lcolor"
#define ADF_OCCUPANCY_ATTRIB "occupancy"
#define ADF_LETTER_ATTRIB    "letter"
#define ADF_SHAPE_ATTRIB     "shape"
#define ADF_TAGNAME_ATTRIB   "tagname"
#define ADF_VALUE_ATTRIB     "value"
#define ADF_TYPE_ATTRIB      "type"
#define ADF_BASE_ATTRIB      "base"
#define ADF_AUTOFLUSH_ATTRIB "autoflush"
#define ADF_DESCRIPTION_ATTRIB "description"
#define ADF_NAME_ATTRIB      "name"
#define ADF_FROM_ATTRIB      "from"
#define ADF_TO_ATTRIB        "to"
#define ADF_DRAW_WHEN_ATTRIB "draw_when"
#define ADF_BW_ATTRIB        "bw"
#define ADF_BOLD_ATTRIB      "bold"
#define ADF_ITALIC_ATTRIB    "italic"
#define ADF_UNDERLINE_ATTRIB "underline"
#define ADF_ROWS_ATTRIB      "rows"
#define ADF_WITHTAG_ATTRIB   "withtag"
#define ADF_ORDERBY_ATTRIB   "orderby"
#define ADF_SELECT_ATTRIB    "selecttag"
#define ADF_SPLITBY_ATTRIB   "splitbytag"
// ADF 2.0 attributes
#define ADF_ITEMTAGNAME_ATTRIB   "itemtag"
#define ADF_NODETAGNAME_ATTRIB   "nodetag"
#define ADF_CYCLETAGNAME_ATTRIB  "cycletag"
#define ADF_BGCOLOR_ATTRIB       "bgcolor"
#define ADF_SHADETBL_ATTRIB      "shadetable"
#define ADF_MINVALUE_ATTRIB      "minvalue"
#define ADF_MAXVALUE_ATTRIB      "maxvalue"
#define ADF_MINVALUETAG_ATTRIB   "minvaluetag"
#define ADF_MAXVALUETAG_ATTRIB   "maxvaluetag"
// ADF 2.5 attributes
#define ADF_NODE_ATTRIB       "node"
#define ADF_EDGE_ATTRIB       "edge"
#define ADF_LAYOUT_ATTRIB     "layout"
#define ADF_MARGIN_ATTRIB     "margin"
#define ADF_ALIGN_ATTRIB      "align"
#define ADF_TITLE_ATTRIB      "title"
#define ADF_TITLEONTOP_ATTRIB "titleontop"
#define ADF_SIZE_ATTRIB       "size"
#define ADF_BORDER_ATTRIB     "border"
#define ADF_SBORDER_ATTRIB    "sborder"
#define ADF_STAGNAME_ATTRIB   "stag"
#define ADF_ITAGNAME_ATTRIB   "itag"
#define ADF_NTAGNAME_ATTRIB   "ntag"
#define ADF_BCOLOR_ATTRIB     "bcolor"
#define ADF_WIDTH_ATTRIB      "width"
#define ADF_FCOLOR_ATTRIB     "fcolor"
#define ADF_TFILTER_ATTRIB    "tfilter"

// shape-attrib values
#define ADF_SHAPE_RECTANGLE  "rectangle"
#define ADF_SHAPE_CIRCLE     "circle"
#define ADF_SHAPE_TRIANGLE   "triangle"
#define ADF_SHAPE_RHOMB      "diamond"

// type-attrib values
#define ADF_TYPE_INTEGER     "integer"
#define ADF_TYPE_STRING      "string"
#define ADF_TYPE_FP          "fp"
#define ADF_TYPE_SETOFVALUES "setofvalues"

// when-attrib values
#define ADF_DWHEN_RECEIVED   "received"
#define ADF_DWHEN_SENT       "sent"

// color-attrib values
#define ADF_COLOR_TRANSP     "transparent"

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// -- ADF-Related Structures
// ------------------------------------------------------------------
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// -- Supported Row Types
// ------------------------------------------------------------------
typedef enum _RowType
{
    UninitializedRow,
    SimpleEdgeRow,
    SimpleNodeRow,
    InputNodeRow,
    OutputNodeRow,
    SlotNodeRow,
    SimpleBlankRow,
    GroupRow
} RowType;

#endif

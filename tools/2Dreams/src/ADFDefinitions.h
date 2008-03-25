// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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

// Qt library
#include <qstring.h>
#include <qcolor.h>
#include <qvaluelist.h>
#include <qintdict.h>

#include "asim/DralDB.h"

// 2Draems
#include "EventShape.h"

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// -- ADF-Related Defines
// ------------------------------------------------------------------
// ------------------------------------------------------------------

// ------------------------------
// --- XML ADF TAGS & Attributes
// ------------------------------

// -- Main XML-Tags
#define ADF_2DREAMS   "dreams2-adf"
#define ADF_TAGDESC   "tagdescriptor"
#define ADF_SHADE     "shade"
#define ADF_EDGE      "edge"
#define ADF_NODE      "node"
#define ADF_SET       "set"
#define ADF_IF        "if"
#define ADF_BLANKROWS  "blank"
#define ADF_SHADETABLE "shadetable"
#define ADF_ITEMFONT   "itemfont"
#define ADF_ITEMWINDOW "itemwindow"
#define ADF_TAB        "tab"
#define ADF_SHOW       "show"
// ADF 2.0
#define ADF_ENTERNODE  "enternode"
#define ADF_EXITNODE   "exitnode"
#define ADF_GROUP      "group"
#define ADF_SHADING    "shading"

// -- XML-Attributes
#define ADF_COLOR_ATTRIB     "color"
#define ADF_OCCUPANCY_ATTRIB "occupancy"
#define ADF_LETTER_ATTRIB    "letter"
#define ADF_SHAPE_ATTRIB     "shape"
#define ADF_TAGNAME_ATTRIB   "tagname"
#define ADF_VALUE_ATTRIB     "value"
#define ADF_TYPE_ATTRIB      "type"
#define ADF_BASE_ATTRIB      "base"
#define ADF_NAME_ATTRIB      "name"
#define ADF_FROM_ATTRIB      "from"
#define ADF_TO_ATTRIB        "to"
#define ADF_DRAW_WHEN_ATTRIB "draw_when"
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
    SimpleBlankRow
} RowType;

// ------------------------------------------------------------------
// -- Edge Row Structures
// ------------------------------------------------------------------
/**
  * @typedef EdgeRow
  * brief
  */
typedef struct
{
    UINT16 edge_id;
} EdgeRow;
typedef QIntDict<EdgeRow> EdgeRowList;


// ------------------------------------------------------------------
// -- Blank Row Structures
// ------------------------------------------------------------------
/**
  * @typedef BlankRow
  * brief
  */
typedef struct
{
    UINT16 row;
    bool   colored;
    QColor color;
} BlankRow;

typedef QIntDict<BlankRow> BlankRowList;

// ------------------------------------------------------------------
// -- Node Row Structures
// ------------------------------------------------------------------
typedef struct
{
    UINT16 row;
    UINT16 node_id;
    INT32  trackId;
    // add slot spec.
} NodeRow;

typedef QIntDict<NodeRow> NodeRowList;

// ------------------------------------------------------------------
// -- EnterNode Row Structures
// ------------------------------------------------------------------
typedef struct
{
    UINT16 node_id;
    INT32  trackId;
} EnterNodeRow;
typedef QIntDict<EnterNodeRow> EnterNodeRowList;

// ------------------------------------------------------------------
// -- EnterNode Row Structures
// ------------------------------------------------------------------
typedef struct
{
    UINT16 node_id;
    INT32  trackId;
} ExitNodeRow;
typedef QIntDict<ExitNodeRow> ExitNodeRowList;

// ------------------------------------------------------------------
// -- Rule Management Structures
// ------------------------------------------------------------------

typedef struct
{
    bool   bold      : 1;
    bool   underline : 1;
    bool   italic    : 1;
    QColor color;
} ItemWinFont;

typedef struct
{
    UINT16      tagId;
    UINT64      tagValue;
    ItemWinFont fontSpec;
    INT32       dfsNum;

    // for ADF2
    UINT8 isItemTag  : 1;
    UINT8 isNodeTag  : 1;
    UINT8 isCycleTag : 1;

    UINT16 nodeid;     // needed if isNodeTag==1
} TagValueItemWinFont;

/**
  * @typedef ItemWinFontRuleList
  * brief
  */
typedef FValueVector<TagValueItemWinFont> ItemWinFontRuleList;

/**
  * @typedef TagValueColor
  * brief
  */
typedef struct
{
    QColor color;
    UINT16 tagId;
    UINT64 tagValue;
    INT32  dfsNum;

    // for ADF2
    UINT8 isItemTag  : 1;
    UINT8 isNodeTag  : 1;
    UINT8 isCycleTag : 1;

    UINT8 isTransparentColor : 1;

    UINT16 nodeid;     // needed if isNodeTag==1
} TagValueColor;

/**
  * @typedef ColorRuleList
  * brief
  */
typedef FValueVector<TagValueColor> ColorRuleList;

/**
  * @typedef TagValueShape
  * brief
  */
typedef struct
{
    EventShape shape;
    UINT16  tagId;
    UINT64 tagValue;
    INT32  dfsNum;

    // for ADF2
    UINT8 isItemTag  : 1;
    UINT8 isNodeTag  : 1;
    UINT8 isCycleTag : 1;

    UINT16 nodeid;     // needed if isNodeTag==1
} TagValueShape;

/**
  * @typedef ShapeRuleList
  * brief
  */
typedef FValueVector<TagValueShape> ShapeRuleList;

/**
  * @typedef TagValueLetter
  * brief
  */
typedef struct
{
    char   letter;
    UINT16 tagId;
    UINT64 tagValue;
    INT32  dfsNum;

    // for ADF2
    UINT8 isItemTag  : 1;
    UINT8 isNodeTag  : 1;
    UINT8 isCycleTag : 1;

    UINT16 nodeid;     // needed if isNodeTag==1
} TagValueLetter;

/**
  * @typedef LetterRuleList
  * brief
  */
typedef FValueVector<TagValueLetter> LetterRuleList;

/**
  * @typedef ShadeRule
  * brief
  */
typedef struct
{
    INT32  occupancy;  // 0..100
    QColor color;
} ShadeRule;

/**
  * @typedef ShadeRuleTbl
  * brief
  */
typedef QValueList<ShadeRule> ShadeRuleTbl;

/**
  * @typedef ShadeRuleTblSet
  * brief
  */
typedef QDict<ShadeRuleTbl> ShadeRuleTblSet;

typedef struct
{
  bool                hasTvf;
  TagValueItemWinFont tvfSet;
} ItemFontInsideIfData;

typedef struct
{
  TagValueColor  tvcSet;
  bool           hasTvc;
  TagValueShape  tvsSet;
  bool           hasTvs;
  TagValueLetter tvlSet;
  bool           hasTvl;
} SetInsideIfData;

#endif


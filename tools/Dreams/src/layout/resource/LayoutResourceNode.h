/*
 * Copyright (C) 2006 Intel Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */

/**
  * @file  LayoutResourceNode.h
  */

#ifndef _LAYOUTRESOURCENODE_H
#define _LAYOUTRESOURCENODE_H

// Dreams includes.
#include "adf/ADFDefinitions.h"
#include "dDB/DralDBSyntax.h"
#include "dDB/ClockDomainMgr.h"
#include "dDB/DRALTag.h"
#include "dDB/aux/AMemObj.h"
#include "dDB/aux/StatObj.h"
#include "expressions/Expression.h"
#include "layout/resource/LayoutResourceFind.h"
#include "mdi/MDIResourceWindowFold.h"
#include "DreamsDefs.h"
#include "CSLCache.h"

// Qt includes.
#include <qstring.h>
#include <qptrlist.h> 
#include <qprogressdialog.h>

// Asim includes.
#include "asim/AScrollView.h"

// Forward declaration.
class DralDB;
class QPainter;
class LayoutResourceMapEntry;
class ItemHandler;

/*
 * @typedef enum layoutResourcePhase
 *
 * Enum of the phases that can be shown in the layout.
 */
typedef enum layoutResourcePhase
{
    LAYOUTRESOURCE_PHASE_HIGH,
    LAYOUTRESOURCE_PHASE_LOW
} ;

/**
  * This class holds DRAL Graph and configuration information.
  *
  * @author Santi Galan
  * @date started at 2004-07-23
  * @version 0.1
  */
class LayoutResourceNode : public AMemObj, public StatObj
{
  public:
    LayoutResourceNode(DralDB * _draldb, UINT32 row, DRAL_ID dralId, TRACK_ID trackId, TAG_ID tagIdColor, RowType type = SimpleEdgeRow, bool _isDrawOnMove = true);
    virtual ~LayoutResourceNode();

    virtual INT64 getObjSize() const;
    virtual QString getUsageDescription() const;
    virtual QString getStats() const;

    virtual void computeLayoutCache(LayoutResourceCache * cache, UINT32 numGroups, UINT32 lastRow, bool isLast) = 0;

    virtual void addNodeRules(ExpressionList * rules, layoutResourcePhase phase);
    virtual void addNodeRules(Expression * rule, layoutResourcePhase phase);

    inline void setBlankRowColor(QColor color);
    inline bool showPhaseHigh(bool expanded);
    inline bool showPhaseLow(bool expanded);

    inline UINT32 getPhysicalRow() const;
    UINT32 getVirtualRow(LayoutResourceCache * cache) const;
    virtual UINT32 getNumRows() const = 0;

    virtual bool getValidData(UINT32 row, CYCLE cycle) const = 0;
    virtual bool getColorShapeLetter(UINT32 row, CYCLE cycle, ColorShapeItem & cslData) const = 0;
    virtual QString getRowInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) = 0;
    virtual QString getRowCSLInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) = 0;
    virtual bool getRowItemInside(UINT32 row, CYCLE cycle, ItemHandler * hnd) const = 0;
    virtual UINT32 getRowOccupancy(UINT32 row) const = 0;

    virtual QString getRowDescription(UINT32 row, LayoutResourceCache * cache, UINT32 mask) const = 0;
    virtual TRACK_ID getRowTrackId(UINT32 row) const = 0;
    inline RowType getRowType() const;
    inline DRAL_ID getRowDralId() const;
    inline CLOCK_ID getRowClockId() const;
    inline bool getRowShowHigh() const;
    inline bool getRowShowLow() const;
    inline bool getRowShowExpanded() const;
    inline bool getRowIsDrawOnReceive(CYCLE * cycle) const;
    inline bool getRowIsDrawOnMove() const;

    virtual UINT32 getRowWithDralId(DRAL_ID dralId, RowType type) const = 0;

    virtual void getFindData(LayoutResourceFindDataSet * entries, CYCLE startCycle, const LayoutResourceFindOptions * env) const = 0;

    virtual void computeCSL(QProgressDialog * progress, QLabel * label) = 0;

    virtual void drawSubSamplingScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, INT32 stepx, INT32 stepy, QPainter * p, LayoutResourceCache * cache, bool paint_events) = 0;
    virtual void drawNoFullScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, QPainter * p, LayoutResourceCache * cache, bool paint_events) = 0;
    virtual void drawFullScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, QPainter * p, AScrollView * asv, LayoutResourceCache * cache, bool paint_events) = 0;

    virtual void dumpRegression() = 0;

  protected:
    DralDB * draldb;          ///< Pointer to the related DralDB to this layout
    UINT32 physicalPos;       ///< Position of this node in the physical layout.
    DRAL_ID dralId;           ///< Identifyer of the dral node element. It could refer to a dralNode o
    CLOCK_ID clockId;         ///< The clock id of the row.
    TRACK_ID trackId;         ///< Track identifyer linked to the row
                              ///  a dralEdge based on the type of the row. If none of the types is applyed
                              ///  to the node, then the value of the property has no meaning
    TAG_ID tagIdColor;        ///< Tag Identifyer that represents the tag that holds the color for the current
                              ///  view element
    RowType type;             ///< Type of the current node
    bool isDrawOnMove;        ///< True if the row is painted when the element is moved.
    CYCLE drawOnMoveDisp;     ///< Displacement due the draw on move of an edge.
    bool showHigh;            ///< Show the high phase?
    bool showLow;             ///< Show the low phase?
    bool showExpanded;        ///< Show the phase expanded?
    QColor background;        ///< Color used only in blank rows.
    CSLCache cslCache;        ///< Cache of color/shape/letter combinations for curent node (Row/Group)
    ExpressionList rulesHigh; ///< Rules of this node for the high phase.
    ExpressionList rulesLow;  ///< Rules of this node for the low phase.
} ;

/* @typdef LayoutResourceNodeList
 * @brief
 * Just a list of pointers to LayoutResourceNodes.
 */
typedef QPtrList<LayoutResourceNode> LayoutResourceNodeList;

/* @typdef LayoutResourceNodeIterator
 * @brief
 * Just a LayoutResourceNodes iterator.
 */
typedef QPtrListIterator<LayoutResourceNode> LayoutResourceNodeIterator;

/*
 * Sets the background color for a blank row.
 *
 * @return void.
 */
void
LayoutResourceNode::setBlankRowColor(QColor color)
{
    Q_ASSERT(type == SimpleBlankRow);
    background = color;
}

/*
 * Sets that the node must show the high phase.
 *
 * @return if everything is ok.
 */
bool
LayoutResourceNode::showPhaseHigh(bool expanded)
{
    // Can't be repeated.
    if(showHigh)
    {
        return false;
    }

    // Can't show the high if the low is expanded.
    if(showLow && showExpanded)
    {
        return false;
    }

    // Can't show the high expanded if the low is already shown.
    if(showLow && expanded)
    {
        return false;
    }

    // Everything is ok.
    showHigh = true;
    showExpanded = expanded;
    return true;
}

/*
 * Sets that the node must show the low phase.
 *
 * @return if everything is ok.
 */
bool
LayoutResourceNode::showPhaseLow(bool expanded)
{
    // Can't be repeated.
    if(showLow)
    {
        return false;
    }

    // Can't show the low if the high is expanded.
    if(showHigh && showExpanded)
    {
        return false;
    }

    // Can't show the low expanded if the high is already shown.
    if(showHigh && expanded)
    {
        return false;
    }

    // Everything is ok.
    showLow = true;
    showExpanded = expanded;
    return true;
}

/*
 * Method to obtain a contained row position.
 *
 * @return position in the physical layout.
 */
UINT32
LayoutResourceNode::getPhysicalRow() const
{
    return physicalPos;
}

/*
 * Method to obtain the type of a row.
 *
 * @return the type.
 */
RowType
LayoutResourceNode::getRowType() const
{
    return type;
}

/*
 * Method to obtain the dral id of a row.
 *
 * @return the dral id.
 */
DRAL_ID
LayoutResourceNode::getRowDralId() const
{
    return dralId;
}

/*
 * Method to obtain the clock id of a row.
 *
 * @return the clock id.
 */
CLOCK_ID
LayoutResourceNode::getRowClockId() const
{
    return clockId;
}

/*
 * Method to obtain if the node shows the high phase.
 *
 * @return if the high is shown.
 */
bool
LayoutResourceNode::getRowShowHigh() const
{
    return showHigh;
}

/*
 * Method to obtain if the node shows the low phase.
 *
 * @return if the low is shown.
 */
bool
LayoutResourceNode::getRowShowLow() const
{
    return showLow;
}

/*
 * Method to obtain if the node shows the phase expanded.
 *
 * @return if the phase is shown expanded.
 */
bool
LayoutResourceNode::getRowShowExpanded() const
{
    return showExpanded;
}

/*
 * Returns when the row events are painted.
 *
 * @return if the row is painted on the move of events.
 */
bool
LayoutResourceNode::getRowIsDrawOnReceive(CYCLE * cycle) const
{
    * cycle = drawOnMoveDisp;
    return !isDrawOnMove;
}

/*
 * Returns when the row events are painted.
 *
 * @return if the row is painted on the move of events.
 */
bool
LayoutResourceNode::getRowIsDrawOnMove() const
{
    return isDrawOnMove;
}

#endif // _LAYOUTRESOURCENODE_H

// ==================================================
// Copyright (C) 2006 Intel Corporation
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
  * @file LayoutResource.h
  */

#ifndef _LAYOUTRESOURCE_H
#define _LAYOUTRESOURCE_H

// General includes.
#include <list>
#include <map>
using namespace std;

// Dreams includes.
#include "adf/ADFDefinitions.h"
#include "adf/ADFMap.h"
#include "expressions/Expression.h"
#include "mdi/MDIResourceWindowFold.h"
#include "layout/resource/LayoutResourceNode.h"
#include "layout/resource/LayoutResourceCache.h"
#include "DreamsDefs.h"

// Qt includes.
#include <qpainter.h>
#include <qcolor.h>
#include <qstring.h>
#include <qintcache.h>

// Class forwarding.
class DralDB;
class ColorShapeItem;
class LogMgr;
class LayoutResourceGroup;

/*
 * Small class to keep cached complex look ups
 * Long  desc.
 *
 * @author Federico Ardanaz
 * @date started at 2003-01-15
 * @version 0.1
 */
class CachedLookUp
{
  public:
    CachedLookUp(CYCLE _cycle, INT32 _row, CYCLE _startCycle)
    { 
        cycle = _cycle;
        row = _row;
        startCycle = _startCycle;
    }

    INT32 row;
    CYCLE startCycle;
    CYCLE cycle;
} ;

/*
 * @typedef rowList, rowListIterator, dralRowList & dralRowListIterator
 *
 * Classes used to know in which rows appears a given edge or node.
 */
typedef list<UINT32> rowList;
typedef list<UINT32>::iterator rowListIterator;
typedef list<UINT32>::const_iterator rowListConstIterator;
typedef map<DRAL_ID, rowList *> dralRowList;
typedef map<DRAL_ID, rowList *>::iterator dralRowListIterator;
typedef map<DRAL_ID, rowList *>::const_iterator dralRowListConstIterator;

/*
 * Defines different elements that the row description functions
 * must return.
 */

#define RV_ROW_DESC_NUM   0x00000001
#define RV_ROW_DESC_TYPE  0x00000002
#define RV_ROW_DESC_NAME  0x00000004
#define RV_ROW_DESC_FROM  0x00000008
#define RV_ROW_DESC_TO    0x00000010
#define RV_ROW_DESC_LANE  0x00000020
#define RV_ROW_DESC_LEVEL 0x00000040
#define RV_ROW_DESC_ALL   0xFFFFFFFF

/*
 * @brief
 * This class holds the layout of the row of the resource view.
 *
 * @description
 * This class stores all the information needed to know which
 * information the user wants to display in the resource view (this
 * data is obtained from the ADF). All the layout is composed by
 * LayoutResourceNodes, which can be rows, multirows or groups. A group
 * is a set of LayoutResourceNodes.
 * As the resource is implemented as a hierarchy of groups, each
 * one having nodes and groups, the access to a given row is slow
 * because a sequential access must be done. To speed up this process
 * a cache is used to allow a direct access to the rows. As the
 * layout can be changed by folding / unfolding groups, this mapping
 * must be recomputed each time the layout changes (with a linear cost),
 * but as this is only done by user events, the mapping won't change
 * that much, so a clear benefit is obtained with the use of this cache.
 *
 * @author Santi Galan
 * @date started at 2004-07-23
 * @version 0.1
 */
class LayoutResource : public AMemObj, public StatObj
{
  public:
    LayoutResource(DralDB * _draldb, LogMgr * _myLogMgr);
    virtual ~LayoutResource();

    INT64  getObjSize() const;
    QString getUsageDescription() const;
    QString getStats() const;

    bool addRow(RowType type, TRACK_ID track_id, TAG_ID tagIdColor, DRAL_ID dralId, UINT32 rows, bool isDrawOnMove);
    bool newGroup(QString name);
    bool endGroup();

    void addGlobalRules(ExpressionList * rules);
    void addGlobalRules(Expression * rule);
    inline void addRowRules(ExpressionList * rules, layoutResourcePhase phase);
    inline void addRowRules(Expression * rule, layoutResourcePhase phase);
    inline void setBlankRowColor(QColor color);
    inline bool showPhaseHigh(bool expanded);
    inline bool showPhaseLow(bool expanded);

    inline bool getUsesDivisions() const;
    inline UINT32 getNumPhysicalRows() const;
    inline UINT32 getNumGroups() const;

    inline bool getValidData(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) const;
    inline ColorShapeItem getColorShapeLetter(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) const;
    inline QString getRowInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache);
    inline QString getRowCSLInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache);
    inline bool getRowItemInside(UINT32 row, CYCLE cycle, ItemHandler * hnd, LayoutResourceCache * cache) const;
    inline UINT32 getRowOccupancy(UINT32 row, LayoutResourceCache * cache) const;

    inline QString getRowDescription(UINT32 row, LayoutResourceCache * cache, UINT32 mask) const;
    inline TRACK_ID getRowTrackId(UINT32 row, LayoutResourceCache * cache) const;
    inline RowType getRowType(UINT32 row, LayoutResourceCache * cache) const;
    inline DRAL_ID getRowDralId(UINT32 row, LayoutResourceCache * cache) const;
    inline CLOCK_ID getRowClockId(UINT32 row, LayoutResourceCache * cache) const;
    inline UINT32 getRowBaseRow(UINT32 row, LayoutResourceCache * cache) const;
    inline bool getRowShowHigh(UINT32 row, LayoutResourceCache * cache) const;
    inline bool getRowShowLow(UINT32 row, LayoutResourceCache * cache) const;
    inline bool getRowShowExpanded(UINT32 row, LayoutResourceCache * cache) const;
    inline bool getRowIsDrawOnReceive(UINT32 row, CYCLE * cycle, LayoutResourceCache * cache) const;

    void getRowsWithDralId(DRAL_ID dralId, RowType type, const rowList ** rows) const;

    bool findItemWindow(ITEM_ID itemId, CYCLE & firstCycle, CYCLE & lastCycle);
    bool lookForFirstAppearence(ITEM_ID itemId, CYCLE startcycle, UINT32 & resrow, CYCLE & rescycle);
    bool lookForLastAppearence(ITEM_ID itemId, CYCLE endcycle, UINT32 & resrow, CYCLE & rescycle);

    void getFindData(LayoutResourceFindDataSet * entries, CYCLE startCycle, const LayoutResourceFindOptions * env) const;

    void computeLayoutCache(LayoutResourceCache * cache);
    void computeLayoutPlainCache();

    bool getRowIsGroup(UINT32 row, LayoutResourceCache * cache) const;
    void switchRowGroup(UINT32 row, LayoutResourceCache * cache);
    QString getRowComposedName(UINT32 row, LayoutResourceCache * cache) const;
    bool getRowIsShown(UINT32 row, LayoutResourceCache * cache) const;
    UINT32 getRowVirtMap(UINT32 row, LayoutResourceCache * cache) const;
    void showRow(UINT32 row, LayoutResourceCache * cache);

    void computeCSL();

    inline LayoutResourceCache * getPlainCache() const;

    void drawSubSamplingScale(INT32 x1, INT32 x2, INT32 y1, INT32 y2, double scf_x, double scf_y, QPainter * p, LayoutResourceCache * cache, bool paint_events);
    void drawNoFullScale(INT32 x1, INT32 x2, INT32 y1, INT32 y2, QPainter * p, LayoutResourceCache * cache, bool paint_events);
    void drawFullScale(INT32 x1, INT32 x2, INT32 y1, INT32 y2, QPainter * p, AScrollView * asv, LayoutResourceCache * cache, bool paint_events);

    void dumpRegression(bool gui_enabled);

  private:
    UINT32 getRowWithDralId(DRAL_ID dralId, RowType type) const;

  private:
    DralDB * draldb;   ///< Database assigned to this layout.
    LogMgr * myLogMgr; ///< Log manager.

    LayoutResourceCache * plainCache;            ///< Cache with all the groups expanded.
    QIntCache<CachedLookUp> * lookUpCache; ///< Cache to save common look-ups.

    ExpressionList rules;               ///< List of the globally defined rules in resource.
    LayoutResourceNodeList nodeList;    ///< List of all the elements added to the layout.
    LayoutResourceNode * currentNode;   ///< Last element added to the layout.
    LayoutResourceGroup * currentGroup; ///< Current working group.

    UINT32 numPhysicalRows; ///< Physical row counting.
    bool useDivisions;      ///< If the clock works with divisions.
    UINT32 groupId;         ///< Last group id.

    dralRowList edgesRows; ///< For each edge id, we have a set of rows where it appears.
    dralRowList slotRows;  ///< For each node id, we have a set of rows where it appears (slots).
    dralRowList nodeRows;  ///< For each node id, we have a set of rows where it appears (root node).
    rowList voidList;      ///< List returned when nothing is found. Avoids asserting that the pointer returned is not null...
} ;

/*
 * Adds local rules to the actual node.
 *
 * @return void.
 */
void
LayoutResource::addRowRules(ExpressionList * rules, layoutResourcePhase phase)
{
    Q_ASSERT(currentNode != NULL);
    currentNode->addNodeRules(rules, phase);
}

/*
 * Adds a local rule to the actual node.
 *
 * @return void.
 */
void
LayoutResource::addRowRules(Expression * rule, layoutResourcePhase phase)
{
    Q_ASSERT(currentNode != NULL);
    currentNode->addNodeRules(rule, phase);
}

/*
 * Sets the background color to the actual blank row
 *
 * @return void.
 */
void
LayoutResource::setBlankRowColor(QColor color)
{
    Q_ASSERT(currentNode != NULL);
    currentNode->setBlankRowColor(color);
}

/*
 * Sets that the current row must show the high phase.
 *
 * @return if everything is ok.
 */
bool
LayoutResource::showPhaseHigh(bool expanded)
{
    Q_ASSERT(currentNode != NULL);
    useDivisions = useDivisions | !expanded;
    return currentNode->showPhaseHigh(expanded);
}

/*
 * Sets that the current row must show the low phase.
 *
 * @return if everything is ok.
 */
bool
LayoutResource::showPhaseLow(bool expanded)
{
    Q_ASSERT(currentNode != NULL);
    useDivisions = useDivisions | !expanded;
    return currentNode->showPhaseLow(expanded);
}

/*
 * Returns if the resource uses clocks with divisions.
 *
 * @return if divisions are used.
 */
bool
LayoutResource::getUsesDivisions() const
{
    return useDivisions;
}

/*
 * Returns the num of rows of the resource.
 *
 * @return the num of rows.
 */
UINT32
LayoutResource::getNumPhysicalRows() const
{
    return numPhysicalRows;
}

/*
 * Returns the num of groups of the resource.
 *
 * @return the num of groups.
 */
UINT32
LayoutResource::getNumGroups() const
{
    return groupId;
}

/*
 * Says if there is any color defined at the given position in the
 * display
 *
 * @param row
 * @param cycle
 *
 * @return if there is any element defined at the position.
 */
bool 
LayoutResource::getValidData(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getValidData(cache->virToPhy[row].getDisp(), cycle);
}

/*
 * Returns the color defined at the given position in the display
 *
 * @param row
 * @param cycle
 *
 * @return the color.
 */
ColorShapeItem 
LayoutResource::getColorShapeLetter(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());
    ColorShapeItem csi;

    cache->virToPhy[row].getNode()->getColorShapeLetter(cache->virToPhy[row].getDisp(), cycle, csi);
    return csi;
}

/*
 * Method to obtain an user friendly description of the row selected at the 
 * cycle givem
 *
 * @param row Visible row
 * @param cycle Time when the information is requested
 *
 * @return textual description of the row
 */
QString 
LayoutResource::getRowInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache)
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowInfo(cache->virToPhy[row].getDisp(), cycle, cache);
}

/*
 * Method to obtain an user friendly description of the CSL
 *
 * @param row Visible row
 * @param cycle Time when the information is requested
 *
 * @return textual description of the row
 */
QString 
LayoutResource::getRowCSLInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache)
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowCSLInfo(cache->virToPhy[row].getDisp(), cycle, cache);
}

/*
 * Method to obtain the description attached to the given visible row.
 *
 * @param row Visible row
 *
 * @return description related to the visible row, if no row is located at
 * the given position, null is returned
 */
QString
LayoutResource::getRowDescription(UINT32 row, LayoutResourceCache * cache, UINT32 mask) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowDescription(cache->virToPhy[row].getDisp(), cache, mask);
}

/*
 * Method to obtain the trackId attached to the given visible row.
 *
 * @param row Visible row
 *
 * @return trackId related to the visible row, if no row is located at
 * the given position, -1 is returned
 */
TRACK_ID
LayoutResource::getRowTrackId(UINT32 row, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowTrackId(cache->virToPhy[row].getDisp());
}

/*
 * Method to obtain the item inside the row in the cycle cycle.
 *
 * @param row Visible row.
 * @param cycle the requested cycle.
 * @param hnd is the address where the item is returned.
 *
 * @return if an item is inside.
 */
bool
LayoutResource::getRowItemInside(UINT32 row, CYCLE cycle, ItemHandler * hnd, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowItemInside(cache->virToPhy[row].getDisp(), cycle, hnd);
}

/*
 * Method to obtain the occupancy attached to the given visible row.
 *
 * @param row Visible row.
 *
 * @return clock id related to the visible row.
 */
UINT32
LayoutResource::getRowOccupancy(UINT32 row, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowOccupancy(cache->virToPhy[row].getDisp());
}

/*
 * Method to obtain the row type attached to the given visible row.
 *
 * @param row Visible row
 *
 * @return row type related to the visible row, if no row is located at
 * the given position, uninitialized is returned
 */
RowType 
LayoutResource::getRowType(UINT32 row, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowType();
}

/*
 * Method to obtain the dral id attached to the given visible row.
 *
 * @param row Visible row.
 *
 * @return dral id related to the visible row.
 */
DRAL_ID
LayoutResource::getRowDralId(UINT32 row, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowDralId();
}

/*
 * Method to obtain the clock id attached to the given visible row.
 *
 * @param row Visible row.
 *
 * @return clock id related to the visible row.
 */
CLOCK_ID
LayoutResource::getRowClockId(UINT32 row, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowClockId();
}

/*
 * Method to obtain the base row of the given visible row.
 *
 * @param row Visible row.
 *
 * @return the first row of the node where row belongs.
 */
UINT32
LayoutResource::getRowBaseRow(UINT32 row, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getVirtualRow(cache);
}

/*
 * Method to obtain if the high phase is shown in the given visible row.
 *
 * @param row Visible row.
 *
 * @return high phase is shown.
 */
bool
LayoutResource::getRowShowHigh(UINT32 row, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowShowHigh();
}

/*
 * Method to obtain if the low phase is shown in the given visible row.
 *
 * @param row Visible row.
 *
 * @return low phase is shown.
 */
bool
LayoutResource::getRowShowLow(UINT32 row, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowShowLow();
}

/*
 * Method to obtain if the phase shown is expanded.
 *
 * @param row Visible row.
 *
 * @return if is expanded.
 */
bool
LayoutResource::getRowShowExpanded(UINT32 row, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowShowExpanded();
}

/*
 * Method to obtain if the phase shown is expanded.
 *
 * @param row Visible row.
 *
 * @return if is expanded.
 */
bool
LayoutResource::getRowIsDrawOnReceive(UINT32 row, CYCLE * cycle, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < cache->getNumRows());

    return cache->virToPhy[row].getNode()->getRowIsDrawOnReceive(cycle);
}

/*
 * Returns a cache that has no mapping between virtual and physical rows.
 *
 * @return a plain cache.
 */
LayoutResourceCache *
LayoutResource::getPlainCache() const
{
    return plainCache;
}

#endif // _LAYOUTRESOURCE_H

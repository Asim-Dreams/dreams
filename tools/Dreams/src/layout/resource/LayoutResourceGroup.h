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
  * @file LayoutResourceGroup.h
  */

#ifndef _LAYOUTRESOURCEGROUP_H
#define _LAYOUTRESOURCEGROUP_H

// General includes.
#include <vector>
using namespace std;

// Dreams includes.
#include "DreamsDefs.h"
#include "layout/resource/LayoutResourceNode.h"

// Class forwarding.
class LayoutResourceRow;

/*
 * @brief
 * This class represents a group of resource rows that can be
 * folded and unfolded.
 * @description
 *
 * @author Santi Galan
 * @date started at 2004-07-23
 * @version 0.1
 */
class LayoutResourceGroup : public LayoutResourceNode
{
  public:
    LayoutResourceGroup(LayoutResourceGroup * root, QString _name, UINT32 _groupId, DralDB * draldb, UINT32 row, TRACK_ID trackId, TAG_ID tagIdColor);
    ~LayoutResourceGroup();

    inline void addNode(LayoutResourceNode * node);
    void addNodeRules(ExpressionList * rules, layoutResourcePhase phase);
    void addNodeRules(Expression * rule, layoutResourcePhase phase);

    void computeLayoutCache(LayoutResourceCache * cache, UINT32 numGroups, UINT32 lastRow, bool isLast);

    inline UINT32 getNumRows() const;
    inline UINT32 getGroupId() const;
    inline LayoutResourceGroup * getRootGroup() const;
    QString getRowComposedName() const;
    void showRow(UINT32 row, LayoutResourceCache * cache);

    inline bool getValidData(UINT32 row, CYCLE cycle) const;
    inline bool getColorShapeLetter(UINT32 row, CYCLE cycle, ColorShapeItem & cslData) const;
    inline QString getRowInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache);
    inline QString getRowCSLInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache);
    inline bool getRowItemInside(UINT32 row, CYCLE cycle, ItemHandler * hnd) const;
    inline UINT32 getRowOccupancy(UINT32 row) const;

    QString getRowDescription(UINT32 row, LayoutResourceCache * cache, UINT32 mask) const;
    inline TRACK_ID getRowTrackId(UINT32 row) const;

    UINT32 getRowWithDralId(DRAL_ID dralId, RowType type) const;

    void getFindData(LayoutResourceFindDataSet * entries, CYCLE startCycle, const LayoutResourceFindOptions * env) const;

    void computeCSL(QProgressDialog * progress, QLabel * label);

    void drawSubSamplingScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, INT32 stepx, INT32 stepy, QPainter * p, LayoutResourceCache * cache, bool paint_events);
    void drawNoFullScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, QPainter * p, LayoutResourceCache * cache, bool paint_events);
    void drawFullScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, QPainter * p, AScrollView * asv, LayoutResourceCache * cache, bool paint_events);

    void dumpRegression();

  private:
    UINT32 groupId;                  ///< Id of the group inside a 2d layout.
    vector<LayoutResourceNode *> nodeList; ///< The list of the rows that belong to this group.
    LayoutResourceGroup * rootGroup;       ///< The root group of this group. Might be NULL.
    QString name;                    ///< Name of the group.
} ;

/*
 * Adds a new node to the group.
 *
 * @return void.
 */
void
LayoutResourceGroup::addNode(LayoutResourceNode * node)
{
    nodeList.push_back(node);
}

/*
 * Returns the number of rows of this node.
 *
 * @return the number of rows.
 */
UINT32
LayoutResourceGroup::getNumRows() const
{
    return 1;
}

/*
 * Returns the id of group.
 *
 * @return the group id.
 */
UINT32
LayoutResourceGroup::getGroupId() const
{
    return groupId;
}

/*
 * Returns the root group of this group.
 *
 * @return the root group.
 */
LayoutResourceGroup *
LayoutResourceGroup::getRootGroup() const
{
    return rootGroup;
}

/*
 * This function accesses to the database to know if in the row
 * row of this node exist an element in the cycle cycle.
 *
 * @return if data exists in the row.
 */
bool 
LayoutResourceGroup::getValidData(UINT32 row, CYCLE cycle) const
{
    return false;
}

/*
 * This function accesses to the database to know if in the row
 * row of this node exist an element in the cycle cycle.
 *
 * @return if data exists in the row.
 */
bool 
LayoutResourceGroup::getColorShapeLetter(UINT32 row, CYCLE cycle, ColorShapeItem & cslData) const
{
    return false;
}

/*
 * Gets information of the group in the cycle cycle.
 *
 * @return information of the group.
 */
QString
LayoutResourceGroup::getRowInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache)
{
    return "Group: " + name;
}

/*
 * Gets color information.
 *
 * @return information of the group.
 */
QString
LayoutResourceGroup::getRowCSLInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache)
{
    Q_ASSERT(false);
    return "Group: " + name;
}

/*
 * Returns the item inside a group row.
 *
 * @return false as none item is inside this type of row.
 */
bool
LayoutResourceGroup::getRowItemInside(UINT32 row, CYCLE cycle, ItemHandler * hnd) const
{
    return false;
}

/*
 * Returns the item inside a group row.
 *
 * @return false as none item is inside this type of row.
 */
UINT32
LayoutResourceGroup::getRowOccupancy(UINT32 row) const
{
    return 0;
}

/*
 * Returns the trackid of the group.
 *
 * @return the trackid of the group.
 */
TRACK_ID
LayoutResourceGroup::getRowTrackId(UINT32 row) const
{
    return TRACK_ID_INVALID;
}

#endif // _LAYOUTRESOURCEGROUP_H.

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
  * @file LayoutResourceRow.h
  */

#ifndef _LAYOUTRESOURCEROW_H
#define _LAYOUTRESOURCEROW_H

// Dreams includes.
#include "DreamsDefs.h"
#include "dDB/DralDB.h"
#include "expressions/RuleEngine.h"
#include "layout/resource/LayoutResourceNode.h"

/*
 * @brief
 * This class represents a resource row.
 * @description
 *
 * @author Santi Galan
 * @date started at 2004-07-23
 * @version 0.1
 */
class LayoutResourceRow : public LayoutResourceNode
{
  public:
    LayoutResourceRow(DralDB * draldb, UINT32 row, DRAL_ID dralId, TRACK_ID trackId, TAG_ID tagIdColor, RowType type, bool isDrawOnMove);
    ~LayoutResourceRow();

    void computeLayoutCache(LayoutResourceCache * cache, UINT32 numGroups, UINT32 lastRow, bool isLast);

    virtual inline UINT32 getNumRows() const;

    inline bool getValidData(UINT32 row, CYCLE cycle) const;
    inline bool getColorShapeLetter(UINT32 row, CYCLE cycle, ColorShapeItem & cslData) const;
    QString getRowInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache);
    QString getRowCSLInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache);
    inline bool getRowItemInside(UINT32 row, CYCLE cycle, ItemHandler * hnd) const;
    UINT32 getRowOccupancy(UINT32 row) const;

    QString getRowDescription(UINT32 row, LayoutResourceCache * cache, UINT32 mask) const;
    inline TRACK_ID getRowTrackId(UINT32 row) const;

    UINT32 getRowWithDralId(DRAL_ID dralId, RowType type) const;

    void getFindData(LayoutResourceFindDataSet * entries, CYCLE startCycle, const LayoutResourceFindOptions * env) const;

    void computeCSL(QProgressDialog * progress, QLabel * label);

    void drawSubSamplingScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, INT32 stepx, INT32 stepy, QPainter * p, LayoutResourceCache * cache, bool paint_events);
    void drawNoFullScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, QPainter * p, LayoutResourceCache * cache, bool paint_events);
    void drawFullScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, QPainter * p, AScrollView * asv, LayoutResourceCache * cache, bool paint_events);

    void dumpRegression();
    static inline void setSteps(UINT32 _steps);

  protected:
    void internalComputeCSL(QProgressDialog * progress, QLabel * label);
    inline void internalFinalComputeCSL(RuleEvalEnv * env, CYCLE cur_cycle, CYCLE drawOnMoveDisp, INT32 j, INT32 first_cycle, ExpressionList * rules);
    inline void internalFinalComputeCSL(RuleEvalEnv * env, CYCLE cur_cycle, INT32 j, INT32 first_cycle, ExpressionList * rules);
    void initRuleEvalStructs();
    void freeRuleEvalStructs();

    void internalDrawSubSamplingScale(UINT8 division, bool useDivisions, INT32 col1, INT32 col2, INT32 row1, INT32 row2, INT32 stepx, INT32 stepy, QPainter * p, LayoutResourceCache * cache, bool paint_events);
    void internalDrawNoFullScale(UINT8 division, bool useDivisions, INT32 col1, INT32 col2, INT32 row1, INT32 row2, QPainter * p, LayoutResourceCache * cache, bool paint_events);
    void internalDrawFullScale(UINT8 division, bool useDivisions, INT32 col1, INT32 col2, INT32 row1, INT32 row2, QPainter * p, AScrollView * asv, LayoutResourceCache * cache, bool paint_events);

    void getEvaluationDescription(UINT32 row, CYCLE cycle, ExpressionList * rules, RuleEvalEnvDesc * desc);

  protected:
    set<TAG_ID> item_tags;  ///< List of item tags that are used in the rules of this rectangle.
    set<TAG_ID> slot_tags;  ///< List of slot tags that are used in the rules of this rectangle.
    set<TAG_ID> node_tags;  ///< List of node tags that are used in the rules of this rectangle.
    set<TAG_ID> cycle_tags; ///< List of cycle tags that are used in the rules of this rectangle.
    static UINT32 steps;
} ;

/*
 * Returns the number of rows of this node.
 *
 * @return the number of rows.
 */
UINT32
LayoutResourceRow::getNumRows() const
{
    return 1;
}

/*
 * This function accesses to the database to know if in the row
 * row of this node exist an element in the cycle cycle.
 *
 * @return if data exists in the row.
 */
bool 
LayoutResourceRow::getValidData(UINT32 row, CYCLE cycle) const
{
    Q_ASSERT(row < getNumRows());
    UINT64 value;

    return ((type != SimpleBlankRow) && draldb->getTrackTagValue(getRowTrackId(row), tagIdColor, cycle, &value));
}

/*
 * This function accesses to the database to know if in the row
 * row of this node exist an element in the cycle cycle.
 *
 * @return if data exists in the row.
 */
bool 
LayoutResourceRow::getColorShapeLetter(UINT32 row, CYCLE cycle, ColorShapeItem & cslData) const
{
    Q_ASSERT(row < getNumRows());
    UINT64 value;
    bool validData;

    validData = ((type != SimpleBlankRow) && draldb->getTrackTagValue(getRowTrackId(row), tagIdColor, cycle, &value));
    if(validData)
    {
        cslData = cslCache.getColorShape(cycle.cycle - draldb->getFirstEffectiveCycle(cycle.clock->getId()).cycle, getPhysicalRow() + row, cycle.division, static_cast<INT32>(value));
    }

    return validData;
}

/*
 * Returns the item inside a row. The cycle is adapted if draw on
 * received is set.
 *
 * @return if an item is inside the given cycle.
 */
bool
LayoutResourceRow::getRowItemInside(UINT32 row, CYCLE cycle, ItemHandler * hnd) const
{
    if((type == SimpleEdgeRow) && !isDrawOnMove)
    {
        UINT32 latency = draldb->getEdge(getRowDralId())->getLatency();

        cycle.cycle -= latency / cycle.clock->getDivisions();
        cycle.division -= latency % cycle.clock->getDivisions();

        if(cycle.division < 0)
        {
            cycle.division = 1;
            cycle.cycle--;
        }
    }
    return draldb->getItemInSlot(getRowTrackId(row), cycle, hnd);
}

/*
 * Returns the track id of the row inside this node.
 *
 * @return the track id.
 */
TRACK_ID
LayoutResourceRow::getRowTrackId(UINT32 row) const
{
    Q_ASSERT(row < getNumRows());
    if(type == SimpleBlankRow)
    {
        return TRACK_ID_INVALID;
    }
    return trackId + row;
}

/*
 * Computes the color for the given rule environment, cycle and row and
 * stores the result i the track heap.
 *
 * @return void.
 */
void
LayoutResourceRow::internalFinalComputeCSL(RuleEvalEnv * env, CYCLE cur_cycle, CYCLE drawOnMoveDisp, INT32 j, INT32 first_cycle, ExpressionList * rules)
{
    INT32 cache_idx; ///< Row cache index for a given CSL.

    // Resets the previous coloring.
    env->cslResult.reset();
    env->cycle = cur_cycle.cycle;

    // Evaluates the rules.
    RuleEngine::eval(rules, env);

    cur_cycle.cycle += drawOnMoveDisp.cycle;
    cur_cycle.division += drawOnMoveDisp.division;
    if(cur_cycle.division > 1)
    {
        cur_cycle.division = 0;
        cur_cycle.cycle++;
    }

    // If a color is defined.
    if(env->cslResult.getDefined())
    {
        // WARNING: the values used for CSL evaluation in case of draw on read are
        // the values that the item had in the moment that was sent!!! Is this correct??
        // Should an item change its values while its moving over an edge???
        cache_idx = cslCache.addColorShape(cur_cycle.cycle - first_cycle, getPhysicalRow() + j, cur_cycle.division, env->cslResult);
        draldb->addTrackTagValue(getRowTrackId(j), tagIdColor, cur_cycle, (UINT64) cache_idx);
    }
    else
    {
        draldb->undefineTrackTagValue(getRowTrackId(j), tagIdColor, cur_cycle);
    }
}

/*
 * Computes the color for the given rule environment, cycle and row and
 * stores the result i the track heap.
 *
 * @return void.
 */
void
LayoutResourceRow::internalFinalComputeCSL(RuleEvalEnv * env, CYCLE cur_cycle, INT32 j, INT32 first_cycle, ExpressionList * rules)
{
    INT32 cache_idx; ///< Row cache index for a given CSL.

    // Resets the previous coloring.
    env->cslResult.reset();
    env->cycle = cur_cycle.cycle;

    // Evaluates the rules.
    RuleEngine::eval(rules, env);

    // If a color is defined.
    if(env->cslResult.getDefined())
    {
        cache_idx = cslCache.addColorShape(cur_cycle.cycle - first_cycle, getPhysicalRow() + j, cur_cycle.division, env->cslResult);
        draldb->addTrackTagValue(getRowTrackId(j), tagIdColor, cur_cycle, (UINT64) cache_idx);
    }
    else
    {
        draldb->undefineTrackTagValue(getRowTrackId(j), tagIdColor, cur_cycle);
    }
}

/*
 * Sets the step for the progress bar.
 *
 * @return void.
 */
void LayoutResourceRow::setSteps(UINT32 _steps)
{
    steps = _steps;
}

#endif // _LAYOUTRESOURCEROW_H.

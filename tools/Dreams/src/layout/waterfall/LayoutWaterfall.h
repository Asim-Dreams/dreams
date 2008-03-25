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
  * @file LayoutWaterfall.h
  */

#ifndef _LAYOUTWATERFALL_H
#define _LAYOUTWATERFALL_H

// Dreams includes.
#include "dDB/DralDB.h"
#include "dDB/aux/AMemObj.h"
#include "dDB/aux/StatObj.h"
#include "adf/ADFDefinitions.h"
#include "adf/ADFMap.h"
#include "expressions/Expression.h"
#include "expressions/RuleEngine.h"
#include "DreamsDefs.h"
#include "CSLCache.h"
#include "ItemTab.h"
#include "TabSpec.h"

// Asim includes.
#include "asim/AScrollView.h"

// Qt includes.
#include <qpainter.h>
#include <qcolor.h>
#include <qstring.h>
#include <qintdict.h>

// Class forwarding.
class DreamsDB;

/*
 * @brief
 * Class that holds fundamental information for Waterfall views 
 *
 * @description
 * 
 *
 * @author Federico Ardanaz 
 * @date started at 2004-10-27
 * @version 0.1
 */
class LayoutWaterfall : public AMemObj, public StatObj
{
  public:
    LayoutWaterfall(DreamsDB * _dreamsdb);
    virtual ~LayoutWaterfall();

    INT64  getObjSize() const;
    QString getUsageDescription() const;
    QString getStats() const;

    bool addMoveItem(DralGraphEdge *edge);
    bool addEnterNode(DralGraphNode *node);
    bool addExitNode(DralGraphNode *node);

    bool addMoveItemRules(DralGraphEdge *edge, ExpressionList &rules);
    bool addEnterNodeRules(DralGraphNode *node, ExpressionList &rules);
    bool addExitNodeRules(DralGraphNode *node, ExpressionList &rules);

    void addDefaultRules(ExpressionList &rules, INT32 level);
    void addOverrideRules(ExpressionList &rules, INT32 level);
    void addRules(ExpressionList &rules, INT32 level);

    void computeRangeForRow(INT32 row);
    inline CYCLE getCurrentRowMinCycle() const;
    inline CYCLE getCurrentRowMaxCycle() const;

    bool hasTags(UINT32 row, ItemHandler * hnd);

    void updateDBInfo();
    /**
     * Says if there is any color defined at the given position in the
     * display
     *
     * @param row
     * @param col 
     * @return if there is any non-filtered out event defined at the position
     */
    bool getValidData(UINT32 row, INT64 ps, ItemHandler *hnd, ExpressionList **rlist);
    bool getRules(UINT32 row, ItemHandler *hnd, ExpressionList **rlist);
    void resetColorShapeLetter();
    ColorShapeItem getColorShapeLetter(ItemHandler* hnd, ExpressionList* list);
    bool getCurrentITagValue(TAG_ID tagId, INT64 * value) const;
    ITEM_ID getItemIdForRow(UINT32 row);

    inline bool getUsesDivisions() const;
    inline UINT32 getWidth() const;
    inline UINT32 getHeight() const;
    inline UINT32 getNumRows() const;
    inline bool getOpened() const;
    inline void setCurrentItemTab(ItemTab * newtab);

    /**
     * We make TabSpec friend to ease some operations
     */
    friend class TabSpec;

  protected:
    void clearRules();
    bool getEventForPs(ItemHandler * hnd, UINT32 row, INT64 ps, ExpressionList **rlist);

  protected:

    bool wfallADF;
    ItemHandler firstHnd;
    ItemHandler lastHnd;

    CYCLE currentRow_minCycle;
    CYCLE currentRow_maxCycle;
    INT32 num_tags;

    ItemTab* itab;          ///< current in use itab
    RuleEvalEnv env;
    DreamsDB * dreamsdb;
    DralDB * draldb;
    LogMgr* myLogMgr;

    typedef QIntDict<ExpressionList> WFTrackIdHash;
    WFTrackIdHash moveItemHash;
    WFTrackIdHash enterNodeHash;
    WFTrackIdHash exitNodeHash;

    ExpressionList defaultRules; 
    ExpressionList overrideRules; 

};

bool
LayoutWaterfall::getUsesDivisions() const
{
    return draldb->hasDivisions();
}

UINT32
LayoutWaterfall::getWidth() const
{
    UINT32 cycles = draldb->getNumCycles((* draldb->getClockDomainFreqIterator())->getId()).cycle;
    if(getUsesDivisions())
    {
        cycles = cycles << 1;
    }
    return cycles * CUBE_SIDE_BIG;
}

UINT32
LayoutWaterfall::getHeight() const
{
    //int nitems = lastHnd.getItemId()-firstHnd.getItemId()+1;    
   if(!itab)
   {
       return 10;
   }

   QValueVector<INT32> * ilist = itab->getItemList();
   Q_ASSERT(ilist);

   UINT32 nitems = ilist->size();
   //cerr << "LayoutWaterfall::getHeight called for " << itab->getTabSpec()->getName().latin1() << " items=" << nitems << endl;
   return nitems * CUBE_SIDE_BIG;
}

UINT32
LayoutWaterfall::getNumRows() const
{
    return itab->getItemList()->size();
}

bool
LayoutWaterfall::getOpened() const
{
    return wfallADF;
}

void
LayoutWaterfall::setCurrentItemTab(ItemTab * newtab)
{
    itab = newtab;
}

CYCLE
LayoutWaterfall::getCurrentRowMinCycle() const
{
    return currentRow_minCycle;
}

CYCLE
LayoutWaterfall::getCurrentRowMaxCycle() const
{
    return currentRow_maxCycle;
}
    
#endif // _LAYOUTWATERFALL_H.

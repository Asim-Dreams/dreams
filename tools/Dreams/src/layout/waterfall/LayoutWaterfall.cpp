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
 * @file LayoutWaterfall.cpp
 */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "layout/waterfall/LayoutWaterfall.h"
#include "ItemTab.h"
#include "DreamsDB.h"

// Qt includes.
#include <qprogressdialog.h>

/*
 * Creator of the class.
 *
 * @return new object.
 */
LayoutWaterfall::LayoutWaterfall(DreamsDB * _dreamsdb)
{
    wfallADF = false;
    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();
    myLogMgr = LogMgr::getInstance();

    // the lookup cache
    //lookUpCache = new QIntCache<CachedLookUp>(1000,1511);
    //lookUpCache->setAutoDelete(true);
    clearRules();

    num_tags = draldb->getNumTags();

    // Allocates space for all the tags used actually.
    env.itemTags.valids = new bool[num_tags];
    env.itemTags.values = new UINT64[num_tags];
    env.slotTags.valids = new bool[num_tags];
    env.slotTags.values = new UINT64[num_tags];
    env.nodeTags.valids = new bool[num_tags];
    env.nodeTags.values = new UINT64[num_tags];
    env.cycleTags.valids = new bool[num_tags];
    env.cycleTags.values = new UINT64[num_tags];

    // Sets all the slots as invalid.
    memset(env.slotTags.valids, 0, num_tags);
    memset(env.nodeTags.valids, 0, num_tags);
    memset(env.cycleTags.valids, 0, num_tags);

    itab = NULL;
}

LayoutWaterfall::~LayoutWaterfall()
{
    // WARNING
    // HACK
    // TODO
    // SAD
    // These pointers are not deleted because the delete [] raises
    // an abort when done. I don't know why this happen, so by now
    // they are not deleted...
    //delete [] env.itemTags.valids;
    //delete [] env.itemTags.values;
    //delete [] env.slotTags.valids;
    //delete [] env.slotTags.values;
    //delete [] env.nodeTags.valids;
    //delete [] env.nodeTags.values;
    //delete [] env.cycleTags.valids;
    //delete [] env.cycleTags.values;
}

void 
LayoutWaterfall::updateDBInfo()
{
    if (wfallADF)
    {
        bool fok = draldb->getFirstItem(&firstHnd);
        bool lok = draldb->getLastItem(&lastHnd);
        Q_ASSERT(fok&&lok);
        // at this point we know we have all the info
        ItemTab::computeItemLists(dreamsdb);
    }
    else
    {
    }
}

void 
LayoutWaterfall::addDefaultRules(ExpressionList &rules, INT32 level)
{
}
void 
LayoutWaterfall::addOverrideRules(ExpressionList &rules, INT32 level)
{
}

void
LayoutWaterfall::clearRules()
{
}

bool
LayoutWaterfall::addMoveItem(DralGraphEdge *edge)
{
    DRAL_ID edgeid = edge->getEdgeId();
    TRACK_ID trackIdBase = draldb->trackEdgeTags(edgeid);
    if(trackIdBase == TRACK_ID_INVALID)
    {
        return false;
    }
    
    // check if trackId already present
    ExpressionList * elist = moveItemHash.find((long)trackIdBase);
    if (elist != NULL)
    {
        // repeated declarations not allowed
        return false;
    }

    // save all the track ids, the expression list is shared, no edge slot-rule support yet
    INT32 numSlots = (INT32) (edge->getBandwidth() * edge->getLatency());
    elist = new ExpressionList();
    for(TRACK_ID id = trackIdBase; id < (trackIdBase + numSlots); id++)
    {
        //cerr << "Adding edge " << edgeid << " trackid " << id << "\n";
        moveItemHash.insert((long)id, elist);
    }
    //cerr << endl;
    return true;
}

bool
LayoutWaterfall::addMoveItemRules(DralGraphEdge *edge, ExpressionList &rules)
{
    // obtain trackid
    DRAL_ID edgeid = edge->getEdgeId();
    TRACK_ID trackIdBase = draldb->trackEdgeTags(edgeid);
    if(trackIdBase == TRACK_ID_INVALID)
    {
        return false;
    }

    ExpressionList* elist = moveItemHash.find((long)trackIdBase);
    if(elist == NULL)
    {
        return false;
    }
    
    ExpressionIterator iter(rules);
    Expression *rule;
    while((rule = iter.current()) != NULL)
    {
        ++iter;
        elist->append(rule);
    }

    wfallADF = true;
    return true;
}

bool
LayoutWaterfall::addEnterNode(DralGraphNode *node)
{
    DRAL_ID nodeid = node->getNodeId();
    TRACK_ID trackIdBase = draldb->trackNodeTags(nodeid);
    if(trackIdBase == TRACK_ID_INVALID)
    {
        return false;
    }
    
    // check if trackId already present
    ExpressionList* elist = enterNodeHash.find((long)trackIdBase);
    if(elist != NULL)
    {
        // repeated declarations not allowed
        return false;
    }

    // save all the track ids, the expression list is shared, no node slot-rule support yet
    INT32 numSlots = (INT32) (node->getCapacity())+1;
    elist = new ExpressionList();
    for(TRACK_ID id = trackIdBase; id < (trackIdBase + numSlots); id++)
    {
        enterNodeHash.insert((long)id, elist);
    }

    wfallADF = true;
    return true;
}
    
bool 
LayoutWaterfall::addEnterNodeRules(DralGraphNode *node, ExpressionList &rules)
{
    // obtain trackid
    DRAL_ID nodeid = node->getNodeId();
    TRACK_ID trackIdBase = draldb->trackNodeTags(nodeid);
    if(trackIdBase == TRACK_ID_INVALID)
    {
        return false;
    }

    ExpressionList * elist = enterNodeHash.find((long)trackIdBase);
    if(elist==NULL)
    {
        return false;
    }
    
    ExpressionIterator iter(rules);
    Expression *rule;
    while((rule = iter.current()) != NULL)
    {
        ++iter;
        elist->append(rule);
    }

    wfallADF = true;
    return true;
}

bool
LayoutWaterfall::addExitNode(DralGraphNode *node)
{
    DRAL_ID nodeid = node->getNodeId();
    TRACK_ID trackIdBase = draldb->trackNodeTags(nodeid);
    if(trackIdBase == TRACK_ID_INVALID)
    {
        return false;
    }

    // check if trackId already present
    ExpressionList* elist = exitNodeHash.find((long)trackIdBase);
    if(elist != NULL)
    {
        // repeated declarations not allowed
        return false;
    }

    // save all the track ids, the expression list is shared, no node slot-rule support yet
    INT32 numSlots = (INT32) (node->getCapacity())+1;
    elist = new ExpressionList();
    for(TRACK_ID id = trackIdBase; id < (trackIdBase + numSlots); id++)
    {
        exitNodeHash.insert((long)id, elist);
    }

    wfallADF = true;
    return true;
}

bool
LayoutWaterfall::addExitNodeRules(DralGraphNode *node, ExpressionList &rules)
{
    // obtain trackid
    DRAL_ID nodeid=node->getNodeId();
    TRACK_ID trackIdBase = draldb->trackNodeTags(nodeid);

    if(trackIdBase == TRACK_ID_INVALID)
    {
        return false;
    }

    ExpressionList* elist = exitNodeHash.find((long)trackIdBase);
    if(elist == NULL)
    {
        return false;
    }
    
    ExpressionIterator iter(rules);
    Expression *rule;
    while((rule = iter.current()) != NULL)
    {
        ++iter;
        elist->append(rule);
    }

    wfallADF = true;
    return true;
}

void
LayoutWaterfall::computeRangeForRow(INT32 row)
{
    ItemHandler hnd;
    TAG_ID delTagId=draldb->getDelItemTagId();
    TAG_ID newTagId=draldb->getNewItemTagId();
    ITEM_ID litemid = getItemIdForRow(row);

    bool error = (litemid == ITEM_ID_INVALID);

    if(!error)
    {
        error = !draldb->lookForItemId(&hnd, litemid);
    }
    if(!error)
    {
        error = (!hnd.isValidItemHandler() || !hnd.isValidTagHandler());
    }
    if(error)
    {
        currentRow_minCycle = draldb->getFirstEffectiveCycle(0);
        currentRow_maxCycle = draldb->getFirstEffectiveCycle(0);
        currentRow_minCycle.cycle = BIGGEST_CYCLE;
        currentRow_maxCycle.cycle = 0;
        return;
    }

    // check for newItem cycle
    bool fnd = false;
    while(!fnd && hnd.isValidTagHandler())
    {
        fnd = (hnd.getTagId() == newTagId);
        if(!fnd)
        {
            hnd.nextTag();
        }
    }

    Q_ASSERT(fnd); 
    currentRow_minCycle = hnd.getTagCycle();

    // Check for deleteItem cycle
    fnd = false;
    while(!fnd && hnd.isValidTagHandler())
    {
        fnd = (hnd.getTagId() == delTagId);
        if(!fnd)
        {
            hnd.nextTag();
        }
    }
    if(!fnd) 
    {
        // non-closed item
        //cerr << "Non-closed item detected for row " << row << " itemid " << litemid << endl;
        currentRow_maxCycle = draldb->getFirstEffectiveCycle(0);
        currentRow_maxCycle.cycle = BIGGEST_CYCLE;
        return;
    }

    currentRow_maxCycle = hnd.getTagCycle();
}

bool 
LayoutWaterfall::hasTags(UINT32 row, ItemHandler * rhnd)
{
    ItemHandler hnd;
    ITEM_ID litemid = getItemIdForRow(row);
    if(litemid == ITEM_ID_INVALID)
    {
        return false;
    }

    if(!draldb->lookForItemId(&hnd, litemid))
    {
        return false;
    }
   
    if(!hnd.isValidItemHandler() || !hnd.isValidTagHandler()) 
    {
        return false;
    }

    * rhnd = hnd;
    return true;
}

bool 
LayoutWaterfall::getValidData(UINT32 row, INT64 ps, ItemHandler * hnd, ExpressionList ** rlist)
{
    // Just find out if some explicit event has been registered in the db.
    ItemHandler resultHnd;
    ITEM_ID litemid = getItemIdForRow(row);

    if(litemid == ITEM_ID_INVALID)
    { 
        return false; 
    }
    if(!draldb->lookForItemId(hnd, litemid)) 
    { 
        return false; 
    }

    Q_ASSERT(hnd->isValidItemHandler());
    return getEventForPs(hnd, row, ps, rlist);
}

bool 
LayoutWaterfall::getRules(UINT32 row, ItemHandler * hnd, ExpressionList ** rlist)
{
    // now checks if the user is interested in this kind of event
    ItemHeapEventSubType etype = hnd->getEventType(); 
    DRAL_ID id; 
    UINT32 idpos;
    TRACK_ID trackid;
    ExpressionList * elist = NULL;
    bool fnd = false;

    switch(etype)
    {
        case IDB_MOVE_ITEM:
            id = hnd->getEventEdgeId();
            idpos = hnd->getEventPosition();
            trackid = draldb->resolveTrackIdForEdge(id);
            trackid += idpos;
            elist = moveItemHash.find((long) trackid);
            fnd = (elist != NULL);
            //cerr << "validated trackid " << trackid << " for WF fnd=" << fnd << endl;
            break;

        case IDB_ENTER_NODE:
            id = hnd->getEventNodeId();
            // FIXME: checl slot...
            trackid = draldb->resolveTrackIdForNode(id);
            elist = enterNodeHash.find((long) trackid);
            fnd = (elist != NULL);
            break;

        case IDB_EXIT_NODE:
            id = hnd->getEventNodeId();
            // FIXME: checl slot...
            trackid = draldb->resolveTrackIdForNode(id);
            elist = exitNodeHash.find((long) trackid);
            fnd = (elist != NULL);
            break;

        case IDB_GEN_EVENT:
            fnd = false;
            break;

        case IDB_EN_EX_NODE_ESC:
            fnd = false;
            break;

        default:
            // unknown type of event
            Q_ASSERT(false);
    }

    // If found copies the result.
    if(fnd)
    {
        * rlist = elist;
    }

    return fnd; 
}

void
LayoutWaterfall::resetColorShapeLetter()
{
    memset(env.itemTags.valids, 0, num_tags);
}

ColorShapeItem
LayoutWaterfall::getColorShapeLetter(ItemHandler * hnd, ExpressionList * list)
{
    Q_ASSERT(list);
    Q_ASSERT(hnd);

    INT64 curCycle = hnd->getEventCycle().toLCMCycles();

    // Gets the actual state of the item inside.
    while(hnd->isValidTagHandler() && (hnd->getTagCachedCycleLCM() <= curCycle))
    {
        env.itemTags.valids[hnd->getTagCachedId()] = hnd->getTagCachedDefined();
        env.itemTags.values[hnd->getTagCachedId()] = hnd->getTagCachedValue();
        if(hnd->nextTag())
        {
            hnd->cacheTag();
        }
    }

    // Resets the previous coloring.
    env.cslResult.reset();

    RuleEngine::eval(list, &env);

    return ColorShapeItem(env.cslResult);
}

bool
LayoutWaterfall::getCurrentITagValue(TAG_ID tagId, INT64 * value) const
{
    * value = env.itemTags.values[tagId];
    return env.itemTags.valids[tagId];
}

// ---- AMemObj Interface methods
INT64
LayoutWaterfall::getObjSize() const
{
    INT64 result = sizeof(LayoutWaterfall);
    return result;
}

QString
LayoutWaterfall::getUsageDescription() const
{
    QString result = "\n --- LayoutWaterfall memory report --- \n\n";
    return result;
}

QString
LayoutWaterfall::getStats() const
{
    QString result = "";
    return (result);
}

ITEM_ID
LayoutWaterfall::getItemIdForRow(UINT32 row)
{
    if(getNumRows() <= row)
    {
        return ITEM_ID_INVALID;
    }

    Q_ASSERT(itab);
    QValueVector<INT32>* ilist = itab->getItemList();
    Q_ASSERT(ilist);
    bool ok;
    ITEM_ID result = ilist->at(row, &ok);
    if(!ok)
    {
        result = ITEM_ID_INVALID;
    }
    return result;
}

bool
LayoutWaterfall::getEventForPs(ItemHandler * hnd, UINT32 row, INT64 ps, ExpressionList **rlist)
{
    bool found = false;
    ItemHandler temp = * hnd;

    temp.rewindToFirstEvent();

    // Loops through all the events of the item that happened
    // before ps.
    while(temp.isValidEventHandler())
    {
        CYCLE cyc = temp.getEventCycle();
        INT64 actPs = cyc.toPs();

        // If the event starts after the actual ps, then
        // we have reached the end of the loop.
        if(actPs > ps)
        {
            break;
        }

        // We have an event that starts before ps, if it ends after
        // ps, then ps is inside the event duration, so this event
        // is a candidate.
        if(getUsesDivisions())
        {
            if(cyc.division == 0)
            {
                cyc.division = 1;
            }
            else
            {
                cyc.cycle++;
                cyc.division = 0;
            }
        }
        else
        {
            cyc.cycle++;
        }
        actPs = cyc.toPs();
        if(actPs >= ps)
        {
            // Tries to get if rules are defined for this event, so
            // in fact a WaterFall event is related to this event.
            if(getRules(row, &temp, rlist))
            {
                * hnd = temp;
                found = true;
            }
        }

        temp.nextEvent();
    }

    return found;
}

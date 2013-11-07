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
 * @file LayoutResource.cpp
 */

// General includes.
#include <iostream>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "dDB/LogMgr.h"
#include "DreamsDefs.h"
#include "layout/resource/LayoutResource.h"
#include "layout/resource/LayoutResourceRow.h"
#include "layout/resource/LayoutResourceMultiRow.h"
#include "layout/resource/LayoutResourceGroup.h"
#include "layout/resource/LayoutResourceFind.h"

// Qt includes.
#include <qprogressdialog.h>

/*
 * Creator of the class. Copies the parameters, creates the cache
 * and sets the default values.
 *
 * @return new object.
 */
LayoutResource::LayoutResource(DralDB * _draldb, LogMgr * _myLogMgr)
{
    draldb = _draldb;
    myLogMgr = _myLogMgr;

    // the lookup cache
    lookUpCache = new QIntCache<CachedLookUp>(1000, 1511);
    lookUpCache->setAutoDelete(true);

    // Ordering.
    numPhysicalRows = 0;
    groupId = 0;

    // Pointers.
    currentNode = NULL;
    currentGroup = NULL;
    plainCache = NULL;

    // Deletes automatically all the elements.
    nodeList.setAutoDelete(true);

    // Others.
    useDivisions = false;
}

/*
 * Destructor of the class. Deletes the allocated cache.
 *
 * @return destroys the object.
 */
LayoutResource::~LayoutResource()
{
    delete lookUpCache;
    delete plainCache;

    dralRowListIterator it;

    // Deletes all the edge rows.
    it = edgesRows.begin();
    while(it != edgesRows.end())
    {
        delete it->second;
        ++it;
    }

    // Deletes all the node rows.
    it = nodeRows.begin();
    while(it != nodeRows.end())
    {
        delete it->second;
        ++it;
    }

    // Deletes all the slot rows.
    it = slotRows.begin();
    while(it != slotRows.end())
    {
        delete it->second;
        ++it;
    }
}

/*
 * Adds a row into the layout.
 *
 * @return if everything is ok.
 */
bool
LayoutResource::addRow(RowType type, TRACK_ID track_id, TAG_ID tagIdColor, DRAL_ID dralId, UINT32 rows, bool isDrawOnMove)
{
    Q_ASSERT(rows > 0);

    LayoutResourceRow * row;
    if(rows == 1)
    {
        row = new LayoutResourceRow(draldb, numPhysicalRows, dralId, track_id, tagIdColor, type, isDrawOnMove);
    }
    else
    {
        row = new LayoutResourceMultiRow(draldb, numPhysicalRows, dralId, track_id, tagIdColor, rows, type, isDrawOnMove);
    }

    // Gets the list of rows.
    dralRowList * list;

    switch(type)
    {
        case SimpleEdgeRow:
            list = &edgesRows;
            break;

        case SimpleNodeRow:
            list = &nodeRows;
            break;

        case SlotNodeRow:
            list = &slotRows;
            break;

        default:
            list = NULL;
            break;
    }

    if(list != NULL)
    {
        // Tries to find the row list.
        dralRowListIterator it;
        rowList * row_list;

        it = list->find(dralId);
        if(it == list->end())
        {
            // Not found, so we create a new entry for this dral id.
            pair<DRAL_ID, rowList *> p;

            row_list = new rowList();
            p.first = dralId;
            p.second = row_list;

            // Adds the new entry.
            list->insert(p);
        }
        else
        {
            // Found, we just get it.
            row_list = it->second;
        }

        // Adds the rows to the row list.
        for(UINT32 i = 0; i < rows; i++)
        {
            row_list->push_back(row->getPhysicalRow() + i);
        }
    }

    // Inserts the row to the actual group or to the root resource.
    if(currentGroup != NULL)
    {
        currentGroup->addNode(row);
    }
    else
    {
        nodeList.append(row);
    }

    currentNode = row;
    numPhysicalRows += rows;

    // Copies the already defined global rules to the new row.
    row->addNodeRules(&rules, LAYOUTRESOURCE_PHASE_HIGH);
    row->addNodeRules(&rules, LAYOUTRESOURCE_PHASE_LOW);
    return true;
}

/*
 * Adds a group into the layout.
 *
 * @return if everything is ok.
 */
bool
LayoutResource::newGroup(QString name)
{
    LayoutResourceGroup * group;

    // TODO: what is the correct trackid and color id???
    group = new LayoutResourceGroup(currentGroup, name, groupId, draldb, numPhysicalRows, TRACK_ID_INVALID, 0);
    groupId++;

    // Inserts the group to the actual group or to the root resource.
    if(currentGroup != NULL)
    {
        currentGroup->addNode(group);
    }
    else
    {
        nodeList.append(group);
    }

    // Updates the number of rows and the actual group.
    numPhysicalRows++;
    currentGroup = group;
    currentNode = group;

    return true;
}

/*
 * Ends a group into the layout.
 *
 * @return if everything is ok.
 */
bool
LayoutResource::endGroup()
{
    Q_ASSERT(currentGroup != NULL);

    // Pops the current group.
    currentGroup = currentGroup->getRootGroup();
    return true;
}

/*
 * Adds global rules to the whole resource.
 *
 * @return void.
 */
void
LayoutResource::addGlobalRules(ExpressionList * rules)
{
    ExpressionIterator it_expr(* rules);
    Expression * expr;

    // First adds the rules to the already globally defined rules.
    this->rules.appendList(* rules);

    QPtrListIterator<LayoutResourceNode> it_node(nodeList);
    LayoutResourceNode * node;

    // Adds the rules to all the nodes.
    while((node = it_node.current()) != 0)
    {
        ++it_node;
        node->addNodeRules(rules, LAYOUTRESOURCE_PHASE_HIGH);
        node->addNodeRules(rules, LAYOUTRESOURCE_PHASE_LOW);
    }
}

/*
 * Adds a global rule to the whole resource.
 *
 * @return void.
 */
void
LayoutResource::addGlobalRules(Expression * rule)
{
    rules.append(rule);

    QPtrListIterator<LayoutResourceNode> it_node(nodeList);
    LayoutResourceNode * node;

    // Adds the rules to all the nodes.
    while((node = it_node.current()) != 0)
    {
        ++it_node;
        node->addNodeRules(rule, LAYOUTRESOURCE_PHASE_HIGH);
        node->addNodeRules(rule, LAYOUTRESOURCE_PHASE_LOW);
    }
}

/*
 * This method updates the state of the cache that has the mapping
 * between the virtual rows in the screen and the physical rows in
 * the resource. As this function is only called when the user
 * folds / unfolds it's performance is not critical.
 *
 * @return void.
 */
void
LayoutResource::computeLayoutCache(LayoutResourceCache * cache)
{
    // Starts the filling of the map.
    LayoutResourceNodeIterator it_node(nodeList); ///< Iterator of the nodes of the layout.
    LayoutResourceNode * node;                    ///< Actual layout node.

    cache->numVirtualRows = 0;

    // Marks all the entries as invalid, just to know rapidly which are shown
    // and which not.
    for(UINT32 i = 0; i < getNumPhysicalRows(); i++)
    {
        cache->phyToVir[i] = (UINT32) -1;
    }

    // Fills the map using all the nodes.
    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        node->computeLayoutCache(cache, 0, 0, false);
    }
}

/*
 * This method updates the state of the cache that has the mapping
 * between the virtual rows in the screen and the physical rows in
 * the resource. As this function is only called when the user
 * folds / unfolds it's performance is not critical.
 *
 * @return void.
 */
void
LayoutResource::computeLayoutPlainCache()
{
    Q_ASSERT(plainCache == NULL);

    plainCache = new LayoutResourceCache(getNumPhysicalRows(), getNumGroups(), true);

    // Starts the filling of the map.
    LayoutResourceNodeIterator it_node(nodeList); ///< Iterator of the nodes of the layout.
    LayoutResourceNode * node;                    ///< Actual layout node.

    plainCache->numVirtualRows = 0;

    // Fills the map using all the nodes.
    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        node->computeLayoutCache(plainCache, 0, 0, false);
    }
}

/*
 * Method to obtain the first position where the dralId is displayed in the
 * layout
 *
 * @param dralId Identifyer to search
 * @return position where is located. -1 if not found.
 */
UINT32
LayoutResource::getRowWithDralId(DRAL_ID dralId, RowType type) const
{
    UINT32 row = (UINT32) -1;
    LayoutResourceNodeIterator it_node(nodeList);
    LayoutResourceNode * node;

    while(((node = it_node.current()) != NULL) && (row == (UINT32) -1))
    {
        ++it_node;
        row = node->getRowWithDralId(dralId, type);
    }

    return row;
}

/*
 * Gets if the row is a group
 *
 * @return true if is a group.
 */
bool
LayoutResource::getRowIsGroup(UINT32 row, LayoutResourceCache * cache) const
{
    LayoutResourceGroup * group;

    // Gets the group.
    group = dynamic_cast<LayoutResourceGroup *>(cache->virToPhy[row].getNode());
    return (group != NULL);
}

/*
 * Switches the row group fold/unfold state.
 *
 * @return void.
 */
void
LayoutResource::switchRowGroup(UINT32 row, LayoutResourceCache * cache)
{
    LayoutResourceGroup * group;

    // Gets the group.
    group = dynamic_cast<LayoutResourceGroup *>(cache->virToPhy[row].getNode());
    Q_ASSERT(group != NULL);

    UINT32 groupId = group->getGroupId();
    cache->groupExpansion[groupId] = !cache->groupExpansion[groupId];
}

/*
 * Switches the row group fold/unfold state.
 *
 * @return void.
 */
QString
LayoutResource::getRowComposedName(UINT32 row, LayoutResourceCache * cache) const
{
    LayoutResourceGroup * group;

    // Gets the group.
    group = dynamic_cast<LayoutResourceGroup *>(cache->virToPhy[row].getNode());
    Q_ASSERT(group != NULL);

    return group->getRowComposedName();
}

/*
 * Gets if the physical row row is shown.
 *
 * @return if is shown.
 */
bool
LayoutResource::getRowIsShown(UINT32 row, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < getNumPhysicalRows());

    return (cache->phyToVir[row] != (UINT32) -1);
}

/*
 * Gets the virtual mapping of the row.
 *
 * @return virtual row.
 */
UINT32
LayoutResource::getRowVirtMap(UINT32 row, LayoutResourceCache * cache) const
{
    Q_ASSERT(row < getNumPhysicalRows());

    return cache->phyToVir[row];
}

/*
 * Changes the folding state so the physical row row is shown.
 *
 * @return void.
 */
void
LayoutResource::showRow(UINT32 row, LayoutResourceCache * cache)
{
    // This is an internal assert to assure that at least one item is inside the layout.
    Q_ASSERT(nodeList.count() > 0);

    LayoutResourceNodeIterator it_node(nodeList);
    LayoutResourceNode * node = NULL;

    // First finds the node where the row is.
    while(it_node.current() != NULL)
    {
        node = it_node.current();
        if(node->getPhysicalRow() > row)
        {
            // The actual node is beyond the requested row, so we
            // must go to the previous node.
            --it_node;
            node = it_node.current();
            break;
        }
        ++it_node;
    }

    // Now node points to the node of the row.
    LayoutResourceGroup * group;

    // We know that the node is a group because this function should be
    // called only if the row is not present in the layout, so if the row
    // is not shown, means that at least belongs to one group.
    group = dynamic_cast<LayoutResourceGroup *>(node);
    Q_ASSERT(group != NULL);
    group->showRow(row, cache);
}

/*
 * Gets all the rows with the row type type and that represents the
 * dral id dralId.
 *
 * @return void.
 */
void
LayoutResource::getRowsWithDralId(DRAL_ID dralId, RowType type, const rowList ** rows) const
{
    // Gets the list of rows.
    const dralRowList * list;

    switch(type)
    {
        case SimpleEdgeRow:
            list = &edgesRows;
            break;

        case SimpleNodeRow:
            list = &nodeRows;
            break;

        case SlotNodeRow:
            list = &slotRows;
            break;

        default:
            list = NULL;
            break;
    }

    if(list != NULL)
    {
        // Tries to find the row list.
        dralRowListConstIterator it;
        rowList * row_list;

        it = list->find(dralId);
        if(it == list->end())
        {
            * rows = &voidList;
        }
        else
        {
            // Found, we just get it.
            * rows = it->second;
        }
    }
    else
    {
        * rows = &voidList;
    }
}

/*
 * Tries to find the first appearance of the item with it itemId
 * in a row of the layout resource starting from the cycle startcycle.
 * The row where the item appears will be stored in resrow and
 * the first cycle in rescycle.
 *
 * @return true if the item appears in a row of the layout resource.
 */
bool
LayoutResource::lookForFirstAppearence(ITEM_ID itemId, CYCLE startcycle, UINT32 & resrow, CYCLE & rescycle)
{
    CachedLookUp * pt; ///< Cached look up.

    // First look into the cache for saving the expensive search.
    pt = lookUpCache->find((long) itemId);

    // Checks if the element matches.
    if((pt != NULL) && (pt->startCycle == startcycle))
    {
        rescycle = pt->cycle;
        resrow = pt->row;
        return true;
    }

    ItemHandler itemHnd;       ///< Handler used to access to the information of the itemId.
    ItemHeapEventSubType type; ///< The event type.
    UINT32 row;                ///< Row where the event has happened.
    DRAL_ID edgeId;            ///< Edge of the event.
    DRAL_ID nodeId;            ///< Node of the event.

    edgeId = DRAL_ID_INVALID;

    // Obtain the ItemHandler to iterate through all the events related to this itemId.
    draldb->lookForItemId(&itemHnd, itemId);
    Q_ASSERT(itemHnd.isValidItemHandler());

    // First skips all the events that are before startcycle.
    while(itemHnd.isValidEventHandler() && (itemHnd.getEventCycle() < startcycle))
    {
        itemHnd.nextEvent();
    }

    // Now can start the real search.
    while(itemHnd.isValidEventHandler())
    {
        type = itemHnd.getEventType();

        // Checks the type of event.
        if(type == IDB_MOVE_ITEM)
        {
            // 
            edgeId = itemHnd.getEventEdgeId();
            row = getRowWithDralId(edgeId, SimpleEdgeRow);
        }
        else if((type == IDB_ENTER_NODE) || (type == IDB_EXIT_NODE))
        {
            nodeId = itemHnd.getEventNodeId();
            row = getRowWithDralId(nodeId, SlotNodeRow);
        }
        else
        {
            row = (UINT32) -1;
        }

        // If the dral id is in the layout.
        if(row != (UINT32) -1)
        {
            // Copy the data to the variables.
            rescycle = itemHnd.getEventCycle();

            // We must search in which row is the event.
            if(type == IDB_MOVE_ITEM)
            {
                ItemHandler tempHnd;
                DralGraphNode * node;
                TRACK_ID trackId;
                UINT32 capacity;
                UINT64 value;

                trackId = getRowTrackId(row, plainCache);
                if(type == IDB_MOVE_ITEM)
                {
                    capacity = draldb->getEdge(edgeId)->getBandwidth();
                }
                else
                {
                    capacity = draldb->getNode(nodeId)->getCapacity();
                }

                // For all the rows of the edge.
                for(UINT32 i = 0; i < capacity; i++)
                {
                    // Checks if theres an item inside.
                    if(draldb->getTrackTagValue(trackId + i, draldb->getSlotItemTagId(), rescycle, &value))
                    {
                        draldb->lookForItemId(&tempHnd, (ITEM_ID) value);
                        Q_ASSERT(tempHnd.isValidItemHandler());

                        // Check if is the item that we are looking.
                        if(tempHnd.getItemId() == itemId)
                        {
                            // Updates the row and ends the search.
                            row += i;
                            break;
                        }
                    }
                }
            }
            resrow = row;

            // Enter the new entry in the cache.
            lookUpCache->insert((long) itemId, new CachedLookUp(rescycle, row, startcycle));
            return true;
        }

        // Moves to the next event.
        itemHnd.nextEvent();
    }

    // Nothing found.
    return false;
}

/*
 * Tries to find the last appearance of the item with it itemId
 * in a row of the layout resource starting from the cycle startcycle.
 * The row where the item appears will be stored in resrow and
 * the first cycle in rescycle.
 *
 * @return true if the item appears in a row of the layout resource.
 */
bool
LayoutResource::lookForLastAppearence(ITEM_ID itemId, CYCLE endcycle, UINT32 & resrow, CYCLE & rescycle)
{
    ItemHandler itemHnd;       ///< Handler used to access to the information of the itemId.
    ItemHeapEventSubType type; ///< The event type.
    CYCLE cycle;               ///< Cycle when the event happens.
    CYCLE valCycle;            ///< Valid cycle found.
    UINT32 row;                ///< Row where the event happens.
    DRAL_ID edgeId;            ///< Edge of the event.
    DRAL_ID nodeId;            ///< Node of the event.
    bool bFound;               ///< Boolean that holds if we've found a match.

    // Obtain the ItemHandler to iterate through all the events related to this itemId
    bFound = draldb->lookForItemId(&itemHnd, itemId);
    Q_ASSERT(bFound);

    // Initially nothing is found.
    bFound = false;
    row = (UINT32) -1;
    edgeId = DRAL_ID_INVALID;

    // Performs searches until no more events are left or until the event happened later than the endcycle.
    while(itemHnd.isValidEventHandler())
    {
        // We store the cycle to avoid accessing twice the item tag heap
        // in case that the event is in the 
        cycle = itemHnd.getEventCycle();

        // If we have gone further than the endcycle, the loop ends.
        if(cycle > endcycle)
        {
            break;
        }

        type = itemHnd.getEventType();

        // Checks the type of event.
        if(type == IDB_MOVE_ITEM)
        {
            // We are in a move item, so we check
            edgeId = itemHnd.getEventEdgeId();
            row = getRowWithDralId(edgeId, SimpleEdgeRow);
        }
        else if((type == IDB_ENTER_NODE) || (type == IDB_EXIT_NODE))
        {
            nodeId = itemHnd.getEventNodeId();
            row = getRowWithDralId(edgeId, SlotNodeRow);
        }
        else
        {
            row = (UINT32) -1;
        }

        // If the dral id is in the layout.
        if(row != (UINT32) -1)
        {
            bFound = true;
            valCycle = cycle;
        }

        // Moves to the next event.
        itemHnd.nextEvent();
    }

    // If we've found something we copy the values.
    if(bFound)
    {
        resrow = row;
        rescycle = valCycle;
    }

    return bFound;
}

/*
 * Method to search which is the first and the last cycles where the itemId
 * appears.
 *
 * @param itemId
 * @out firstCycle
 * @out lastCycle
 *
 * @return true if the itemId has been found, false otherwise
 */
bool
LayoutResource::findItemWindow(ITEM_ID itemId, CYCLE & firstCycle, CYCLE & lastCycle)
{
    UINT32 row;     ///< Dummy variable.
    bool found;     ///< Boolean that stores if the item is found.

    // Finds the first appearance of the item.
    found = lookForFirstAppearence(itemId, draldb->getFirstCycle(), row, firstCycle);
    if(!found)
    {
        return false;
    }

    // Finds the last appeareance of the item.
    found = lookForLastAppearence(itemId, draldb->getLastCycle(), row, lastCycle);
    if(!found)
    {
        return false;
    }

    // Done!
    return true;
}

/*
 * Fills the data set with the information of all the rows.
 *
 * @return void.
 */
void
LayoutResource::getFindData(LayoutResourceFindDataSet * entries, CYCLE startCycle, const LayoutResourceFindOptions * env) const
{
    LayoutResourceNodeIterator it_node(nodeList);
    LayoutResourceNode * node;

    // Gets the data of all the nodes.
    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        node->getFindData(entries, startCycle, env);
    }
}

/**
 * Method to compute the colors and shape of the events of a given
 * chunk of cycles. Just forwards the call to all the resource nodes.
 *
 * @param cycle First cycle of the chunk.
 * @param chunk_size Size of the the chunk.
 * @param nrcelist List where the computed events are stored.
 *
 * @return void.
 */
void
LayoutResource::computeCSL()
{
    QProgressDialog * progress; ///< Progress dialog to show the evolution of the color computing.
    QLabel * label;             ///< Label of the progress dialog.

    progress = new QProgressDialog("Resource View is computing colors...", 0, getNumPhysicalRows(), NULL, "commitprogress", TRUE);
    label = new QLabel(QString("\nResource View is computing Colors...               ") +
                       QString("\nRows                      : 0"), progress);
    Q_ASSERT(label != NULL);
    label->setMargin(5);
    label->setAlignment(Qt::AlignLeft);
    progress->setLabel(label);

    progress->show();
    progress->repaint();

    if(progress->totalSteps() < 100)
    {
        LayoutResourceRow::setSteps(0);
    }
    else if(progress->totalSteps() < 256)
    {
        LayoutResourceRow::setSteps(1);
    }
    else if(progress->totalSteps() < 512)
    {
        LayoutResourceRow::setSteps(3);
    }
    else
    {
        LayoutResourceRow::setSteps(7);
    }


    LayoutResourceNodeIterator it_node(nodeList);
    LayoutResourceNode * node;

    // Computes the colors of all the nodes.
    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        node->computeCSL(progress, label);
    }

    delete progress;
}

/**
 * Method to paint all the events as points
 *
 * @param x1 Starting horizontal coordinate
 * @param x2 Ending horizontal coordinate
 * @param y1 Starting vertical coordinate
 * @param y2 Ending vertical coordinate
 * @param p Painter where to paint
 *
 * @return void.
 */
void 
LayoutResource::drawSubSamplingScale(INT32 x1, INT32 x2, INT32 y1, INT32 y2, double scf_x, double scf_y, QPainter * p, LayoutResourceCache * cache, bool paint_events)
{
    double dstepx = 1.0 / scf_x;
    double dstepy = 1.0 / scf_y;

    INT32 stepx = (INT32) ceil(dstepx);
    INT32 stepy = (INT32) ceil(dstepy);
    INT32 mstepx = stepx / CUBE_SIDE;
    INT32 mstepy = stepy / CUBE_SIDE;

    INT32 col1 = x1 / CUBE_SIDE;
    INT32 col2 = (x2 + 1) / CUBE_SIDE;
    INT32 row1 = y1 / CUBE_SIDE;
    INT32 row2 = (y2 + 1) / CUBE_SIDE;

    while(row1 < row2)
    {
        cache->virToPhy[row1].getNode()->drawSubSamplingScale(useDivisions, col1, col2, &row1, row2, stepx, stepy, p, cache, paint_events);
    }
}

/**
 * Method to paint all the events with their color and without their border
 *
 * @param x1 Starting horizontal coordinate
 * @param x2 Ending horizontal coordinate
 * @param y1 Starting vertical coordinate
 * @param y2 Ending vertical coordinate
 * @param p Painter where to paint
 *
 * @return void.
 */
void 
LayoutResource::drawNoFullScale(INT32 x1, INT32 x2, INT32 y1, INT32 y2, QPainter * p, LayoutResourceCache * cache, bool paint_events)
{
    INT32 col1 = x1 / CUBE_SIDE;
    INT32 col2 = (x2 + 1) / CUBE_SIDE;
    INT32 row1 = y1 / CUBE_SIDE;
    INT32 row2 = (y2 + 1) / CUBE_SIDE;

    while(row1 < row2)
    {
        cache->virToPhy[row1].getNode()->drawNoFullScale(useDivisions, col1, col2, &row1, row2, p, cache, paint_events);
    }
}

/**
 * Method to paint all the events with their border and color
 *
 * @param x1 Starting horizontal coordinate
 * @param x2 Ending horizontal coordinate
 * @param y1 Starting vertical coordinate
 * @param y2 Ending vertical coordinate
 * @param p Painter where to paint
 *
 * @return void.
 */
void 
LayoutResource::drawFullScale(INT32 x1, INT32 x2, INT32 y1, INT32 y2, QPainter * p, AScrollView * asv, LayoutResourceCache * cache, bool paint_events)
{
    INT32 col1 = x1 / CUBE_SIDE;
    INT32 col2 = (x2 + 1) / CUBE_SIDE;
    INT32 row1 = y1 / CUBE_SIDE;
    INT32 row2 = (y2 + 1) / CUBE_SIDE;

    UINT32 currentRow = 0;

    while(row1 < row2)
    {
        cache->virToPhy[row1].getNode()->drawFullScale(useDivisions, col1, col2, &row1, row2, p, asv, cache, paint_events);
    }
}

/*
 * Dumps the content of the actual trace to the stdout.
 *
 * @return void.
 */
void
LayoutResource::dumpRegression(bool gui_enabled)
{
    UINT32 row;
    INT32 offset;               ///< Number of cycles between progress dialog updates.
    QProgressDialog * progress; ///< Progress dialog to show the evolution of the dumping.
    QLabel * label;             ///< Label of the progress dialog.
    LayoutResourceNode * node;

    offset = 0;
    progress = NULL;
    label = NULL;

    if(gui_enabled)
    {
        progress = new QProgressDialog("Resource View is dumping the color database...", 0, getNumPhysicalRows(), NULL, "commitprogress", TRUE);

        label = new QLabel(QString("\nResource View is dumping the database...               ") +
                           QString("\nRows                              : 0"), progress);
        Q_ASSERT(label != NULL);
        label->setMargin(5);
        label->setAlignment(Qt::AlignLeft);
        //label->setFont(QFont("Courier", qApp->font().pointSize(), QFont::Bold));
        progress->setLabel(label);

        offset = getNumPhysicalRows() / 100;

        // Sanity check.
        offset = QMAX(offset, 1);
        offset = QMIN(offset, 10);
    }

    // Dumps all the cycles of the resource database.
    row = 0;
    QPtrListIterator<LayoutResourceNode> it(nodeList);

    while(row < getNumPhysicalRows())
    {
        node = it.current();
        node->dumpRegression();

        // Updates the iterators.
        row += node->getNumRows();
        ++it;

        // Updates the progress bar.
        if(gui_enabled && ((row % offset) == 0))
        {
            progress->setProgress(row);
            label->setText(QString("\nDreams is dumping the Resource color database...               ") +
                           QString("\nRows                              : ") + QString::number(row));
        }
    }

    // Destroys the progress bar.
    if(gui_enabled)
    {
        delete progress;
    }
}

// ---- AMemObj Interface methods
INT64
LayoutResource::getObjSize() const
{
    INT64 result = sizeof(LayoutResource);

    // aprox.
    LayoutResourceNodeIterator it_node(nodeList);
    LayoutResourceNode * node = NULL;
    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        result += node->getObjSize();
    }

    return result;
}

QString
LayoutResource::getUsageDescription() const
{
    QString result = "\n --- LayoutResource memory report --- \n\n";
    result += "LayoutResource size: " + QString::number(getObjSize()) + " bytes\n";
    result += "Object LayoutResource contains:\n";

    INT64 rules = 0;
    result += "1) ADF Rules:\t" + QString::number(rules) + " bytes\n";

    LayoutResourceNodeIterator it_node(nodeList);
    LayoutResourceNode *node = NULL;
    INT64 sizeNode = 0;

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        sizeNode += node->getObjSize();
    }
    result += "2) Resource nodes:\t" + QString::number(sizeNode) + " bytes\n";
    it_node.toFirst();
    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        result += node->getUsageDescription();
    }

    return result;
}

QString
LayoutResource::getStats() const
{
    QString result = "";

    return (result);
}

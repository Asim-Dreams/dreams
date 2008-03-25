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

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "LayoutResourceRow.h"
#include "LayoutResource.h"
#include "DrawingTools.h"

UINT32 LayoutResourceRow::steps = 0;

/*
 * Forwards the creation to its super class, checks that the type is ok
 * and sets the default clock id.
 *
 * @return new object.
 */
LayoutResourceRow::LayoutResourceRow(DralDB * draldb, UINT32 row, DRAL_ID dralId, TRACK_ID trackId, TAG_ID tagIdColor, RowType type, bool isDrawOnMove)
    : LayoutResourceNode(draldb, row, dralId, trackId, tagIdColor, type, isDrawOnMove)
{
    Q_ASSERT((type == SimpleEdgeRow) || (type == SimpleNodeRow) || (type == SimpleBlankRow));

    if(type == SimpleBlankRow)
    {
        showHigh = true;
    }
    clockId = 0;
}

/*
 * Destructor of the class.
 *
 * @return destroys the object.
 */
LayoutResourceRow::~LayoutResourceRow()
{
}

/*
 * Updates the state of the mapping for the row. Allocates a row
 * for itself and if is expanded, forwards the call to all of its
 * nodes.
 *
 * @return void.
 */
void
LayoutResourceRow::computeLayoutCache(LayoutResourceCache * cache, UINT32 numGroups, UINT32 lastRow, bool isLast)
{
    ResourceViewFoldEnum foldState;
    UINT32 row = cache->numVirtualRows;

    // Updates the row counter.
    cache->numVirtualRows = row + getNumRows();

    // Gets the type of grouping depending if we are inside a group or not.
    if(numGroups == 0)
    {
        foldState = ResourceViewFoldNothing;
    }
    else
    {
        foldState = ResourceViewFoldLine;
    }

    UINT32 i;

    // Iterates through all the rows of this row / multi-row.
    for(i = 0; i < getNumRows() - 1; i++)
    {
        cache->virToPhy[row + i].setNode(this);
        cache->virToPhy[row + i].setDisp(i);
        cache->phyToVir[physicalPos + i] = row + i;
        cache->foldCache[row + i] = foldState;
    }

    cache->virToPhy[row + i].setNode(this);
    cache->virToPhy[row + i].setDisp(i);
    cache->phyToVir[physicalPos + i] = row + i;

    // The last row has special folding info.
    if(numGroups == 0)
    {
        cache->foldCache[row + i] = ResourceViewFoldNothing;
    }
    else
    {
        if(isLast)
        {
            if(numGroups == lastRow)
            {
                cache->foldCache[row + i] = ResourceViewFoldEnd;
            }
            else
            {
                cache->foldCache[row + i] = ResourceViewFoldLineAndEnd;
            }
        }
        else
        {
            cache->foldCache[row + i] = ResourceViewFoldLine;
        }
    }
}

/*
 * Returns the occupancy for this row.
 *
 * @return the occupancy.
 */
UINT32
LayoutResourceRow::getRowOccupancy(UINT32 row) const
{
    return draldb->getTrackTagOccupancy(getRowTrackId(row), tagIdColor);
}

/*
 * Returns a description of the row.
 *
 * @return a description of the row.
 */
QString
LayoutResourceRow::getRowDescription(UINT32 row, LayoutResourceCache * cache, UINT32 mask) const
{
    Q_ASSERT(row < getNumRows());
    QString rowDesc;

    if(mask & RV_ROW_DESC_NUM)
    {
        rowDesc = "Row " + QString::number(getVirtualRow(cache) + row) + " ";
    }

    // Set the row description
    if(type == SimpleEdgeRow)
    {
        DralGraphEdge * edge = draldb->getEdge(getRowDralId());
        if(mask & RV_ROW_DESC_TYPE)
        {
            rowDesc += "Edge ";
        }
        if(mask & RV_ROW_DESC_NAME)
        {
            rowDesc += edge->getName() + " ";
        }
        if(mask & RV_ROW_DESC_FROM)
        {
            rowDesc += "from " + edge->getSourceNodeName() + " ";
        }
        if(mask & RV_ROW_DESC_TO)
        {
            rowDesc += "to " + edge->getDestinationNodeName() + " ";
        }
        if(mask & RV_ROW_DESC_LANE)
        {
            rowDesc += "lane " + QString::number(row);
        }
    }
    else if(type == SimpleNodeRow)
    {
        DralGraphNode * node = draldb->getNode(getRowDralId());
        if(mask & RV_ROW_DESC_TYPE)
        {
            rowDesc += "Node ";
        }
        if(mask & RV_ROW_DESC_NAME)
        {
            rowDesc += node->getName() + " ";
        }
        if(mask & RV_ROW_DESC_LEVEL)
        {
            rowDesc += "(root level)";
        }
    }
    else
    {
        if(mask & RV_ROW_DESC_TYPE)
        {
            rowDesc += "Blank";
        }
    }

    return rowDesc;
}

/*
 * Returns its position if represents the dral id id and is a row of type
 * type.
 *
 * @return the row position.
 */
UINT32
LayoutResourceRow::getRowWithDralId(DRAL_ID id, RowType type) const
{
    if((dralId == id) && (type == this->type))
    {
        return getPhysicalRow();
    }
    else
    {
        return (UINT32) -1;
    }
}

/*
 * Gets information of the row in the cycle cycle.
 *
 * @return information of the node.
 */
QString
LayoutResourceRow::getRowInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache)
{
    Q_ASSERT(row < getNumRows());
    QString result = QString::null;
    DralGraphNode * dralNode;
    DralGraphEdge * dralEdge;
    ItemHandler hnd;

    switch(type)
    {
        case SimpleNodeRow:
            dralNode = draldb->getNode(getRowDralId());
            if(dralNode == NULL) 
            { 
                return QString("Unexpected inconsistency at ResourceDAvtView::getRowInfo(), please report this problem!"); 
            }
            result += "Row Number:\t " + QString::number(getVirtualRow(cache) + row) + "\n";
            result += "Row Type:\t Simple Node Row\n";
            result += "Node ID:\t\t " + QString::number(getRowDralId()) + "\n";
            result += "Node Name:\t " + dralNode->getName() + "\n";
            result += "Node Capacity:\t " + QString::number(dralNode->getCapacity()) + "\n";
            result += "\n";

            /*if(rules != NULL)
            {
                QString shdtbl = rules->getShadingTbl();
                if(shdtbl != QString::null)
                {
                    result += "Using color shading table:\t " + shdtbl + "\n";
                    result += "Shading minimum value:\t " + QString::number(rules->getShadingMinValue())+ "\n";
                    result += "Shading maximum value:\t " + QString::number(rules->getShadingMaxValue())+ "\n";
                    INT32 tgId = rules->getShadingTag();
                    result += "Shading tag:\t\t " + draldb->getTagName(tgId) + "\n";
                }
            }*/

            // XXX
            // Should we maintain the number of the lines where the node is used??
            //rrlist = gnode->getRowList();
            //if (rrlist->count()>1)
            //{
            //    report += "Node used on rows:\t ";
            //    QIntDictIterator<RowRules> it( *rrlist );
            //    for ( ; it.current(); ++it )
            //    {
            //        rrules = it.current();
            //        report += QString::number(rrules->getRowPos());
            //        report += " ";
            //    }
            //    report += "\n";
            //}
            break;

        case SimpleEdgeRow:
            dralEdge = draldb->getEdge(getRowDralId());
            if(dralEdge == NULL) 
            { 
                return QString("Unexpected inconsistency at ResourceDAvtView::getRowInfo(), please report this problem!"); 
            }

            result += "Row Number:\t\t" + QString::number(getVirtualRow(cache) + row) + "\n";
            result += "Row Type:\t\tSimple Edge Row \n";
            result += "Edge ID:\t\t\t" + QString::number(getRowDralId()) + "\n";
            result += "Edge Name:\t\t" + dralEdge->getName() + "\n";
            result += "Edge Source:\t\t" + dralEdge->getSourceNodeName() + "\n";
            result += "Edge Destination:\t" + dralEdge->getDestinationNodeName() + "\n";
            result += "Edge Bandwidth:\t\t" + QString::number(dralEdge->getBandwidth()) + "\n";
            result += "Edge Latency:\t\t" + QString::number(dralEdge->getLatency()) + "\n";
            result += "\n";

            if(getRowIsDrawOnMove())
            {
                result += "Draw event when the item is sent\n";
            }
            else
            {
                result += "Draw event when the item is received\n";
            }
            break;

        case SimpleBlankRow:
            result += "Row Number:\t " + QString::number(getVirtualRow(cache) + row) + "\n";
            result += "Row Type:\t Blank Row \n";
            break;

        default:
            break;
    }

    return result;
}

/*
 * Gets information of the row in the cycle cycle.
 *
 * @return information of the node.
 */
QString
LayoutResourceRow::getRowCSLInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache)
{
    Q_ASSERT(row < getNumRows());
    QString result = QString::null;
    ItemHandler hnd;

    switch(type)
    {
        case SimpleNodeRow:
            if(trackId != TRACK_ID_INVALID)
            {
                RuleEvalEnvDesc desc; ///< Rule evaluation description.
                QString border_color;
                QString fill_color;
                QString font;
                QString letter;
                QString letter_color;
                QString shape;

                desc.border_color = &border_color;
                desc.fill_color = &fill_color;
                desc.font = &font;
                desc.letter = &letter;
                desc.letter_color = &letter_color;
                desc.shape = &shape;

                if(cycle.division == 0)
                {
                    getEvaluationDescription(row, cycle, &rulesHigh, &desc);
                }
                else
                {
                    getEvaluationDescription(row, cycle, &rulesLow, &desc);
                }

                result += "Color Decision for current event:\n";
                result += fill_color;
                result += "\nShape Decision for current event:\n";
                result += shape;
                result += "\nLetter Decision for current event:\n";
                result += letter;
                result += "\n";
            }
            break;

        case SimpleEdgeRow:
            // put xml associated with color and shape decision
            getRowItemInside(row, cycle, &hnd);

            if(hnd.isValidItemHandler())
            {
                RuleEvalEnvDesc desc; ///< Rule evaluation description.
                QString border_color;
                QString fill_color;
                QString font;
                QString letter;
                QString letter_color;
                QString shape;

                desc.border_color = &border_color;
                desc.fill_color = &fill_color;
                desc.font = &font;
                desc.letter = &letter;
                desc.letter_color = &letter_color;
                desc.shape = &shape;

                getEvaluationDescription(row, cycle, &rulesHigh, &desc);

                result += "Color Decision for current event:\n";
                result += fill_color;
                result += "\nShape Decision for current event:\n";
                result += shape;
                result += "\nLetter Decision for current event:\n";
                result += letter;
                result += "\n";
            }
            break;

        case SimpleBlankRow:
            Q_ASSERT(false);
            break;

        default:
            break;
    }

    return result;
}

/*
 * Fills the data set with the information of the row.
 *
 * @return void.
 */
void
LayoutResourceRow::getFindData(LayoutResourceFindDataSet * entries, CYCLE startCycle, const LayoutResourceFindOptions * env) const
{
    // As we are looking for items, we only look at edges and slots.
    if((type != SimpleEdgeRow) && (type != SlotNodeRow))
    {
        return;
    }

    CYCLE internalCycle;
    UINT64 drawReceivedDisp = 0;

    internalCycle.clock = draldb->getClockDomain(getRowClockId());
    internalCycle.fromLCMCycles(startCycle.toLCMCycles());

    // If it's an edge and is shown when received.
    if(!isDrawOnMove)
    {
        // Changes the cycle to LCM cycles.
        drawReceivedDisp = drawOnMoveDisp.toLCMCycles();
    }

    // If the row shows highs and the user wants to search in highs.
    // To catch the exception when is an edge with draw on receive and
    // has the latency with division = 1, and the values are stored in
    // the inverse phase than the shown ones.
    if((getRowShowHigh() || (getRowShowLow() && (drawOnMoveDisp.division == 1))) && env->getFindHigh())
    {
        CYCLE cycle = internalCycle;

        if(cycle.division == 1)
        {
            cycle.cycle++;
            cycle.division = 0;
        }

        // For all the rows.
        for(UINT32 i = 0; i < getNumRows(); i++)
        {
            LayoutResourceFindDataEntry * entry; ///< Entry that will be inserted.

            // Allocates the new entry.
            entry = new LayoutResourceFindDataEntry(getPhysicalRow() + i, getRowTrackId(i), cycle, drawReceivedDisp, draldb);

            // Checks if the entry has any item inside.
            if(entry->isValidDataEntry())
            {
                // If so, inserts it in the data set, that will sort it.
                entries->insert(entry);
            }
            else
            {
                // Stupid creation and deletion...
                delete entry;
            }
        }
    }
    if((getRowShowLow() || (getRowShowHigh() && (drawOnMoveDisp.division == 1))) && env->getFindLow())
    {
        CYCLE cycle = internalCycle;

        if(cycle.division == 0)
        {
            cycle.division = 1;
        }

        // For all the rows.
        for(UINT32 i = 0; i < getNumRows(); i++)
        {
            LayoutResourceFindDataEntry * entry; ///< Entry that will be inserted.

            // Allocates the new entry.
            entry = new LayoutResourceFindDataEntry(getPhysicalRow() + i, getRowTrackId(i), cycle, drawReceivedDisp, draldb);

            // Checks if the entry has any item inside.
            if(entry->isValidDataEntry())
            {
                // If so, inserts it in the data set, that will sort it.
                entries->insert(entry);
            }
            else
            {
                // Stupid creation and deletion...
                delete entry;
            }
        }
    }
}

/*
 * Computes the color for the different divisions of the row.
 *
 * @return void.
 */
void
LayoutResourceRow::computeCSL(QProgressDialog * progress, QLabel * label)
{
    if(type == SimpleEdgeRow)
    {
        // In this stage we try to get an item in the last cycle in both high and low
        // phases. In which phase an item appears, that's the working phase for this row.
        CYCLE tempCycle = draldb->getFirstEffectiveCycle(clockId);
        TAG_ID slot_item = draldb->getSlotItemTagId();
        TagHandler * hnd;

        // OK, by now, we have nothing to show.
        showHigh = false;
        showLow = false;

        // We access to all the rows.
        for(UINT32 j = 0; j < getNumRows(); j++)
        {
            // We create a handler that iterates along the items inside this row.
            draldb->initTagHandler(&hnd, getRowTrackId(j), slot_item, tempCycle, true);

            // We iterate until we find something or the end of the tag handler is reached.
            // If some item is inside in the first or second iteration the showHigh will become
            // true, if no item is inside, in the second iteration the tag cycle will be inf.
            while(hnd->getTagCycle().cycle < BIGGEST_CYCLE)
            {
                // OK!!! we've found something.
                if(hnd->getTagDefined())
                {
                    // It's in the high phase??
                    if(hnd->getTagCycle().division == 0)
                    {
                        showHigh = true;
                    }
                    else
                    {
                        showLow = true;
                    }
                    break;
                }
                hnd->skipToNextCycleWithChange();
            }

            delete hnd;
        }
    }
    internalComputeCSL(progress, label);
}

/*
 * Method to compute the colors and shape of the events of a given
 * chunk of cycles.
 *
 * @return void.
 */
void
LayoutResourceRow::internalComputeCSL(QProgressDialog * progress, QLabel * label)
{
    RuleEvalEnv env;            ///< Rule evaluation evironment.
    TagFlipHandlerEnv flip_env; ///< Environment of the tag flip handler.
    UINT64 value;               ///< Value used to get the items.
    UINT32 track_id;            ///< The track id of the actual cell.
    CYCLE cur_cycle;            ///< Current cycle.
    INT32 first_cycle;          ///< First cycle.
    INT32 last_cycle;           ///< Last cycle.
    CYCLE next_change_cycle;    ///< Next cycle where something changes.
    TAG_ID num_tags;            ///< Stores the number of tags used in this trace.
    TAG_ID slot_item;           ///< Tag that has the item inside a slot.
    bool something_changed;     ///< True if something has changed when skipToNextCycleWithChange has been called.

    // Sets the clock id.
    if(type == SimpleNodeRow)
    {
        DralGraphNode * node = draldb->getNode(dralId);
        clockId = node->getClockId();
        flip_env.nodeTrackId = draldb->resolveTrackIdForNode(dralId);
    }
    else if(type == SimpleEdgeRow)
    {
        DralGraphEdge * edge = draldb->getEdge(dralId);
        clockId = edge->getClockId();
        flip_env.nodeTrackId = draldb->resolveTrackIdForEdge(dralId);
    }

    drawOnMoveDisp.clock = draldb->getFirstEffectiveCycle(clockId).clock;
    drawOnMoveDisp.cycle = 0;
    drawOnMoveDisp.division = 0;

    if(!showHigh && !showLow)
    {
        return;
    }

    // TODO: revise if moving this variable to static of the class with the maximum space allocated, so
    // the alloc and free is just done once.
    num_tags = draldb->getNumTags();
    slot_item = draldb->getSlotItemTagId();

    // Allocates space for all the tags used actually.
    env.itemTags.valids = new bool[num_tags];
    env.itemTags.values = new UINT64[num_tags];
    env.slotTags.valids = new bool[num_tags];
    env.slotTags.values = new UINT64[num_tags];
    env.nodeTags.valids = new bool[num_tags];
    env.nodeTags.values = new UINT64[num_tags];
    env.cycleTags.valids = new bool[num_tags];
    env.cycleTags.values = new UINT64[num_tags];

    last_cycle = draldb->getLastEffectiveCycle(clockId).cycle + 1;

    // Initializes the flip environment.
    flip_env.numTags = num_tags;
    flip_env.itemTags = env.itemTags;
    flip_env.usedItemTags = &item_tags;
    flip_env.slotTags = env.slotTags;
    flip_env.usedSlotTags = &slot_tags;
    flip_env.nodeTags = env.nodeTags;
    flip_env.usedNodeTags = &node_tags;
    flip_env.cycleTags = env.cycleTags;
    flip_env.usedCycleTags = &cycle_tags;
    flip_env.cycleTrackId = draldb->resolveTrackIdForCycleTag(clockId);

    first_cycle = draldb->getFirstEffectiveCycle(clockId).cycle;

    // Switches on the type of row.
    switch(type)
    {
        case SimpleEdgeRow:
            initRuleEvalStructs();

            if(!getRowIsDrawOnMove())
            {
                UINT32 latency;

                latency = draldb->getEdge(getRowDralId())->getLatency();

                // Changes the phases to cycle.
                drawOnMoveDisp.cycle = latency / drawOnMoveDisp.clock->getDivisions();
                drawOnMoveDisp.division = latency % drawOnMoveDisp.clock->getDivisions();

            }

            // Iterates through all the elements of the multirow.
            for(UINT32 j = 0; j < getNumRows(); j++)
            {
                TagFlipHandler flips; ///< Flip handler used to know when something changes.

                flip_env.slotTrackId = getRowTrackId(j);
                env.trackId = getRowTrackId(j);

                // Initializes the tag flip handler.
                draldb->initTagFlipHandler(&flips, &flip_env);

                cur_cycle = draldb->getFirstEffectiveCycle(clockId);

                // Compute the colors for all the cycles.
                while(cur_cycle.cycle < last_cycle)
                {
                    something_changed = true;

                    // Performs consecutive changes until a gap is found.
                    while(something_changed)
                    {
                        // Updates all the data until the current cycle and gets the next cycle.
                        something_changed = flips.skipToNextCycleWithChange(cur_cycle, &next_change_cycle);

                        if(cur_cycle.division == 0)
                        {
                            // We are in a high "moment".
                            if(getRowShowHigh())
                            {
                                internalFinalComputeCSL(&env, cur_cycle, drawOnMoveDisp, j, first_cycle, &rulesHigh);
                            }

                            if(getRowShowLow())
                            {
                                // If we also want to paint lows, move to the the low phase.
                                cur_cycle.division = 1;
                            }
                            else
                            {
                                // Else, we move to the next change.
                                something_changed = false;
                            }
                        }
                        else
                        {
                            // We are in a low "moment".
                            if(getRowShowLow())
                            {
                                internalFinalComputeCSL(&env, cur_cycle, drawOnMoveDisp, j, first_cycle, &rulesHigh);
                            }

                            if(getRowShowHigh())
                            {
                                // If we also want to paint highs, move to the the next cycle.
                                cur_cycle.division = 0;
                                cur_cycle.cycle++;
                            }
                            else
                            {
                                // Else, we move to the next change.
                                something_changed = false;
                            }
                        }
                    }

                    // Avoids out of bounds cycles.
                    next_change_cycle.cycle = QMIN(next_change_cycle.cycle, last_cycle);

                    // Updates the current cycle.
                    cur_cycle = next_change_cycle;
                }

                // Compresses the color.
                draldb->compressTrackTag(getRowTrackId(j), tagIdColor, cur_cycle, true);

                // Updates the progress state.
                if(((getPhysicalRow() + j) & steps) == 0)
                {
                    progress->setProgress(getPhysicalRow() + j);
                    label->setText(QString("\nResource View is computing Colors...               ") +
                                   QString("\nRows                      : ") + QString::number(getPhysicalRow() + j));
                }
            }
            // An edge just shows one phase. If the division of the latency is not zero,
            // the phase shown is inversed.
            if(drawOnMoveDisp.division != 0)
            {
                showHigh = !showHigh;
                showLow = !showLow;
            }
            break;

        case SimpleNodeRow:
            initRuleEvalStructs();

            // Iterates through all the elements of the multirow.
            for(UINT32 j = 0; j < getNumRows(); j++)
            {
                TagFlipHandler flips; ///< Flip handler used to know when something changes.

                flip_env.slotTrackId = getRowTrackId(j);
                env.trackId = getRowTrackId(j);

                // Initializes the tag flip handler.
                draldb->initTagFlipHandler(&flips, &flip_env);

                cur_cycle = draldb->getFirstEffectiveCycle(clockId);

                // Compute the colors for all the cycles.
                while(cur_cycle.cycle < last_cycle)
                {
                    something_changed = true;

                    while(something_changed)
                    {
                        // Updates all the data until the current cycle and gets the next cycle.
                        something_changed = flips.skipToNextCycleWithChange(cur_cycle, &next_change_cycle);

                        if(cur_cycle.division == 0)
                        {
                            // We are in a high "moment".
                            if(getRowShowHigh())
                            {
                                // If we want to paint highs, then lets go!
                                internalFinalComputeCSL(&env, cur_cycle, j, first_cycle, &rulesHigh);
                            }

                            if(getRowShowLow())
                            {
                                // If we also want to paint lows, move to the low phase.
                                cur_cycle.division = 1;
                            }
                            else
                            {
                                // Else, we move to the next change.
                                something_changed = false;
                            }
                        }
                        else
                        {
                            // We are in a low "moment".
                            if(getRowShowLow())
                            {
                                // If we want to paint lows, then lets go!
                                internalFinalComputeCSL(&env, cur_cycle, j, first_cycle, &rulesLow);
                            }

                            if(getRowShowHigh())
                            {
                                // If we also want to paint highs, move to the the next cycle.
                                cur_cycle.division = 0;
                                cur_cycle.cycle++;
                            }
                            else
                            {
                                // Else, we move to the next change.
                                something_changed = false;
                            }
                        }
                    }

                    // Avoids out of bounds cycles.
                    next_change_cycle.cycle = QMIN(next_change_cycle.cycle, last_cycle);

                    // Updates the current cycle.
                    cur_cycle = next_change_cycle;
                }

                // Compresses the color.
                draldb->compressTrackTag(getRowTrackId(j), tagIdColor, cur_cycle, true);

                // Updates the progress state.
                if(((getPhysicalRow() + j) & steps) == 0)
                {
                    progress->setProgress(getPhysicalRow() + j);
                    label->setText(QString("\nResource View is computing Colors...               ") +
                                   QString("\nRows                      : ") + QString::number(getPhysicalRow() + j));
                }
            }
            break;

        case SimpleBlankRow:
            // Nothing to be computed in this case.
            break;

        default:
            // All the other types are not supported.
            Q_ASSERT(false);
            break;
    }

    freeRuleEvalStructs();

    delete [] env.itemTags.valids;
    delete [] env.itemTags.values;
    delete [] env.slotTags.valids;
    delete [] env.slotTags.values;
    delete [] env.nodeTags.valids;
    delete [] env.nodeTags.values;
    delete [] env.cycleTags.valids;
    delete [] env.cycleTags.values;
}

/*
 * Gets all the needed tags of the different structs for
 * the given list of expressions.
 *
 * @return void.
 */
void
LayoutResourceRow::initRuleEvalStructs()
{
    // Blank rows don't compute anything.
    if(type == SimpleBlankRow)
    {
        return;
    }

    slot_tags.clear();
    slot_tags.insert(draldb->getSlotItemTagId());

    Expression * exp;

    {
        ExpressionIterator it_exp(rulesHigh);

        // Computes the list of used slot tags.
        while((exp = it_exp.current()) != NULL)
        {
            ++it_exp;
            exp->getUsedTags(&slot_tags, &item_tags, &node_tags, &cycle_tags);
        }
    }

    {
        ExpressionIterator it_exp(rulesLow);

        // Computes the list of used slot tags.
        while((exp = it_exp.current()) != NULL)
        {
            ++it_exp;
            exp->getUsedTags(&slot_tags, &item_tags, &node_tags, &cycle_tags);
        }
    }
}

/*
 * Deletes all the needed tags.
 *
 * @return void.
 */
void
LayoutResourceRow::freeRuleEvalStructs()
{
    slot_tags.clear();
    item_tags.clear();
    node_tags.clear();
    cycle_tags.clear();
}

/*
 * Method to paint all the node events as points
 *
 * @param x1 Starting horizontal coordinate. This param is constant
 * @param x2 Ending horizontal coordinate. This param is constant
 * @param y1 Starting vertical coordinate. This param is modifyed by each
 *           node after the painting process. 
 * @param y2 Ending vertical coordinate. This param is constant
 * @param p Painter where to paint
 */
void
LayoutResourceRow::drawSubSamplingScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, INT32 stepx, INT32 stepy, QPainter * p, LayoutResourceCache * cache, bool paint_events)
{
    if(getRowShowHigh())
    {
        internalDrawSubSamplingScale(0, useDivisions, col1, col2, * row1, row2, stepx, stepy, p, cache, paint_events);
    }
    if(getRowShowLow())
    {
        internalDrawSubSamplingScale(1, useDivisions, col1, col2, * row1, row2, stepx, stepy, p, cache, paint_events);
    }
    * row1 = getVirtualRow(cache) + getNumRows();
}

/*
 * Method to paint all the node events with their color and without their border
 *
 * @param x1 Starting horizontal coordinate. This param is constant
 * @param x2 Ending horizontal coordinate. This param is constant
 * @param y1 Starting vertical coordinate. This param is modifyed by each
 *           node after the painting process. 
 * @param y2 Ending vertical coordinate. This param is constant
 * @param p Painter where to paint
 */
void 
LayoutResourceRow::drawNoFullScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, QPainter * p, LayoutResourceCache * cache, bool paint_events)
{
    if(getRowShowHigh())
    {
        internalDrawNoFullScale(0, useDivisions, col1, col2, * row1, row2, p, cache, paint_events);
    }
    if(getRowShowLow())
    {
        internalDrawNoFullScale(1, useDivisions, col1, col2, * row1, row2, p, cache, paint_events);
    }
    * row1 = getVirtualRow(cache) + getNumRows();
}

/*
 * Method to paint all the node events with their borders and colors
 *
 * @param x1 Starting horizontal coordinate. This param is constant
 * @param x2 Ending horizontal coordinate. This param is constant
 * @param y1 Starting vertical coordinate. This param is modifyed by each
 *           node after the painting process. 
 * @param y2 Ending vertical coordinate. This param is constant
 * @param p Painter where to paint
 */
void 
LayoutResourceRow::drawFullScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, QPainter * p, AScrollView * asv, LayoutResourceCache * cache, bool paint_events)
{
    if(getRowShowHigh())
    {
        internalDrawFullScale(0, useDivisions, col1, col2, * row1, row2, p, asv, cache, paint_events);
    }
    if(getRowShowLow())
    {
        internalDrawFullScale(1, useDivisions, col1, col2, * row1, row2, p, asv, cache, paint_events);
    }
    * row1 = getVirtualRow(cache) + getNumRows();
}

/*
 * Method to paint all the node events as points
 *
 * @param x1 Starting horizontal coordinate. This param is constant
 * @param x2 Ending horizontal coordinate. This param is constant
 * @param y1 Starting vertical coordinate. This param is modifyed by each
 *           node after the painting process. 
 * @param y2 Ending vertical coordinate. This param is constant
 * @param p Painter where to paint
 */
void
LayoutResourceRow::internalDrawSubSamplingScale(UINT8 division, bool useDivisions, INT32 col1, INT32 col2, INT32 row1, INT32 row2, INT32 stepx, INT32 stepy, QPainter * p, LayoutResourceCache * cache, bool paint_events)
{
    UINT64 value;
    ColorShapeItem csh;

    UINT32 initialRow = row1 - getVirtualRow(cache);
    if(type == SimpleBlankRow)
    {
        if(background != QColor(255, 255, 255))
        {
            p->setBrush(QBrush(background, Qt::Dense5Pattern));
            p->setPen(Qt::NoPen);
            DrawingTools::drawRectangle((col1 * CUBE_SIDE) - 1, (row1 * CUBE_SIDE) - 1, p, (CUBE_SIDE * (col2 - col1)) + 2, (CUBE_SIDE * (getNumRows() - initialRow)) + 2);
            p->setPen(QPen(QColor(0, 0, 0), CUBE_BORDER));
        }
    }
    else if(paint_events)
    {
        CYCLE cycle(draldb->getClockDomain(clockId), 0, division);
        const ClockDomainEntry * clock; ///< Clock domain of the row.
        INT32 disp;
        INT32 first_cycle;
        INT32 last_cycle;

        // Gets the clock domain.
        clock = draldb->getClockDomain(clockId);
        first_cycle = draldb->getFirstEffectiveCycle(clockId).cycle;
        last_cycle = draldb->getLastEffectiveCycle(clockId).cycle;

        // We use the displacement as a shift, because is faster than a mul and
        // the compilar probably won't optimize it...
        if(!useDivisions)
        {
            disp = 0;
        }
        else
        {
            disp = 1;
        }

        // Computes the columns to paint using the clock ratio.
        col1 = (INT32) floorf((float) col1 / clock->getFastestRatio());
        col2 = (INT32) ceilf((float) col2 / clock->getFastestRatio());
        col2 = QMIN(col2, (last_cycle - first_cycle) << disp);

        // Division mode.
        if(useDivisions)
        {
            if(getRowShowExpanded())
            {
                // This row is expanded, so we must move to an even col.
                if(col1 & 1)
                {
                    col1--;
                }
            }
            else
            {
                // This row is unexpanded...
                if(division == 0)
                {
                    // ...so if we want to show highs, we must move to an even col.
                    if(col1 & 1)
                    {
                        col1++;
                    }
                }
                else
                {
                    // ...so if we want to show lows, we must move to an odd col.
                    if((col1 & 1) == 0)
                    {
                        col1++;
                    }
                }
            }
        }

        // Draws all the rows.
        for(UINT32 i = initialRow; i < getNumRows(); i++)
        {
            INT32 myCol = col1;

            // Sets the first cycle.
            if(useDivisions)
            {
                cycle.cycle = col1 >> 1;
            }
            else
            {
                cycle.cycle = col1;
            }

            cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

            // Paints all the columns of this row.
            while(myCol <= col2)
            {
                // Gets the value stored in the database.
                if(draldb->getTrackTagValue(getRowTrackId(i), tagIdColor, cycle, &value))
                {
                    // Gets the color.
                    csh = cslCache.getColorShape(cycle.cycle - first_cycle, getPhysicalRow() + i, division, (INT32) value);
                    if(!csh.getTransparency())
                    {
                        // Paints the point.
                        p->setPen(QPen(AColorPalette::getColor(csh.getColorIdx()),1));
                        p->drawPoint((INT32) ((float) (myCol * CUBE_SIDE) * clock->getFastestRatio()), row1 * CUBE_SIDE);
                    }
                }
                // Moves the next cycle.
                myCol += stepx << disp;
                cycle.cycle += stepx;
            }
            // Moves to the next row.
            row1 += stepy;
        }
    }
}

/*
 * Method to paint all the node events with their color and without their border
 *
 * @param x1 Starting horizontal coordinate. This param is constant
 * @param x2 Ending horizontal coordinate. This param is constant
 * @param y1 Starting vertical coordinate. This param is modifyed by each
 *           node after the painting process. 
 * @param y2 Ending vertical coordinate. This param is constant
 * @param p Painter where to paint
 */
void 
LayoutResourceRow::internalDrawNoFullScale(UINT8 division, bool useDivisions, INT32 col1, INT32 col2, INT32 row1, INT32 row2, QPainter * p, LayoutResourceCache * cache, bool paint_events)
{
    UINT64 value;
    QColor color;
    ColorShapeItem csh;

    UINT32 initialRow = row1 - getVirtualRow(cache);
    if(type == SimpleBlankRow)
    {
        if(background != QColor(255, 255, 255))
        {
            p->setBrush(QBrush(background, Qt::Dense5Pattern));
            p->setPen(Qt::NoPen);
            DrawingTools::drawRectangle((col1 * CUBE_SIDE) - 1, (row1 * CUBE_SIDE) - 1, p, (CUBE_SIDE * (col2 - col1)) + 2, (CUBE_SIDE * (getNumRows() - initialRow)) + 2);
            p->setPen(QPen(QColor(0, 0, 0), CUBE_BORDER));
        }
    }
    else if(paint_events)
    {
        CYCLE cycle(draldb->getClockDomain(clockId), 0, division);
        const ClockDomainEntry * clock; ///< Clock domain of the row.
        INT32 rect_width;
        INT32 disp;
        INT32 first_cycle;
        INT32 last_cycle;

        // Gets the clock domain.
        clock = draldb->getClockDomain(clockId);
        first_cycle = draldb->getFirstEffectiveCycle(clockId).cycle;
        last_cycle = draldb->getLastEffectiveCycle(clockId).cycle;

        if(!useDivisions)
        {
            disp = 1;
            rect_width = (INT32) ((float) CUBE_SIDE * clock->getFastestRatio());
        }
        else if(getRowShowExpanded())
        {
            disp = 2;
            rect_width = (INT32) ((float) CUBE_SIDE * clock->getFastestRatio() * 2.0f);
        }
        else
        {
            disp = 2;
            rect_width = (INT32) ((float) CUBE_SIDE * clock->getFastestRatio());
        }

        // Computes the columns to paint using the clock ratio.
        col1 = (INT32) floorf((float) col1 / clock->getFastestRatio());
        col2 = (INT32) ceilf((float) col2 / clock->getFastestRatio());
        col2 = QMIN(col2, (last_cycle - first_cycle) * disp);

        // Division mode.
        if(useDivisions)
        {
            if(getRowShowExpanded())
            {
                // This row is expanded, so we must move to an even col.
                if(col1 & 1)
                {
                    col1--;
                }
            }
            else
            {
                // This row is unexpanded...
                if(division == 0)
                {
                    // ...so if we want to show highs, we must move to an even col.
                    if(col1 & 1)
                    {
                        col1++;
                    }
                }
                else
                {
                    // ...so if we want to show lows, we must move to an odd col.
                    if((col1 & 1) == 0)
                    {
                        col1++;
                    }
                }
            }
        }

        // Draws all the rows.
        for(UINT32 i = initialRow; i < getNumRows(); i++)
        {
            INT32 myCol = col1;

            // Sets the first cycle.
            if(useDivisions)
            {
                cycle.cycle = col1 >> 1;
            }
            else
            {
                cycle.cycle = col1;
            }

            cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

            // Draws all the columns of this row.
            while(myCol <= col2)
            {
                // Gets the value stored in the database.
                if(draldb->getTrackTagValue(getRowTrackId(i), tagIdColor, cycle, &value))
                {
                    // Gets the CSL.
                    csh = cslCache.getColorShape(cycle.cycle - first_cycle, getPhysicalRow() + i, division, (INT32) value);
                    if(!csh.getTransparency())
                    {
                        // Paints the element.
                        color = AColorPalette::getColor(csh.getColorIdx());
                        p->setPen(QPen(color, 1));
                        p->setBrush(QBrush(color));
                        DrawingTools::drawShape((INT32) ((float) (myCol * CUBE_SIDE) * clock->getFastestRatio()), row1 * CUBE_SIDE, p, csh.getShape(), rect_width, CUBE_SIDE);
                    }
                }

                // Moves to the next column.
                myCol += disp;
                cycle.cycle++;
            }

            // Moves to the next row.
            row1++;
        }
    }
}

/*
 * Method to paint all the node events with their borders and colors
 *
 * @param x1 Starting horizontal coordinate. This param is constant
 * @param x2 Ending horizontal coordinate. This param is constant
 * @param y1 Starting vertical coordinate. This param is modifyed by each
 *           node after the painting process. 
 * @param y2 Ending vertical coordinate. This param is constant
 * @param p Painter where to paint
 */
void 
LayoutResourceRow::internalDrawFullScale(UINT8 division, bool useDivisions, INT32 col1, INT32 col2, INT32 row1, INT32 row2, QPainter * p, AScrollView * asv, LayoutResourceCache * cache, bool paint_events)
{
    UINT64 value;
    ColorShapeItem csh;

    UINT32 initialRow = row1 - getVirtualRow(cache);
    if(type == SimpleBlankRow)
    {
        if(background != QColor(255, 255, 255))
        {
            p->setBrush(QBrush(background, Qt::Dense5Pattern));
            p->setPen(Qt::NoPen);

            DrawingTools::drawRectangle((col1 * CUBE_SIDE) - 1, (row1 * CUBE_SIDE) - 1, p, (CUBE_SIDE * (col2 - col1)) + 2, (CUBE_SIDE * (getNumRows() - initialRow)) + 2);
            p->setPen(QPen(QColor(0, 0, 0), CUBE_BORDER));
        }
    }
    else if(paint_events)
    {
        CYCLE cycle(draldb->getClockDomain(clockId), 0, division);
        const ClockDomainEntry * clock; ///< Clock domain of the row.
        INT32 rect_width;
        INT32 disp;
        INT32 first_cycle;
        INT32 last_cycle;

        // Gets the clock domain.
        clock = draldb->getClockDomain(clockId);
        first_cycle = draldb->getFirstEffectiveCycle(clockId).cycle;
        last_cycle = draldb->getLastEffectiveCycle(clockId).cycle;

        if(!useDivisions)
        {
            disp = 1;
            rect_width = (INT32) ((float) CUBE_SIDE * clock->getFastestRatio());
        }
        else if(getRowShowExpanded())
        {
            disp = 2;
            rect_width = (INT32) ((float) CUBE_SIDE * clock->getFastestRatio() * 2.0f);
        }
        else
        {
            disp = 2;
            rect_width = (INT32) ((float) CUBE_SIDE * clock->getFastestRatio());
        }

        // Computes the columns to paint using the clock ratio.
        col1 = (INT32) floorf((float) col1 / clock->getFastestRatio());
        col2 = (INT32) ceilf((float) col2 / clock->getFastestRatio());
        col2 = QMIN(col2, (last_cycle - first_cycle) * disp);

        // Division mode.
        if(useDivisions)
        {
            if(getRowShowExpanded())
            {
                // This row is expanded, so we must move to an even col.
                if(col1 & 1)
                {
                    col1--;
                }
            }
            else
            {
                // This row is unexpanded...
                if(division == 0)
                {
                    // ...so if we want to show highs, we must move to an even col.
                    if(col1 & 1)
                    {
                        col1++;
                    }
                }
                else
                {
                    // ...so if we want to show lows, we must move to an odd col.
                    if((col1 & 1) == 0)
                    {
                        col1++;
                    }
                }
            }
        }

        // Draws all the rows.
        for(UINT32 i = initialRow; i < getNumRows(); i++)
        {
            INT32 myCol = col1;

            // Sets the first cycle.
            if(useDivisions)
            {
                cycle.cycle = col1 >> 1;
            }
            else
            {
                cycle.cycle = col1;
            }

            cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

            // Draws all the columns of this row.
            while(myCol <= col2)
            {
                // Gets the value stored in the database.
                if(draldb->getTrackTagValue(getRowTrackId(i), tagIdColor, cycle, &value))
                {
                    // Gets the color.
                    csh = cslCache.getColorShape(cycle.cycle - first_cycle, getPhysicalRow() + i, division, static_cast<INT32>(value));

                    // brush selection: if no fill_color/transparency has been set
                    // use special pattern to indicate it
                    if(!csh.getTransparencySet() && !csh.getFillColorSet())
                    {
                        p->setBrush(QBrush(QColor(255, 0, 0), Qt::DiagCrossPattern));
                    }
                    else
                    {
                        if(!csh.getTransparency())
                        {
                            p->setBrush(QBrush(AColorPalette::getColor(csh.getColorIdx())));
                        }
                        else
                        {
                            p->setBrush(Qt::NoBrush);
                        }
                    }

                    // Paints the CSL element.
                    DrawingTools::drawShape((INT32) ((float) (myCol * CUBE_SIDE) * clock->getFastestRatio()), row1 * CUBE_SIDE,
                            p, csh.getShape(), QChar(csh.getLetter()), AColorPalette::getColor(csh.getLetterColorIdx()),
                            rect_width, CUBE_SIDE, asv);
                }
                // Moves to the next column.
                myCol += disp;
                cycle.cycle++;
            }
            // Moves to the next row.
            row1++;
        }
    }
}

void
LayoutResourceRow::dumpRegression()
{
    char str[256];      ///< Temporal buffer to dump data.
    ColorShapeItem csh; ///< Color and shape of the dumped cell.
    QColor color;       ///< Temporal color variable.
    EventShape shape;   ///< Temporal shape variable.
    UINT64 value;       ///< Used to gather the color cache index.
    INT32 R, G, B;      ///< RGB color components.
    INT32 cycle;        ///< Iterates through the cycles.
    UINT16 row;         ///< Iterates through the rows.

    if(type != SimpleBlankRow)
    {
        CYCLE firstCycle;
        CYCLE lastCycle;

        firstCycle = draldb->getFirstEffectiveCycle(clockId);
        lastCycle = draldb->getLastEffectiveCycle(clockId);

        // Iterates through all the rows (multirow case).
        for(row = 0; row < getNumRows(); row++)
        {
            TagHandler * hnd;

            draldb->initTagHandler(&hnd, getRowTrackId(row), tagIdColor, firstCycle, true);

            while(hnd->getTagCycle().cycle < lastCycle.cycle)
            {
                if(hnd->getTagDefined())
                {
                    CYCLE temp;

                    temp = hnd->getTagCycle();
                    csh = cslCache.getColorShape(temp.cycle, getPhysicalRow() + row, temp.division, (INT32) hnd->getTagValue());
                    color = AColorPalette::getColor(csh.getColorIdx());
                    color.getRgb(&R, &G, &B);
                    shape = csh.getShape();

                    // I index.
                    sprintf(str, FMT32X, getPhysicalRow() + row);
                    printf("R=%s,", str);

                    // J index.
                    sprintf(str, FMT32X, temp.cycle);
                    printf("Cyc=%s,", str);

                    // R color.
                    sprintf(str, FMT32X, R);
                    printf("Col=(%s,", str);

                    // G color.
                    sprintf(str, FMT32X, G);
                    printf("%s,", str);

                    // B color.
                    sprintf(str, FMT32X, B);
                    printf("%s),", str);

                    // Shape.
                    sprintf(str, FMT32X, shape);
                    printf("S=%s\n", str);
                }

                hnd->skipToNextCycleWithChange();
            }
        }
    }
}

void
LayoutResourceRow::getEvaluationDescription(UINT32 row, CYCLE cycle, ExpressionList * rules, RuleEvalEnvDesc * desc)
{
    RuleEvalEnv env;      ///< Rule evaluation evironment.
    ItemHandler hnd;      ///< Handler to obtain the item tags.
    TAG_ID num_tags;      ///< Stores the number of tags used in this trace.

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

    // Sets all the tags as invalid.
    memset(env.itemTags.valids, 0, num_tags);
    memset(env.slotTags.valids, 0, num_tags);
    memset(env.nodeTags.valids, 0, num_tags);
    memset(env.cycleTags.valids, 0, num_tags);

    // Gets all the tags of the slot.
    for(TAG_ID i = 0; i < num_tags; i++)
    {
        env.slotTags.valids[i] = draldb->getTrackTagValue(getRowTrackId(row), i, cycle, &env.slotTags.values[i]);
    }

    // If an item is inside the slot.
    if(getRowItemInside(row, cycle, &hnd))
    {
        // Gets all the tags of the item.
        while(hnd.isValidTagHandler() && (hnd.getTagCycle() <= cycle))
        {
            env.itemTags.valids[hnd.getTagId()] = true;
            env.itemTags.values[hnd.getTagId()] = hnd.getTagValue();
            hnd.nextTag();
        }
    }

    // Fills the rest of the environment.
    env.trackId = getRowTrackId(row);
    env.cslResult.reset();
    env.cycle = cycle.cycle;

    // Evaluates the rules.
    RuleEngine::eval(rules, &env, desc);

    // Deletes the allocated items.
    delete [] env.itemTags.valids;
    delete [] env.itemTags.values;
    delete [] env.slotTags.valids;
    delete [] env.slotTags.values;
    delete [] env.nodeTags.valids;
    delete [] env.nodeTags.values;
    delete [] env.cycleTags.valids;
    delete [] env.cycleTags.values;
}

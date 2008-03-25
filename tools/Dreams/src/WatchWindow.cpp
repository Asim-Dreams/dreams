/*
 * Copyright (C) 2004-2006 Intel Corporation
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

// General includes.
#include <iostream>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "WatchWindow.h"

// Asim includes.
#include "asim/IconFactory.h"

WatchWindow::WatchWindow(DralDB * db)
    : draldb(db), orderByColunmn(NULL), capacities(NULL), accCapacities(NULL)
{ 
    nSameCaption = 0;
    nTags = 0;
    nPointerColumn = 0;
    itemTags.valids = NULL;
    itemTags.values = NULL;
}

WatchWindow::~WatchWindow() 
{ 
    if(capacities)
    {
        delete [] capacities;
        delete [] accCapacities;
    }
    if(itemTags.valids)
    {
        delete [] itemTags.valids;
        delete [] itemTags.values;
    }
}

void 
WatchWindow::addRules(ExpressionList &rules)
{
    ExpressionIterator iter(rules);
    Expression *rule;
    while((rule = iter.current()) != NULL)
    {
        ++iter;
        qlistRules.append(rule);
    }
}

bool 
WatchWindow::setOrderBy(TAG_ID tagId)
{
    QPtrListIterator<WatchColumn> iter(qlColumn);
    WatchColumn *column = NULL;
    bool bFound = false;
    while(((column = iter.current()) != NULL) && !bFound)
    {
        ++iter;
        if(column->getTagId() == tagId)
        {
            bFound = true;
            orderByColunmn = column;
        }
    }
    return bFound;
}

void 
WatchWindow::setNode(UINT16 _dralId)
{
    dralId = _dralId;
    sCaption = "Node " + draldb->getNode(dralId)->getName(); 

    nodeTrackId = draldb->trackNodeTags(dralId);
    trackIdBase = nodeTrackId + 1;
    watchType = Node;
    clockId = draldb->getNode(dralId)->getClockId();
}

void 
WatchWindow::setEdge(UINT16 _dralId)
{
    dralId = _dralId;
    sCaption = "Edge " + draldb->getEdge(dralId)->getName() + "(" + draldb->getEdge(dralId)->getSourceNodeName() + " -> " + draldb->getEdge(dralId)->getDestinationNodeName() + ")";
    trackIdBase = draldb->trackEdgeTags(dralId);
    watchType = Edge;
    clockId = draldb->getEdge(dralId)->getClockId();
}

/**
 * Sets the capacity of the current watch window based on the node.
 */
void 
WatchWindow::setCapacity(UINT16 dimensions, UINT32 *capacities)
{

    this->dimensions = dimensions;

    Q_ASSERT(dimensions > 0);

    if(capacities)
    {
        delete [] this->capacities;
        delete [] this->accCapacities;
    }
    this->capacities = new UINT32[dimensions];
    this->accCapacities = new UINT32[dimensions];

    totalCapacity = 1;
    for(INT16 i = dimensions - 1; i >= 0; i--) 
    {
        this->capacities[i] = capacities[i];
        totalCapacity *= capacities[i];
        accCapacities[i] = totalCapacity;
    }
}

void
WatchWindow::initRuleEvalStructs(void)
{

    ExpressionIterator it_exp(qlistRules);
    Expression * exp;

    slot_tags.clear();
    slot_tags.insert(draldb->getSlotItemTagId());

    // Computes the list of used slot tags.
    while((exp = it_exp.current()) != NULL)
    {
        ++it_exp;
        exp->getUsedTags(&slot_tags, &item_tags, &node_tags, &cycle_tags);
    }
}

void
WatchWindow::freeRuleEvalStructs(void)
{
}

/*
 * Method to compute the colors and shape of the events of a given
 * chunk of cycles.
 *
 * @param cycle First cycle of the chunk.
 * @param chunk_size Size of the the chunk.
 * @param nrcelist List where the computed events are stored.
 *
 * @return void.
 */
void
WatchWindow::computeCSL(QProgressDialog * progress, QLabel * label, UINT32 &currentCell)
{
    RuleEvalEnv env;                ///< Rule evaluation evironment.
    TagFlipHandlerEnv flip_env;     ///< Environment of the tag flip handler.
    UINT64 value;                   ///< Value used to get the items.
    UINT32 track_id;                ///< The track id of the actual cell.
    INT32 cache_idx;                ///< Row cache index for a given CSL.
    UINT16 num_tags;                ///< Stores the number of tags used in this trace.
    UINT16 slot_item;               ///< Tag id where the items are stored inside a slot.
    CYCLE cur_cycle;                ///< Current cycle.
    INT32 last_cycle;               ///< Last cycle.
    CYCLE next_change_cycle;        ///< Next cycle where something changes.
    const ClockDomainEntry * clock; ///< Clock domain of the edge or node.
    bool something_changed;         ///< True if something has changed when skipToNextCycleWithChange has been called.

    if(watchType == Node)
    {
        flip_env.nodeTrackId = draldb->resolveTrackIdForNode(dralId);
    }
    else
    {
        flip_env.nodeTrackId = draldb->resolveTrackIdForEdge(dralId);
    }

    // TODO: revise if moving this variable to static of the class with the maximum space allocated, so
    // the alloc and free is just done once.
    num_tags = draldb->getNumTags();
    slot_item = draldb->getSlotItemTagId();

    nTags = draldb->getNumTags();

    // Allocates space for all the tags used actually.
    itemTags.valids = new bool[nTags];
    itemTags.values = new UINT64[nTags];

    // Allocates space for all the tags used actually.
    env.itemTags.valids = new bool[num_tags];
    env.itemTags.values = new UINT64[num_tags];
    env.slotTags.valids = new bool[num_tags];
    env.slotTags.values = new UINT64[num_tags];
    env.nodeTags.valids = new bool[num_tags];
    env.nodeTags.values = new UINT64[num_tags];
    env.cycleTags.valids = new bool[num_tags];
    env.cycleTags.values = new UINT64[num_tags];

    initRuleEvalStructs();

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

    // Computes the colors for all the cells in this watch window.
    for(UINT16 cellId = 0; cellId < totalCapacity; cellId++)
    {
        TagFlipHandler flips;   ///< Flip handler used to know when something changes.

        flip_env.slotTrackId = getTrackIdForCell(cellId);
        env.trackId = getTrackIdForCell(cellId);

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

                // Resets the previous coloring.
                env.cslResult.reset();
                env.cycle = cur_cycle.cycle;

                // Evaluates the rules.
                RuleEngine::eval(&qlistRules, &env);

                // If a color is defined.
                if(env.cslResult.getDefined())
                {
                    cache_idx = cslCache.addColorShape(cur_cycle.cycle, cellId, cur_cycle.division, env.cslResult);
                    draldb->addTrackTagValue(getTrackIdForCell(cellId), tagIdColor, cur_cycle, (UINT64) cache_idx);
                }
                else
                {
                    draldb->undefineTrackTagValue(getTrackIdForCell(cellId), tagIdColor, cur_cycle);
                }
            }

            // Avoids out of bounds cycles.
            next_change_cycle.cycle = QMIN(next_change_cycle.cycle, last_cycle);

            // Updates the current cycle.
            cur_cycle = next_change_cycle;
        }

        // Compresses the color.
        draldb->compressTrackTag(getTrackIdForCell(cellId), tagIdColor, cur_cycle, true);

        // Increments the number of rows computed.
        currentCell++;

        if((currentCell % 13) == 0)
        {
            // Updates the progress state.
            progress->setProgress(currentCell);
            label->setText(QString("\nWatch Window is computing Colors...               ") +
                        QString("\nCells                     : ") + QString::number(currentCell));
        }
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

void
WatchWindow::initDraw(QTable *table)
{

    table->setNumRows(getCapacity());

    UINT32 nColumns = getNumCols() - nPointerColumn + ((nPointerColumn > 0) ? 1 : 0);
    table->setNumCols(nColumns);

    // Set the cpolumn caption
    QHeader *colHeader = table->horizontalHeader();
    QPtrListIterator<WatchColumn> iter(qlColumn);
    WatchColumn *currentCol = NULL;
    UINT32 nCol = 0;
    while((currentCol = iter.current()) != NULL)
    {
        ++iter;
        if(!currentCol->getIsPointer())
        {
            colHeader->setLabel(nCol, draldb->getTagName(currentCol->getTagId()));
            table->adjustColumn(nCol);
            nCol++;
        }
    }

    // Pointer column
    if(nPointerColumn > 0) 
    {
        colHeader->setLabel(table->numCols() - 1, "Pointers");
    }

    // Set the row caption
    QHeader *rowHeader = table->verticalHeader();
    for(UINT32 cellId = 0; cellId < totalCapacity; cellId++)
    {
        UINT32 dimPos = cellId;
        QString rowCaption = "";
        for(UINT16 i = 0; i < dimensions - 1; i++)
        {
            UINT32 coordinate = dimPos / (totalCapacity / capacities[i]);
            rowCaption = rowCaption + "[" + QString::number(coordinate) + "]";
            dimPos -= coordinate * accCapacities[i+1];
        }
        rowCaption = rowCaption + "[" + QString::number(dimPos) + "]";
        rowHeader->setLabel(cellId, rowCaption);
    }
}

void
WatchWindow::draw(QTable *table, CYCLE cycle)
{
    // Set the column caption
    QPtrListIterator<WatchColumn> iter(qlColumn);
    WatchColumn *currentCol = NULL;

    UINT32 pointerColumn = table->numCols() - 1;

    // We must assert that the cycle is in the same clock domain.
    Q_ASSERT(clockId == cycle.clock->getId());

    for(UINT32 cellId = 0; cellId < totalCapacity; cellId++)
    {
        QColor letterColor;
        QColor borderColor;
        QColor fillColor;
        bool bFillTransp = true;

        INT32 trackId = getTrackIdForCell(cellId);
        UINT64 cslIndex;
        if(draldb->getTrackTagValue(trackId, tagIdColor, cycle, &cslIndex))
        {
            ColorShapeItem csh = cslCache.getColorShape(cycle.cycle, cellId, cycle.division, (INT32)cslIndex);
            bFillTransp = csh.getTransparency();
            fillColor = AColorPalette::getColor(csh.getColorIdx());
            borderColor = AColorPalette::getColor(csh.getBColorIdx());
            letterColor = AColorPalette::getColor(csh.getLetterColorIdx());
        }

        ItemHandler hnd;
        draldb->getItemInSlot(trackId, cycle, &hnd);

        // Gets the actual state of the item inside.
        memset(itemTags.valids, 0, nTags);

        // Fills the vector with all the values of the actual item.
        while(hnd.isValidTagHandler() && (hnd.getTagCycle().cycle <= cycle.cycle))
        {
            itemTags.valids[hnd.getTagId()] = true;
            itemTags.values[hnd.getTagId()] = hnd.getTagValue();
            hnd.nextTag();
        }

        UINT32 nCol = 0;
        iter.toFirst();

        // Draws all the columns for the current cell.
        while((currentCol = iter.current()) != NULL)
        {
            QString sValue = QString::null;
            if(currentCol->getIsPointer())
            {
                // Pointer columns
                UINT64 nodeValue;
                bool bDefined = draldb->getTrackTagValue(nodeTrackId, currentCol->getTagId(), cycle, &nodeValue);
                if(bDefined && (nodeValue == cellId))
                {
                    drawCell(table, cellId, pointerColumn, IconFactory::getInstance()->image41, currentCol->getPointerName(), currentCol->getAlign(), bFillTransp, fillColor, letterColor);
                }
                else if(table->item(cellId, pointerColumn) && (table->item(cellId, pointerColumn)->text() == currentCol->getPointerName())) 
                {
                    table->clearCell(cellId, pointerColumn);
                }
            }
            else if(currentCol->getIsSlotTag())
            {
                // Slot tags
                draldb->getFormatedTrackTagValue(trackId, currentCol->getTagId(), cycle, &sValue);
                drawCell(table, cellId, nCol, sValue, currentCol->getAlign(), bFillTransp, fillColor, letterColor);
                nCol++;
            }
            else 
            {
                // Item tags
                if(itemTags.valids[currentCol->getTagId()])
                {
                    sValue = draldb->getFormatedTagValue(currentCol->getTagId(), itemTags.values[currentCol->getTagId()]);
                }

                drawCell(table, cellId, nCol, sValue, currentCol->getAlign(), bFillTransp, fillColor, letterColor);
                nCol++;
            }

            ++iter;
        }
    }
    table->repaintContents(false);
}

void 
WatchWindow::drawCell(QTable *table, INT32 row, INT32 col, QPixmap &pixmap, QString &value, Qt::AlignmentFlags align, bool bTransp, QColor fillColor, QColor letterColor)
{
    WatchWindowTableItem *item = static_cast<WatchWindowTableItem *>(table->item(row, col));
    if(item)
    {
        item->setPixmap(pixmap);
    }
    else
    {
        item = new WatchWindowTableItem(table, value);
        item->setPixmap(pixmap);
        table->setItem(row, col, item);
    }
    drawCell(table, row, col, value, align, bTransp, fillColor, letterColor);
}

void 
WatchWindow::drawCell(QTable *table, INT32 row, INT32 col, QString &value, Qt::AlignmentFlags align, bool bTransp, QColor fillColor, QColor letterColor)
{
    WatchWindowTableItem *item = static_cast<WatchWindowTableItem *>(table->item(row, col));
    if(item)
    {
        item->setText(value);
    }
    else
    {
        item = new WatchWindowTableItem(table, value);
        table->setItem(row, col, item);
    }

    item->setAlignment(align);
    item->setTransparency(bTransp);
    item->setFillColor(fillColor);
    item->setLetterColor(letterColor);
}

set<TAG_ID>
WatchWindow::getSlotTags() const
{
    set<TAG_ID> list;
    QPtrListIterator<WatchColumn> iter(qlColumn);
    WatchColumn * column = NULL;

    while((column = iter.current()) != NULL)
    {
        ++iter;
        if(column->getIsSlotTag())
        {
            list.insert(column->getTagId());
        }
    }

    return list;
}

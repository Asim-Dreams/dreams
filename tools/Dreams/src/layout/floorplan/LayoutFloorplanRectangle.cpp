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

/*
 * @file LayoutFloorplanRectangle.cpp
 */

// C
#include <cmath>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "layout/floorplan/LayoutFloorplanRectangle.h"
#include "highlight/HighLightMgrFloorplan.h"
#include "PreferenceMgr.h"
#include "DrawingTools.h"
#include "expressions/RuleEngine.h"
#include "CSLData.h"

/**
 * Access the database and registers all the slots of the node.
 *
 * @param currentDimension Number that indicates wich dimension is being treated
 * @param currentCoordinates
 * @param elemsCreated
 */
void 
LayoutFloorplanRectangle::registerDimensionSlots(INT16 currentDimension, UINT32 *currentCoordinates, UINT32 &elemsCreated)
{
    if(rectType == Node)
    {
        trackIdBase = (draldb->trackNodeTags(dralId) + 1);
    }
    else if(rectType == Edge)
    {
        trackIdBase = draldb->trackEdgeTags(dralId);
    }
    else
    {
       Q_ASSERT(false);
    }
}

/**
 * Sets the capacity of the current rectangle based on the node.
 */
void 
LayoutFloorplanRectangle::setCapacity(UINT16 dimensions, UINT32 *capacities)
{
    this->dimensions = dimensions;

    Q_ASSERT(dimensions > 0);

    if(capacities) delete this->capacities;
    this->capacities = new UINT32[dimensions];
    if(accCapacities) delete this->accCapacities;
    this->accCapacities = new UINT32[dimensions];

    totalCapacity = 1;
    for(INT16 i = dimensions - 1; i >= 0; i--) 
    {
        this->capacities[i] = capacities[i];
        totalCapacity *= capacities[i];
        accCapacities[i] = totalCapacity;
    }

    //cerr << "LayoutFloorplanRectangle::setCapacity: totalCapacity = " << totalCapacity << endl;

    // This array is used in the getTrackIdForCell method. We create it here for the future use
    coordinates = new UINT32[dimensions];

    UINT32 elemsCreated = 0;
    UINT32 *currentCoordinates = new UINT32[dimensions];
    registerDimensionSlots(0, currentCoordinates, elemsCreated);
    delete currentCoordinates;

    // We initialize the layout of the current node, we assume as current
    // layout the square root type
    for(UINT32 dimension = 0; dimension < dimensions; dimension++)
    {
        UINT32 capacity = getCapacity(dimension);
        UINT32 nRow = static_cast<UINT32>(sqrt(static_cast<double>(capacity)));
        UINT32 nCol = nRow;
        if((nRow * nCol) != getCapacity(dimension))
        {
            nRow++;
            nCol++;
        }
        Q_ASSERT((nRow * nCol) >= getCapacity(dimension));

        setLayout(dimension, nRow, nCol);
    }
}

/**
 * Geometry computation algorithm.
 * This algorithm computes the size of the current rectangle based on:
 *  - cell geometry specifyed
 *  - border 
 *  - margin
 *  - node dimensions 
 *  - node capacities
 *
 * The algorithm starts form the most inner dimension and computes the sizes
 * based on the previous dimension size.
 */
void
LayoutFloorplanRectangle::computeGeometry(void)
{
    /*
     * We only add one border and one margin per cell and direction
     * the other is added at the end.
     *
     *      +------+
     *      |      |*
     *      | cell |* <- margin added
     *      |      |*
     *      +------+*
     *      *********
     *      ^
     *      |
     *      margin added
     *
     * The border must be added if present
     */

    //cerr << "LayoutFloorplanRectangle::computeGeometry: Computing geometry for rectangle " << getName() << " dimensions = " << dimensions << endl;

    if(dimensionHeight) delete dimensionHeight;
    if(dimensionWidth) delete dimensionWidth;

    dimensionHeight = new UINT32[dimensions];
    dimensionWidth = new UINT32[dimensions];

    dimensionWidth[dimensions - 1] = (nCellWidth + (bSlotBorder ? 2 * CELL_BORDER : 0) + getMargin());
    dimensionHeight[dimensions - 1] = (nCellHeight + (bSlotBorder ? 2 * CELL_BORDER : 0) + getMargin());
    nWidth  = dimensionWidth[dimensions - 1] * pnLayoutFloorplan[dimensions - 1][0];
    nHeight = dimensionHeight[dimensions - 1] * pnLayoutFloorplan[dimensions - 1][1];

    //cerr << "LayoutFloorplanRectangle::computeGeometry: dimensionHeight[" << (dimensions - 1) << "] = " << dimensionHeight[dimensions - 1] << " dimensionWidth[" << (dimensions - 1) << "] = " << dimensionWidth[dimensions - 1] << endl;

    for(INT16 i = dimensions - 2; i >= 0; i--)
    {
        dimensionWidth[i] = nWidth;
        dimensionHeight[i] = nHeight;

        //cerr << "LayoutFloorplanRectangle::computeGeometry: dimensionHeight[" << i << "] = " << dimensionHeight[i] << " dimensionWidth[" << i << "] = " << dimensionWidth[i] << endl;

        nWidth *= pnLayoutFloorplan[i][0];
        nHeight *= pnLayoutFloorplan[i][1];
    }

    /*
     * At the end we add the global top-left margin
     */
    nWidth += getMargin();
    nHeight += getMargin();

    /*
     * If we're drawing the title of the node we need to modify the
     * nWidth/nHeight accordingly
     */
    if(!bTitleOnTop)
    {
        nWidth  += ((tpTitle == East) || (tpTitle == West)) ? TITLE_HEIGHT : 0;
        nHeight += ((tpTitle == North) || (tpTitle == South)) ? TITLE_HEIGHT : 0;
    }
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
LayoutFloorplanRectangle::computeCSL(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell)
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
    UINT64 dummy;                   ///< Dummy variable.
    const ClockDomainEntry * clock; ///< Clock domain of the edge or node.
    bool something_changed;         ///< True if something has changed when skipToNextCycleWithChange has been called.
    INT32 pendingFadeCycles;        ///<
    UINT32 fade = 0;                ///<
    CSLData cslFading;              ///<
    PreferenceMgr * prefMgr;        ///<
    QColor bg;                      ///<
    
    prefMgr = PreferenceMgr::getInstance("BSSAD");
    bg = prefMgr->getBackgroundColor();

    if(rectType == Node)
    {
        DralGraphNode * node = draldb->getNode(dralId);
        clockId = node->getClockId();
        flip_env.nodeTrackId = draldb->resolveTrackIdForNode(dralId);
    }
    else
    {
        DralGraphEdge * edge = draldb->getEdge(dralId);
        clockId = edge->getClockId();
        flip_env.nodeTrackId = draldb->resolveTrackIdForEdge(dralId);
    }

    // We assume by now two schemes, the first scheme is the opriginal colour
    // and the second is the filtered.
    // Given that this method is invoqued to compute the "original" colour, the currentTagId
    // is the first
    currentTagIdColor = 0;

    // TODO: revise if moving this variable to static of the class with the maximum space allocated, so
    // the alloc and free is just done once.
    num_tags = draldb->getNumTags();
    slot_item = draldb->getSlotItemTagId();
    clock = draldb->getClockDomain(clockId);

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

    // Computes the colors for all the cells in this rectangle.
    for(UINT16 cellId = 0; cellId < totalCapacity; cellId++)
    {
        TagFlipHandler flips;   ///< Flip handler used to know when something changes.
        INT32 cellRow, cellCol; ///< Cell coordinates.

        getCellIdCoordinates(cellId, cellCol, cellRow);
        flip_env.slotTrackId = getTrackIdForCell(cellId);
        env.trackId = getTrackIdForCell(cellId);

        // Initializes the tag flip handler.
        draldb->initTagFlipHandler(&flips, &flip_env);

        cur_cycle = draldb->getFirstEffectiveCycle(clockId);
        pendingFadeCycles = 0;

        // Compute the colors for all the cycles.
        while(cur_cycle.cycle < last_cycle)
        {
            something_changed = flips.skipToNextCycleWithChange(cur_cycle, &next_change_cycle);

            // Avoids out of bounds cycles.
            next_change_cycle.cycle = QMIN(next_change_cycle.cycle, last_cycle);

            // Resets the previous coloring.
            env.cslResult.reset();
            env.cycle = cur_cycle.cycle;

            // Evaluates the rules.
            RuleEngine::eval(&qlistRules, &env);
            //cerr << "After RuleEngine " << env.cslResult.toString() << endl;

            // If a color is defined.
            if(env.cslResult.getDefined())
            {
                cache_idx = cslCache.addColorShape(cur_cycle.cycle, cellRow, cellCol, env.cslResult);
                draldb->addTrackTagValue(env.trackId, tagIdColor[0], cur_cycle, (UINT64) cache_idx);

                // Fading:
                // Because we just defined a new Fill Color, we can
                // not keep playing the fading game. Re-set the
                // pending counter to avoid any more fading
                // computation
                //
                pendingFadeCycles = 0;

                // Fading:
                fade = env.cslResult.getFillColorFade();
                if ( fade > 0 )
                {
                    pendingFadeCycles = fade;
                    cslFading = env.cslResult;
                    cslFading.fadeParams(bg,fade);
                }

            }
            else if(pendingFadeCycles > 0)
            {
                cslFading.fadeStep(fade - pendingFadeCycles);
                cache_idx = cslCache.addColorShape(cur_cycle.cycle, cellRow, cellCol, cslFading);
                draldb->addTrackTagValue(env.trackId, tagIdColor[0], cur_cycle, (UINT64) cache_idx);
                pendingFadeCycles--;
                //cerr << "Fading: (A) cycle=" << cur_cycle << " trackid=" << getTrackIdForCell(cellId) << " cellRow=" << cellRow << " cellCol=" << cellCol << " next_change=" << next_change_cycle << " pendingFade=" << pendingFadeCycles << " lettercolor=" << cslFading.getLetterColor().name() << " cache_idx" << cache_idx << endl;
            }
            else 
            {
                draldb->undefineTrackTagValue(env.trackId, tagIdColor[0], cur_cycle);
            }

            for(cur_cycle.cycle = (cur_cycle.cycle + 1); (cur_cycle.cycle < next_change_cycle.cycle) && pendingFadeCycles; cur_cycle.cycle++, pendingFadeCycles--)
            {
                  cslFading.fadeStep(fade - pendingFadeCycles);
                  cache_idx = cslCache.addColorShape(cur_cycle.cycle, cellRow, cellCol, cslFading);
                  draldb->addTrackTagValue(env.trackId, tagIdColor[0], cur_cycle, (UINT64) cache_idx);
                  //cerr << "Fading: (C) cycle=" << i << " trackid=" << getTrackIdForCell(cellId) << " cellRow=" << cellRow << " cellCol=" << cellCol << " next_change=" << next_change_cycle << " pendingFade=" << pendingFadeCycles << " lettercolor=" << cslFading.getLetterColor().name() << " cache_idx" << cache_idx << endl;
            }

            // Updates the current cycle.
            cur_cycle = next_change_cycle;
        }

        // Compresses the color.
        draldb->compressTrackTag(env.trackId, tagIdColor[0], cur_cycle, true);

        // Increments the number of rows computed.
        * currentCell = (* currentCell) + 1;

        // Updates the progress state.
        if(((* currentCell) & 0x0FF) == 0)
        {
            progress->setProgress(* currentCell);
            label->setText(QString("\nFloorplan is computing Colors...               ") +
                            QString("\nCells                     : ") + QString::number(* currentCell));
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

/*
 * Method to compute the colors and shape of the events of a given
 * chunk of cycles. This method takes the result of compute CSL
 * and apply a number of filters
 *
 * @param cycle First cycle of the chunk.
 * @param chunk_size Size of the the chunk.
 *
 * @return void.
 */
void
LayoutFloorplanRectangle::computeCSLFilters(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell)
{
    CYCLE cycle;                    ///< Cycle used to store data.

    // No filter defined
    if(filter == NULL) 
    {
        *currentCell += totalCapacity;
        progress->setProgress(*currentCell);
        label->setText(QString("\nFloorplan is computing filtered colors...               ") +
                       QString("\nCells                     : ") + QString::number(*currentCell));
        return;
    }

    currentTagIdColor = nTagIdFilter;

    // Obtain the background color
    PreferenceMgr * prefMgr = PreferenceMgr::getInstance("BSSAD");
    QColor bg = prefMgr->getBackgroundColor();
    INT32 tagIdFilter = tagIdColor[nTagIdFilter];

    QColor *streamFillColor = new QColor[filter->GetTaps()];
    //QColor *streamBorderColor = new QColor[filter->GetTaps()];
    //QColor *streamLetterColor = new QColor[filter->GetTaps()];

    for(UINT16 cellId = 0; cellId < totalCapacity; cellId++)
    {
        TRACK_ID trackId = getTrackIdForCell(cellId);

        INT32 cellRow, cellCol; 
        getCellIdCoordinates(cellId, cellCol, cellRow);
    
        //if(getName() == "Node MC{0}")
        //    cerr << "Filtering Slot[" << cellRow << "][" << cellCol << "] trackId = " << trackId << endl;

        cycle = draldb->getFirstEffectiveCycle(clockId);
        INT32 last_cycle = draldb->getLastEffectiveCycle(clockId).cycle + 1;

        // Prologe: 
        //
        // - Fill the stream with bg color up to the central position, where the
        //   initial cycle will reside
        //
        UINT32 i = 0;
        for(; i < (filter->GetTaps() / 2); i++) 
        {
            streamFillColor[i] = bg;
            //streamBorderColor = bg;
            //streamLetterColor = bg;
        }

        //
        // - Fill with the tag values from initial_cycle up to initial_cycle + (taps / 2) + 1
        //
        TagHandler * tagHnd;

        draldb->initTagHandler(&tagHnd, trackId, tagIdColor[0], cycle, true);
        while((i < filter->GetTaps()) && (cycle.cycle < last_cycle))
        {
            if(tagHnd->getTagDefined())
            {
                UINT64 cslIndex = tagHnd->getTagValue();
                ColorShapeItem csh = cslCache.getColorShape(cycle.cycle, cellRow, cellCol, (INT32)cslIndex);
                streamFillColor[i] = (!csh.getTransparency()) ? AColorPalette::getColor(csh.getColorIdx()) : bg;
            }
            else
            {
                streamFillColor[i] = bg;
            }

            tagHnd->nextCycle();
            cycle.cycle++;
            i++;
        }

        UINT32 streamHead = 0;

        // Compute the filtered colors for all the cycles.
        cycle = draldb->getFirstEffectiveCycle(clockId);
        while(cycle.cycle < last_cycle)
        {
            //cerr << "Cycle = " << cycle << " tagDefined = " << tagHnd.getTagDefined() << " tagCycle = " << tagHnd.getTagCycle() << endl;
            if(tagHnd->getTagDefined())
            {
                UINT64 cslIndex = tagHnd->getTagValue();

                ColorShapeItem csh = cslCache.getColorShape(tagHnd->getTagCycle().cycle, cellRow, cellCol, (INT32)cslIndex);
                streamFillColor[streamHead] = (!csh.getTransparency()) ? AColorPalette::getColor(csh.getColorIdx()) : bg;
                //streamBorderColor[i] = getSlotBorder() ? AColorPalette::getColor(csh.getBColorIdx()) : streamFillColor[i];
                //streamLetterColor[i] = (csh.getLetter() != ' ') ? AColorPalette::getColor(csh.getLetterColorIdx()) : streamFillColor[i];
            }
            else
            {
                streamFillColor[streamHead] = bg;
                //streamBorderColor[i] = getSlotBorder() ? Qt::black : streamFillColor[i];
                //streamLetterColor[i] =  streamFillColor[i];
            }

            // DEBUG
            //for(UINT32 i = 0; i < filter->GetTaps(); i++)
            //    cerr << "\t streamFillColor[" << i << "] = " << streamFillColor[i].name() << endl;

            CSLData cslFilter;
            cslFilter.setFillColor(filter->DoTemporalFiltering(streamFillColor, bg));
            //cslFilter.setBorderColor(filter->DoTemporalFiltering(streamBorderColor, bg));
            //cslFilter.setFillColor(filter->DoTemporalFiltering(streamLetterColor, bg));

            INT32 cache_idx = cslCache.addColorShape(cycle.cycle, cellRow, cellCol, cslFilter);
            draldb->addTrackTagValue(trackId, tagIdFilter, cycle, (UINT64) cache_idx);
            //cerr << "Cycle = " << cycle << " ==> filtered color = " << cslFilter.getFillColor().name() << " cacheIdx = " << cache_idx << endl;

            tagHnd->nextCycle();
            cycle.cycle++;
            streamHead = (streamHead + 1) % filter->GetTaps();      ///< This modulus should be changed to a faster way to compute it...
        }

        delete tagHnd;

        // Compresses the filter.
        draldb->compressTrackTag(trackId, tagIdFilter, cycle, true);

        // Increments the number of rows computed.
        *currentCell = (*currentCell) + 1;

        // Updates the progress state.
        if(((*currentCell) & 0x0FF) == 0)
        {
            progress->setProgress(* currentCell);
            label->setText(QString("\nFloorplan is computing filtered colors...               ") +
                            QString("\nCells                     : ") + QString::number(*currentCell));
        }
    }

    delete []streamFillColor;
    //delete []streamBorderColor;
    //delete []streamLetterColor;
}

void
LayoutFloorplanRectangle::initRuleEvalStructs()
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
LayoutFloorplanRectangle::freeRuleEvalStructs()
{
}

/** 
 * Scans the geometry to obtain the cellId that belong to the cell located under 
 * the given point
 *
 * @param refX X reference coordinates 
 * @param refY Y reference coordinates 
 * @return cellId 
 */
INT32 
LayoutFloorplanRectangle::getCellIdAt(UINT32 refX, UINT32 refY) const
{
    // TODO 
    // Do it global observable by all the elements
    // Delta applieds to the coordinathe thanks to the title
    UINT32 nTitleXStart = 0;
    UINT32 nTitleXEnd   = 0;
    UINT32 nTitleYStart = 0;
    UINT32 nTitleYEnd   = 0;
    if(!bTitleOnTop && tpTitle == North)      nTitleYStart = TITLE_HEIGHT; 
    else if(!bTitleOnTop && tpTitle == South) nTitleYEnd = TITLE_HEIGHT;
    else if(!bTitleOnTop && tpTitle == East)  nTitleXEnd = TITLE_HEIGHT;
    else if(!bTitleOnTop && tpTitle == West)  nTitleXStart = TITLE_HEIGHT;
    
    // We need to take into account the fact that maybe the mouse is on the
    // margin
    UINT32 base_x = nTitleXStart + x + getMargin();
    UINT32 base_y = nTitleYStart + y + getMargin();

    rectNode.setLeft(base_x);  
    rectNode.setTop(base_y); 
    rectNode.setRight(base_x + nWidth - 2 * getMargin() - nTitleXEnd); 
    rectNode.setBottom(base_y + nHeight - 2 * getMargin() - nTitleYEnd);

    if(rectNode.contains(QPoint(refX, refY))) 
    {
        //cerr << "LayoutFloorplanRectangle::getCellAt(" << getNodeName() << "): x = " << base_x << " y = " << base_y << " width = " << nWidth << " height = " << nHeight << ": Searching cell at " << getNodeName() << " x = " << refX << " y = " << refY << endl;

        for(UINT16 i = 0; i < dimensions; i++)
        {

            //cerr << "\tdimensionWidth[" << i << "] = " << dimensionWidth[i] << " dimensionHeight[" << i << "] = " << dimensionHeight[i] << endl;

            UINT32 currentRow = (refY - base_y) / dimensionHeight[i];
            UINT32 currentCol = (refX - base_x) / dimensionWidth[i];

            //cerr << "\tcurrentRow " << currentRow << " currentCol " << currentCol << endl;

            coordinates[i] = currentRow * pnLayoutFloorplan[i][0] + currentCol;
            //cerr << "\tcoordinates[" << i << "] = " << coordinates[i] << endl;

            base_x += currentCol * dimensionWidth[i];
            base_y += currentRow * dimensionHeight[i];
        }

        UINT32 nElem = 0;
        for(INT16 i = 0; i < dimensions - 1; i++)
        {
            nElem += coordinates[i] * accCapacities[i + 1];
            //cerr << "\tnElem = " << nElem << " nCapacity = " << accCapacities[i] << endl;
        }
        nElem += coordinates[dimensions - 1];

        if(!(nElem < totalCapacity)) 
        {
            // TODO
            // Verify in the limit of the coordinates what happens
            //cerr << "\tnElem = " << nElem << " totalCapacity = " << totalCapacity << endl;
            //Q_ASSERT(nElem < totalCapacity);
            return -1;
        }

        return nElem;
    }

    return -1;
}

/**
 * Scans the list of sons in order to search the cell located under the point given
 *
 * @param point QPoint that holds the reference coordinates to compare to
 */
QString 
LayoutFloorplanRectangle::getCellAt(QPoint &point)
{
    QString qsResult = QString::null;

    // TODO 
    // Do it global observable by all the elements
    // Delta applieds to the coordinathe thanks to the title
    UINT32 nTitleXStart = 0;
    UINT32 nTitleXEnd   = 0;
    UINT32 nTitleYStart = 0;
    UINT32 nTitleYEnd   = 0;
    if(!bTitleOnTop && tpTitle == North)      nTitleYStart = TITLE_HEIGHT; 
    else if(!bTitleOnTop && tpTitle == South) nTitleYEnd = TITLE_HEIGHT;
    else if(!bTitleOnTop && tpTitle == East)  nTitleXEnd = TITLE_HEIGHT;
    else if(!bTitleOnTop && tpTitle == West)  nTitleXStart = TITLE_HEIGHT;
    
    // We need to take into account the fact that maybe the mouse is on the
    // margin
    UINT32 base_x = nTitleXStart + x + getMargin();
    UINT32 base_y = nTitleYStart + y + getMargin();

    rectNode.setLeft(base_x);  
    rectNode.setTop(base_y); 
    rectNode.setRight(base_x + nWidth - 2 * getMargin() - nTitleXEnd); 
    rectNode.setBottom(base_y + nHeight - 2 * getMargin() - nTitleYEnd);

    if(rectNode.contains(point)) 
    {
        //cerr << "LayoutFloorplanRectangle::getCellAt: Searching cell at " << getNodeName() << " point.x = " << point.x() << " point.y = " << point.y() << endl;

        qsResult = getName();
        for(UINT16 i = 0; i < dimensions; i++)
        {
            //cerr << "\tbase_x = " << base_x << " base_y = " << base_y << endl;
            //cerr << "\tdimensionWidth[" << i << "] = " << dimensionWidth[i] << " dimensionHeight[" << i << "] = " << dimensionHeight[i] << endl;

            UINT32 currentRow = (point.y() - base_y) / dimensionHeight[i];
            UINT32 currentCol = (point.x() - base_x) / dimensionWidth[i];

            //cerr << "\tcurrentRow = " << currentRow << " currentCol = " << currentCol << endl;

            qsResult += "[" + QString::number(currentRow * pnLayoutFloorplan[i][0] + currentCol) + "]";

            base_x += currentCol * dimensionWidth[i];
            base_y += currentRow * dimensionHeight[i];
        }
    }

    return qsResult;
}

/**
 * Returns the node name.
 *
 * @param point QPoint that holds the reference coordinates to compare to
 */
QString 
LayoutFloorplanRectangle::getNodeNameAt(QPoint &point)
{
    QString name = QString::null;
    if(rectType == Node) 
    {
        name = "Node " + draldb->getNode(dralId)->getName(); 
    }
    else
    {
        Q_ASSERT(false);
    }

    return name;
}

/**
 * Scans the list of sons in order to search the cell that contains any Item 
 * that matches the search criteria
 *
 * @param point QPoint that holds the reference coordinates to compare to
 */
ItemHandler 
LayoutFloorplanRectangle::getCellWithTagValue(TAG_ID tagId, UINT64 tagValue, CYCLE cycle, INT32 &startX, INT32 &startY)
{
    //cerr << "LayoutFloorplanRectangle::getCellWithTagValue: Looking at rectangle " << sNode << endl;
    ItemHandler hndResult;
    bool bFound = false;
    INT32 cellId = ((startX == -1) && (startY == -1)) ? 0 : getCellIdAt(startX, startY);
    CYCLE cyc;
    CYCLE tgWhen;
    ItemHandler handler;

    // Changes the parameter cycle to the internal clock domain.
    cyc.clock = draldb->getClockDomain(clockId);
    cyc.fromLCMCycles(cycle.toLCMCycles());

    while((static_cast<UINT32>(cellId) < totalCapacity) && !bFound)
    {
        if(draldb->getItemInSlot(trackIdBase + cellId, cyc, &handler))
        {
            UINT64 tgValue;
            TagValueType tgvType;
            TagValueBase tgBase;
            bool ok = handler.getTagById(tagId, &tgValue, &tgvType, &tgBase, &tgWhen, cyc);

            if(ok && (tgValue == tagValue))
            {
                bFound = true;
                hndResult = handler;

                // We obtain the coordinates for the next cell. Returning this value, we can start the
                // next search without finding the same element
                getCellIdCoordinates(cellId + 1, startX, startY);

                //cerr << "LayoutFloorplanRectangle::getCellWithTagValue: Found cellId = " << cellId << " value = " << tgValue << " new startX = " << startX << " startY = " << startY << endl;
                //cerr << "\t cellId obtained with the coordinates " << getCellIdAt(startX, startY) << endl;
            }
        }

        cellId++;
    }

    if(!hndResult.isValidItemHandler())
    {
        //cerr << "\tNo cellId obtained" << endl;
        startX = -1;
        startY = -1;
    }

    return hndResult;
}

/**
 * Scans the list of sons in order to search the cell that contains any Item 
 * that matches the search criteria
 *
 * @param point QPoint that holds the reference coordinates to compare to
 */
ItemHandler 
LayoutFloorplanRectangle::getCellWithTagValue(TAG_ID tagId, QString &tagValue, bool cs, bool em, CYCLE cycle, INT32 &startX, INT32 &startY)
{
    //cerr << "LayoutFloorplanRectangle::getCellWithTagValue: Looking at rectangle " << sNode << endl;
    ItemHandler hndResult;
    bool bFound = false;
    INT32 cellId = ((startX == -1) && (startY == -1)) ? 0 : getCellIdAt(startX, startY);
    CYCLE cyc;
    CYCLE tgWhen;
    ItemHandler handler;

    // Changes the parameter cycle to the internal clock domain.
    cyc.clock = draldb->getClockDomain(clockId);
    cyc.fromLCMCycles(cycle.toLCMCycles());

    while(static_cast<UINT32>(cellId) < totalCapacity && !bFound)
    {
        if(draldb->getItemInSlot(trackIdBase + cellId, cyc, &handler))
        {
            UINT64 tgValue;
            TagValueType tgvType;
            TagValueBase tgBase;
            bool ok = handler.getTagById(tagId, &tgValue, &tgvType, &tgBase, &tgWhen, cyc);

            if(ok)
            {
                QString fvalue = draldb->getString(tgValue);
                if(em)
                {
                    if(!cs)
                    {
                        fvalue = fvalue.upper();
                    }
                    bFound = (fvalue == tagValue);
                }
                else
                {
                    bFound = (fvalue.contains(tagValue, cs) > 0);
                }

                if(bFound)
                {
                    hndResult = handler;

                    // We obtain the coordinates for the next cell. Returning this value, we can start the
                    // next search without finding the same element
                    getCellIdCoordinates(cellId + 1, startX, startY);

                    //cerr << "LayoutFloorplanRectangle::getCellWithTagValue: Found cellId = " << cellId << " value = " << tgValue << " new startX = " << startX << " startY = " << startY << endl;
                    //cerr << "\t cellId obtained with the coordinates " << getCellIdAt(startX, startY) << endl;
                }
            }
        }

        cellId++;
    }

    if(!hndResult.isValidItemHandler())
    {
        //cerr << "\tNo cellId obtained" << endl;
        startX = -1;
        startY = -1;
    }

    return hndResult;
}


/**
 * Scans the list of sons in order to search the cell that contains any Item 
 * that matches the search criteria
 *
 * @param point QPoint that holds the reference coordinates to compare to
 */
ItemHandler 
LayoutFloorplanRectangle::getCellWithTagValue(TAG_ID tagId, QRegExp &tagValue, CYCLE cycle, INT32 &startX, INT32 &startY)
{
    //cerr << "LayoutFloorplanRectangle::getCellWithTagValue: Looking at rectangle " << sNode << endl;
    ItemHandler hndResult;
    bool bFound = false;
    INT32 cellId = ((startX == -1) && (startY == -1)) ? 0 : getCellIdAt(startX, startY);
    ItemHandler handler;
    CYCLE cyc;
    CYCLE tgWhen;

    // Changes the parameter cycle to the internal clock domain.
    cyc.clock = draldb->getClockDomain(clockId);
    cyc.fromLCMCycles(cycle.toLCMCycles());

    while(static_cast<UINT32>(cellId) < totalCapacity && !bFound)
    {
        if(draldb->getItemInSlot(trackIdBase + cellId, cyc, &handler))
        {
            UINT64 tgValue;
            TagValueType tgvType;
            TagValueBase tgBase;
            bool ok = handler.getTagById(tagId, &tgValue, &tgvType, &tgBase, &tgWhen, cyc);

            if(ok)
            {
                QString fvalue = draldb->getString(tgValue);
                bFound = (fvalue.contains(tagValue) > 0);
                if(bFound)
                {
                    hndResult = handler;

                    // We obtain the coordinates for the next cell. Returning this value, we can start the
                    // next search without finding the same element
                    getCellIdCoordinates(cellId + 1, startX, startY);
                    //cerr << "LayoutFloorplanRectangle::getCellWithTagValue: Found cellId = " << cellId << " value = " << tgValue << " new startX = " << startX << " startY = " << startY << endl;
                    //cerr << "\t cellId obtained with the coordinates " << getCellIdAt(startX, startY) << endl;
                }
            }
        }
        cellId++;
    }

    if(!hndResult.isValidItemHandler())
    {
        //cerr << "\tNo cellId obtained" << endl;
        startX = -1;
        startY = -1;
    }

    return hndResult;
}

void 
LayoutFloorplanRectangle::getCellIdCoordinates(INT32 cellId, INT32 &startX, INT32 &startY) const
{
    if(cellId >= static_cast<INT32>(totalCapacity))
    {
        startX = -1;
        startY = -1;
        return;
    }

    startX = x + getMargin();
    startY = y + getMargin();
    //cerr << "LayoutFloorplanRectangle::getCellIdCoordinates: startx = " << startX << " starty = " << startY << endl;

    UINT32 currentRow;
    UINT32 currentCol;
    UINT16 i;
    for(i = 0; i < dimensions - 1; i++)
    {
        coordinates[i] = cellId / (totalCapacity / capacities[i]);
        //cerr << "cellId = " << cellId << " numElems[" << i << "] = " << (totalCapacity/capacities[i]) << " coordinates[" << i << "] = " << coordinates[i] << endl;
        cellId -= coordinates[i] * accCapacities[i+1];


        currentRow = coordinates[i] / pnLayoutFloorplan[i][0];
        currentCol = coordinates[i] - currentRow * pnLayoutFloorplan[i][0];

        //cerr << "pnLayoutFloorplan[" << i << "][0] = " << pnLayoutFloorplan[i][0] << " pnLayoutFloorplan[" << i << "][1] = " << pnLayoutFloorplan[i][1] << " currentRow = " << currentRow << " currentCol = " << currentCol << endl;

        startX += currentCol * dimensionWidth[i];
        startY += currentRow * dimensionHeight[i];
    }
    coordinates[i] = cellId;

    //cerr << "cellId = " << cellId << " numElems[" << i << "] = 1 coordinates[" << i << "] = " << coordinates[i] << endl;

    currentRow = coordinates[i] / pnLayoutFloorplan[i][0];
    currentCol = coordinates[i] - currentRow * pnLayoutFloorplan[i][0];

    //cerr << "pnLayoutFloorplan[" << i << "][0] = " << pnLayoutFloorplan[i][0] << " pnLayoutFloorplan[" << i << "][1] = " << pnLayoutFloorplan[i][1] << " currentRow = " << currentRow << " currentCol = " << currentCol << " y = " << currentRow * dimensionHeight[i] << " x = " << currentCol * dimensionWidth[i] << " base_x = " << x << " base_y = " << y << endl;

    startX += currentCol * dimensionWidth[i];
    startY += currentRow * dimensionHeight[i];
}

void 
LayoutFloorplanRectangle::drawTitle(QPainter *painter)
{
    UINT32 x;
    UINT32 y;
    UINT32 borderWidth = getSlotBorder() ? LayoutFloorplanNode::CELL_BORDER : 0;
    UINT32 marginWidth = getMargin();
    UINT32 width;
    UINT32 height = 2 * TITLE_HEIGHT - borderWidth;

    painter->save();

    if(tpTitle == North)
    {
        painter->translate(getX(), getY());
        width = getWidth();
    }
    else if(tpTitle == South)
    {
        painter->translate(getX(), getY() + getHeight() - TITLE_HEIGHT);
        width = getWidth();
    }
    else if(tpTitle == East)
    {
        painter->translate(getX() + getWidth(), getY());
        painter->rotate(+90.0);
        width = getHeight();
    }
    else if(tpTitle == West)
    {
        painter->translate(getX(), getY() + getHeight());
        painter->rotate(-90.0);
        width = getHeight();
    }
    else 
    {
        painter->restore();
        return;
    }


    //Qt::RasterOp op = painter->rasterOp();

    painter->setRasterOp(Qt::XorROP);
    painter->setPen(QPen(QColor("grey"), 1));
    painter->setBrush(Qt::NoBrush);

    QFont fontTitle("Helvetica");
    fontTitle.setPixelSize(CELL_HEIGHT);
    fontTitle.setWeight(QFont::Bold);
    painter->setFont(fontTitle);

    painter->drawText(0, 0, width, height, Qt::AlignLeft, getName());

    //painter->setRasterOp(op);
    painter->restore();
}

void 
LayoutFloorplanRectangle::draw(QPainter * painter, HighLightMgrFloorplan * hlmgr, CYCLE cycle, AScrollView * asv)
{

    // We cannot paint if we don't have any place to store the color
    Q_ASSERT((nTagIdColor > 0) && (tagIdColor[0] > -1));

    if(bBorder || bBGColorDefined)
    {
        painter->setBrush((bBGColorDefined ? QBrush(bgColor) : Qt::NoBrush));
        painter->setPen((bBorder ? QPen(Qt::black) : Qt::NoPen));
        DrawingTools::drawRectangle(getX(), getY(), painter, getWidth(), getHeight());
    }

    // FIXME
    // By now we draw all the cells inside a rectangle. This code
    // should take into account the coordinates of the viewport
    UINT32 borderWidth = getSlotBorder() ? LayoutFloorplanNode::CELL_BORDER : 0;
    UINT32 marginWidth = getMargin();

    // DEBUG
    //    cerr << "Cycle " << cycle << ": Painting cells of rectangle " << getName() << " at x = " << getX() << " y = " << getY() << " width = " << getWidth() << " cellWidth = " << getCellWidth() << " height = " << getHeight() << " getCellHeight = " << getCellHeight() << " borderWidth = " << borderWidth << " marginWidth " << marginWidth << endl;

    // TODO 
    // Do it global observable by all the elements
    // Delta applieds to the coordinathe thanks to the title
    UINT32 nTitleXStart = 0;
    UINT32 nTitleXEnd   = 0;
    UINT32 nTitleYStart = 0;
    UINT32 nTitleYEnd   = 0;
    if(!bTitleOnTop && tpTitle == North)      nTitleYStart = TITLE_HEIGHT;
    else if(!bTitleOnTop && tpTitle == South) nTitleYEnd = TITLE_HEIGHT;
    else if(!bTitleOnTop && tpTitle == East)  nTitleXEnd = TITLE_HEIGHT;
    else if(!bTitleOnTop && tpTitle == West)  nTitleXStart = TITLE_HEIGHT;

    CYCLE cyc;
    ItemHandler handler;

    cyc.clock = draldb->getClockDomain(clockId);
    cyc.fromLCMCycles(cycle.toLCMCycles());

    // If we have computed a filter, we choose the filtered color by default.
    UINT32 col = 0;
    for(UINT32 x = getX() + marginWidth + nTitleXStart; x < (getX() + getWidth() - nTitleXEnd); x += (getCellWidth() + 2 * borderWidth + marginWidth))
    {
        UINT32 row = 0;
        for(UINT32 y = getY() + marginWidth + nTitleYStart; y < (getY() + getHeight() - nTitleYEnd); y += (getCellHeight() + 2 * borderWidth + marginWidth))
        {
            TRACK_ID trackId = getTrackIdForCell(x, y);
            Q_ASSERT(trackId != TRACK_ID_INVALID);

            //if(getName() == "Node MC{0}")
            //    cerr << "\tSlot[" << y << "][" << x << "] trackId = " << trackId << endl;

            UINT64 cslIndex;
            EventShape shape = EVENT_SHAPE_RECTANGLE;
            char letter = ' ';
            QColor letterColor;

            if(draldb->getTrackTagValue(trackId, tagIdColor[currentTagIdColor], cyc, &cslIndex))
            {
                //if(getName() == "Node MC{0}")
                //    cerr << "LayoutFloorplanRectangle::draw: trackId = " << trackId << " row = " << row << " col = " << col << " cslIndex = " << cslIndex << endl;

                ColorShapeItem csh = cslCache.getColorShape(cyc.cycle, y, x, (INT32)cslIndex);

                // Set the fill color
                if(!csh.getTransparencySet() && !csh.getFillColorSet())
                {
                    painter->setBrush(QBrush(QColor(255,0,0),Qt::DiagCrossPattern));
                }
                else
                {
                    if(!csh.getTransparency())
                    {
                        painter->setBrush(QBrush(AColorPalette::getColor(csh.getColorIdx())));
                    }
                    else
                    {
                        painter->setBrush(Qt::NoBrush);
                    }
                }

                // Set the border color
                if(getSlotBorder())
                {
                    painter->setPen(QPen(AColorPalette::getColor(csh.getBColorIdx()), LayoutFloorplanNode::CELL_BORDER));
                }
                else
                {
                    painter->setPen(Qt::NoPen);
                }

                // Set the shape
                shape = csh.getShape();

                // letter
                letter = csh.getLetter();
                letterColor = AColorPalette::getColor(csh.getLetterColorIdx());
                //cerr << "LayoutFloorplanRectangle: getfrom Cache: cycle=" << cycle << " trackId=" << trackId << " y=" << y << " x=" << x << " cache_idx= " <<  cslIndex <<  " letterColoridx " << csh.getLetterColorIdx() << " letterColor " << letterColor.name() <<endl;
            }
            else
            {
                painter->setBrush(Qt::NoBrush);
                if(getSlotBorder())
                {
                    painter->setPen(QPen(Qt::black, LayoutFloorplanNode::CELL_BORDER));
                }
                else
                {
                    painter->setPen(Qt::NoPen);
                }
            }

            // TODO
            // Recode the Highlight routine to the resource mode...
            QPen border;

            draldb->getItemInSlot(trackId, cyc, &handler);
            if((hlmgr->getNumActiveCriteria() > 0) && handler.isValidItemHandler() && hlmgr->getHighlightColor(&handler, cyc, border))
            {
                painter->setPen(border);
            }

            if(letter == ' ')
            {
                DrawingTools::drawShape(x,  y, painter, shape, getCellWidth() + 2 * borderWidth, getCellHeight() + 2 * borderWidth);
            }
            else
            {
                DrawingTools::drawShape(x,  y, painter, shape, QChar(letter), letterColor, getCellWidth() + 2 * borderWidth, getCellHeight() + 2 * borderWidth, asv);
            }
            row++;
        }
        col++;
    }

    drawTitle(painter);
}

/*
 * Gets a set of tag handlers of all the slots of the layout.
 *
 * @return void.
 */
void
LayoutFloorplanRectangle::getFindData(LayoutFloorplanFindDataSet * entries, CYCLE startCycle, const LayoutFloorplanFindOptions * env) const
{
    CYCLE cyc;
    ItemHandler handler;
    INT32 row;
    INT32 col;
    LayoutFloorplanFindDataEntry * entry;
    TRACK_ID trackId;

    cyc.clock = draldb->getClockDomain(clockId);
    cyc.fromLCMCycles(startCycle.toLCMCycles());

    for(UINT32 cellId = 0; cellId < totalCapacity; cellId++)
    {
        trackId = getTrackIdForCell(cellId);

        getCellIdCoordinates(cellId, col, row);
        entry = new LayoutFloorplanFindDataEntry(row, col, trackId, startCycle, draldb);

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

void
LayoutFloorplanRectangle::addDefaultRules(ExpressionList &rules, INT32 level)
{
    RuleEngine::addDefaultRules(rules, this, level);
}

void 
LayoutFloorplanRectangle::addOverrideRules(ExpressionList &rules, INT32 level)
{
    RuleEngine::addOverrideRules(rules, this, level);
}

INT64
LayoutFloorplanRectangle::getObjSize() const
{
    INT64 result = sizeof(*this);

    // Size of "capacities"
    result += sizeof(UINT32) * dimensions;

    // Size of "accCapacities"
    result += sizeof(UINT32) * dimensions;

    // Size of "dimensionHeight"
    result += sizeof(UINT32) * dimensions;

    // Size of "dimensionWidth"
    result += sizeof(UINT32) * dimensions;

    // Size of "pnLayoutFloorplan"
    result += sizeof(UINT32) * dimensions * 2;

    // Size of "coordinates"
    result += sizeof(UINT32) * dimensions;

    // Size of "qlistRules"
    ExpressionIterator it_exp(qlistRules);
    Expression * exp;
    while((exp = it_exp.current()) != NULL)
    {
        ++it_exp;
        result += exp->getObjSize();
    }

    // Size of "cslCache"
    result += sizeof(cslCache);

    return result;
}

QString
LayoutFloorplanRectangle::getUsageDescription() const
{
    QString result = "\nLayoutFloorplanRectangle(" + getName() + ") size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n";
    result += "Detailed memory usage for each component:\n";
    result += "cslCache: " + QString::number(sizeof(cslCache)) + "\n";
    result += "Rules:\n";
    ExpressionIterator it_exp(qlistRules);
    Expression * exp;

    while((exp = it_exp.current()) != NULL)
    {
        ++it_exp;
        result += exp->getUsageDescription();
    }

    return result;
}

QString 
LayoutFloorplanRectangle::getName(void) const
{ 
    QString name = QString::null;
    if(rectType == Node) 
    {
        name = "Node " + draldb->getNode(dralId)->getName(); 
    }
    else if(rectType == Edge) 
    {
        DralGraphEdge *edge = draldb->getEdge(dralId);
        name = "Edge " + edge->getName() + "(" + edge->getSourceNodeName() + " -> " + edge->getDestinationNodeName() + ")";
    }
    else
    {
        Q_ASSERT(false);
    }

    return name;
}

void 
LayoutFloorplanRectangle::setTFilter(INT32 tagIdFilter, UINT32 nTaps)
{
    if(filter == NULL)
    {
        // This is the first filter activated, so we store the index to the tagIdColor
        // used to store the result of the filtering
        nTagIdFilter = nTagIdColor;
        setTagIdColor(tagIdFilter);

        filter = new FILTER_HANDLER_CLASS();
    }
    filter->ActivateTFilter(2 * nTaps + 1, FILTER_TRIANGLE, 1.1);
}


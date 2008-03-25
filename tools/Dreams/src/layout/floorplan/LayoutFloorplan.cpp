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
 * @file LayoutFloorplan.cpp
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "DreamsDB.h"
#include "layout/floorplan/LayoutFloorplan.h"

// Qt includes.
#include <qprogressdialog.h>

/*
 * Creates an empty LayoutFloorplan associated to the given dral
 * database.
 *
 * @return new object.
 */
LayoutFloorplan::LayoutFloorplan(DralDB * _draldb)
{
    draldb = _draldb;
    pltRoot = NULL;
}

/*
 * Destroys the root level.
 *
 * @return destroy object.
 */
LayoutFloorplan::~LayoutFloorplan()
{
    delete pltRoot;
}

/*
 * Method to compute the colors and shape of the events of a given
 * chunk of cycles. Just forwards the call to all the resource nodes.
 *
 * @return void.
 */
void
LayoutFloorplan::computeCSL(void)
{
    if(pltRoot != NULL)
    {
        QProgressDialog * progress; ///< Progress dialog to show the evolution of the color computing.
        QLabel * label;             ///< Label of the progress dialog.

        progress = new QProgressDialog("Layout View is computing colors...", 0, getNumCells(), NULL, "commitprogress", TRUE);

        label = new QLabel(QString("\nLayout View is computing colors...               ") +
                           QString("\nCells                     : 0"), progress);
        Q_ASSERT(label != NULL);
        label->setMargin(5);
        label->setAlignment(Qt::AlignLeft);
        progress->setLabel(label);
        UINT32 currentCell;

        currentCell = 0;

        // Computes the CSLs.
        pltRoot->computeCSL(progress, label, getNumCells(), &currentCell);

        delete progress;
    }
}

/**
 * Method to compute the colors and shape of the events of a given
 * chunk of cycles. This method is only appliable to those rectangles
 * that have the filters activated.
 *
 * @return void.
 */
void
LayoutFloorplan::computeCSLFilters(void)
{
    if(pltRoot != NULL)
    {
        QProgressDialog * progress; ///< Progress dialog to show the evolution of the color computing.
        QLabel * label;             ///< Label of the progress dialog.

        progress = new QProgressDialog("Layout View is computing filtered colors...", "&Stop", getNumCells(), NULL, "commitprogress", TRUE);

        label = new QLabel(QString("\nLayout View is computing filtered colors...               ") +
                           QString("\nCells                     : 0"), progress);
        Q_ASSERT(label != NULL);
        label->setMargin(5);
        label->setAlignment(Qt::AlignLeft);
        progress->setLabel(label);
        UINT32 currentCell;

        currentCell = 0;

        // Computes the CSLs.
        pltRoot->computeCSLFilters(progress, label, getNumCells(), &currentCell);

        delete progress;
    }
}

/**
 * Search method that looks for a ItemHandler located in any position located
 * within the region compressed by the right-bottom of the point (startX,
 * startY) that matches the search criteria specifyed by the params tagId,
 * tagValue
 *
 * @param tagId Tag to look for.  
 * @param tagValue Value to look for.  
 * @param cycle Current searching cycle 
 *
 * @out startX X coordinates of the point where to start the search. Is also 
 *             updated with the X coordinate of the cell where to start the 
 *             next search. If the returned value is -1, means that the last
 *             in the given cycle is the one that matches the criteria
 *
 * @out startY Y coordinates of the point where to start the search. Is also 
 *             updated with the X coordinate of the cell where to start the 
 *             next search. If the returned value is -1, means that the last
 *             in the given cycle is the one that matches the criteria
 *
 * @return Item handler located at the returned cell in the given cycle
 */
ItemHandler
LayoutFloorplan::getCellWithTagValue(TAG_ID tagId, UINT64 value, CYCLE cycle, INT32 &startX, INT32 &startY)
{
    return pltRoot->getCellWithTagValue(tagId, value, cycle, startX, startY);
}

/**
 * Search method that looks for a ItemHandler located in any position located
 * within the region compressed by the right-bottom of the point (startX,
 * startY) that matches the search criteria specifyed by the params tagId,
 * tagValue
 *
 * @param tagId Tag to look for.  
 * @param tagValue Value to look for.  
 * @param cs Boolean that indicates if the match must be case sensitive
 * @param em Boolean that indicates if the match must be exact
 * @param cycle Current searching cycle 
 *
 * @out startX X coordinates of the point where to start the search. Is also 
 *             updated with the X coordinate of the cell where to start the 
 *             next search. If the returned value is -1, means that the last
 *             in the given cycle is the one that matches the criteria
 *
 * @out startY Y coordinates of the point where to start the search. Is also 
 *             updated with the X coordinate of the cell where to start the 
 *             next search. If the returned value is -1, means that the last
 *             in the given cycle is the one that matches the criteria
 *
 * @return Item handler located at the returned cell in the given cycle
 */
ItemHandler
LayoutFloorplan::getCellWithTagValue(TAG_ID tagId, QString &value, bool cs, bool em, CYCLE cycle, INT32 &startX, INT32 &startY)
{
    return pltRoot->getCellWithTagValue(tagId, value, cs, em, cycle, startX, startY);
}

/**
 * Search method that looks for a ItemHandler located in any position located
 * within the region compressed by the right-bottom of the point (startX,
 * startY) that matches the search criteria specifyed by the params tagId,
 * tagValue
 *
 * @param tagId Tag to look for.  
 * @param tagValue Value to look for.  
 * @param cycle Current searching cycle 
 *
 * @out startX X coordinates of the point where to start the search. Is also 
 *             updated with the X coordinate of the cell where to start the 
 *             next search. If the returned value is -1, means that the last
 *             in the given cycle is the one that matches the criteria
 *
 * @out startY Y coordinates of the point where to start the search. Is also 
 *             updated with the X coordinate of the cell where to start the 
 *             next search. If the returned value is -1, means that the last
 *             in the given cycle is the one that matches the criteria
 *
 * @return Item handler located at the returned cell in the given cycle
 */
ItemHandler
LayoutFloorplan::getCellWithTagValue(TAG_ID tagId, QRegExp &value, CYCLE cycle, INT32 &startX, INT32 &startY)
{
    return pltRoot->getCellWithTagValue(tagId, value, cycle, startX, startY);
}

/**
 * Looks for the first appeareance of any item in the LayoutFloorplan
 *
 * @param tagId
 * @param tagValue
 * @out cycle
 * @out startX
 * @out startY
 *
 * @return if any element has been found
 */
bool 
LayoutFloorplan::getFirstCellAppearenceWithTagValue(TAG_ID tagId, UINT64 tagValue, CYCLE & cycle, INT32 &startX, INT32 &startY)
{
    LayoutFloorplanFindOptions env;       ///< Layoutfloorplan find options. By now, default options.
    LayoutFloorplanFindDataSet entries;   ///< Set of handlers in the layout.
    LayoutFloorplanFindDataEntry * entry; ///< Working entry.
    ItemHandler hnd;                      ///< Handler to access to the current item.
    TagValueType tgType;                  ///< The type of the tag.
    TagValueBase tgBase;                  ///< The base.
    CYCLE tgWhen;                         ///< When the tag was set.
    UINT64 tgValue;                       ///< Value of the item tag.
    bool ok;                              ///< Used to get some boolean results.

    // Gets all the handlers.
    getFindData(&entries, draldb->getFirstCycle(), &env);

    // Tries to find until no more entries are left or until something is found.
    while(!entries.empty())
    {
        // Gets the first entry, which is the actual candidate as is the row
        // with a non-visited item that has the lowest cycle or in tie case
        // the smallest non-visited position.
        entry = * entries.begin();

        // Gets the item inside.
        ok = draldb->lookForItemId(&hnd, entry->getItemId());
        Q_ASSERT(ok);

        // Gets the value.
        ok = hnd.getTagById(tagId, &tgValue, &tgType, &tgBase, &tgWhen, draldb->getLastCycle());

        // Checks if match.
        if(ok && (tgValue == tagValue))
        {
            // Gets the position.
            startY = entry->getRow();
            startX = entry->getCol();
            cycle.clock = draldb->getClockDomain(entry->getClockId());
            cycle.fromLCMCycles(entry->getCycle());

            return true;
        }

        // Removes the entry from the set.
        entries.erase(entries.begin());

        // Moves the handler to the next item.
        entry->skipToNextItem();

        // If still valid is reinserted to the set.
        if(entry->isValidDataEntry())
        {
            entries.insert(entry);
        }
    }

    return false;
}

/*
 * Gets a set of tag handlers of all the slots of the layout.
 *
 * @return void.
 */
void
LayoutFloorplan::getFindData(LayoutFloorplanFindDataSet * entries, CYCLE startCycle, const LayoutFloorplanFindOptions * env) const
{
    pltRoot->getFindData(entries, startCycle, env);
}

/*
 * Returns the size of the layout.
 *
 * @return object size..
 */
INT64
LayoutFloorplan::getObjSize() const
{
    INT64 result = sizeof(LayoutFloorplan);

    if(pltRoot != NULL)
    {
        result += pltRoot->getObjSize();
    }
    return result;
}

/*
 * Returns a string with a description of the layout memory
 * usage.
 *
 * @return the usage description.
 */
QString
LayoutFloorplan::getUsageDescription() const
{
    QString result;
    if(pltRoot != NULL)
    {
        result = "\n --- LayoutFloorplan memory report --- \n\n";

        result += "LayoutFloorplan size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n\n";
        result += "Object LayoutFloorplan contains:\n";
        result += " 1) FloorplanNodes:\t"+QString::number(static_cast<long>(pltRoot->getObjSize())) + " bytes\n";

        result += "\n\nDetailed memory usage for each component:\n";
        result += pltRoot->getUsageDescription();
    }
    return (result);
}

/*
 * Dumps the content of the layout.
 *
 * @return void.
 */
void
LayoutFloorplan::dump(void) const
{
    if(pltRoot)
    {
        //cerr << "Dumping current layout" << endl;
        pltRoot->dump();
    }
}

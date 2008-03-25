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
 * @file LayoutWatch.cpp
 */

// General includes.
#include <iostream>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/PrimeList.h"
#include "WatchWindow.h"
#include "layout/watch/LayoutWatch.h"

// Qt includes.
#include <qprogressdialog.h>

LayoutWatch::LayoutWatch()
{ 
    // We create the string dictionary as case sensitive
    pqdicWindows = new QDict<WatchWindow>(PrimeList::nearPrime(100), true);
}

LayoutWatch::~LayoutWatch()
{ 
    delete pqdicWindows;
}

void 
LayoutWatch::addWatchWindow(WatchWindow *window)
{
    WatchWindow *sameCaptionWindow = pqdicWindows->find(window->getCaption());

    // Another watch window has been defined with the same name
    if(sameCaptionWindow != NULL)
    {
        window->setCaption(window->getCaption() + QString::number(sameCaptionWindow->incNumSameCaption()));
    }

    pqdicWindows->insert(window->getCaption(), window);
}

WatchWindow *
LayoutWatch::getWatchWindow(QString caption)
{
    Q_ASSERT(pqdicWindows);
    return pqdicWindows->find(caption);
}

/**
 * Computes the number of slots tracked by all the defined watch windows
 */
UINT32
LayoutWatch::getNumCells(void)
{
    UINT32 numCells = 0;
    QDictIterator<WatchWindow> iter(*pqdicWindows);
    WatchWindow *currentWindow = NULL;
    while((currentWindow = iter.current()) != NULL)
    {
        ++iter;
        numCells += currentWindow->getCapacity();
    }
    return numCells;
}

QDictIterator<WatchWindow>
LayoutWatch::getWatchWindowIterator(void)
{
    Q_ASSERT(pqdicWindows);
    QDictIterator<WatchWindow> iter(*pqdicWindows);
    while(iter.current() != NULL)
    {
        //cerr << "\t" << iter.current()->getCaption() << endl;
        ++iter;
    }

    iter.toFirst();
    return iter;
}

bool 
LayoutWatch::getOpened(void)
{
    return(pqdicWindows->size() > 0);
}

/**
 * Method to compute the colors and shape of the events of a given
 * chunk of cycles. Just forwards the call to all the resource nodes.
 *
 * @return void.
 */
void
LayoutWatch::computeCSL(void)
{
    if(pqdicWindows->size())
    {
        QProgressDialog * progress; ///< Progress dialog to show the evolution of the color computing.
        QLabel * label;             ///< Label of the progress dialog.

        progress = new QProgressDialog("WatchWindow is computing colors...", 0, getNumCells(), NULL, "commitprogress", TRUE);

        label = new QLabel(QString("\nWatchWindow is computing Colors...               ") +
                           QString("\nCells                     : 0"), progress);
        Q_ASSERT(label != NULL);
        label->setMargin(5);
        label->setAlignment(Qt::AlignLeft);
        progress->setLabel(label);
        UINT32 currentCell;

        currentCell = 0;

        // Computes the CSLs.
        QDictIterator<WatchWindow> iter(*pqdicWindows);
        WatchWindow *currentWindow = NULL;
        while((currentWindow = iter.current()) != NULL)
        {
            ++iter;
            currentWindow->computeCSL(progress, label, currentCell);
        }

        delete progress;
    }
}



// ---- AMemObj Interface methods
INT64
LayoutWatch::getObjSize() const
{
    INT64 result = sizeof(LayoutWatch);

    return result;
}

QString
LayoutWatch::getUsageDescription() const
{
    QString result = "\n --- LayoutWatch memory report --- \n\n";
    result += "LayoutWatch size: " + QString::number(getObjSize()) + " bytes\n";
    result += "Object LayoutWatch contains:\n";

    return result;
}

QString
LayoutWatch::getStats() const
{
    QString result = "";

    return (result);
}

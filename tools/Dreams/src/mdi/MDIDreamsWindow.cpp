/*
 * Copyright (C) 2005-2006 Intel Corporation
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
#include "dDB/DralDB.h"
#include "dockwin/tag/TagDockWin.h"
#include "MDIDreamsWindow.h"
#include "PreferenceMgr.h"
#include "DreamsDB.h"

/*
 * Sets the initial values and forwards the creation parameters to
 * its super class.
 *
 * @return the new object.
 */
MDIDreamsWindow::MDIDreamsWindow(DreamsDB * _dreamsdb, QWidget * parent, const char * name, int wflags)
    : MDIWindow(parent, name, wflags)
{
    tagWinEnabled = false;
    tagWin = NULL;
    myMDINumber = -1;
    displacement = 0;
    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();

    // Sets the currentCycle as the first cycle of the fastest clock.
    currentCycle = draldb->getFirstEffectiveCycle((* draldb->getClockDomainFreqIterator())->getId());
}

void
MDIDreamsWindow::createTagWindow()
{
    // create the docked window
    tagWin = new TagDockWin(this, draldb);
    Q_ASSERT(tagWin!=NULL);
    bool val = PreferenceMgr::getInstance()->getShowTagWindow();  
    if ( val )
    {
        tagWin->show();
        setEnabledTagWindow(true);
    }
    else
    {
        tagWin->hide();
        setEnabledTagWindow(false);
    }
}

TagDockWin *
MDIDreamsWindow::getTagWindow()
{
    return tagWin;
}

void 
MDIDreamsWindow::setEnabledTagWindow(bool v)
{
    tagWinEnabled = v;
}

bool 
MDIDreamsWindow::getEnabledTagWindow()
{
    return tagWinEnabled;
}

void
MDIDreamsWindow::refreshFavoriteTagList()
{
    if(tagWin)
    {
        tagWin->updateTagList();
    }
}
    
void 
MDIDreamsWindow::setMDINumber(int v)
{
    myMDINumber = v;
    if(tagWin)
    {
        if(v > 1)
        {
            tagWin->setCaption("Tag Window (" + QString::number(v) + ")");
        }
    }
}

/*
 * Returns the biggest possible cycle of the accumulated displacement.
 *
 * @return the accumulated cycles..
 */
CYCLE
MDIDreamsWindow::getAccCycleChanged(const ClockDomainEntry * clock, bool divisions)
{
    CYCLE ret(clock);

    ret.fromPs(abs(displacement));

    if(!divisions)
    {
        ret.division = 0;
    }

    if(displacement < 0)
    {
        displacement += ret.toPs();
        ret.cycle = -ret.cycle;
        ret.division = -ret.division;
    }
    else
    {
        displacement -= ret.toPs();
    }
    return ret;
}

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
  * @file LayoutWatch.h
  */

#ifndef _LAYOUTWATCH_H_
#define _LAYOUTWATCH_H_

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/aux/AMemObj.h"
#include "dDB/aux/StatObj.h"

// Qt includes.
#include <qdict.h>

// Class forwarding.
class WatchWindow;

/*
 * @brief
 * This class holds DRAL Graph and configuration information.
 *
 * @description
 * 
 *
 * @author Santi Galan
 * @date started at 2004-10-26
 * @version 0.1
 */
class LayoutWatch : public AMemObj, public StatObj
{
  private:
    QDict<WatchWindow> *pqdicWindows;

  public:
    INT64  getObjSize() const;
    QString getUsageDescription() const;
    QString getStats() const;

    LayoutWatch();

    virtual ~LayoutWatch();

    void addWatchWindow(WatchWindow *window);
    WatchWindow *getWatchWindow(QString caption);

    UINT32 getNumCells(void);
    QDictIterator<WatchWindow> getWatchWindowIterator(void);
    bool getOpened(void);

    // CSL compute method
    void computeCSL(void);
};

#endif // _LAYOUTWATCH_H_.

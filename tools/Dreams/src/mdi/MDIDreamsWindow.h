// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
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
  * @file  MDIDreamsWindow.h
  * @brief Virtual class that define the common interface between the main application
  * control and the differents views of the current event file.
  */

#ifndef _MDIDREAMSWINDOW_H_
#define _MDIDREAMSWINDOW_H_

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/ClockDomainMgr.h"

// Asim includes.
#include "asim/MDIWindow.h"

// Class forwarding.
class TagDockWin;
class DralDB;
class DreamsDB;

/**
 * This is a specialization of MDIWindow to Dreams project.
 * @brief Virtual class that define the common interface between the main application
 *        control and the differents mdis displayed.
 * @version 0.2
 * @date started at 2004-11-10
 * @author Santi Galan
 */
class MDIDreamsWindow : public MDIWindow
{
  Q_OBJECT   
  public:

    MDIDreamsWindow(DreamsDB * _dreamsdb, QWidget * parent, const char * name, int wflags);

    // Function to know if the mdi is locked or not. Some of the current views
    // have the lock state at the views.
    virtual bool getMDILocked(void) = 0;

    // Function that indicates if the MDI is based on a timer. Used for synchronizing the Floorplan/WatchWindow
    // with the rest of view when the clock is active
    virtual bool getIsClocked(void) = 0;

    virtual void setMDINumber(int);

    // Set of methods that attend the different movement signals emitted by
    // the other views
    virtual void attendCycleChanged(INT64 ps) { }
    virtual void attendResourceChanged(double resource) { }
    virtual void attendItemChanged(double item) { }
    virtual void attendThreeDMoved(double x, double y) { }
    virtual void attendFourDMoved(double x, double y, double z) { }
    virtual void showCycle(CYCLE cycle) { }

    // TagWindow related
    virtual void createTagWindow();
    virtual TagDockWin * getTagWindow(void);
    virtual void setEnabledTagWindow(bool);
    virtual bool getEnabledTagWindow();
    virtual void refreshFavoriteTagList(void);

    // Cycle related.
    inline void setCycle(CYCLE cycle);
    inline void setCycleWithConversion(CYCLE cycle);
    inline CYCLE getCycle() const;
    CYCLE getAccCycleChanged(const ClockDomainEntry * clock, bool divisions);

    inline DreamsDB * getDreamsDB() const;

    inline bool getVHIGH();
    inline bool getVNHIGH();
    inline bool getVBHIGH();

    inline void setVHIGH(bool);
    inline void setVNHIGH(bool);
    inline void setVBHIGH(bool);

  protected:
    TagDockWin * tagWin;
    bool tagWinEnabled;
    int myMDINumber;
    DreamsDB * dreamsdb;    ///< Pointer to the actual trace.
    DralDB * draldb;        ///< Pointer to the dral database.
    CYCLE currentCycle;     ///< Current displayed cycle
    bool VHIGH;
    bool VNHIGH;
    bool VBHIGH;
    INT64 displacement; ///< Accumulated displacement due to cycle changes.
} ;
    
/*
 * Sets the cycle to the current cycle.
 *
 * @return void.
 */
void
MDIDreamsWindow::setCycle(CYCLE cycle)
{
    currentCycle = cycle;
}

/*
 * Sets the cycle to the current cycle but maintaining the original
 * clock domain.
 *
 * @return void.
 */
void
MDIDreamsWindow::setCycleWithConversion(CYCLE cycle)
{
    currentCycle.fromLCMCycles(cycle.toLCMCycles());
}

/*
 * Returns the actual cycle.
 *
 * @return the cycle.
 */
CYCLE
MDIDreamsWindow::getCycle() const
{
    return currentCycle;
}

bool
MDIDreamsWindow::getVHIGH()
{
    return VHIGH;
}

bool 
MDIDreamsWindow::getVNHIGH()
{
    return VNHIGH;
}

bool 
MDIDreamsWindow::getVBHIGH()
{
    return VBHIGH;
}

void 
MDIDreamsWindow::setVHIGH(bool value)
{
    VHIGH=value;
    VNHIGH=!value;
    VBHIGH=!value;
}

void 
MDIDreamsWindow::setVNHIGH(bool value)
{
    VHIGH=!value;
    VNHIGH=value;
    VBHIGH=!value;
}

void 
MDIDreamsWindow::setVBHIGH(bool value)
{
    VHIGH=!value;
    VNHIGH=!value;
    VBHIGH=value;
}

DreamsDB *
MDIDreamsWindow::getDreamsDB() const
{
    return dreamsdb;
}

#endif /* _MDIDREAMSWINDOW_H_ */

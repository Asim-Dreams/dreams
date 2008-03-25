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
  * @file  MDIFloorplanWindow.h
  * @brief 
  */


#ifndef _MDIFLOORPLANWINDOW_H
#define _MDIFLOORPLANWINDOW_H

#include <qglobal.h>
#include "DreamsDefs.h"
#include "MDIDreamsWindow.h"

// Class forwarding.
class QSlider;
class QTimer;
class QVBox;
class QMainWindow;

class MDIFloorplanWindow: public MDIDreamsWindow
{

    Q_OBJECT

  public:
    MDIFloorplanWindow(DreamsDB * dreamsdb, QMainWindow * m, QWidget * parent, const char * name, int wflags);
    ~MDIFloorplanWindow();
    bool init();

    bool getMDILocked(void) { return asv->getMDILocked(); }
    bool getIsClocked(void) { return true; }

    void showCycle(CYCLE cycle);
    UINT32 getFPS(void) { return nFPS; }
    UINT32 getCPF(void) { return nCPF; }

    void attendCycleChanged(INT64 ps);
    void attendThreeDMoved(double x, double y);

  signals:
    void mdiCloseEvent(MDIWindow*);
    void firstCycle(MDIWindow *);
    void lastCycle(MDIWindow *);

    // Movement signals
    void cycleChanged(Q_INT64 ps);

  public slots:
    void setFPS(const QString &fps);
    void setCPF(const QString &fps);
    void nextFrame();
    void setFrame(int cycle);
    void sliderPressed();
    void sliderReleased();
    void setClockDomain(int clockId);

    void rewind(void);
    void play(void);
    void stepForward(void);
    void stepBackward(void);
    void stop(void);
    void forward(void);

  protected:
    void closeEvent( QCloseEvent * );
    inline CYCLE sliderValueToCycle(UINT32 value) const;
    inline UINT32 cycleToSliderValue(CYCLE cycle) const;

  protected:
    QMainWindow* mainWin;
    QWidget* myParent;

    QVBox     * pqvbLayout;
    QHBox     * pqhbLayout;
    QSlider   * psldCycle;
    QTimer    * pTimer;
    QComboBox * qcbClockDomain;

    UINT32 nFPS;
    UINT32 nCPF;

    INT32 lastValue; ///< Last value of the slider. Used to compute displacements of cycles.

    bool sliderDrag; ///< True if the slider is being dragged.

    // Direction of the time
    // If 1, forward, if -1 backward
    int nextCycle;

    // Indicates if we are playng at normat rate (play) or fast (rewind/forward)
    UINT32 fastStep;
};

CYCLE
MDIFloorplanWindow::sliderValueToCycle(UINT32 value) const
{
    CYCLE ret;

    ret.clock = currentCycle.clock;
    if(currentCycle.clock->getDivisions() > 1)
    {
        ret.cycle = value / currentCycle.clock->getDivisions();
        ret.division = value % currentCycle.clock->getDivisions();
    }
    else
    {
        ret.cycle = value;
        ret.division = 0;
    }
    return ret;
}

UINT32
MDIFloorplanWindow::cycleToSliderValue(CYCLE cycle) const
{
    UINT32 ret;

    if(cycle.clock->getDivisions() > 1)
    {
        ret = (cycle.cycle * cycle.clock->getDivisions()) + cycle.division;
    }
    else
    {
        ret = cycle.cycle;
    }
    return ret;
}

#endif

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
  * @file  MDIWatchWindow.h
  * @brief 
  */


#ifndef _MDIWATCHWINDOW_H_
#define _MDIWATCHWINDOW_H_

// Qt includes.
#include <qtable.h>
#include <qglobal.h>

// Dreams includes.
#include "MDIDreamsWindow.h"
#include "dDB/taghandler/TagFlipHandler.h"

// Class forwarding.
class QMainWindow;
class QTimer;
class QVBox;
class QSlider;
class QPopupMenu;
class WatchWindow;

class MDIWatchWindow : public MDIDreamsWindow
{
    Q_OBJECT
  private:
    /**
     * Class that inherits from a QTable in order to redefine the 
     * column resize method. Redefining the method allows us to 
     * synch the table mechanism with the MDI window.
     */
    class WatchWindowTable : public QTable
    {
      private:
        MDIWatchWindow *parentWindow;

      public:
        WatchWindowTable(MDIWatchWindow *parentWindow, QWidget *parent);

      public slots:
        void columnWidthChanged(int col);
    };

  public:
    MDIWatchWindow(DreamsDB * dreamsdb, QMainWindow * m, QWidget * parent, const char * name, int wflags);
    ~MDIWatchWindow();
    bool init();

    void setTableSize(void);

    void setWatchWindow(WatchWindow *window);

    UINT32 getFPS(void) { return nFPS; }
    UINT32 getCPF(void) { return nCPF; }

    bool getMDILocked(void) { return mdiLocked; }
    bool getIsClocked(void) { return true; }

    void attendCycleChanged(INT64 ps);

  signals:
    void status_msg_changed(QString);
    void cycle_msg_changed(QString);
    void row_msg_changed(QString);
    void mdiCloseEvent(MDIWindow*);
    void firstCycle(MDIWindow *);
    void lastCycle(MDIWindow *);
    void cycleChanged(Q_INT64 ps);
    void syncToCycle(CYCLE cycle);
    void syncToCycleTrace(CYCLE cycle);

  public slots:
    void showContextMenu(int row, int col, const QPoint &point);
    void do_showSlotInfo();
    void do_syncToCycle();
    void do_syncToCycleTrace();
    void do_switchRebel();

    void setFPS(const QString &fps);
    void setCPF(const QString &fps);
    void nextFrame();
    void setFrame(int cycle);
    void sliderPressed();
    void sliderReleased();

    void showCycle(CYCLE cycle);
    void rewind(void);
    void play(void);
    void stepForward(void);
    void stepBackward(void);
    void stop(void);
    void forward(void);

    void setFocus();

  protected:
    void closeEvent( QCloseEvent * );
    inline CYCLE sliderValueToCycle(UINT32 value) const;
    inline UINT32 cycleToSliderValue(CYCLE cycle) const;
    QString getCycleString() const;

    void addContextMenu(void);
    QPopupMenu *contextMenu;
    QPopupMenu *contextMenuSlotTools;
    QPopupMenu *contextMenuPlayTools;
    QPopupMenu *contextMenuSyncTools;
    int show_slot_info_id;
    int rew_id;
    int stepb_id;
    int play_id;
    int stop_id;
    int stepf_id;
    int forw_id;
    int rebel_id;
    int popUpRow;

  protected:
    QMainWindow* mainWin;
    QWidget* myParent;

    bool mdiLocked;

    QVBox   *pqvbLayout;
    QSlider *psldCycle;
    QTable  *ptblSlots;
    QTimer  *pTimer;

    bool sliderDrag; ///< True if the slider is being dragged.

    UINT32 nFPS;
    UINT32 nCPF;

    INT32 lastValue; ///< Last value of the slider. Used to compute displacements of cycles.

    // Direction of the time
    // If 1, forward, if -1 backward
    int nextCycle;

    // Indicates if we are playng at normat rate (play) or fast (rewind/forward)
    UINT32 fastStep;

  private:
    WatchWindow * watchData;
} ;

CYCLE
MDIWatchWindow::sliderValueToCycle(UINT32 value) const
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
MDIWatchWindow::cycleToSliderValue(CYCLE cycle) const
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

#endif /* _MDIWATCHWINDOW_H_ */

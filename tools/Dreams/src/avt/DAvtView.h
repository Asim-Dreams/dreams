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
  * @file  DAvtView.h
  * @brief Virtual class that define the common interface between the main application
  * control and the differents views (Resource & Floorplan) of the current event file.
  */

#ifndef _DAVTVIEW_H
#define _DAVTVIEW_H

#include "DreamsDefs.h"
#include "asim/AScrollView.h"
#include "highlight/HighLightMgr.h"

#include <qmainwindow.h>
#include <qglobal.h>

// Class forwarding.
class FindDialogImpl;
class PreferenceMgr;
class DisDockWin;
class ItemDockWin;
class EventDockWin;
class FavoriteTags;

/**
 * This is a specialization of AScrollView to Dreams project.
 * @brief Virtual class that define the common interface between the main application
 *        control and the differents views (Resource & Floorplan) of the current event file.
 * @version 0.1
 * @date started at 2004-06-07
 * @author Santi Galan
 */
class DAvtView : public AScrollView
{
    Q_OBJECT

  protected:
    QMainWindow*    myMDI;
    QMainWindow*    mainWin;

    QString         currentTagSelector;
    HighLightType   htype;

    int numCycles;
    CYCLE currentCycle;

    PreferenceMgr*  prefMgr;
    FavoriteTags * ft;

    // Find support
    FindDialogImpl *fndDialog;

    // Highlight support
    HighLightMgr *hlmgr;
    bool vHEvents;
    bool vNHEvents;
    bool disWinEnabled;

    // Docked windows
    DisDockWin * disWin;

    void initDisWindow();

  public:
    /**
     * Enum to distiguish between movement in the time or in the space. Is
     * used to transform the movement  in the x axis into cycles by the classes
     * that implement the scrollByMatrix method
     */
    DAvtView(QWidget* parent, QMainWindow* main, int width, int height);
    ~DAvtView();

    inline QMainWindow * getMDI()
    {
        return myMDI;
    }

    inline bool getMDILocked()
    {
        return mdiLocked;
    }

    inline void setMDILocked(bool v)
    {
        mdiLocked = v;
    }

    virtual void setMDINumber(int v);

    /**
     * Method to show a column the screen.
     *
     * @param x Column where the cursor should be displayed
     * @param moveToCursor Boolean that forces the screen to display where the cursor is
     */
    virtual void showCursor(CYCLE cycle, bool moveToCursor) { }

    /**
     * Method that performs the search if a dialog is avaliable All the
     * subclasses are responsibles of creating a proper search dialog depending
     * theis typus.
     */
    void find(void);

    /**
     * Method that continues a previous search if a search dialog is avaliable
     * All the subclasses are responsibles of creating a proper search dialog
     * depending theis type.
     */
    void findNext(void);

    virtual void scrollByMatrix(double x, double y, bool time = false)
    {
        AScrollView::scrollByMatrix(x, y);
    }

    virtual void annotationCopy() = 0;
    virtual void annotationPaste() = 0;
    virtual void annotationRemove() = 0;

    // highlight stuff methods
    void disableLastHightlight();
    void disableAllHighlight();
    void showHighlightCriteria();
    void viewHEvents();
    void viewNHEvents();
    void viewAllEvents();
    inline void changeHighlightSize(UINT32 size);

    HighLightMgr* getHighLightMgr()
    {
        return hlmgr;
    }
    inline void copyHighLightMgrState(HighLightMgr* value);
    inline void slotTgSelComboActivated(const QString & s);

    inline DisDockWin* getDisWindow();
    //inline ItemDockWin* getItemWindow();
    inline void setEnabledDisWindow(bool);
    inline bool getEnabledDisWindow();

    inline QString getCurrentTagSelector();

    inline void goToPoint(INT32 x, INT32 y);

    virtual void showCycle(CYCLE cycle) = 0;

    // Set of methods that attend the different movement signals emitted by
    // the other views
    virtual void attendCycleChanged(INT64 ps) { }
    virtual void attendResourceChanged(double resource) { }
    virtual void attendItemChanged(double item) { }
    virtual void attendThreeDMoved(double x, double y) { }
    virtual void attendFourDMoved(double x, double y, double z) { }

  signals:
    void purgeLastHighlight();
    void purgeAllHighlight();

    // Movement signals
    void cycleChanged(Q_INT64 ps);
    void resourceChanged(double resource);
    void itemChanged(double item);
    void threeDMoved(double x, double y);
    void fourDMoved(double x, double y, double z);

  public slots:
    /** 
     * Slot called when the AScrollview slidebars are moved It is used to
     * properly generate the signal adequate to the subtype of AvtView
     *
     * TODO
     * When the 4Dreams views is implemented, maybe a 3rd parameter (z axys)
     * should be added
     */
    virtual void raiseMovementSignal(double x, double y) = 0;
};

void
DAvtView::changeHighlightSize(UINT32 size)
{
    if(hlmgr != NULL)
    {
        hlmgr->changeSize(size);
    }
}

void 
DAvtView::copyHighLightMgrState(HighLightMgr* value) 
{ 
    if(hlmgr != NULL)
    {
        hlmgr->copyState(value);
    }
}

void
DAvtView::setEnabledDisWindow(bool v)
{
    disWinEnabled = v;
}

bool 
DAvtView::getEnabledDisWindow()
{
    return disWinEnabled;
}

DisDockWin *
DAvtView::getDisWindow()
{
    return disWin;
}

/*
inline ItemDockWin*
DAvtView::getItemWindow()
{ return itemWin; }
*/

QString
DAvtView::getCurrentTagSelector()
{
    return currentTagSelector;
}

void 
DAvtView::goToPoint(INT32 x, INT32 y)
{
    INT32 cw   = (INT32)((double)visibleWidth()/scf_x);
    INT32 ch   = (INT32)((double)visibleHeight()/scf_y);
    INT32 cx   = (INT32)((double)contentsX()/scf_x);
    INT32 cy   = (INT32)((double)contentsY()/scf_y);

    // if is already visible dont move!
    if((x >= cx) && (x <= (cx+cw)) && (y >= cy) && (y <= (cy+ch))) return;
        
    INT32 scaledX = (int)floor((double)x*scf_x);
    INT32 scaledY = (int)floor((double)y*scf_y);
    setContentsPos(scaledX - visibleWidth()/2, scaledY - visibleHeight()/2);
}   

void 
DAvtView::slotTgSelComboActivated(const QString & s)
{
    currentTagSelector = s;
    if(hlmgr)
    {
        hlmgr->purgeAll();
    }
    repaintContents(false);
}

#endif /* _DAVTVIEW_H */

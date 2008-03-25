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
 * @file ResourceDAvtView.h
 */

#ifndef _RESOURCEDAVTVIEW_H
#define _RESOURCEDAVTVIEW_H

// General includes.
#include <vector>
using namespace std;

// Dreams includes.
#include "dDB/DralDB.h"
#include "DreamsDefs.h"
#include "AColorPalette.h"
#include "DrawingTools.h"
#include "highlight/HighLightMgrResource.h"
#include "DreamsDB.h"
#include "DAvtView.h"

// Qt includes.
#include <qvaluelist.h>
#include <qmainwindow.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qtoolbar.h>

// Asim includes.
#include "asim/AScrollView.h"

// Class forwarding.
class LayoutResourceCache;
class AncestorDockWin;
class PushoutDockWin;

// Some defines.
const INT32 cube_size = CUBE_SIZE;
const INT32 cube_border_size = CUBE_BORDER;
const INT32 cube_side = CUBE_SIDE;

class TipResourceDreams : public QToolTip
{
  public:
    TipResourceDreams(QWidget * parent);

  protected:
    void maybeTip(const QPoint &);
} ;

/**
  * This was a specialization of AScrollView to Dreams project.
  * After the addition of the Floorplan view inherits from the common interface
  * with the Floorplan view, the DAvtView. This may be revisited.
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class ResourceDAvtView : public DAvtView 
{
  Q_OBJECT
    friend class TipResourceDreams;

  public:
    ResourceDAvtView(DreamsDB * _dreamsdb, LayoutResourceCache * _cache, QWidget * parent, QMainWindow * mdi, QMainWindow * main, INT32 numcycles = 1, INT32 numevents = 1);
    ~ResourceDAvtView();

    void setRealContentsSize();
    void reset();

    inline AncestorDockWin * getAncestorWindow() const;
    inline PushoutDockWin * getPushoutWindow() const;
    inline void setEnabledAncestorWindow(bool v);
    inline void setEnabledPushoutWindow(bool v);
    inline bool getEnabledAncestorWindow() const;
    inline bool getEnabledPushoutWindow() const;

    void updateTagWindow(ItemHandler * hnd);

    inline CYCLE getCentralCycle();

    void setAncestorTrackingTag(QString str);

    //virtual void setStatusBar(QStatusBar* value);
    //void  addTagSelector(QToolBar* parent);

    // highlight stuff methods
    void disableLastHightlight();
    void disableAllHighlight();
    void showHighlightCriteria();
    void viewHEvents();
    void viewNHEvents();
    void viewAllEvents();

    void clearShadedColumns();
    void clearShadedColumnsNoPropagate();

    void showCursor(CYCLE cycle, bool moveToCursor);
    void showCycle(CYCLE cycle);
    void showItem(QString edgeName, QString fromNodeName, QString toNodeName, QString tagName, UINT64 tagValue);
    
    //void forceRefreshDis();

    void refreshFavoriteTagList();

    QString getTipString(bool eventOnly);
    void setMDINumber(int v);
    QString getRowInfo(INT32 selrow, INT32 selcol);

    void annotationCopy();
    void annotationPaste();
    void annotationRemove();
    void cloneAnnotationCtrl(AnnotationCtrl*);    
    
    inline double getBaseElementSize();
    void goToColumn(INT32 column);
    void goToRow(INT32 row);
    inline INT32 getInternalBaseElementSize() const;

    void foldingChanged();
    void getFoldInfo(UINT32 * offset, double * scfy) const;

  signals:
    void clearAllShadedCols();
    void syncToCycle(CYCLE cycle);
    void syncToCycleTrace(CYCLE cycle);
    void syncToItem(QString edgeName, QString fromNodeName, QString toNodeName, QString tagName, UINT64 tagValue);

  public slots:
    void do_ancScan();
    void do_showColInfo();
    void do_showAllTags();
    void do_showAllEvents();
    void do_showOccupancy();
    void do_showRowInfo();
    void do_showRowTrackId();
    void do_syncToCycle();
    void do_syncToCycleTrace();
    void do_syncToItem();
    void do_switchRebel();

    void raiseMovementSignal(double x, double y);

  protected:
    void createAncestorWindow();
    void createPushoutWindow();
    //bool eventFilter ( QObject * watched, QEvent * e );
    void keyReleaseEvent(QKeyEvent * e);
    void contentsMovingForce(int x, int y, bool force);
    void drawDstRubber(int x, int y, int mx);
    void drawDstRubberDistance(QPainter * p, QString * str, bool high, INT32 y, INT32 vx);

    void mouseWorldMoveEvent(QMouseEvent * e, double x, double y);
    void rightClickPressEvent(QMouseEvent * e);
    void rightClickReleaseEvent(QMouseEvent * e);

    /** double click events are processed with this function */
    void mouseWorldDoubleClickEvent(QMouseEvent* e, double x, double y);

    virtual bool drawWorldCoordinatesContents(QPainter * p, int ncx, int ncy, int ncw, int nch);
    void drawCursorColumn(QPainter * p, int x1, int x2, int y1, int y2);

    void drawHighlights(QPainter * p, int x1, int x2, int y1, int y2, bool visible);
    
    inline bool hasContentsOn(int col, int row);

    void contentsSelectEvent(INT32 col, INT32 row, bool adding, bool shifted);

    void updateRowBar(double x, double y);
    void updateCycleBar(double x);

    void addContextMenu();
    void updateCloseMenuState();
    void applyMagicWand(int mx, int my);
    void showDistance(int pressed_mx, int released_mx);

    void toCycle(INT32 col, INT32 row, bool inval, CYCLE * cycle) const;
    INT32 toCol(CYCLE * cycle) const;

    void initShadeFlags();
    void clearShadeFlags();
    void drawShadingColumns(int x1, int x2, int y1, int y2, QPainter * p);
    void switchShadeFlag(INT32 col);

public:
    void shadeColumn(INT32 col, bool shade);
    void shadeColumn(CYCLE cycle, bool shade);
    void copyColumnShadows(AScrollView * parent);

  private:
    DreamsDB * dreamsdb;   ///< Pointer to the trace.
    DralDB * draldb;       ///< Pointer to the database.
    LayoutResourceCache * cache; ///< Cache of the mapping to LayoutResource.
     
    int numCycles;
    int numEvents;

    // Cursor support
    CYCLE cursorCycle;
    QBrush brushCursor;

    // for context menu popup
    QPopupMenu* contextMenu;
    QPopupMenu* contextMenuRowTools;
    QPopupMenu* contextMenuEventTools;
    QPopupMenu* contextMenuSyncTools;
    int rc_pressed_x;
    int rc_pressed_y;
    int rc_released_x;
    int rc_released_y;

    INT32 popup_row;        ///< Row where the right click has been done.
    CYCLE popup_cycle;      ///< Cycle where the right click has been done.
    CYCLE popup_cycle_sync; ///< Cycle where the right click has been done. No invalidation and used for sync.

    int rel_scan_id;
    int show_color_id;
    int show_all_tags_id;
    int show_all_events_id;
    int show_row_occ_id;
    int show_row_info_id;
    int sync_to_item_id;
    int rebel_id;
    
    INT64 totalPs;     ///< Total ps of the trace.
    INT64 totalPixels; ///< Total pixels of the resource layout.

    TipResourceDreams* tip;

    AnnotationItem* copyAnnItem;
    vector<ColumnShadows *> myShadows;
    
    //QAccel *escAccel;
    //bool escapeHitted;
  private: // for docked windows
    AncestorDockWin * ancWin;
    PushoutDockWin * pushWin;

    bool ancWinEnabled;
    bool pushWinEnabled;
};

AncestorDockWin *
ResourceDAvtView::getAncestorWindow() const
{
    return ancWin;
}

PushoutDockWin *
ResourceDAvtView::getPushoutWindow() const
{
    return pushWin;
}

void
ResourceDAvtView::setEnabledAncestorWindow(bool v)
{
    ancWinEnabled = v;
}

void
ResourceDAvtView::setEnabledPushoutWindow(bool v)
{
    pushWinEnabled = v;
}

bool
ResourceDAvtView::getEnabledAncestorWindow() const
{
    return ancWinEnabled;
}

bool
ResourceDAvtView::getEnabledPushoutWindow() const
{
    return pushWinEnabled;
}

double
ResourceDAvtView::getBaseElementSize()
{
    return (double) 1.0;
}

INT32
ResourceDAvtView::getInternalBaseElementSize() const
{
    return cube_side;
}

bool 
ResourceDAvtView::hasContentsOn(int col, int row)
{
    return false;
}

/*
 * Method to compute the cycle related to the given X position
 * 
 * @return the LCM cycle that corresponds to this element.
 */
CYCLE
ResourceDAvtView::getCentralCycle()
{
    ClockDomainFreqIterator it = draldb->getClockDomainFreqIterator();
    CYCLE cycle = draldb->getFirstEffectiveCycle((* it)->getId());
    INT32 disp = (INT32) ((contentsX() + (visibleWidth() >> 1)) / QMAX(1.0, getBaseElementSize()));

    // Checks if the divisions are used.
    if(dreamsdb->layoutResourceGetUsesDivisions())
    {
        cycle.cycle += disp >> 1;
        cycle.division = disp & 1;
    }
    else
    {
        cycle.cycle += disp;
        cycle.division = 0;
    }

    return cycle;
}

#endif

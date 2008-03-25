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
  * @file WaterfallDAvtView.h
  */

#ifndef _WATERFALLAVTVIEW_H
#define _WATERFALLAVTVIEW_H

// General includes.
#include <iostream>

// Dreams includes.
#include "DreamsDefs.h"
#include "dDB/DralDB.h"
#include "DAvtView.h"
#include "DreamsDB.h"

// Qt includes.
#include <qvaluelist.h>
#include <qmainwindow.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qtoolbar.h>
#include <qlcdnumber.h>

// Asim includes.
#include "asim/AScrollView.h"

class TipWaterfallDreams : public QToolTip
{
public:
    TipWaterfallDreams( QWidget * parent );

protected:
    void maybeTip( const QPoint & );
};

/**
 * This is a specialization of AScrollView for waterfall view
 *
 * @version 0.1
 * @date started at 2004-10-28
 * @author Federico Ardanaz 
 */
class WaterfallDAvtView : public DAvtView 
{
  Q_OBJECT
    friend class TipWaterfallDreams;

  public:

    static const int cube_size = CUBE_SIZE_BIG;
    static const int cube_border_size = CUBE_BORDER_BIG;
    static const int cube_side = CUBE_SIDE_BIG;

    WaterfallDAvtView(DreamsDB * _dreamsdb, QWidget * parent, QMainWindow * mdi, QMainWindow * main);
    ~WaterfallDAvtView();

    void reset();
    inline bool hasContentsOn(int col, int row);

    inline QMainWindow*  getMDI() { return myMDI; }
    inline bool getMDILocked() { return mdiLocked; }
    inline void setMDILocked(bool v) { mdiLocked=v; }

    QString getTipString(bool eventOnly);
    QString getRowInfo(INT32 selrow);
   
    void showCycle(CYCLE cycle);

    // highlight stuff methods
    void disableLastHightlight();
    void disableAllHighlight();
    void showHighlightCriteria();
    void viewHEvents();
    void viewNHEvents();
    void viewAllEvents();

    void annotationCopy();
    void annotationPaste();
    void annotationRemove();
    void cloneAnnotationCtrl(AnnotationCtrl*);    

    void setSelectedRow(int row);
    inline double getBaseElementSize();
    inline INT32 getInternalBaseElementSize() const;

    void switchRebel(bool value);

    void clearShadedColumns();
    void clearShadedColumnsNoPropagate();

  signals:
    void clearAllShadedCols();
    void syncToCycle(CYCLE cycle);
    void syncToCycleTrace(CYCLE cycle);

  public slots:
    void raiseMovementSignal(double x, double y);

    void do_switchRebel();
    void do_showAllEvents();
    void do_showAllTags();
    void do_syncToCycle();
    void do_syncToCycleTrace();

  protected:
    void keyReleaseEvent(QKeyEvent* e);
    void contentsMovingForce(int x, int y, bool force);
    void drawDstRubber(int x, int y, int mx);

    void mouseWorldMoveEvent(QMouseEvent* e, double x, double y);
    void rightClickPressEvent(QMouseEvent* e);
    void rightClickReleaseEvent(QMouseEvent* e);

    /** double click events are processed with this function */
    void mouseWorldDoubleClickEvent (QMouseEvent* e, double x, double y);

    void toCycle(INT32 col, INT32 row, bool inval, CYCLE * cycle) const;
    INT32 toCol(CYCLE * cycle) const;

    void drawDstRubberDistance(QPainter * p, QString * str, bool high, INT32 y, INT32 vx);

    void contentsSelectEvent(INT32 col, INT32 row, bool adding, bool shifted);

    virtual bool drawWorldCoordinatesContents(QPainter* p, int ncx, int ncy, int ncw, int nch);

    void updateTagWindow(ItemHandler * hnd);
    void updateRowBar(double x, double y);

    void addContextMenu();
    void updateCloseMenuState();
    void applyMagicWand (int mx,int my);
    void updateCycleBar (double x);
    void showDistance(int pressed_mx, int released_mx);

    void drawShadingColumns(int x1, int x2, int y1, int y2, QPainter * p);
    void switchShadeFlag(INT32 col);

  public:
    void shadeColumn(INT32 col, bool shade);

  private:
    DreamsDB * dreamsdb;
    DralDB * draldb;
    PreferenceMgr * prefMgr;
    
    // For mouseWorldMoveEvent
    int current_x;
    int current_y;

    // for context menu popup
    QPopupMenu * contextMenu;
    QPopupMenu * contextMenuItemTools;
    QPopupMenu * contextMenuEventTools;
    QPopupMenu * contextMenuSyncTools;

    INT32 popup_row;        ///< Row where the right click has been done.
    INT32 popup_col;        ///< Col where the right click has been done.
    CYCLE popup_cycle;      ///< Cycle where the right click has been done.
    CYCLE popup_cycle_sync; ///< Cycle where the right click has been done. No invalidation and used for sync.

    int rc_pressed_x;
    int rc_pressed_y;
    int rc_released_x;
    int rc_released_y;

    INT64 totalPs;     ///< Total ps of the trace.
    INT64 totalPixels; ///< Total pixels of the resource layout.

    // Context menu actions identifyers
    int show_all_events_id;
    int show_all_tags_id;
    int rebel_id;
    
    TipWaterfallDreams* tip;

    AnnotationItem* copyAnnItem;
    int selectedRow;
    QColor background;
};

/*
void 
WaterfallDAvtView::attendThreeDMoved(double x, double y)
{
    scrollByMatrix(x, y);
}
*/

double
WaterfallDAvtView::getBaseElementSize() 
{
    return (double) 1.0;
}

INT32
WaterfallDAvtView::getInternalBaseElementSize() const
{
    return cube_side;
}

bool 
WaterfallDAvtView::hasContentsOn(int col, int row)
{
    ItemHandler tmphnd;
    ExpressionList* tmperuleptr;
    return dreamsdb->layoutWaterfallHasValidData(row, col, &tmphnd, &tmperuleptr);
}

#endif

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
  * @file FloorplanDAvtView.h
  */

#ifndef _FLOORPLANDAVTVIEW_H
#define _FLOORPLANDAVTVIEW_H

// General includes.
#include <iostream>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "layout/floorplan/LayoutFloorplan.h"
#include "layout/floorplan/LayoutFloorplanRectangle.h"
#include "DreamsDB.h"
#include "DAvtView.h"
#include "highlight/HighLightMgrFloorplan.h"
#include "DreamsDefs.h"

// Asim includes.
#include "asim/AScrollView.h"

// Qt include.
#include <qvaluelist.h>
#include <qmainwindow.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qtoolbar.h>
#include <qlcdnumber.h>

class TipFloorplanDreams : public QToolTip
{
  public:
    TipFloorplanDreams(QWidget * parent);

  protected:
    void maybeTip(const QPoint & );
} ;

/**
 * This is a specialization of AScrollView to Dreams project.
 * This window will represent the Floorplan view of the events file. This file
 * was born from ResourceDAvtView. So it will evolve from this file to the final 
 * implementation showing the defined Floorplan view
 *
 * @version 0.1
 * @date started at 2004-06-03
 * @author Santi Galan
 */
class FloorplanDAvtView : public DAvtView 
{
  Q_OBJECT
    friend class TipFloorplanDreams;

  public:

    static const int cube_size = CUBE_SIZE;
    static const int cube_border_size = CUBE_BORDER;
    static const int cube_side = CUBE_SIDE;

    FloorplanDAvtView(DreamsDB * _dreamsdb, QWidget * parent, QMainWindow * mdi, QMainWindow * main);
    ~FloorplanDAvtView();

    void reset();

    inline QMainWindow*  getMDI() { return myMDI; }

    QString getTipString(bool eventOnly);
    void slotTgSelComboActivated(const QString &s );
    QString getRowInfo(INT32 selrow);

    // highlight stuff methods
    void disableLastHightlight();
    void disableAllHighlight();
    void showHighlightCriteria();
    void viewHEvents();
    void viewNHEvents();
    void viewAllEvents();

    // View synch methods
    inline void attendThreeDMoved(double x, double y);

    // Floorplan play controls methods
    void rewind();
    void play();
    void stop();
    void forward();

    void annotationCopy();
    void annotationPaste();
    void annotationRemove();
    void cloneAnnotationCtrl(AnnotationCtrl*);    
    
  signals:
    void clearAllShadedCols();
    void syncToCycle(CYCLE cycle);
    void syncToCycleTrace(CYCLE cycle);

  public slots:
    inline void raiseMovementSignal(double x, double y);

    void showCycle(CYCLE cycle);

    void do_showSlotInfo();
    void do_launchWatchWindow();
    void do_syncToCycle();
    void do_syncToCycleTrace();
    void do_switchRebel();

  protected:
    void keyReleaseEvent(QKeyEvent* e);
    void contentsMovingForce(int x, int y, bool force);
    void drawDstRubber(int x, int y, int mx);

    void mouseWorldMoveEvent(QMouseEvent* e, double x, double y);
    void rightClickPressEvent(QMouseEvent* e);
    void rightClickReleaseEvent(QMouseEvent* e);

    /** double click events are processed with this function */
    void mouseWorldDoubleClickEvent (QMouseEvent* e, double x, double y);

    inline double getBaseElementSize();

    void contentsSelectEvent(INT32 col, INT32 row, bool adding, bool shifted);

    virtual bool drawWorldCoordinatesContents(QPainter* p, int ncx, int ncy, int ncw, int nch);
    inline bool drawFullScale(QPainter* painter, QPtrList<LayoutFloorplanNode> &qlistVisible);
    bool drawNoFullScale(int x1,int x2, int y1, int y2, QPainter* painter);
    bool drawSubSamplingScale(int x1,int x2, int y1, int y2, QPainter* painter);

    inline void drawHighlights(QPainter* p, int x1, int x2, int y1, int y2);
    inline void drawFullScaleEvent(int mcol, int mrow, QPainter* painter);
    inline void drawNoFullScaleEvent(int mcol, int mrow, QPainter* painter);
    inline void drawSubSamplingScaleEvent(int mcol, int mrow, QPainter* painter);

    void drawShadingColumns(int x1, int x2, int y1, int y2, QPainter * p);
    void showLastShadded(int x1) { };
    
    inline bool hasContentsOn(int col, int row);

    void updateTagWindow(ItemHandler * hnd);
    void updateRowBar(double x, double y);

    void addContextMenu();
    void updateCloseMenuState();
    void applyMagicWand (int mx,int my);
    void updateCycleBar (double x);
    void showDistance(int pressed_mx, int released_mx);

  private:
    DralDB *   draldb;
    DreamsDB * dreamsdb;

    int             numCycles;
    int             numEvents;

    // List to hold the nodes to be drawn given a section of the world.
    QPtrList<LayoutFloorplanNode> qlToDraw;

    // For mouseWorldMoveEvent
    int current_x;
    int current_y;

    // for context menu popup
    QPopupMenu* contextMenu;
    QPopupMenu *contextMenuSlotTools;
    QPopupMenu *contextMenuPlayTools;
    QPopupMenu *contextMenuSyncTools;
    int rc_pressed_x;
    int rc_pressed_y;
    int rc_released_x;
    int rc_released_y;
    int popup_col;
    int popup_row;

    // Context menu actions identifyers
    int show_slot_info_id;
    int launch_watch_id;
    int rew_id;
    int stepb_id;
    int play_id;
    int stop_id;
    int stepf_id;
    int forw_id;
    int rebel_id;
    
    TipFloorplanDreams* tip;

    AnnotationItem* copyAnnItem;
   
};


void 
FloorplanDAvtView::attendThreeDMoved(double x, double y)
{
    scrollByMatrix(x, y);
}


void 
FloorplanDAvtView::raiseMovementSignal(double x, double y)
{
    emit threeDMoved(x, y);
}

double
FloorplanDAvtView::getBaseElementSize() { return (double)1.0; }

inline bool
FloorplanDAvtView::drawFullScale(QPainter* painter, QPtrList<LayoutFloorplanNode> &qlistVisible)
{
    int cnt = 0;
    QPtrListIterator<LayoutFloorplanNode> it_rect(qlistVisible);
    LayoutFloorplanNode *rect;

    while((rect = it_rect.current()) != NULL)
    {
        ++it_rect;
        cnt++;
        emit progress_cnt_changed (cnt);

        rect->draw(painter, static_cast<HighLightMgrFloorplan *>(hlmgr), currentCycle, this);
    }
    return true;
}

inline bool 
FloorplanDAvtView::hasContentsOn(int col, int row)
{
    TRACK_ID trackId = dreamsdb->layoutFloorplanGetTrackIdForCell(col, row);

    //cerr << "FloorplanDAvtView::hasContentsOn: col = " << col << " row = " << row << " trackId = " << trackId << endl;
    if(trackId != TRACK_ID_INVALID)
    {
        ItemHandler hnd;
        UINT16 clockId = draldb->getClockId(trackId);
        CYCLE cycle(draldb->getClockDomain(clockId), currentCycle.cycle, 0);

        return draldb->getItemInSlot(trackId, cycle, &hnd);
    }

    return false;
}

#endif


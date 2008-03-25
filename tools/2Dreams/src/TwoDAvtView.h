// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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
  * @file  TwoDAvtView.h
  */

#ifndef _TWODAVTVIEW_H
#define _TWODAVTVIEW_H

#include <qvaluelist.h>
#include <qmainwindow.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qtoolbar.h>

#include "asim/AScrollView.h"
#include "asim/DralDB.h"

#include "AColorPalette.h"
#include "ZDBase.h"
#include "ConfigDB.h"
#include "DrawingTools.h"
#include "DisDockWin.h"
#include "EventDockWin.h"
#include "AncestorDockWin.h"
#include "HighLightMgr.h"
#include "eventTagDialogImpl.h"
#include "PreferenceMgr.h"
#include "2DreamsDefs.h"

const int cube_size = CUBE_SIZE;
const int cube_border_size = CUBE_BORDER;
const int cube_side = CUBE_SIDE;

class Tip2Dreams : public QToolTip
{
public:
    Tip2Dreams( QWidget * parent );

protected:
    void maybeTip( const QPoint & );
};

/**
  * This is a specialization of AScrollView to 2Dreams project.
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class TwoDAvtView : public AScrollView
{
Q_OBJECT
friend class Tip2Dreams;

public:
    TwoDAvtView (
                QMainWindow* parent,
                QMainWindow* main,
                int numcycles = 1,
                int numevents = 1
                );
    ~TwoDAvtView();

    void setRealContentsSize(INT32 cycles, INT32 events);
    void reset();

    inline DisDockWin* getDisWindow();
    inline EventDockWin* getEventWindow();
    inline AncestorDockWin* getAncestorWindow();
    inline HighLightMgr* getHighLightMgr();
    inline void setHighLightMgr(HighLightMgr* value);
    inline QMainWindow*  getMDI() { return myMDI; }
    inline bool getMDILocked() { return mdiLocked; }
    inline void setMDILocked(bool v) { mdiLocked=v; }
    inline void setEnabledDisWindow(bool);
    inline void setEnabledEventWindow(bool);
    inline void setEnabledAncestorWindow(bool);
    inline bool getEnabledDisWindow();
    inline bool getEnabledEventWindow();
    inline bool getEnabledAncestorWindow();

    void setAncestorTrackingTag(QString str);

    //virtual void setStatusBar(QStatusBar* value);
    //void  addTagSelector(QToolBar* parent);
    inline QString getCurrentTagSelector();

    // highlight stuff methods
    void disableLastHightlight();
    void disableAllHighlight();
    void showHighlightCriteria();
    void viewHEvents();
    void viewNHEvents();
    void viewAllEvents();

    void clearShadedColumns();
    void clearShadedColumnsNoPropagate();
    
    //void forceRefreshDis();

    void refreshFavoriteTagList();
    void updateEventWindow(ItemHandler * hnd);

    QString getTipString(bool eventOnly);
    void slotTgSelComboActivated(const QString &s );
    void setMDINumber(int v);
    QString getRowInfo(INT32 selrow);

    void annotationCopy();
    void annotationPaste();
    void annotationRemove();
    void cloneAnnotationCtrl(AnnotationCtrl*);    
    
signals:
    void clearAllShadedCols();
    void contentsSelectedEvent(int,int,bool,QString,HighLightType,bool);
    void purgeLastHighlight();
    void purgeAllHighlight();
    
public slots:
    void do_ancScan ();
    void do_showAllTags();
	void do_showRowInfo();
    void do_switchRebel();
    //void escKey();

protected:
    void createDisWindow();
    void createEventWindow();
    void createAncestorWindow();
    //bool eventFilter ( QObject * watched, QEvent * e );
    void keyReleaseEvent(QKeyEvent* e);
    void contentsMovingForce(int x, int y, bool force);
    void drawDstRubber(int x, int y, int mx);

    void mouseWorldMoveEvent(QMouseEvent* e, double x, double y);
    void rightClickPressEvent(QMouseEvent* e);
    void rightClickReleaseEvent(QMouseEvent* e);

    /** double click events are processed with this function */
    void mouseWorldDoubleClickEvent (QMouseEvent* e, double x, double y);

    inline double getBaseElementSize();

    virtual bool drawWorldCoordinatesContents(QPainter* p, int ncx, int ncy, int ncw, int nch);
    bool drawFullScale(int x1,int x2, int y1, int y2, QPainter* painter);
    bool drawNoFullScale(int x1,int x2, int y1, int y2, QPainter* painter);
    bool drawSubSamplingScale(int x1,int x2, int y1, int y2, QPainter* painter);

    inline void drawHighlights(QPainter* p, int x1, int x2, int y1, int y2, bool visible);
    inline void drawFullScaleEvent(int mcol, int mrow, QPainter* painter);
    inline void drawNoFullScaleEvent(int mcol, int mrow, QPainter* painter);
    inline void drawSubSamplingScaleEvent(int mcol, int mrow, QPainter* painter);
    
    inline bool hasContentsOn(int col, int row);
    void contentsSelectEvent(int col,int row, bool adding, bool shifted);


    void updateRowBar(double x, double y);

    void addContextMenu();
    void updateCloseMenuState();
    void applyMagicWand (int mx,int my);
    void updateCycleBar (double x);
    void showDistance(int pressed_mx, int released_mx);

private:
    QMainWindow*    myMDI;
    QMainWindow*    mainWin;
    ZDBase*         myDB;
    ConfigDB*       myConfigDB;
    DralDB*         draldb;
    FavoriteTags*   ft;
    HighLightMgr*   hlmgr;
    QString         currentTagSelector;
    HighLightType   htype;
    //QLabel*         currentOccLbl;
    PreferenceMgr*  prefMgr;
    int numCycles;
    int numEvents;

     // for context menu popup
    QPopupMenu* contextMenu;
    int rc_pressed_x;
    int rc_pressed_y;
    int rc_released_x;
    int rc_released_y;
    int popup_col;
    int popup_row;

    int rel_scan_id;
    int show_all_tags_id;
	int show_row_info_id;
    int rebel_id;
	
    Tip2Dreams* tip;

    AnnotationItem* copyAnnItem;
    
    //QAccel *escAccel;
    //bool escapeHitted;
private: // for docked windows
    DisDockWin*   disWin;
    EventDockWin* eventWin;
    AncestorDockWin* ancWin;

    bool disWinEnabled;
    bool eventWinEnabled;
    bool ancWinEnabled;

    bool vHEvents;
    bool vNHEvents;

};

void
TwoDAvtView::setEnabledDisWindow(bool v)
{ disWinEnabled = v; }

void
TwoDAvtView::setEnabledEventWindow(bool v)
{ eventWinEnabled = v; }

void
TwoDAvtView::setEnabledAncestorWindow(bool v)
{ ancWinEnabled = v; }

inline bool 
TwoDAvtView::getEnabledDisWindow()
{ return disWinEnabled; }

inline bool 
TwoDAvtView::getEnabledEventWindow()
{ return eventWinEnabled; }
    
inline bool 
TwoDAvtView::getEnabledAncestorWindow()
{ return ancWinEnabled; }
    
inline DisDockWin*
TwoDAvtView::getDisWindow()
{ return disWin; }

inline EventDockWin* 
TwoDAvtView::getEventWindow()
{ return eventWin; }

inline 
HighLightMgr* 
TwoDAvtView::getHighLightMgr()
{ return hlmgr; }

inline AncestorDockWin* 
TwoDAvtView::getAncestorWindow()
{ return ancWin; }

double
TwoDAvtView::getBaseElementSize() { return (double)cube_side; }

inline void
TwoDAvtView::drawFullScaleEvent(int mcol, int mrow, QPainter* painter)
{
    QColor bcolor;
    bool bcolored;

    if ( myDB->hasValidData(mcol+myDB->getFirstEffectiveCycle(),mrow) )
    {
        ColorShapeItem csh = myDB->getColorShape(mcol+myDB->getFirstEffectiveCycle(),mrow);
        painter->setBrush(QBrush( AColorPalette::getColor(csh.getColorIdx())) );
        DrawingTools::drawShape(mcol*cube_side,mrow*cube_side,painter,csh.getShape(),cube_side);
    }
    else if (myConfigDB->isBlankRow (mrow,bcolored,bcolor))
    {
        if (bcolored)
        {
            painter->setBrush(QBrush(bcolor,Dense5Pattern));
            painter->setPen(NoPen);
            DrawingTools::drawRectangle(mcol*cube_side-1,mrow*cube_side-1,painter,cube_side+2);
            painter->setPen( QPen(QColor(0,0,0),cube_border_size));
        }
    }
}

inline void 
TwoDAvtView::drawNoFullScaleEvent(int mcol, int mrow, QPainter* painter)
{
    QColor bcolor;
    bool bcolored;

    //printf ("refreshing col = %d, row=%d\n",mcol,mrow);fflush(stdout);
    if ( myDB->hasValidData(mcol+myDB->getFirstEffectiveCycle(),mrow) )
    {
        ColorShapeItem csh = myDB->getColorShape(mcol+myDB->getFirstEffectiveCycle(),mrow);
        painter->setPen(QPen(AColorPalette::getColor(csh.getColorIdx()),1));
        painter->setBrush(QBrush( AColorPalette::getColor(csh.getColorIdx())) );
        DrawingTools::drawShape(mcol*cube_side,mrow*cube_side,painter,csh.getShape(),cube_side);
    }
    else if (myConfigDB->isBlankRow (mrow,bcolored,bcolor))
    {
        if (bcolored)
        {
            painter->setBrush(QBrush(bcolor,Dense5Pattern));
            painter->setPen(NoPen);
            DrawingTools::drawRectangle(mcol*cube_side-1,mrow*cube_side-1,painter,cube_side+2);
            painter->setPen( QPen(QColor(0,0,0),cube_border_size));
        }
    }
}

inline void 
TwoDAvtView::drawSubSamplingScaleEvent(int mcol, int mrow, QPainter* painter)
{
    QColor bcolor;
    bool bcolored;

    //printf ("refreshing col = %d, row=%d\n",mcol,mrow);fflush(stdout);
    if ( myDB->hasValidData(mcol+myDB->getFirstEffectiveCycle(),mrow) )
    {
        ColorShapeItem csh = myDB->getColorShape(mcol+myDB->getFirstEffectiveCycle(),mrow);
        painter->setPen(QPen(AColorPalette::getColor(csh.getColorIdx()),1));
        painter->drawPoint(mcol*cube_side,mrow*cube_side);
    }
    else if (myConfigDB->isBlankRow (mrow,bcolored,bcolor))
    {
        if (bcolored)
        {
            painter->setBrush(QBrush(bcolor,Dense5Pattern));
            painter->setPen(NoPen);
            DrawingTools::drawRectangle(mcol*cube_side-1,mrow*cube_side-1,painter,cube_side+2);
            painter->setPen( QPen(QColor(0,0,0),cube_border_size));
        }
    }
}

void 
TwoDAvtView::drawHighlights(QPainter* p, int x1, int x2, int y1, int y2, bool visible)
{
    // limit y1/y2
    //if (y1>=numEvents) y1 = numEvents-1;
    //if (y2>=numEvents) y2 = numEvents-1;

    // warning highlight update must be over the whole visible area
    double cx = contentsX();
    double cy = contentsY();
    double cw = visibleWidth();
    double ch = visibleHeight();

    // transform to AGT world coordinates
    double cwx = (cx/scf_x);
    double cwy = (cy/scf_y);
    double cww = (cw/scf_x);
    double cwh = (ch/scf_y);

    // rounding
    int rx1 = (int)floor(cwx);
    int rx2 = (int)floor(cwx+cww-1.0);
    int ry1 = (int)ceil(cwy);
    int ry2 = (int)ceil(cwy+cwh-1.0);

    // divide by element-size...
    int mx1 = (int)floor((double)rx1/cube_side);
    int mx2 = (int)ceil ((double)rx2/cube_side);
    int my1 = (int)floor((double)ry1/cube_side);
    int my2 = (int)ceil ((double)ry2/cube_side);

    // apply first effective cycle
    mx1 += myDB->getFirstEffectiveCycle();
    mx2 += myDB->getFirstEffectiveCycle();

    hlmgr->updateViewRange(mx1,mx2,my1,my2);

    // refresh can be done in the dirty area only
    hlmgr->drawHighlights(p,x1,x2,y1,y2,visible);
}

bool 
TwoDAvtView::hasContentsOn(int col, int row)
{
    bool result = myDB->hasValidData(col+myDB->getFirstEffectiveCycle(),row);
    return result;
}

QString
TwoDAvtView::getCurrentTagSelector()
{ return currentTagSelector; }

void 
TwoDAvtView::setHighLightMgr(HighLightMgr* value)
{
    delete hlmgr;
    hlmgr = value;
    connect(hlmgr,SIGNAL(highlightChanged(int)),mainWin,SLOT(highlightChanged(int)));
}


#endif


/*****************************************************************************
*
* @brief Scroll View used in qcache. Source File.
*
* @author Oscar Rosell
*
*  Copyright (C) 2003-2006 Intel Corporation
*  
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*  
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*  
*
*****************************************************************************/

#include "qcacheview.h"
#include "qcachemain.h"
#include "visualizations.h"
#include "dbase.h"
#include "QCacheSpecificOrders.h"
#include "HistoryList.h"
#include <math.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <iterator>

using qcdb::ItemDefinition;

QCacheView::QCacheView(QWidget *parent, int width, int height):AScrollView(parent,width,height),selectedChild_(NULL) 
{
    initShadeFlags();
    setWFlags(WRepaintNoErase);
}

QCacheView::~QCacheView()
{
}

void 
QCacheView::childAdded(QCacheViewItem* item)
{
    cout << "Child Added" << endl;
    children_.push_back(item);
}

void 
QCacheView::childDeleted(QCacheViewItem* item)
{
}

void 
QCacheView::contentsSelectEvent(int col, int row, bool adding)
{
}

void 
QCacheView::drawWorldCoordinatesContents(QPainter *p, int cx, int cy, int cw, int ch)
{
    for (ChildrenIterator it = children_.begin(); it!= children_.end(); ++it)
    {
        if ((*it)->intersects(QRect(cx,cy,cw==0?1:cw,ch==0?1:ch)))
        {
            (*it)->drawWorldCoordinatesContents(p,cx,cy,cw==0?1:cw,ch==0?1:ch);
        }
    }
}

void 
QCacheView::drawContents(QPainter* p, int cx, int cy, int cw, int ch)
{
    QCursor prevCursor;

    if (redrawing)
    {
        return;
    }    
    redrawing = true;

    double ncx = ((double)cx/scf_x);
    double ncy = ((double)cy/scf_y);
    double ncw = ((double)cw/scf_x);
    double nch = ((double)ch/scf_y);

    // create the fake painter for double buffering
    QPainter painter;
    ensureOffScrSize(cw,ch);
    painter.begin(&offscr);
    painter.fillRect(0,0,cw,ch,bgColor);
    painter.setBackgroundColor(bgColor);
    painter.translate(-(double)cx,-(double)cy);
    painter.scale(scf_x,scf_y);

    // call implementation dependent repaint routine
    drawWorldCoordinatesContents(&painter,(int)rint(ncx),(int)rint(ncy),(int)rint(ncw),(int)rint(nch));

    // grid line
    if (showGridLines)
    {
        drawGridLines(&painter,ncx,ncy,ncw,nch);
    }    

    // redraw annotation tools if any
    annCtrl->drawArea(&painter,ncx,ncy,ncw,nch,scf_x,scf_y);

    // bit copy of pixmap inside the original widget
    painter.end();
    p->drawPixmap(cx,cy,offscr,0,0,cw,ch);

    progressBar->hide();
    redrawing = false;
}

double 
QCacheView::getBaseElementSize()
{
    return 1.0;
}

bool 
QCacheView::hasContentsOn (int col, int row)
{
    return false;
}

void 
QCacheView::mouseWorldMoveEvent(QMouseEvent* e, double x, double y)
{
}

void 
QCacheView::leftClickPressEvent(QMouseEvent* e)
{
    int cx, cy;

    cout << "Left click" << endl;

    viewportToContents(e->x(),e->y(),cx,cy);

    for (ChildrenIterator it = children_.begin(); it!= children_.end(); ++it)
    {
        if ((*it)->contains(cx,cy))
        {
            (*it)->select();
        }
    }
}

void 
QCacheView::rightClickPressEvent(QMouseEvent* e)
{
    int cx, cy;

    cout << "Right click" << endl;

    viewportToContents(e->x(),e->y(),cx,cy);
    rightClickPressEvent(e,((double)cx/scf_x),((double)cy/scf_y));
}

void 
QCacheView::rightClickPressEvent(QMouseEvent* e, double x, double y)
{
    int x_int = static_cast<int>(x);
    int y_int = static_cast<int>(y);

    for (ChildrenConstIterator it = children_.begin(); it!= children_.end(); ++it)
    {
        if ((*it)->contains(x,y))
        {
            (*it)->rightClickEvent(x_int-(*it)->X(),y_int-(*it)->Y());
        }
    }
}

void 
QCacheView::rightClickReleaseEvent(QMouseEvent* e)
{
}

void 
QCacheView::mouseWorldDoubleClickEvent (QMouseEvent* e, double x, double y)
{
    int x_int = static_cast<int>(x);
    int y_int = static_cast<int>(y);

    for (ChildrenConstIterator it = children_.begin(); it!= children_.end(); ++it)
    {
        if ((*it)->contains(x,y))
        {
            (*it)->doubleClickEvent(x_int-(*it)->X(),y_int-(*it)->Y());
        }
    }
}

void 
QCacheView::mouseWorldPressEvent_Panning (QMouseEvent *e, double x, double y)
{
    ChildrenConstIterator it;

    for (it = children_.begin(); it!= children_.end(); ++it)
    {
        if ((*it)->contains(x,y))
        {
            selectedChild_ = (*it);
        }
    }
}

void 
QCacheView::mouseWorldReleaseEvent_Panning (QMouseEvent *e, double x, double y)
{
    int x_int = static_cast<int>(x);
    int y_int = static_cast<int>(y);

    if (selectedChild_)
    {
        // New object takes precedence in the painting. Painter's algorithm -> last painted -> fully painted
        // Last Object in the vector <=> Last Painted
        int i = 0;
        for (ChildrenIterator it = children_.begin(); it != children_.end(); ++it, ++i)
        {
            if ((*it)==selectedChild_)
            {                
                children_[i]=children_[children_.size()-1];
                children_.back() = selectedChild_;
            }
        } 

        Order_MoveItem* order = new Order_MoveItem(selectedChild_,QPoint(x_int, y_int));
        HistoryList hl;
        hl.execute(order);
        
//        selectedChild_->moveTo(x_int, y_int);
        selectedChild_ = NULL;
    }
}

void 
QCacheView::repaintWorldCoordinates(int x, int y, int w, int h, bool erase)
{
    QRect tempRect(static_cast<int>(x*scf_x)-2,
		   static_cast<int>(y*scf_y)-2,
		   static_cast<int>(w*scf_x)+4,
		   static_cast<int>(h*scf_y)+4);
    repaintContents(tempRect, erase);
}

void 
QCacheView::repaintWorldCoordinates(const QRect& window, bool erase)
{
    repaintWorldCoordinates(window.left(),window.top(),window.width(),window.height(),erase);
}

void 
QCacheView::updateCycle(UINT64 x)
{
    currentCycleLbl->setText(QString(" Cycle : ")+QString::number(static_cast<UINT32>(x))+QString(" "));
}

void 
QCacheView::Cycle(UINT64 cycle)
{
    // Update every ten cycles
    if ((cycle % 10) == 0)
        updateCycle(cycle);
}

void 
QCacheView::centerNode(UINT16 node_id)
{
}

void 
QCacheView::updateSize()
{
    // Set minimum values
    int newwidth = 1;
    int newheight = 1;

    // Look for the size needed -> all the children must be fit inside

    for (ChildrenIterator it=children_.begin();it!=children_.end(); ++it)
    {
        if ((*it)->right()>newwidth)
        {
            newwidth = (*it)->right();
        }
        if ((*it)->bottom()>newheight)
        {
            newheight = (*it)->bottom();
        }
    }
    int scalled_width  = static_cast<int>(ceil(newwidth* getScallingFactorX()));
    int scalled_height = static_cast<int>(ceil(newheight*getScallingFactorY()));

    setRealContentsSize(scalled_width,scalled_height);
    cout << "Size updated " << endl;
}

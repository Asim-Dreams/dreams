/*****************************************************************************
*
* @brief Item inside a qcache view with a bounding box. Source File.
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

#include "QCacheViewItem.h"
#include "qcacheview.h"
#include <qpoint.h>
#include "DrawingTools.h"

const char* QCacheViewItem::SELECTED_COLOR = "light blue";

QCacheViewItem::QCacheViewItem(QCacheView *parent, int width, int height) : 
    QObject(parent), 
    bbox_(0,0,width,height),
    selected_(false),
    hidden_(false)
{
    cerr << "QCacheViewItem being created " << parent << endl;

    parent_ = parent;
    if (parent != NULL)
    {
        parent_->childAdded(this);
        parent_->updateSize();
    }
}

QCacheViewItem::~QCacheViewItem()
{
}

void 
QCacheViewItem::setScrollView(QCacheView *parent)
{
    parent_ = parent;
    if (parent != NULL)
    {
        parent_->childAdded(this);
        parent_->updateSize(); 
    }
}

void 
QCacheViewItem::drawWorldCoordinatesContents (QPainter* p, int cx, int cy, int cw, int ch)
{
    if (!hidden_)
    {
        // We apply a transformation to transform from world coordinates to item coordinates
        p->translate(bbox_.left(),bbox_.top());

        drawItemCoordinatesContents(p,
                                (cx-bbox_.left())>0?(cx-bbox_.left())-1:0,
                                (cy-bbox_.top())>0?(cy-bbox_.top())-1:0,
                                cw+2,
                                ch+2
                               );
        // We return the painter to the previous state
        p->translate(-bbox_.left(),-bbox_.top());

        if (selected_)
        {
            p->setPen(SELECTED_COLOR);
            DrawingTools::drawRectangle(bbox_.left(),bbox_.top(),p,bbox_.width(),bbox_.height());			           
        }
    }
}

void 
QCacheViewItem::redrawItemCoordinatesContents(int cx, int cy, int cw, int ch)
{
    parent_->repaintWorldCoordinates(X()+cx,
                                     Y()+cy,
                                         cw,
                                         ch,
                                     false);
}

void 
QCacheViewItem::moveTo(int newx, int newy)
{
    cout << "Moving to: " << newx << "," << newy << endl;
    if (!hidden())
    {
        hide();
        bbox_.moveTopLeft(QPoint(newx,newy));
        unhide();
    }
    else
    {
        bbox_.moveTopLeft(QPoint(newx,newy));
    }

    parent_->updateSize();
}

void 
QCacheViewItem::moveTo(const QPoint& to)
{
    moveTo(to.x(),to.y());
}

void 
QCacheViewItem::moveBy(int disx, int disy)
{
    if (!hidden())
    {
        hide();
        bbox_.moveBy(disx, disy);
        unhide();
    }
    else
    {
        bbox_.moveBy(disx, disy);
    }
    parent_->updateSize();
}

void
QCacheViewItem::hide()
{ 
    hidden_ = true; 
    parent_->repaintWorldCoordinates(rect(), false);
};

void 
QCacheViewItem::unhide() 
{ 
    if (hidden())
    {
        hidden_ = false; 
        parent_->repaintWorldCoordinates(rect(), false);
    }
};
 
bool
QCacheViewItem::hidden()
{
    return hidden_;
}

int 
QCacheViewItem::X() const
{
    return bbox_.left();
}

int 
QCacheViewItem::Y() const
{
    return bbox_.top();
}

int 
QCacheViewItem::width() const
{
    return  bbox_.width();
}
int 
QCacheViewItem::height() const
{
    return  bbox_.height();
}

QCacheView* 
QCacheViewItem::scrollView()
{
    return parent_;
}

bool 
QCacheViewItem::contains(double x, double y) const
{
    return (bbox_.contains(QPoint(static_cast<int>(x),static_cast<int>(y))));
}

bool 
QCacheViewItem::intersects(const QCacheViewItem &other) const
{    
    return (bbox_.intersects(other.rect()));
}

bool 
QCacheViewItem::intersects(const QRect &other) const
{
    return (bbox_.intersects(other));
}

const QRect & 
QCacheViewItem::rect() const
{
    return (bbox_);
}

void 
QCacheViewItem::setWidth(const int width)
{
    bbox_.setWidth(width);
    if (parent_!=NULL)
    {
        cout << "Parent not NULL" << endl;
        parent_->updateSize();
    }
}

void 
QCacheViewItem::setHeight(const int height)
{
    bbox_.setHeight(height);
    if (parent_!=NULL)
    {
        cout << "Parent not NULL" << endl;
        parent_->updateSize();
    }
}

QPoint 
QCacheViewItem::bottomRight()
{
    return bbox_.bottomRight();
}

int 
QCacheViewItem::right()
{
    return bbox_.right();
}

int 
QCacheViewItem::bottom()
{
    return bbox_.bottom();
}

void 
QCacheViewItem::redraw()
{
    parent_->repaintWorldCoordinates(X(),
                                     Y(),
                                 width(),
                                height(),
                                  false);
}

void 
QCacheViewItem::doubleClickEvent(int x, int y)
{        
    redraw();
}

void 
QCacheViewItem::leftClickEvent(int x, int  y)
{
}


void 
QCacheViewItem::rightClickEvent(int x, int  y)
{
}

QString 
QCacheViewItem::label() const
{
    return QString("QCache View Item");
}

bool
QCacheViewItem::readConfiguration(const QDomElement& elem)
{
    bool ok = true;
    QPoint position;

    cout << "Reading configuration " << this->label() << endl;
    
    QString left = elem.attribute("left","NOT FOUND");

    if (left!="NOT FOUND")
    {
        bool is_a_number;
        int left_int = left.toInt(&is_a_number);
        if (is_a_number)
        {
            position.setX(left_int);
        }
        ok = ok && is_a_number;
    }

    QString top = elem.attribute("top","NOT FOUND");
    if (top!="NOT FOUND")
    {
        bool is_a_number;
        int top_int = top.toInt(&is_a_number);
        if (is_a_number)
        {
            position.setY(top_int);
        }
        ok = ok && is_a_number;
    }
    if (ok)
    {
        cout << "Setting new position from XML: " << position.x() << "," << position.y() << endl;
        bbox_.moveTopLeft(position);
    }
    return ok;
}

void 
QCacheViewItem::drawItemCoordinatesContents(QPainter* p, int cx, int cy, int cw, int ch)
{
}

void 
QCacheViewItem::select()
{
    cout << "Item selected" << endl;
    selected_ = true;
}

void 
QCacheViewItem::unselect()
{
    selected_ = false;
}


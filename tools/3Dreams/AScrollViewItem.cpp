/*
 * Copyright (C) 2003-2006 Intel Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */

#include "AScrollViewItem.h"
#include "AScrollView.h"
#include "qprova.h"
#include <qpoint.h>
#include "DrawingTools.h"

const char* AScrollViewItem::SELECTED_COLOR = "light blue";

AScrollViewItem::AScrollViewItem(AScrollView *parent, int width, int height) : 
    QObject(parent), 
    rect_(0,0,width,height),
    selected_(false),
    hidden_(false)
{
    cout << "AScrollViewItem creating" << endl;

    parent_ = static_cast<QCacheView *>(parent);
    if (parent_)
    {
        static_cast<QCacheView *>(parent)->updateSize();
    }
}

AScrollViewItem::~AScrollViewItem()
{

}

void AScrollViewItem::setScrollView(AScrollView *parent)
{
    if (parent)
    {
        parent_ = static_cast<QCacheView *>(parent);
        static_cast<QCacheView *>(parent_)->updateSize();
    }
}

void AScrollViewItem::drawWorldCoordinatesContents (QPainter* p, int cx, int cy, int cw, int ch)
{
    if (!hidden_)
    {
        // We apply a transformation to transform from world coordinates to item coordinates
        p->translate(rect_.left(),rect_.top());

        drawItemCoordinatesContents(p,
                                (cx-rect_.left())>0?(cx-rect_.left())-1:0,
                                (cy-rect_.top())>0?(cy-rect_.top())-1:0,
                                cw+2,
                                ch+2
                               );
        // We return the painter to the previous state
        p->translate(-rect_.left(),-rect_.top());

        if (selected_)
        {
            p->setPen(SELECTED_COLOR);
            DrawingTools::drawRectangle(rect_.left(),rect_.top(),p,rect_.width(),rect_.height());			           
        }
    }
}

void AScrollViewItem::redrawItemCoordinatesContents(int cx, int cy, int cw, int ch)
{
    (static_cast<QCacheView *>(parent_))->repaintWorldCoordinates(X()+cx,
                                                  Y()+cy,
                                                  cw,
                                                  ch,
                                                  false);
}

void AScrollViewItem::move(int newx, int newy)
{
    bool resizeNeeded;

    newx = max(newx,0);
    newy = max(newy,0);
    if (newx > X() || newy > Y())
    {
        resizeNeeded = true;
    }
    rect_.moveTopLeft(QPoint(newx,newy));
    if (resizeNeeded)
    {
        static_cast<QCacheView *>(parent_)->updateSize();
    }
}

int AScrollViewItem::X() const
{
    return rect_.left();
}

int AScrollViewItem::Y() const
{
    return rect_.top();
}

int AScrollViewItem::width() const
{
    return  rect_.width();
}
int AScrollViewItem::height() const
{
    return  rect_.height();
}

AScrollView* AScrollViewItem::scrollView()
{
    return parent_;
}

bool AScrollViewItem::contains(double x, double y) const
{
    return (rect_.contains(QPoint(static_cast<int>(x),static_cast<int>(y))));
}

bool AScrollViewItem::intersects(const AScrollViewItem &other) const
{    
    cout << "Left: " << X();
    cout << " Width: " << width() << endl;
    return (rect_.intersects(other.rect()));
}

bool AScrollViewItem::intersects(const QRect &other) const
{
    cout << "Left: " << X();
    cout << " Width: " << width() << endl;
    return (rect_.intersects(other));
}

const QRect & AScrollViewItem::rect() const
{
    return (rect_);
}

void AScrollViewItem::setWidth(const int width)
{
    rect_.setWidth(width);
    if (parent_!=NULL)
    {
        cout << "Parent not NULL" << endl;
        static_cast<QCacheView *>(parent_)->updateSize();
    }
}

void AScrollViewItem::setHeight(const int height)
{
    rect_.setHeight(height);
    if (parent_!=NULL)
    {
        cout << "Parent not NULL" << endl;
        static_cast<QCacheView *>(parent_)->updateSize();
    }
}

QPoint AScrollViewItem::bottomRight()
{
    return rect_.bottomRight();
}

int AScrollViewItem::right()
{
    return rect_.right();
}

int AScrollViewItem::bottom()
{
    return rect_.bottom();
}

void AScrollViewItem::redraw()
{
    (static_cast<QCacheView *>(parent_))->repaintWorldCoordinates(X(),
                                                  Y(),
                                                  width(),
                                                  height(),
                                                  false);
}

void AScrollViewItem::doubleClickEvent(int x, int y)
{        
    redraw();
}

void AScrollViewItem::drawItemCoordinatesContents(QPainter* p, int cx, int cy, int cw, int ch)
{
}


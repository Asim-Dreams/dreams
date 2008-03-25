/*****************************************************************************
*
* @brief The scroll view used in qcache.
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

#ifndef QCACHEVIEW_H
#define QCACHEVIEW_H
/** \file */

#include "dbase.h"
#include <AScrollView.h>
#include <vector>
#include <qdom.h>

class QPainter;
class QCacheNode;
class QCacheViewItem;
class PaintEvent;
class QCacheVisualization;

using qcdb::ItemDefinition;

class QCacheView : public AScrollView
{
  Q_OBJECT
  public:
    /** Default constructor */
    QCacheView(QWidget *parent, int width, int height);
    /** Destructor */
    ~QCacheView();
    virtual void
    drawContents (
                 QPainter * p,      ///< Painter where to draw on
                 int clipx,         ///< left X coordinate (QScrollView coordinates system)
                 int clipy,         ///< upper Y coordinate (QScrollView coordinates system)
                 int clipw,         ///< redrawing area width (QScrollView coordinates system)
                 int cliph          ///< redrawing area height (QScrollView coordinates system)
                 );
    void contentsSelectEvent(int col, int row, bool adding);
    void drawWorldCoordinatesContents(QPainter *p, int cx, int cy, int cw, int ch);
    double getBaseElementSize();
    bool hasContentsOn (int col, int row);

    void mouseWorldMoveEvent(QMouseEvent* e, double x, double y);
    void leftClickPressEvent(QMouseEvent* e);
    void rightClickPressEvent(QMouseEvent* e); 
    void rightClickPressEvent(QMouseEvent* e, double x, double y);
    void rightClickReleaseEvent(QMouseEvent* e);
    void mouseWorldDoubleClickEvent (QMouseEvent* e, double x, double y);
    virtual void 
    mouseWorldPressEvent_Panning (QMouseEvent *e, double x, double y);
    virtual void 
    mouseWorldReleaseEvent_Panning (QMouseEvent *e, double x, double y);
    void childAdded(QCacheViewItem* item);
    void childDeleted(QCacheViewItem* item);

    void repaintWorldCoordinates(int x, int y, int w, int h, bool erase);
    void repaintWorldCoordinates(const QRect& rect, bool erase = false);

    void Cycle(
        UINT64 cycle);

    void updateSize();

    void centerNode(UINT16 node_id);

  private:
    typedef vector<QCacheViewItem*> Items;
    typedef Items::iterator ChildrenIterator;
    typedef Items::const_iterator ChildrenConstIterator;

    Items children_;

    QCacheViewItem* selectedChild_;

    void updateCycle(
                       UINT64 x  /// New cycle  
                       );

};

#endif

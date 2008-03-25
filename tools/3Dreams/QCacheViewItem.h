/*****************************************************************************
*
* @brief Item inside a qcache view with a bounding box. Header File.
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

#ifndef QCacheViewItem_H
#define QCacheViewItem_H

#include <qpainter.h>
#include <qobject.h>
#include <qrect.h>
#include <qdom.h>

class QCacheView;

/** 
    An item inside the QCacheView. Keeps track of its position and dimensions (rectangle).
*/
class QCacheViewItem : public QObject
{
  Q_OBJECT
  public:
    /** Constructor */
    QCacheViewItem(QCacheView *parent = 0, int width = 0, int height = 0);
    /** Destructor */
    ~QCacheViewItem();

    /** Changes the view */
    void setScrollView(QCacheView *parent);
    /** Realizes the necessary drawing operations that must be done to paint the object in the received window*/
    void
    drawWorldCoordinatesContents (
                                 QPainter* p,     ///< Painter where to draw on
                                 int cx,          ///< left X coordinate (World Coordinates System)
                                 int cy,          ///< upper Y coordinate (World Coordinates System)
                                 int cw,          ///< redrawing area width (World Coordinates System)
                                 int ch           ///< redrawing are height (World Coordinates System)
                                 );
    /** Moves the object to a new position*/
    virtual void 
    moveTo (  
            int newx,   ///< New x position   
            int newy    ///< New y position
            );

    /** Behaves as the one above. Just provided for convenience */
    virtual void 
    moveTo (
            const QPoint& to   ///< Destination
            );


    virtual void 
    moveBy (  
            int disx,   ///< X displacement
            int disy    ///< Y displacement
            );

    /** Returns the left X coordinate of the item (World Coordinates System)*/
    int X() const;
    /** Returns the upper Y coordinate of the item (World Coordinates System)*/
    int Y() const;
    /** Returns the height of the item (World Coordinates System)*/
    int height() const;
    /** Returns the width of the item (World Coordinates System)*/
    int width() const;
    /** Returns a pointer to parent*/
    QCacheView* scrollView();
    /** Check if a given point is contained inside the item
        @returns True if the point is contained in the item
    */
    bool contains(
                    double x,   ///< X coordinate of the point 
                    double y    ///< Y coordinate of the point
                    ) const;
    /** Check if a given QCacheViewItem intersects with this
        @returns True if there is an intersection
    */
    bool intersects(
                    const QCacheViewItem & other   ///< Another item to check the intersection
                    ) const;
    /** Check if a given QRect intersects with this 
        @returns True if there is an intersection
    */
    bool intersects(
                    const QRect & other     ///< A QRect object to check the intersection
                    ) const;
    /** Returns a QRect object that represents the position and dimensions of the item*/
    const QRect & rect() const;
    /** Changes the height of the object. The origin remains unchanged. */
    void setHeight(const int height);
    /** Changes the width of the object. The origin remains unchanged */
    void setWidth(const int width);
    /** Returns the bottom right point of the rectangle. */
    QPoint bottomRight();
    int right();
    int bottom();

    void redraw();

    void hide();
    void unhide();
    bool hidden();
    
    void select();
    void unselect();
    
    virtual void doubleClickEvent(int x, int y);
    virtual void leftClickEvent(int x, int y);
    virtual void rightClickEvent(int x, int y);

    virtual QString label() const;

    bool readConfiguration(const QDomElement& elem);

  public slots:
    virtual void configure() = 0;
    
  protected:
    void redrawItemCoordinatesContents(int cx, int cy, int cw, int ch);

    virtual void
    drawItemCoordinatesContents (
                                    QPainter* p,     ///< Painter where to draw on
                                    int cx,          ///< left X coordinate (Item Coordinates System)
                                    int cy,          ///< upper Y coordinate (Item Coordinates System)
                                    int cw,          ///< redrawing area width (Item Coordinates System)
                                    int ch           ///< redrawing are height (Iten Coordinates System)
                                );

    /** Pointer to the objects parent: this should be an QCacheViewItem */
    QCacheView* parent_; 
    /** Pointer to a rectangle object that represents the position and dimensions of the item */
    QRect bbox_;
    
    /** Indicates if the item is currently selected */
    bool selected_;
    /** Indicates if the item is hidden or not */
    bool hidden_;

  private:
    static const char* SELECTED_COLOR;
};

#endif

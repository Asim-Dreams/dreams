/*
 * Copyright (C) 2005-2006 Intel Corporation
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

/*
 * @file ItemTabWidgetColumn.cpp
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "ItemTabWidgetColumn.h"
#include "ItemTabWidget.h"
#include "PreferenceMgr.h"
#include "DreamsDB.h"

/*
 * Creator of the class.
 *
 * @return the new object.
 */
ItemTabWidgetColumn::ItemTabWidgetColumn(DreamsDB * _dreamsdb, QValueVector<INT32> * _ilist, TAG_ID _tagId, UINT32 _cube_side, QTable * table, EditType et)
    : QTableItem(table, et)
{
    // Parameter copy.
    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();
    ilist = _ilist;
    tagId = _tagId;
    cube_side = _cube_side;

    // Parameter init.
    scfy = 1.0;
    background = PreferenceMgr::getInstance()->getBackgroundColor();
    textcolor = QColor(0, 0, 0);
}

/*
 * Destructor of the class.
 *
 * @return void.
 */
ItemTabWidgetColumn::~ItemTabWidgetColumn()
{
}

/*
 * Painting of the widget.
 *
 * @return void.
 */
void
ItemTabWidgetColumn::paint(QPainter * p, const QColorGroup & cg, const QRect & cr, bool selected)
{
    ItemTabWidget * tab = dynamic_cast<ItemTabWidget *>(table());
    Q_ASSERT(tab);

    //printf("Tab contentsY is: %i\n", tab->contentsY());

    // First gets the starting row.
    UINT32 frow = (UINT32) floor((double) tab->contentsY() / ((double) cube_side * scfy));
    UINT32 lrow = (UINT32) ceil((double) tab->frameSize().height() / ((double) cube_side * scfy));
    lrow += frow;
    lrow = QMIN(lrow, ilist->count());
    //printf("Rect: (%i, %i, %i, %i)\n", cr.left(), cr.top(), cr.width(), cr.height());
    //printf("Rows from: %i to %i\n", frow, lrow);

    for(UINT32 row = frow; row < lrow; row++)
    {
        paintCell(p, row, cr.width(), cg, (row == (UINT32) tab->getSelectedRow()));
    }
}

/*
 * Paints a cell.
 *
 * @return void.
 */
void
ItemTabWidgetColumn::paintCell(QPainter * p, INT32 row, INT32 width, const QColorGroup & cg, bool selected)
{
    bool ok;
    ITEM_ID itemid = ilist->at(row, &ok);
    Q_ASSERT(ok);

    INT32 initPixel = (INT32) floor((double) (row * cube_side) * scfy);
    INT32 lastPixel = (INT32) floor((double) ((row + 1) * cube_side) * scfy);
    INT32 bfsize = lastPixel - initPixel;

    // Case of separation lines.
    if(itemid == ITEM_ID_INVALID)
    {
        p->fillRect(0, initPixel, width, bfsize, QBrush(QColor(255, 255, 255)));
        return;
    }

    ItemHandler hnd;
    ok = draldb->lookForItemId(&hnd, itemid);
    Q_ASSERT(ok);

    // Draw the cell.
    QFont fnt = QFont("Courier");
    fnt.setPixelSize(QMAX(4, QMIN(bfsize - 3, 64)));
    p->setFont(fnt);

    if(selected)
    {
        p->fillRect(0, initPixel, width, bfsize, QBrush(cg.highlight()));
    }
    else
    {
        p->fillRect(0, initPixel, width, bfsize, QBrush(background));
    }

    if(!hnd.hasTag(tagId))
    {
        p->setPen(QPen(QColor(255, 0, 0), 1));
        p->drawText(1, initPixel + 1, width - 5, bfsize - 2, Qt::AlignVCenter || Qt::AlignLeft, "Undefined");
    }
    else
    {
        p->setPen(QPen(textcolor, 1));
        QString strvalue = hnd.getTagFormatedValue();
        p->drawText(1, initPixel + 1, width - 5, bfsize - 2, Qt::AlignVCenter || Qt::AlignLeft, strvalue);
    }
}

/*
 * Gets the height for the requested row.
 *
 * @return the row height.
 */
INT32
ItemTabWidgetColumn::getHeightForRow(INT32 row) const
{
    double height = static_cast<double>(cube_side) * scfy;
    int baseH = static_cast<int>(height);
    int result = baseH;
    double rem = height-static_cast<double>(baseH);
    //cerr << "ItemTabWidget::getHeightForRow called for row " << row <<  endl;
    //cerr << "\t height " << height << " baseH " << baseH << " rem " << rem <<  endl;
    if (rem!=0.0)
    {
        double a = rem*static_cast<double>(row+1);
        double b = rem*static_cast<double>(row);
        int ca = static_cast<int>(a+0.00000001);
        int cb = static_cast<int>(b+0.00000001);
        //cerr << "\t a " << a << " b " << b  <<  endl;
        //cerr << "\t ca " << ca << " cb " << cb  <<  endl;
        if ((ca-cb)>=1) ++result;
    }
    //cerr << "\t result " << result <<  endl;
    return result;
}

/*
 * Updates the widget with the new zoom factor.
 *
 * @return void.
 */
void
ItemTabWidgetColumn::updateVerticalZoom(double _scfy)
{
    scfy = _scfy;
}

/*
 * Returns the size of the widget.
 *
 * @return the widget's size.
 */
QSize
ItemTabWidgetColumn::sizeHint() const
{
    QSize ret;

    ret.setWidth(myWidth);
    ret.setHeight((INT32) (ilist->count() * cube_side * scfy));
    return ret;
}

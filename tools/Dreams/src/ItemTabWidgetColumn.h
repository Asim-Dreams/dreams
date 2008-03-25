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

/*
 * @file ItemTabWidgetColumn.h
 */

#ifndef _ITEMTABWIDGETTABLE_H
#define _ITEMTABWIDGETTABLE_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"

// Qt includes.
#include <qtable.h>
#include <qvaluevector.h>

// Class forwarding.
class DreamsDB;
class DralDB;

/*
 * @brief
 * QGridView derivation to accomodate the item tabs
 *
 * @description
 * ...
 *
 * @version 0.1
 * @date started at 2005-03-23
 * @author Guillem Sole
 */
class QM_EXPORT_TABLE ItemTabWidgetColumn : public QTableItem
{
    public:
        ItemTabWidgetColumn(DreamsDB * _dreamsdb, QValueVector<INT32> * _ilist, TAG_ID _tagId, UINT32 _cube_side, QTable * table, EditType et);
        ~ItemTabWidgetColumn();

        void paint(QPainter * p, const QColorGroup & cg, const QRect & cr, bool selected);
        void updateVerticalZoom(double _scfy);
        void updateWidth(INT32 width);
        INT32 getHeightForRow(INT32 row) const;
        QSize sizeHint() const;

    protected:
        void paintCell(QPainter * p, INT32 row, INT32 width, const QColorGroup & cg, bool selected);

    private:
        DreamsDB * dreamsdb;         ///< Trace.
        DralDB * draldb;             ///< Database of the trace.
        QValueVector<INT32> * ilist; ///< List of the itemids in the waterfall tab.
        TAG_ID tagId;                ///< Tag that the column must paint.
        UINT32 cube_side;            ///< Base size of the row height.
        UINT32 myWidth;              ///< The width.
        QColor background;           ///< Background color.
        QColor textcolor;            ///< Text color.
        double scfy;                 ///< Scale factor y.
} ;

#endif // _ITEMTABWIDGETCOLUMN_H

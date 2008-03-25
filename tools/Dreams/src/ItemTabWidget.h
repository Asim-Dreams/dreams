// ==================================================
// Copyright (C) 2004-2006 Intel Corporation
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
  * @file ItemDockWin.h
  */

#ifndef _ITEMTABWIDGET_H
#define _ITEMTABWIDGET_H

// Dreams includes.
#include "TabSpec.h"
#include "ItemTab.h"
#include "DreamsDefs.h"
#include "ItemTabWidgetColumn.h"

// Qt includes.
#include <qdockwindow.h>
#include <qscrollview.h>
#include <qvaluelist.h>
#include <qvbox.h>
#include <qtabwidget.h>
#include <qvaluevector.h>
#include <qlabel.h>
#include <qtable.h>
#include <qapplication.h>
#include <qprogressdialog.h>
#include <qstringlist.h>

// Class forwarding.
class DralDB;
class DreamsDB;

/**
  * QTable derivation to accomodate the item tabs
  *
  * @version 0.1
  * @date started at 2004-11-03
  * @author Federico Ardanaz
  */
class ItemTabWidget : public QTable
{
    Q_OBJECT
        
    public:
        ItemTabWidget (DreamsDB * _dreamsdb, ItemTab* itab, int cside, QWidget* parent = 0, const char* name = 0);
        ~ItemTabWidget ();

        void init();
        inline ItemTab* getItemTab() { return myItemTab; }
        void putVerticalZoom(double dy);
        void synchWithASVTop(double);
        void setSelectedRow(INT32 row);

        // QTable redefined method
        inline double getVerticalZoom() const;
        inline INT32 rowHeight(INT32 row) const;
        inline INT32 getSelectedRow() const;

    protected:
        void updateVerticalZoom();

    private:
        DreamsDB * dreamsdb;
        DralDB * draldb;
        ItemTab * myItemTab;
        QValueVector<INT32> * ilist;   
        QStringList * strlist;
        INT32 cube_side;
        double myCurrentVerticalZoom;
        INT32 selectedRow;
};

double
ItemTabWidget::getVerticalZoom() const
{
    return myCurrentVerticalZoom;
}

INT32
ItemTabWidget::rowHeight(INT32 row) const
{
    ItemTabWidgetColumn * col = dynamic_cast<ItemTabWidgetColumn *>(item(0, 0));
    if(col != NULL)
    {
        return col->sizeHint().height();
    }
    else
    {
        return 1;
    }
}

INT32
ItemTabWidget::getSelectedRow() const
{
    return selectedRow;
}

#endif

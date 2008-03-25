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
  * @file ItemWFCombo.h
  */

#ifndef _ITEMWFCOMBO_H
#define _ITEMWFCOMBO_H

#include "DreamsDefs.h"

// Qt library
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


// Dreams
#include "TabSpec.h"
#include "ItemTab.h"
#include "dDB/DralDB.h"
#include "ItemTabWidget.h"

#include <qsplitter.h>

class WaterfallDAvtView;
class DAvtView;
class DreamsDB;
class DralDB;

/**
  * Splitter widget to aid on the WF + ItemTabWidget manipulation 
  *
  * @version 0.1
  * @date started at 2004-11-03
  * @author Federico Ardanaz
  */
class ItemWFCombo : public QSplitter
{
    Q_OBJECT
    public:
        ItemWFCombo(DreamsDB * _dreamsdb, QMainWindow *main, QMainWindow* mdi, ItemTab* itab);
        ~ItemWFCombo();

        void reset();
        void alignWith(INT32 col, INT32 row, CYCLE cycle);

        //friend class DisView;
        DAvtView* getDAvtView();
        inline ItemTabWidget* getItemTabWidget() { return myItemTabWidget; }

        void recomputeSize();
        void applyJoystickMovement(double x, double y);

    public slots:
        void currentChanged ( int row, int col ); 
        void contentsMoving ( int x, int y );
        //void asvMoved (double x, double y); 
        void newScallingFactors(double,double);
        void selectionChanged ();
        void avtContentSelected(int,int,bool,bool);
        //void asvCentered(double,double); 
        void asvNewTLCorner(double,double); 

    protected:
        //void scanSplitBy(TabSpec* tab);
        void alignWith(ItemHandler &hnd);

        QValueList<int> splitSizes;

        DreamsDB*        dreamsdb;
        DralDB*          draldb;

        WaterfallDAvtView*    myAvt;
        ItemTabWidget*   myItemTabWidget;
        
        QMainWindow*     myMW;
        QMainWindow*     myMDI;

        int verticalOffset;   
        bool tableScrollFromAvt;
        bool avtScrollFromTable;
};

#endif

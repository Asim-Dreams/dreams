/*
 * Copyright (C) 2004-2006 Intel Corporation
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

/**
  * @file ItemWFCombo.cpp
  */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "avt/WaterfallDAvtView.h"
#include "ItemWFCombo.h"
#include "DreamsDB.h"

ItemWFCombo::ItemWFCombo(DreamsDB * _dreamsdb, QMainWindow *main, QMainWindow* mdi, ItemTab* itab)
{
    //cerr << "ItemWFCombo constructor called" << endl;

    //setFocusPolicy(ClickFocus);

    dreamsdb = _dreamsdb;
    myMW = main;
    myMDI = mdi;

    myItemTabWidget = new ItemTabWidget(dreamsdb, itab, CUBE_SIDE_BIG, this);
    myAvt = new WaterfallDAvtView(dreamsdb, this, mdi, main);
    
    connect(myItemTabWidget,SIGNAL(currentChanged(int,int)),this,SLOT(currentChanged(int,int)));
    connect(myItemTabWidget,SIGNAL(contentsMoving(int,int)),this,SLOT(contentsMoving(int,int)));
    connect(myItemTabWidget,SIGNAL(selectionChanged()),this,SLOT(selectionChanged()));
    //connect(myAvt,SIGNAL(asvMoved(double,double)),this,SLOT(asvMoved(double,double)));
    //connect(myAvt,SIGNAL(asvZoomed(double,double)),this,SLOT(asvZoomed(double,double))) ;
    connect(myAvt,SIGNAL(asvNewScallingFactors(double,double)),this,SLOT(newScallingFactors(double,double))) ;
    connect(myAvt,SIGNAL(contentSelected(int,int,bool,bool)),this,SLOT(avtContentSelected(int,int,bool,bool))) ;
    connect(myAvt,SIGNAL(asvNewTLCorner(double,double)),this,SLOT(asvNewTLCorner(double,double))) ;

    connect(myAvt, SIGNAL(syncToCycle(CYCLE)), main, SLOT(do_syncToCycle(CYCLE)));
    connect(myAvt, SIGNAL(syncToCycleTrace(CYCLE)), main, SLOT(do_syncToCycleTrace(CYCLE)));

    // compute offset produced by header
    QSize sz = myItemTabWidget->horizontalHeader()->size();
    verticalOffset= sz.height();
    myAvt->addVerticalHeaderOffset(verticalOffset);

    /*
    progress = new QProgressDialog ("Creating Tabs...", "&Cancel",1, mw,"lbl_progress", TRUE);
    Q_ASSERT(progress!=NULL);
    progress->setMinimumDuration(250);
    */

    //max_width=0;
    //tabs.clear();

    tableScrollFromAvt = false;
    avtScrollFromTable = false;
    myAvt->setSelectedRow(0);
   
    setResizeMode(myItemTabWidget,QSplitter::Stretch);
    
    // adjust splitter initial sizes
    QValueList<int> szl;
    double wd = static_cast<double>(width());
    szl.append(static_cast<int>((1.0/3.0)*wd));
    szl.append(static_cast<int>((2.0/3.0)*wd));
    setSizes(szl);

    //cerr << "ItemWFCombo constructor finished" << endl;
}

ItemWFCombo::~ItemWFCombo()
{
    delete myItemTabWidget;
    delete myAvt;
}

void
ItemWFCombo::reset()
{
}

void
ItemWFCombo::recomputeSize()
{
    myAvt->setRealContentsSize((INT32) dreamsdb->layoutWaterfallGetWidth(), (INT32) dreamsdb->layoutWaterfallGetHeight() + verticalOffset);
}

/**
 * Align the first found tab with the handler given as parameter
 *
 * @param hnd Handler to synchronize to
 */
void
ItemWFCombo::alignWith(ItemHandler &hnd)
{
}

void 
ItemWFCombo::alignWith(INT32 col, INT32 row, CYCLE cycle)
{
}

void 
ItemWFCombo::currentChanged ( int row, int col )
{
    //cerr << "ItemWFCombo::currentChanged called on row " << row << " col " << col << endl;
    //myItemTabWidget->selectRow(row);
    Q_ASSERT(myAvt);
    myAvt->setSelectedRow(row);
}
    
void 
ItemWFCombo::contentsMoving ( int x, int y )
{
    avtScrollFromTable = false;
    if (!tableScrollFromAvt)
    {
        Q_ASSERT(myAvt);
        int offset = y-myItemTabWidget->contentsY();
        if (offset!=0)
        {
            //cerr << "ItemWFCombo::contentsMoving called on x " << x << " y " << y << " dy " << offset << endl;
            avtScrollFromTable = true;
            myAvt->scrollByMatrix(0,offset); 
        }
        else
        {
            //cerr << "ItemWFCombo::contentsMoving called on x " << x << " y " << y << " null y offset..."<< endl;
        }
    }
    else
    {
        //cerr << "ItemWFCombo::contentsMoving ingonring contentsMoving due to tableScrollFromAvt" << endl;
    }
    tableScrollFromAvt=false;
}
        
DAvtView* 
ItemWFCombo::getDAvtView() 
{ return (DAvtView*) myAvt; }

/*
void
ItemWFCombo::asvMoved(double dx, double dy)
{
    Q_ASSERT(myItemTabWidget);
    cerr << "ItemWFCombo::asvMoved called dx " << dx << " dy " << dy << endl;
    tableScrollFromAvt=true;
    myItemTabWidget->scrollBy(0,(int)dy);
}
*/

void 
ItemWFCombo::newScallingFactors(double nscfx,double nscfy)
{
    Q_ASSERT(myItemTabWidget);
    double oscfy = myItemTabWidget->getVerticalZoom();

    //cerr << "ItemWFCombo::newScallingFactors called nscfy " << nscfy  << endl;
    myItemTabWidget->putVerticalZoom(nscfy);
    QValueList<int> szl;
    szl.append(0);
    szl.append(width());
    if ((oscfy>=0.5) && (nscfy<0.50) )
    {
        splitSizes=sizes();
        setSizes(szl);
    }
    if ((nscfy>=0.5) && (oscfy<0.5) )
    {
        // restore table
        setSizes(splitSizes);
    }
}

void 
ItemWFCombo::selectionChanged()
{
}

void 
ItemWFCombo::avtContentSelected(int col ,int row ,bool adding ,bool shifted)
{
    //cerr << "ItemWFCombo::avtContentSelected detected on row " << row << endl;
    Q_ASSERT(myItemTabWidget);
    myItemTabWidget->setSelectedRow(row);
}

/*
void 
ItemWFCombo::asvCentered(double tlx, double tly)
{
    cerr << "ItemWFCombo::asvCentered tlx" << tlx << " tly " << tly << endl;
    tableScrollFromAvt=true;
    int row = (int)tly;
    // ensure this row is the first in the table
    myItemTabWidget->synchWithASVTop(tly);
}
*/

void 
ItemWFCombo::asvNewTLCorner(double x,double y)
{
    tableScrollFromAvt=false;
    if (!avtScrollFromTable)
    {
        //cerr << "ItemWFCombo::asvNewTLCorner x " << x << " y " << y << endl;
        tableScrollFromAvt=true;
        //myItemTabWidget->setContentsPos(myItemTabWidget->contentsX(),(int)y);
        myItemTabWidget->synchWithASVTop(y);
    }
    else
    {
        //cerr << "ItemWFCombo::asvNewTLCorner event ignored'cause avtScrollFromTable" << endl;
    }
    avtScrollFromTable = false;
}

        
void 
ItemWFCombo::applyJoystickMovement(double x, double y)
{
    double cs = myAvt->getBaseElementSize();
    myAvt->scrollBy(static_cast<int>(x*cs*5.0),static_cast<int>(-y*cs*5.0));
}



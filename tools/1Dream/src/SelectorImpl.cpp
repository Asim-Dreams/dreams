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

#include "SelectorImpl.h"

/* 
 *  Constructs a SelectorImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SelectorImpl::SelectorImpl( QWidget* parent, 
                            const char* name, 
                            bool modal, 
                            WFlags fl, 
                            allFilesData *DB, 
                            StripChartView *asv, 
                            ColorsImpl *vpal )
    : Selector( parent, name, modal, fl )
{
    QPixmap qp(fopenIcon);

    tablePen[0]=Qt::SolidLine;
    tablePen[1]=Qt::DashLine;
    tablePen[2]=Qt::DotLine;
    tablePen[3]=Qt::DashDotLine;
    tablePen[4]=Qt::DashDotDotLine;

     myDB=DB;   
    visiblePalette=vpal;
    view=asv;

    // Adding all the elems to the Widget
    for(INT32 i=0;i<MAX_OPENED_FILES;i++) 
    {
        if(myDB->active[i]==1) 
        {
            FilesOpened->insertItem(qp,*myDB->files[i].fileName);
        }
    }
}

void SelectorImpl::do_ThreadClicked(QListBoxItem *item)
{
    int style;
    int index;
    int h,s,v;
    QColor cl;
    QPixmap qp(ok);

    index=ThreadsOnStrip->index(item);

    threadSelected=index;

    if(   (myDB->files[fileSelected].fullData[stripSelected].visible[threadSelected]==1)
        ||(myDB->files[fileSelected].fullData[stripSelected].activated[threadSelected]==1)) 
    {
        myDB->files[fileSelected].fullData[stripSelected].visible[threadSelected]=0;
    } 
    else 
    {
        myDB->files[fileSelected].fullData[stripSelected].visible[threadSelected]=1;
        StripsAvailable->changeItem(qp,StripsAvailable->text(StripsAvailable->currentItem()),StripsAvailable->currentItem());
    }

    updateStripList();
    updateThreadList();

    StripsAvailable->setCurrentItem(stripSelected);
    StripsAvailable->centerCurrentItem();
    ThreadsOnStrip->setCurrentItem(threadSelected);
    ThreadsOnStrip->centerCurrentItem();
}

void SelectorImpl::connectRoutines()
{
    // FilesOpened
    connect(FilesOpened,SIGNAL(currentChanged(QListBoxItem *)),
            this,SLOT(do_IconClicked(QListBoxItem *)));
    connect(FilesOpened,SIGNAL(clicked(QListBoxItem *)),
            this,SLOT(do_IconClicked(QListBoxItem *)));
    connect(FilesOpened,SIGNAL(doubleClicked(QListBoxItem *)),
            this,SLOT(do_IconClicked(QListBoxItem *)));

    // StripsAvailable
    connect(StripsAvailable,SIGNAL(selected(int)),
            this,SLOT(do_StripClicked(int)));
    connect(StripsAvailable,SIGNAL(highlighted(int)),
            this,SLOT(do_StripClicked(int)));

    // ThreadsOnStrip
    connect(ThreadsOnStrip,SIGNAL(doubleClicked(QListBoxItem *)),
            this,SLOT(do_ThreadClicked(QListBoxItem *)));

    // OK
    connect(buttonOk,SIGNAL(pressed()),
            this,SLOT(do_OKClicked()));
    connect(buttonOk,SIGNAL(clicked()),
            this,SLOT(do_OKClicked()));
}

void SelectorImpl::updateStripList()
{
    QPixmap qp(ok);

    // Empty the list.
    StripsAvailable->clear();

    for(INT32 i=0;i<myDB->files[fileSelected].elems;i++) 
    {
        int SomeSelected=0;

        for(INT32 j=0;j<myDB->files[fileSelected].fullData[i].threads;j++) 
        {
            if( (myDB->files[fileSelected].fullData[i].visible[j]==1)
                ||(myDB->files[fileSelected].fullData[i].activated[j]==1)) 
            {
                SomeSelected=1;
            }
        }
        if(SomeSelected==1)
        {
            StripsAvailable->insertItem(qp,QString(myDB->files[fileSelected].fullData[i].description));
        }
        else
        {
            StripsAvailable->insertItem(QString(myDB->files[fileSelected].fullData[i].description));
        }
    }
}

void SelectorImpl::updateThreadList()
{
    char str[100];
    QPixmap qp(ok);

    // Empty the list.
    ThreadsOnStrip->clear();

    // Add all the threads.
    if(myDB->files[fileSelected].fullData[stripSelected].threads==1) 
    {
        if(myDB->files[fileSelected].fullData[stripSelected].visible[0]==1) 
        {
            ThreadsOnStrip->insertItem(qp,QString("Accumulated\0"));
        } 
        else 
        {
            ThreadsOnStrip->insertItem(QString("Accumulated\0"));
        }
    } 
    else 
    {
        for(INT32 i=0;i<myDB->files[fileSelected].fullData[stripSelected].threads-1;i++) 
        {
            if(myDB->files[fileSelected].fullData[stripSelected].visible[i]==1) 
            {
                ThreadsOnStrip->insertItem(qp,QString("Thread ")+QString::number(i));
            } 
            else 
            {
                ThreadsOnStrip->insertItem(QString("Thread ")+QString::number(i));
            }    
        }
        if(myDB->files[fileSelected].fullData[stripSelected].visible[myDB->files[fileSelected].fullData[stripSelected].threads-1]==1)
        {
            ThreadsOnStrip->insertItem(qp,QString("Accumulated\0"));
        } 
        else 
        {
            ThreadsOnStrip->insertItem(QString("Accumulated\0"));
        }
    }
}

void SelectorImpl::do_OKClicked()
{
    float max;
    int max_int;
    int rem;

    visiblePalette->UpdateColors();
    max=MaxOnVisibleStrips(myDB);

    max_int=(int)max;
    max_int=(int)(max+5.0);
    rem=max_int%5;
    rem=5-rem;
    if(rem!=0)
    {
        max_int+=rem;
    }    

    if(max_int<view->visibleHeight())
    {
        max_int=view->visibleHeight();
    }

    view->setRealContentsSize(MaxLinesOnVisibleStrips(myDB),max_int);
    view->scaleToFitInitial();
    view->updateContents();
}

void SelectorImpl::do_StripClicked(int index)
{
    stripSelected=index;

    updateThreadList();
}

void SelectorImpl::do_IconClicked(QListBoxItem *item)
{
    int index;

    index=FilesOpened->index(item);

    ThreadsOnStrip->clear();

    // Take the file of item and fill with the stripcharts available

    fileSelected=index;

    updateStripList();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
SelectorImpl::~SelectorImpl()
{
    // no need to delete child widgets, Qt does it all for us
}


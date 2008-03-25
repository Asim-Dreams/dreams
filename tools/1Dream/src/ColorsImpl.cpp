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

#include "ColorsImpl.h"

/* 
 *  Constructs a ColorsImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ColorsImpl::ColorsImpl( QWidget* parent,  
                        const char* name, 
                        bool modal, 
                        WFlags fl, 
                        allFilesData *DB)
    : Colors( parent, name, modal, fl )
{
    myDB=DB;
    connect(listColors,SIGNAL(selected(int)),
            this,SLOT(do_StripClicked(int)));
            
    accelSupr=new QAccel(listColors);
    accelIns=new QAccel(listColors);
    accelPlus=new QAccel(listColors);
    accelMinus=new QAccel(listColors);
    
    accelSupr->connectItem(accelSupr->insertItem(Key_Delete),
                           this,SLOT(removeElem()));
    accelIns->connectItem(accelIns->insertItem(Key_Insert),
                          this,SLOT(insertElem()));
    accelPlus->connectItem(accelPlus->insertItem(Key_Plus),
                           this,SLOT(plusSmoothElem()));
    accelMinus->connectItem(accelMinus->insertItem(Key_Minus),
                            this,SLOT(minusSmoothElem()));
    
    connect(Down,SIGNAL(clicked()),
            this,SLOT(removeElem()));
    connect(Up,SIGNAL(clicked()),
            this,SLOT(insertElem()));
}

void ColorsImpl::Hide()
{
    Up->setDisabled(TRUE);
    Down->setDisabled(FALSE);
}

void ColorsImpl::Unhide()
{
    Up->setDisabled(FALSE);
    Down->setDisabled(TRUE);
}

void ColorsImpl::insertElem()
{
    int pos=listColorsActived->currentItem()+1;

    // We know the position in visible order of the strip. We search in the structure where is
    // the stripchart associated


    for(INT32 i=0;i<MAX_OPENED_FILES;i++) 
    {
        if(myDB->active[i]==1) 
        {
            for(INT32 j=0;j<myDB->files[i].elems;j++) 
            {
                for(INT32 k=0;k<myDB->files[i].fullData[j].threads;k++) 
                {
                    if(myDB->files[i].fullData[j].activated[k]==1) 
                    {
                        pos--;
                        if(pos==0) 
                        {
                            myDB->files[i].fullData[j].visible[k]=1;
                            myDB->files[i].fullData[j].activated[k]=0;
                            UpdateColors();
                            view->updateContents();
                            return;
                        }
                    }
                }
            }
        }
    }
}

void ColorsImpl::removeElem()
{
    int pos=listColors->currentItem()+1;

    // We know the position in visible order of the strip. We search in the structure where is
    // the stripchart associated


    for(INT32 i=0;i<MAX_OPENED_FILES;i++) 
    {
        if(myDB->active[i]==1) 
        {
            for(INT32 j=0;j<myDB->files[i].elems;j++) 
            {
                for(INT32 k=0;k<myDB->files[i].fullData[j].threads;k++) 
                {
                    if(myDB->files[i].fullData[j].visible[k]==1) 
                    {
                        pos--;
                        if(pos==0) 
                        {
                            myDB->files[i].fullData[j].visible[k]=0;
                            myDB->files[i].fullData[j].activated[k]=1;
                            UpdateColors();
                            view->updateContents();
                            return;
                        }
                    }
                }
            }
        }
    }
}

void ColorsImpl::plusSmoothElem()
{
    int pos=listColors->currentItem()+1;

    // We know the position in visible order of the strip. We search in the structure where is
    // the stripchart associated


    for(INT32 i=0;i<MAX_OPENED_FILES;i++) 
    {
        if(myDB->active[i]==1) 
        {
            for(INT32 j=0;j<myDB->files[i].elems;j++) 
            {
                for(INT32 k=0;k<myDB->files[i].fullData[j].threads;k++) 
                {
                    if(myDB->files[i].fullData[j].visible[k]==1) 
                    {
                        pos--;
                        if(pos==0) 
                        {
                            myDB->files[i].fullData[j].smoothView[k]=1;
                            if(myDB->files[i].fullData[j].smooth[k]==0) 
                            {
                                myDB->files[i].fullData[j].smooth[k]=1;
                            } 
                            else 
                            {
                                myDB->files[i].fullData[j].smooth[k]*=2;
                            }    
                            view->updateContents();
                            return;
                        }
                    }
                }
            }
        }
    }
}

void ColorsImpl::minusSmoothElem()
{
    int pos=listColors->currentItem()+1;

    // We know the position in visible order of the strip. We search in the structure where is
    // the stripchart associated


    for(INT32 i=0;i<MAX_OPENED_FILES;i++) 
    {
        if(myDB->active[i]==1) 
        {
            for(INT32 j=0;j<myDB->files[i].elems;j++) 
            {
                for(INT32 k=0;k<myDB->files[i].fullData[j].threads;k++) 
                {
                    if(myDB->files[i].fullData[j].visible[k]==1) 
                    {
                        pos--;
                        if(pos==0) 
                        {
                            if(myDB->files[i].fullData[j].smooth[k]>1) 
                            {
                                myDB->files[i].fullData[j].smooth[k]/=2;
                                view->updateContents();
                            } 
                            else 
                            {
                                myDB->files[i].fullData[j].smoothView[k]=0;
                                myDB->files[i].fullData[j].smooth[k]=0;
                                view->updateContents();
                            }
                            return;
                        }
                    }
                }
            }
        }
    }
}

void ColorsImpl::setView(StripChartView *vw)
{
    view=vw;
}

void ColorsImpl::do_StripClicked(int index)
{
    int pos=index+1;

    // We know the position in visible order of the strip. We search in the structure where is
    // the stripchart associated


    for(INT32 i=0;i<MAX_OPENED_FILES;i++) 
    {
        if(myDB->active[i]==1) 
        {
            for(INT32 j=0;j<myDB->files[i].elems;j++) 
            {
                for(INT32 k=0;k<myDB->files[i].fullData[j].threads;k++) 
                {
                    if(myDB->files[i].fullData[j].visible[k]==1) 
                    {
                        pos--;
                        if(pos==0) 
                        {
                            LittleSelectorImpl *Sel=new LittleSelectorImpl(this,"StripChart",TRUE,0,myDB,i,j,k,this,view);
                            Sel->fillData();    
                            Sel->connectRoutines();
                            Sel->show();

                            return;
                        }
                    }
                }
            }
        }
    }

}

/*  
 *  Destroys the object and frees any allocated resources
 */
ColorsImpl::~ColorsImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void ColorsImpl::UpdateColors()
{

    QPixmap *qp;
    char strName[200];

    qp=new QPixmap(block);

    // Update paletteContents
    listColors->clear();
    listColorsActived->clear();

    for(INT32 i=0;i<MAX_OPENED_FILES;i++) 
    {
        if(myDB->active[i]==1) 
        {
            for(INT32 j=0;j<myDB->files[i].elems;j++) 
            {
                for(INT32 k=0;k<myDB->files[i].fullData[j].threads;k++) 
                {
                    if(myDB->files[i].fullData[j].visible[k]==1) 
                    {
                        if((k==(myDB->files[i].fullData[j].threads-1)) && k>1) 
                        {
                            // Add the element to the palette
                            qp->fill(myDB->files[i].fullData[j].color[k]);
                            if(myDB->files[i].fullData[j].relativeView[k]==1) 
                            {
                                listColors->insertItem(*qp,QString("R File ")+QString::number(i)+QString(":")+QString(myDB->files[i].fullData[j].description)+QString(" Accum"));
                            } 
                            else if( myDB->files[i].fullData[j].maxView[k]==1) 
                            {
                                listColors->insertItem(*qp,QString("M File ")+QString::number(i)+QString(":")+QString(myDB->files[i].fullData[j].description)+QString(" Accum"));
                            } 
                            else 
                            {
                                listColors->insertItem(*qp,QString("  File ")+QString::number(i)+QString(": ")+QString(myDB->files[i].fullData[j].description)+QString(" Accum"));
                            }
                        } 
                        else 
                        {
                            // Add the element to the palette
                            qp->fill(myDB->files[i].fullData[j].color[k]);
                            if(myDB->files[i].fullData[j].relativeView[k]==1) 
                            {
                                listColors->insertItem(*qp,QString("R File ")+QString::number(i)+QString(":")+QString(myDB->files[i].fullData[j].description)+QString(" thread ")+QString::number(k));
                            } 
                            else if( myDB->files[i].fullData[j].maxView[k]==1) 
                            {
                                listColors->insertItem(*qp,QString("M File ")+QString::number(i)+QString(":")+QString(myDB->files[i].fullData[j].description)+QString(" thread ")+QString::number(k));
                            } 
                            else 
                            {
                                listColors->insertItem(*qp,QString("  File ")+QString::number(i)+QString(":")+QString(myDB->files[i].fullData[j].description)+QString(" thread ")+QString::number(k));
                            }
                        }
                    }
                }
            }
        }
    }

    for(INT32 i=0;i<MAX_OPENED_FILES;i++) 
    {
        if(myDB->active[i]==1) 
        {
            for(INT32 j=0;j<myDB->files[i].elems;j++) 
            {
                for(INT32 k=0;k<myDB->files[i].fullData[j].threads;k++) 
                {
                    if(myDB->files[i].fullData[j].activated[k]==1) 
                    {
                        if((k==(myDB->files[i].fullData[j].threads-1)) && k>1) 
                        {
                            // Add the element to the palette
                            qp->fill(QColor("White"));
                            if(myDB->files[i].fullData[j].relativeView[k]==1) 
                            {
                                listColorsActived->insertItem(*qp,QString("  File ")+QString::number(i)+QString(": ")+QString(myDB->files[i].fullData[j].description)+QString(" Accum"));
                            } 
                            else if( myDB->files[i].fullData[j].maxView[k]==1) 
                            {
                                listColorsActived->insertItem(*qp,QString("  File ")+QString::number(i)+QString(": ")+QString(myDB->files[i].fullData[j].description)+QString(" Accum"));
                            } 
                            else 
                            {
                                listColorsActived->insertItem(*qp,QString("  File ")+QString::number(i)+QString(": ")+QString(myDB->files[i].fullData[j].description)+QString(" Accum"));
                            }
                        } 
                        else 
                        {
                            // Add the element to the palette
                            qp->fill(QColor("White"));
                            if(myDB->files[i].fullData[j].relativeView[k]==1) 
                            {
                                listColorsActived->insertItem(*qp,QString("  File ")+QString::number(i)+QString(":")+QString(myDB->files[i].fullData[j].description)+QString(" thread ")+QString::number(k));
                            } 
                            else if( myDB->files[i].fullData[j].maxView[k]==1) 
                            {
                                listColorsActived->insertItem(*qp,QString("  File ")+QString::number(i)+QString(":")+QString(myDB->files[i].fullData[j].description)+QString(" thread ")+QString::number(k));
                            } 
                            else 
                            {
                                listColorsActived->insertItem(*qp,QString("  File ")+QString::number(i)+QString(":")+QString(myDB->files[i].fullData[j].description)+QString(" thread ")+QString::number(k));
                            }
                        }
                    }
                }
            }
        }
    }
}



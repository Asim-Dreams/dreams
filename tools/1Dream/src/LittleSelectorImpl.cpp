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

#include "LittleSelectorImpl.h"

/* 
 *  Constructs a LittleSelectorImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
LittleSelectorImpl::LittleSelectorImpl( QWidget* parent, 
                                        const char* name, 
                                        bool modal, 
                                        WFlags fl, 
                                        allFilesData *DB, 
                                        int fs, 
                                        int ss, 
                                        int ts , 
                                        ColorsImpl *vp, 
                                        StripChartView *vw )
    : LittleSelector( parent, name, modal, fl )
{
    tablePen[0]=Qt::SolidLine;
    tablePen[1]=Qt::DashLine;
    tablePen[2]=Qt::DotLine;
    tablePen[3]=Qt::DashDotLine;
    tablePen[4]=Qt::DashDotDotLine;

    visiblePalette=vp;
    view=vw;
    fileSelected=fs;
    stripSelected=ss;
    threadSelected=ts;

    myDB=DB;
}

void LittleSelectorImpl::fillData()
{
    int style;
    int h,s,v;
    QColor cl;
    char fn[500];
    char st[500];
    char th[500];

    Filename->setText(QString((Filename->text()).latin1())+QString(myDB->files[fileSelected].fileName->latin1()));

    StripChart->setText(QString((StripChart->text()).latin1())+QString(myDB->files[fileSelected].fullData[stripSelected].description));

    if((threadSelected>1)&&((myDB->files[fileSelected].fullData[stripSelected].threads-1)==threadSelected)) 
    {
        Thread->setText(QString((Thread->text()).latin1())+QString("Accumulated"));
    } 
    else 
    {
        Thread->setText(QString((Thread->text()).latin1())+QString::number(threadSelected));
    }

    if(myDB->files[fileSelected].fullData[stripSelected].lineStyle[threadSelected]==Qt::SolidLine) 
    {
        style=0;
    } 
    else if(myDB->files[fileSelected].fullData[stripSelected].lineStyle[threadSelected]==Qt::DashLine) 
    {
        style=1;
    } 
    else if(myDB->files[fileSelected].fullData[stripSelected].lineStyle[threadSelected]==Qt::DotLine) 
    {
        style=2;
    } 
    else if(myDB->files[fileSelected].fullData[stripSelected].lineStyle[threadSelected]==Qt::DashDotLine) 
    {
        style=3;
    } 
    else if(myDB->files[fileSelected].fullData[stripSelected].lineStyle[threadSelected]==Qt::DashDotDotLine) 
    {
        style=4;
    }
    
    // Fill the buttons
    myDB->files[fileSelected].fullData[stripSelected].visible[threadSelected]=1;
    Visible->setChecked(myDB->files[fileSelected].fullData[stripSelected].visible[threadSelected]==1);
    Hidden->setChecked(myDB->files[fileSelected].fullData[stripSelected].activated[threadSelected]==1);
    BreakView->setChecked(myDB->files[fileSelected].fullData[stripSelected].breakView[threadSelected]==1);
    Smooth->setChecked(myDB->files[fileSelected].fullData[stripSelected].smoothView[threadSelected]==1);
    SmoothValue->setValue(myDB->files[fileSelected].fullData[stripSelected].smooth[threadSelected]);
    BreakValue->setValue(myDB->files[fileSelected].fullData[stripSelected].breakSmooth[threadSelected]);
    SkewXValue->setValue(myDB->files[fileSelected].fullData[stripSelected].hSkew[threadSelected]);
    SkewYValue->setValue(myDB->files[fileSelected].fullData[stripSelected].vSkew[threadSelected]);
    LineWidthValue->setValue(myDB->files[fileSelected].fullData[stripSelected].lineWidth[threadSelected]);
    LineStyle->setCurrentItem(style);
    Color->setPaletteBackgroundColor(myDB->files[fileSelected].fullData[stripSelected].color[threadSelected]);
    cl=myDB->files[fileSelected].fullData[stripSelected].color[threadSelected];
    cl.getHsv(h,s,v);
    cl.setHsv(h,s,255-v);
    Color->setPaletteForegroundColor(cl);
    if(myDB->files[fileSelected].fullData[stripSelected].breakView[threadSelected]==1) 
    {
        BreakValue->setEnabled(TRUE);
    } 
    else 
    {
        BreakValue->setDisabled(TRUE);
    }
    
    if(myDB->files[fileSelected].fullData[stripSelected].smoothView[threadSelected]==1) 
    {
        SmoothValue->setEnabled(TRUE);
    } 
    else 
    {
        SmoothValue->setDisabled(TRUE);
    }
    
    if(myDB->files[fileSelected].fullData[stripSelected].maxValue[threadSelected]==0) 
    {
        HardwareMAX->setDisabled(TRUE);
        RelativeHardwareMAX->setDisabled(TRUE);
    } 
    else 
    {
        HardwareMAX->setEnabled(TRUE);
        RelativeHardwareMAX->setEnabled(TRUE);
        HardwareMAX->setText(QString::number(myDB->files[fileSelected].fullData[stripSelected].maxValue[threadSelected]));
    }
    
    FileMAX->setText(QString::number(myDB->files[fileSelected].fullData[stripSelected].maxInStripValue[threadSelected]));

    Absolute->setChecked(1==0);
    RelativeFileMAX->setChecked(1==0);
    RelativeHardwareMAX->setChecked(1==0);
    if(myDB->files[fileSelected].fullData[stripSelected].relativeView[threadSelected]==1) 
    {
        RelativeFileMAX->setChecked(myDB->files[fileSelected].fullData[stripSelected].relativeView[threadSelected]==1);
    } 
    else if (myDB->files[fileSelected].fullData[stripSelected].maxView[threadSelected]==1) 
    {
        RelativeHardwareMAX->setChecked(myDB->files[fileSelected].fullData[stripSelected].maxView[threadSelected]==1);
    } 
    else 
    {
        Absolute->setChecked(1==1);
    }
}


/*  
 *  Destroys the object and frees any allocated resources
 */
LittleSelectorImpl::~LittleSelectorImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void LittleSelectorImpl::connectRoutines()
{
    // Color
    connect(Color,SIGNAL(pressed()),
            this,SLOT(do_ColorClicked()));
    connect(Color,SIGNAL(clicked()),
            this,SLOT(do_ColorClicked()));

    // OK
    connect(buttonOk,SIGNAL(pressed()),
            this,SLOT(do_OKClicked()));
    connect(buttonOk,SIGNAL(clicked()),
            this,SLOT(do_OKClicked()));

    // Visible
    connect(Visible,SIGNAL(clicked()),
            this,SLOT(do_VisibleClicked()));

    // Hidden
    connect(Hidden,SIGNAL(clicked()),
            this,SLOT(do_HiddenClicked()));

    // SmoothValue
    connect(SmoothValue,SIGNAL(valueChanged(int)),
            this,SLOT(do_SmoothClicked(int)));

    // BreakValue
    connect(BreakValue,SIGNAL(valueChanged(int)),
            this,SLOT(do_BreakClicked(int)));

    // SkewXValue
    connect(SkewXValue,SIGNAL(valueChanged(int)),
            this,SLOT(do_HSkewClicked(int)));

    // SkewYValue
    connect(SkewYValue,SIGNAL(valueChanged(int)),
            this,SLOT(do_VSkewClicked(int)));

    // LineWidthValue
    connect(LineWidthValue,SIGNAL(valueChanged(int)),
            this,SLOT(do_LineWidthClicked(int)));

    // LineStyle
    connect(LineStyle,SIGNAL(activated(int)),
            this,SLOT(do_LineStyleClicked(int)));
    connect(LineStyle,SIGNAL(highlighted(int)),
            this,SLOT(do_LineStyleClicked(int)));

    // Absolute 
    connect(Absolute,SIGNAL(clicked()),
            this,SLOT(do_AbsoluteClicked()));

    // RelativeHardwareMAX
    connect(RelativeHardwareMAX,SIGNAL(clicked()),
            this,SLOT(do_RelativeMAXClicked()));

    // RelativeFileMAX
    connect(RelativeFileMAX,SIGNAL(clicked()),
            this,SLOT(do_RelativeValueClicked()));

    // BreakView
    connect(BreakView,SIGNAL(clicked()),
            this,SLOT(do_BreakViewClicked()));

    // SmoothView
    connect(Smooth,SIGNAL(clicked()),
            this,SLOT(do_SmoothViewClicked()));
}

void LittleSelectorImpl::do_VisibleClicked()
{
    myDB->files[fileSelected].fullData[stripSelected].visible[threadSelected]=Visible->isChecked()==TRUE;

    if(myDB->files[fileSelected].fullData[stripSelected].visible[threadSelected]==0) 
    {
        myDB->files[fileSelected].fullData[stripSelected].activated[threadSelected]=0;
        Hidden->setDisabled(TRUE);
    } 
    else 
    {
        Hidden->setChecked(myDB->files[fileSelected].fullData[stripSelected].activated[threadSelected]==1);
        Hidden->setEnabled(TRUE);
    }
}

void LittleSelectorImpl::do_HiddenClicked()
{
    myDB->files[fileSelected].fullData[stripSelected].activated[threadSelected]=Hidden->isChecked()==TRUE;
}

void LittleSelectorImpl::do_RelativeMAXClicked()
{
    myDB->files[fileSelected].fullData[stripSelected].maxView[threadSelected]=1;
    myDB->files[fileSelected].fullData[stripSelected].relativeView[threadSelected]=0;
    Absolute->setChecked(1==0);
    RelativeFileMAX->setChecked(1==0);
    RelativeHardwareMAX->setChecked(1==0);
    
    if(myDB->files[fileSelected].fullData[stripSelected].relativeView[threadSelected]==1) 
    {
        RelativeFileMAX->setChecked(myDB->files[fileSelected].fullData[stripSelected].relativeView[threadSelected]==1);
    } 
    else if (myDB->files[fileSelected].fullData[stripSelected].maxView[threadSelected]==1) 
    {
        RelativeHardwareMAX->setChecked(myDB->files[fileSelected].fullData[stripSelected].maxView[threadSelected]==1);
    } 
    else 
    {
        Absolute->setChecked(1==1);
    }
}

void LittleSelectorImpl::do_RelativeValueClicked()
{
    myDB->files[fileSelected].fullData[stripSelected].maxView[threadSelected]=0;
    myDB->files[fileSelected].fullData[stripSelected].relativeView[threadSelected]=1;
    Absolute->setChecked(1==0);
    RelativeFileMAX->setChecked(1==0);
    RelativeHardwareMAX->setChecked(1==0);
    
    if(myDB->files[fileSelected].fullData[stripSelected].relativeView[threadSelected]==1) 
    {
        RelativeFileMAX->setChecked(myDB->files[fileSelected].fullData[stripSelected].relativeView[threadSelected]==1);
    } 
    else if (myDB->files[fileSelected].fullData[stripSelected].maxView[threadSelected]==1) 
    {
        RelativeHardwareMAX->setChecked(myDB->files[fileSelected].fullData[stripSelected].maxView[threadSelected]==1);
    } 
    else 
    {
        Absolute->setChecked(1==1);
    }
}

void LittleSelectorImpl::do_AbsoluteClicked()
{
    myDB->files[fileSelected].fullData[stripSelected].maxView[threadSelected]=0;
    myDB->files[fileSelected].fullData[stripSelected].relativeView[threadSelected]=0;
    Absolute->setChecked(1==0);
    RelativeFileMAX->setChecked(1==0);
    RelativeHardwareMAX->setChecked(1==0);
    
    if(myDB->files[fileSelected].fullData[stripSelected].relativeView[threadSelected]==1) 
    {
        RelativeFileMAX->setChecked(myDB->files[fileSelected].fullData[stripSelected].relativeView[threadSelected]==1);
    } 
    else if (myDB->files[fileSelected].fullData[stripSelected].maxView[threadSelected]==1) 
    {
        RelativeHardwareMAX->setChecked(myDB->files[fileSelected].fullData[stripSelected].maxView[threadSelected]==1);
    } 
    else 
    {
        Absolute->setChecked(1==1);
    }
}

void LittleSelectorImpl::do_BreakViewClicked()
{
    myDB->files[fileSelected].fullData[stripSelected].breakView[threadSelected]=BreakView->isChecked()==TRUE;
    myDB->files[fileSelected].fullData[stripSelected].smoothView[threadSelected]=0;
    Smooth->setChecked(myDB->files[fileSelected].fullData[stripSelected].smoothView[threadSelected]==1);
    
    if(myDB->files[fileSelected].fullData[stripSelected].breakView[threadSelected]==1) 
    {
        BreakValue->setEnabled(TRUE);
    } 
    else 
    {
        BreakValue->setDisabled(TRUE);
    }
    
    if(myDB->files[fileSelected].fullData[stripSelected].smoothView[threadSelected]==1) 
    {
        SmoothValue->setEnabled(TRUE);
    } 
    else 
    {
        SmoothValue->setDisabled(TRUE);
    }
}

void LittleSelectorImpl::do_SmoothViewClicked()
{
    myDB->files[fileSelected].fullData[stripSelected].smoothView[threadSelected]=Smooth->isChecked()==TRUE;
    myDB->files[fileSelected].fullData[stripSelected].breakView[threadSelected]=0;
    BreakView->setChecked(myDB->files[fileSelected].fullData[stripSelected].breakView[threadSelected]==1);
    
    if(myDB->files[fileSelected].fullData[stripSelected].smoothView[threadSelected]==1) 
    {
        SmoothValue->setEnabled(TRUE);
    } 
    else 
    {
        SmoothValue->setDisabled(TRUE);
    }
    
    if(myDB->files[fileSelected].fullData[stripSelected].breakView[threadSelected]==1) 
    {
        BreakValue->setEnabled(TRUE);
    } 
    else 
    {
        BreakValue->setDisabled(TRUE);
    }
}

void LittleSelectorImpl::do_LineStyleClicked(int index)
{
    myDB->files[fileSelected].fullData[stripSelected].lineStyle[threadSelected]=tablePen[index];
}

void LittleSelectorImpl::do_SmoothClicked(int value)
{
    myDB->files[fileSelected].fullData[stripSelected].smooth[threadSelected]=value;
}

void LittleSelectorImpl::do_BreakClicked(int value)
{
    myDB->files[fileSelected].fullData[stripSelected].breakSmooth[threadSelected]=value;
}

void LittleSelectorImpl::do_HSkewClicked(int value)
{
    myDB->files[fileSelected].fullData[stripSelected].hSkew[threadSelected]=value;
}

void LittleSelectorImpl::do_VSkewClicked(int value)
{
    myDB->files[fileSelected].fullData[stripSelected].vSkew[threadSelected]=value;
}

void LittleSelectorImpl::do_LineWidthClicked(int value)
{
    myDB->files[fileSelected].fullData[stripSelected].lineWidth[threadSelected]=value;
}

void LittleSelectorImpl::do_OKClicked()
{
    visiblePalette->UpdateColors();
    view->updateContents();
}

void LittleSelectorImpl::do_ColorClicked()
{
    int h,s,v;
    QColor cl;

    QColor c=QColorDialog::getColor( myDB->files[fileSelected].fullData[stripSelected].color[threadSelected], this );
    
    if ( c.isValid() ) 
    {
            myDB->files[fileSelected].fullData[stripSelected].color[threadSelected]=c;
    }
    
    Color->setPaletteBackgroundColor(myDB->files[fileSelected].fullData[stripSelected].color[threadSelected]);
    Color->setDown(FALSE);

    cl=myDB->files[fileSelected].fullData[stripSelected].color[threadSelected];
    cl.getHsv(h,s,v);
    cl.setHsv(h,s,255-v);
    Color->setPaletteForegroundColor(cl);
}


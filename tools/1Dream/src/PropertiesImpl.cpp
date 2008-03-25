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

#include "PropertiesImpl.h"

#include <qpushbutton.h>
#include <qcolordialog.h>
#include <qspinbox.h>

/* 
 *  Constructs a PropertiesImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PropertiesImpl::PropertiesImpl( QWidget* parent,
                                const char* name, 
                                bool modal, 
                                WFlags fl, 
                                allFilesData *DB )
    : Properties( parent, name, modal, fl )
{
    myDB=DB;
}

void PropertiesImpl::connectRoutines()
{
    UpdateColors();

    connect(HGridColor,SIGNAL(pressed()),
            this,SLOT(do_HGridColorClicked()));
    connect(HGridColor,SIGNAL(clicked()),
            this,SLOT(do_HGridColorClicked()));

    connect(VGridColor,SIGNAL(pressed()),
            this,SLOT(do_VGridColorClicked()));
    connect(VGridColor,SIGNAL(clicked()),
            this,SLOT(do_VGridColorClicked()));

    connect(HAxisColor,SIGNAL(pressed()),
            this,SLOT(do_HAxisColorClicked()));
    connect(HAxisColor,SIGNAL(clicked()),
            this,SLOT(do_HAxisColorClicked()));

    connect(VAxisColor,SIGNAL(pressed()),
            this,SLOT(do_VAxisColorClicked()));
    connect(VAxisColor,SIGNAL(clicked()),
            this,SLOT(do_VAxisColorClicked()));

    connect(BackgroundColor,SIGNAL(pressed()),
            this,SLOT(do_BackgroundColorClicked()));
    connect(BackgroundColor,SIGNAL(clicked()),
            this,SLOT(do_BackgroundColorClicked()));

    connect(MarkersLineColor,SIGNAL(pressed()), 
            this,SLOT(do_MarkersLineColorClicked()));
    connect(MarkersLineColor,SIGNAL(clicked()),
            this,SLOT(do_MarkersLineColorClicked()));
}

void PropertiesImpl::UpdateColors()
{
    int h,s,v;
    QColor cl;

    HGridColor->setPaletteBackgroundColor(myDB->generalProperties.HGrid);
    cl=myDB->generalProperties.HGrid;
    cl.getHsv(h,s,v);
    cl.setHsv(h,s,255-v);
    HGridColor->setPaletteForegroundColor(cl);
    VGridColor->setPaletteBackgroundColor(myDB->generalProperties.VGrid);
    cl=myDB->generalProperties.VGrid;
    cl.getHsv(h,s,v);
    cl.setHsv(h,s,255-v);
    VGridColor->setPaletteForegroundColor(cl);
    HAxisColor->setPaletteBackgroundColor(myDB->generalProperties.HAxis);
    cl=myDB->generalProperties.HAxis;
    cl.getHsv(h,s,v);
    cl.setHsv(h,s,255-v);
    HAxisColor->setPaletteForegroundColor(cl);
    VAxisColor->setPaletteBackgroundColor(myDB->generalProperties.VAxis);
    cl=myDB->generalProperties.VAxis;
    cl.getHsv(h,s,v);
    cl.setHsv(h,s,255-v);
    VAxisColor->setPaletteForegroundColor(cl);
    BackgroundColor->setPaletteBackgroundColor(myDB->generalProperties.Background);
    cl=myDB->generalProperties.Background;
    cl.getHsv(h,s,v);
    cl.setHsv(h,s,255-v);
    BackgroundColor->setPaletteForegroundColor(cl);
    MarkersLineColor->setPaletteBackgroundColor(myDB->generalProperties.Markers);
    cl=myDB->generalProperties.Markers;
    cl.getHsv(h,s,v);
    cl.setHsv(h,s,255-v);
    MarkersLineColor->setPaletteForegroundColor(cl);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
PropertiesImpl::~PropertiesImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void PropertiesImpl::do_HGridColorClicked()
{
    QColor c=QColorDialog::getColor(myDB->generalProperties.HGrid,this);

    if ( c.isValid() ) 
    {
            myDB->generalProperties.HGrid=c;
    }
    
    HGridColor->setDown(FALSE);

    UpdateColors();
}

void PropertiesImpl::do_VGridColorClicked()
{
    QColor c=QColorDialog::getColor(myDB->generalProperties.VGrid,this);

    if ( c.isValid() ) 
    {
            myDB->generalProperties.VGrid=c;
    }
    
    VGridColor->setDown(FALSE);

    UpdateColors();
}

void PropertiesImpl::do_HAxisColorClicked()
{
    QColor c=QColorDialog::getColor(myDB->generalProperties.HAxis,this);

    if ( c.isValid() ) 
    {
            myDB->generalProperties.HAxis=c;
    }
    
    HAxisColor->setDown(FALSE);

    UpdateColors();
}

void PropertiesImpl::do_VAxisColorClicked()
{
    QColor c=QColorDialog::getColor(myDB->generalProperties.VAxis,this);

    if ( c.isValid() ) 
    {
            myDB->generalProperties.VAxis=c;
    }
    
    VAxisColor->setDown(FALSE);

    UpdateColors();
}

void PropertiesImpl::do_BackgroundColorClicked()
{
    QColor c=QColorDialog::getColor(myDB->generalProperties.Background,this);

    if ( c.isValid() ) 
    {
            myDB->generalProperties.Background=c;
    }
    
    BackgroundColor->setDown(FALSE);

    UpdateColors();
}

void PropertiesImpl::do_MarkersLineColorClicked()
{
    QColor c=QColorDialog::getColor(myDB->generalProperties.Markers,this);

    if ( c.isValid() ) 
    {
            myDB->generalProperties.Markers=c;
    }
    
    MarkersLineColor->setDown(FALSE);

    UpdateColors();
}


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

#ifndef PROPERTIESIMPL_H
#define PROPERTIESIMPL_H
#include "Properties.h"
#include "StripEntry.h"

class PropertiesImpl : public Properties
{ 
    Q_OBJECT

  public:
    PropertiesImpl( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 , allFilesData *DB=NULL);
    ~PropertiesImpl();
    void connectRoutines();
    void UpdateColors();
    
  public slots:    
    void do_HGridColorClicked();
    void do_VGridColorClicked();
    void do_HAxisColorClicked();
    void do_VAxisColorClicked();
    void do_BackgroundColorClicked();
    void do_MarkersLineColorClicked();
    
  public:
    allFilesData *myDB;

};

#endif // PROPERTIESIMPL_H

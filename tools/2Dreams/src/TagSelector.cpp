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

/**
  * @file TagSelector.cpp
  */

#include <qstrlist.h>

#include "TagSelector.h"

TagSelector::TagSelector(QWidget* parent, FavoriteTags* ft):QComboBox(false,parent)
{
    myFT = ft;
    init();
}

TagSelector::~TagSelector()
{}

void
TagSelector::init()
{
    clear();
    QStrList strlist = myFT->getList();
    char* str;
    for (str=strlist.first();str!=NULL;str=strlist.next())
    {
        insertItem(QString(str));
    }
    if (strlist.first()!=NULL)
    {
        setCurrentItem(0);
    }
}


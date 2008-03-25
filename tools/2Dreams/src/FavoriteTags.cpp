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
  * @file FavoriteTags.cpp
  */

// 2Dreams
#include "FavoriteTags.h"
#include "PreferenceMgr.h"

FavoriteTags* FavoriteTags::_myInstance;

FavoriteTags*
FavoriteTags::getInstance()
{
    if (_myInstance==NULL)
        _myInstance = new FavoriteTags();

    return _myInstance;
}

void
FavoriteTags::destroy()
{
    if (_myInstance!=NULL) { delete _myInstance; }
}

FavoriteTags::FavoriteTags()
{
    refresh();
}

FavoriteTags::~FavoriteTags()
{
    (PreferenceMgr::getInstance())->setFavoriteTags(myList);
}

void
FavoriteTags::refresh()
{
    myList.clear();

    // get the preferences favorite list:
    QStrList prefList = (PreferenceMgr::getInstance())->getFavoriteTags();
    char* str;

    for (str=prefList.first();str!=NULL;str=prefList.next())
    {
        myList.append(str);
    }
}

/**
  * @todo review sizeof this object, how to count the String objects inside...
  */
INT64 
FavoriteTags::getObjSize() const
{return sizeof(FavoriteTags);}

QString 
FavoriteTags::getUsageDescription() const
{return NULL;}

QString 
FavoriteTags::getSummaryUsageDescription() const
{return NULL;}


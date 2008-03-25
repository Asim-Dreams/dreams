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
  * @file  FavoriteTags.h
  */

#ifndef _FAVORITETAGS_H
#define _FAVORITETAGS_H

#include "DreamsDefs.h"
// QT Library
#include <qstrlist.h>

#include "dDB/DralDBSyntax.h"

/**
  * This class holds a list with the "favorite" Tags.
  * Favorite Tags are used for the "event docked window"
  * and as basis for TagSelector combo box class.
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  *
  * @todo Right now this class has a harcoded list, the idea is that
  * you gonna have a default list but user must be able to change its
  * favorite list tag.
  */
class FavoriteTags
{
    public:
        // ---- AMemObj Interface methods
        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;
        virtual QString getSummaryUsageDescription() const;

    public:
        static FavoriteTags* getInstance();
        static void destroy();

        void refresh();
        inline QStrList getList() { return myList; }

    protected:    
        FavoriteTags();
        virtual ~FavoriteTags();

    private:
        QStrList myList;
        static FavoriteTags* _myInstance;

};

#endif

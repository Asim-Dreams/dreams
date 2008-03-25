// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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
  * @file findImpl.h
  */

#ifndef FINDIMPL_H
#define FINDIMPL_H

//#include "asim/AScrollView.h"
#include "asim/DralDB.h"

#include "find.h"
#include "DreamsDB.h"
#include "DisDockWin.h"

#include "TwoDAvtView.h"

/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-11-21
  * @version 0.1
  */
class findImpl : public FindForm
{
    Q_OBJECT

    public:
        findImpl(TwoDAvtView* avt, QWidget* parent = 0, const char* name = 0,bool modal = FALSE, WFlags fl = 0);
        ~findImpl();
        
        void aboutToFind();

    public:
        static void find_next();
        static bool hadSucessFullSearch();

    public slots:
        void knownTagsClicked(void);
        void findClicked(void);
        void cancelClicked(void);
        void tagNameTextChanged(const QString&);
        void reToggled(bool);
        
    protected:
        static void doFind(QString str, INT32 tagId, TagValueType tgtype, INT32 startCycle, INT32 startRow,bool cs,bool em,bool re);

    protected:
        static DralDB*      draldb;
        static DreamsDB*    db;
        static TwoDAvtView* myAvt;
        static DisDockWin*  disWin;

        static bool    sucessFullSearch;
        //static INT32   lastStartCycle;
        static INT32   lastCycle;
        static INT32   lastRow;
        static UINT16  lastTagId;
        static QString lastValue;
        static bool    lastCS;
        static bool    lastEM;
        static bool    lastRE;
        static TagValueType lastTagValueType;
};

#endif

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
  * @file HighLightMgr.h
  */

#ifndef _HIGHLIGHT_H
#define _HIGHLIGHT_H

#include <stdio.h>
#include <math.h>

#include <qvaluelist.h>
#include <qvaluestack.h>
#include <qpainter.h>
#include <qwidget.h>
#include <qlistview.h>
#include <qpixmap.h>
#include <qbrush.h>
#include <qpen.h>

#include "asim/DralDB.h"

#include "ZDBase.h"
#include "DrawingTools.h"
#include "hlightDialogImpl.h"

typedef enum
{
     HLT_DEFAULT = 0,
     HLT_TYPE1,
     HLT_TYPE2,
     HLT_TYPE3,
     HLT_TYPE4,
     HLT_TYPE5,
     HLT_TYPE6,
     HLT_TYPE7,
     HLT_TYPE8,
     HLT_MAXTYPE
} HighLightType;

#define MAX_HIGHLIGHTS 8

/**
  * This is a node (place holder) for highlight criteria list.
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class HighLightListItem
{
    public:
        HighLightListItem(){};
        HighLightListItem (int c, int r, HighLightType t, ItemHandler * hnd)
        {
            col = c;
            row = r;
            highlight_type = t;
            handler = *hnd;
        }

    public:
        int           col;
        int           row;
        ItemHandler   handler;
        HighLightType highlight_type;
};

typedef QValueList<HighLightListItem> HLightList;
typedef QValueListIterator<HighLightListItem> HLightListIterator;

/**
  * This class wraps all the requiered information to keep a highlight criteria.
  * Basically this means the tag Id, and the matching value and a list of matching events.
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class HighLightCriteria
{
    public:
        HighLightCriteria() {eventList=NULL;}
        HighLightCriteria (INT32 tid, UINT64 val, HighLightType t)
        {
            tagId = tid;
            tagValue = val;
            type = t;
            eventList = new HLightList();
            Q_ASSERT(eventList!=NULL);
        }

    public:
        INT32 tagId;
        UINT64 tagValue;
        HighLightType type;

        // the event list
        HLightList* eventList;
};

typedef QValueStack<HighLightCriteria> HLCriteriaStack;
typedef QValueStack<HighLightType> HLFreeTypeStack;

#define HL_BORDER_SZ 2

/**
  * This class wraps all the functionality needed to manage highlighting and
  * highlighting criteria stack.
  *
  * This class is resposable of managing a criteria stack, to draw and update
  * event lists (for each criteria) as long as a scrolling or view change is
  * detected by low-level QT events.
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class HighLightMgr : public QObject
{
    Q_OBJECT
    public:  
        HighLightMgr(int);
        HighLightMgr(HighLightMgr* parent);
        ~HighLightMgr();
        
        void drawHighlights(QPainter* p, int x1, int x2, int y1, int y2, bool visible);

        bool contentsSelectEvent(int col,int row, bool adding,
             QString currentTagSelector,HighLightType htype,bool noErase=false);

        bool contentsSelectEvent(ItemHandler * hnd, bool adding,
             QString currentTagSelector, HighLightType htype,bool noErase=false, int col=0);

        void  updateViewRange (int x1, int x2, int y1, int y2);

        bool  purgeLast();
        bool  purgeLayer(int layer);
        bool  purgeAll();

        void showCriteria();

        inline INT32 getNumActiveCriteria();

        INT32 lookForEvent(int col, int row);
        INT32 lookForEvent(ItemHandler * hnd);
        QColor peekNextColor();
        
    signals:
        void highlightChanged(int);
        
    protected:
        void init(int);
        inline void addHighLight (int ,int , HighLightType type, HLightList* eventList, ItemHandler * hnd);
        inline void purge();
        inline bool isInner (int, int, int, int, int, int);
        inline void selectPen(QPainter* p,HighLightType htype );

        void initFreeStack();
        HighLightType popFreeType();
        void pushFreeType(HighLightType type);
        
    private:
        ZDBase*        myDB;
        DralDB*        draldb;
        int            cube_side;

        // pre-built QPen's & colors
        QPen   hpens[MAX_HIGHLIGHTS];
        QColor hcolors[MAX_HIGHLIGHTS];

        INT32 last_x1;
        INT32 last_x2;
        INT32 last_y1;
        INT32 last_y2;

        HLCriteriaStack* criteriaStack;
        HLFreeTypeStack* hltypeStack;

        hlightDialogImpl* hcd;
        QListView* qlv;
};


void 
HighLightMgr::purge()
{
    initFreeStack();
    criteriaStack->clear();
    //updateMenus();
}

void 
HighLightMgr::addHighLight (int col, int row, HighLightType type,
     HLightList* eventList, ItemHandler * hnd)
{
    //printf("!!! addHighLight col=%d,row=%d,itemIdx=%d\n",col,row,itemIdx);fflush(stdout);
    HighLightListItem item(col,row,type,hnd);
    eventList->append(item);
}

bool 
HighLightMgr::isInner (int x1, int x2, int y1, int y2, int col, int row)
{
    bool c1 = (col>=x1) && (col<=x2);
    bool c2 = (row>=y1) && (row<=y2);
    return (c1&&c2);
}

void 
HighLightMgr::selectPen(QPainter* p,HighLightType htype )
{
    p->setPen( hpens[(INT32)htype - 1] );
}

INT32
HighLightMgr::getNumActiveCriteria()
{
    return (INT32) criteriaStack->count();
}

#endif

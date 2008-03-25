// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
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

// General includes.
#include <math.h>

// Dreams includes.
#include "dDB/itemheap/ItemHeap.h"
#include "DreamsDefs.h"

// Qt includes.
#include <qvaluelist.h>
#include <qvaluestack.h>
#include <qpainter.h>
#include <qpen.h>
#include <qobject.h>

// Class forwarding.
class HighLightMgrDialogImpl;
class QListView;
class DreamsDB;
class DralDB;

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
        HighLightListItem()
        {
        }

        HighLightListItem(CYCLE c, INT32 r, HighLightType t, ItemHandler * hnd)
        {
            cycle = c;
            row = r;
            highlight_type = t;
            handler = * hnd;
        }

    public:
        CYCLE cycle;
        INT32 row;
        ItemHandler handler;
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
        HighLightCriteria()
        {
            eventList = NULL;
        }

        HighLightCriteria(TAG_ID tid, UINT64 val, HighLightType t)
        {
            tagId = tid;
            tagValue = val;
            type = t;
            eventList = new HLightList();
            Q_ASSERT(eventList!=NULL);
        }

    public:
        TAG_ID tagId;
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
    HighLightMgr(DreamsDB * dreamsdb, INT32 cside);
    HighLightMgr(HighLightMgr * parent);
    ~HighLightMgr();
  
    virtual void updateViewRange(int x1, int x2, int y1, int y2) { }
    virtual void drawHighlights(QPainter * p, int x1, int x2, int y1, int y2, bool visible) { }
    virtual bool contentsSelectEvent(INT32 col, INT32 row, CYCLE cycle, bool adding, QString currentTagSelector, HighLightType htype, bool noErase) = 0;
    bool contentsSelectEvent(ItemHandler * hnd, bool adding, QString currentTagSelector, HighLightType htype, bool noErase, CYCLE cycle);
    bool contentsSelectEvent(TAG_ID tagId, UINT64 tagValue, bool adding, HighLightType htype, bool noErase, bool reemit);

    bool purgeLast();
    bool purgeLayer(int layer, bool reemit);
    bool purgeAll();
    bool unhighlightCriteria(TAG_ID tagId, UINT64 tagValue);

    void showCriteria();
    void copyState(HighLightMgr * base);

    inline INT32 getNumActiveCriteria();
    inline HighLightCriteria &getCriteriaAt(UINT32 layer);
    inline void selectPen(QPainter* p, HighLightType htype );

    INT32 lookForEvent(CYCLE cycle, INT32 row);
    INT32 lookForEvent(ItemHandler * hnd);
    INT32 getMatchingCriteria(TAG_ID tagId, UINT64 tagValue);
    QColor peekNextColor();

    inline DreamsDB * getDreamsDB() const;
    inline void reset();

    void changeSize(UINT32 size);
        
  signals:
    void highlightChanged(int);
    void purgeLastHighlight();
    void unhighlightCriteriaSignal(TAG_ID tagId, UINT64 tagValue);
    void contentsSelectedEvent(TAG_ID tagId, UINT64 tagValue, bool adding, HighLightType htype, bool noErase);
        
  protected:
    void init(DreamsDB * _dreamsdb, INT32 cside);
    inline void addHighLight(CYCLE cycle, INT32 row, HighLightType type, HLightList * eventList, ItemHandler * hnd);
    inline void purge();
    inline bool isInner (int, int, int, int, int, int);

    void initFreeStack();
    HighLightType popFreeType();
    void pushFreeType(HighLightType type);
    
    DreamsDB*      dreamsdb;
    DralDB*        draldb;
    INT32          cube_side;

    // pre-built QPen's & colors
    QPen   hpens[MAX_HIGHLIGHTS];
    QColor hcolors[MAX_HIGHLIGHTS];

    INT32 last_x1;
    INT32 last_x2;
    INT32 last_y1;
    INT32 last_y2;

    HLCriteriaStack* criteriaStack;
    HLFreeTypeStack* hltypeStack;

    HighLightMgrDialogImpl* hcd;
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
HighLightMgr::addHighLight(CYCLE cycle, INT32 row, HighLightType type, HLightList * eventList, ItemHandler * hnd)
{
    //printf("!!! addHighLight col=%d,row=%d,itemIdx=%d\n",col,row,itemIdx);fflush(stdout);
    HighLightListItem item(cycle, row, type, hnd);
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

HighLightCriteria &
HighLightMgr::getCriteriaAt(UINT32 layer)
{
    Q_ASSERT(layer < criteriaStack->count());
    HLCriteriaStack::iterator it = criteriaStack->at(layer);
    return *it;
}

DreamsDB *
HighLightMgr::getDreamsDB() const
{
    return dreamsdb;
}

void
HighLightMgr::reset()
{
    last_x1 = 0x7fffffff;
    last_x2 = 0;
    last_y1 = 0x7fffffff;
    last_y2 = 0;
}

#endif

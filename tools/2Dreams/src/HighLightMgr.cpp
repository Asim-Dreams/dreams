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
  * @file HighLightMgr.cpp
  */

// 2Dreams
#include "HighLightMgr.h"

// QT Library
#include <qmessagebox.h>

HighLightMgr::HighLightMgr(int cside)
{
    init(cside);

    criteriaStack= new HLCriteriaStack();
    Q_ASSERT(criteriaStack!=NULL);
    hltypeStack = new HLFreeTypeStack();
    Q_ASSERT(hltypeStack!=NULL);
    initFreeStack();
}

HighLightMgr::HighLightMgr(HighLightMgr* parent)
{
    init(parent->cube_side);

    // copy state
    last_x1 = parent->last_x1;
    last_x2 = parent->last_x2;
    last_y1 = parent->last_y1;
    last_y2 = parent->last_y2;

    criteriaStack = new HLCriteriaStack(*(parent->criteriaStack));
    hltypeStack = new HLFreeTypeStack(*(parent->hltypeStack));
}

void 
HighLightMgr::init(int cside)
{

    myDB = ZDBase::getInstance();
    cube_side = cside;
    //nextHType = HLT_TYPE1;
    last_x1=0x7fffffff;
    last_x2=0;
    last_y1=0x7fffffff;
    last_y2=0;

    // right now use hardcoded colors
    hcolors[0] = QColor(246,210,8  );
    hcolors[1] = QColor(176,246,96 );
    hcolors[2] = QColor(101,255,252);
    hcolors[3] = QColor(255,134,139);
    hcolors[4] = QColor(213,202,255);
    hcolors[5] = QColor(30 ,255,0  );
    hcolors[6] = QColor(255,2  ,247);
    hcolors[7] = QColor(255,255,255);

    // pre-build dialog
    hcd = new hlightDialogImpl(NULL, "hcd", true);
    Q_ASSERT(hcd!=NULL);
    qlv = hcd->getListView();

    for (INT32 i=0;i<MAX_HIGHLIGHTS;i++)
    {
        hpens[i] = QPen(hcolors[i],HL_BORDER_SZ);
    }
    draldb = DralDB::getInstance();
}

HighLightMgr::~HighLightMgr()
{
    if (criteriaStack!=NULL)
    {
        delete criteriaStack;
    }
}

void
HighLightMgr::initFreeStack()
{
    hltypeStack->clear();
    // insert all types as "free" (or available if you want)
    for (INT32 i=(INT32)HLT_TYPE1; i < (INT32)HLT_MAXTYPE; i++)
    {
        hltypeStack->push((HighLightType)i);
    }
}

HighLightType
HighLightMgr::popFreeType()
{
  Q_ASSERT(!hltypeStack->isEmpty());
  return hltypeStack->pop();
}

void
HighLightMgr::pushFreeType(HighLightType type)
{
    hltypeStack->push(type);
    if (hltypeStack->count() == (HLT_MAXTYPE-1))
    {
        initFreeStack();
    }
}

QColor
HighLightMgr::peekNextColor()
{
    if (hltypeStack->isEmpty())
    {
        return hcolors[1];
    }
    else
    {
        HighLightType type = hltypeStack->pop();
        hltypeStack->push(type);
        return (hcolors[(INT32)type - 1]);
    }    
}


void 
HighLightMgr::drawHighlights(QPainter* p, int x1, int x2, int y1, int y2, bool visible)
{
    // divide by element-size...
    x1 = (int)floor((double)x1/cube_side);
    x2 = (int)floor((double)x2/cube_side);
    y1 = (int)ceil((double)y1/cube_side);
    y2 = (int)ceil((double)y2/cube_side);

    // iterate through criteria list
    HLCriteriaStack::iterator it;
    for (it = criteriaStack->begin(); it != criteriaStack->end(); ++it )
    {
        HighLightCriteria criteria = *it;
        HLightList* eventList = criteria.eventList;

        // iterate through the highlight list
        HLightListIterator hl_iter;
        for (hl_iter = eventList->begin(); hl_iter != eventList->end(); ++hl_iter )
        {
            HighLightListItem item = *hl_iter;
            // unpack the stored data
            int col = item.col;
            int row = item.row;
            HighLightType htype = item.highlight_type;

            if (isInner(x1,x2,y1,y2,col-myDB->getFirstEffectiveCycle(),row))
            {
                if ( myDB->hasValidData(col,row))
                {
                    ColorShapeItem csh = myDB->getColorShape(col,row);
                    col -=myDB->getFirstEffectiveCycle();
                    if (visible)
                    {
                        p->setBrush(QBrush( AColorPalette::getColor(csh.getColorIdx())) );
                        selectPen(p,htype);
                        DrawingTools::drawShape(col*cube_side,row*cube_side,p,csh.getShape(),cube_side);
                    }
                    else
                    {
                        p->eraseRect (col*cube_side, row*cube_side, cube_side, cube_side);
                    }
                }
            }
        }        // inner loop
    }            // outer loop
}



bool
HighLightMgr::contentsSelectEvent(ItemHandler * hnd, bool adding,
QString currentTagSelector, HighLightType htype,bool noErase, int col)
{
    //printf ("@@@ contentsSelectEvent called with itemIdx=%d, col=%d\n",itemIdx,col);fflush(stdout);
    UINT64 tgvalue;
    UINT64 tgwhen;
    TagValueType tgvtype;
    INT16 tgbase;

    // checks
    if (currentTagSelector==NULL) return (false);

    if (criteriaStack->count()>=MAX_HIGHLIGHTS)
    {
        QMessageBox::warning (NULL,"Highlighting capacity exceeded.","Unable to highlight last item.",
        QMessageBox::Ok ,QMessageBox::NoButton);
        return (false);
    }

    // get clicked-event tag value
    ItemHandler temp = * hnd;
    Q_ASSERT(temp.isValidItemHandler());
    if(!temp.getTagByName(currentTagSelector,&tgvalue,&tgvtype,&tgbase,&tgwhen,col))
    {
        QMessageBox::warning (NULL,"Scanning error","Unable to find TAG "+currentTagSelector+
        " on ItemID "+QString::number(hnd->getItemId())+".",
        QMessageBox::Ok ,QMessageBox::NoButton);
        return (false);
    }

    if (!adding)
    {
        purgeLast();
    }

    // build a new "highlight criteria" object
    HighLightType type;
    if (htype==HLT_DEFAULT)
    {
        type = popFreeType();
    }
    else
    {
        type = htype;
    }

    HighLightCriteria newCriteria(temp.getTagId(), tgvalue,type);
    criteriaStack->push(newCriteria);
    //printf ("@@@ \t criteria created and stacked id=%d,value=%d \n",tagId,tgvalue);fflush(stdout);

    // force view updating
    last_x1=0x7fffffff;
    last_x2=0;
    last_y1=0x7fffffff;
    last_y2=0;

    emit highlightChanged(criteriaStack->count());

    return (true);
}

bool
HighLightMgr::contentsSelectEvent(int col,int row, bool adding,
              QString currentTagSelector, HighLightType htype,bool noErase)
{
    /// @todo review this
    if (myDB->getRowType(row) != SimpleEdgeRow) { return false;}

    //printf ("!!! contentsSelectEvent called on col=%d,row=%d,tag=%s\n",col,row,currentTagSelector.latin1());fflush(stdout);fflush(stderr);
    // checks
     if (! myDB->hasValidData(col,row) )
     {
         //printf ("\t...ops, no data!>\n");fflush(stdout);fflush(stderr);
         return (false);
     }

    // check if <col,row> is already highlighted
    int stackIdx = lookForEvent(col,row);
    if (stackIdx>=0)
    {
        if (noErase)
        {
            //printf ("\t already highlighted, no erase!\n");fflush(stdout);fflush(stderr);
            return(false);
        }
        else
        {
             // in this case we un-highlight such criteria
            purgeLayer(stackIdx);
            return (true);
        }
    }

     // tag heap entry for clicked event:
     ItemHandler hnd;
     draldb->getMoveItem(&hnd, myDB->getRowEdgeId(row),col,myDB->getRowEdgePos(row));
     return contentsSelectEvent(&hnd,adding,currentTagSelector,htype,noErase,col);
}

void  
HighLightMgr::updateViewRange (int x1, int x2, int y1, int y2)
{
    if (criteriaStack->count()<=0) return;

    //printf ("updateViewRange over: (%d,%d)->(%d,%d) \n",x1,y1,x2,y2);

    bool c1 = (last_x1>x1);
    bool c2 = (last_x2<x2);
    bool c3 = (last_y1>y1);
    bool c4 = (last_y2<y2);
    if ( !(c1 || c2 || c3 || c4) ) return;

    //printf ("\t update efective.. \n");fflush(stdout);fflush(stderr);

    last_x1=x1;
    last_x2=x2;
    last_y1=y1;
    last_y2=y2;

    int i,j;

    // iterate through criteria list
    HLCriteriaStack::iterator it;
    for (it = criteriaStack->begin(); it != criteriaStack->end(); ++it )
    {
        HighLightCriteria criteria = *it;
        criteria.eventList->clear();
        for (i=x1; i<=x2; i++)
        {
            for (j=y1; j<=y2; j++)
            {
                if (myDB->matchTagIdValue(i,j,criteria.tagId,criteria.tagValue))
                {
                    ItemHandler hnd;
                    draldb->getMoveItem(&hnd, myDB->getRowEdgeId(j),i,myDB->getRowEdgePos(j));
                    addHighLight (i,j,criteria.type,criteria.eventList,&hnd);
                }
            }
        }
    }
}


// remove the "last level" on highlighting
bool 
HighLightMgr::purgeLast()
{
    //printf ("purgeLast called\n");

    if (criteriaStack->count()<=0) return (false);

    // force updating
    last_x1=0x7fffffff;
    last_x2=0;
    last_y1=0x7fffffff;
    last_y2=0;

    HighLightCriteria criteria = criteriaStack->pop();

    pushFreeType(criteria.type);
    emit highlightChanged(criteriaStack->count());

    return (true);
}

// remove the "given level" of highlighting
bool 
HighLightMgr::purgeLayer(int layer)
{
    if (layer>=(int)criteriaStack->count()) return (false);

    // force updating
    last_x1=0x7fffffff;
    last_x2=0;
    last_y1=0x7fffffff;
    last_y2=0;

    HLCriteriaStack::iterator it = criteriaStack->at(layer);
    HighLightCriteria criteria = *it;
    pushFreeType(criteria.type);
    criteriaStack->remove(it);
    emit highlightChanged(criteriaStack->count());
    return (true);
}

// remove the all levels of highlighting
bool 
HighLightMgr::purgeAll()
{
    if (criteriaStack->count()<=0) return (false);
    purge();

    // force updating
    last_x1=0x7fffffff;
    last_x2=0;
    last_y1=0x7fffffff;
    last_y2=0;
    emit highlightChanged(criteriaStack->count());
    return (true);
}

void 
HighLightMgr::showCriteria()
{
    static const int cube_icon_sz = 20;

    // fill qlv with current criteria stack
    qlv->clear();

    QPixmap colorPixmap(cube_icon_sz,cube_icon_sz);
    QPainter p(&colorPixmap);

    HLCriteriaStack::iterator it;
    for (it = criteriaStack->begin(); it != criteriaStack->end(); ++it )
    {
        QString tagName;
        QString formatedTagValue;
        QListViewItem* item = new QListViewItem(qlv);Q_ASSERT(item!=NULL);

        HighLightCriteria criteria = *it;
        tagName = draldb->getTagDescription(criteria.tagId);
        formatedTagValue = draldb->getFormatedTagValue(criteria.tagId,criteria.tagValue);

        colorPixmap.fill(hcolors[criteria.type-1]);
        p.drawRect(0,0,cube_icon_sz,cube_icon_sz);

        item->setPixmap(0,colorPixmap);
        item->setText(1,tagName);
        item->setText(2,formatedTagValue);

        qlv->insertItem(item);
    }

    hcd->exec();
}

INT32 
HighLightMgr::lookForEvent(int col, int row)
{
    int cnt = 0;
    int pos = -1;
    bool fnd = false;
    HLCriteriaStack::iterator it        = criteriaStack->begin();
    HLCriteriaStack::iterator it_end    = criteriaStack->end();

    HighLightCriteria criteria;

    while ( !fnd && (it != it_end) )
    {
        criteria = *it;

        // look for the given <col,row> in the list
        HLightList* eventList = (criteria.eventList);
        HLightList::iterator iter = eventList->begin();
        HLightList::iterator iter_end = eventList->end();

        while ( !fnd && (iter != iter_end) )
        {
            HighLightListItem item = *iter;
            fnd = (item.col == col) && (item.row==row);
            ++iter;
        }
        ++it;
        ++cnt;
    }

    if (fnd)
    {
        pos = cnt-1;
    }

    return (pos);
}

INT32
HighLightMgr::lookForEvent(ItemHandler * hnd)
{
    //printf (">>> HighLightMgr::lookForEvent on %d\n",itemIdx);

    int cnt = 0;
    int pos = -1;
    bool fnd = false;
    HLCriteriaStack::iterator it        = criteriaStack->begin();
    HLCriteriaStack::iterator it_end    = criteriaStack->end();

    HighLightCriteria criteria;

    while ( !fnd && (it != it_end) )
    {
        //printf("*** searching in criteria %d\n",cnt);
        criteria = *it;

        // look for the given <col,row> in the list
        HLightList* eventList = (criteria.eventList);
        HLightList::iterator iter = eventList->begin();
        HLightList::iterator iter_end = eventList->end();

        while ( !fnd && (iter != iter_end) )
        {
            HighLightListItem item = *iter;
            //printf("\t checking element %d, looking for %d \n",item.itemIdx,itemIdx);
            fnd = (item.handler == * hnd);
            ++iter;
        }
        ++it;
        ++cnt;
    }

    if (fnd)
    {
        pos = cnt-1;
    }

    return (pos);
}

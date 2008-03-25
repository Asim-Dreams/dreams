/*
 * Copyright (C) 2005-2006 Intel Corporation
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

// General includes.
#include <iostream>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "dialogs/highlightmgr/HighLightMgrDialogImpl.h"
#include "DreamsDB.h"
#include "HighLightMgr.h"
#include "PreferenceMgr.h"

// Qt includes.
#include <qmessagebox.h>
#include <qlistview.h>
#include <qpixmap.h>

HighLightMgr::HighLightMgr(DreamsDB * dreamsdb, INT32 cside)
{
    init(dreamsdb, cside);

    criteriaStack = new HLCriteriaStack();
    hltypeStack = new HLFreeTypeStack();
    initFreeStack();
}

HighLightMgr::HighLightMgr(HighLightMgr* parent)
{
    init(parent->dreamsdb, parent->cube_side);

    // copy state
    last_x1 = parent->last_x1;
    last_x2 = parent->last_x2;
    last_y1 = parent->last_y1;
    last_y2 = parent->last_y2;

    criteriaStack = new HLCriteriaStack(*(parent->criteriaStack));
    hltypeStack = new HLFreeTypeStack(*(parent->hltypeStack));
}

void 
HighLightMgr::init(DreamsDB * _dreamsdb, INT32 cside)
{
    cube_side = cside;
    //nextHType = HLT_TYPE1;
    last_x1 = 0x7fffffff;
    last_x2 = 0;
    last_y1 = 0x7fffffff;
    last_y2 = 0;

    // right now use hardcoded colors
    hcolors[0] = QColor(246, 210,   8);
    hcolors[1] = QColor(176, 246,  96);
    hcolors[2] = QColor(101, 255, 252);
    hcolors[3] = QColor(255, 134, 139);
    hcolors[4] = QColor(213, 202, 255);
    hcolors[5] = QColor( 30, 255,   0);
    hcolors[6] = QColor(255,   2, 247);
    hcolors[7] = QColor(255, 255, 255);

    // pre-build dialog
    hcd = new HighLightMgrDialogImpl(NULL, "hcd", true);
    Q_ASSERT(hcd!=NULL);
    qlv = hcd->getListView();

    for(INT32 i = 0; i < MAX_HIGHLIGHTS; i++)
    {
        hpens[i] = QPen(hcolors[i], PreferenceMgr::getInstance()->getHighlightSize());
    }

    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();
}

HighLightMgr::~HighLightMgr()
{
    delete criteriaStack;
    delete hltypeStack;
    delete hcd;
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

/**
 * Method that updates the hightlight criteria with the value of the tag currentTagSelector
 * for the item hnd.
 *
 * @param hnd Pointer to the item selected
 * @param adding Boolean that indicates if we add the current selection to the stack or
 *               substitutes the last selected element
 * @param currentTagSelector Current selector tag name. With this value in conjuction with the 
 *        value of the tag specify the highlight rule
 * @param htype Highlight color. Indicates if the next color is selected or the specifyed is used
 * @out tagValue Output param. In this param the value of the current item is returned. This value
 *               is used to notify the rest of the syncronized windows in order to highlignt the same elements
 * @param noErase
 * @param col Cycle where the selection is done
 *
 * @return If a repaint is needed
 */
bool
HighLightMgr::contentsSelectEvent(ItemHandler * hnd, bool adding, QString currentTagSelector, HighLightType htype, bool noErase, CYCLE cycle)
{
    CYCLE tgwhen;
    TagValueType tgvtype;
    TagValueBase tgbase;

    // checks
    if(currentTagSelector == NULL)
    {
        return false;
    }

    if(criteriaStack->count() >= MAX_HIGHLIGHTS)
    {
        QMessageBox::warning(NULL," Highlighting capacity exceeded.", "Unable to highlight last item.", QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }

    // get clicked-event tag value
    UINT64 tagValue;
    ItemHandler temp = * hnd;
    Q_ASSERT(temp.isValidItemHandler());

    if(!temp.getTagByName(currentTagSelector, &tagValue, &tgvtype, &tgbase, &tgwhen, cycle))
    {
        QMessageBox::warning(NULL, "Scanning error", "Unable to find TAG " + currentTagSelector + " on ItemID " + QString::number(hnd->getItemId()) + ".", QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }

    // check if crtiteria is already highlighted
    TAG_ID tagId = temp.getTagId();
    int stackIdx = getMatchingCriteria(tagId, tagValue);
    if(stackIdx >= 0)
    {
        if(noErase)
        {
            return false;
        }
        else
        {
             // in this case we un-highlight such criteria
            purgeLayer(stackIdx, true);
            return true;
        }
    }
    if(!adding)
    {
        purgeLast();
        emit purgeLastHighlight();
    }

    // build a new "highlight criteria" object
    HighLightType type;
    if(htype == HLT_DEFAULT)
    {
        type = popFreeType();
    }
    else
    {
        type = htype;
    }

    //cerr << "HighLightMgr::contentsSelectEvent: Createing new Criteria: tagId = " << tagId << " tagValue = " << tagValue << endl;
    HighLightCriteria newCriteria(tagId, tagValue, type);
    criteriaStack->push(newCriteria);
    //printf ("@@@ \t criteria created and stacked id=%d,value=%d \n",tagId,tgvalue);fflush(stdout);

    // force view updating
    last_x1 = 0x7fffffff;
    last_x2 = 0;
    last_y1 = 0x7fffffff;
    last_y2 = 0;

    emit highlightChanged(criteriaStack->count());
    emit contentsSelectedEvent(tagId, tagValue, adding, htype, true);

    return true;
}

bool 
HighLightMgr::contentsSelectEvent(TAG_ID tagId, UINT64 tagValue, bool adding, HighLightType htype, bool noErase, bool reemit)
{ 
    // checks
    if (criteriaStack->count()>=MAX_HIGHLIGHTS)
    {
        QMessageBox::warning (NULL,"Highlighting capacity exceeded.","Unable to highlight last item.",
        QMessageBox::Ok ,QMessageBox::NoButton);
        return (false);
    }

    // check if crtiteria is already highlighted
    int stackIdx = getMatchingCriteria(tagId, tagValue);
    if (stackIdx >= 0)
    {
        HighLightCriteria criteria = getCriteriaAt(stackIdx);
        if (noErase)
        {
            //cerr << "HighLightMgr::contentsSelectEvent: Sortim perque ja esta seleccionat" << endl;
            return(false);
        }
        else
        {
             // in this case we un-highlight such criteria
            purgeLayer(stackIdx, reemit);
            return (true);
        }
    }

    if (!adding)
    {
        purgeLast();
        if(reemit)
        {
            emit purgeLastHighlight();
        }
    }

    // build a new "highlight criteria" object
    HighLightType type;
    if(htype == HLT_DEFAULT)
    {
        type = popFreeType();
    }
    else
    {
        type = htype;
    }

    HighLightCriteria newCriteria(tagId, tagValue, type);
    criteriaStack->push(newCriteria);

    // force view updating
    last_x1=0x7fffffff;
    last_x2=0;
    last_y1=0x7fffffff;
    last_y2=0;

    if(reemit)
    {
        emit highlightChanged(criteriaStack->count());
        emit contentsSelectedEvent(tagId, tagValue, adding, htype, true);
    }

    return true; 
}

// remove the "last level" on highlighting
bool 
HighLightMgr::purgeLast()
{

    if (criteriaStack->count() <= 0) return false;

    // force updating
    last_x1=0x7fffffff;
    last_x2=0;
    last_y1=0x7fffffff;
    last_y2=0;

    HighLightCriteria criteria = criteriaStack->pop();

    pushFreeType(criteria.type);

    emit highlightChanged(criteriaStack->count());

    return true;
}

// remove the "given level" of highlighting
bool 
HighLightMgr::purgeLayer(int layer, bool reemit)
{
    if(layer >= (int) criteriaStack->count())
    {
        return (false);
    }

    // force updating
    last_x1=0x7fffffff;
    last_x2=0;
    last_y1=0x7fffffff;
    last_y2=0;

    HLCriteriaStack::iterator it = criteriaStack->at(layer);
    HighLightCriteria criteria = *it;
    pushFreeType(criteria.type);
    criteriaStack->remove(it);
    if(reemit)
    {
        emit highlightChanged(criteriaStack->count());
        emit unhighlightCriteriaSignal(criteria.tagId, criteria.tagValue);
    }
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

// remove the requested criteria
bool 
HighLightMgr::unhighlightCriteria(TAG_ID tagId, UINT64 tagValue)
{
    int stackIdx = getMatchingCriteria(tagId, tagValue);
    if(stackIdx >= 0)
    {
        // in this case we un-highlight such criteria
        purgeLayer(stackIdx, false);
        return true;
    }
    return false;
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
        tagName = draldb->getTagName(criteria.tagId);
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

void 
HighLightMgr::copyState(HighLightMgr * base)
{
    // copy state
    last_x1 = base->last_x1;
    last_x2 = base->last_x2;
    last_y1 = base->last_y1;
    last_y2 = base->last_y2;

    delete criteriaStack;
    delete hltypeStack;

    criteriaStack = new HLCriteriaStack(*(base->criteriaStack));
    hltypeStack = new HLFreeTypeStack(*(base->hltypeStack));
}

INT32
HighLightMgr::lookForEvent(CYCLE cycle, INT32 row)
{
    int cnt = 0;
    int pos = -1;
    bool fnd = false;
    HLCriteriaStack::iterator it = criteriaStack->begin();
    HLCriteriaStack::iterator it_end = criteriaStack->end();

    HighLightCriteria criteria;

    while(!fnd && (it != it_end))
    {
        criteria = * it;

        // look for the given <col,row> in the list
        HLightList * eventList = criteria.eventList;
        HLightList::iterator iter = eventList->begin();
        HLightList::iterator iter_end = eventList->end();

        while(!fnd && (iter != iter_end))
        {
            HighLightListItem item = * iter;
            fnd = ((item.cycle == cycle) && (item.row == row));
            ++iter;
        }
        ++it;
        ++cnt;
    }

    if(fnd)
    {
        pos = cnt - 1;
    }

    return pos;
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

INT32 
HighLightMgr::getMatchingCriteria(TAG_ID tagId, UINT64 tagValue)
{
    INT32 nPosition = 0;
    bool bFound = false;
    HLCriteriaStack::Iterator iter        = criteriaStack->begin();
    HLCriteriaStack::Iterator iter_end    = criteriaStack->end();
    while(!bFound && (iter != iter_end))
    {
        bFound = ((*iter).tagId == tagId) && ((*iter).tagValue == tagValue);
        nPosition++;
        iter++;
    }

    nPosition = bFound ? nPosition - 1 : -1;
    return nPosition;
}

void
HighLightMgr::changeSize(UINT32 size)
{
    for(INT32 i = 0; i < MAX_HIGHLIGHTS; i++)
    {
        hpens[i] = QPen(hcolors[i], size);
    }
}

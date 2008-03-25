/*
 * Copyright (C) 2004-2006 Intel Corporation
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
  * @file  ShowTags.cpp
  * @brief
  */

// General includes.
#include <iostream>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include <dDB/DralDB.h>
#include "ShowTags.h"

// Qt includes.
#include <qapplication.h>
#include <qpainter.h>
#include <qpen.h>

// Asim includes.
#include <asim/IconFactory.h>

INT32 TagListViewItem::cycle_col = 2;

/*
 * Creator of the class.
 *
 * @return new object.
 */
TagListViewItem::TagListViewItem(ItemHandler * hnd, TRACK_ID track_id, INT32 sort_cycle, QListView * parent, QString tag_name, QString value, QString cycle, QString phase, QString clock)
    : QListViewItem(parent, tag_name, value, cycle, phase, clock)
{
    my_hnd = * hnd;
    my_track_id = track_id;
    my_sort_cycle = sort_cycle;
}

/**
 * We paint in different colors the tags related to the slots (red) and the items (blue)
 */
void 
TagListViewItem::paintCell(QPainter * p, const QColorGroup & cg, int column, int width, int align)
{
    QColorGroup _cg(cg);
    QColor c = _cg.text();

    if(my_hnd.isValidItemHandler())
    {
        _cg.setColor(QColorGroup::Text, Qt::blue);
    }
    else
    {
        _cg.setColor(QColorGroup::Text, Qt::red);
    }

    QListViewItem::paintCell(p, _cg, column, width, align);

    _cg.setColor(QColorGroup::Text, c);
}

/*
 * Redefines the compare function for cycles
 *
 * @return new object.
 */
int
TagListViewItem::compare(QListViewItem * i, int col, bool ascending) const
{
    if(col == cycle_col)
    {
        TagListViewItem * j = (TagListViewItem *) i;
        return (my_sort_cycle - j->my_sort_cycle);
    }
    else
    {
        return key(col, ascending).compare(i->key(col, ascending));
    }
}

void
ShowTags::showItemTags(DralDB * draldb, QListView * list, ItemHandler * hnd, CYCLE cycle)
{
    UINT64 tgvalue;
    CYCLE tgwhen;
    TAG_ID tagId;
    TagValueType tgvtype;
    TagValueBase tgbase;
    UINT64 first_sort_cycle = draldb->getFirstCycle().toLCMCycles();

    if(!hnd->isValidItemHandler())
    {
        return;
    }

    ItemHandler temp;
    temp = * hnd;

    // For each tag get its value, type & so on
    for(UINT16 i = 0; i < draldb->getNumTags(); i++)
    {
        if(temp.getTagById(i, &tgvalue, &tgvtype, &tgbase, &tgwhen, cycle))
        {
            QString tagName = draldb->getTagName(i);
            QString tagFormatedValue = draldb->getFormatedTagValue(i, tgvalue);
            QString clockFormated = tgwhen.clock->getFormattedFreq(CLOCK_GHZ);
            QString divisionFormated;

            if(tgwhen.division == 0)
            {
                divisionFormated = "High";
            }
            else
            {
                divisionFormated = "Low";
            }
            INT32 sort_cycle = (INT32) (tgwhen.toLCMCycles() - first_sort_cycle);
            QListViewItem * item = new TagListViewItem(&temp, TRACK_ID_INVALID, sort_cycle, list, tagName, tagFormatedValue, QString::number((long int) tgwhen.cycle), divisionFormated, clockFormated);
            Q_ASSERT(item != NULL);
            if(temp.getTagIsMutable())
            {
                item->setPixmap(2,IconFactory::getInstance()->image20);
            }
        }
    }
}

QString
ShowTags::getItemTags(DralDB * draldb, ItemHandler * hnd, CYCLE cycle)
{
    UINT64 tgvalue;
    CYCLE tgwhen;
    UINT16 tagId;
    TagValueType tgvtype;
    TagValueBase tgbase;

    if(!hnd->isValidItemHandler())
    {
        return QString::null;
    }

    QString result ="";
    ItemHandler temp;

    temp = * hnd;
    // for each tag get its value, type & so on
    for(UINT16 i = 0; i < draldb->getNumTags(); i++)
    {
        if(temp.getTagById(i, &tgvalue, &tgvtype, &tgbase, &tgwhen, cycle))
        {
            QString tagName = draldb->getTagName(i);
            QString tagFormatedValue = draldb->getFormatedTagValue(i, tgvalue);
            result += tagName + ":\t"+ tagFormatedValue + "\n";
        }
    }

    return result;
}

void
ShowTags::showTrackTags(DralDB * draldb, QListView * list, TRACK_ID trackId, CYCLE cycle)
{
    // get a list of known tag-ids for this track
    TagIDList * idlist = draldb->getKnownTagIDs(trackId);
    int maxidx = idlist->getMaxUsedIdx();
    ItemHandler nullhnd;
    const ClockDomainEntry * clock;
    clock = draldb->getClockDomain(draldb->getClockId(trackId));
    QString clockFormated = clock->getFormattedFreq(CLOCK_GHZ);
    UINT64 first_sort_cycle = draldb->getFirstCycle().toLCMCycles();
    
    for(INT32 i = 0; i <= maxidx; i++)
    {
        CYCLE atcycle;
        QString fmtvalue;
        QString summarizedValue;
        TagListViewItem * item;

        bool gval = draldb->getFormatedTrackTagValue(trackId, idlist->at(i), cycle, &fmtvalue, &atcycle);
        if(gval)
        {
            QString desc = draldb->getTagName(idlist->at(i));

            // Hides internal tags.
            if(desc.startsWith("__color"))
            {
                continue;
            }

            QString divisionFormated;

            if(atcycle.division == 0)
            {
                divisionFormated = "High";
            }
            else
            {
                divisionFormated = "Low";
            }

            INT32 sort_cycle = (INT32) (atcycle.toLCMCycles() - first_sort_cycle);
            // if more than one line (\n) summarize and put indicator
            if(isMultipleLines(fmtvalue))
            {
                summarizedValue = summarizeMultiline(fmtvalue);
                item = new TagListViewItem(&nullhnd, trackId, sort_cycle, list, desc, summarizedValue, QString::number(atcycle.cycle), divisionFormated, clockFormated);
                item->setPixmap(1,IconFactory::getInstance()->image24);
                item->setLongStr(fmtvalue);
            }
            else
            {
                item = new TagListViewItem(&nullhnd, trackId, sort_cycle, list, desc, fmtvalue, QString::number(atcycle.cycle), divisionFormated, clockFormated);
            }
            Q_ASSERT(item != NULL);
        }
    }

    // Show all the tags of the contained item
    delete idlist;
    ItemHandler hnd;
    draldb->getItemInSlot(trackId, cycle, &hnd);
    showItemTags(draldb, list, &hnd, cycle);
}

QString
ShowTags::getTrackTags(DralDB * draldb, TRACK_ID trackId, CYCLE cycle)
{
    QString result="";
    
    // get a list of known tag-ids for this track
    TagIDList * idlist = draldb->getKnownTagIDs(trackId);
    int maxidx = idlist->getMaxUsedIdx();
    for(INT32 i = 0; i <= maxidx; i++)
    {
        CYCLE atcycle;
        QString fmtvalue;

        bool gval = draldb->getFormatedTrackTagValue(trackId, idlist->at(i), cycle, &fmtvalue,& atcycle);
        if(gval)
        {
            QString desc = draldb->getTagName(idlist->at(i));

            // Hides internal tags.
            if(desc.startsWith("__color"))
            {
                continue;
            }

            result = result + desc + ":\t"+ fmtvalue + "\n";
        }
    }

    delete idlist;
    return result;
}

bool 
ShowTags::isMultipleLines(QString str)
{
    return (str.contains ("\n") > 0);
}

QString
ShowTags::summarizeMultiline(QString str)
{
    QString result = str.left(str.find("\n",0));
    result += "...";
    return result;
}

void
ShowTags::showItemMutableTagValue(DralDB * draldb, QListView * list, ItemHandler * hnd)
{
    INT16 tgbase;
    UINT64 tgvalue;
    CYCLE tgwhen;
    TagValueType tgvtype;

    // rewind the handler
    ItemHandler temp;

    TAG_ID myTagId = hnd->getTagId();

    temp = * hnd;
    temp.rewindToFirstTag();

    while(temp.isValidTagHandler())
    {
        if(temp.getTagId() == myTagId)
        {
            QString tagFormatedValue;
            QString scycle;
            QString sdiv;
            CYCLE cycle;

            tagFormatedValue = temp.getTagFormatedValue();
            cycle = temp.getTagCycle();
            scycle = QString::number((long) cycle.cycle);
            scycle = scycle.rightJustify(10, ' ');
            if(cycle.division == 0)
            {
                sdiv = "High";
            }
            else
            {
                sdiv = "Low";
            }
            sdiv = sdiv.rightJustify(10, ' ');

            QListViewItem * item = new QListViewItem(list, tagFormatedValue, scycle, sdiv, cycle.clock->getFormattedFreq(CLOCK_GHZ));
            Q_ASSERT(item != NULL);
        }
        temp.nextTag();
    }
}

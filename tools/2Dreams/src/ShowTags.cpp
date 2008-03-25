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
  * @file  ShowTags.cpp
  * @brief
  */

#include <qapplication.h>
#include <asim/DralDB.h>
#include <asim/IconFactory.h>

#include "ShowTags.h"

TagListViewItem::TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListView * parent ) :
                 QListViewItem (parent), myhnd(* hnd), myTrackId(trackId) {};

TagListViewItem::TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListViewItem * parent ) :
                 QListViewItem (parent), myhnd(* hnd), myTrackId(trackId) {};

TagListViewItem::TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListView * parent, QListViewItem * after ) :
                 QListViewItem (parent,after), myhnd(* hnd), myTrackId(trackId) {};

TagListViewItem::TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListViewItem * parent, QListViewItem * after ) :
                 QListViewItem (parent,after), myhnd(* hnd), myTrackId(trackId) {};

TagListViewItem::TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListView * parent, QString label1, QString label2  , QString label3  , QString label4  , QString label5  , QString label6  , QString label7  , QString label8   ) :
                 QListViewItem (parent,label1,label2,label3,label4,label5,label6,label7,label8), myhnd(* hnd), myTrackId(trackId) {};

TagListViewItem::TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListViewItem * parent, QString label1, QString label2  , QString label3  , QString label4  , QString label5  , QString label6  , QString label7  , QString label8   ) :
                 QListViewItem (parent,label1,label2,label3,label4,label5,label6,label7,label8), myhnd(* hnd), myTrackId(trackId) {};

TagListViewItem::TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListView * parent, QListViewItem * after, QString label1, QString label2  , QString label3  , QString label4  , QString label5  , QString label6  , QString label7  , QString label8   ) :
                 QListViewItem (parent,after,label1,label2,label3,label4,label5,label6,label7,label8), myhnd(* hnd), myTrackId(trackId) {};

TagListViewItem::TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListViewItem * parent, QListViewItem * after, QString label1, QString label2  , QString label3  , QString label4  , QString label5  , QString label6  , QString label7  , QString label8   ) :
                 QListViewItem (parent,after,label1,label2,label3,label4,label5,label6,label7,label8), myhnd(* hnd), myTrackId(trackId) {};

void
ShowTags::showItemTags(QListView* list, ItemHandler * hnd, INT32 cycle)
{
    //printf("ShowTags::showItemTags called on idx=%d,cycle=%d\n",idx,cycle);
    DralDB* draldb = DralDB::getInstance();
    INT16  tgbase;
    UINT64 tgvalue;
    UINT64 tgwhen;
    UINT16 tagId;
    TagValueType tgvtype;

    if (!hnd->isValidItemHandler()) return;

    ItemHandler temp;
    temp = * hnd;
    temp.goToFirstTag();
    // for each tag get its value, type & so on
    bool goon=false;
    do
    {
        bool ok = temp.getCurrentTagValue(&tagId, &tgvalue,&tgvtype,&tgbase,&tgwhen,cycle);
        if (ok)
        {
            QString tagName = draldb->getTagDescription(tagId);
            QString tagFormatedValue = temp.getFormatedTagValue();
            QListViewItem* item = new TagListViewItem(&temp,-1,list,tagName,
                           tagFormatedValue,QString::number((long int)tgwhen));
            Q_ASSERT(item!=NULL);
            if (temp.isMutableTag())
            {
                item->setPixmap(2,IconFactory::getInstance()->image20);
            }
    
            if (temp.isSOVTag())
            {
                goon = temp.skipToNextEntry();
            }
            else
            {
                goon = temp.skipToNextTag();
            }

            //printf("added %s with value %s\n",tagName.latin1(),tagFormatedValue.latin1());
        }
        else
        {
			QString err = "Error getting tgId=";
			err += QString::number((int)tagId);
			err += ",cycle=" + QString::number(cycle);
			//qApp->beep();
			LogMgr::getInstance()->addLog(err);
        }
    } while (goon);
}

QString
ShowTags::getItemTags(ItemHandler * hnd, INT32 cycle)
{
    //printf("ShowTags::showItemTags called on idx=%d,cycle=%d\n",idx,cycle);
    DralDB* draldb = DralDB::getInstance();
    INT16  tgbase;
    UINT64 tgvalue;
    UINT64 tgwhen;
    UINT16 tagId;
    TagValueType tgvtype;

    if (! hnd->isValidItemHandler()) return QString::null;

    QString result ="";
    ItemHandler temp;

    temp = * hnd;
    // for each tag get its value, type & so on
    bool goon=false;
    do
    {
        bool ok = temp.getCurrentTagValue(&tagId, &tgvalue,&tgvtype,&tgbase,&tgwhen,cycle);
        if (ok)
        {
            QString tagName = draldb->getTagDescription(tagId);
            QString tagFormatedValue = temp.getFormatedTagValue();

            result += tagName + ":\t"+ tagFormatedValue + "\n";
    
            if (temp.isSOVTag())
            {
                goon = temp.skipToNextEntry();
            }
            else
            {
                goon = temp.skipToNextTag();
            }
        }
        else
        {
			QString err = "Error getting tgId=";
			err += QString::number((int)tagId);
			err += ",cycle=" + QString::number(cycle);
			LogMgr::getInstance()->addLog(err);
        }
    } while (goon);
    
    return result;
}

void
ShowTags::showItemTags(QListView* list, INT32 trackId, INT32 cycle)
{
    //printf("ShowTags::showItemTags called on idx=%d,cycle=%d\n",idx,cycle);
    DralDB* draldb = DralDB::getInstance();

    // get a list of known tag-ids for this track
    TagIDList* idlist = draldb->getKnownTagIDs(trackId);
    int maxidx = idlist->getMaxUsedIdx();
    ItemHandler nullhnd;
    
    for (int i=0;i<=maxidx;i++)
    {
		UINT32 atcycle;
        QString fmtvalue;
        QString summarizedValue;
        TagListViewItem* item;
        bool gval = draldb->getFormatedTrackTagValue(trackId,idlist->at(i),cycle,&fmtvalue,&atcycle);
        if (gval)
        {
            // if more than one line (\n) summarize and put indicator
            if (isMultipleLines(fmtvalue))
            {
                summarizedValue = summarizeMultiline(fmtvalue);
                item = new TagListViewItem(&nullhnd,trackId,list,draldb->getTagDescription(idlist->at(i)),summarizedValue,QString::number(atcycle));
                item->setPixmap(1,IconFactory::getInstance()->image24);
                item->setLongStr(fmtvalue);
            }
            else
            {
                item = new TagListViewItem(&nullhnd,trackId,list,draldb->getTagDescription(idlist->at(i)),fmtvalue,QString::number(atcycle));
            }
            Q_ASSERT(item!=NULL);
        }
    }
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


QString
ShowTags::getItemTags(INT32 trackId, INT32 cycle)
{
    //printf("ShowTags::showItemTags called on idx=%d,cycle=%d\n",idx,cycle);
    DralDB* draldb = DralDB::getInstance();
    QString result="";
    
    // get a list of known tag-ids for this track
    TagIDList* idlist = draldb->getKnownTagIDs(trackId);
    int maxidx = idlist->getMaxUsedIdx();
    for (int i=0;i<=maxidx;i++)
    {
		UINT32 atcycle;
        QString fmtvalue;
        bool gval = draldb->getFormatedTrackTagValue(trackId,idlist->at(i),cycle,&fmtvalue,&atcycle);
        if (gval)
        {
            result = result + draldb->getTagDescription(idlist->at(i)) + ":\t"+ fmtvalue + "\n";
        }
    }
    return result;
}

void
ShowTags::showItemMutableTagValue(QListView* list, ItemHandler * hnd)
{
    //printf("ShowTags::showItemMutableTagValue called on tagidx=%d\n",tagidx);

    INT16 tgbase;
    UINT64 tgvalue;
    UINT64 tgwhen;
    TagValueType tgvtype;

    // rewind the handler
    ItemHandler temp;

    temp = * hnd;
    temp.rewindToFirstTagValue();
    UINT16 myTagId = temp.getTagId();
    bool eol;
    do
    {
        QString tagFormatedValue = temp.getFormatedTagValue();
        QString scycle = QString::number((long)temp.getCurrentTagCycle());
        scycle = scycle.rightJustify(10,' ');
        QListViewItem* item = new QListViewItem(list,scycle,tagFormatedValue);
        Q_ASSERT(item!=NULL);
        eol = !temp.skipToNextEntry();
    } while ( !eol && temp.isMutableTag() && (temp.getTagId()==myTagId) );
}

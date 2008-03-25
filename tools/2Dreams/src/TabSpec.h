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
  * @file  TabSpec.h
  */

#ifndef _TABSPEC_H
#define _TABSPEC_H

// QT library
#include <qstring.h>
#include <qcolor.h>
#include <qptrlist.h>
#include <qstringlist.h>

#include "asim/DralDB.h"

#include "ADFDefinitions.h"

/**
  * @typedef TabRule
  * brief
  */
class TabSpec
{
    public:
        TabSpec(QString tname);
        ~TabSpec();

        bool addShowTag(QString);

        void addFontRule(INT32 tagId, UINT64 tagValue,INT32 dfsNum,QColor color, bool bold=false, bool underline=false,bool italic=false);
        inline void addFontRule (TagValueItemWinFont rule) { iwfrlist->append(rule); }

        inline void setDefaultFont(ItemWinFont v) { defaultItemWinFont=v;}
        inline ItemWinFont getDefaultFont() { return defaultItemWinFont; }
        inline void setDefaultFontDFS(INT32 v) { defaultItemWinFontDFSNum = v; }
        inline INT32 getDefaultFontDFS() { return defaultItemWinFontDFSNum; }

        inline bool isSplit() {return isSplitBy; }

        inline bool hasKnownValue(QString str) { return (knownValues->findIndex(str)>=0); }
        inline bool addKnownValue(QString str);

        inline QString getName() { return tabName; }

        inline void setStrValue(QString v) { strValue = v;}
        inline void setSortBy(QString v) { sortBy = v;}
        inline void setWithTag(QString v) { withTag = v;}

        inline void setSelectTag(QString v) { selectTag = v;}
        inline void setIsSplit(bool v) { isSplitBy =v; }

        inline QString getTabName() { return tabName; }
        inline QString getStrValue() { return strValue; }
        inline QString getSortBy() { return sortBy; }
        inline QString getWithTag() { return withTag; }
        inline QString getSelectTag() { return selectTag; }
        inline QStringList* getShowList() { return showList; }
        inline ItemWinFontRuleList* getFontRuleList() { return iwfrlist; }

        inline UINT64 getMatchingValue() { return matchValue; }
        inline void setMatchingValue(UINT64 v) { matchValue=v; }

    protected:
        QString      tabName;
        QStringList* showList;
        QString      sortBy;   ///< null means first item of showList

        QString     withTag;
        QString     selectTag;
        QString     strValue;
        UINT64      matchValue;

        bool         isSplitBy;
        QStringList* knownValues;

        ItemWinFont defaultItemWinFont;       // default (no if) font for THIS tab
        INT32       defaultItemWinFontDFSNum;

        ItemWinFontRuleList* iwfrlist;        // if font rules for this tab
};

bool
TabSpec::addKnownValue(QString str)
{
    if (knownValues->findIndex(str)>=0)
    {
        return false;
    }
    else
    {
        knownValues->append(str);
        return (true);
    }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

typedef QPtrList<TabSpec> TabSpecList;


#endif


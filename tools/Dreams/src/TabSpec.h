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
  * @file  TabSpec.h
  */

#ifndef _TABSPEC_H
#define _TABSPEC_H

// Dreams includes.
#include "DreamsDefs.h"
#include "adf/ADFDefinitions.h"
#include "expressions/Expression.h"

// Qt library.
#include <qstring.h>
#include <qcolor.h>
#include <qptrlist.h>
#include <qstringlist.h>

// Class forwarding.
class ItemHandler;
class DralDB;

/**
  * @typedef TabRule
  * brief
  */
class TabSpec
{
    public:
        TabSpec(QString tname, DralDB * _draldb);
        ~TabSpec();

        bool addShowTag(QString);
        void addFontRules(ExpressionList & lrules);

        inline bool isSplit() const;

        inline bool hasKnownValue(QString str) const;
        inline bool addKnownValue(QString str);

        inline QString getName() const;

        inline void setStrValue(QString v);
        void setSortBy(QString v);
        void setWithTag(QString v);
        void setSelectTag(QString v);
        inline void setIsSplit(bool v);

        inline QString getTabName() const;
        inline QString getStrValue() const;
        inline QString getSortBy() const;
        inline TAG_ID getSortById() const;
        inline QString getWithTag() const;
        inline TAG_ID getWithTagId() const;
        inline QString getSelectTag() const;
        inline TAG_ID getSelectTagId() const;
        inline QStringList * getShowList() const;

        inline UINT64 getMatchingValue() const;
        inline void setMatchingValue(UINT64 v);

        void evalItemWinFontProperties(ItemHandler * hnd, CSLData * data);

    protected:
        QString       tabName;
        QStringList * showList;

        QString sortBy;
        TAG_ID  sortById;
        QString withTag;
        TAG_ID  withTagId;
        QString selectTag;
        TAG_ID  selectTagId;
        QString strValue;
        UINT64  matchValue;

        bool         isSplitBy;
        QStringList* knownValues;

        ExpressionList rules; ///< Rules for this tab.
        RuleEvalEnv env;      ///< Rule evaluation evironment.
        DralDB * draldb;      ///< Pointer to the draldb;
} ;

bool
TabSpec::isSplit() const
{
    return isSplitBy;
}

bool
TabSpec::hasKnownValue(QString str) const
{
    return (knownValues->findIndex(str) >= 0);
}

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

QString
TabSpec::getName() const
{
    return tabName;
}

void
TabSpec::setStrValue(QString v)
{
    strValue = v;
}

void
TabSpec::setIsSplit(bool v)
{
    isSplitBy = v;
}

QString
TabSpec::getTabName() const
{
    return tabName;
}

QString
TabSpec::getStrValue() const
{
    return strValue;
}

QString
TabSpec::getSortBy() const
{
    return sortBy;
}

TAG_ID
TabSpec::getSortById() const
{
    return sortById;
}

QString
TabSpec::getWithTag() const
{
    return withTag;
}

TAG_ID
TabSpec::getWithTagId() const
{
    return withTagId;
}

QString
TabSpec::getSelectTag() const
{
    return selectTag;
}

TAG_ID
TabSpec::getSelectTagId() const
{
    return selectTagId;
}

QStringList *
TabSpec::getShowList() const
{
    return showList;
}

UINT64
TabSpec::getMatchingValue() const
{
    return matchValue;
}

void
TabSpec::setMatchingValue(UINT64 v)
{
    matchValue = v;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

typedef QPtrList<TabSpec> TabSpecList;

#endif

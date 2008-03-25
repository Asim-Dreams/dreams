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
  * @file TabSpec.cpp
  */

// 2Dreams includes
#include "TabSpec.h"

TabSpec::TabSpec(QString tname)
{
    tabName = tname;

    showList = new QStringList();
    knownValues = new QStringList();
    iwfrlist = new ItemWinFontRuleList();
    Q_ASSERT(iwfrlist);

    sortBy = QString::null;
    withTag = QString::null;
    selectTag = QString::null;
    strValue = QString::null;

    isSplitBy = false;
    defaultItemWinFontDFSNum=-1;

    defaultItemWinFont.bold = false;
    defaultItemWinFont.underline = false;
    defaultItemWinFont.italic = false;
    defaultItemWinFont.color = QColor(0,0,0);
}

TabSpec::~TabSpec()
{
    delete iwfrlist;
    delete showList;
    delete knownValues;
}

bool
TabSpec::addShowTag(QString tag)
{
    // check if is already there
    if (showList->findIndex(tag)>=0)
    {
        return (false);
    }
    showList->append(tag);
    return (true);
}

void
TabSpec::addFontRule(INT32 tagId, UINT64 tagValue,INT32 dfsNum,QColor color, bool bold, bool underline,bool italic)
{
    TagValueItemWinFont fr;

    fr.tagId    = tagId;
    fr.tagValue = tagValue;
    fr.dfsNum   = dfsNum;
    fr.fontSpec.bold      = bold;
    fr.fontSpec.underline = underline;
    fr.fontSpec.italic    = italic;
    fr.fontSpec.color     = color;

    iwfrlist->append(fr);
}


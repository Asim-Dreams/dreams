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
  * @file TabSpec.cpp
  */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "dDB/itemheap/ItemHeap.h"
#include "expressions/RuleEngine.h"
#include "TabSpec.h"

TabSpec::TabSpec(QString tname, DralDB * _draldb)
{
    //cerr << "TabSpec created for " << tname;

    tabName = tname;

    showList = new QStringList();
    knownValues = new QStringList();

    sortBy = QString::null;
    sortById = TAG_ID_INVALID;
    withTag = QString::null;
    withTagId = TAG_ID_INVALID;
    selectTag = QString::null;
    selectTagId = TAG_ID_INVALID;
    strValue = QString::null;

    isSplitBy = false;

    // Starts the environment.
    draldb = _draldb;
    env.cycle = -1;
    env.trackId = TRACK_ID_INVALID;
    env.itemTags.valids = NULL;
    env.itemTags.values = NULL;
    env.cycleTags.valids = NULL;
    env.cycleTags.values = NULL;
    env.nodeTags.valids = NULL;
    env.nodeTags.values = NULL;
    env.slotTags.valids = NULL;
    env.slotTags.values = NULL;
}

TabSpec::~TabSpec()
{
    if(env.itemTags.valids != NULL)
    {
        delete [] env.itemTags.valids;
        delete [] env.itemTags.values;
    }
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

/*
 * Appends a list of expression to the tab.
 *
 * @return void.
 */
void
TabSpec::addFontRules(ExpressionList & lrules)
{
    ExpressionIterator it_expr(lrules);
    Expression * expr;

    // First adds the rules to the already globally defined rules.
    while((expr = it_expr.current()) != NULL)
    {
        ++it_expr;
        rules.append(expr);
    }
}

/*
 * Evaluates the rules associated to this tab.
 *
 * @return void.
 */
void
TabSpec::evalItemWinFontProperties(ItemHandler * hnd, CSLData * data)
{
    TAG_ID tag_id;

    if(env.itemTags.valids == NULL)
    {
        env.itemTags.valids = new bool[draldb->getNumTags()];
        env.itemTags.values = new UINT64[draldb->getNumTags()];
    }

    // Rewinds the handler and resets the data.
    hnd->rewindToFirstTag();
    data->reset();
    env.cslResult.reset();
    memset(env.itemTags.valids, 0, draldb->getNumTags());

    // Gets all the values.
    while(hnd->isValidTagHandler())
    {
        // Caches the information.
        hnd->cacheTag();

        // Updates the state of the values.
        tag_id = hnd->getTagCachedId();
        env.itemTags.valids[tag_id] = hnd->getTagCachedDefined();
        env.itemTags.values[tag_id] = hnd->getTagCachedValue();
        hnd->nextTag();
    }

    // Evaluates the color and copies the result.
    RuleEngine::eval(&rules, &env);
    * data = env.cslResult;
}

void
TabSpec::setSortBy(QString v)
{
    sortBy = v;
    if(v != QString::null)
    {
        sortById = draldb->getTagId(v);
    }
    else
    {
        sortById = TAG_ID_INVALID;
    }
}

void
TabSpec::setWithTag(QString v)
{
    withTag = v;
    if(v != QString::null)
    {
        withTagId = draldb->getTagId(v);
    }
    else
    {
        withTagId = TAG_ID_INVALID;
    }
}

void
TabSpec::setSelectTag(QString v)
{
    selectTag = v;
    if(v != QString::null)
    {
        selectTagId = draldb->getTagId(v);
    }
    else
    {
        selectTagId = TAG_ID_INVALID;
    }
}

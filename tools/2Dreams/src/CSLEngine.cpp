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
  * @file CSLEngine.cpp
  */

#include "CSLEngine.h"

CSLEngine* CSLEngine::_myInstance = NULL;

CSLEngine*
CSLEngine::getInstance()
{
    if (_myInstance==NULL)
    {
        _myInstance = new CSLEngine();
    }

    Q_ASSERT(_myInstance!=NULL);
    return _myInstance;
}

void
CSLEngine::destroy()
{
    if (_myInstance!=NULL)
    {
        delete _myInstance;
    }
}

CSLEngine::CSLEngine()
{
    zdb = ZDBase::getInstance();
    cdb = ConfigDB::getInstance();
    ddb = DralDB::getInstance();
    log = LogMgr::getInstance();
    
    shadeWarnings = 0;
}

CSLEngine::~CSLEngine()
{
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// -- Warning!! this is time critical, make sure you know what you do
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void
CSLEngine::checkColorShapeRules(ColorRuleList* clist,ShapeRuleList* slist,ItemHandler * hnd,
          INT32* colorDfs, QColor* color,INT32* shapeDfs, EventShape* shape,INT32 cycle)
{
    bool goon = hnd->isValidTagHandler();
    Q_ASSERT(goon);

    //printf ("CSLEngine::checkColorRules() on list=0x%lx,tgIdx=%d,cycle=%d\n",list,tagHeapIdx,cycle);
    INT32 myColorDFS = *colorDfs;
    INT32 myShapeDFS = *shapeDfs;
    ItemHandler temp;

    temp = * hnd;

    while (goon)
    {
        UINT64 value;
        UINT16 tagId = temp.getTagId();
        bool fnd;
        bool gotValue=false;
        int i;

        // evaluate global color rules
        i = cdb->crlist->size() - 1;
        fnd = false;
        while ((i>=0) && (!fnd) )
        {
            TagValueColor* rule = cdb->crlist->ref(i);
            if (rule->dfsNum <= myColorDFS) {break;}
            if (tagId==rule->tagId)
            {
                //printf ("fnd rule for tgId=%d with value=%ul\n",tagId,rule.tagValue);
                if (!gotValue)
                {
                    //printf ("getting tag value...\n");
                    gotValue = temp.getCurrentTagValue(&value,cycle);
                    if (!gotValue) {break;}
                }
                if (value==rule->tagValue)
                {
                    myColorDFS   = rule->dfsNum;
                    *color = rule->color;
                    fnd    = true;
                    //printf ("match!\n");
                }
            }
            --i;
        }

        // evaluate local color rules
        if (clist!=NULL)
        {
            fnd = false;
            i = clist->size() - 1;
            while ((i>=0) && (!fnd) )
            {
                TagValueColor* rule = clist->ref(i);
                if (rule->dfsNum <= myColorDFS) {break;}
                if (tagId==rule->tagId)
                {
                    //printf ("fnd rule for tgId=%d with value=%ul\n",tagId,rule.tagValue);
                    if (!gotValue)
                    {
                        //printf ("getting tag value...\n");
                        gotValue = temp.getCurrentTagValue(&value,cycle);
                        if (!gotValue) {break;}
                    }
                    if (value==rule->tagValue)
                    {
                        myColorDFS   = rule->dfsNum;
                        *color = rule->color;
                        fnd    = true;
                        //printf ("match!\n");
                    }
                }
                --i;
            }
        }

        fnd = false;
        i = cdb->srlist->size() - 1;
        while ((i>=0) && (!fnd) )
        {
            TagValueShape* rule = cdb->srlist->ref(i);
            if (rule->dfsNum <= myShapeDFS) {break;}
            if (tagId==rule->tagId)
            {
                //printf ("fnd rule for tgId=%d with value=%ul\n",tagId,rule.tagValue);
                if (!gotValue)
                {
                    //printf ("getting tag value...\n");
                    gotValue = temp.getCurrentTagValue(&value,cycle);
                    if (!gotValue) {break;}
                }
                if (value==rule->tagValue)
                {
                    myShapeDFS   = rule->dfsNum;
                    *shape = rule->shape;
                    fnd    = true;
                    //printf ("match!\n");
                }
            }
            --i;
        }

        // evaluate local shape rules
        if (slist!=NULL)
        {
            fnd = false;
            i = slist->size() - 1;
            while ((i>=0) && (!fnd) )
            {
                TagValueShape* rule = slist->ref(i);
                if (rule->dfsNum <= myShapeDFS) {break;}
                if (tagId==rule->tagId)
                {
                    //printf ("fnd rule for tgId=%d with value=%ul\n",tagId,rule.tagValue);
                    if (!gotValue)
                    {
                        //printf ("getting tag value : %u\n", cycle);
                        gotValue = temp.getCurrentTagValue(&value,cycle);
                        if (!gotValue) {break;}
                    }
                    if (value==rule->tagValue)
                    {
                        myShapeDFS   = rule->dfsNum;
                        *shape = rule->shape;
                        fnd    = true;
                        //printf ("match!\n");
                    }
                }
                --i;
            }
        }

        // next tag to look at
        goon = temp.skipToNextTag();
    }
    *colorDfs = myColorDFS;
    *shapeDfs = myShapeDFS;
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// -- Warning!! this is time critical, make sure you know what you do
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void
CSLEngine::checkTrackColorRules(ColorRuleList* clist,INT32 trackId,INT32* colorDfs, QColor* color,
           bool* isTransp,INT32 cycle)
{
    UINT64 value;
    UINT16 tagId;
    INT32  i;
    bool fnd;

    INT32 myColorDFS = *colorDfs;

    //printf ("CSLEngine::checkTrackColorShapeRules colorDfs=%d,shapeDfs=%d,cycle=%d\n",myColorDFS,myShapeDFS,cycle);

    // evaluate global color rules
    i = cdb->crlist->size() - 1;
    fnd = false;
    while ((i>=0) && (!fnd) )
    {
        TagValueColor* rule = cdb->crlist->ref(i);
        if (rule->dfsNum <= myColorDFS) { break;}
        if (rule->isNodeTag)
        {
            bool gotVal = ddb->getTrackTagValue(trackId,rule->tagId,cycle,&value);
            //printf ("global shape rules: quering about tgId=%d,trackId=%d,cycle=%d, gotVal=%d\n",(int)rule->tagId,(int)trackId,(int)cycle,(int)gotVal);
            if (gotVal && (value==rule->tagValue))
            {
                myColorDFS = rule->dfsNum;
                *colorDfs  = myColorDFS;
                *color     = rule->color;
                *isTransp  = rule->isTransparentColor;
                fnd        = true;
            }
        }
        --i;
    }

    // evaluate local color rules
    if (clist!=NULL)
    {
        i = clist->size() - 1;
        fnd = false;
        while ((i>=0) && (!fnd) )
        {
            TagValueColor* rule = clist->ref(i);
            if (rule->dfsNum <= myColorDFS) {break;}
            if (rule->isNodeTag)
            {
                bool gotVal = ddb->getTrackTagValue(trackId,rule->tagId,cycle,&value);
                //printf ("local color rules: quering about tgId=%d,trackId=%d,cycle=%d, gotVal=%d\n",(int)rule->tagId,(int)trackId,(int)cycle,(int)gotVal);
                if (gotVal && (value == rule->tagValue) )
                {
                    myColorDFS = rule->dfsNum;
                    *colorDfs  = myColorDFS;
                    *color     = rule->color;
                    *isTransp  = rule->isTransparentColor;
                    fnd        = true;
                }
            }
            --i;
        }
    }
}

void
CSLEngine::checkTrackShapeRules(ShapeRuleList* slist,INT32 trackId,INT32* shapeDfs,EventShape* shape,INT32 cycle)
{
    UINT64 value;
    UINT16 tagId;
    INT32  i;
    bool fnd;

    INT32 myShapeDFS = *shapeDfs;

    //printf ("CSLEngine::checkTrackColorShapeRules colorDfs=%d,shapeDfs=%d,cycle=%d\n",myColorDFS,myShapeDFS,cycle);

    // evaluate global shape rules
    i = cdb->srlist->size() - 1;
    fnd = false;
    while ((i>=0) && (!fnd) )
    {
        TagValueShape* rule = cdb->srlist->ref(i);
        if (rule->dfsNum <= myShapeDFS) {break;}
        if (rule->isNodeTag)
        {
            bool gotVal = ddb->getTrackTagValue(trackId,rule->tagId,cycle,&value);
            //printf ("global shape rules: quering about tgId=%d,trackId=%d,cycle=%d, gotVal=%d\n",(int)rule->tagId,(int)trackId,(int)cycle,(int)gotVal);
            if (gotVal && (value==rule->tagValue))
            {
                myShapeDFS = rule->dfsNum;
                *shapeDfs = myShapeDFS;
                *shape     = rule->shape;
                fnd        = true;
            }
        }
        --i;
    }

    // evaluate local shape rules
    if (slist!=NULL)
    {
        i = slist->size() - 1;
        fnd = false;
        while ((i>=0) && (!fnd) )
        {
            TagValueShape* rule = slist->ref(i);
            if (rule->dfsNum <= myShapeDFS) {break;}
            if (rule->isNodeTag)
            {
                bool gotVal = ddb->getTrackTagValue(trackId,rule->tagId,cycle,&value);
                //printf ("local shape rules: quering about tgId=%d,trackId=%d,cycle=%d, gotVal=%d\n",(int)rule->tagId,(int)trackId,(int)cycle,(int)gotVal);
                if (gotVal && (value==rule->tagValue))
                {
                    myShapeDFS = rule->dfsNum;
                    *shapeDfs = myShapeDFS;
                    *shape     = rule->shape;
                    fnd        = true;
                }
            }
            --i;
        }
    }
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// -- Warning!! this is time critical, make sure you know what you do
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void
CSLEngine::checkItemFontRules(ItemWinFontRuleList* myFRList, ItemHandler * hnd,INT32* dfs, ItemWinFont& prop,INT32 cycle)
{
    bool goon = hnd->isValidTagHandler();
    Q_ASSERT(goon);

    //printf ("CSLEngine::checkColorRules() on list=0x%lx,tgIdx=%d,cycle=%d\n",list,tagHeapIdx,cycle);
    INT32 myDFS = *dfs;
    ItemHandler temp = * hnd;

    while (goon)
    {
        UINT64 value;
        UINT16 tagId = temp.getTagId();
        bool fnd;
        bool gotValue=false;
        int i;

        // evaluate global color rules
        i = cdb->iwfrlist->size() - 1;
        fnd = false;
        while ((i>=0) && (!fnd) )
        {
            TagValueItemWinFont* rule = cdb->iwfrlist->ref(i);
            if (rule->dfsNum <= myDFS) {break;}
            if (tagId==rule->tagId)
            {
                //printf ("fnd rule for tgId=%d with value=%ul\n",tagId,rule.tagValue);
                if (!gotValue)
                {
                    //printf ("getting tag value...\n");
                    gotValue = temp.getCurrentTagValue(&value,cycle);
                    if (!gotValue) {break;}
                }
                if (value==rule->tagValue)
                {
                    myDFS = rule->dfsNum;
                    prop = rule->fontSpec;
                    fnd    = true;
                }
            }
            --i;
        }

        // evaluate local color rules
        if (myFRList!=NULL)
        {
            fnd = false;
            i = myFRList->size() - 1;
            while ((i>=0) && (!fnd) )
            {
                TagValueItemWinFont* rule = myFRList->ref(i);
                if (rule->dfsNum <= myDFS) {break;}
                if (tagId==rule->tagId)
                {
                    //printf ("fnd rule for tgId=%d with value=%ul\n",tagId,rule.tagValue);
                    if (!gotValue)
                    {
                        //printf ("getting tag value...\n");
                        gotValue = temp.getCurrentTagValue(&value,cycle);
                        if (!gotValue) {break;}
                    }
                    if (value==rule->tagValue)
                    {
                        myDFS = rule->dfsNum;
                        prop = rule->fontSpec;
                        fnd    = true;
                    }
                }
                --i;
            }
        }

        // next tag to look at
        goon = temp.skipToNextTag();
    }
    *dfs = myDFS;
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// -- Warning!! this is time critical, make sure you know what you do
// ------------------------------------------------------------------
// ------------------------------------------------------------------
bool
CSLEngine::computeNodeRowEvents(INT32 cycle,NRCEList* nrceList)
{
    nrceList->reset();
    // iterate through all nodes-rows
    QIntDictIterator<NodeRow> it ( *(cdb->nrlist) );
    while (it.current())
    {
        NodeRow* noderow = it.current();
        DralGraphNode* node = cdb->getNode(noderow->node_id);
        Q_ASSERT(node!=NULL);

        QColor color;
        EventShape shape;
        bool isTransp=true;
        INT32 row = (INT32)(it.currentKey());
        bool ok = getTrackEventColorShape (row,noderow->trackId,cycle,&color,&shape,&isTransp);
        if (!ok) { return false; }
        if (!isTransp)
        {
            NRComputedEvent ce;
            ce.row   = noderow->row;
            ce.color = color;
            ce.shape = shape;
            nrceList->append(ce);
            zdb->setEventUsed(cycle,ce.row);
        }
        ++it;
    }
    return true;
    //printf("\nCSLEngine::computeNodeRowEvents result in %d events\n",(nrceList->getMaxUsedIdx()+1));
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// -- Warning!! this is time critical, make sure you know what you do
// ------------------------------------------------------------------
// ------------------------------------------------------------------
void
CSLEngine::commitNodeEvents(INT32 cycle,NRCEList* nrceList)
{
    int maxIdx = nrceList->getMaxUsedIdx();
    for (int i=0;i<=maxIdx;i++)
    {
        NRComputedEvent ce = nrceList->at(i);
        zdb->addTrackEventData(cycle, ce.row,ce.shape,ce.color);
    }
}


void
CSLEngine::getNearestSamples(QString shadetbl,INT32 idx,QColor& urc,QColor& lrc,INT32& uri,INT32& lri)
{
    ShadeRuleTbl *myshrt  = cdb->getShadeRuleTable(shadetbl);
    Q_ASSERT(myshrt!=NULL);
    Q_ASSERT(myshrt->count()!=0);

    ShadeRuleTbl::iterator it = myshrt->begin();
    uri=101;
    lri=-1;
    bool lrc_set = false;
    bool urc_set = false;
    while (it != myshrt->end())
    {
        INT32 currentIdx=(*it).occupancy;

        // check for nearest upper reference
        if ((currentIdx>idx) && (currentIdx<uri) )
        {
            urc_set = true;
            urc = (*it).color;
            uri = currentIdx;
        }

        // check for nearest lower reference
        if ((currentIdx<idx) && (currentIdx>lri) )
        {
            lrc_set = true;
            lrc = (*it).color;
            lri = currentIdx;
        }

        // exact match case
        if (currentIdx==idx)
        {
            urc_set = true;
            urc = (*it).color;
            uri = currentIdx;
            lrc_set = true;
            lrc = (*it).color;
            lri = currentIdx;
        }


        ++it;
    }
    Q_ASSERT(lrc_set);
    Q_ASSERT(urc_set);
}


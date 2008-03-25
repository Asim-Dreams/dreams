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
  * @file  CSLEngine.h
  */

#ifndef _CSLENGINE_H
#define _CSLENGINE_H

// QT library
#include <qcolor.h>
#include <qstring.h>

#include "asim/DralDB.h"

// 2Dreams
#include "ADFDefinitions.h"
#include "DBStructs.h"
#include "TabSpec.h"
#include "DralGraphNode.h"
#include "DralGraphEdge.h"
#include "ZDBase.h"
#include "ConfigDB.h"

class CSLEngine
{
    public:
        inline void getEventColorShape (
                            INT16 row,
                            ItemHandler* hnd,
                            INT32 cycle,
                            QColor* color,
                            EventShape* shape,
                            INT32* colorDFS = NULL, 
                            INT32* shapeDFS= NULL
                            );
                            
        inline bool getTrackEventColorShape (
                            INT32 row,
                            INT32 trackId, 
                            INT32 cycle,
                            QColor*color,
                            EventShape*shape,
                            bool* isTransp,
                            INT32 *colorDFS=NULL, 
                            INT32* shapeDFS=NULL
                            );

        bool computeNodeRowEvents(
                           INT32 cycle,
                           NRCEList* nrceList
                           );

        void commitNodeEvents(
                           INT32 cycle,
                           NRCEList* nrceList
                           );

        inline void addItemEventData (
                          INT16 edgeid,
                          UINT32 cycle,
                          INT32 position,
                          ItemHandler * hnd,
                          UINT32 item_id
                          );

        inline void markEventData (
                           INT16 edgeid,
                           UINT32 cycle,
                           INT32 position
                           );
        /**
          * Returns the Item Docked Window font properties (for a given ItemId)
          */
        inline void getItemWinFontProperties (TabSpec* tab, ItemHandler * hnd, ItemWinFont& prop);

    public:
        static CSLEngine* getInstance();
        static void destroy();

    protected:
        CSLEngine();
        virtual ~CSLEngine();

    protected:
        /**
          * Checks for color specific and global color definitions and
          * decides the right color for a given event
          * @return the event color
          */
        void checkColorShapeRules(ColorRuleList* clist,ShapeRuleList* slist,ItemHandler * hnd, INT32* colorDfs, QColor* color,INT32* shapeDfs, EventShape* shape,INT32 cycle);
        void checkTrackColorRules(ColorRuleList* clist,INT32 trackId, INT32* colorDfs, QColor* color,bool* isTransp, INT32 cycle);
        void checkTrackShapeRules(ShapeRuleList* slist,INT32 trackId, INT32* shapeDfs, EventShape* shape,INT32 cycle);
        void checkItemFontRules(ItemWinFontRuleList* myFRList, ItemHandler * hnd,INT32* dfs, ItemWinFont& prop,INT32 cycle);

        inline bool computeShadingColor(RowRules* rr,INT32 trackId,QColor *color,INT32 cycle, bool* isTransp);
        void getNearestSamples (QString tbl,INT32 idx,QColor& urc,QColor& lrc,INT32& uri,INT32& lri);

    private:
        // ref to compressed dbase
        ZDBase*   zdb;
        ConfigDB* cdb;
        DralDB*   ddb;
        LogMgr*   log;

        INT32 shadeWarnings;
    private:
        static CSLEngine* _myInstance;
};

void
CSLEngine::getEventColorShape (INT16 row, ItemHandler* hnd, INT32 cycle,QColor* color, EventShape* shape, INT32* colorDFS, INT32* shapeDFS)
{
    RowRules* rr = cdb->getRowRules(row);
    Q_ASSERT(rr!=NULL);

    *color = cdb->defaultColor;
    INT32  cdfs = cdb->defaultColorDFSNum;

    *shape = cdb->defaultShape;
    INT32  sdfs = cdb->defaultShapeDFSNum;

    // check againts row specific color
    if (rr->getColorDfsNum()>cdfs)
    {
        cdfs = rr->getColorDfsNum();
        *color = rr->getColor();
    }

    // check againts row specific shapes
    if (rr->getShapeDfsNum()>sdfs)
    {
        sdfs = rr->getShapeDfsNum();
        *shape = rr->getShape();
    }

    // check rules
    checkColorShapeRules(rr->getCrlist(),rr->getSrlist(),hnd,&cdfs,color,&sdfs,shape,cycle);
    
    if (colorDFS!=NULL) *colorDFS=cdfs;
    if (shapeDFS!=NULL) *shapeDFS=sdfs;
}

bool
CSLEngine::getTrackEventColorShape (INT32 row,INT32 trackId, INT32 cycle,QColor*color,EventShape*shape,bool* isTransp, INT32 *colorDFS, INT32* shapeDFS)
{
    bool result = true;

    RowRules* rr = cdb->getRowRules(row);
    Q_ASSERT(rr!=NULL);

    *color = cdb->defaultColor;
    INT32  cdfs = cdb->defaultColorDFSNum;
    *isTransp = false;

    *shape = cdb->defaultShape;
    INT32  sdfs = cdb->defaultShapeDFSNum;

    // check againts row specific color
    if (rr->getColorDfsNum()>cdfs)
    {
        cdfs = rr->getColorDfsNum();
        *color = rr->getColor();
        *isTransp = rr->getIsTransparent();
    }

    // check againts row specific shapes
    if (rr->getShapeDfsNum()>sdfs)
    {
        sdfs = rr->getShapeDfsNum();
        *shape = rr->getShape();
    }

    // check color rules
    QString shrtbl = rr->getShadingTbl();
    if (shrtbl==QString::null)
    { 
        checkTrackColorRules(rr->getCrlist(),trackId,&cdfs,color,isTransp,cycle); 
    }
    else
    {
        result = computeShadingColor(rr,trackId,color,cycle,isTransp);
        cdfs = rr->getShadingDfsNum();
    }

    // check shape rules
    checkTrackShapeRules(rr->getSrlist(),trackId,&sdfs,shape,cycle);

    if (colorDFS!=NULL) *colorDFS=cdfs;
    if (shapeDFS!=NULL) *shapeDFS=sdfs;
    
    return result;
}

bool
CSLEngine::computeShadingColor(RowRules* rr,INT32 trackId,QColor *color,INT32 cycle, bool* isTransp)
{
    QString tbl    = rr->getShadingTbl();
    INT32   tgId   = rr->getShadingTag();
    INT32   minVal = rr->getShadingMinValue();
    INT32   maxVal = rr->getShadingMaxValue();

    QColor upperRefColor,lowerRefColor;
    INT32  upperRefIdx;
    INT32  lowerRefIdx;
    UINT64 value=0;
    INT32  cvalue;
    bool vok = ddb->getTrackTagValue(trackId,tgId,cycle,&value);
    if (!vok)
    {
        if (shadeWarnings<100)
        {
            //printf("CSLEngine::computeShadingColor cycle = %d trackId=%d, value not fnd!\n",cycle,trackId);
            QString msg = "Warning, node tag " + ddb->getTagDescription(tgId);
            msg += " not found during shading color evaluation.\n";
            msg += "Cycle: "+QString::number(cycle)+"\n";
            msg += "Row:   "+QString::number(rr->getRowPos())+"\n";
            log->addLog(msg);
            *isTransp = true;
            shadeWarnings++;
            return true;      // no fatal error
        }
        else if (shadeWarnings==100)
        {
            log->addLog("Too many shading warnings, going to silent mode...\n");
            ++shadeWarnings;
            return true;
        }
        else 
        {
            return true;
        }
    }

    cvalue = (INT32)value;
    if ((cvalue<minVal)||(cvalue>maxVal))
    {
        //printf("CSLEngine::computeShadingColor cycle = %d trackId=%d, value out of range, min=%d, max=%d,val=%d\n",cycle,trackId,minVal,maxVal,cvalue);
        QString msg = "Warning, node tag " + ddb->getTagDescription(tgId);
        msg += " with out of range value during shading color evaluation.\n";
        msg += "Cycle: "+QString::number(cycle)+"\n";
        msg += "Row:   "+QString::number(rr->getRowPos())+"\n";
        msg += "Minimum Value: "+QString::number(minVal)+"\n";
        msg += "Maximum Value: "+QString::number(maxVal)+"\n";
        msg += "Found Value  : "+QString::number(cvalue)+"\n";
        log->addLog(msg);

        *isTransp = true;
        return false;
    }

    INT32 normalizedPos = cvalue - minVal;
    normalizedPos = (INT32)( (double)(normalizedPos)/(double)(maxVal-minVal)*100.0 );
    Q_ASSERT (normalizedPos>=0);
    Q_ASSERT (normalizedPos<=100);

    //printf ("CSLEngine::computeShadingColor cycle = %d trackId=%d,minVal=%d,maxVal=%d,val=%d,npos=%d\n",cycle,trackId,minVal,maxVal,cvalue,normalizedPos);

    getNearestSamples(tbl,normalizedPos,upperRefColor,lowerRefColor,upperRefIdx,lowerRefIdx);
    double prop = (double)(upperRefIdx-normalizedPos)/(double)(upperRefIdx-lowerRefIdx+1);

    // simple interpolation
    INT32 red = (INT32)((double)upperRefColor.red()*(1.0-prop) +
                        (double)lowerRefColor.red()*prop);

    INT32 green = (INT32)((double)upperRefColor.green()*(1.0-prop) +
                        (double)lowerRefColor.green()*prop);

    INT32 blue = (INT32)((double)upperRefColor.blue()*(1.0-prop) +
                        (double)lowerRefColor.blue()*prop);

    *color = QColor(red,green,blue);
    return true;
}

void
CSLEngine::getItemWinFontProperties (TabSpec* tab, ItemHandler * hnd, ItemWinFont& prop)
{
    //printf("getItemWinFontProperties called with tgHeapIdx=%d\n",tagHeapIdx);fflush(stdout);

    // 1) global default
    prop = cdb->defaultItemWinFont;
    INT32  dfs = cdb->defaultItemWinFontDFSNum;

    // 2) tab specific default
    if (tab->getDefaultFontDFS() > dfs)
    {
        dfs = tab->getDefaultFontDFS();
        prop = tab->getDefaultFont();
    }

    //if (tagHeapIdx<0) return (false);

    // 3) check rules
    checkItemFontRules(tab->getFontRuleList(),hnd,&dfs,prop,0);
}

void
CSLEngine::addItemEventData(INT16 edgeid, UINT32 cycle, INT32 position, ItemHandler * hnd, UINT32 item_id)
{
    /*printf ("@@ addEventData(edgeid=%d,cycle=%llu,position=%d,itemEntryPoint=%d,item_id=%u)\n",
           edgeid,cycle,position,itemEntryPoint,item_id);fflush(stdout);*/

    DralGraphEdge* edge = cdb->dgeList->find((long)edgeid);
    Q_ASSERT (edge!=NULL);

    // iterate through all edge's rows
    int edgeLat = cdb->getEdgeLatency(edgeid);
    QIntDictIterator<RowRules> it( *(edge->getRowList()) );
    for ( ; it.current(); ++it )
    {
        QColor color;
        EventShape shape;
        INT16 row = (INT16)(it.currentKey());
        INT32 frow = position + row;

        if(hnd->isValidTagHandler())
        {
            getEventColorShape(row,hnd,cycle,&color,&shape);
        }
        //printf("\t\t adding event data, row=%d,frow=%d, cycle=%llu, addlat=%d\n",(INT32)row,frow,cycle,latToAdd);fflush(stdout);
        zdb->addItemEventData(cycle+((it.current()->getDrawOnMove()) ? 0 : edgeLat),
                  frow,shape,color,item_id);
    }
    //printf ("@@ addEventData() finished ok\n");fflush(stdout);
}

void
CSLEngine::markEventData(INT16 edgeid, UINT32 cycle, INT32 position)
{
    //printf ("!! markEventData(edgeid=%d,cycle=%llu,position=%d)\n",edgeid,cycle,position);fflush(stdout);

    DralGraphEdge* edge = cdb->dgeList->find((long)edgeid);
    Q_ASSERT(edge!=NULL);

    // iterate through all edge's rows
    int edgeLat = cdb->getEdgeLatency(edgeid);
    QIntDictIterator<RowRules> it( *(edge->getRowList()) );
    for ( ; it.current(); ++it )
    {
        INT16 row = (INT16)(it.currentKey());
        INT32 frow = position + row;
        // check latency computation
        zdb->setEventUsed(cycle+((it.current()->getDrawOnMove()) ? 0 : edgeLat),frow);
    }
}

#endif

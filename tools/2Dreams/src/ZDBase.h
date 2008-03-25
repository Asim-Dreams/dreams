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
  * @file  ZDBase.h
  */

#ifndef _ZDBASE_H
#define _ZDBASE_H

#include <stdio.h>

#include <qstrlist.h>
#include <qintcache.h>
#include <qpoint.h>

#include "asim/DralDB.h"

#include "AColorPalette.h"
#include "ColDescVector.h"
#include "RowDescVector.h"

/**
  * Small class to keep cached complex look ups
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2003-01-15
  * @version 0.1
  */
class CachedLookUp
{
    public:
        CachedLookUp(INT32 _col, INT32 _row, INT32 _startingAt) :
        col(_col),row(_row),startingAt(_startingAt){};

        INT32 col,row,startingAt;
};

/**
  * This is a wrapper that centralizes all the 2Dreams dbase API.
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class ZDBase : public AMemObj, public StatObj
{
    public:
        static ZDBase* getInstance ();
        static void destroy();

    public:
        // ---- AMemObj Interface methods
        virtual INT64  getObjSize() const;
        virtual QString getUsageDescription() const;
        // ---- StatObj Interface methods
        QString getStats() const;
        // -----------------------------------------------

    protected:
        // init & memory allocation methods:
        ZDBase ();
        virtual ~ZDBase ();

    public:
        void flushCachedLookUps();

        //------------------------------------------------------------
        // helper methods
        //------------------------------------------------------------
        inline INT32 getMaxCycles();
        inline INT32 getMaxRows();
        inline INT32 getMaxUsg();

        void reset();
        inline void   setFirstEffectiveCycle(UINT32 n);
        inline UINT64 getFirstEffectiveCycle();

        //------------------------------------------------------------
        // event matrix related methods
        //------------------------------------------------------------
        inline INT32 getNumCycles ();
        inline bool  incNumCycles (INT32 offset=1);

        inline INT32 getNumRows   ();
        inline bool  setNumRows   (INT32 numrows);

        inline bool isUsed     (INT32 col, INT32 row);
        inline bool hasValidData(INT32 col, INT32 row);
        inline bool isActive   (INT32 row);

        inline ColorShapeItem getColorShape (INT32 col, INT32 row);

        inline RowType getRowType(INT32 row);
        inline void    setRowType(INT32 row, RowType type);

        inline QString getRowDescription(INT32 row);
        inline void setRowDescription(INT32 row, QString);

        inline void setRowEdgePos(INT32 row,UINT16 value);
        inline void setRowEdgeId(INT32 row,UINT16 value);
        inline UINT16 getRowEdgeId(INT32 row);
        inline UINT16 getRowEdgePos(INT32 row);

        inline void setRowNodeId(INT32 row,UINT16 value);
        inline UINT16 getRowNodeId(INT32 row);

        inline void setEventUsed(INT32 col, INT32 row);

        inline void computeNumEvents(INT32 col);

        inline void addItemEventData(INT32 col, INT32 row, EventShape shape,QColor color,UINT32 item_id);
        inline void addTrackEventData(INT32 col, INT32 row, EventShape shape,QColor color);

        inline INT32 getMinItemId(INT32 col);
        inline INT32 getMaxItemId(INT32 col);

        inline INT32 hasItemId(INT32 col,INT32 itemId);

        bool lookForFirstAppearence(INT32 itemId, INT32 startingAt, INT32& col, INT32& row);
        bool findItemWindow(INT32 itemId,INT32* birth,INT32* death);

        //------------------------------------------------------------
        // Matching methods
        //------------------------------------------------------------
        inline bool matchTagIdValue(INT32 col, INT32 row, INT32 tagId,UINT64 matchValue);
        inline bool matchTagIdValue(INT32 col, INT32 row, INT32 tagId,QString matchValue,bool cs, bool em);
        inline bool matchTagIdValue(INT32 col, INT32 row, INT32 tagId,QRegExp matchValue);

        INT32 matchTagIdValue(INT32 col, INT32 tagId,UINT64 matchValue,INT32 stRow=0);
        INT32 matchTagIdValue(INT32 col, INT32 tagId,QString matchValue,bool cs, bool em,INT32 stRow=0);
        INT32 matchTagIdValue(INT32 col, INT32 tagId,QRegExp matchValue,INT32 stRow=0);

        void dumpRegression();

    private:
        UINT32 firstEffectiveCycle;

        // The main event matrix:
        RowDescVector*   rowvec;
        ColDescVector*   colvec;

        DralDB* draldb;

        // cache to save common look-ups
        QIntCache<CachedLookUp>* lookUpCache;

    private:
        static ZDBase* _myInstance;
};

// -------------------------------------------------------------------
// Event Matrix related methods
// -------------------------------------------------------------------
INT32
ZDBase::getNumCycles ()
{ return colvec->getNumCycles(); }

bool
ZDBase::incNumCycles (INT32 offset)
{ return colvec->incNumCycles(offset); }

bool 
ZDBase::setNumRows(INT32 numrows)
{return rowvec->setNumRows(numrows); }

INT32 
ZDBase::getNumRows ()
{ return rowvec->getNumRows(); }

bool
ZDBase::isActive (INT32 row)
{
    if (row>=getNumRows()) { return false; }
    return rowvec->isActive(row);
}

INT32
ZDBase::hasItemId(INT32 col,INT32 itemId)
{
    /// @todo review this method when noderows are enabled...
    INT32 i=0;
    bool fnd = false;
    while ( !fnd && i<COLDESC_MAX_EVENTS)
    {
        if (colvec->hasValidData(col-firstEffectiveCycle,i) && (getRowType(i)==SimpleEdgeRow))
        {
            ItemHandler hnd;
            draldb->getMoveItem(&hnd, getRowEdgeId(i),col,getRowEdgePos(i));
            if(hnd.isValidItemHandler())
            {
                fnd = (hnd.getItemId()==itemId);
            }
        }
        ++i;
    }

    INT32 result;
    if (fnd)
    {
        result = i-1;
    }
    else
    {
        result = -1;
    }
    return (result);
}



ColorShapeItem
ZDBase::getColorShape (INT32 col, INT32 row)
{
    col-=firstEffectiveCycle;
    Q_ASSERT(row<getNumRows());
    //printf (">>colorShapeIdx for col=%d, row=%d => \n",col,row);fflush(stdout);fflush(stderr);
    //printf (" %d\n",colvec->getColorShapeIdx(col,row));
    //fflush(stdout);fflush(stderr);
    return rowvec->getColorShape(col, row, colvec->getColorShapeIdx(col,row));
    //return rowvec->getColorShape(col, row, 1);
}

QString
ZDBase::getRowDescription(INT32 row)
{
    Q_ASSERT(row<getNumRows());
    return rowvec->getRowDescription(row);
}

void
ZDBase::setRowDescription(INT32 row, QString desc)
{
    Q_ASSERT(row<getNumRows());
    rowvec->setRowDescription(row,desc);
}

RowType
ZDBase::getRowType(INT32 row)
{
    Q_ASSERT(row<getNumRows());
    return rowvec->getRowType(row);
}

void
ZDBase::setRowType(INT32 row, RowType type)
{
    Q_ASSERT(row<getNumRows());
    rowvec->setRowType(row,type);
}

UINT16
ZDBase::getRowEdgeId(INT32 row)
{
    Q_ASSERT(row<getNumRows());
    return rowvec->getEdgeId(row);
}

UINT16
ZDBase::getRowEdgePos(INT32 row)
{
    Q_ASSERT(row<getNumRows());
    return rowvec->getEdgePos(row);
}

void
ZDBase::setRowEdgeId(INT32 row,UINT16 value)
{
    Q_ASSERT(row<getNumRows());
    rowvec->setEdgeId(row,value);
}

void
ZDBase::setRowEdgePos(INT32 row,UINT16 value)
{
    Q_ASSERT(row<getNumRows());
    rowvec->setEdgePos(row,value);
}

void
ZDBase::setRowNodeId(INT32 row,UINT16 value)
{
    Q_ASSERT(row<getNumRows());
    rowvec->setNodeId(row,value);
}

UINT16
ZDBase::getRowNodeId(INT32 row)
{
    Q_ASSERT(row<getNumRows());
    return rowvec->getNodeId(row);
}

bool
ZDBase::isUsed(INT32 col, INT32 row)
{
    col-=firstEffectiveCycle;
    if (row>=getNumRows()) { return false; }
    return colvec->isUsed(col,row);
}

bool
ZDBase::hasValidData(INT32 col, INT32 row)
{
    col-=firstEffectiveCycle;
    if (row>=getNumRows()) { return false; }
    return colvec->hasValidData(col,row);
}

void
ZDBase::addItemEventData(INT32 col, INT32 row, EventShape shape, QColor color,UINT32 item_id)
{
    col-=firstEffectiveCycle;
    if (row>=getNumRows())
    {
        (LogMgr::getInstance())->addLog("Internal error, ZDBase::addItemEventData() on row "+QString::number(row));
    }
    // manage to get some color/shape identifier
    INT32 csidx = rowvec->createColorShapeIndex(col,row,shape,color);

    // just by-pass the responsability to colvec object
    colvec->addItemEventData(col,row,csidx,item_id);
}

void
ZDBase::addTrackEventData(INT32 col, INT32 row, EventShape shape, QColor color)
{
    col-=firstEffectiveCycle;
    if (row>=getNumRows())
    {
        (LogMgr::getInstance())->addLog("Internal error, ZDBase::addTrackEventData() on row "+QString::number(row));
    }
    // manage to get some color/shape identifier
    INT32 csidx = rowvec->createColorShapeIndex(col,row,shape,color);

    // just by-pass the responsability to colvec object
    colvec->addTrackEventData(col,row,csidx);
}

void
ZDBase::setEventUsed(INT32 col, INT32 row)
{
    col-=firstEffectiveCycle;
    Q_ASSERT(row<getNumRows());
    colvec->setUsed(col,row);
}

void
ZDBase::computeNumEvents(INT32 col)
{
    col-=firstEffectiveCycle;
    colvec->computeNumEvents(col);
}

INT32
ZDBase::getMinItemId(INT32 col)
{
    col-=firstEffectiveCycle;
    return colvec->getMinItemId(col);
}

INT32
ZDBase::getMaxItemId(INT32 col)
{
    col-=firstEffectiveCycle;
    return colvec->getMaxItemId(col);
}

// -------------------------------------------------------------------
// Other methods
// -------------------------------------------------------------------
bool
ZDBase::matchTagIdValue(INT32 col, INT32 row, INT32 tagId,UINT64 matchValue)
{
    //printf ("ZDBase::matchTagIdValue col=%d,row=%d,tagId=%d, val=%d\n",(int)col,(int)row,(int)tagId,(int)matchValue);fflush(stdout);fflush(stderr);
    UINT64 tgvalue;
    UINT64 tgwhen;
    TagValueType tgvtype;
    INT16 tgbase;

    if (row>=getNumRows()) return (false);

    // 1) we check if the col/row is used & active
    if (! hasValidData(col,row) ) return (false);

   // tmp
   if (getRowType(row)!=SimpleEdgeRow) return (false);

    //if (! isActive(row) ) return (false);

    // 2) get the tag-heap index of the current event
    ItemHandler hnd;
    draldb->getMoveItem(&hnd, getRowEdgeId(row),col,getRowEdgePos(row));
    if (!hnd.isValidItemHandler())
    {
        return false;
    }

    INT32 pos = hnd.getTagById(tagId,&tgvalue,&tgvtype,&tgbase,&tgwhen,col);

    /*
    printf("ZDBase::matchTagIdValue fnd=%d, tgvalue=%ld, matchVal=%ld\n",(
           int)fnd,(long)tgvalue,(long)matchValue);fflush(stdout);
    */
    return ((pos>=0) && (tgvalue==matchValue) );
}

bool
ZDBase::matchTagIdValue(INT32 col, INT32 row, INT32 tagId,QString matchValue,bool cs, bool em)
{
    UINT64 tgvalue;
    UINT64 tgwhen;
    TagValueType tgvtype;
    INT16 tgbase;

    if (row>=getNumRows()) return (false);

    // 1) we check if the col/row is used & active
    if (! hasValidData(col,row) ) return (false);
    //if (! isActive(row) ) return (false);

    if (getRowType(row)!=SimpleEdgeRow) return (false);

    ItemHandler hnd;
    draldb->getMoveItem(&hnd, getRowEdgeId(row),col,getRowEdgePos(row));
    if (!hnd.isValidItemHandler())
    {
        return false;
    }

    bool fnd = hnd.getTagById(tagId,&tgvalue,&tgvtype,&tgbase,&tgwhen,col);
    /*
    printf("ZDBase::matchTagIdValue fnd=%d, tgvalue=%ld, matchVal=%ld\n",(
           int)fnd,(long)tgvalue,(long)matchValue);fflush(stdout);
    */

    if (fnd)
    {
        QString fvalue = (StrTable::getInstance())->getString(tgvalue);
        if (em)
        {
            if (!cs)
            {
                fvalue = fvalue.upper();
            }
            fnd = (fvalue==matchValue);
        }
        else
        {
            fnd = (fvalue.contains(matchValue,cs)>0);
        }
        return (fnd);
    }
    else
    {
        return false;
    }
}

bool
ZDBase::matchTagIdValue(INT32 col, INT32 row, INT32 tagId,QRegExp matchValue)
{
    UINT64 tgvalue;
    UINT64 tgwhen;
    TagValueType tgvtype;
    INT16 tgbase;

    if (row>=getNumRows()) return (false);

    // 1) we check if the col/row is used & active
    if (! hasValidData(col,row) ) return (false);
    //if (! isActive(row) ) return (false);

    if (getRowType(row)!=SimpleEdgeRow) return (false);

    ItemHandler hnd;
    draldb->getMoveItem(&hnd, getRowEdgeId(row),col,getRowEdgePos(row));
    if (!hnd.isValidItemHandler())
    {
        return false;
    }

    //printf("ZDBase::matchTagIdValue tgheapidx=%d\n",(int)tgheapidx);fflush(stdout);
    // 3) now look for the Tag "tagId' in such heap entry
    bool fnd = hnd.getTagById(tagId,&tgvalue,&tgvtype,&tgbase,&tgwhen,col);

    /*
    printf("ZDBase::matchTagIdValue fnd=%d, tgvalue=%ld, matchVal=%ld\n",(
           int)fnd,(long)tgvalue,(long)matchValue);fflush(stdout);
    */

    if (fnd)
    {
        QString fvalue = (StrTable::getInstance())->getString(tgvalue);
        fnd = (fvalue.contains(matchValue)>0);
        return (fnd);
    }
    else
    {
        return false;
    }
}


void
ZDBase::setFirstEffectiveCycle(UINT32 n)
{ firstEffectiveCycle=n; }

UINT64
ZDBase::getFirstEffectiveCycle()
{ return firstEffectiveCycle; }

#endif


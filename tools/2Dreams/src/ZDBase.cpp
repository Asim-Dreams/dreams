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
  * @file ZDBase.cpp
  */

#include "ZDBase.h"

ZDBase* ZDBase::_myInstance = NULL;

ZDBase*
ZDBase::getInstance()
{
    if (_myInstance==NULL)
        _myInstance = new ZDBase();

    Q_ASSERT(_myInstance!=NULL);
    return _myInstance;
}

void
ZDBase::destroy()
{
    if (_myInstance!=NULL) { delete _myInstance; }
}


ZDBase::ZDBase ()
{
    firstEffectiveCycle=0;

    // the event matrix:
    colvec = new ColDescVector();Q_ASSERT(colvec!=NULL);
    ColDescriptor::initPopCnt();
    rowvec = new RowDescVector();Q_ASSERT(rowvec!=NULL);

    draldb = DralDB::getInstance();

    // the lookup cache
    lookUpCache = new QIntCache<CachedLookUp>(1000,1511);
    lookUpCache->setAutoDelete(true);
}

ZDBase::~ZDBase()
{
    if (rowvec!=NULL) { delete  rowvec; }
    if (colvec!=NULL) { delete  colvec; }
}


// -------------------------------------------------------------------
// event matrix related methods
// -------------------------------------------------------------------
bool
ZDBase::lookForFirstAppearence(INT32 itemId, INT32 startingAt, INT32& rescol, INT32& resrow)
{
    //printf("lookForFirstAppearence on %d\n",itemId);

    CachedLookUp* pt = lookUpCache->find((long)itemId);
    if (pt!=NULL)
    {
        rescol = pt->col;
        resrow = pt->row;
        if (pt->startingAt == startingAt)
        {
            //printf("lookForFirstAppearence done due to cache hit\n");fflush(stdout);fflush(stderr);
            return (true);
        }
    }

    bool result;
    bool fnd=false;
    INT32 col=startingAt-firstEffectiveCycle;
    INT32 row=-1;
    INT32 max=getNumCycles();

    while (!fnd && col<max)
    {
        // 1) check bounds
        INT32 minId = colvec->getMinItemId(col);
        INT32 maxId = colvec->getMaxItemId(col);

        if ( (itemId<minId)||(itemId>maxId) )
        {
            ++col;
            continue;
        }

        // 2) loop in col descriptor...
        row = hasItemId(col+firstEffectiveCycle,itemId);
        fnd = (row>=0);

        ++col;
    }

    if (fnd)
    {
        rescol = firstEffectiveCycle+col-1;
        resrow = row;
        result = true;
        lookUpCache->insert((long)itemId,new CachedLookUp(rescol,resrow,startingAt));
        //printf("found at col=%d\n",result);
    }
    else
    {
        result = false;
        rescol = -1;
        resrow = -1;
        //printf("not found !?\n");
    }

    //printf("lookForFirstAppearence done\n");fflush(stdout);fflush(stderr);
    return result;
}

bool
ZDBase::findItemWindow(INT32 itemId,INT32* birth,INT32* death)
{
    //printf("findItemWindow on %d\n",itemId);

    // check first event
    bool ffnd;
    INT32 fcol,frow;
    ffnd = lookForFirstAppearence(itemId,firstEffectiveCycle,fcol,frow);
    if (!ffnd)
    {
        return false;
    }
    (*birth) = fcol;

    // look for last event
    INT32 max=getNumCycles();
    INT32 col = fcol-firstEffectiveCycle;
    INT32 lcol = col;
    while (col<max)
    {
        // 1) check bounds
        INT32 minId = colvec->getMinItemId(col);
        INT32 maxId = colvec->getMaxItemId(col);

        if ( (itemId<minId)||(itemId>maxId) )
        {
            ++col;
            continue;
        }

        // 2) loop in col descriptor...
        INT32 row = hasItemId(col+firstEffectiveCycle,itemId);
        if (row>=0)
        {
            lcol = col;
        }
        ++col;
    }
    (*death)=(lcol+firstEffectiveCycle);
    //printf("findItemWindow done \n");fflush(stdout);
    return (true);
}

// -------------------------------------------------------------------
// Helper methods
// -------------------------------------------------------------------
void 
ZDBase::reset()
{
    draldb->reset();
    rowvec->reset();
    colvec->reset();
    firstEffectiveCycle=0;
}

void
ZDBase::flushCachedLookUps()
{
    lookUpCache->clear();
}

INT32
ZDBase::matchTagIdValue(INT32 col, INT32 tagId,UINT64 matchValue,INT32 stRow)
{
    INT32 result;
    INT32 numRows = getNumRows();
    bool fnd = false;
    INT32 row=stRow;
    while (!fnd && row<numRows)
    {
        fnd = matchTagIdValue(col,row,tagId,matchValue);
        ++row;
    }
    if (fnd)
    {
        result = row-1;
    }
    else
    {
        result =-1;
    }
    return (result);
}

INT32
ZDBase::matchTagIdValue(INT32 col, INT32 tagId,QString matchValue,bool cs, bool em,INT32 stRow)
{
    INT32 result;
    INT32 numRows = getNumRows();
    bool fnd = false;
    INT32 row=stRow;
    while (!fnd && row<numRows)
    {
        fnd = matchTagIdValue(col,row,tagId,matchValue,cs,em);
        ++row;
    }
    if (fnd)
    {
        result = row-1;
    }
    else
    {
        result =-1;
    }
    return (result);
}

INT32
ZDBase::matchTagIdValue(INT32 col, INT32 tagId,QRegExp matchValue,INT32 stRow)
{
    INT32 result;
    INT32 numRows = getNumRows();
    bool fnd = false;
    INT32 row=stRow;
    while (!fnd && row<numRows)
    {
        fnd = matchTagIdValue(col,row,tagId,matchValue);
        ++row;
    }
    if (fnd)
    {
        result = row-1;
    }
    else
    {
        result =-1;
    }
    return (result);
}

void
ZDBase::dumpRegression()
{
    char str[256];
    ColorShapeItem csh;
    EventShape shape;
    QColor color;
    INT32 R, G, B;

    for(INT32 i = 0; i < rowvec->getNumRows(); i++)
    {
        for(INT32 j = firstEffectiveCycle; j < colvec->getNumCycles() + firstEffectiveCycle; j++)
        {
            if(colvec->hasValidData(j, i))
            {
                csh = getColorShape(j, i);
                color = AColorPalette::getColor(csh.getColorIdx());
                color.getRgb(&R, &G, &B);
                shape = csh.getShape();

                // I index.
                sprintf(str, FMT32X, i);
                printf("i=%s,", str);

                // J index.
                sprintf(str, FMT32X, j);
                printf("j=%s,", str);

                // R color.
                sprintf(str, FMT32X, R);
                printf("C=(%s,", str);

                // G color.
                sprintf(str, FMT32X, G);
                printf("%s,", str);

                // B color.
                sprintf(str, FMT32X, B);
                printf("%s),", str);

                // Shape.
                sprintf(str, FMT32X, shape);
                printf("S=%s\n", str);
            }
        }
    }
}

// -------------------------------------------------------------------
// AMemObj methods
// -------------------------------------------------------------------

INT64
ZDBase::getObjSize() const
{
    INT64 result = sizeof(this);

    if (rowvec!=NULL)
    {
        result += rowvec->getObjSize();
    }

    if (colvec!=NULL)
    {
        result += colvec->getObjSize();
    }

    result += lookUpCache->count()*(sizeof(CachedLookUp)+3*sizeof(void*));
    return (result);
}

QString
ZDBase::getUsageDescription() const
{
    QString result = "\nObject ZDBase contains:\n";

    if (rowvec!=NULL)        result += " 1) Row Descriptors:\t\t"+QString::number((long)(rowvec->getObjSize())) +" bytes\n";
    if (colvec!=NULL)        result += " 2) Column Descriptors:\t\t"+QString::number((long)(colvec->getObjSize())) +" bytes\n";

    INT64 cacheSize;
    cacheSize = lookUpCache->count()*(sizeof(CachedLookUp)+3*sizeof(void*));
    result += " 3) Lookup Cache:\t\t"+QString::number((long)cacheSize) +" bytes\n";

    return (result);
}

QString
ZDBase::getStats() const
{
   QString  result = "\tObject ZDBase contains:\n";

    result += "\n\t1) Column Descriptors\n";
    result += colvec->getStats();

    result += "\n\t2) Row Descriptors\n";
    result += rowvec->getStats();

    return (result);
}

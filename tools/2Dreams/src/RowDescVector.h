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
  * @file  RowDescVector.h
  */

#ifndef _ROWDESCVECTOR_H
#define _ROWDESCVECTOR_H

#include <stdio.h>

#include "asim/DralDB.h"

#include "RowDescriptor.h"

/**
  * This class holds a vector with the row descriptions on the event matrix.
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class RowDescVector : public AMemObj, public StatObj
{
    public:
        // ---- AMemObj Interface methods
        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;
        // ---- StatObj Interface methods
        QString getStats() const;
        // -----------------------------------------------
    public:

        RowDescVector();
        virtual ~RowDescVector();

        inline INT32 getNumRows();
        inline bool  setNumRows (INT32 numrows);

        inline bool isActive (INT32 row);
        inline ColorShapeItem getColorShape (INT32 col, INT32 row, INT32 idx);
        inline INT32 createColorShapeIndex(INT32 col, INT32 row,EventShape shape, QColor color);
        void   reset();

        inline RowType getRowType(INT32 row);
        inline void setRowType(INT32 row, RowType type);

        inline QString getRowDescription(INT32 idx);
        inline void setRowDescription(INT32 row, QString desc);

        inline void setEdgeId(INT32 row,UINT16 value);
        inline UINT16 getEdgeId(INT32 row);
        inline void setEdgePos(INT32 row,UINT16 value);
        inline UINT16 getEdgePos(INT32 row);

        inline void setNodeId(INT32 row,UINT16 value);
        inline UINT16 getNodeId(INT32 row);
    private:
        INT32 maxRows;
        INT32 numRows;
        RowDescriptor* rowvector;
};

INT32
RowDescVector::getNumRows()
{ return numRows; }

bool
RowDescVector::setNumRows(INT32 numrows)
{
    if ((numrows>maxRows)||(numrows<=0))
    {
        return (false);
    }

    numRows = numrows;
    return (true);
}

bool
RowDescVector::isActive (INT32 row)
{
    if (row<numRows)
    {
       return rowvector[row].isActive();
    }
    else
    {
       return (false);
    }
}

ColorShapeItem
RowDescVector::getColorShape (INT32 col, INT32 row, INT32 idx)
{
    Q_ASSERT(row<numRows);
    Q_ASSERT(row>=0);
    return rowvector[row].getColorShape(col,row,idx);
}

RowType 
RowDescVector::getRowType(INT32 row)
{
    Q_ASSERT(row<numRows);
    Q_ASSERT(row>=0);
    return rowvector[row].getRowType();
}

void
RowDescVector::setRowType(INT32 row, RowType type)
{
    Q_ASSERT(row<numRows);
    Q_ASSERT(row>=0);
    rowvector[row].setRowType(type);
}

QString
RowDescVector::getRowDescription(INT32 row)
{
    Q_ASSERT(row<numRows);
    Q_ASSERT(row>=0);
    return rowvector[row].getRowDescription();
}

void
RowDescVector::setRowDescription(INT32 row, QString desc)
{
    Q_ASSERT(row<numRows);
    Q_ASSERT(row>=0);
    rowvector[row].setRowDescription(desc);
}

INT32
RowDescVector::createColorShapeIndex(INT32 col, INT32 row,EventShape shape, QColor color)
{
    Q_ASSERT(row<numRows);
    Q_ASSERT(row>=0);
    return rowvector[row].createColorShapeIndex(col,row,shape,color);
}

void
RowDescVector::setEdgeId(INT32 row,UINT16 value)
{
    Q_ASSERT(row<numRows);
    Q_ASSERT(row>=0);
    rowvector[row].setEdgeId(value);
}

UINT16
RowDescVector::getEdgeId(INT32 row)
{
    Q_ASSERT(row<numRows);
    Q_ASSERT(row>=0);
    return rowvector[row].getEdgeId();
}

void
RowDescVector::setNodeId(INT32 row,UINT16 value)
{
    Q_ASSERT(row<numRows);
    Q_ASSERT(row>=0);
    rowvector[row].setNodeId(value);
}

UINT16
RowDescVector::getNodeId(INT32 row)
{
    Q_ASSERT(row<numRows);
    Q_ASSERT(row>=0);
    return rowvector[row].getNodeId();
}

void
RowDescVector::setEdgePos(INT32 row,UINT16 value)
{
    Q_ASSERT(row<numRows);
    Q_ASSERT(row>=0);
    rowvector[row].setEdgePos(value);
}

UINT16
RowDescVector::getEdgePos(INT32 row)
{
    Q_ASSERT(row<numRows);
    Q_ASSERT(row>=0);
    return rowvector[row].getEdgePos();
}

#endif



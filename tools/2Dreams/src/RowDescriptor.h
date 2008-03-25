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
  * @file RowDescriptor.h
  */

#ifndef _ROWDESCRIPTOR_H
#define _ROWDESCRIPTOR_H

//#include <stdio.h>

#include <qptrlist.h>

#include "asim/DralDB.h"

#include "ColorShapeItem.h"
#include "EventItem.h"
#include "EventShape.h"
#include "AColorPalette.h"
#include "ADFDefinitions.h"

/**
  * This class hold the information needed for each row on the event matrix.
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class RowDescriptor : public AMemObj
{
    public:
        // ---- AMemObj Interface methods
        INT64  getObjSize() const;
        QString getUsageDescription() const;
        // -----------------------------------------------

    public:

        RowDescriptor();
        RowDescriptor(RowType type);
        virtual ~RowDescriptor();

        inline bool isActive();
        inline void setActive(bool);

        inline void  setRowDescription(QString);
        inline QString getRowDescription();

        inline ColorShapeItem getColorShape (INT32 col, INT32 row, INT32 idx);
        inline INT32 createColorShapeIndex(INT32 col, INT32 row,EventShape shape, QColor color);

        inline UINT16 getEdgeId();
        inline void setEdgeId(UINT16 id);
        inline UINT16 getEdgePos();
        inline void setEdgePos(UINT16 pos);

        inline UINT16 getNodeId();
        inline void setNodeId(UINT16 id);

        inline UINT16 getInputBw();
        inline void setInputBw(UINT16 bw);

        inline UINT16 getOutputBw();
        inline void setOutputBw(UINT16 bw);

        inline void setRowType(RowType type);
        inline RowType getRowType();

        void reset();

    public:
        static double getNonCachedProp();

    protected:
        void init(RowType type);
        /**
         * Return the Color/Shape combination for
         * a given column/row event (look in linked list)
         */
        ColorShapeItem getCSH(INT32 col, INT32 row);

        /**
         */
        inline INT32 lookForCachedColorShape(EventShape shape,QColor color);

    private:
        // rows can be represent edges, nodes, node slots, enter/exit cmd on nodes,
        // or can be shaded rows (like occupancy ones).
        RowType rowtype;

        // row description
        QString desc;

        // if this is an edge row, we need the edge id
        UINT16 edge_id;
        UINT16 edge_pos; // edges have bw associated rows

        // if this is a node row, we need the node id
        UINT16 node_id;

        // if this is an input node row, we need its bandwidth
        UINT16 input_bw;

        // if this is an input node row, we need its bandwidth
        UINT16 output_bw;

        // activte? (non-hidden)
        bool activeRow;

        // normal events try to use some of these
        // "cached" precomputed color/shape combinations
        // [0] is reserved for special combinations
        ColorShapeItem csitems[EITEM_MAX_COLORSHAPE_ITEMS];
        INT32 nextColorShapeIdx;

        // for rare special cases where more than EITEM_MAX_COLORSHAPE_ITEMS
        // shape/color
        // combinations are requiered, we implement a
        // dynamic linked list of Color/Shape combinations
        // each ColorShapeItem holds the cycle/row of the related
        // event
        CSHList *cshlist;

    private:
        static UINT64 _nonCachedCnt;
        static UINT64 _totalCnt;
};


// -----------------------------------------------
// -----------------------------------------------
// Inlined functions
// -----------------------------------------------
// -----------------------------------------------

UINT16
RowDescriptor::getEdgeId()
{
    Q_ASSERT(rowtype==SimpleEdgeRow);
    //if (rowtype!=SimpleEdgeRow) { abort(); }
    return edge_id;
}

void
RowDescriptor::setEdgeId(UINT16 id)
{
    Q_ASSERT(rowtype==SimpleEdgeRow);
    edge_id = id;
}

UINT16
RowDescriptor::getEdgePos()
{
    Q_ASSERT(rowtype==SimpleEdgeRow);
    return edge_pos;
}

void
RowDescriptor::setEdgePos(UINT16 pos)
{
    Q_ASSERT(rowtype==SimpleEdgeRow);
    edge_pos = pos;
}

UINT16
RowDescriptor::getNodeId()
{
    Q_ASSERT(rowtype!=SimpleEdgeRow);
    return node_id;
}

void
RowDescriptor::setNodeId(UINT16 id)
{
    Q_ASSERT(rowtype!=SimpleEdgeRow);
    node_id = id;
}

UINT16
RowDescriptor::getInputBw()
{
    Q_ASSERT(rowtype==InputNodeRow);
    return input_bw;
}

void
RowDescriptor::setInputBw(UINT16 bw)
{
    Q_ASSERT(rowtype==InputNodeRow);
    input_bw = bw;
}

UINT16
RowDescriptor::getOutputBw()
{
    Q_ASSERT(rowtype==OutputNodeRow);
    return output_bw;
}

void
RowDescriptor::setOutputBw(UINT16 bw)
{
    Q_ASSERT(rowtype==OutputNodeRow);
    output_bw = bw;
}

RowType
RowDescriptor::getRowType()
{ return rowtype; }

void 
RowDescriptor::setRowType(RowType type)
{
    Q_ASSERT(rowtype==UninitializedRow);
    rowtype=type;
}
void
RowDescriptor::setRowDescription(QString value)
{ desc = value; }


QString
RowDescriptor::getRowDescription()
{ return desc; }

ColorShapeItem
RowDescriptor::getColorShape (INT32 col, INT32 row, INT32 idx)
{
    if (idx!=0)
        return csitems[idx];
    else
        return getCSH(col,row);
}

bool 
RowDescriptor::isActive()
{return activeRow;}

void 
RowDescriptor::setActive(bool v)
{activeRow=v;}

INT32
RowDescriptor::createColorShapeIndex(INT32 col, INT32 row,EventShape shape, QColor color)
{
    INT32 resultIdx;
    ++_totalCnt;
    // 1) check if this comb. is already on the cache
    resultIdx = lookForCachedColorShape(shape,color);
    if (resultIdx>0) return resultIdx;

    // 2) check whether we have room in the cache for another combination
    if (nextColorShapeIdx<EITEM_MAX_COLORSHAPE_ITEMS)
    {
        resultIdx = nextColorShapeIdx;
        csitems[resultIdx].setShape(shape);
        csitems[resultIdx].setColorIdx(AColorPalette::createColor(color));
        nextColorShapeIdx++;
        return (resultIdx);
    }

    // 3) create a new 'special' case (non-cached => slow)
    ColorShapeItem* nobj = new ColorShapeItem(shape,AColorPalette::createColor(color),col,row);
    Q_ASSERT(nobj!=NULL);
    cshlist->append(nobj);
    ++ _nonCachedCnt;
    return (0); // reserved for non-chaced cases.
}

INT32
RowDescriptor::lookForCachedColorShape(EventShape shape,QColor color)
{
    INT32 i = 1;            ///< Position 0 is reserved for non cached ones
    bool fnd = false;

    while ( !fnd && (i<nextColorShapeIdx) )
    {
        bool c1,c2;
        c1 = (csitems[i].getShape() == shape);

        INT32 colidx = csitems[i].getColorIdx();
        QColor ic = AColorPalette::getColor(colidx);
        c2 = (ic == color);

        fnd = (c1&&c2);
        ++i;
    }
    if (fnd)
    {
       return (i-1);
    }
    else
    {
       return (-1);
    }
}



#endif

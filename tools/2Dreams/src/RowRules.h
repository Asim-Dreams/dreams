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
  * @file  RowRules.h
  * @brief
  */

#ifndef _ROWRULES_H_
#define _ROWRULES_H_

// QT Library
#include <qintdict.h>

#include "asim/DralDB.h"

#include "ADFDefinitions.h"

class RowRules
{
    public:
        RowRules (UINT16 _id, UINT16 _row, RowType _rtype);
        ~RowRules();

        // for edge rows
        inline UINT16 getEdgeId();
        inline void setDrawOnMove(bool value);
        inline bool getDrawOnMove();

        // for node, input and output rows
        inline UINT16 getNodeId();

        // common stuff
        inline RowType getType();
        inline UINT16 getId();
        inline INT32 getRowPos ();
        inline void  setRowPos (INT32 value);

        inline void setColor (QColor c, INT32 colorDfsNum, bool isTransp);
        inline void setShape (EventShape esh, INT32 shapeDfsNum);
        inline void setLetter(char l, INT32 letterDfsNum);

        inline QColor getColor();
        inline bool   getIsTransparent();
        inline EventShape getShape();
        inline char getLetter();

        inline INT32  getColorDfsNum();
        inline INT32  getShapeDfsNum();
        inline INT32  getLetterDfsNum();

        inline ColorRuleList*  getCrlist();
        inline ShapeRuleList*  getSrlist();
        inline LetterRuleList* getLrlist();

        inline QString getShadingTbl();
        inline void setShadingTbl(QString shadingTbl);
        inline INT32 getShadingTag();
        inline void setShadingTag(INT32 value);
        inline INT32 getShadingMinValue();
        inline void  setShadingMinValue(INT32);
        inline INT32 getShadingMaxValue();
        inline void  setShadingMaxValue(INT32);
        inline INT32 getShadingDfsNum();
        inline void  setShadingDfsNum(INT32);

    private:
        RowType rtype;
        UINT16 id;               ///< the edge/node the row belongs to
        INT16  row;              ///< the first row used (badwidth/slot determines the last)

        // -- ADF dependent attributes
        QColor color;                 ///< edge default color
        char   letter;                ///< edge default letter ('\0' means no letter)
        EventShape shape;             ///< edge default shape

        INT32  colorDfsNum;
        INT32  shapeDfsNum;
        INT32  letterDfsNum;

        ColorRuleList*  crlist;        ///< colors for <tag,value> specifications
        ShapeRuleList*  srlist;        ///< shapes for <tag,value> specifications
        LetterRuleList* lrlist;        ///< letter for <tag,value> specifications

        bool drawOnMove;              ///< draw on cycle or cycle+lat (true <=> on cycle)

        // for shading support
        QString shadingTbl;
        INT32   shadingTag;
        INT32   shadingMinValue;
        INT32   shadingMaxValue;
        INT32   shadingDfs;

        // for transparent colors
        bool isTransparent;
};

UINT16
RowRules::getId()
{ return id; }

UINT16
RowRules::getEdgeId()
{
    Q_ASSERT(rtype==SimpleEdgeRow);
    return id;
}

UINT16
RowRules::getNodeId()
{
    Q_ASSERT(rtype==SimpleNodeRow);
    return id;
}

INT32
RowRules::getRowPos()
{ return row; }

void
RowRules::setRowPos (INT32 value)
{ row=value; }

void
RowRules::setColor (QColor c, INT32 colorDfsNum, bool isTransp)
{
    this->colorDfsNum = colorDfsNum;
    this->color = c;
    this->isTransparent = isTransp;
}

void
RowRules::setShape (EventShape esh, INT32 shapeDfsNum)
{
    this->shapeDfsNum = shapeDfsNum;
    this->shape = esh;
}

void
RowRules::setLetter(char l, INT32 letterDfsNum)
{
    this->letterDfsNum = letterDfsNum;
    this->letter = l;
}

ColorRuleList*
RowRules::getCrlist()
{
    // lazy initialization
    if (crlist==NULL) { crlist = new ColorRuleList(32); }
    Q_ASSERT(crlist!=NULL);
    return crlist;
}

ShapeRuleList*
RowRules::getSrlist()
{
    // lazy initialization
    if (srlist==NULL) { srlist = new ShapeRuleList(32); }
    Q_ASSERT(srlist!=NULL);
    return srlist;
}

LetterRuleList*
RowRules::getLrlist()
{
    // lazy initialization
    if (lrlist==NULL) { lrlist = new LetterRuleList(32); }
    Q_ASSERT(lrlist!=NULL);
    return lrlist;
}

void
RowRules::setDrawOnMove(bool value)
{ drawOnMove = value; }

bool
RowRules::getDrawOnMove()
{ return drawOnMove; }


QColor 
RowRules::getColor() 
{ return color; }

EventShape 
RowRules::getShape() 
{ return shape; }

char 
RowRules::getLetter() 
{ return letter; }

INT32
RowRules::getColorDfsNum()
{return colorDfsNum;}

INT32
RowRules::getShapeDfsNum()
{return shapeDfsNum;}

INT32
RowRules::getLetterDfsNum()
{return letterDfsNum;}

RowType 
RowRules::getType()
{ return rtype; }

QString 
RowRules::getShadingTbl()
{ return shadingTbl; }

void
RowRules::setShadingTbl(QString _shadingTbl)
{ shadingTbl = _shadingTbl; }

INT32 
RowRules::getShadingTag()
{ return shadingTag; }

void
RowRules::setShadingTag(INT32 value)
{ shadingTag = value; }

INT32 
RowRules::getShadingMinValue()
{ return shadingMinValue; }

void
RowRules::setShadingMinValue(INT32 value)
{ shadingMinValue = value; }

INT32
RowRules::getShadingMaxValue()
{ return shadingMaxValue; }

void
RowRules::setShadingMaxValue(INT32 value)
{ shadingMaxValue = value; }

bool 
RowRules::getIsTransparent()
{ return isTransparent; }

INT32 
RowRules::getShadingDfsNum()
{ return shadingDfs; }

void
RowRules::setShadingDfsNum(INT32 value)
{ shadingDfs = value; }

/**
  * @typedef DGEList
  * @brief descrip.
  */
typedef QIntDict<RowRules> RRList;

#endif


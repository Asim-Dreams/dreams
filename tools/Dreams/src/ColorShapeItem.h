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
  * @file  ColorShapeItem.h
  */

#ifndef _COLORSHAPEITEM_H
#define _COLORSHAPEITEM_H

#include "DreamsDefs.h"
#include <qcolor.h>
#include <qptrlist.h>

#include "dDB/DralDBSyntax.h"
#include "EventShape.h"
#include "CSLData.h"
#include "AColorPalette.h"

/**
  * This class combines a pallete color and an event shape.
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class ColorShapeItem
{
    public:

        // ------------------------------------------------------------------------------
        // -- Constructos & destructors 
        // ------------------------------------------------------------------------------
        ColorShapeItem();
        ColorShapeItem(CSLData csldata);
        ColorShapeItem(EventShape sh, INT32 color, char letter, INT32 lcolor);
        ColorShapeItem(EventShape sh, INT32 color, char letter, INT32 lcolor, INT32 cycle, INT32 row);
        ColorShapeItem(EventShape sh, INT32 color, char letter, INT32 lcolor, INT32 cycle, INT32 row, INT32 column);
        ColorShapeItem(EventShape sh, INT32 fcolor, INT32 bcolor, char letter, INT32 cycle, INT32 row, INT32 column);

        ~ColorShapeItem();
        static ColorShapeItem * clone(ColorShapeItem);

        // ------------------------------------------------------------------------------
        // -- Operators 
        // ------------------------------------------------------------------------------
        inline bool operator==(const ColorShapeItem& csi) const;
        
        // ------------------------------------------------------------------------------
        // -- Consultors and modificators 
        // ------------------------------------------------------------------------------
        inline EventShape getShape();
        inline INT32 getColorIdx();
        inline INT32 getBColorIdx();
        inline char getLetter();
        inline INT32 getLetterColorIdx();
        inline INT32 getCycle();
        inline INT32 getRow();
        inline INT32 getColumn();
        inline bool  getTransparency();

        inline bool getFillColorSet();
        inline bool getBorderColorSet();
        inline bool getLetterColorSet();
        inline bool getShapeSet();
        inline bool getLetterSet();
        inline bool getFontSet();
        inline bool getTransparencySet();

        inline void setShape(EventShape);
        inline void setColorIdx(INT32);
        inline void setBColorIdx(INT32);
        inline void setLetter(char);
        inline void setLetterColorIdx(INT32);
        inline void setTransparency(bool value);

        inline void setCycle(INT32 cycleValue);
        inline void setRow(INT32 rowValue);
        inline void setColumn(INT32 colValue);

    protected:
        void defaultValues();

        QString toString();

    private:

        // flags to 'set' attributes
        UINT16 compute_mask;

        // we guess no more than 256 shapes:
        BYTE shape;

        // we index a color pallete
        UINT16 colorIdx;
        bool   isTransparent;   // transparent fill color

        // we index a color pallete
        UINT16 bcolorIdx;

        // a char is enough to store a letter :)
        char letter;

        // color for the letter
        UINT16 letterColorIdx;

        // only for special events
        UINT32 cycle;

        // Warning: column and row are required for Floorplan views only 
        UINT32 row;         
        UINT32 column;         // at most 64k events per cycle ...
};

// ------------------------------------------------------------------
// ------------------------------------------------------------------

bool
ColorShapeItem::operator==(const ColorShapeItem& csi) const
{
    bool c1 = (csi.shape == shape);
    bool c2 = (csi.colorIdx == colorIdx);
    bool c3 = (csi.bcolorIdx == bcolorIdx);
    bool c4 = (csi.letter == letter);
    bool c5 = (csi.letterColorIdx == letterColorIdx);

    return c1 && c2 && c3 && c4 && c5;
}

EventShape 
ColorShapeItem::getShape()
{
    return (EventShape) shape;
}

INT32
ColorShapeItem::getColorIdx()
{
    return colorIdx;
}

INT32
ColorShapeItem::getBColorIdx()
{
    return bcolorIdx;
}

char
ColorShapeItem::getLetter()
{
    return letter;
}

INT32
ColorShapeItem::getLetterColorIdx()
{
    return letterColorIdx;
} 

bool  
ColorShapeItem::getTransparency()
{
    return isTransparent;
}

INT32
ColorShapeItem::getCycle()
{
    return cycle;
}

INT32
ColorShapeItem::getRow()
{
    return (INT32) row;
}

INT32
ColorShapeItem::getColumn()
{
    return (INT32) column;
}

// ------------------------------------------------------------------

void
ColorShapeItem::setShape(EventShape value)
{
    compute_mask = compute_mask & ~CSL_SHAPE_MASK;
    shape = value;
}

void
ColorShapeItem::setColorIdx(INT32 value)
{
    compute_mask = compute_mask & ~CSL_FILL_COLOR_MASK;
    colorIdx = value;
}

void
ColorShapeItem::setBColorIdx(INT32 value)
{
    compute_mask = compute_mask & ~CSL_BORDER_COLOR_MASK;
    bcolorIdx = value;
}

void
ColorShapeItem::setLetter(char value)
{
    compute_mask = compute_mask & ~CSL_LETTER_MASK;
    letter = value;
}

void
ColorShapeItem::setLetterColorIdx(INT32 value)
{
    compute_mask = compute_mask & ~CSL_LETTER_COLOR_MASK;
    letterColorIdx = value;
}

void
ColorShapeItem::setCycle(INT32 cycleValue)
{
    cycle = cycleValue;
}

void
ColorShapeItem::setRow(INT32 rowValue)
{
    Q_ASSERT(rowValue < 65536);
    row = rowValue;
}

void
ColorShapeItem::setColumn(INT32 colValue)
{
    Q_ASSERT(colValue < 65536);
    column = colValue;
}

void
ColorShapeItem::setTransparency(bool value)
{
    compute_mask = compute_mask & ~CSL_TRANSPARENCY_MASK;
    isTransparent = value;
}

// ------------------------------------------------------------------

bool
ColorShapeItem::getFillColorSet()
{
    return (compute_mask & CSL_FILL_COLOR_MASK) == 0;
}

bool
ColorShapeItem::getBorderColorSet()
{
    return (compute_mask & CSL_BORDER_COLOR_MASK) == 0;
}

bool
ColorShapeItem::getLetterColorSet()
{
    return (compute_mask & CSL_LETTER_COLOR_MASK) == 0;
}

bool
ColorShapeItem::getShapeSet()
{
    return (compute_mask & CSL_SHAPE_MASK) == 0;
}

bool
ColorShapeItem::getLetterSet()
{
    return (compute_mask & CSL_LETTER_MASK) == 0;
}

bool
ColorShapeItem::getFontSet()
{
    return (compute_mask & CSL_FONT_MASK) == 0;
}

bool
ColorShapeItem::getTransparencySet()
{
    return (compute_mask & CSL_TRANSPARENCY_MASK) == 0;
}

#endif

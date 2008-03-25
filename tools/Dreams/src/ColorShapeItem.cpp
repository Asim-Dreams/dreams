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
  * @file ColorShapeItem.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "ColorShapeItem.h"

ColorShapeItem::ColorShapeItem()
{
    defaultValues();
}

ColorShapeItem::ColorShapeItem(CSLData csldata)
{
    defaultValues();
    // override with csldata that is defined
    if(csldata.getFillColorSet()) 
    {    
        setColorIdx(AColorPalette::createColor(csldata.getFillColor()));
        setTransparency(false);
    }
    if (csldata.getBorderColorSet())  setBColorIdx (AColorPalette::createColor(csldata.getBorderColor()));
    if (csldata.getLetterColorSet())  setLetterColorIdx (AColorPalette::createColor(csldata.getLetterColor()));
    if (csldata.getShapeSet())        setShape (csldata.getShape());
    if (csldata.getLetterSet())       setLetter (csldata.getLetter());
    if (csldata.getTransparencySet()) setTransparency(csldata.getTransparency());
    
    // convenience hack for letter color
    if (csldata.getLetterSet() && !csldata.getLetterColorSet() && csldata.getBorderColorSet())
    {
        // use border color to letter color
        setLetterColorIdx(bcolorIdx);
    }

    // copy the 'set' mask
    compute_mask = csldata.getComputeMask();
}


ColorShapeItem::ColorShapeItem(EventShape sh, INT32 color, char _letter, INT32 lcolor)
{
    defaultValues();
   
    setShape(sh);
    setColorIdx((UINT16)color);
    setLetter(_letter);
    setLetterColorIdx((UINT16)lcolor);
    setTransparency(false);
}

ColorShapeItem::ColorShapeItem(EventShape sh, INT32 color, char _letter, INT32 lcolor, INT32 cy, INT32 rw)
{
    defaultValues();
    
    setShape(sh);
    setColorIdx((UINT16)color);
    setLetter(_letter);
    setLetterColorIdx((UINT16)lcolor);
    setTransparency(false);
    setCycle(cy);
    setRow(rw);
}

ColorShapeItem::ColorShapeItem(EventShape sh, INT32 color, char _letter, INT32 lcolor,INT32 cy, INT32 rw, INT32 col)
{
    defaultValues();

    setShape(sh);
    setColorIdx((UINT16)color);
    setLetter(_letter);
    setLetterColorIdx((UINT16)lcolor);
    setTransparency(false);
    setCycle(cy);
    setRow(rw);
    setColumn(col);
}

ColorShapeItem::ColorShapeItem(EventShape sh, INT32 fcolor, INT32 bcolor, char _letter, INT32 cy, INT32 rw, INT32 col)
{
    defaultValues();

    setShape(sh);
    setColorIdx((UINT16)fcolor);
    setBColorIdx((UINT16)bcolor);
    setLetter(_letter);
    setTransparency(false);
    setCycle(cy);
    setRow(rw);
    setColumn(col);
}

ColorShapeItem::~ColorShapeItem()
{
}

ColorShapeItem *
ColorShapeItem::clone(ColorShapeItem c)
{
    ColorShapeItem * nobj = new ColorShapeItem();
    Q_ASSERT(nobj != NULL);

    // shallow copy is enought
    * nobj = c;
    return nobj;
}

QString
ColorShapeItem::toString()
{
    QString res;

    res =  "ColorShapeItem letter="  + QString(QChar(letter)) + "\n";
    return res;
}

void
ColorShapeItem::defaultValues()
{    
    // default values
    shape = EVENT_SHAPE_RECTANGLE;
    colorIdx = AColorPalette::createColor(QColor(0,0,0));
    isTransparent=true;
    bcolorIdx = AColorPalette::createColor(QColor(0,0,0));
    letter = ' ';
    letterColorIdx = AColorPalette::createColor(QColor(0,0,0));

    cycle = 0;
    row = 0;
    column = 0;

    // Marks all the elements as not computed.
    compute_mask = 0x007F;
}

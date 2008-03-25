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
  * @file RowDescriptor.cpp
  */

#include "RowDescriptor.h"

UINT64 RowDescriptor::_nonCachedCnt;
UINT64 RowDescriptor::_totalCnt;

double
RowDescriptor::getNonCachedProp()
{ return (double)_nonCachedCnt/(double)_totalCnt; }

RowDescriptor::RowDescriptor()
{
	cshlist = NULL;
	init (UninitializedRow); 
}

RowDescriptor::RowDescriptor(RowType type)
{
	cshlist = NULL;
	init (type); 
}

void
RowDescriptor::init(RowType type)
{
    rowtype = type;
    activeRow = true;
	if (cshlist!=NULL) { delete cshlist; }
    cshlist = new CSHList();Q_ASSERT(cshlist!=NULL);
    cshlist->setAutoDelete(true);
    nextColorShapeIdx = 1;       ///< 0 reserved
    desc = QString::null;
    node_id=0;
    edge_id=0;
    edge_pos=0;
    input_bw=0;
    output_bw=0;

    // reserve last cspair for non-complete info mark
    /** @todo make configurable the grey color ... */
    INT32 coloridx = AColorPalette::createColor(QColor(200,200,200));
    csitems[EITEM_NON_INIT_COLORSHAPE] = ColorShapeItem(EVENT_SHAPE_RECTANGLE,coloridx);
	
	// stats
    _nonCachedCnt = 0;
    _totalCnt = 0;
}

RowDescriptor::~RowDescriptor()
{ if (cshlist!=NULL) delete cshlist; }

ColorShapeItem
RowDescriptor::getCSH(INT32 col, INT32 row)
{
    //printf (">> RowDescriptor::getCSH col = %d, row = %d\n",col,row);

    // look for a special color-shape combination
    bool fnd = false;
     ColorShapeItem* item=NULL;

    item = cshlist->first();
    while (!fnd && (item!=NULL) )
    {
        fnd = (item->getCycle()==col) && (item->getRow()==row);
        if (!fnd) item = cshlist->next();
    }

    Q_ASSERT(fnd);
    return (*item);
}

void
RowDescriptor::reset()
{
	init (UninitializedRow);
}


// -------------------------------------------------------------------
// AMemObj methods
// -------------------------------------------------------------------

INT64
RowDescriptor::getObjSize() const
{
    INT64 result = sizeof(RowDescriptor);
    if (desc!=QString::null) result+= desc.length()*sizeof(QChar);
    if (cshlist!=NULL)       result+= cshlist->count()*(sizeof(ColorShapeItem)+3*sizeof(void*));
    
    return (result);
}

QString
RowDescriptor::getUsageDescription() const
{
    return QString::null;
}



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
  * @file RowDescVector.cpp
  */

#include "RowDescVector.h"

RowDescVector::RowDescVector()
{
    rowvector = new RowDescriptor[COLDESC_MAX_EVENTS];
    Q_ASSERT(rowvector!=NULL);
    maxRows = COLDESC_MAX_EVENTS;
    numRows = 0;
}

RowDescVector::~RowDescVector()
{
    if (rowvector!=NULL) delete [] rowvector;
}

void
RowDescVector::reset()
{
    for (INT32 i=0;i<numRows;i++)
    {
        rowvector[i].reset();
    }
    numRows = 0;
}


INT64 
RowDescVector::getObjSize() const
{
    INT64 result = sizeof(RowDescVector);
    result += sizeof(RowDescriptor)*maxRows;
    return (result);
}

QString
RowDescVector::getUsageDescription() const
{ return NULL; }

QString
RowDescVector::getStats() const
{
    QString result = "";
    double usageProp = ((double)numRows/(double)maxRows)*100.0;
    result += "\t\tDescriptors Usage:\t\t\t"+QString::number(numRows)+" from "+QString::number(maxRows)+" ("+QString::number(usageProp)+"%)\n";
    result += "\t\tNon Cached Color/Shape Combinations:\t"+QString::number(RowDescriptor::getNonCachedProp()*100.0)+"%\n";
    return (result);
}


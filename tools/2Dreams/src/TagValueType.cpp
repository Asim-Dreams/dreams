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
  * @file TagValueType.cpp
  */
#include "TagValueType.h"
#include "ADFDefinitions.h"

bool
TagValueTypeToolkit::parseValueType(QString str, TagValueType* result)
{
    bool ret = true;

    if      (str==ADF_TYPE_STRING)
    { *result = TagStringValue; }
    else if (str==ADF_TYPE_INTEGER)
    { *result = TagIntegerValue; }
    else if (str==ADF_TYPE_FP)
    { *result = TagFPValue; }
    else if (str==ADF_TYPE_SETOFVALUES)
    { *result = TagSetOfValues; }
    else
    { ret = false; }

    return (ret);
}

QString
TagValueTypeToolkit::toString(TagValueType value)
{
    QString result;
    switch(value)
    {
        case TagStringValue:
             result =  QString(ADF_TYPE_STRING);
             break;
        case TagIntegerValue:
             result =  QString(ADF_TYPE_INTEGER);
             break;
        case TagFPValue:
             result =  QString(ADF_TYPE_FP);
             break;
        case TagSetOfValues:
             result =  QString(ADF_TYPE_SETOFVALUES);
             break;
        default:
             result =  QString("unknown tagvaluetype");
    }
    return result;
}


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
  * @file TagValueType.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "TagValueType.h"
#include "adf/ADFDefinitions.h"

bool
TagValueToolkit::parseValueType(QString str, TagValueType* result)
{
    bool ret = true;

    if      (str==ADF_TYPE_STRING)
    { *result = TAG_STRING_TYPE; }
    else if (str==ADF_TYPE_INTEGER)
    { *result = TAG_INTEGER_TYPE; }
    else if (str==ADF_TYPE_FP)
    { *result = TAG_FP_TYPE; }
    else if (str==ADF_TYPE_SETOFVALUES)
    { *result = TAG_SOV_TYPE; }
    else
    { ret = false; }

    return (ret);
}

QString
TagValueToolkit::toString(TagValueType value)
{
    QString result;
    switch(value)
    {
        case TAG_STRING_TYPE:
             result =  QString(ADF_TYPE_STRING);
             break;
        case TAG_INTEGER_TYPE:
             result =  QString(ADF_TYPE_INTEGER);
             break;
        case TAG_FP_TYPE:
             result =  QString(ADF_TYPE_FP);
             break;
        case TAG_SOV_TYPE:
             result =  QString(ADF_TYPE_SETOFVALUES);
             break;
        default:
             result =  QString("unknown tagvaluetype");
    }
    return result;
}

bool
TagValueToolkit::parseValueBase(QString str, TagValueBase * base)
{
    if((str == "2") || (str == "BINARY"))
    {
        * base = TAG_BASE_BINARY;
    }
    else if((str == "8") || (str == "OCTAL"))
    {
        * base = TAG_BASE_OCTAL;
    }
    else if((str == "10") || (str == "DECIMAL"))
    {
        * base = TAG_BASE_DECIMAL;
    }
    else if((str == "16") || (str == "HEXA"))
    {
        * base = TAG_BASE_HEXA;
    }
    else
    {
        return false;
    }
    return true;
}

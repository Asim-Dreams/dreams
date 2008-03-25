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

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "adf/ADFDefinitions.h"
#include "EventShape.h"

bool
EventShapeToolkit::parseShape(QString str, EventShape *shape)
{
    bool result = true;

    if      (str==ADF_SHAPE_RECTANGLE)
    { *shape = EVENT_SHAPE_RECTANGLE; }
    else if (str==ADF_SHAPE_CIRCLE)
    { *shape = EVENT_SHAPE_CIRCLE; }
    else if (str==ADF_SHAPE_TRIANGLE)
    { *shape = EVENT_SHAPE_TRIANGLE; }
    else if (str==ADF_SHAPE_RHOMB)
    { *shape = EVENT_SHAPE_RHOMB; }
    else
    { result = false ;}

    return (result);
}

QString
EventShapeToolkit::toString(EventShape shape)
{
    QString result;
    switch (shape)
    {
        case EVENT_SHAPE_RECTANGLE:
             result =  QString(ADF_SHAPE_RECTANGLE);
             break;
        case EVENT_SHAPE_CIRCLE:
             result =  QString(ADF_SHAPE_CIRCLE);
             break;
        case EVENT_SHAPE_TRIANGLE:
             result =  QString(ADF_SHAPE_TRIANGLE);
             break;
        case EVENT_SHAPE_RHOMB:
             result =  QString(ADF_SHAPE_RHOMB);
             break;
        default:
             result =  QString("Unknown shape class");
    }
    return (result);
}



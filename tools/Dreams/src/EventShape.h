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

/*
 * @file  EventShape.h
 */

#ifndef _EVENTSHAPE_H
#define _EVENTSHAPE_H

#include "DreamsDefs.h"
// QT library
#include <qstring.h>

/**
  * Simple enumeration with known shape types.
  *       
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
typedef enum
{
    EVENT_SHAPE_RECTANGLE = 0,
    EVENT_SHAPE_CIRCLE,
    EVENT_SHAPE_TRIANGLE,
    EVENT_SHAPE_RHOMB
} EventShape;

class EventShapeToolkit
{
    public:
        static bool parseShape(QString str, EventShape * shape);
        static QString toString(EventShape shape);
};

#endif

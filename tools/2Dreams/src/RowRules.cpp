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
  * @file  RowRules.cpp
  * @brief
  */

#include "RowRules.h"

RowRules::RowRules (UINT16 _id, UINT16 _row, RowType _rtype)
{
    row = _row;
    id = _id;
    rtype = _rtype;

    // ADF parameters unknown at this point so default values
    color = QColor(255,255,255);
    letter = '\0';
    shape  = EVENT_SHAPE_RECTANGLE;
    drawOnMove  = true;
    
    shadingTbl = QString::null;
    shadingTag = -1;
    isTransparent = false;

    crlist = NULL;
    srlist = NULL;
    lrlist = NULL;

    colorDfsNum=0;
    shapeDfsNum=0;
    letterDfsNum=0;
    shadingDfs=0;
}

RowRules::~RowRules()
{
    if (crlist!=NULL) { delete crlist; }
    if (srlist!=NULL) { delete srlist; }
    if (lrlist!=NULL) { delete lrlist; }
}



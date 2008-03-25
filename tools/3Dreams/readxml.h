/*****************************************************************************
*
* @brief XML configuration file parsing helper functions. Header File.
*
* @author Oscar Rosell
*
*  Copyright (C) 2003-2006 Intel Corporation
*  
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*  
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*  
*
*****************************************************************************/

#ifndef READXML_H
#define READXML_H

#include "taginspector.h"
#include <qdom.h>

bool readConfigurationSetElement(const QDomElement& elem, PaintEvent** pe);
bool readConfigurationConditionElement(const QDomElement& elem, TagInspectorRoot** ti);
bool readConfigurationAndElement(const QDomElement& elem, TagInspectorRoot** ti);
bool readConfigurationOrElement(const QDomElement& elem, TagInspectorRoot** ti);
bool readConfigurationAndElement(const QDomElement& elem, TagInspectorRoot** ti);
bool readConfigurationIfElement(const QDomElement& elem, TagInspectorRoot** ti);
bool readConfigurationThenElement(const QDomElement& elem, TagInspectorRoot** ti);
bool readConfigurationIfThenElement(const QDomElement& elem, TagInspectorRoot** ti);

#endif

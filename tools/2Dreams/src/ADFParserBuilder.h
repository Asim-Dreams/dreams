// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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
  * @file ADFParserBuilder.h
  */

#ifndef _ADFPARSERBUILDER_H
#define _ADFPARSERBUILDER_H

// Qt library
#include <qstring.h>
#include <qdom.h>

// 2Draems
#include "ADFParserInterface.h"
#include "asim/DralDB.h"

#define MINADFVERSION 100   // 1.0
#define MAXADFVERSION 200   // 2.0

class ADFParserBuilder
{
    public:
        static ADFParserInterface* getSuitableParser(QDomDocument* document);
        static QString getErrorMsg();

    private:
        static QString errorMsg;
        static QString supportedVersions;
};


#endif


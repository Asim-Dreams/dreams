// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
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

// Dreams includes.
#include "DreamsDefs.h"

// Qt includes.
#include <qstring.h>

#define MINADFVERSION 100   // 1.0
#define MAXADFVERSION 250   // 2.5

// Class forwarding.
class ADFParserInterface;
class QDomDocument;
class DreamsDB;

class ADFParserBuilder
{
    public:
        ADFParserBuilder();

        ADFParserInterface * getSuitableParser(DreamsDB * dreamsdb, LogMgr * logmgr, QDomDocument * document);
        QString getErrorMsg();

    private:
        QString errorMsg;
        QString supportedVersions;
};

#endif

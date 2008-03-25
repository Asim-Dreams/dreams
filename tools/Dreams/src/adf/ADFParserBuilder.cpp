/*
 * Copyright (C) 2005-2006 Intel Corporation
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
  * @file AProgressDialog.cpp
  */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/LogMgr.h"
#include "ADFParserBuilder.h"
#include "ADFDefinitions.h"
#include "ADFParser.h"
#include "DreamsDB.h"

ADFParserBuilder::ADFParserBuilder()
{
    errorMsg = QString::null;
    supportedVersions = QString("Supported ADF Versions: 2.7");
}

ADFParserInterface *
ADFParserBuilder::getSuitableParser(DreamsDB * dreamsdb, LogMgr * logmgr, QDomDocument * document)
{
    if(document == NULL)
    {
        errorMsg = "Parsing error, NULL ADF!!";
        return NULL;
    }

    QDomElement docElem = document->documentElement(); // get root tag
    QString tname = docElem.tagName();

    // DEBUG
    //cerr << "ADFParserBuilder::getSuitableParser: docElem.tagName() = " << tname << endl;

    if((tname != ADF_2DREAMS) && (tname != ADF_DREAMS))
    {
        errorMsg = "Parsing error reading ADF: unrecognized XML structure. Is this really an ADF?";
        return NULL;
    }

    QString strversion = docElem.attribute("version");

    //cerr << "ADFParserBuilder::getSuitableParser: docElem.version() = " << strversion << endl;

    if(strversion == QString::null)
    {
        errorMsg = "Parsing error reading ADF: missing version attribute.";
        return NULL;
    }

    INT32 major, minor, nversion;
    INT32 match = sscanf(strversion.latin1(), "%d.%d", &major, &minor);
    nversion = major * 100 + minor;
    
    if(match != 2)
    {
        errorMsg = "Parsing error reading ADF: unrecognized version attribute encoding.";
        return NULL;
    }
    //printf("ADFParserBuilder::getSuitableParser computed adf version %d",nversion);fflush(stdout);
    if((nversion < MINADFVERSION) || (nversion > MAXADFVERSION))
    {
        char ctmp[128];
        sprintf(ctmp, "Unable to create a suitable parser for and ADF version %d.%d.\n", major, minor);
        errorMsg = QString(ctmp) + supportedVersions;
        return NULL;
    }

    //cerr << "ADFParserBuilder::getSuitableParser: nversion() = " << nversion << endl;

    // asume one parser for each major version
    ADFParserInterface * result = NULL;
    switch(nversion)
    {
        case 100:
            //result = new ADFParser1(dreamsdb, logmgr, document);
            logmgr->addLog("ADF Major Version 1. Backward compatibility not implemented. Use Dreams if you want to visualize old traces.");
            //cerr << "ADF Major Version 1. Backward compatibility not implemented. Use Dreams if you want to visualize old traces." << endl;
            break;

        case 200:
            //result = new ADFParser2(dreamsdb, logmgr, document);
            logmgr->addLog("ADF Version 2.0. Backward compatibility not implemented. Use Dreams if you want to visualize old traces.");
            //cerr << "ADF Major Version 2.0. Backward compatibility not implemented. Use Dreams if you want to visualize old traces." << endl;
            break;

        case 207:
            result = new ADFParser(dreamsdb, logmgr, document);
            logmgr->addLog("ADF Version 2.7");
            break;

        default:
            Q_ASSERT(false);
    }

    return result;
}

QString
ADFParserBuilder::getErrorMsg()
{
    return errorMsg;
}

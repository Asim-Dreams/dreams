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
  * @file AProgressDialog.cpp
  */

// 2Dreams
#include "ADFParserBuilder.h"

// include all known parser implementations
#include "ADFParser1.h"
#include "ADFParser2.h"


ADFParserInterface*
ADFParserBuilder::getSuitableParser(QDomDocument* document)
{
    if (document==NULL)
    {
      errorMsg = "Parsing error, NULL ADF!!";
      return (NULL);
    }

    QDomElement docElem = document->documentElement(); // get root tag
    //Q_ASSERT(docElem!=NULL);
    QString tname = docElem.tagName();
    if (tname!=ADF_2DREAMS)
    {
      errorMsg = "Parsing error reading ADF: unrecognized XML structure. Is this really an ADF?";
      return (NULL);
    }

    QString strversion = docElem.attribute("version");
    if (strversion==QString::null)
    {
      errorMsg = "Parsing error reading ADF: missing version attribute.";
      return (NULL);
    }

    int major,minor,nversion;
    int match = sscanf(strversion.latin1(),"%d.%d",&major,&minor);
    nversion = major*100+minor;
    if (match!=2)
    {
      errorMsg = "Parsing error reading ADF: unrecognized version attribute encoding.";
      return (NULL);
    }
    //printf("ADFParserBuilder::getSuitableParser computed adf version %d",nversion);fflush(stdout);
    if ((nversion<MINADFVERSION) || (nversion>MAXADFVERSION) )
    {
        char ctmp[128];
        sprintf(ctmp,"Unable to create a suitable parser for and ADF version %d.%d.\n",major,minor);
        errorMsg = QString(ctmp)+supportedVersions;
        return (NULL);
    }

    // asume one parser for each major version
    ADFParserInterface* result = NULL;
    switch (major)
    {
        case 1:
            result = new ADFParser1(document);
            LogMgr::getInstance()->addLog("ADF Major Version 1. Backward compatibility mode activated.");
            LogMgr::getInstance()->addLog("Warning! ADF 1 Shadowing compatibility not implement yet.");
            LogMgr::getInstance()->addLog("Occupancy rows will be only shown with ADF2.");
            break;
        case 2:
            result = new ADFParser2(document);
            LogMgr::getInstance()->addLog("ADF Major Version 2.");
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

QString ADFParserBuilder::errorMsg = QString::null;
QString ADFParserBuilder::supportedVersions = QString("Supported ADF Versions: 1.0, 1.1 and 2.0");


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
  * @file PreferenceMgr.cpp
  */

// General includes.
#include <stdlib.h>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "PreferenceMgr.h"

// Qt includes.
#include <qstringlist.h>
#include <qapplication.h>
#include <qsettings.h>

PreferenceMgr* PreferenceMgr::_myInstance=NULL;

// ------------------------------------------------------------------
// -- Singleton pattern methods
// ------------------------------------------------------------------
PreferenceMgr * PreferenceMgr::getInstance(QString companyName)
{
    if(_myInstance == NULL)
    {
        _myInstance = new PreferenceMgr(companyName);
        Q_ASSERT(_myInstance != NULL);
    }
    return _myInstance;

}

PreferenceMgr * PreferenceMgr::getInstance()
{
    if(_myInstance == NULL)
    {
        _myInstance = new PreferenceMgr("BSSAD");
        Q_ASSERT(_myInstance != NULL);
    }
    return _myInstance;
}

// ------------------------------------------------------------------
// -- Constructors & co.
// ------------------------------------------------------------------
PreferenceMgr::PreferenceMgr(QString companyName)
{
    companyNamePrefix = companyName;
    applicationPrefix = QString("/Dreams/"); // By now hardcoded app name

    //favoriteTags = NULL;
    //itemWindowTags = NULL;
}

PreferenceMgr::~PreferenceMgr()
{
}

// ------------------------------------------------------------------
// -- Save & restore
// ------------------------------------------------------------------
void
PreferenceMgr::savePreferences()
{
    //printf ("savePreferences called...\n");

    QString prefix = "/" + companyNamePrefix;
    settings = new QSettings();Q_ASSERT(settings!=NULL);
    // If running under windows 'company' prefix is mandatory...
    settings->insertSearchPath(QSettings::Windows,prefix);

    saveDBasePreferences();
    saveGridLinePreferences();
    saveAppearencePreferences();
    saveDockedWindowsPreferences();
    saveDockedToolbarsPreferences();
    saveMiscPreferences();

    delete settings;
}

bool
PreferenceMgr::loadPreferences()
{
    //printf ("loadPreferences called...\n");

    QString prefix = "/" + companyNamePrefix;
    settings = new QSettings();Q_ASSERT(settings!=NULL);
    // If running under windows 'company' prefix is mandatory...
    settings->insertSearchPath(QSettings::Windows,prefix);

    // Use default values if the keys don't exist !!!!!
    bool ok = true;

    ok = ok && loadDBasePreferences();
    ok = ok && loadGridLinePreferences();
    ok = ok && loadAppearencePreferences();
    ok = ok && loadDockedWindowsPreferences();
    ok = ok && loadDockedToolbarsPreferences();
    ok = ok && loadMiscPreferences();

    delete settings;
    return (ok);
}

bool
PreferenceMgr::loadDBasePreferences()
{
    maxIFI = (UINT32)
    settings->readNumEntry(applicationPrefix+"Dbase/" + "maxIFI", 0);

    maxIFIEnabled = settings->readBoolEntry(applicationPrefix+"Dbase/" + "maxIFIEnabled",FALSE);
    ignoreEdgeBandwidthConstrain = settings->readBoolEntry(applicationPrefix+"Dbase/" + "ignoreEdgeBandwidthConstrain",FALSE);

    return (true);
}

bool
PreferenceMgr::loadGridLinePreferences()
{
    showHGridLines = settings->readBoolEntry(applicationPrefix+"Gridlines/" + "showHGridLines",false);
    showVGridLines = settings->readBoolEntry(applicationPrefix+"Gridlines/" + "showVGridLines",false);
    snapToGrid = settings->readBoolEntry(applicationPrefix+"Gridlines/" + "snapToGrid",true);
    snapGap = (UINT32)settings->readNumEntry(applicationPrefix+"Gridlines/"+"snapGap",90);
    return (true);
}

bool
PreferenceMgr::loadAppearencePreferences()
{
    fontSize = settings->readNumEntry(applicationPrefix+"Appearence/"+"fontSize",qApp->font().pointSize());

    mainWindowX = settings->readNumEntry(applicationPrefix+"Appearence/"+"mainWindowX",0);
    mainWindowY = settings->readNumEntry(applicationPrefix+"Appearence/"+"mainWindowY",0);
    mainWindowWidth = settings->readNumEntry(applicationPrefix+"Appearence/"+"mainWindowWidth",997);
    mainWindowHeight= settings->readNumEntry(applicationPrefix+"Appearence/"+"mainWindowHeight",704);

    QString backgroundColorString =
            settings->readEntry(applicationPrefix+"Appearence/"+"backgroundColor","#DCDCDC");
    backgroundColor = QColor(backgroundColorString);

    QString shadingColorString =
            settings->readEntry(applicationPrefix+"Appearence/"+"shadingColor","#9C9C9C");
    shadingColor = QColor(shadingColorString);

    guiStyle = settings->readEntry(applicationPrefix+"Appearence/"+"guiStyle2","Dreams");

    highlightSize = settings->readNumEntry(applicationPrefix+"Appearence/"+"highlightSize",2);

    return backgroundColor.isValid() && shadingColor.isValid();
}

bool
PreferenceMgr::loadDockedWindowsPreferences()
{
        disassemblyWindowHeight =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"disassemblyWindowHeight",468);

        tagWindowHeight =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"tagWindowHeight",150);

        relationshipWindowHeight =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"relationshipWindowHeight",150);

        logWindowHeight =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"logWindowHeight",80);

        // --

        disassemblyWindowWidth =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"disassemblyWindowWidth",256);

        tagWindowWidth =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"tagWindowWidth",256);

        relationshipWindowWidth =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"relationshipWindowWidth",256);

        logWindowWidth =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"logWindowWidth",500);

        // --

        disassemblyWindowIdx =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"disassemblyWindowIdx",0);

        tagWindowIdx =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"tagWindowIdx",1);

        relationshipWindowIdx =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"relationshipWindowIdx",2);

        logWindowIdx =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"logWindowIdx",0);

        // --

        disassemblyWindowOffset =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"disassemblyWindowOffset",0);

        tagWindowOffset =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"tagWindowOffset",0);

        relationshipWindowOffset =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"relationshipWindowOffset",0);

        logWindowOffset =
        settings->readNumEntry(applicationPrefix+"DockedWindows/"+"logWindowOffset",0);

        // --

        disassemblyWindowNl =
        settings->readBoolEntry(applicationPrefix+"DockedWindows/"+"disassemblyWindowNl",false);

        tagWindowNl =
        settings->readBoolEntry(applicationPrefix+"DockedWindows/"+"tagWindowNl",false);

        relationshipWindowNl =
        settings->readBoolEntry(applicationPrefix+"DockedWindows/"+"relationshipWindowNl",false);

        logWindowNl =
        settings->readBoolEntry(applicationPrefix+"DockedWindows/"+"logWindowNl",false);

        // --

        showDisassemblyWindow =
        settings->readBoolEntry(applicationPrefix+"DockedWindows/"+"showDisassemblyWindow",true);

        showTagWindow =
        settings->readBoolEntry(applicationPrefix+"DockedWindows/"+"showTagWindow",true);

        showRelationshipWindow =
        settings->readBoolEntry(applicationPrefix+"DockedWindows/"+"showRelationshipWindow",false);

        showLogWindow =
        settings->readBoolEntry(applicationPrefix+"DockedWindows/"+"showLogWindow",false);

        showPushoutWindow =
        settings->readBoolEntry(applicationPrefix+"DockedWindows/"+"showPushoutWindow",false);

        // --

        QString DisassemblyWindowDockPolicyString =
        settings->readEntry(applicationPrefix+"DockedWindows/"+"disassemblyWindowDockPolicy","DockRight");

        QString TagWindowDockPolicyString =
        settings->readEntry(applicationPrefix+"DockedWindows/"+"tagWindowDockPolicy","DockRight");

        QString RelationshipWindowDockPolicyString =
        settings->readEntry(applicationPrefix+"DockedWindows/"+"relationshipWindowDockPolicy","DockRight");

        QString LogWindowDockPolicyString =
        settings->readEntry(applicationPrefix+"DockedWindows/"+"logWindowDockPolicy","DockBottom");

        // parse dock policies
        bool ok = true;
        ok = ok && parseDock(DisassemblyWindowDockPolicyString,disassemblyWindowDockPolicy);
        ok = ok && parseDock(TagWindowDockPolicyString,tagWindowDockPolicy);
        ok = ok && parseDock(RelationshipWindowDockPolicyString,relationshipWindowDockPolicy);
        ok = ok && parseDock(LogWindowDockPolicyString,logWindowDockPolicy);

        return (ok);
}


bool
PreferenceMgr::loadDockedToolbarsPreferences()
{
        toolsToolbarIdx =
        settings->readNumEntry(applicationPrefix+"DockedToolbars/"+"toolsToolbarIdx",5);

        fileToolbarIdx =
        settings->readNumEntry(applicationPrefix+"DockedToolbars/"+"fileToolbarIdx",1);

        viewToolbarIdx =
        settings->readNumEntry(applicationPrefix+"DockedToolbars/"+"viewToolbarIdx",2);

        annotationsToolbarIdx =
        settings->readNumEntry(applicationPrefix+"DockedToolbars/"+"annotationsToolbarIdx",3);

        colorToolbarIdx =
        settings->readNumEntry(applicationPrefix+"DockedToolbars/"+"colorToolbarIdx",4);

        mdiToolbarIdx =
        settings->readNumEntry(applicationPrefix+"DockedToolbars/"+"mdiToolbarIdx",6);

        // --

        toolsToolbarOffset =
        settings->readNumEntry(applicationPrefix+"DockedToolbars/"+"toolsToolbarOffset",0);

        fileToolbarOffset =
        settings->readNumEntry(applicationPrefix+"DockedToolbars/"+"fileToolbarOffset",0);

        viewToolbarOffset =
        settings->readNumEntry(applicationPrefix+"DockedToolbars/"+"viewToolbarOffset",0);

        annotationsToolbarOffset =
        settings->readNumEntry(applicationPrefix+"DockedToolbars/"+"annotationsToolbarOffset",0);

        colorToolbarOffset =
        settings->readNumEntry(applicationPrefix+"DockedToolbars/"+"colorToolbarOffset",0);

        mdiToolbarOffset =
        settings->readNumEntry(applicationPrefix+"DockedToolbars/"+"mdiToolbarOffset",0);

        // --

        toolsToolbarNl =
        settings->readBoolEntry(applicationPrefix+"DockedToolbars/"+"toolsToolbarNl",false);

        fileToolbarNl =
        settings->readBoolEntry(applicationPrefix+"DockedToolbars/"+"fileToolbarNl",false);

        viewToolbarNl =
        settings->readBoolEntry(applicationPrefix+"DockedToolbars/"+"viewToolbarNl",false);

        annotationsToolbarNl =
        settings->readBoolEntry(applicationPrefix+"DockedToolbars/"+"annotationsToolbarNl",false);

        colorToolbarNl =
        settings->readBoolEntry(applicationPrefix+"DockedToolbars/"+"colorToolbarNl",false);

        mdiToolbarNl =
        settings->readBoolEntry(applicationPrefix+"DockedToolbars/"+"mdiToolbarNl",false);

        // --

        showToolsToolbar =
        settings->readBoolEntry(applicationPrefix+"DockedToolbars/"+"showToolsToolbar",true);

        showFileToolbar =
        settings->readBoolEntry(applicationPrefix+"DockedToolbars/"+"showFileToolbar",true);

        showViewToolbar =
        settings->readBoolEntry(applicationPrefix+"DockedToolbars/"+"showViewToolbar",true);

        showAnnotationsToolbar =
        settings->readBoolEntry(applicationPrefix+"DockedToolbars/"+"showAnnotationsToolbar",false);

        showColorToolbar =
        settings->readBoolEntry(applicationPrefix+"DockedToolbars/"+"showColorToolbar ",false);

        showMDIToolbar =
        settings->readBoolEntry(applicationPrefix+"DockedToolbars/"+"showMDIToolbar ",false);

        // --

        QString toolsToolbarDockPolicyString =
        settings->readEntry(applicationPrefix+"DockedToolbars/"+"toolsToolbarDockPolicy","DockTop");

        QString fileToolbarDockPolicyString =
        settings->readEntry(applicationPrefix+"DockedToolbars/"+"fileToolbarDockPolicy","DockTop");

        QString viewToolbarDockPolicyString =
        settings->readEntry(applicationPrefix+"DockedToolbars/"+"viewToolbarDockPolicy","DockTop");

        QString annotationsToolbarDockPolicyString =
        settings->readEntry(applicationPrefix+"DockedToolbars/"+"annotationsToolbarDockPolicy","DockTop");

        QString colorToolbarDockPolicyString =
        settings->readEntry(applicationPrefix+"DockedToolbars/"+"colorToolbarDockPolicy","DockTop");

        QString mdiToolbarDockPolicyString =
        settings->readEntry(applicationPrefix+"DockedToolbars/"+"mdiToolbarDockPolicy","DockTop");

        // parse dock policies
        bool ok = true;
        ok = ok && parseDock(toolsToolbarDockPolicyString,toolsToolbarDockPolicy);
        ok = ok && parseDock(fileToolbarDockPolicyString,fileToolbarDockPolicy);
        ok = ok && parseDock(viewToolbarDockPolicyString,viewToolbarDockPolicy);
        ok = ok && parseDock(annotationsToolbarDockPolicyString,annotationsToolbarDockPolicy);
        ok = ok && parseDock(colorToolbarDockPolicyString,colorToolbarDockPolicy);
        ok = ok && parseDock(mdiToolbarDockPolicyString,mdiToolbarDockPolicy);

        return (ok);
}

bool
PreferenceMgr::loadMiscPreferences()
{
        // Default tag used for Relationship scanning
        relationshipTag =
        settings->readEntry(applicationPrefix+"relationShipTag","ANCESTOR");

        // Favorite Tags
        bool favoriteTagsOk;
        QStringList favoriteTags_string =
                    settings->readListEntry(applicationPrefix+"favoriteTags",',',&favoriteTagsOk);
        if (!favoriteTagsOk)
        {
            // default tag list
            favoriteTags.clear();
            favoriteTags.append("DID");
            favoriteTags.append("UID");
            favoriteTags.append("ITEMID");
            favoriteTags.append("DIS");
            favoriteTags.append("VPC");
        }
        else
        {
            // default tag list
            favoriteTags.clear();
            QStringList::iterator it;
            for ( it = favoriteTags_string.begin(); it != favoriteTags_string.end(); ++it )
            {
                favoriteTags.append(*it);
            }
        }

        // Default Architecture definition file
        architectureDefinitionFile =
        settings->readEntry(applicationPrefix+"architectureDefinitionFile","defaultADF.xml");

        performValidation=
        settings->readBoolEntry(applicationPrefix+"performValidation",false);

        validationCommand =
        settings->readEntry(applicationPrefix+"validationCommand","xmllint --noout --valid");

        // log file
        QString basePath="";
#ifdef Q_OS_UNIX
        char* homedir = getenv("HOME");
        basePath += QString(homedir) + "/.";
#endif
        logFileName =
        settings->readEntry(applicationPrefix + "logFileName", basePath + "DreamsLog.txt");

        // smart filtering (on item window)
        smartFiltering = settings->readBoolEntry(applicationPrefix+"smartFiltering",false);

        autoPurge = settings->readBoolEntry(applicationPrefix+"autoPurge",true);
        incrementalPurge = settings->readBoolEntry(applicationPrefix+"incrementalPurge",false);
        itemMaxAge = settings->readNumEntry(applicationPrefix+"itemMaxAge",1000);
        tagWinFavouriteOnly = settings->readBoolEntry(applicationPrefix+"tagWinFavouriteOnly",false);
        lastUsedDir = settings->readEntry(applicationPrefix+"lastUsedDir",".");
        keepSynchTabs = settings->readBoolEntry(applicationPrefix+"keepSynchTabs",true);
        relaxedEENode = settings->readBoolEntry(applicationPrefix+"relaxedEENode",true);
        tagBackPropagate = settings->readBoolEntry(applicationPrefix+"tagBackPropagate",true);
        compressMutableTags = settings->readBoolEntry(applicationPrefix+"compressMutableTags",true);

        return (true);
}

bool
PreferenceMgr::parseDock(QString value, Dock &dockPolicy)
{
    bool result = true;

    // we only support basic topb,bottom... placements
    if (value == "DockRight")
    {
        dockPolicy = DockRight;
    }
    else if (value == "DockLeft")
    {
        dockPolicy = DockLeft;
    }
    else if (value == "DockTop")
    {
        dockPolicy = DockTop;
    }
    else if (value == "DockBottom")
    {
        dockPolicy = DockBottom;
    }
    else if (value == "DockTornOff")
    {
        dockPolicy = DockBottom; // hack
    }
    else
    {
        result = false;
    }
    return (result);
}

void
PreferenceMgr::saveDBasePreferences()
{
    settings->writeEntry(applicationPrefix+"Dbase/"+"maxIFI",(int)maxIFI);
    settings->writeEntry(applicationPrefix+"Dbase/"+"maxIFIEnabled",maxIFIEnabled);
    settings->writeEntry(applicationPrefix+"Dbase/"+"ignoreEdgeBandwidthConstrain",ignoreEdgeBandwidthConstrain);
}

void
PreferenceMgr::saveGridLinePreferences()
{
    settings->writeEntry(applicationPrefix+"Gridlines/" + "showVGridLines",showVGridLines);
    settings->writeEntry(applicationPrefix+"Gridlines/" + "showHGridLines",showHGridLines);
    settings->writeEntry(applicationPrefix+"Gridlines/" + "snapToGrid",snapToGrid);
    settings->writeEntry(applicationPrefix+"Gridlines/" + "snapGap",(int)snapGap);
}

void
PreferenceMgr::saveAppearencePreferences()
{
    settings->writeEntry(applicationPrefix+"Appearence/" + "mainWindowX",mainWindowX);
    settings->writeEntry(applicationPrefix+"Appearence/" + "mainWindowY",mainWindowY);
    settings->writeEntry(applicationPrefix+"Appearence/" + "mainWindowWidth",mainWindowWidth);
    settings->writeEntry(applicationPrefix+"Appearence/" + "mainWindowHeight",mainWindowHeight);
    settings->writeEntry(applicationPrefix+"Appearence/" + "backgroundColor",backgroundColor.name());
    settings->writeEntry(applicationPrefix+"Appearence/" + "shadingColor",shadingColor.name());
    settings->writeEntry(applicationPrefix+"Appearence/" + "fontSize",fontSize);
    settings->writeEntry(applicationPrefix+"Appearence/" + "guiStyle2",guiStyle);
    settings->writeEntry(applicationPrefix+"Appearence/" + "highlightSize",highlightSize);
}

void
PreferenceMgr::saveDockedWindowsPreferences()
{
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"disassemblyWindowHeight",disassemblyWindowHeight);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"tagWindowHeight",tagWindowHeight);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"relationshipWindowHeight",relationshipWindowHeight);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"logWindowHeight",logWindowHeight);

    settings->writeEntry(applicationPrefix+"DockedWindows/"+"disassemblyWindowWidth",disassemblyWindowWidth);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"tagWindowWidth",tagWindowWidth);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"relationshipWindowWidth",relationshipWindowWidth);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"logWindowWidth",logWindowWidth);

    settings->writeEntry(applicationPrefix+"DockedWindows/"+"disassemblyWindowIdx",disassemblyWindowIdx);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"tagWindowIdx",tagWindowIdx);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"relationshipWindowIdx",relationshipWindowIdx);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"logWindowIdx",logWindowIdx);

    settings->writeEntry(applicationPrefix+"DockedWindows/"+"disassemblyWindowOffset",disassemblyWindowOffset);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"tagWindowOffset",tagWindowOffset);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"relationshipWindowOffset",relationshipWindowOffset);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"logWindowOffset",logWindowOffset);

    settings->writeEntry(applicationPrefix+"DockedWindows/"+"disassemblyWindowNl",disassemblyWindowNl);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"tagWindowNl",tagWindowNl);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"relationshipWindowNl",relationshipWindowNl);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"logWindowNl",logWindowNl);

    settings->writeEntry(applicationPrefix+"DockedWindows/"+"showDisassemblyWindow",showDisassemblyWindow);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"showTagWindow",showTagWindow);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"showRelationshipWindow",showRelationshipWindow);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"showLogWindow",showLogWindow);
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"showPushoutWindow",showPushoutWindow);

    settings->writeEntry(applicationPrefix+"DockedWindows/"+"disassemblyWindowDockPolicy",dock2String(disassemblyWindowDockPolicy));
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"tagWindowDockPolicy",dock2String(tagWindowDockPolicy));
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"relationshipWindowDockPolicy",dock2String(relationshipWindowDockPolicy));
    settings->writeEntry(applicationPrefix+"DockedWindows/"+"logWindowDockPolicy",dock2String(logWindowDockPolicy));
}

void
PreferenceMgr::saveDockedToolbarsPreferences()
{
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"toolsToolbarIdx",toolsToolbarIdx);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"fileToolbarIdx",fileToolbarIdx);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"viewToolbarIdx",viewToolbarIdx);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"annotationsToolbarIdx",annotationsToolbarIdx);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"colorToolbarIdx",colorToolbarIdx);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"mdiToolbarIdx",mdiToolbarIdx);

    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"toolsToolbarOffset",toolsToolbarOffset);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"fileToolbarOffset",fileToolbarOffset);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"viewToolbarOffset",viewToolbarOffset);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"annotationsToolbarOffset",annotationsToolbarOffset);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"colorToolbarOffset",colorToolbarOffset);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"mdiToolbarOffset",mdiToolbarOffset);

    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"toolsToolbarNl",toolsToolbarNl);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"fileToolbarNl",fileToolbarNl);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"viewToolbarNl",viewToolbarNl);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"annotationsToolbarNl",annotationsToolbarNl);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"colorToolbarNl",colorToolbarNl);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"mdiToolbarNl",mdiToolbarNl);

    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"showToolsToolbar",showToolsToolbar);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"showFileToolbar",showFileToolbar);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"showViewToolbar",showViewToolbar);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"showAnnotationsToolbar",showAnnotationsToolbar);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"showColorToolbar",showColorToolbar);
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"showMDIToolbar",showMDIToolbar);

    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"toolsToolbarDockPolicy",dock2String(toolsToolbarDockPolicy));
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"fileToolbarDockPolicy",dock2String(fileToolbarDockPolicy));
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"viewToolbarDockPolicy",dock2String(viewToolbarDockPolicy));
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"annotationsToolbarDockPolicy",dock2String(annotationsToolbarDockPolicy));
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"colorToolbarDockPolicy",dock2String(colorToolbarDockPolicy));
    settings->writeEntry(applicationPrefix+"DockedToolbars/"+"mdiToolbarDockPolicy",dock2String(mdiToolbarDockPolicy));
}

void
PreferenceMgr::saveMiscPreferences()
{
    settings->writeEntry(applicationPrefix+"relationshipTag",relationshipTag);
    settings->writeEntry(applicationPrefix+"favoriteTags",QStringList::fromStrList(favoriteTags),',');
    settings->writeEntry(applicationPrefix+"architectureDefinitionFile",architectureDefinitionFile);
    settings->writeEntry(applicationPrefix+"logFileName",logFileName);
    settings->writeEntry(applicationPrefix+"smartFiltering",smartFiltering);
    settings->writeEntry(applicationPrefix+"performValidation",performValidation);
    settings->writeEntry(applicationPrefix+"validationCommand",validationCommand);
    settings->writeEntry(applicationPrefix+"autoPurge",autoPurge);
    settings->writeEntry(applicationPrefix+"incrementalPurge",incrementalPurge);
    settings->writeEntry(applicationPrefix+"itemMaxAge",itemMaxAge);
    settings->writeEntry(applicationPrefix+"tagWinFavouriteOnly",tagWinFavouriteOnly);
    settings->writeEntry(applicationPrefix+"lastUsedDir",lastUsedDir);
    settings->writeEntry(applicationPrefix+"keepSynchTabs",keepSynchTabs);
    settings->writeEntry(applicationPrefix+"relaxedEENode",relaxedEENode);
    settings->writeEntry(applicationPrefix+"tagBackPropagate",tagBackPropagate);
    settings->writeEntry(applicationPrefix+"compressMutableTags",compressMutableTags);
}


QString
PreferenceMgr::dock2String(Dock dockPolicy)
{
    QString result;
    switch (dockPolicy)
    {
        case DockRight:
             result = "DockRight";
             break;

        case DockLeft:
             result = "DockLeft";
             break;

        case DockTop:
             result = "DockTop";
             break;

        case DockBottom:
             result = "DockBottom";
             break;

        case DockTornOff:
             result = "DockBottom";        // hack
             break;

        default:
             result = "DockBottom";        // hack
             //Q_ASSERT(false);
    }
    return (result);
}





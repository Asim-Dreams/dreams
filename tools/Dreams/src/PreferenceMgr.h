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

/**
  * @file  PreferenceMgr.h
  */

#ifndef _PREFERENCEMGR_H
#define _PREFERENCEMGR_H

#include "DreamsDefs.h"

// QT Library
#include <qobject.h>
#include <qstring.h>
#include <qstrlist.h>
#include <qcolor.h>

#include "dDB/DralDBSyntax.h"

// Class forwarding.
class QSettings;

/**
  * A small helper object for Dreams preference management.
  * This class relies on QSettings for low level saving/restoring.
  * The application should use PreferenceMgr at start-up for loading
  * previously saved preferences and at exit time to saving some
  * detected modification.
  *
  * @author Federico Ardanaz
  * @date started at 2002-09-09
  * @version 0.1
  *
  * @remarks This class uses Singleton Pattern which means that only one instance will be available.
  */
class PreferenceMgr : public QObject
{
    Q_OBJECT
    public:
        static PreferenceMgr* getInstance();
        static PreferenceMgr* getInstance(QString companyName);

    // ----------------------------------------------------
    // -- save & restore methods
    // ----------------------------------------------------
    public:
        void savePreferences();
        bool loadPreferences();

    // ----------------------------------------------------
    // -- consulting methods
    // ----------------------------------------------------
    public:
        // grid line parameters
        inline bool getShowHGridLines();
        inline bool getShowVGridLines();
        inline bool getSnapToGrid();
        inline UINT32 getSnapGap();

        // basic appearence settings
        inline QColor  getBackgroundColor();
        inline QColor  getShadingColor();
        inline QString getGuiStyle();
        inline int getFontSize();
        inline int getMainWindowX();
        inline int getMainWindowY();
        inline int getMainWindowWidth();
        inline int getMainWindowHeight();
        inline int getHighlightSize();

        // Docked getGetWindows
        inline bool getShowDisassemblyWindow();
        inline bool getShowTagWindow();
        inline bool getShowRelationshipWindow();
        inline bool getShowPushoutWindow();
        inline bool getShowLogWindow();

        inline Dock getDisassemblyWindowDockPolicy();
        inline Dock getTagWindowDockPolicy();
        inline Dock getRelationshipWindowDockPolicy();
        inline Dock getPushoutWindowDockPolicy();
        inline Dock getLogWindowDockPolicy();

        inline int getDisassemblyWindowIdx();
        inline int getTagWindowIdx();
        inline int getRelationshipWindowIdx();
        inline int getPushoutWindowIdx();
        inline int getLogWindowIdx();

        inline int getDisassemblyWindowOffset();
        inline int getTagWindowOffset();
        inline int getRelationshipWindowOffset();
        inline int getPushoutWindowOffset();
        inline int getLogWindowOffset();

        inline bool getDisassemblyWindowNl();
        inline bool getTagWindowNl();
        inline bool getRelationshipWindowNl();
        inline bool getPushoutWindowNl();
        inline bool getLogWindowNl();

        inline int  getDisassemblyWindowWidth();
        inline int  getTagWindowWidth();
        inline int  getRelationshipWindowWidth();
        inline int  getPushoutWindowWidth();
        inline int  getLogWindowWidth();

        inline int  getDisassemblyWindowHeight();
        inline int  getTagWindowHeight();
        inline int  getRelationshipWindowHeight();
        inline int  getPushoutWindowHeight();
        inline int  getLogWindowHeight();

        // Docked toolbars
        inline bool getShowToolsToolbar();
        inline bool getShowFileToolbar();
        inline bool getShowViewToolbar();
        inline bool getShowAnnotationsToolbar();
        inline bool getShowColorToolbar();
        inline bool getShowMDIToolbar();

        inline Dock getToolsToolbarDockPolicy();
        inline Dock getFileToolbarDockPolicy();
        inline Dock getViewToolbarDockPolicy();
        inline Dock getAnnotationsToolbarDockPolicy();
        inline Dock getColorToolbarDockPolicy();
        inline Dock getMDIToolbarDockPolicy();

        inline int  getToolsToolbarIdx();
        inline int  getFileToolbarIdx();
        inline int  getViewToolbarIdx();
        inline int  getAnnotationsToolbarIdx();
        inline int  getColorToolbarIdx();
        inline int  getMDIToolbarIdx();

        inline int  getToolsToolbarOffset();
        inline int  getFileToolbarOffset();
        inline int  getViewToolbarOffset();
        inline int  getAnnotationsToolbarOffset();
        inline int  getColorToolbarOffset();
        inline int  getMDIToolbarOffset();

        inline bool  getToolsToolbarNl();
        inline bool  getFileToolbarNl();
        inline bool  getViewToolbarNl();
        inline bool  getAnnotationsToolbarNl();
        inline bool  getColorToolbarNl();
        inline bool  getMDIToolbarNl();

        // Default tag used for Relationship scanning
        inline QString getRelationshipTag();

        // Favorite Tags
        inline QStrList getFavoriteTags();

        // Default Architecture definition file
        inline QString getArchitectureDefinitionFile();
        inline bool    getPerformValidation();
        inline QString getValidationCommand();

        // Log filename/path
        inline QString getLogFileName();

        // smart filtering on/off
        inline bool getSmartFiltering();

        // auto purge on/off
        inline bool getAutoPurge();
        inline bool getIncrementalPurge();
        inline int  getItemMaxAge();
        inline int  getMaxIFI();
        inline bool getMaxIFIEnabled();
        inline bool getIgnoreEdgeBandwidthConstrain();

        inline bool getTagWinFavouriteOnly();

        inline QString getLastUsedDir();
        inline bool getKeepSynchTabs();
        inline bool getRelaxedEENode();
        inline bool getTagBackPropagate();
        inline bool getCompressMutableTags();

        inline bool getGUIEnabled();

    // ----------------------------------------------------
    // -- modifing methods
    // ----------------------------------------------------
    public:
        // grid line parameters
        inline void setShowVGridLines(bool value);
        inline void setShowHGridLines(bool value);
        inline void setSnapToGrid(bool value);
        inline void setSnapGap(UINT32 value);

        // basic appearence settings
        inline void setBackgroundColor(QColor  value);
        inline void setShadingColor(QColor  value);
        inline void setGuiStyle(QString value);
        inline void setFontSize(int value);
        inline void setMainWindowX(int value);
        inline void setMainWindowY(int value);
        inline void setMainWindowWidth(int value);
        inline void setMainWindowHeight(int value);
        inline void setHighlightSize(int value);

        // Docked windows
        inline void setDisassemblyWindowWidth(int  value);
        inline void setTagWindowWidth(int  value);
        inline void setRelationshipWindowWidth(int  value);
        inline void setPushoutWindowWidth(int  value);
        inline void setLogWindowWidth(int  value);

        inline void setDisassemblyWindowHeight(int  value);
        inline void setTagWindowHeight(int  value);
        inline void setRelationshipWindowHeight(int  value);
        inline void setPushoutWindowHeight(int  value);
        inline void setLogWindowHeight(int  value);

        inline void setShowDisassemblyWindow(bool value);
        inline void setShowTagWindow(bool value);
        inline void setShowRelationshipWindow(bool value);
        inline void setShowPushoutWindow(bool value);
        inline void setShowLogWindow(bool value);

        inline void setDisassemblyWindowDockPolicy(Dock value);
        inline void setTagWindowDockPolicy(Dock value);
        inline void setRelationshipWindowDockPolicy(Dock value);
        inline void setPushoutWindowDockPolicy(Dock value);
        inline void setLogWindowDockPolicy(Dock value);

        inline void setDisassemblyWindowIdx(int value);
        inline void setTagWindowIdx(int value);
        inline void setRelationshipWindowIdx(int value);
        inline void setPushoutWindowIdx(int value);
        inline void setLogWindowIdx(int value);

        inline void setDisassemblyWindowOffset(int value);
        inline void setTagWindowOffset(int value);
        inline void setRelationshipWindowOffset(int value);
        inline void setPushoutWindowOffset(int value);
        inline void setLogWindowOffset(int value);

        inline void setDisassemblyWindowNl(bool value);
        inline void setTagWindowNl(bool value);
        inline void setRelationshipWindowNl(bool value);
        inline void setPushoutWindowNl(bool value);
        inline void setLogWindowNl(bool value);

        // Docked toolbars
        inline void setShowToolsToolbar(bool value);
        inline void setShowFileToolbar(bool value);
        inline void setShowViewToolbar(bool value);
        inline void setShowAnnotationsToolbar(bool value);
        inline void setShowColorToolbar(bool value);
        inline void setShowMDIToolbar(bool value);

        inline void setToolsToolbarDockPolicy(Dock value);
        inline void setFileToolbarDockPolicy(Dock value);
        inline void setViewToolbarDockPolicy(Dock value);
        inline void setAnnotationsToolbarDockPolicy(Dock value);
        inline void setColorToolbarDockPolicy(Dock value);
        inline void setMDIToolbarDockPolicy(Dock value);

        inline void setToolsToolbarIdx(int value);
        inline void setFileToolbarIdx(int value);
        inline void setViewToolbarIdx(int value);
        inline void setAnnotationsToolbarIdx(int value);
        inline void setColorToolbarIdx(int value);
        inline void setMDIToolbarIdx(int value);

        inline void setToolsToolbarOffset(int value);
        inline void setFileToolbarOffset(int value);
        inline void setViewToolbarOffset(int value);
        inline void setAnnotationsToolbarOffset(int value);
        inline void setColorToolbarOffset(int value);
        inline void setMDIToolbarOffset(int value);

        inline void setToolsToolbarNl(bool value);
        inline void setFileToolbarNl(bool value);
        inline void setViewToolbarNl(bool value);
        inline void setAnnotationsToolbarNl(bool value);
        inline void setColorToolbarNl(bool value);
        inline void setMDIToolbarNl(bool value);

        // Default tag used for Relationship scanning
        inline void setRelationshipTag(QString value);

        // Favorite Tags
        inline void setFavoriteTags(QStrList value);

        // Default Architecture definition file
        inline void setArchitectureDefinitionFile(QString value);
        inline void setPerformValidation(bool value);
        inline void setValidationCommand(QString value);

        // Log filename/path
        inline void setLogFileName(QString);

        // smart filtering on/off
        inline void setSmartFiltering(bool value);

        // Auto Purge on/off
        inline void setAutoPurge(bool value);
        inline void setIncrementalPurge(bool value);
        inline void setItemMaxAge(int value);
        inline void setMaxIFI(int value);
        inline void setMaxIFIEnabled(bool value);
        inline void setIgnoreEdgeBandwidthConstrain(bool value);

        inline void setTagWinFavouriteOnly(bool value);

        inline void setLastUsedDir(QString value);
        inline void setKeepSynchTabs(bool value);

        inline void setRelaxedEENode(bool value);
        inline void setTagBackPropagate(bool value);
        inline void setCompressMutableTags(bool value);

        // GUI enabled.
        inline void setGUIEnabled(bool value);

    protected:
        PreferenceMgr(QString);
        ~PreferenceMgr();

        bool loadDBasePreferences();
        bool loadGridLinePreferences();
        bool loadAppearencePreferences();
        bool loadDockedWindowsPreferences();
        bool loadDockedToolbarsPreferences();
        bool loadMiscPreferences();
        bool parseDock(QString value, Dock &dockPolicy);

        void saveDBasePreferences();
        void saveGridLinePreferences();
        void saveAppearencePreferences();
        void saveDockedWindowsPreferences();
        void saveDockedToolbarsPreferences();
        void saveMiscPreferences();
        QString dock2String(Dock dockPolicy);

    private:
        QString companyNamePrefix;
        QString applicationPrefix;
        QSettings* settings;

    // ----------------------------------------------------
    // -- setting parameters:
    // ----------------------------------------------------
    private:
        // grid line parameters
        bool showHGridLines;
        bool showVGridLines;
        bool snapToGrid;
        UINT32 snapGap;

        // basic appearence settings
        QColor  backgroundColor;
        QColor  shadingColor;
        QString guiStyle;
        int fontSize;
        int mainWindowX;
        int mainWindowY;
        int mainWindowWidth;
        int mainWindowHeight;
        int highlightSize;

        // Docked windows
        // Pushout window does not seem to be fully implented, so as of now
        // we just make sure that the showPushoutWindow is properly initialized
        int  disassemblyWindowWidth;
        int  tagWindowWidth;
        int  relationshipWindowWidth;
        int  pushoutWindowWidth;
        int  logWindowWidth;

        int  disassemblyWindowHeight;
        int  tagWindowHeight;
        int  relationshipWindowHeight;
        int  pushoutWindowHeight;
        int  logWindowHeight;

        bool showDisassemblyWindow;
        bool showTagWindow;
        bool showRelationshipWindow;
        bool showPushoutWindow;
        bool showLogWindow;

        Dock disassemblyWindowDockPolicy;
        Dock tagWindowDockPolicy;
        Dock relationshipWindowDockPolicy;
        Dock pushoutWindowDockPolicy;
        Dock logWindowDockPolicy;

        int  disassemblyWindowIdx;
        int  tagWindowIdx;
        int  relationshipWindowIdx;
        int  pushoutWindowIdx;
        int  logWindowIdx;

        int  disassemblyWindowOffset;
        int  tagWindowOffset;
        int  relationshipWindowOffset;
        int  pushoutWindowOffset;
        int  logWindowOffset;

        bool disassemblyWindowNl;
        bool tagWindowNl;
        bool relationshipWindowNl;
        bool pushoutWindowNl;
        bool logWindowNl;

        // Docked toolbars
        bool showToolsToolbar;
        bool showFileToolbar;
        bool showViewToolbar;
        bool showAnnotationsToolbar;
        bool showColorToolbar;
        bool showMDIToolbar;

        Dock toolsToolbarDockPolicy;
        Dock fileToolbarDockPolicy;
        Dock viewToolbarDockPolicy;
        Dock annotationsToolbarDockPolicy;
        Dock colorToolbarDockPolicy;
        Dock mdiToolbarDockPolicy;

        int  toolsToolbarIdx;
        int  fileToolbarIdx;
        int  viewToolbarIdx;
        int  annotationsToolbarIdx;
        int  colorToolbarIdx;
        int  mdiToolbarIdx;

        int  toolsToolbarOffset;
        int  fileToolbarOffset;
        int  viewToolbarOffset;
        int  annotationsToolbarOffset;
        int  colorToolbarOffset;
        int  mdiToolbarOffset;

        bool  toolsToolbarNl;
        bool  fileToolbarNl;
        bool  viewToolbarNl;
        bool  annotationsToolbarNl;
        bool  colorToolbarNl;
        bool  mdiToolbarNl;

        // Default tag used for Relationship scanning
        QString relationshipTag;

        // Favorite Tags
        QStrList favoriteTags;

        // Default Architecture definition file
        QString architectureDefinitionFile;
        bool    performValidation;
        QString validationCommand;

        // Log filename
        QString logFileName;

        bool smartFiltering;
        bool autoPurge;
        bool incrementalPurge;
        int itemMaxAge;

        bool tagWinFavouriteOnly;
        bool keepSynchTabs;
        QString lastUsedDir;

        bool relaxedEENode;

        int  maxIFI;
        bool maxIFIEnabled;

        bool ignoreEdgeBandwidthConstrain;

        bool tagBackPropagate;
        bool compressMutableTags;
        bool GUIEnabled;
    private:
        static PreferenceMgr* _myInstance;
};


// ------------------------------------------------------------------
// -- Consulting methods
// ------------------------------------------------------------------
int
PreferenceMgr::getMainWindowX()
{ return mainWindowX; }

int
PreferenceMgr::getMainWindowY()
{ return mainWindowY; }

int
PreferenceMgr::getMainWindowWidth()
{ return mainWindowWidth; }

int
PreferenceMgr::getMainWindowHeight()
{ return mainWindowHeight; }

int
PreferenceMgr::getHighlightSize()
{ return highlightSize; }

bool
PreferenceMgr::getShowVGridLines()
{ return showVGridLines; }

bool
PreferenceMgr::getShowHGridLines()
{ return showHGridLines; }

bool
PreferenceMgr::getSnapToGrid()
{ return snapToGrid; }

UINT32
PreferenceMgr::getSnapGap()
{ return snapGap; }

QColor
PreferenceMgr::getBackgroundColor()
{ return backgroundColor; }

QColor
PreferenceMgr::getShadingColor()
{ return shadingColor; }

QString
PreferenceMgr::getGuiStyle()
{ return guiStyle; }

bool
PreferenceMgr::getShowDisassemblyWindow()
{ return showDisassemblyWindow; }

bool
PreferenceMgr::getShowTagWindow()
{ return showTagWindow; }

bool
PreferenceMgr::getShowRelationshipWindow()
{ return showRelationshipWindow; }

bool
PreferenceMgr::getShowPushoutWindow()
{ return showPushoutWindow; }

bool
PreferenceMgr::getShowLogWindow()
{ return showLogWindow; }

enum Qt::Dock
PreferenceMgr::getDisassemblyWindowDockPolicy()
{ return disassemblyWindowDockPolicy; }


enum Qt::Dock
PreferenceMgr::getTagWindowDockPolicy()
{ return tagWindowDockPolicy; }

enum Qt::Dock
PreferenceMgr::getRelationshipWindowDockPolicy()
{ return relationshipWindowDockPolicy; }

enum Qt::Dock
PreferenceMgr::getPushoutWindowDockPolicy()
{ return pushoutWindowDockPolicy; }

enum Qt::Dock
PreferenceMgr::getLogWindowDockPolicy()
{ return logWindowDockPolicy; }

bool
PreferenceMgr::getShowToolsToolbar()
{ return showToolsToolbar; }

bool
PreferenceMgr::getShowFileToolbar()
{ return showFileToolbar; }

bool
PreferenceMgr::getShowViewToolbar()
{ return showViewToolbar; }

bool
PreferenceMgr::getShowAnnotationsToolbar()
{ return showAnnotationsToolbar; }

bool
PreferenceMgr::getShowColorToolbar()
{ return showColorToolbar; }

bool
PreferenceMgr::getShowMDIToolbar()
{ return showMDIToolbar; }

enum Qt::Dock
PreferenceMgr::getToolsToolbarDockPolicy()
{ return toolsToolbarDockPolicy; }

enum Qt::Dock
PreferenceMgr::getFileToolbarDockPolicy()
{ return fileToolbarDockPolicy; }

enum Qt::Dock
PreferenceMgr::getViewToolbarDockPolicy()
{ return viewToolbarDockPolicy; }

enum Qt::Dock
PreferenceMgr::getAnnotationsToolbarDockPolicy()
{ return annotationsToolbarDockPolicy; }

enum Qt::Dock
PreferenceMgr::getColorToolbarDockPolicy()
{ return colorToolbarDockPolicy; }

enum Qt::Dock
PreferenceMgr::getMDIToolbarDockPolicy()
{ return mdiToolbarDockPolicy; }

QString
PreferenceMgr::getRelationshipTag()
{ return relationshipTag; }

QStrList
PreferenceMgr::getFavoriteTags()
{ return favoriteTags; }

QString
PreferenceMgr::getArchitectureDefinitionFile()
{ return architectureDefinitionFile; }

QString
PreferenceMgr::getLogFileName()
{ return logFileName; }

int
PreferenceMgr::getDisassemblyWindowIdx()
{ return disassemblyWindowIdx; }

int
PreferenceMgr::getTagWindowIdx()
{ return tagWindowIdx; }

int
PreferenceMgr::getRelationshipWindowIdx()
{ return relationshipWindowIdx; }

int
PreferenceMgr::getPushoutWindowIdx()
{ return pushoutWindowIdx; }

int
PreferenceMgr::getLogWindowIdx()
{ return logWindowIdx; }


int
PreferenceMgr::getDisassemblyWindowOffset()
{ return disassemblyWindowOffset; }

int
PreferenceMgr::getTagWindowOffset()
{ return tagWindowOffset; }

int
PreferenceMgr::getRelationshipWindowOffset()
{ return relationshipWindowOffset; }

int
PreferenceMgr::getPushoutWindowOffset()
{ return pushoutWindowOffset; }

int
PreferenceMgr::getLogWindowOffset()
{ return logWindowOffset; }

bool
PreferenceMgr::getDisassemblyWindowNl()
{ return disassemblyWindowNl; }

bool
PreferenceMgr::getTagWindowNl()
{ return tagWindowNl; }

bool
PreferenceMgr::getRelationshipWindowNl()
{ return relationshipWindowNl; }

bool
PreferenceMgr::getPushoutWindowNl()
{ return pushoutWindowNl; }

bool
PreferenceMgr::getLogWindowNl()
{ return logWindowNl; }

int  
PreferenceMgr::getToolsToolbarIdx()
{ return toolsToolbarIdx; }

int
PreferenceMgr::getFileToolbarIdx()
{ return fileToolbarIdx; }

int
PreferenceMgr::getViewToolbarIdx()
{ return viewToolbarIdx; }

int
PreferenceMgr::getAnnotationsToolbarIdx()
{ return annotationsToolbarIdx; }

int
PreferenceMgr::getColorToolbarIdx()
{ return colorToolbarIdx; }

int
PreferenceMgr::getMDIToolbarIdx()
{ return mdiToolbarIdx; }

int
PreferenceMgr::getToolsToolbarOffset()
{ return toolsToolbarOffset; }

int  
PreferenceMgr::getFileToolbarOffset()
{ return fileToolbarOffset; }

int  
PreferenceMgr::getViewToolbarOffset()
{ return viewToolbarOffset; }

int  
PreferenceMgr::getAnnotationsToolbarOffset()
{ return annotationsToolbarOffset; }

int
PreferenceMgr::getColorToolbarOffset()
{ return colorToolbarOffset; }

int
PreferenceMgr::getMDIToolbarOffset()
{ return mdiToolbarOffset; }

bool
PreferenceMgr::getToolsToolbarNl()
{ return toolsToolbarNl; }

bool
PreferenceMgr::getFileToolbarNl()
{ return fileToolbarNl; }

bool
PreferenceMgr::getViewToolbarNl()
{ return viewToolbarNl; }

bool
PreferenceMgr::getAnnotationsToolbarNl()
{ return annotationsToolbarNl; }

bool
PreferenceMgr::getColorToolbarNl()
{ return colorToolbarNl; }

bool
PreferenceMgr::getMDIToolbarNl()
{ return mdiToolbarNl; }

int
PreferenceMgr::getDisassemblyWindowWidth()
{ return disassemblyWindowWidth; }

int
PreferenceMgr::getTagWindowWidth()
{ return tagWindowWidth; }

int
PreferenceMgr::getRelationshipWindowWidth()
{ return relationshipWindowWidth; }

int
PreferenceMgr::getPushoutWindowWidth()
{ return pushoutWindowWidth; }

int
PreferenceMgr::getLogWindowWidth()
{ return logWindowWidth; }

int
PreferenceMgr::getDisassemblyWindowHeight()
{ return disassemblyWindowHeight; }

int
PreferenceMgr::getTagWindowHeight()
{ return tagWindowHeight; }

int
PreferenceMgr::getRelationshipWindowHeight()
{ return relationshipWindowHeight; }

int
PreferenceMgr::getPushoutWindowHeight()
{ return pushoutWindowHeight; }

int
PreferenceMgr::getLogWindowHeight()
{ return logWindowHeight; }

bool
PreferenceMgr::getSmartFiltering()
{ return smartFiltering; }

int
PreferenceMgr::getFontSize()
{ return fontSize; }

bool
PreferenceMgr::getPerformValidation()
{ return performValidation; }

QString
PreferenceMgr::getValidationCommand()
{ return validationCommand; }

bool
PreferenceMgr::getAutoPurge()
{ return autoPurge; }

bool
PreferenceMgr::getIncrementalPurge()
{ return incrementalPurge; }

bool
PreferenceMgr::getTagWinFavouriteOnly()
{ return tagWinFavouriteOnly; }

QString
PreferenceMgr::getLastUsedDir()
{ return lastUsedDir; }

bool
PreferenceMgr::getKeepSynchTabs()
{ return keepSynchTabs; }

int
PreferenceMgr::getItemMaxAge()
{ return itemMaxAge; }

bool
PreferenceMgr::getRelaxedEENode()
{ return relaxedEENode; }

int
PreferenceMgr::getMaxIFI()
{ return maxIFI; }

bool
PreferenceMgr::getMaxIFIEnabled()
{ return maxIFIEnabled; }

bool
PreferenceMgr::getIgnoreEdgeBandwidthConstrain()
{ return ignoreEdgeBandwidthConstrain; }

bool
PreferenceMgr::getTagBackPropagate()
{ return tagBackPropagate; }

bool
PreferenceMgr::getCompressMutableTags()
{ return compressMutableTags; }

bool
PreferenceMgr::getGUIEnabled()
{ return GUIEnabled; }

// ------------------------------------------------------------------
// -- Modification methods
// ------------------------------------------------------------------
void
PreferenceMgr::setShowVGridLines(bool value)
{ showVGridLines = value; }

void
PreferenceMgr::setShowHGridLines(bool value)
{ showHGridLines = value; }

void
PreferenceMgr::setSnapToGrid(bool value)
{ snapToGrid = value; }

void
PreferenceMgr::setSnapGap(UINT32 value)
{ snapGap = value; }

void
PreferenceMgr::setBackgroundColor(QColor  value)
{ backgroundColor = value; }

void
PreferenceMgr::setShadingColor(QColor  value)
{ shadingColor = value; }


void
PreferenceMgr::setGuiStyle(QString value)
{ guiStyle = value; }

void
PreferenceMgr::setShowDisassemblyWindow(bool value)
{ showDisassemblyWindow = value; }

void
PreferenceMgr::setShowTagWindow(bool value)
{ showTagWindow = value; }

void
PreferenceMgr::setShowRelationshipWindow(bool value)
{ showRelationshipWindow = value; }

void
PreferenceMgr::setShowPushoutWindow(bool value)
{ showPushoutWindow = value; }

void
PreferenceMgr::setShowLogWindow(bool value)
{ showLogWindow = value; }

void
PreferenceMgr::setDisassemblyWindowDockPolicy(Dock value)
{ disassemblyWindowDockPolicy = value; }

void
PreferenceMgr::setTagWindowDockPolicy(Dock value)
{ tagWindowDockPolicy = value; }

void
PreferenceMgr::setRelationshipWindowDockPolicy(Dock value)
{ relationshipWindowDockPolicy = value; }

void
PreferenceMgr::setPushoutWindowDockPolicy(Dock value)
{ pushoutWindowDockPolicy = value; }

void
PreferenceMgr::setLogWindowDockPolicy(Dock value)
{ logWindowDockPolicy = value; }

void
PreferenceMgr::setShowToolsToolbar(bool value)
{ showToolsToolbar = value; }

void
PreferenceMgr::setShowFileToolbar(bool value)
{ showFileToolbar = value; }

void
PreferenceMgr::setShowViewToolbar(bool value)
{ showViewToolbar = value; }

void
PreferenceMgr::setShowAnnotationsToolbar(bool value)
{ showAnnotationsToolbar = value; }

void
PreferenceMgr::setShowColorToolbar(bool value)
{ showColorToolbar = value; }

void
PreferenceMgr::setShowMDIToolbar(bool value)
{ showMDIToolbar = value; }

void
PreferenceMgr::setToolsToolbarDockPolicy(Dock value)
{ toolsToolbarDockPolicy = value; }

void
PreferenceMgr::setFileToolbarDockPolicy(Dock value)
{ fileToolbarDockPolicy = value; }

void
PreferenceMgr::setViewToolbarDockPolicy(Dock value)
{ viewToolbarDockPolicy = value; }

void
PreferenceMgr::setAnnotationsToolbarDockPolicy(Dock value)
{ annotationsToolbarDockPolicy = value; }

void
PreferenceMgr::setColorToolbarDockPolicy(Dock value)
{ colorToolbarDockPolicy = value; }

void
PreferenceMgr::setMDIToolbarDockPolicy(Dock value)
{ mdiToolbarDockPolicy = value; }

void
PreferenceMgr::setRelationshipTag(QString value)
{ relationshipTag = value; }

void
PreferenceMgr::setFavoriteTags(QStrList value)
{ favoriteTags = value; }

void
PreferenceMgr::setArchitectureDefinitionFile(QString value)
{ architectureDefinitionFile = value; }

void
PreferenceMgr::setLogFileName(QString value)
{ logFileName = value; }

void
PreferenceMgr::setDisassemblyWindowIdx (int  value)
{ disassemblyWindowIdx = value; }

void
PreferenceMgr::setTagWindowIdx   (int  value)
{ tagWindowIdx   = value; }

void
PreferenceMgr::setRelationshipWindowIdx   (int  value)
{ relationshipWindowIdx   = value; }

void
PreferenceMgr::setPushoutWindowIdx   (int  value)
{ pushoutWindowIdx   = value; }

void
PreferenceMgr::setLogWindowIdx   (int  value)
{ logWindowIdx   = value; }

void
PreferenceMgr::setDisassemblyWindowOffset  (int  value)
{ disassemblyWindowOffset   = value; }

void
PreferenceMgr::setTagWindowOffset   (int  value)
{ tagWindowOffset   = value; }

void
PreferenceMgr::setRelationshipWindowOffset  (int  value)
{ relationshipWindowOffset   = value; }

void
PreferenceMgr::setPushoutWindowOffset  (int  value)
{ pushoutWindowOffset   = value; }

void
PreferenceMgr::setLogWindowOffset   (int  value)
{ logWindowOffset   = value; }

void
PreferenceMgr::setDisassemblyWindowNl (bool value)
{ disassemblyWindowNl  = value; }

void
PreferenceMgr::setTagWindowNl  (bool value)
{ tagWindowNl  = value; }

void
PreferenceMgr::setRelationshipWindowNl  (bool value)
{ relationshipWindowNl  = value; }

void
PreferenceMgr::setPushoutWindowNl  (bool value)
{ pushoutWindowNl  = value; }

void
PreferenceMgr::setLogWindowNl  (bool value)
{ logWindowNl  = value; }

void 
PreferenceMgr::setToolsToolbarIdx(int value)
{ toolsToolbarIdx = value; }

void
PreferenceMgr::setFileToolbarIdx(int value)
{ fileToolbarIdx = value; }

void 
PreferenceMgr::setViewToolbarIdx(int value)
{ viewToolbarIdx=value; }

void
PreferenceMgr::setAnnotationsToolbarIdx(int value)
{ annotationsToolbarIdx=value; }

void
PreferenceMgr::setColorToolbarIdx(int value)
{ colorToolbarIdx=value; }

void
PreferenceMgr::setMDIToolbarIdx(int value)
{ mdiToolbarIdx=value; }

void
PreferenceMgr::setToolsToolbarOffset(int value)
{ toolsToolbarOffset=value; }

void
PreferenceMgr::setFileToolbarOffset(int value)
{ fileToolbarOffset=value; }

void
PreferenceMgr::setViewToolbarOffset(int value)
{ viewToolbarOffset=value; }

void
PreferenceMgr::setAnnotationsToolbarOffset(int value)
{ annotationsToolbarOffset=value; }

void
PreferenceMgr::setColorToolbarOffset(int value)
{ colorToolbarOffset=value; }

void
PreferenceMgr::setMDIToolbarOffset(int value)
{ mdiToolbarOffset=value; }

void
PreferenceMgr::setToolsToolbarNl(bool value)
{ toolsToolbarNl=value; }

void
PreferenceMgr::setFileToolbarNl(bool value)
{ fileToolbarNl=value; }

void
PreferenceMgr::setViewToolbarNl(bool value)
{ viewToolbarNl=value; }

void
PreferenceMgr::setAnnotationsToolbarNl(bool value)
{ annotationsToolbarNl=value; }

void
PreferenceMgr::setColorToolbarNl(bool value)
{ colorToolbarNl=value; }

void
PreferenceMgr::setMDIToolbarNl(bool value)
{ mdiToolbarNl=value; }

void
PreferenceMgr::setDisassemblyWindowWidth(int  value)
{ disassemblyWindowWidth=value; }

void 
PreferenceMgr::setTagWindowWidth(int  value)
{ tagWindowWidth=value; }

void 
PreferenceMgr::setRelationshipWindowWidth(int  value)
{ relationshipWindowWidth=value; }

void 
PreferenceMgr::setPushoutWindowWidth(int  value)
{ pushoutWindowWidth=value; }

void 
PreferenceMgr::setLogWindowWidth(int  value)
{ logWindowWidth=value; }

void 
PreferenceMgr::setDisassemblyWindowHeight(int  value)
{ disassemblyWindowHeight=value; }

void 
PreferenceMgr::setTagWindowHeight(int  value)
{ tagWindowHeight=value; }

void 
PreferenceMgr::setRelationshipWindowHeight(int  value)
{ relationshipWindowHeight=value; }

void 
PreferenceMgr::setPushoutWindowHeight(int  value)
{ pushoutWindowHeight=value; }

void 
PreferenceMgr::setLogWindowHeight(int  value)
{ logWindowHeight=value; }

void 
PreferenceMgr::setMainWindowX(int value)
{ mainWindowX = value; }

void 
PreferenceMgr::setMainWindowY(int value)
{ mainWindowY = value; }

void 
PreferenceMgr::setMainWindowWidth(int value)
{ mainWindowWidth = value; }

void 
PreferenceMgr::setMainWindowHeight(int value)
{ mainWindowHeight = value; }

void 
PreferenceMgr::setHighlightSize(int value)
{ highlightSize = value; }

void
PreferenceMgr::setSmartFiltering(bool value)
{ smartFiltering=value; }

void
PreferenceMgr::setFontSize(int value)
{ fontSize=value; }

void
PreferenceMgr::setPerformValidation(bool value)
{ performValidation=value; }

void
PreferenceMgr::setValidationCommand(QString value)
{ validationCommand=value; }

void
PreferenceMgr::setAutoPurge(bool value)
{ autoPurge=value; }

void
PreferenceMgr::setIncrementalPurge(bool value)
{ incrementalPurge=value; }


void
PreferenceMgr::setTagWinFavouriteOnly(bool value)
{  tagWinFavouriteOnly = value; }

void
PreferenceMgr::setLastUsedDir(QString value)
{  lastUsedDir = value; }

void
PreferenceMgr::setKeepSynchTabs(bool value)
{  keepSynchTabs = value; }


void
PreferenceMgr::setItemMaxAge(int value)
{ itemMaxAge = value; }

void
PreferenceMgr::setRelaxedEENode(bool value)
{  relaxedEENode = value; }

void
PreferenceMgr::setMaxIFI(int value)
{ maxIFI = value; }

void
PreferenceMgr::setMaxIFIEnabled(bool value)
{ maxIFIEnabled = value; }

void
PreferenceMgr::setIgnoreEdgeBandwidthConstrain(bool value)
{ ignoreEdgeBandwidthConstrain = value; }

void
PreferenceMgr::setTagBackPropagate(bool value)
{ tagBackPropagate = value; }

void
PreferenceMgr::setCompressMutableTags(bool value)
{ compressMutableTags = value; }

void
PreferenceMgr::setGUIEnabled(bool value)
{ GUIEnabled = value; }

#endif

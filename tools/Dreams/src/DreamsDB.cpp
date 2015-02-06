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
  * @file DreamsDB.cpp
  */

// General includes.
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "DreamsDefs.h"
#include "DreamsDB.h"
#include "PreferenceMgr.h"
#include "adf/ADFParserInterface.h"
#include "adf/ADFParserBuilder.h"
#include "IOMgr.h"
#include "adf/ADFMgr.h"

// Asim includes.
#include "asim/BookmarkMgr.h"
#include "asim/dralCommonDefines.h"

// Qt includes.
#include <qprogressdialog.h>
#include <qtextstream.h>

/*
 * Creates a new Dreams database associated to an existing trace
 * and a bookmark manager.
 *
 * @return new object.
 */
DreamsDB::DreamsDB(QString trace, BookmarkMgr * bookMarkMgr, LogMgr * logmgr)
{
    // Copies the parameters.
    trace_file = trace;
    bkMgr = bookMarkMgr;
    myLogMgr = logmgr;

    PreferenceMgr * prefMgr = PreferenceMgr::getInstance();

    numMDI = 0;

    // Creates the draldb for this trace.
    draldb = new DralDB();
    draldb->trackItemTags(true);
    setDralDBPreferences(prefMgr);

    // Creates the IOMgr;
    iomgr = new IOMgr(draldb, logmgr, NULL);
    connect(iomgr, SIGNAL(loadFinish()), this, SLOT(loadFinishSlot()));
    connect(iomgr, SIGNAL(loadCancel()), this, SLOT(loadCancelSlot()));

    // Creates an ADF manager.
    adfmgr = new ADFMgr(logmgr);
    adfparser = NULL;

    // No layouts active.
    resource = NULL;
    floorplan = NULL;
    watch = NULL;
    waterfall = NULL;

    // No layout data computed.
    dataresource = false;
    datafloorplan = false;
    datawatch = false;
    datawaterfall = false;

    // Init Item window layout configuration.
    tabspeclist = new TabSpecList();
    Q_ASSERT(tabspeclist!=NULL);
    tabspeclist->setAutoDelete(true);

    itablist = new ItemTabList();
    Q_ASSERT(itablist!=NULL);
    itablist->setAutoDelete(true);

    // The maps defined in the ADF are autodeleted.
    mapset.setAutoDelete(true);

    // This resets might be dangerous if multiple file support is enabled...
    //AColorPalette::reset();
    //bkMgr->reset();
}

/*
 * Destroys everything related to a dral trace.
 *
 * @return destroys the object.
 */
DreamsDB::~DreamsDB()
{
    // Deletes the database, adf and io managers.
    delete draldb;
    delete iomgr;
    delete adfmgr;
    delete adfparser;

    // Deletes the layouts.
    delete resource;
    delete floorplan;
    delete watch;
    delete waterfall;

    // Deletes the tab lists.
    delete tabspeclist;
    delete itablist;
}

/*
 * Sets the preferences to the dral database.
 *
 * @return void.
 */
void
DreamsDB::setDralDBPreferences(PreferenceMgr * prefMgr)
{
    draldb->setGUIEnabled(true);
    draldb->setAutoPurge(prefMgr->getAutoPurge());
    draldb->setIncrementalPurge(prefMgr->getIncrementalPurge());
    draldb->setMaxIFIEnabled(prefMgr->getMaxIFIEnabled());
    draldb->setMaxIFI(prefMgr->getMaxIFI());
    draldb->setTagBackPropagate(prefMgr->getTagBackPropagate());
    draldb->setCompressMutable(prefMgr->getCompressMutableTags());
    draldb->setItemMaxAge(prefMgr->getItemMaxAge());
    if(prefMgr->getIgnoreEdgeBandwidthConstrain())
    {
        draldb->setIgnoreEdgeBandwidthConstrain();
    }
}

/*
 * Returns the adf.
 *
 * @returns the adf.
 */
QDomDocument *
DreamsDB::getDomDocument() const
{
    return adfmgr->getDomDocument();
}

/*
 * Loads the header of the trace associated to this dreams database.
 *
 * @return if the header is loaded correctly.
 */
bool
DreamsDB::loadDRLHeader()
{
    //cerr << "DreamsDB::loadDRLHeader: called on " << trace_file << endl;

    // Opens the file.
    if(!draldb->openDRLFile(trace_file))
    {
        QMessageBox::critical(NULL, "IO Error", "Unable to read " + trace_file + " file.", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
        return false;
    }

    //cerr << "DreamsDB::loadDRLHeader: reading drl header..." << endl;

    // Loads the trace header.
    if(!draldb->loadDRLHeader())
    {
        myLogMgr->addLog("Error reading header of " + trace_file);
        return false;
    }

    // All right then!
    return true;
}

/*
 * Tries to parse the ADF requested by the user: default, embedded
 * or command line.
 *
 * @return if an ADF could be parsed.
 */
bool
DreamsDB::parseADF(QString defaultADF, QString paramADF)
{
    bool dadf = adfmgr->setDefaultADF(defaultADF);
    bool padf = adfmgr->setCommandLineADF(paramADF);
    bool eadf = false;

    // Gets all the comments of the header.
    const DralComment * comment;

    comment = draldb->getFirstDral2Comment();

    // Searches along all the comments.
    while(comment != NULL)
    {
        switch(comment->getMagicNumber())
        {
            case DREAMS2_EMBEDDED_ADF_MAGIC_NUMBER:
            {
                QString adfstr(comment->getComment());
                adfmgr->setEmbeddedADF(adfstr);
                eadf = true;
                break;
            }

            case TAR_FILE_MAGICNUM:
            {
                iomgr->getTarContent(comment);
                break;
            }

            case DREAMS2_SETENV_MAGIC_NUMBER:
            {
                putenv(comment->getComment());
                break;
            }

            default:
                break;
        }

        comment = draldb->getNextDral2Comment();
    }

    // Checks that at least one adf is set.
    if(!dadf && !eadf && !padf)
    {
        QString err = "Error: no ADF defined!!";
        myLogMgr->addLog(err);
        return false;
    }

    // Decides the adf.
    if(!getADF())
    {
        //cerr << "Dreams: Error parsing ADF" << endl;
        return false;
    }

    // Finally parses it.
    if(!adfparser->parseADF())
    {
        myLogMgr->addLog("ADF Parsing failed!");
        QMessageBox::critical(NULL, "Load error", "ADF Parsing error.\nSee log file for details.", QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);

        return false;
    }

    return true;
}

/*
 * Gets the ADF and also gets a suitable parser.
 *
 * @return if it could get the adf and parser.
 */
bool
DreamsDB::getADF()
{
    // Run all the low level stuff.
    if(!adfmgr->getADF())
    {
        return false;
    }

    // Get an appropiated parser.
    ADFParserBuilder builder;
    adfparser = builder.getSuitableParser(this, myLogMgr, adfmgr->getDomDocument());

    // Error checking.
    if(adfparser == NULL)
    {
        QString err = builder.getErrorMsg();
        myLogMgr->addLog(err);
        return false;
    }

    return true;
}

/*
 * This slot bypasses the signal of load finish to Dreams.
 *
 * @return void.
 */
void
DreamsDB::loadFinishSlot()
{
    emit loadFinishSignal();
}

/*
 * This slot bypasses the signal of load cancel to Dreams.
 *
 * @return void.
 */
void
DreamsDB::loadCancelSlot()
{
    emit loadCancelSignal();
}

bool
DreamsDB::saveSession(QFile * file)
{
    unsigned mgc = DREAMS2_SESSION_MAGIC_NUMBER;
    {
        QTextStream ostream (file);
        ostream << mgc << "\n";
    }

    // bookmarks section
    {
        QTextStream ostream (file);
        ostream << QString("<Bookmarks>\n");
    }
    bkMgr->exportBookmarks(file);


    // shadowed columns section
    {
        QTextStream ostream (file);
        ostream << QString("<ShadowedColumns>\n");
    }
    cshadows->saveShadows(file);

    // annotations section
    {
        QTextStream ostream (file);
        ostream << QString("<Annotations>\n");
    }
    return true;
}

/*
 * Starts the loading of the trace itself.
 *
 * @return void.
 */
void
DreamsDB::startLoading()
{
    // Forwards the call to the IOMgr and DralDB.
    draldb->startLoading();
    iomgr->startLoading();
}

/*
 * Clears the dictionary of the database.
 *
 * @return void.
 */
void
DreamsDB::clearDict()
{
    draldb->clearDict();
}

bool
DreamsDB::loadSession(QFile* file)
{
    unsigned mgc;
    QString line;

    // check magic number
    {
        QTextStream istream (file);
        istream >> mgc;
    }

    if (mgc!=DREAMS2_SESSION_MAGIC_NUMBER)
    {
        QMessageBox::critical (NULL,"Load Session Error",
        "Unrecognized format.",
        QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
        return false;
    }

    // check bookmark flag
    {
        QTextStream istream (file);
        istream >> line ;
    }
    if (line!="<Bookmarks>")
    {
        QMessageBox::critical (NULL,"Load Session Error",
        "Bookmark section not found!?",
        QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
        return false;
    }
    bkMgr->importBookmarks(file);

    // check shaded flag
    {
        QTextStream istream (file);
        istream >> line ;
    }
    if (line!="<ShadowedColumns>")
    {
        QMessageBox::critical (NULL,"Load Session Error",
        "ShadowedColumns section not found!?",
        QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
        return false;
    }
    cshadows->loadShadows(file);

    // check annotations flag
    {
        QTextStream istream (file);
        istream >> line ;
    }
    if (line!="<Annotations>")
    {
        QMessageBox::critical (NULL,"Load Session Error",
        "Annotations section not found!?",
        QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
        return false;
    }
    return true;
}

/*
 * This function compute the Dreams data needed to display this view.
 *
 * @return void.
 */
void
DreamsDB::layoutResourceComputeData()
{
    // If it's already computed skips it.
    if(getHasFatalError(true) || dataresource)
    {
        return;
    }

    //struct rusage ini, end;
    //struct timeval val;

    //getrusage(RUSAGE_SELF, &ini);
    resource->computeCSL();
    //getrusage(RUSAGE_SELF, &end);
    //printf("LayoutResource time: %.3f\n", (float) (end.ru_utime.tv_sec + end.ru_stime.tv_sec - ini.ru_utime.tv_sec - ini.ru_stime.tv_sec) + ((float) (end.ru_utime.tv_usec + end.ru_stime.tv_usec - ini.ru_utime.tv_usec - ini.ru_stime.tv_usec) / 1000000.0f));

    // Purges all the allocated vectors.
    draldb->purgeTrackHeapAllocatedVectors();

    dataresource = true;
}

/*
 * This function compute the Floorplan data needed to display this view.
 *
 * @return void.
 */
void
DreamsDB::layoutFloorplanComputeData()
{
    // If it's already computed skips it.
    if(getHasFatalError(true) || datafloorplan)
    {
        return;
    }

    //struct rusage ini, end;
    //struct timeval val;

    //getrusage(RUSAGE_SELF, &ini);
    floorplan->computeGeometry();
    floorplan->computeCSL();
    //getrusage(RUSAGE_SELF, &end);
    //printf("LayoutFloorplan time: %.3f\n", (float) (end.ru_utime.tv_sec + end.ru_stime.tv_sec - ini.ru_utime.tv_sec - ini.ru_stime.tv_sec) + ((float) (end.ru_utime.tv_usec + end.ru_stime.tv_usec - ini.ru_utime.tv_usec - ini.ru_stime.tv_usec) / 1000000.0f));

    //getrusage(RUSAGE_SELF, &ini);
    floorplan->computeCSLFilters();
    //getrusage(RUSAGE_SELF, &end);
    //printf("LayoutFloorplanFilter time: %.3f\n", (float) (end.ru_utime.tv_sec + end.ru_stime.tv_sec - ini.ru_utime.tv_sec - ini.ru_stime.tv_sec) + ((float) (end.ru_utime.tv_usec + end.ru_stime.tv_usec - ini.ru_utime.tv_usec - ini.ru_stime.tv_usec) / 1000000.0f));

    // Purges all the allocated vectors.
    draldb->purgeTrackHeapAllocatedVectors();

    datafloorplan = true;
}

/*
 * This function compute the Watch Window data needed to display this view.
 *
 * @return void.
 */
void
DreamsDB::layoutWatchComputeData()
{
    // If it's already computed skips it.
    if(getHasFatalError(true) || datawatch)
    {
        return;
    }

    //struct rusage ini, end;
    //struct timeval val;

    //getrusage(RUSAGE_SELF, &ini);
    watch->computeCSL();
    //getrusage(RUSAGE_SELF, &end);
    //printf("WatchWindow time: %.3f\n", (float) (end.ru_utime.tv_sec + end.ru_stime.tv_sec - ini.ru_utime.tv_sec - ini.ru_stime.tv_sec) + ((float) (end.ru_utime.tv_usec + end.ru_stime.tv_usec - ini.ru_utime.tv_usec - ini.ru_stime.tv_usec) / 1000000.0f));

    // Purges all the allocated vectors.
    draldb->purgeTrackHeapAllocatedVectors();

    datawatch = true;
}

/*
 * This function compute the WaterFall data needed to display this view.
 *
 * @return void.
 */
void
DreamsDB::layoutWaterfallComputeData()
{
    // If it's already computed skips it.
    if(getHasFatalError(true) || datawaterfall)
    {
        return;
    }

    waterfall->updateDBInfo();

    // Purges all the allocated vectors.
    draldb->purgeTrackHeapAllocatedVectors();

    datawaterfall = true;
}

/*
 * Adds rules to all the tabs.
 *
 * @return void.
 */
void
DreamsDB::addItemWinGlobalRule(ExpressionList & lrules)
{
    // Appends the list of rules to the globally defined.
    tabrules.appendList(lrules);

    QPtrListIterator<TabSpec> it_tab(* tabspeclist);
    TabSpec * tab;

    // Adds the rules to all the nodes.
    while((tab = it_tab.current()) != 0)
    {
        ++it_tab;
        tab->addFontRules(lrules);
    }
}

/*
 * Adds a new tab.
 *
 * @return void.
 */
void
DreamsDB::addNewTab(TabSpec * tab)
{
    // Adds the rules to the tab and appends it in the list.
    tab->addFontRules(tabrules);
    tabspeclist->append(tab);
}

/*
 * Dumps the content of the actual trace to the stdout.
 *
 * @return void.
 */
void
DreamsDB::dumpRegression(bool gui_enabled)
{
    // Dumps the contents.
    draldb->dumpRegression(gui_enabled);
    if(layoutResourceHasLayout() && layoutResourceGetOpened())
    {
        resource->dumpRegression(gui_enabled);
    }
    if(layoutFloorplanHasLayout())
    {
        //floorplan->dumpRegression(gui_enabled, 
    }
    if(layoutWaterfallHasLayout())
    {
    }
    if(layoutWatchHasLayout())
    {
    }
}

// -------------------------------------------------------------------
// AMemObj methods
// -------------------------------------------------------------------

/**
 * TODO Add support for layout DBase
 */
INT64
DreamsDB::getObjSize() const
{
    INT64 result = tabspeclist->count() * (sizeof(TabSpec) + 3 * sizeof(void*));
    result += draldb->getObjSize();
    result += resource->getObjSize();
    result += floorplan->getObjSize();
    result += waterfall->getObjSize();
    result += watch->getObjSize();
    result += sizeof(this);

    return result;
}

QString
DreamsDB::getUsageDescription() const
{
    INT64 rules = 0;
    rules += tabspeclist->count() * (sizeof(TabSpec) + 3 * sizeof(void *));

    QString result = "\n --- Trace " + getTraceName() + " memory report --- \n\n";

    result += "Trace size: " + QString::number((long) getObjSize()) + " bytes\n";
    result += "Trace contains:\n";
    result += " 1) LayoutFloorplan:\t" + QString::number((long) floorplan->getObjSize()) + " bytes\n";
    result += " 2) LayoutResource:\t" + QString::number((long) resource->getObjSize()) + " bytes\n";
    result += " 3) LayoutWaterfall:\t" + QString::number((long) waterfall->getObjSize()) + " bytes\n";
    result += " 4) LayoutWatch:\t" + QString::number((long) watch->getObjSize()) + " bytes\n";
    result += " 5) DralDB:\t" + QString::number((long) draldb->getObjSize()) + " bytes\n";
    result += " 6) ADF Rules:\t " + QString::number((long) rules) + " bytes\n";

    result += "\nDetailed memory usage for each component:\n";
    result += "\n\n";
    result += resource->getUsageDescription();
    result += "\n\n";
    result += floorplan->getUsageDescription();
    result += "\n\n";
    result += waterfall->getUsageDescription();
    result += "\n\n";
    result += watch->getUsageDescription();
    result += "\n\n";
    result += draldb->getUsageDescription();
    result += "\n\n";

    return result;
}

QString
DreamsDB::getStats() const
{
    QString result = "\n --- DreamsDB Statistics Report --- \n\n";

    result += "Object DreamsDB is a wrapper that contains:\n";

    return result;
}

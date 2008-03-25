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
  * @file  DralDB.cpp
  */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDB.h"

// Qt includes.
#include <qfile.h>

#define DRALDB_PROCESS_STEP 4096

/**
 * Creator of this class. Gets all the instances and sets the other
 * fields to the default values.
 *
 * @return new object.
 */
DralDB::DralDB()
{
    logMgr        = LogMgr::getInstance();
    strtable      = new StrTable();
    dict          = new Dict2064();
    dbConfig      = new DBConfig();
    dbGraph       = new DralGraph();
    clocks        = new ClockDomainMgr(dbGraph);
    tagDescVector = new TagDescVector(strtable);
    item_heap   = new ItemHeap(tagDescVector, dict, dbConfig, strtable, clocks);
    track_heap     = new TrackHeap(item_heap, dbGraph, tagDescVector, dict, strtable);
    dblistener    = new DBListener(tagDescVector, strtable, item_heap, dbConfig, dbGraph, track_heap, clocks, dict);
    eventFile     = NULL;
    dralClient    = NULL;
    numTrackedEdges = 0;
    firstTime = true;
}

/**
 * Destructor of this class. Just close the DRL file.
 *
 * @return destroys the object.
 */
DralDB::~DralDB()
{
    closeDRLFile();
    delete strtable;
    delete dict;
    delete dbConfig;
    delete dbGraph;
    delete tagDescVector;
    delete item_heap;
    delete track_heap;
    delete dblistener;
    delete clocks;
}

// -------------------------------------------------------------------
// -- Initialization Methods
// -------------------------------------------------------------------
/**
 * Opens filename to read a dral trace.
 *
 * @return if the file could be opened.
 */
bool
DralDB::openDRLFile(QString filename)
{
    // Opens the file.
    eventFile = new QFile(filename);
    Q_ASSERT(eventFile != NULL);

    // Checks for errors.
    if(!eventFile->open(IO_ReadOnly))
    {
        QString err = "IO Error, unable to read " + filename;
        logMgr->addLog(err);
        return false;
    }
    logMgr->addLog("Starting to read event file: " + filename + "...");

    // Creates the dral client.
    INT32 fd = eventFile->handle();
    dralClient = new DRAL_CLIENT_CLASS(fd, dblistener, 1024 * 64);

    return (dralClient != NULL);
}

/**
 * Closes the actual dral trace file.
 *
 * @return if the file could be closed.
 */
bool
DralDB::closeDRLFile()
{
    if(eventFile == NULL)
    {
        return false;
    }

    delete eventFile;
    eventFile = NULL;

    // Flush any pending itemids.
    dblistener->flush();
    return true;
}

// -------------------------------------------------------------------
// -- DRAL Processing Methods
// -------------------------------------------------------------------
/*
 * Process the header of the dral trace file.
 *
 * @return true if the header was loaded correctly.
 */
bool
DralDB::loadDRLHeader()
{
    INT32 result = 1;

    // Processes events while the header is still being loaded, no error has happened and
    // events are left in the trace.
    while(
            dblistener->isProcessingDralHeader() &&
            dblistener->getLastProcessedEventOk() &&
            (result == 1)
         )
    {
        result = dralClient->ProcessNextEvent(true, 1);
    }

    return (result == 1);
}

// -------------------------------------------------------------------
// -- Misc
// -------------------------------------------------------------------

/*
 * Clears the dictionary of the database.
 *
 * @return void.
 */
void
DralDB::clearDict()
{
    dict->clearDict();
}

/**
 * Dumps the graph description. Forwards the call to dbgraph.
 *
 * @return void.
 */
void
DralDB::dumpGraphDescription()
{
    // just for debugging
    QString str = dbGraph->getGraphDescription();
    printf("%s\n",str.latin1());
}

/**
 * Gets the graph description. Forwards the call to dbgraph.
 *
 * @return the graph description.
 */
QString
DralDB::getGraphDescription()
{
    return dbGraph->getGraphDescription();
}

/*
 * Returns the size of the object and its components.
 *
 * @return the size of the object.
 */
INT64
DralDB::getObjSize() const
{
    INT64 size;

    size = sizeof(this) + dblistener->getObjSize() + track_heap->getObjSize() + item_heap->getObjSize() + tagDescVector->getObjSize() + strtable->getObjSize();
    return size;
}

/*
 * Returns a description of the usage of this class.
 *
 * @return the description.
 */
QString
DralDB::getUsageDescription() const
{
    QString ret;

    ret = "Usage description of the Dral Database:\n";
    ret += "1) DBListener:\n";
    ret += dblistener->getUsageDescription();
    ret += "\n\n";
    ret += "2) TrackHeap:\n";
    ret += track_heap->getUsageDescription();
    ret += "\n\n";
    ret += "3) ItemHeap:\n";
    ret += item_heap->getUsageDescription();
    ret += "\n\n";
    ret += "4) TagDescVector:\n";
    ret += tagDescVector->getUsageDescription();
    ret += "\n\n";
    ret += "5) StrTable:\n";
    ret += strtable->getUsageDescription();
    ret += "\n\n";
    return ret;
}

/*
 * Returns the statistics of this class. Unimplemented by now.
 *
 * @return the statistics.
 */
QString
DralDB::getStats() const
{
    return "";
}

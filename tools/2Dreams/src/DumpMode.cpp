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
  * @file DumpMode.cpp
  */

#include "DumpMode.h"

/**
 * Cretor of the class.
 *
 * @return new object.
 */
DumpMode::DumpMode()
{
}

/**
 * Destructor of the class.
 *
 * @return destroys the object.
 */
DumpMode::~DumpMode()
{
}

/**
 * Performs the code main code in dumping mode.
 *
 * @return void.
 */
void
DumpMode::start(DreamsLineParams params)
{
    bool ok; ///< Used to collect return values.

    prefMgr = PreferenceMgr::getInstance("BSSAD");
    ok = prefMgr->loadPreferences();
    if(!ok)
    {
        printf("Error: can't load the preferences.\n");
        exit(-1);
    }
    prefMgr->setGUIEnabled(false);

    draldb = DralDB::getInstance();
    draldb->setGUIEnabled(false);

    dreamsdb = DreamsDB::getInstance();

    draldb->setAutoPurge(prefMgr->getAutoPurge()) ;
    draldb->setIncrementalPurge(prefMgr->getIncrementalPurge()) ;
    draldb->setMaxIFIEnabled(prefMgr->getMaxIFIEnabled()) ;
    draldb->setMaxIFI(prefMgr->getMaxIFI()) ;
    draldb->setTagBackPropagate(prefMgr->getTagBackPropagate()) ;
    draldb->setCompressMutable(prefMgr->getCompressMutableTags()) ;
    draldb->setItemMaxAge(prefMgr->getItemMaxAge()) ;

    dreamsdb->reset();

    adfmgr = ADFMgr::getInstance();
    adfmgr->reset();

    zdb = ZDBase::getInstance();
    livedb = LiveDB::getInstance();

    // Sets the ADF defaults.
    bool dadf = adfmgr->setDefaultADF(prefMgr->getArchitectureDefinitionFile());
    bool eadf = true;
    eadf = adfmgr->setCommandLineADF(params.getADFFile());
    if(!dadf && !eadf)
    {
        printf("Error: couldn't set the adf.\n");
        exit(-1);
    }

    // Loads the DRL file.
    ok = draldb->openDRLFile(params.getDRLFile());
    if(!ok)
    {
        printf("Error: unable to read %s file.\n", params.getDRLFile().latin1());
        exit(-1);
    }

    // Loads it header.
    ok = draldb->loadDRLHeader();
    if(!ok)
    {
        printf("Error: drl header read uncorrectly.\n");
        exit(-1);
    }

    // Gets the ADF.
    ok = adfmgr->getADF();
    if(!ok)
    {
        printf("Error: can't get the ADF.\n");
        exit(-1);
    }

    // Creates the ADF parser.
    parser = ADFParserBuilder::getSuitableParser(adfmgr->getDomDocument());
    if(parser == NULL)
    {
        printf("Error: can't find a suitable parser.\n");
        exit(-1);
    }

    // Propagates the parser.
    dreamsdb->setADFParser(parser);

    // Starts the loading of the DRL.
    dreamsdb->startActivity(draldb->getFirstEffectiveCycle());

    INT32 ncmd; ///< Counts the number of commands processed.

    // Reads all the DRL contents.
    while(!dreamsdb->reachedEOS())
    {
        // Process a dral command.
        ncmd = draldb->processEvents(4096);
        ok = !dreamsdb->getHasFatalError();

        if((ncmd==0) || !ok || dreamsdb->reachedEOS())
        {
            if(!draldb->closeDRLFile())
            {
                printf("Error closing DRL file!!\n");
                exit(-1);
            }
            livedb->Do_EndSimulation();
            zdb->flushCachedLookUps();
            if(!ok)
            {
                printf("Critical errors found while loading the trace!!\n");
                exit(-1);
            }
            else if (dreamsdb->nonCriticalErrors())
            {
                printf("Non-critical errors found while loading the trace!!\n");
                exit(-1);
            }
        }
    }

    // Dumps the database content.
    draldb->dumpRegression();
    zdb->dumpRegression();
}

/**
 * Prints the usage message.
 *
 * @return void.
 */
void
DumpMode::print_usage()
{
    printf("Usage : 2Dreams [-adf file] [-drl file] [-dump-mode]\n");
}

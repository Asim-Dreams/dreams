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
  * @file LiveDB.cpp
  */

#include "LiveDB.h"

#include <qprogressdialog.h>
#include <qapplication.h>

#include "ADFParserInterface.h"
#include "DreamsDB.h"
#include "ADFMgr.h"

LiveDB* LiveDB::_myInstance = NULL;

//bool debug_on = false;

// -------------------------------------------------------------------
// Constructors & co
// -------------------------------------------------------------------
LiveDB*
LiveDB::getInstance()
{
    if (_myInstance==NULL)
        _myInstance = new LiveDB();

    return _myInstance;
}

void
LiveDB::destroy()
{
    if (_myInstance!=NULL) { delete _myInstance; }
}

LiveDB::LiveDB()
{
    deleteList = new DeleteItemList2D();
    Q_ASSERT(deleteList!=NULL);

    nrceList = new NRCEList();
    Q_ASSERT(nrceList!=NULL);

    //enterList = new EnterNodeList2D(2048);Q_ASSERT(enterList!=NULL);
    //exitList = new ExitNodeList2D(2048);Q_ASSERT(exitList!=NULL);

    itemList = new NewItemList2D(PrimeList::nearPrime(500000));
    Q_ASSERT(itemList!=NULL);
    itemList->setAutoDelete(true);

    // get a reference singleton objects
    myZDB      = ZDBase::getInstance();
    myConfigDB = ConfigDB::getInstance();
    csle       = CSLEngine::getInstance();
    myLogMgr   = LogMgr::getInstance();
    pref       = PreferenceMgr::getInstance();
    draldb     = DralDB::getInstance();

    lastUsedPosVector = NULL;

    // general reset procedure
    reset();
}

LiveDB::~LiveDB()
{
    if (itemList!=NULL) { delete itemList; }
    //if (enterList!=NULL) delete enterList;
    //if (exitList!=NULL) delete exitList;
    if (deleteList!=NULL) { delete deleteList; }
    if (nrceList!=NULL) { delete nrceList; }
    if (lastUsedPosVector!=NULL) { delete [] lastUsedPosVector; }
}


// -------------------------------------------------------------------
// DRAL LISTENER methods
// -------------------------------------------------------------------

void
LiveDB::Cycle (UINT64 n)
{
    if (hasFatalError) { return; }

    static int dcnt=0;
    ++dcnt;

    if (!firstCycle)
    {
        if (! incNumCycles((UINT32)(n)-currentCycle))
        {
            QString err = "2Dreams: Error trying add cycle " + QString::number(currentCycle) + ".";
            err = err + '\n';
            err = err + "This can be the result of an invalid cycle sequence or an internal bug.\n";
            myLogMgr->addLog(err);
            hasFatalError = true;
            return;
        }

        // add cnt for node-row events (and keep precomputed color/shape values)
        computeNodeRowNumEvents();

        // compute the final number of events for the cycle (alloc space)
        myZDB->computeNumEvents(currentCycle);

        // commit dead items
        computeDeleteItems();

        // and finally put the color/shape for node-row events
        commitNodeEvents();
    }
    firstCycle=false;
    resetEdgeLastUsedPos();
    currentCycle = (UINT32)n;
}

void
LiveDB::StartActivity(UINT64 _firstEffectiveCycle)
{
    if (hasFatalError)
    {
        return;
    }

	firstEffectiveCycle = (UINT32)_firstEffectiveCycle;
    myZDB->setFirstEffectiveCycle(firstEffectiveCycle);

    // at the first cycle found we need to apply ADF to DRAL edges, compute
    // edge ordering, etc.
    ADFParserInterface* adfParser = DreamsDB::getInstance()->getADFParser();
    if (adfParser==NULL)
    {
        myLogMgr->addLog("No suitable ADF parser found!");
        hasFatalError=true;
        return;
    }

    bool adfok = adfParser->parseADF();
    if (!adfok)
    {
        myLogMgr->addLog("ADF Parsing failed!");
        hasFatalError=true;
        return;
    }

    // set the number of event rows
    INT32 nrows = myConfigDB->getNumRows();
    adfok = myZDB->setNumRows(nrows);
    if (!adfok)
    {
        QString err = "Error trying to set up " + QString::number(nrows) + " rows.";
        err += '\n';
        err += "Current 2Dreams version is able to show at most "+QString::number(COLDESC_MAX_EVENTS);
        err += " event rows.";
        myLogMgr->addLog(err);
        hasFatalError=true;
        return;
    }

    // configure RowDescVector with ADF results:
    myConfigDB->applyRowConfig();

    // compute biggest known edge latency
    biggestLat = myConfigDB->getBiggestLatency();
    bool incok = incNumCycles(1+biggestLat); // currentCycle cannot be -1 'cause is UINT64...
    Q_ASSERT(incok);

    // remove all the non-configured edges and nodes from the graph
    myConfigDB->clearNonConfiguredEdges();
    myConfigDB->clearNonConfiguredNodes();
    
    // allocate "fast" indirection tbl for "nextPos" on edge-rows
    numConfEdges = QMAX(myConfigDB->getNumEdges(),1);
    lastUsedPosVector = new UINT8[numConfEdges];
    bzero((char*)lastUsedPosVectorIndirection,sizeof(lastUsedPosVectorIndirection));
    int ecnt=0;
    for (INT32 i=0;i<65536;i++)
    {
        if (myConfigDB->hasEdge(i))
        {
            lastUsedPosVectorIndirection[i] = ecnt++;
        }

    }
    resetEdgeLastUsedPos();
}

void
LiveDB::NewItem (UINT32 item_id)
{
    if (hasFatalError) { return; }
    LNewItemListNode2D* node = new LNewItemListNode2D(currentCycle,item_id);
    Q_ASSERT(node!=NULL);
    itemList->insert((long)item_id,node);
}

void
LiveDB::MoveItems (UINT16 edge_id,UINT32 n,UINT32* item_id)
{
    if (hasFatalError) { return; }
    // check if the edge_id is present in XML config
    if (!myConfigDB->hasConfiguredEdge(edge_id)) return;

    // check that the number of moved items is less or equal the bw of the edge
    INT32 bw = myConfigDB->getEdgeBandwidth(edge_id);
    Q_ASSERT(bw>0);

    // multiple moveitems can be launched to the same edge/cycle:
    UINT8 nextPos= lastUsedPosVector[lastUsedPosVectorIndirection[edge_id]];
    for (UINT32 i=0;i<n;i++)
    {
        // I need to check that a "new item" was launched for item_id
        LNewItemListNode2D* newItemNode = itemList->find((long)item_id[i]);
        if (newItemNode!=NULL)
        {
            // check if previous moveitems command have been launched for this edge in this cycle
            if (nextPos<bw)
            {
                LMoveItemListNode2D node (currentCycle,edge_id,(UINT16)nextPos);
                newItemNode->getMyMovs()->append(node);
                csle->markEventData(edge_id,currentCycle,nextPos);
                ++nextPos;
            }
        }
    }
    lastUsedPosVector[lastUsedPosVectorIndirection[edge_id]] = nextPos;
}

void
LiveDB::MoveItemsWithPositions (UINT16 edge_id, UINT32 n,UINT32* item_id, UINT32* positions)
{
    if (hasFatalError) { return; }

    // check if the edge_id is present in XML config
    if (! myConfigDB->hasConfiguredEdge(edge_id)) return;
    // check that the number of moved items is less or equal the bw of the edge
    INT32 bw = myConfigDB->getEdgeBandwidth(edge_id);
    Q_ASSERT(bw>0);

    for (UINT32 i=0;i<n;i++)
    {
        // I need to check that a "new item" was launched for item_id
        LNewItemListNode2D* newItemNode = itemList->find((long)item_id[i]);
        if (newItemNode!=NULL)
        {
            if (positions[i]<(UINT32)bw)
            {
                LMoveItemListNode2D node (currentCycle,edge_id,positions[i]);
                csle->markEventData(edge_id,currentCycle,positions[i]);
            }
        }
    }
}

void
LiveDB::DeleteItem (UINT32 item_id)
{
    if (hasFatalError) { return; }
    // I need to check that a "new item" was launched for item_id
    LNewItemListNode2D* newItemNode = itemList->find((long)item_id);
    if (newItemNode!=NULL) { deleteList->append(item_id); }
}

void
LiveDB::EndSimulation()
{ Do_EndSimulation(); }

void
LiveDB::Do_EndSimulation()
{
    if (hasFatalError)
    {
        itemList->clear();
        deleteList->reset();
        return;
    }

    // to prevent spureous timer events
    if (eoSimulation) return;
    eoSimulation=true;

    /// @todo review this
    //for (int i=0;i<biggestLat;i++) Cycle(currentCycle+1);
    Cycle(currentCycle+1);

    // call delete items over non explicitely deleted items if auto purge is on
    if (pref->getAutoPurge())
    {
        // itarate throught the live guys:
        INT32 nitems = itemList->count();
        QIntDictIterator<LNewItemListNode2D> it(*itemList);
        QProgressDialog * progress = NULL;
        if (pref->getGUIEnabled())
        {
            progress = new QProgressDialog("2Dreams is commiting EOF Auto-Purged Items...", "&Abort", nitems ,NULL, "commitprogress",TRUE);
            progress->setMinimumDuration(1000);
        }
        LNewItemListNode2D* itnode = it.current();
        ItemHandler hnd;
        if (itnode!=NULL)
        {
            draldb->lookForItemId(&hnd, itnode->item_id);
            Q_ASSERT(hnd.isValidItemHandler());
        }

        int cnt=0;
        while (itnode!=NULL)
        {
            if (pref->getGUIEnabled())
            {
        	    if (cnt%128==0)
        	    {
                    progress->setProgress( cnt );
                    qApp->processEvents();
                    if ( progress->wasCancelled() )
                    {
                	    myLogMgr->addLog("Item List Commit cancelled by user!");
                	    break;
                    }
                }
            }
            draldb->lookForItemId(&hnd, itnode->item_id);
            Q_ASSERT(hnd.isValidItemHandler());
            bool deleted = computeDeleteItem(&hnd,itnode);

            ++cnt;
            if (!deleted) ++it;
            itnode = it.current();
        }
        if (pref->getGUIEnabled())
        {
            progress->setProgress(nitems);
            delete progress;
        }
    }
    itemList->clear();
    deleteList->reset();
}

void
LiveDB::Comment (char * comment)
{
    // DRAL 1 comments are ignored
}

void
LiveDB::Comment (UINT32 magic_num, char comment [])
{
    if (hasFatalError) { return; }
    // DRAL 2 comments are processed

    // check for embedded adf:
    if (magic_num==DREAMS2_EMBEDDED_ADF_MAGIC_NUMBER)
    {
        QString adfstr(comment);
        ADFMgr::getInstance()->setEmbeddedADF(adfstr);
    }
}

void
LiveDB::CommentBin (UINT16 magic_num, char * cont, UINT32 length)
{
}

void
LiveDB::AddNode (UINT16 node_id,char * node_name,UINT16 parent_id,UINT16 instance)
{
    if (hasFatalError) { return; }
    QString qnodename(node_name);
    // errors already dumped into the log file by ConfigDB class
    hasFatalError = !myConfigDB->addNode (qnodename.stripWhiteSpace(),node_id,parent_id,instance);
}

void
LiveDB::AddEdge (UINT16 sourceNode, UINT16 destNode, UINT16 edge_id,UINT32 bandwidth,
                 UINT32 latency,char* name)
{
    if (hasFatalError) { return; }
    QString qname(name);
    // errors already dumped into the log file by ConfigDB class
    hasFatalError = !myConfigDB->addEdge (sourceNode,destNode,edge_id,bandwidth,latency,qname.stripWhiteSpace());
}

void
LiveDB::SetCapacity (UINT16 node_id,UINT32 capacity,
                     UINT32 capacities [], UINT16 dimensions)
{
    if (hasFatalError) { return; }
    // errors already dumped into the log file by ConfigDB class
    hasFatalError = !myConfigDB->setCapacity(node_id,capacity);
}

void
LiveDB::SetHighWaterMark (UINT16 node_id,UINT32 mark)
{
}

void
LiveDB::Error (char * error)
{
    hasFatalError=true;
}

void
LiveDB::NonCriticalError (char * error)
{
}

void 
LiveDB::SetNodeInputBandwidth(UINT16 node_id, UINT32 bandwidth)
{
}

void 
LiveDB::SetNodeOutputBandwidth(UINT16 node_id, UINT32 bandwidth)
{
}

void 
LiveDB::SetTagDescription (char tag_name [], char description [])
{
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// ------------------------------------------------------------------
// ------------------------------------------------------------------
// ------------------------------------------------------------------

void
LiveDB::computeDeleteItems()
{
    if (hasFatalError) { return; }

    if (deleteList->getMaxUsedIdx() > 1000)
    {
      if (eoSimulation)
      { return computeDeleteItemsLongDlg(); }
      else
      { return computeDeleteItemsLong(); }
    }
    
    for (int i=deleteList->getMinUsedIdx();i<=deleteList->getMaxUsedIdx();i++)
    { computeDeleteItem(deleteList->at(i)); }

    deleteList->reset();
}

void
LiveDB::computeDeleteItemsLongDlg()
{
    INT32 nelem = deleteList->getMaxUsedIdx();
    QString msg = "2Dreams detected a long list (";
    msg = msg + QString::number(nelem) +") of items to delete.\n";
    msg = msg + "This is going to take a while...";
    QProgressDialog * progress = NULL;
    if (pref->getGUIEnabled())
    {
        progress = new QProgressDialog(msg,"&Abort",nelem,NULL, "commitprogress",TRUE);
        progress->setMinimumDuration(1);
    }

    INT32 cnt=0;
    ItemHandler hnd;
    LNewItemListNode2D* itnode;
    INT32 maxIdx = deleteList->getMaxUsedIdx();
    for (INT32 i = deleteList->getMinUsedIdx();i<=maxIdx;i++)
    {
        itnode = itemList->find((long)deleteList->at(i));
        if (itnode!=NULL)
        {
            draldb->lookForItemId(&hnd, itnode->item_id);
            Q_ASSERT(hnd.isValidItemHandler());
            computeDeleteItem(&hnd,itnode);
            if (pref->getGUIEnabled())
            {
            	if (cnt%128==0)
        	    {
                    progress->setProgress( cnt );
                    qApp->processEvents();
                    if ( progress->wasCancelled() )
                    {
                	    myLogMgr->addLog("deleteitem commit cancelled by user!");
                	    break;
                    }
                }
            }
        }
        ++cnt;
    }
    if (pref->getGUIEnabled())
    {
        progress->setProgress(nelem);
        delete progress;
    }
    deleteList->reset();
}

void
LiveDB::computeDeleteItemsLong()
{
    INT32 nelem = deleteList->getMaxUsedIdx();
    ItemHandler hnd;
    LNewItemListNode2D* itnode;
    INT32 maxIdx = deleteList->getMaxUsedIdx();
    for (INT32 i = deleteList->getMinUsedIdx();i<=maxIdx;i++)
    {
        itnode = itemList->find((long)deleteList->at(i));
        if (itnode!=NULL)
        {
            draldb->lookForItemId(&hnd, itnode->item_id);
            Q_ASSERT(hnd.isValidItemHandler());
            computeDeleteItem(&hnd,itnode);
        }
    }
    deleteList->reset();
}

bool
LiveDB::computeDeleteItem (ItemHandler * hnd, LNewItemListNode2D* newItemNode)
{
    // At this point I need to look for tags on the given item id
    // and put on ZDBase the item itself and all the related tags
    // toghether.

    INT32 item_id = hnd->getItemId();

    // add the events
    if (newItemNode->hasMovs())
    {
        MoveItemList2D* mlst = newItemNode->getMyMovs();
        for (int i=0;i<=mlst->getMaxUsedIdx();i++)
        {
            csle->addItemEventData((*mlst)[i].edge_id,(*mlst)[i].cycle,(*mlst)[i].position,hnd,item_id);
        }
    }

    // delete the item_id from the "new" item list
    bool delete_ok = itemList->remove((long)item_id);
    Q_ASSERT(delete_ok);
    return true;
}

void
LiveDB::reset()
{
    currentCycle=0;
    firstCycle=true;
    //hasNonCriticalErros=false;
    hasFatalError=false;
    eoSimulation = false;
    currentTraceVersion=0;
    /*
    if (enterList!=NULL)
    {
        enterList->reset();
    }
    if (exitList!=NULL)
    {
        exitList->reset();
    }
    */
    if (deleteList!=NULL)
    {
        deleteList->reset();
    }
    if (itemList!=NULL)  itemList->clear();
}

void
LiveDB::computeNodeRowNumEvents()
{
    if (hasFatalError) { return; }
    bool ok = csle->computeNodeRowEvents(currentCycle,nrceList);
    hasFatalError = hasFatalError || !ok;
}

void
LiveDB::commitNodeEvents()
{
    if (hasFatalError) { return; }
    csle->commitNodeEvents(currentCycle,nrceList);
}

// -------------------------------------------------------------------
// AMemObj methods
// -------------------------------------------------------------------

INT64
LiveDB::getObjSize() const
{
    INT64 result=sizeof(LiveDB);

    result += itemList->count()*(sizeof(LNewItemListNode2D)+ 2*sizeof(void*));
    QIntDictIterator<LNewItemListNode2D> it(*itemList);

    int nitems = itemList->count();
    QProgressDialog * progress = NULL;

    if (pref->getGUIEnabled())
    {
        progress = new QProgressDialog( "Scanning LiveDB...", "&Abort", nitems ,NULL, "progress",TRUE);
        progress->setMinimumDuration(1000);
    }

    int cnt=0;
    for ( ; it.current(); ++it )
    {
        if (pref->getGUIEnabled())
        {
    	    if (cnt%1024==0)
    	    {
                progress->setProgress( cnt );
                qApp->processEvents();
                if ( progress->wasCancelled() )
                {
            	    myLogMgr->addLog("Scanning LiveDB cancelled y user!");
            	    break;
                }
            }
        }
        /*
        LNewItemListNode2D* current = it.current();
        if (current->hasMovs())
        {
            result += (current->getMyMovs())->count()*sizeof(LMoveItemListNode2D);
        }
        */
        ++cnt;
    }
    if (pref->getGUIEnabled())
    {
        progress->setProgress(nitems);
        delete progress;
    }

    //result += deleteList->size()*sizeof(UINT32);
    //result += enterList->count()*sizeof(UINT32);
    //result += exitList->count()*sizeof(UINT32);
    return result;
}

QString
LiveDB::getUsageDescription() const
{
    INT64 main=0;
    INT64 aux=0;

    main += itemList->count()*(sizeof(LNewItemListNode2D)+ 2*sizeof(void*));
    QIntDictIterator<LNewItemListNode2D> it(*itemList);

    int nitems = itemList->count();
    QProgressDialog * progress = NULL;

    if (pref->getGUIEnabled())
    {
        progress = new QProgressDialog( "Scanning LiveDB...", "&Abort", nitems ,NULL, "progress",TRUE);
        progress->setMinimumDuration(1000);
    }

    int cnt=0;
    for ( ; it.current(); ++it )
    {
        if (pref->getGUIEnabled())
        {
    	    if (cnt%1024==0)
    	    {
                progress->setProgress( cnt );
                qApp->processEvents();
                if ( progress->wasCancelled() )
                {
            	    myLogMgr->addLog("Scanning LiveDB cancelled y user!");
            	    break;
                }
            }
        }
        /*
        LNewItemListNode2D* current = it.current();
        if (current->hasMovs())
        {
            main += (current->getMyMovs())->count()*sizeof(LMoveItemListNode2D);
        }
        ++cnt;
        */
    }
    if (pref->getGUIEnabled())
    {
        progress->setProgress(nitems);
        delete progress;
    }

    //aux += deleteList->size()*sizeof(UINT32);
    //aux += enterList->count()*sizeof(UINT32);
    //aux += exitList->count()*sizeof(UINT32);

    QString result = "\nObject LiveDB contains:\n";
    result += " 1) Items, Tags and Movements (used only while loading):\t"+QString::number((long)main) +" bytes\n";
    result += " 2) Other auxiliar structures:\t"+QString::number((long)aux) +" bytes\n";

    return result;
}

QString
LiveDB::getStats() const
{
    QString result = "";
    return (result);
}

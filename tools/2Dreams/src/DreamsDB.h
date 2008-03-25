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
  * @file  DreamsDB.h
  */

#ifndef _DreamsDB_H
#define _DreamsDB_H

#include <asim/DralDB.h>

// Qt library
#include <qstrlist.h>
#include <qdom.h>

// AGT library
#include "asim/BookmarkMgr.h"
#include "asim/ColumnShadows.h"

// 2Dreams
#include "ZDBase.h"
#include "LiveDB.h"
#include "ADFParserInterface.h"
#include "ConfigDB.h"

/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class DreamsDB : public AMemObj, public StatObj
{
    public:
        static DreamsDB* getInstance (BookmarkMgr* bookMarkMgr);
        static DreamsDB* getInstance ();
        static void destroy();

    public:
        // ---- AMemObj Interface methods
        INT64  getObjSize() const;
        QString getUsageDescription() const;
        // ---- StatObj Interface methods
        QString getStats() const;
        // -----------------------------------------------
    protected:
        DreamsDB(BookmarkMgr* bookMarkMgr);
        virtual ~DreamsDB();

    public:
        bool hasLoadedData();
        void reset();

        void setADFParser(ADFParserInterface*);
        ADFParserInterface* getADFParser();

        bool saveSession(QFile* file);
        bool loadSession(QFile* file);

        inline bool getHasFatalError();
        inline bool reachedEOS();
        inline bool nonCriticalErrors();
        inline INT32 getNumCycles ();
        inline INT32 getNumRows   ();
        inline void  setBookMarkMgr(BookmarkMgr* mgr);
        inline void  setColumnShadows(ColumnShadows* sh);
        inline QStrList getKnownTags();
        inline void startActivity(INT32 fec);

    private:
        DralDB*   drb;        ///< Generic (compressed) DRAL dbase
        ZDBase*   zdb;        ///< compressed dbase.
        LiveDB*   ldb;        ///< live (non-compressed) dbase.
        ConfigDB* cdb;        ///< configuration dbase (DRAL graph, etc).
        ADFParserInterface* adfParser;

        BookmarkMgr* bkMgr;
        ColumnShadows* cshadows;
    public:
        static DreamsDB* _myInstance;
};

bool
DreamsDB::getHasFatalError()
{ return ldb->getHasFatalError(); }

bool
DreamsDB::reachedEOS()
{ return ldb->reachedEOS(); }

bool
DreamsDB::nonCriticalErrors()
{ return ldb->nonCriticalErrors(); }

INT32
DreamsDB::getNumCycles ()
{return zdb->getNumCycles ();}

INT32
DreamsDB::getNumRows   ()
{return zdb->getNumRows   ();}

void
DreamsDB::setBookMarkMgr(BookmarkMgr* mgr)
{ bkMgr = mgr; }

QStrList
DreamsDB::getKnownTags()
{ return drb->getKnownTags(); }

void
DreamsDB::setColumnShadows(ColumnShadows* sh)
{ cshadows = sh; }

void
DreamsDB::startActivity(INT32 fec)
{ ldb->StartActivity(fec); }

#endif


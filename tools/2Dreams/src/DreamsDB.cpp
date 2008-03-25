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
  * @file DreamsDB.cpp
  */

#include "DreamsDB.h"
#include "PreferenceMgr.h"
#include "2DreamsDefs.h"

DreamsDB* DreamsDB::_myInstance=NULL;

DreamsDB*
DreamsDB::getInstance()
{
    if (_myInstance==NULL)
        _myInstance = new DreamsDB(NULL);

    Q_ASSERT(_myInstance!=NULL);
    return _myInstance;
}

DreamsDB*
DreamsDB::getInstance(BookmarkMgr* bookMarkMgr)
{
    if (_myInstance==NULL)
        _myInstance = new DreamsDB(bookMarkMgr);

    Q_ASSERT(_myInstance!=NULL);
    return _myInstance;
}

void
DreamsDB::destroy()
{
    if (_myInstance!=NULL)
    {
        delete _myInstance;
    }
}

DreamsDB::DreamsDB(BookmarkMgr* bookMarkMgr)
{
    /** @warning The order of creation/init is relevant! */

    // 0) the generic dbase
    drb = DralDB::getInstance();

    // 1) init compressed dbase
    AColorPalette::reset();
    PreferenceMgr* prefMgr = PreferenceMgr::getInstance();
    zdb = ZDBase::getInstance();

    // 2) init live-items dbase
    ldb = LiveDB::getInstance();

    // 3) config database
    cdb = ConfigDB::getInstance();

    // 4) keep references to: annotation manager, bookmark manager
    bkMgr = bookMarkMgr;

    // 5) register the ldb as external listener
    drb->attachDralListener(ldb);

    // 6) I want to keep track of item tags so
    drb->trackItemTags(true);
    
    adfParser = NULL;
}

DreamsDB::~DreamsDB()
{
    // bookMarkMgr is not built in DreamsDB so leve it alive
    zdb->destroy();
    ldb->destroy();
    cdb->destroy();
    drb->destroy();
}

void
DreamsDB::reset()
{
    ldb->reset();
    zdb->reset();
    cdb->reset();
    drb->reset();
    AColorPalette::reset();
    if (bkMgr!=NULL) { bkMgr->reset(); }
}

bool
DreamsDB::hasLoadedData()
{return (zdb->getNumCycles() > 0); }

ADFParserInterface*
DreamsDB::getADFParser()
{return adfParser; }

void
DreamsDB::setADFParser(ADFParserInterface* parser)
{ adfParser = parser; }


bool
DreamsDB::saveSession(QFile* file)
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

// -------------------------------------------------------------------
// AMemObj methods
// -------------------------------------------------------------------

INT64
DreamsDB::getObjSize() const
{
    return sizeof(this)+ldb->getObjSize()+zdb->getObjSize()+cdb->getObjSize();
}

QString
DreamsDB::getUsageDescription() const
{
    QString result = "\n --- DreamsDB memory report --- \n\n";

    result += "DreamsDB size: "+QString::number((long)(getObjSize())) +" bytes\n\n";
    result += "Object DreamsDB contains:\n";
    result += " 1) ConfigDB:\t"+QString::number((long)(cdb->getObjSize())) +" bytes\n";
    result += " 2) LiveDB:\t"+QString::number((long)(ldb->getObjSize())) +" bytes\n";
    result += " 3) ZDBase:\t"+QString::number((long)(zdb->getObjSize())) +" bytes\n";

    result += "\n\nDetailed memory usage for each component:\n";
    result += cdb->getUsageDescription();
    result += ldb->getUsageDescription();
    result += zdb->getUsageDescription();
    return (result);
}

QString
DreamsDB::getStats() const
{
    QString result = "\n --- DreamsDB Statistics Report --- \n\n";

    result += "Object DreamsDB is a wrapper that contains:\n";

    result += "\n 1) ConfigDB\n";
    result += cdb->getStats();

    result += "\n 2) LiveDB\n";
    result += ldb->getStats();

    result += "\n 3) ZDBase\n";
    result += zdb->getStats();

    return (result);
}



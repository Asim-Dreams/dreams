// ==================================================
// Copyright (C) 2006 Intel Corporation
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
  * @file FindDialogImpl.h
  */

#ifndef FINDDIALOGIMPL_H
#define FINDDIALOGIMPL_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"
#include "dDB/ClockDomainMgr.h"
#include "dialogs/find/FindDialog.h"
#include "DreamsDefs.h"
#include "TagValueType.h"

// Class forwarding.
class DralDB;
class DreamsDB;
class DAvtView;
class DisDockWin;

/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  *         Santiago Galan
  * @date started at 2002-11-21
  * @date revisioned at 2004-07-14
  * @version 0.2
  */
class FindDialogImpl : public FindDialog
{
    Q_OBJECT

  public:
    FindDialogImpl(DreamsDB * _dreamsdb, DAvtView * avt, QWidget * parent = 0, const char * name = 0, bool modal = FALSE, WFlags fl = 0);
    ~FindDialogImpl();
      
    void aboutToFind();

  public:
    void find_next();
    bool hadSucessFullSearch();

  public slots:
    void knownTagsClicked(void);
    void findClicked(void);
    void cancelClicked(void);
    void tagNameTextChanged(const QString&);
    void reToggled(bool);
      
  protected:
    virtual void doFind(QString str, TAG_ID tagId, TagValueType tgtype, CYCLE startCycle, bool resetSearch = false) = 0;

  protected:
    DralDB *     draldb;   ///< Pointer to the database.
    DreamsDB *   dreamsdb; ///< Pointer to the trace.
    DAvtView *   myAvt;    ///< Pointer to the avt.
    DisDockWin * disWin;   ///< Pointer to the disasembly window.

    bool    sucessFullSearch;
    TAG_ID lastTagId;              ///< Last tag id that was searched.
    TagValueType lastTagValueType; ///< Type of the last tag id searched.
    QString lastValue;
    CYCLE   lastCycle;
    bool    lastCS;
    bool    lastEM;
    bool    lastRE;
};

#endif

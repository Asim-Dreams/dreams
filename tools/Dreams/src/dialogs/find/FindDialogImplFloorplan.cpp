/*
 * Copyright (C) 2006 Intel Corporation
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
  * @file FindDialogImplFloorplan.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "avt/DAvtView.h"
#include "avt/FloorplanDAvtView.h"
#include "dialogs/find/FindDialogImplFloorplan.h"
#include "dockwin/disassembly/DisDockWin.h"
#include "highlight/HighLightMgr.h"
#include "mdi/MDIFloorplanWindow.h"
#include "preferencesImpl.h"
#include "DreamsDB.h"

// Qt includes.
#include <qwidget.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qapplication.h>
#include <qstrlist.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qregexp.h>
#include <qspinbox.h>
#include <qprogressdialog.h>

FindDialogImplFloorplan::FindDialogImplFloorplan(DreamsDB * dreamsdb, FloorplanDAvtView * avt, QWidget * parent, const char * name, bool modal, WFlags fl)
    : FindDialogImpl(dreamsdb, avt, parent, name, modal, fl)
{
}

void
FindDialogImplFloorplan::doFind(QString str, TAG_ID tagId, TagValueType tgtype, CYCLE startCycle, bool reset)
{
    ItemHandler hnd;               ///< Handler to access to the current item.
    LayoutFloorplanFindDataEntry * entry; ///< Working entry.
    CYCLE tgWhen;                  ///< When the tag was set.
    TagValueType tgType;           ///< The type of the tag.
    TagValueBase tgBase;           ///< The base.
    UINT64 tgValue;                ///< Value of the item tag.
    UINT64 value;                  ///< Base value.
    UINT32 searches;               ///< How many searches we have done.
    UINT32 initCycle;              ///< The first cycle where we start to look.
    bool eventFound;               ///< True when a match is found.
    bool ok;                       ///< Used to get some boolean results.
    bool cs = CheckBox_CaseSensitive->isChecked();
    bool em = CheckBox_ExactMatch->isChecked();
    bool re = CheckBox_Regular->isChecked();

    if(reset)
    {
        LayoutFloorplanFindOptions env;

        entries.clear();
        dreamsdb->layoutFloorplanGetFindData(&entries, startCycle, &env);

        lastFoundCellX = -1;
        lastFoundCellY = -1;
    }

    // No success search and no event found.
    sucessFullSearch = false;
    eventFound = false;
    value = 0;
    searches = 0;
    initCycle = 0;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if(!entries.empty())
    {
        CYCLE cycle(draldb->getClockDomain(0), 0, 0);

        // Gets the cycles in domain 0 of the first entry.
        cycle.fromLCMCycles((* entries.begin())->getCycle());

        // Gets the cycle.
        initCycle = cycle.cycle;
    }

    // Progress dialog.
    QProgressDialog progress("Searching...", "Abort", draldb->getNumCycles(0).cycle - initCycle, NULL, "fprogress", TRUE);
    progress.setMinimumDuration(500);
    progress.setProgress(1);

    value = AGTTypes::str2uint64(str.latin1(), ok);

    // Converts the string to upper case if needed.
    if((tgtype != TAG_INTEGER_TYPE) && !re && !cs)
    {
        str = str.upper();
    }

    // Tries to find until no more entries are left or until something is found.
    while(!eventFound && !entries.empty())
    {
        // Gets the first entry, which is the actual candidate as is the row
        // with a non-visited item that has the lowest cycle or in tie case
        // the smallest non-visited row.
        entry = * entries.begin();

        // Gets the item inside.
        ok = draldb->lookForItemId(&hnd, entry->getItemId());
        Q_ASSERT(ok);

        // Gets the value.
        ok = hnd.getTagById(tagId, &tgValue, &tgType, &tgBase, &tgWhen, draldb->getLastCycle());

        // Checks if match.
        if(ok)
        {
            // Chooses the type of find. No problems with this select as the
            // program always follows the same flow.
            if(tgtype == TAG_INTEGER_TYPE)
            {
                // Integer match.
                if(tgValue == value)
                {
                    eventFound = true;

                    // Gets some information.
                    lastFoundCellX = entry->getCol();
                    lastFoundCellY = entry->getRow();
                    lastCycle = draldb->getFirstEffectiveCycle(entry->getClockId());
                    lastCycle.fromLCMCycles(entry->getCycle());
                }
            }
            else if(!re)
            {
                // String match.
                // Gets the string.
                QString fvalue = draldb->getString(tgValue);

                // Exact match case.
                if(em)
                {
                    // No case sensitive.
                    if(!cs)
                    {
                        fvalue = fvalue.upper();
                    }

                    // Match?
                    if(fvalue == str)
                    {
                        eventFound = true;

                        // Gets some information.
                        lastFoundCellX = entry->getCol();
                        lastFoundCellY = entry->getRow();
                        lastCycle = draldb->getFirstEffectiveCycle(entry->getClockId());
                        lastCycle.fromLCMCycles(entry->getCycle());
                    }
                }
                else
                {
                    // Check if contains the string.
                    if(fvalue.contains(str, cs) > 0)
                    {
                        eventFound = true;

                        // Gets some information.
                        lastFoundCellX = entry->getCol();
                        lastFoundCellY = entry->getRow();
                        lastCycle = draldb->getFirstEffectiveCycle(entry->getClockId());
                        lastCycle.fromLCMCycles(entry->getCycle());
                    }
                }

            }
            else
            {
                // Regular expression match.
                // Gets the string.
                QString fvalue = draldb->getString(tgValue);

                // Checks if matches the expression.
                if((fvalue.contains(QRegExp(str, cs)) > 0)) 
                {
                    eventFound = true;

                    // Gets some information.
                    lastFoundCellX = entry->getCol();
                    lastFoundCellY = entry->getRow();
                    lastCycle = draldb->getFirstEffectiveCycle(entry->getClockId());
                    lastCycle.fromLCMCycles(entry->getCycle());
                }
            }
        }

        searches++;

        // Updates the progress bar.
        // Is done before the deletion because this might be the last element.
        if(searches == 1024)
        {
            // Clears the counter.
            searches = 0;

            // Gets the cycles in domain 0.
            CYCLE cycle(draldb->getClockDomain(0), 0, 0);

            cycle.fromLCMCycles((* entries.begin())->getCycle());

            progress.setProgress(cycle.cycle - initCycle);
            qApp->processEvents();
            if(progress.wasCancelled())
            {
                break;
            }
        }

        // Removes the entry from the set.
        entries.erase(entries.begin());

        // Moves the handler to the next item.
        entry->skipToNextItem();

        // If still valid is reinserted to the set.
        if(entry->isValidDataEntry())
        {
            entries.insert(entry);
        }
    }

    QApplication::restoreOverrideCursor();

    // Finishes the progress.
    progress.setProgress(draldb->getNumCycles(0).cycle - initCycle);
    if(progress.wasCancelled())
    {
        return;
    }

    // Check if something found.
    if(!eventFound)
    {
        QMessageBox::critical(NULL, "Search Error", "No event found related with search criterion", "&Dismiss");
    }
    else
    {
        // Success!!!
        sucessFullSearch = true;

        //cerr << "FindDialogImplFloorplan::doFind: Highlighting element..." << endl;
        myAvt->getHighLightMgr()->contentsSelectEvent(&hnd, true, myAvt->getCurrentTagSelector(), HLT_DEFAULT, true, lastCycle);

        // go there!
        // Show cycle also repaints
        static_cast<MDIFloorplanWindow *>(myAvt->getMDI())->showCycle(lastCycle);
        myAvt->showCycle(lastCycle);
        myAvt->goToPoint(lastFoundCellX, lastFoundCellY);
        disWin->alignWith(lastFoundCellX, lastFoundCellY, lastCycle);
    }
}

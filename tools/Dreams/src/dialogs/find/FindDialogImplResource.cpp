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
  * @file FindDialogImplResource.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "avt/ResourceDAvtView.h"
#include "avt/DAvtView.h"
#include "dialogs/find/FindDialogImplResource.h"
#include "dockwin/disassembly/DisDockWin.h"
#include "highlight/HighLightMgr.h"
#include "preferencesImpl.h"
#include "DreamsDB.h"

// Qt includes.
#include <qwidget.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qregexp.h>
#include <qspinbox.h>
#include <qprogressdialog.h>

FindDialogImplResource::FindDialogImplResource(DreamsDB * dreamsdb, LayoutResourceCache * _cache, ResourceDAvtView * avt, QWidget * parent, const char * name, bool modal, WFlags fl)
    : FindDialogImpl(dreamsdb, avt, parent, name, modal, fl)
{
    cache = _cache;
}

void
FindDialogImplResource::doFind(QString str, TAG_ID tagId, TagValueType tgtype, CYCLE startCycle, bool reset)
{
    ItemHandler hnd;               ///< Handler to access to the current item.
    CYCLE tgWhen;                  ///< When the tag was set.
    LayoutResourceFindDataEntry * entry; ///< Working entry.
    TagValueType tgType;           ///< The type of the tag.
    TagValueBase tgBase;           ///< The base.
    UINT64 tgValue;                ///< Value of the item tag.
    UINT64 value;                  ///< Base value.
    UINT32 searches;               ///< How many searches we have done.
    UINT32 initCycle;              ///< The first cycle where we start to look.
    bool ok;                       ///< Used to get some boolean results.
    bool cs = CheckBox_CaseSensitive->isChecked();
    bool em = CheckBox_ExactMatch->isChecked();
    bool re = CheckBox_Regular->isChecked();

    row = 0;
    if(reset)
    {
        LayoutResourceFindOptions env;

        entries.clear();
        dreamsdb->layoutResourceGetFindData(&entries, startCycle, &env);
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
                    found(entry);
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
                        found(entry);
                    }
                }
                else
                {
                    // Check if contains the string.
                    if(fvalue.contains(str, cs) > 0)
                    {
                        found(entry);
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
                    found(entry);
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

        // First of all, we want to map from physical rows to virtual rows.
        if(!dreamsdb->layoutResourceGetRowIsShown(row, cache))
        {
            // We need to be check that the destination row is shown.
            // If changed we don't need to request a repaint, because
            // is done later.
            dreamsdb->layoutResourceShowRow(row, cache);
            dreamsdb->layoutResourceComputeLayoutCache(cache);
            ((ResourceDAvtView *) myAvt)->foldingChanged();
            (myAvt->getHighLightMgr())->reset();
        }

        row = dreamsdb->layoutResourceGetRowVirtMap(row, cache);

        CYCLE cyc;

        cyc.clock = draldb->getClockDomain(entry->getClockId());
        cyc.cycle = lastCycle.cycle - draldb->getFirstEffectiveCycle(entry->getClockId()).cycle;

        // Go there!
        myAvt->goToColumn((INT32) ((float) cyc.cycle * cyc.clock->getFastestRatio()));
        myAvt->goToRow(row);
        ((ResourceDAvtView *) myAvt)->shadeColumn(cyc, true);

        // Highlighting.
        (myAvt->getHighLightMgr())->contentsSelectEvent(0, row, lastCycle, true, ((ResourceDAvtView *) myAvt)->getCurrentTagSelector(), HLT_DEFAULT, true);
        myAvt->repaintContents(false);
        cache->getFoldWidget()->repaint();
        disWin->alignWith(0, row, lastCycle);
    }
}

void
FindDialogImplResource::found(LayoutResourceFindDataEntry * entry)
{
    eventFound = true;

    // Gets some information.
    row = entry->getRow();
    lastCycle.clock = draldb->getFirstEffectiveCycle(entry->getClockId()).clock;
    lastCycle.fromLCMCycles(entry->getCycle());
}

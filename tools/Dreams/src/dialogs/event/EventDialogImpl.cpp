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

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "dialogs/event/EventDialogImpl.h"

// Qt includes.
#include <qlistview.h>

class EventDialogListViewItem : public QListViewItem
{
    public:
        EventDialogListViewItem(QListView * event_list_view, QString event, QString dralid, QString position, QString cycle, QString division, QString clock, INT32 sort_cycle);

        int compare(QListViewItem * i, int col, bool ascending) const;

    private:
        INT32 sort_cycle;
        static INT32 cycle_col;
} ;

INT32 EventDialogListViewItem::cycle_col = 3;

/* 
 *  Constructs a EventDialogImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
EventDialogImpl::EventDialogImpl(DralDB * draldb, ItemHandler * hnd, CYCLE cycle, QWidget * parent, const char * name, bool modal, WFlags fl)
    : EventDialog(parent, name, modal, fl)
{
    setCaption(tr("Events for item " + QString::number(hnd->getItemId())));
    QListViewItem * item;
    UINT64 first_sort_cycle = draldb->getFirstCycle().toLCMCycles();

    // Adds all the events to the dialog.
    while(hnd->isValidEventHandler())
    {
        QString event;
        QString dralid;
        QString position;
        QString cycle;
        QString clock;
        QString division;
        DralGraphNode * node;
        NodeSlot * slot;
        NodeSlot result;

        switch(hnd->getEventType())
        {
            case IDB_MOVE_ITEM:
                event = "Move Item";
                dralid = draldb->getEdge(hnd->getEventEdgeId())->getName();
                position = QString::number(hnd->getEventPosition());
                break;

            case IDB_ENTER_NODE:
                event = "Enter Node";
                dralid = draldb->getNode(hnd->getEventNodeId())->getName();
                node = draldb->getNode(hnd->getEventNodeId());
                slot = node->getLayout();
                result = slot->linearSlot(hnd->getEventPosition());
                position = result.toString();
                break;

            case IDB_EXIT_NODE:
                event = "Exit Node";
                dralid = draldb->getNode(hnd->getEventNodeId())->getName();
                node = draldb->getNode(hnd->getEventNodeId());
                slot = node->getLayout();
                result = slot->linearSlot(hnd->getEventPosition());
                position = result.toString();
                break;

            case IDB_GEN_EVENT:
                event = "Generic Event";
                dralid = "";
                break;

            default:
                event = "";
                dralid = "";
                break;
        }

        cycle = QString::number(hnd->getEventCycle().cycle);
        clock = hnd->getEventCycle().clock->getFormattedFreq(CLOCK_GHZ);

        if(hnd->getEventCycle().division == 0)
        {
            division = "High";
        }
        else
        {
            division = "Low";
        }

        // Adds the entry in the list.
        INT32 sort_cycle = (INT32) (hnd->getEventCycle().toLCMCycles() - first_sort_cycle);
        item = new EventDialogListViewItem(EventListView, event, dralid, position, cycle, division, clock, sort_cycle);

        hnd->nextEvent();
    }
}

/*  
 *  Destroys the object and frees any allocated resources
 */
EventDialogImpl::~EventDialogImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 * public slot
 */
void EventDialogImpl::AcceptButton_clicked()
{
    done(1);
}

/*
 * Creates a new entry
 *
 * @return new object.
 */
EventDialogListViewItem::EventDialogListViewItem(QListView * event_list_view, QString event, QString dralid, QString position, QString cycle, QString division, QString clock, INT32 _sort_cycle)
    : QListViewItem(event_list_view, event, dralid, position, cycle, division, clock)
{
    sort_cycle = _sort_cycle;
}

/*
 * Redefines the compare function for cycles
 *
 * @return new object.
 */
int
EventDialogListViewItem::compare(QListViewItem * i, int col, bool ascending) const
{
    if(col == cycle_col)
    {
        EventDialogListViewItem * j = (EventDialogListViewItem *) i;
        return (sort_cycle - j->sort_cycle);
    }
    else
    {
        return key(col, ascending).compare(i->key(col, ascending));
    }
}

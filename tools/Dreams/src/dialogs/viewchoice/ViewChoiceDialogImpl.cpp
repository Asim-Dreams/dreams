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

/*
 * @file ViewChoiceDialogImpl.cpp
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dialogs/viewchoice/ViewChoiceDialog.h"
#include "dialogs/viewchoice/ViewChoiceDialogImpl.h"
#include "DreamsDB.h"

// Qt includes.
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qtimer.h>

/*
 * Constructor of the class.
 *
 * @return new object.
 */
ViewChoiceDialogImpl::ViewChoiceDialogImpl(DreamsDB * dreamsdb, QWidget * parent,  const char * name, bool modal, WFlags fl)
    : ViewChoiceDialog(parent, name, modal, fl)
{
    if(dreamsdb->layoutResourceHasLayout() && dreamsdb->layoutResourceGetOpened())
    {
        checkBox2->setChecked(true);
    }
    else
    {
        checkBox2->setEnabled(false);
    }

    if(dreamsdb->layoutFloorplanHasLayout() && dreamsdb->layoutFloorplanGetOpened())
    {
        checkBox3->setChecked(true);
    }
    else
    {
        checkBox3->setEnabled(false);
    }

    if(dreamsdb->layoutWatchHasLayout() && dreamsdb->layoutWatchGetOpened())
    {
        checkBox4->setChecked(true);
    }
    else
    {
        checkBox4->setEnabled(false);
    }

    if(dreamsdb->layoutWaterfallHasLayout() && dreamsdb->layoutWaterfallGetOpened())
    {
        checkBox5->setChecked(true);
    }
    else
    {
        checkBox5->setEnabled(false);
    }

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    wait = 10;
    timer->start(1000);
    timeout->setText("Autocomputing in " + QString::number(wait) + " seconds");
}

/*  
 *  Destroys the object and frees any allocated resources
 */
ViewChoiceDialogImpl::~ViewChoiceDialogImpl()
{
    delete timer;
}

/*
 * Returns if the user has checked the resource check box.
 *
 * @return true if the user want to load the resource view.
 */
bool
ViewChoiceDialogImpl::loadLayoutResource() const
{
    return checkBox2->isChecked();
}

/*
 * Returns if the user has checked the Floorplan check box.
 *
 * @return true if the user want to load the Floorplan view.
 */
bool
ViewChoiceDialogImpl::loadLayoutFloorplan() const
{
    return checkBox3->isChecked();
}

/*
 * Returns if the user has checked the watch window check box.
 *
 * @return true if the user want to load the watch window view.
 */
bool
ViewChoiceDialogImpl::loadLayoutWatch() const
{
    return checkBox4->isChecked();
}

/*
 * Returns if the user has checked the WaterFall check box.
 *
 * @return true if the user want to load the WaterFall view.
 */
bool
ViewChoiceDialogImpl::loadLayoutWaterfall() const
{
    return checkBox5->isChecked();
}

/*
 * Returns if the user has checked a box.
 *
 * @return if something checked.
 */
bool
ViewChoiceDialogImpl::loadSomething() const
{
    return (checkBox2->isChecked() || checkBox3->isChecked() || checkBox4->isChecked() || checkBox5->isChecked());
}

/*
 * Slot called when a timeout occurs.
 *
 * @return void.
 */
void
ViewChoiceDialogImpl::timerTimeout()
{
    wait--;
    if(wait < 0)
    {
        timer->stop();
        done(0);
    }
    else
    {
        timeout->setText("Autocomputing in " + QString::number(wait) + " seconds");
    }
}

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
 * @file GoToCycleDialogImpl.cpp
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "DreamsDB.h"
#include "GoToCycleDialogImpl.h"

// Qt includes.
#include <qspinbox.h>
#include <qradiobutton.h>

/* 
 * Constructor of the class. Forwards the call to its super and fills
 * the combo box of clocks domain.
 *
 * @return new object.
 */
GoToCycleDialogImpl::GoToCycleDialogImpl(DreamsDB * dreamsdb, QWidget * parent, const char * name, bool modal, WFlags fl)
    : GoToCycleDialog(parent, name, modal, fl)
{
    draldb = dreamsdb->getDralDB();

    // Fills the combo box with the different clock domains.
    for(UINT32 i = 0; i < draldb->getNumClocks(); i++)
    {
        comboBox1->insertItem(draldb->getClockDomain(i)->toString());
    }

    // Sets the clock domain of the first clock.
    setClockDomain(0);

    connect(comboBox1, SIGNAL(activated(int)), this, SLOT(setClockDomain(int)));
}

/* 
 * Destructor of the class.
 *
 * @return deletes the object.
 */
GoToCycleDialogImpl::~GoToCycleDialogImpl()
{
}

/* 
 * Function called when the clock domain is changed.
 *
 * @return void.
 */
void
GoToCycleDialogImpl::setClockDomain(int clockId)
{
    // Sets the new min and max.
    spinBox1->setMinValue(draldb->getFirstEffectiveCycle(clockId).cycle);
    spinBox1->setMaxValue(draldb->getLastEffectiveCycle(clockId).cycle);
}

/* 
 * Returns the chose cycle.
 *
 * @return the cycle.
 */
CYCLE
GoToCycleDialogImpl::getEnteredCycle() const
{
    CYCLE ret;

    ret.clock = draldb->getClockDomain(comboBox1->currentItem());
    ret.cycle = spinBox1->value();

    if(radioButton1->isChecked())
    {
        ret.division = 0;
    }
    else
    {
        ret.division = 1;
    }
    return ret;
}

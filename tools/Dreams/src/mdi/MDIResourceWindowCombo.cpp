/*
 * Copyright (C) 2005-2006 Intel Corporation
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

/*
 * @file MDIResourceWindowCombo.cpp
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "avt/ResourceDAvtView.h"
#include "layout/resource/LayoutResourceCache.h"
#include "mdi/MDIResourceWindowCombo.h"
#include "mdi/MDIResourceWindowFold.h"
#include "DreamsDB.h"

#define SCALE_THRESHOLD (float) 0.6f

/*
 * Creates a new two dreams combo widget. Basically creates a ResourceDAvtView
 * and a folding / unfolding widget.
 *
 * @return the new object.
 */
MDIResourceWindowCombo::MDIResourceWindowCombo(DreamsDB * _dreamsdb, QMainWindow * main, QMainWindow * mdi)
    : QSplitter(mdi)
{
    // Copies the parameters.
    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();
    myMw = main;
    myMdi = mdi;

    // Creates the two widgets that will contain and the resource cache.
    cache = new LayoutResourceCache(dreamsdb->layoutResourceGetNumPhyisicalRows(), dreamsdb->layoutResourceGetNumGroups());
    dreamsdb->layoutResourceComputeLayoutCache(cache);
    myFold = new MDIResourceWindowFold(dreamsdb, cache, this);
    myAvt = new ResourceDAvtView(dreamsdb, cache, this, mdi, main);
    cache->setFoldWidget(myFold);
    myFold->setTwoDAvtView(myAvt);

    connect(myAvt, SIGNAL(resourceChanged(double)), myFold, SLOT(resourceChanged(double)));
    connect(myAvt, SIGNAL(asvNewScallingFactors(double, double)), this, SLOT(newScallingFactors(double, double)));

    // Specifies some layout properties.
    setFrameStyle(QFrame::NoFrame);
    //setSpacing(0);
    is_fold_hide = false;
}

/*
 * Destructor of the class.
 *
 * @return the new object.
 */
MDIResourceWindowCombo::~MDIResourceWindowCombo()
{
    // myAVT is deleted by MDIResourceWindow!!!
    delete myFold;
    delete cache;
}

/*
 * Returns the two dreams ascroll view.
 *
 * @return the asv.
 */
AScrollView *
MDIResourceWindowCombo::getAScrollView() const
{
    return myAvt;
}

/*
 * Returns the two dreams fold widget.
 *
 * @return the fold widget.
 */
MDIResourceWindowFold *
MDIResourceWindowCombo::getResourceViewFold() const
{
    return myFold;
}

/*
 * Slot called when the scaling factors of its two davt changes.
 *
 * @return void.
 */
void
MDIResourceWindowCombo::newScallingFactors(double scfx, double scfy)
{
    if((scfy < SCALE_THRESHOLD) && !is_fold_hide)
    {
        myFold->hide();
        is_fold_hide = true;
    }
    else if((scfy >= SCALE_THRESHOLD) && is_fold_hide)
    {
        myFold->show();
        is_fold_hide = false;
    }
    myFold->repaint();
}

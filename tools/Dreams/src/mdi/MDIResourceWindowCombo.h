// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
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
 * @file MDIResourceWindowCombo.h
 */

#ifndef _TWODREAMSCOMBO_H
#define _TWODREAMSCOMBO_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"

// Qt includes.
#include <qsplitter.h>

// Class forwarding.
class ResourceDAvtView;
class MDIResourceWindowFold;
class DreamsDB;
class DralDB;
class AScrollView;
class LayoutResourceCache;
class QMainWindow;

/*
 * @brief
 * Widget that contains the two dreams avt view and the folding widget.
 *
 * @description
 *
 *
 * @version 0.1
 * @date started at 2005-03-16
 * @author Guillem Sole
 */
class MDIResourceWindowCombo : public QSplitter
{
    Q_OBJECT
    public:
        MDIResourceWindowCombo(DreamsDB * _dreamsdb, QMainWindow * main, QMainWindow * mdi);
        ~MDIResourceWindowCombo();

        AScrollView * getAScrollView() const;
        MDIResourceWindowFold * getResourceViewFold() const;

    public slots:
        void newScallingFactors(double scfx, double scfy);

    protected:
        DreamsDB * dreamsdb;            ///< Pointer to the trace.
        DralDB * draldb;                ///< Pointer to the database.
        LayoutResourceCache * cache;    ///< Cache of the mapping to LayoutResource.
        ResourceDAvtView * myAvt;       ///< Pointer to the dreams avtview.
        MDIResourceWindowFold * myFold; ///< Pointer to the folding widget.
        QMainWindow * myMw;             ///< Pointer to the main window.
        QMainWindow * myMdi;            ///< Pointer to the mdi.
        bool is_fold_hide;              ///< Is the widget hidden.
} ;

#endif // _TWODREAMSCOMBO_H.

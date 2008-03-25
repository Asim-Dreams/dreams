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

#ifndef SELECTORIMPL_H
#define SELECTORIMPL_H

#include "StripEntry.h"
#include "Selector.h"
#include "asim/AScrollView.h"
#include "StripChartView.h"
#include "ColorsImpl.h"
#include "Main.h"

#include <qiconview.h>
#include <qlistbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qcolordialog.h>
#include "xpm/ok.xpm"
#include "xpm/fopenIcon.xpm"

class SelectorImpl : public Selector
{ 
    Q_OBJECT

  public:
    SelectorImpl( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0, allFilesData *DB=NULL , StripChartView *asv=NULL, ColorsImpl *vpal=NULL);
    ~SelectorImpl();
    void connectRoutines();

    allFilesData *myDB;
    ColorsImpl *visiblePalette;
    StripChartView *view;
    int fileSelected;
    int stripSelected;
    int threadSelected;
    int tablePen[5];

  public slots:
    void do_IconClicked(QListBoxItem *item);
    void do_StripClicked(int index);
    void do_ThreadClicked(QListBoxItem *item);
    void do_OKClicked();
    void updateStripList();
    void updateThreadList();

};

#endif // SELECTORIMPL_H

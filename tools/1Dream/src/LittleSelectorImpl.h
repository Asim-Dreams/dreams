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

#ifndef LITTLESELECTORIMPL_H
#define LITTLESELECTORIMPL_H
#include "StripEntry.h"
#include "StripChartView.h"
#include "ColorsImpl.h"
#include "LittleSelector.h"
#include <qiconview.h>
#include <qlistbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qcolordialog.h>
#include <qwidget.h>

class LittleSelectorImpl : public LittleSelector
{ 
    Q_OBJECT

  public:
    LittleSelectorImpl( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0, allFilesData *DB=NULL, int fs=0, int ss=0, int ts=0, ColorsImpl *pal=NULL, StripChartView *vw=NULL);
    ~LittleSelectorImpl();
    void fillData();

    void connectRoutines();

    allFilesData *myDB;
    ColorsImpl *visiblePalette;
    StripChartView *view;
    int fileSelected;
    int stripSelected;
    int threadSelected;
    int tablePen[5];
  public slots:
    void do_LineStyleClicked(int index);
    void do_ColorClicked();
    void do_OKClicked();
    void do_VisibleClicked();
    void do_HiddenClicked();
    void do_RelativeMAXClicked();
    void do_RelativeValueClicked();
    void do_AbsoluteClicked();
    void do_BreakViewClicked();
    void do_SmoothViewClicked();
    void do_SmoothClicked(int value);
    void do_BreakClicked(int value);
    void do_HSkewClicked(int value);
    void do_VSkewClicked(int value);
    void do_LineWidthClicked(int value);
};

#endif // LITTLESELECTORIMPL_H

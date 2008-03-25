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

#ifndef COLORSIMPL_H
#define COLORSIMPL_H
#include "colors.h"
#include "xpm/block.xpm"
#include <qpixmap.h>
#include "StripEntry.h"
#include "Selector.h"
#include "asim/AScrollView.h"
#include "StripChartView.h"
#include "LittleSelectorImpl.h"
#include <qiconview.h>
#include <qlistbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qcolordialog.h>
#include <qaccel.h>

class ColorsImpl : public Colors
{ 
    Q_OBJECT

  public:
    ColorsImpl( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0,allFilesData *DB=NULL);
    ~ColorsImpl();
    void UpdateColors();
    void setView(StripChartView *vw);

    allFilesData *myDB;
    StripChartView *view;    
    QAccel    *accelSupr;
    QAccel    *accelIns;
    QAccel    *accelPlus;
    QAccel    *accelMinus;
    int fileSelected;
    int stripSelected;
    int threadSelected;

  public slots:    
    void do_StripClicked(int index);
    void Hide();
    void Unhide();
    void removeElem();
    void insertElem();
    void plusSmoothElem();
    void minusSmoothElem();
};

#endif // COLORSIMPL_H

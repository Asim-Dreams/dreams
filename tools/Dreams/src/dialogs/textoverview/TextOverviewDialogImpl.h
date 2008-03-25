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

#ifndef TEXTOVERVIEWIMPL_H
#define TEXTOVERVIEWIMPL_H

#include "DreamsDefs.h"
#include "dialogs/textoverview/TextOverviewDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

class TextOverviewDialogImpl : public TextOverviewDialog
{
    Q_OBJECT

public:
    TextOverviewDialogImpl( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~TextOverviewDialogImpl();

public slots:
    void SAllButton_clicked();
    void SNondeButton_clicked();
    void CopyButton_clicked();
    void AcceptButton_clicked();
};

#endif // TEXTOVERVIEWIMPL_H

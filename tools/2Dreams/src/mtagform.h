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

/****************************************************************************
** Form interface generated from reading ui file 'mtagform.ui'
**
** Created: Tue Feb 3 16:21:52 2004
**      by: The User Interface Compiler ($Id: mtagform.h 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MTAGFORM_H
#define MTAGFORM_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QListView;
class QListViewItem;
class QPushButton;

class MTagForm : public QDialog
{
    Q_OBJECT

public:
    MTagForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MTagForm();

    QListView* MTagListView;
    QPushButton* AcceptPushButton;

protected:
    QGridLayout* MTagFormLayout;
    QVBoxLayout* layout26;
    QHBoxLayout* layout25;

protected slots:
    virtual void languageChange();
private:
    QPixmap image0;

};

#endif // MTAGFORM_H

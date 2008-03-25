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

/****************************************************************************
** Form interface generated from reading ui file 'GoToCycleDialog.ui'
**
** Created: Tue May 24 14:51:19 2005
**      by: The User Interface Compiler ($Id: GoToCycleDialog.h 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GOTOCYCLEDIALOG_H
#define GOTOCYCLEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QComboBox;
class QButtonGroup;
class QRadioButton;
class QSpinBox;
class QPushButton;

class GoToCycleDialog : public QDialog
{
    Q_OBJECT

public:
    GoToCycleDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GoToCycleDialog();

    QLabel* textLabel1_2;
    QLabel* textLabel1;
    QComboBox* comboBox1;
    QButtonGroup* buttonGroup2;
    QRadioButton* radioButton1_2;
    QRadioButton* radioButton1;
    QSpinBox* spinBox1;
    QPushButton* pushButton1;
    QPushButton* pushButton1_2;

protected:

protected slots:
    virtual void languageChange();

};

#endif // GOTOCYCLEDIALOG_H

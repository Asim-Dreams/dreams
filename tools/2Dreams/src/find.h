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
** Form interface generated from reading ui file 'find.ui'
**
** Created: Tue Feb 3 16:18:22 2004
**      by: The User Interface Compiler ($Id: find.h 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FINDFORM_H
#define FINDFORM_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QToolButton;

class FindForm : public QDialog
{
    Q_OBJECT

public:
    FindForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~FindForm();

    QLabel* TextLabel1;
    QLineEdit* LineEdit_TAGName;
    QToolButton* ToolButton_Search;
    QLabel* TextLabel1_2;
    QLineEdit* LineEdit_TAGValue;
    QLabel* TextLabel1_2_2;
    QSpinBox* SpinBox_Cycle;
    QLabel* textLabel1;
    QCheckBox* CheckBox_CaseSensitive;
    QCheckBox* CheckBox_ExactMatch;
    QCheckBox* CheckBox_Regular;
    QPushButton* FindButton;
    QPushButton* CancelButton;

protected:
    QGridLayout* FindFormLayout;
    QVBoxLayout* layout30;
    QHBoxLayout* layout12;
    QHBoxLayout* layout13;
    QHBoxLayout* layout14;
    QHBoxLayout* layout29;
    QHBoxLayout* layout7;

protected slots:
    virtual void languageChange();
private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;

};

#endif // FINDFORM_H

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

/****************************************************************************
** Form interface generated from reading ui file 'PushoutWidget.ui'
**
** Created: Tue Apr 12 14:40:53 2005
**      by: The User Interface Compiler ($Id: PushoutWidget.h 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PUSHOUTWIDGET_H
#define PUSHOUTWIDGET_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;

class PushoutWidget : public QWidget
{
    Q_OBJECT

public:
    PushoutWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~PushoutWidget();

    QPushButton* nextButton;
    QPushButton* prevButton;
    QPushButton* syncButton;

protected:
    QVBoxLayout* layout4;
    QHBoxLayout* layout2;
    QSpacerItem* spacer1;
    QHBoxLayout* layout3;
    QSpacerItem* spacer2;
    QSpacerItem* spacer2_2;

protected slots:
    virtual void languageChange();

};

#endif // PUSHOUTWIDGET_H

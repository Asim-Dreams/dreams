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
** Form interface generated from reading ui file 'Properties.ui'
**
** Created: Tue May 28 16:47:37 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QPushButton;
class QSpinBox;

class Properties : public QDialog
{ 
    Q_OBJECT

public:
    Properties( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Properties();

    QLabel* TextLabel1;
    QSpinBox* HGridSize;
    QLabel* TextLabel1_3;
    QSpinBox* HGridSteps;
    QLabel* TextLabel1_2;
    QSpinBox* VGridSize;
    QLabel* TextLabel1_2_2;
    QSpinBox* VGridSteps;
    QPushButton* HGridColor;
    QPushButton* VGridColor;
    QPushButton* MarkersLineColor;
    QPushButton* HAxisColor;
    QPushButton* VAxisColor;
    QPushButton* BackgroundColor;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;


protected:
    QGridLayout* PropertiesLayout;
    QVBoxLayout* Layout35;
    QHBoxLayout* Layout28;
    QHBoxLayout* Layout28_3;
    QHBoxLayout* Layout28_2;
    QHBoxLayout* Layout28_2_2;
    QHBoxLayout* Layout1;
};

#endif // PROPERTIES_H

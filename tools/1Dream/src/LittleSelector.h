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
** Form interface generated from reading ui file 'LittleSelector.ui'
**
** Created: Mon Jul 15 15:03:37 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef LITTLESELECTOR_H
#define LITTLESELECTOR_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QComboBox;
class QFrame;
class QLabel;
class QPushButton;
class QSpinBox;
class QTabWidget;
class QWidget;

class LittleSelector : public QDialog
{ 
    Q_OBJECT

public:
    LittleSelector( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~LittleSelector();

    QFrame* StripProperties;
    QCheckBox* Visible;
    QCheckBox* Hidden;
    QTabWidget* Tablet;
    QWidget* tab;
    QLabel* TextLabel4;
    QCheckBox* Absolute;
    QCheckBox* RelativeHardwareMAX;
    QCheckBox* RelativeFileMAX;
    QLabel* HardwareMAX;
    QLabel* FileMAX;
    QCheckBox* SkewY;
    QSpinBox* SkewYValue;
    QWidget* tab_2;
    QCheckBox* SkewX;
    QSpinBox* SkewXValue;
    QWidget* tab_3;
    QCheckBox* BreakView;
    QLabel* TextLabel1_2;
    QSpinBox* BreakValue;
    QCheckBox* Smooth;
    QLabel* TextLabel1;
    QSpinBox* SmoothValue;
    QWidget* tab_4;
    QPushButton* Color;
    QLabel* LineWidth;
    QSpinBox* LineWidthValue;
    QComboBox* LineStyle;
    QLabel* Filename;
    QLabel* StripChart;
    QLabel* Thread;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;


protected:
    QGridLayout* LittleSelectorLayout;
    QGridLayout* StripPropertiesLayout;
    QHBoxLayout* Layout29;
    QGridLayout* tabLayout;
    QVBoxLayout* Layout47;
    QVBoxLayout* Layout46;
    QHBoxLayout* Layout45;
    QVBoxLayout* Layout16;
    QHBoxLayout* Layout15;
    QVBoxLayout* Layout14;
    QVBoxLayout* Layout13;
    QHBoxLayout* Layout21;
    QGridLayout* tabLayout_2;
    QHBoxLayout* Layout23;
    QGridLayout* tabLayout_3;
    QVBoxLayout* Layout26;
    QHBoxLayout* Layout24;
    QHBoxLayout* Layout25;
    QGridLayout* tabLayout_4;
    QVBoxLayout* Layout28;
    QHBoxLayout* Layout27;
    QVBoxLayout* Layout11;
    QHBoxLayout* Layout1;
};

#endif // LITTLESELECTOR_H

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
** Form interface generated from reading ui file 'Selector.ui'
**
** Created: Tue Jul 9 08:42:38 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef SELECTOR_H
#define SELECTOR_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QListBox;
class QListBoxItem;
class QPushButton;

class Selector : public QDialog
{ 
    Q_OBJECT

public:
    Selector( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Selector();

    QFrame* StripSelector;
    QListBox* FilesOpened;
    QListBox* StripsAvailable;
    QListBox* ThreadsOnStrip;
    QPushButton* buttonHelp;
    QPushButton* Properties;
    QPushButton* buttonOk;


protected:
    QGridLayout* SelectorLayout;
    QGridLayout* StripSelectorLayout;
    QVBoxLayout* Layout43;
    QGridLayout* Layout12;
    QHBoxLayout* Layout44;
};

#endif // SELECTOR_H

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
** Form interface generated from reading ui file 'ancestorwidget.ui'
**
** Created: Fri Feb 4 10:37:02 2005
**      by: The User Interface Compiler ($Id: AncestorWidget.h 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ANCESTORWIDGET_H
#define ANCESTORWIDGET_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QSplitter;
class QListView;
class QListViewItem;

class AncestorWidget : public QWidget
{
    Q_OBJECT

public:
    AncestorWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~AncestorWidget();

    QSplitter* theSplitter;
    QListView* ancestorsListView;
    QListView* tagsListView;

protected:
    QGridLayout* AncestorWidgetLayout;

protected slots:
    virtual void languageChange();

};

#endif // ANCESTORWIDGET_H

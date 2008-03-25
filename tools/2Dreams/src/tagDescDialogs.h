/*
 * Copyright (C) 2004-2006 Intel Corporation
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

/**
  * @file tagDescDialogs.h
  */

#ifndef TAGDESCDIALOGS_H
#define TAGDESCDIALOGS_H

#include <qwidget.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qstylefactory.h>
#include <qvariant.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qinputdialog.h>
#include <qstrlist.h>

class ChooseTagDialog : public QDialog
{
    Q_OBJECT

    public:
        ChooseTagDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
        ~ChooseTagDialog();
        void insertItem(QString,QString);
        QString getSelected();
    public slots:
        void mySelectionChanged(QListViewItem*);
        void acceptBtnClicked();
        void cancelBtnClicked();
    private:
        QVBoxLayout* ChooseDialogLayout;
        QHBoxLayout* Layout1;
        QListView* tagList;
        QPushButton* acceptBtn;
        QPushButton* cancelBtn;
        QString      selected;
};

class ShowTagDescDialog : public QDialog
{
    Q_OBJECT

    public:
        ShowTagDescDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
        ~ShowTagDescDialog();
        void insertItem(QString,QString);
        
    public slots:
        void acceptBtnClicked();
        
    private:
        QVBoxLayout* ChooseDialogLayout;
        QHBoxLayout* Layout1;
        QListView* tagList;
        QPushButton* acceptBtn;
        QString      selected;
};

#endif


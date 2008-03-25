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
** Form interface generated from reading ui file 'TextOverviewDialog.ui'
**
** Created: Wed May 25 09:57:24 2005
**      by: The User Interface Compiler ($Id: TextOverviewDialog.h 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef TEXTOVERVIEWDIALOG_H
#define TEXTOVERVIEWDIALOG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QToolButton;
class QTextEdit;

class TextOverviewDialog : public QDialog
{
    Q_OBJECT

public:
    TextOverviewDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~TextOverviewDialog();

    QPushButton* SAllButton;
    QPushButton* SNondeButton;
    QToolButton* CopyButton;
    QTextEdit* textEdit;
    QToolButton* AcceptButton;

public slots:
    virtual void SAllButton_stateChanged(int);

protected:
    QGridLayout* TextOverviewDialogLayout;
    QVBoxLayout* layout131;
    QHBoxLayout* layout128;
    QSpacerItem* spacer62;
    QHBoxLayout* layout129;
    QSpacerItem* spacer63;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;

};

#endif // TEXTOVERVIEWDIALOG_H

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

/****************************************************************************
** Form interface generated from reading ui file 'textOverview.ui'
**
** Created: Tue Feb 3 16:18:24 2004
**      by: The User Interface Compiler ($Id: textOverview.h 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef TEXTOVERVIEWFORM_H
#define TEXTOVERVIEWFORM_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QTextEdit;
class QToolButton;

class TextOverviewForm : public QDialog
{
    Q_OBJECT

public:
    TextOverviewForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~TextOverviewForm();

    QPushButton* SAllButton;
    QPushButton* SNondeButton;
    QToolButton* CopyButton;
    QTextEdit* textEdit;
    QToolButton* AcceptButton;

public slots:
    virtual void SAllButton_stateChanged(int);

protected:
    QGridLayout* TextOverviewFormLayout;
    QVBoxLayout* layout131;
    QHBoxLayout* layout128;
    QHBoxLayout* layout129;

protected slots:
    virtual void languageChange();
private:
    QPixmap image0;
    QPixmap image1;

};

#endif // TEXTOVERVIEWFORM_H

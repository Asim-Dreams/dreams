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

#ifndef QCONFIGDIALOG_H
#define QCONFIGDIALOG_H
/** \file */

#include <qdialog.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qdom.h>
#include <qpushbutton.h>
#include <qfiledialog.h>


typedef int           INT32;
typedef long          INT64;
typedef unsigned int  UINT32;
typedef unsigned long UINT64;


class Entrada
{
  public:
    QHBoxLayout* box;
    QComboBox* combo;
    QCheckBox* check;
};

class QConfigDialog:public QDialog
{
  Q_OBJECT

  public:
    QConfigDialog( QWidget * parent = 0, const char * name = 0, bool modal = FALSE, WFlags f = 0 );
    void toFile();
    ~QConfigDialog();

  public slots:
    void clickedOk();
    void clickedLoad();
    void clickedSave();
    void clickedCancel();

  signals:

  protected:
    void closeEvent( QCloseEvent* );

  private:
    Entrada entrades[4];
    QVBoxLayout* box;
    QHBoxLayout* buttonBox;
    QPushButton *buttonOK, *buttonCancel, *buttonLoad, *buttonSave;
};

#endif //QCONFIGDIALOG_H

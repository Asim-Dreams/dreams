/*****************************************************************************
*
* @brief Dialog to enter a cycle number. Header File.
*
* @author Oscar Rosell
*
*  Copyright (C) 2003-2006 Intel Corporation
*  
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*  
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*  
*
*****************************************************************************/

#include "qcycledialog.h"
#include <qvalidator.h>

QCycleDialog::QCycleDialog(QWidget * parent, const char * name, bool modal, WFlags f):QDialog(parent,name,modal,f)
{
    QIntValidator *val;

    this->setCaption("Destination Cycle");

    val = new QIntValidator(this);
    cycleEdit1 = new QLineEdit(this);
    cycleEdit1->setValidator(val);
    cycleEdit1->show();
    connect(cycleEdit1, SIGNAL( textChanged(const QString &) ),this, SLOT( changedText(const QString &) ));
}

QCycleDialog::~QCycleDialog()
{
}

void QCycleDialog::closeEvent( QCloseEvent* ce )
{
    printf("closeEvent signal received\n");
    ce->accept();
}

void QCycleDialog::changedText( const QString &newString )
{
    cycle = newString.toULong();
    emit (cycleChanged(cycle));
}

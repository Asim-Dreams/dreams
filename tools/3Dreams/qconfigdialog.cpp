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

#include "qconfigdialog.h"
#include "gui/qaddressentry.h"

#define SPACING 5

QConfigDialog::QConfigDialog(QWidget * parent, const char * name, bool modal, WFlags f):QDialog(parent,name,modal,f)
{
    this->setCaption("Mode Configuration");

    box = new QVBoxLayout( this , 0, -1, "one");
    box->addSpacing( SPACING );

    for ( int i=0; i<=3; i++ )
    {
        entrades[i].box = new QHBoxLayout( box , -1, "two" );
        entrades[i].box->addSpacing( SPACING );
        entrades[i].box->addWidget( entrades[i].check = new QCheckBox( this ) );
        entrades[i].box->addSpacing( SPACING );
        entrades[i].box->addWidget( entrades[i].combo = new QComboBox( this ) );
        entrades[i].box->addSpacing( SPACING );
        entrades[i].box->addWidget( new QAddressEntry( this ) );
        entrades[i].box->addSpacing( SPACING );
        box->addSpacing( SPACING );
    }
    buttonBox = new QHBoxLayout( box ,-1, "three" );
    buttonOK = new QPushButton( "OK",this );
    connect(buttonOK, SIGNAL( clicked() ), this, SLOT ( accept() ));
    buttonLoad = new QPushButton( "Load",this );
    connect(buttonLoad, SIGNAL( clicked() ), this, SLOT ( clickedLoad() ));
    buttonSave = new QPushButton( "Save",this );
    connect(buttonSave, SIGNAL( clicked() ), this, SLOT ( clickedSave() ));
    buttonCancel = new QPushButton( "Cancel",this );
    connect(buttonCancel, SIGNAL( clicked() ), this, SLOT ( reject() ));
    buttonBox->addSpacing( SPACING );
    buttonBox->addWidget( buttonOK );
    buttonBox->addSpacing( SPACING );
    buttonBox->addWidget( buttonLoad );
    buttonBox->addSpacing( SPACING );
    buttonBox->addWidget( buttonSave );
    buttonBox->addSpacing( SPACING );
    buttonBox->addWidget( buttonCancel );
    buttonBox->addSpacing( SPACING );
    box->addSpacing( SPACING );
}

QConfigDialog::~QConfigDialog()
{
}

void QConfigDialog::toFile()
{
    QDomDocument* doc = new QDomDocument("Config Dialog State");
    QDomElement root = doc->createElement("config_dialog_state");

    doc->appendChild(root);

    for ( int i=0; i<=3; i++ )
    {
        QDomElement new_elem = doc->createElement("input_line");
        root.appendChild(new_elem);
        QDomElement domcheck = doc->createElement("checkbox");
        QDomAttr domcheckattr = doc->createAttribute("checked");
        domcheckattr.setValue(entrades[i].check->isChecked()?"true":"false");
        domcheck.setAttributeNode(domcheckattr);
        new_elem.appendChild(domcheck);
        QDomElement domcombo = doc->createElement("combobox");
        QDomAttr domcomboattr = doc->createAttribute("selected");
        domcomboattr.setValue(QString::number(entrades[i].combo->currentItem()));
        domcombo.setAttributeNode(domcomboattr);
        new_elem.appendChild(domcombo);
    }
    QString dom= doc->toString();
    printf(dom);
}

void QConfigDialog::clickedOk()
{
}

void QConfigDialog::clickedCancel()
{
}

void QConfigDialog::clickedLoad()
{
    QString filename = QFileDialog::getOpenFileName();
}

void QConfigDialog::clickedSave()
{
    QString filename = QFileDialog::getSaveFileName();
    toFile();
}

void QConfigDialog::closeEvent( QCloseEvent* ce )
{
    printf("closeEvent signal received\n");
    ce->accept();
}

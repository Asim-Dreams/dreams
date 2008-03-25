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

/**
  * @file FindDialogImpl.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "avt/DAvtView.h"
#include "dDB/DralDB.h"
#include "dialogs/find/FindDialogImpl.h"
#include "preferencesImpl.h"
#include "DreamsDB.h"
#include "tagDescDialogs.h"

// Qt includes.
#include <qwidget.h>
#include <qstrlist.h>
#include <qmessagebox.h>
#include <qspinbox.h>

FindDialogImpl::FindDialogImpl(DreamsDB * _dreamsdb, DAvtView * avt, QWidget * parent, const char * name, bool modal, WFlags fl)
    : FindDialog(parent, name, modal, fl)
{
    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();
    myAvt = avt;
    disWin = myAvt->getDisWindow();

    // Combo box with the different clock domains. Fills the combo box with all the clocks.
    for(UINT32 i = 0; i < draldb->getNumClocks(); i++)
    {
        comboBox1->insertItem(draldb->getClockDomain(i)->toString());
    }

    sucessFullSearch = false;
    lastCycle        = draldb->getFirstCycle();
    lastTagId        = 0;
    lastValue        = QString::null;
    lastCS           = false;
    lastEM           = false;
    lastRE           = false;
    lastTagValueType = TAG_UNKNOWN_TYPE;

    ToolButton_Search->setEnabled(false);

    connect(ToolButton_Search, SIGNAL(clicked())                  , this, SLOT(knownTagsClicked()));
    connect(CancelButton     , SIGNAL(clicked())                  , this, SLOT(cancelClicked()));
    connect(FindButton       , SIGNAL(clicked())                  , this, SLOT(findClicked()));
    connect(LineEdit_TAGName , SIGNAL(textChanged(const QString&)), this, SLOT(tagNameTextChanged(const QString&)));
    connect(CheckBox_Regular , SIGNAL(toggled(bool))              , this, SLOT(reToggled(bool)));
}

/*
 *  Destroys the object and frees any allocated resources
 */
FindDialogImpl::~FindDialogImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void
FindDialogImpl::knownTagsClicked(void)
{
    QStrList strlist (false);
    strlist = dreamsdb->getKnownTags();

    ChooseTagDialog dlgChooseTag(this, "ctd", true);
    QStrListIterator it_str(strlist);
    char *str;

    while((str = it_str.current()) != NULL)
    {
        ++it_str;
        dlgChooseTag.insertItem(QString(str), draldb->getTagDesc(str));
    }
    dlgChooseTag.exec();
    if(dlgChooseTag.getSelected() != QString::null)
    {
        LineEdit_TAGName->setText(dlgChooseTag.getSelected());
    }
}

void
FindDialogImpl::tagNameTextChanged(const QString& str)
{
    bool isString = false;
    TAG_ID tagId = draldb->getTagId(LineEdit_TAGName->text());
    if(tagId != TAG_ID_INVALID)
    {
        TagValueType tgtype = draldb->getTagValueType(tagId);
        isString = (tgtype == TAG_STRING_TYPE);
    }
    CheckBox_CaseSensitive->setEnabled(isString);
    CheckBox_Regular->setEnabled(isString);
    CheckBox_ExactMatch->setEnabled(isString && !(CheckBox_Regular->isChecked()));
}

void
FindDialogImpl::reToggled(bool on)
{
    CheckBox_ExactMatch->setEnabled(!(CheckBox_Regular->isChecked()));
}

void
FindDialogImpl::findClicked(void)
{
    // 1) check the tag is known
    TAG_ID tagId = draldb->getTagId(LineEdit_TAGName->text());
    if(tagId == TAG_ID_INVALID)
    {
        QMessageBox::critical(this, "Search Error", "Unknown TAG Name", "&Dismiss");
        return;
    }

    // 2) check get the tag type: only integer and string allowed
    TagValueType tgtype = draldb->getTagValueType(tagId);
    if((tgtype != TAG_INTEGER_TYPE) && (tgtype != TAG_STRING_TYPE))
    {
        QMessageBox::critical(this, "Search Error", "Search allowed only with Integer and String Tags", "&Dismiss");
        return;
    }

    if(tgtype == TAG_INTEGER_TYPE)
    {
        bool parseOk;
        UINT64 value = AGTTypes::str2uint64(LineEdit_TAGValue->text().latin1(), parseOk);
        if(!parseOk)
        {
            QMessageBox::critical(NULL, "Search Error", "Invalid Value for Integer Tag", "&Dismiss");
            return;
        }
    }

    CYCLE cycle = draldb->getFirstEffectiveCycle(comboBox1->currentItem());
    cycle.cycle = QMAX(SpinBox_Cycle->value(), cycle.cycle);

    // 3) look for it
    doFind(LineEdit_TAGValue->text(), tagId, tgtype, cycle, true);

    // 4) save for "search next"
    if(sucessFullSearch)
    {
        lastTagId = tagId;
        lastTagValueType = tgtype;
        lastValue = LineEdit_TAGValue->text();

        lastCS = CheckBox_CaseSensitive->isChecked();
        lastEM = CheckBox_ExactMatch->isChecked();
        lastRE = CheckBox_Regular->isChecked();

        done(QDialog::Accepted);
    }
}

void
FindDialogImpl::find_next()
{
    if(!sucessFullSearch)
    {
        return;
    }

    if(lastCycle >= draldb->getLastCycle())
    {
        QMessageBox::critical(NULL, "Search Error", "End of file reached", "&Dismiss");
        sucessFullSearch = false;
        return;
    }

    doFind(lastValue, lastTagId, lastTagValueType, lastCycle, false);
}

bool
FindDialogImpl::hadSucessFullSearch()
{
    return sucessFullSearch;
}

void
FindDialogImpl::cancelClicked(void)
{
    done(QDialog::Rejected);
}

void
FindDialogImpl::aboutToFind()
{
    ToolButton_Search->setEnabled(dreamsdb->hasLoadedData());
    LineEdit_TAGName->setFocus();
}

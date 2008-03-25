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

/**
  * @file findImpl.cpp
  */

// Qt Library
#include <qwidget.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qapplication.h>
#include <qstrlist.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qregexp.h>
#include <qspinbox.h>
#include <qprogressdialog.h>

#include "findImpl.h"
#include "ZDBase.h"
#include "HighLightMgr.h"

// I reuse a subclass of preferencesImpl
#include "preferencesImpl.h"

bool    findImpl::sucessFullSearch = false;
INT32   findImpl::lastCycle=-1;
INT32   findImpl::lastRow=-1;
UINT16  findImpl::lastTagId=0;
QString findImpl::lastValue=QString::null;
bool    findImpl::lastCS=false;
bool    findImpl::lastEM=false;
bool    findImpl::lastRE=false;
TagValueType findImpl::lastTagValueType=TagUnknownType;

DralDB*      findImpl::draldb = NULL;
DreamsDB*    findImpl::db     = NULL;
TwoDAvtView* findImpl::myAvt  = NULL;
DisDockWin*  findImpl::disWin = NULL;

findImpl::findImpl( TwoDAvtView* avt, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : FindForm( parent, name, modal, fl )
{
    draldb = DralDB::getInstance();

    myAvt=avt;
    disWin=myAvt->getDisWindow();
    db = DreamsDB::getInstance();

    ToolButton_Search->setEnabled(false);
    connect(ToolButton_Search,SIGNAL(clicked()),this,SLOT(knownTagsClicked()));
    connect(CancelButton,SIGNAL(clicked()),this,SLOT(cancelClicked()));
    connect(FindButton,SIGNAL(clicked()),this,SLOT(findClicked()));
    connect(LineEdit_TAGName,SIGNAL(textChanged(const QString&)),this,SLOT(tagNameTextChanged(const QString&)));
    connect(CheckBox_Regular,SIGNAL(toggled(bool)),this,SLOT(reToggled(bool)));
}

/*
 *  Destroys the object and frees any allocated resources
 */
findImpl::~findImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void
findImpl::knownTagsClicked(void)
{
    QStrList strlist (false);
    strlist = db->getKnownTags();

    ChooseTagDialog *dlg = new ChooseTagDialog(this,"ctd",true);
    char *str;
    for (str=strlist.first();str!=NULL;str=strlist.next())
    {
        dlg->insertItem(QString(str),draldb->getTagLongDesc(str));
    }
    dlg->exec();
    if (dlg->getSelected()!=QString::null)
    {
        LineEdit_TAGName->setText(dlg->getSelected());
    }

}

void
findImpl::tagNameTextChanged(const QString& str)
{
    bool isString=false;
    INT32 pos = draldb->scanTagName(LineEdit_TAGName->text());
    if (pos>=0)
    {
        TagValueType tgtype = draldb->getTagValueType(pos);
        isString = (tgtype == TagStringValue);
    }
    CheckBox_CaseSensitive->setEnabled(isString);
    CheckBox_Regular->setEnabled(isString);
    CheckBox_ExactMatch->setEnabled(isString && !(CheckBox_Regular->isChecked()));
}

void
findImpl::reToggled(bool on)
{
    CheckBox_ExactMatch->setEnabled(!(CheckBox_Regular->isChecked()));
}

void
findImpl::findClicked(void)
{
    // 1) check the tag is known
    INT32 pos = draldb->scanTagName(LineEdit_TAGName->text());
    if (pos<0)
    {
        QMessageBox::critical(this, "Search Error", "Unknown TAG Name", "&Dismiss");
        return;
    }

    // 2) check get the tag type: only integer and string allowed
    TagValueType tgtype = draldb->getTagValueType(pos);
    if ( (tgtype!=TagIntegerValue) && (tgtype!=TagStringValue) )
    {
        QMessageBox::critical(this, "Search Error", "Search allowed only with Integer and String Tags", "&Dismiss");
        return;
    }

    // 3) look for it
    doFind(LineEdit_TAGValue->text(),pos,tgtype,SpinBox_Cycle->value(),0,
           CheckBox_CaseSensitive->isChecked(),
           CheckBox_ExactMatch->isChecked(),
           CheckBox_Regular->isChecked()
           );

    // 4) save for "search next"
    if (sucessFullSearch)
    {
        lastTagId=pos;
        lastTagValueType= tgtype;
        lastValue=LineEdit_TAGValue->text();
        lastCS = CheckBox_CaseSensitive->isChecked();
        lastEM = CheckBox_ExactMatch->isChecked();
        lastRE = CheckBox_Regular->isChecked();
        done(QDialog::Accepted);
    }
}

void
findImpl::find_next()
{
    if (!sucessFullSearch)
    {
        return;
    }
    ZDBase* zdb = ZDBase::getInstance();
    INT32 startCycle = lastCycle;
    INT32 startRow = lastRow+1;
    if (startRow >= zdb->getNumRows())
    {
        startRow =0;
        ++startCycle;
    }
    if (startCycle>=(zdb->getNumCycles()+(INT64)(zdb->getFirstEffectiveCycle())))
    {
        QMessageBox::critical(NULL, "Search Error", "End of file reached", "&Dismiss");
        sucessFullSearch=false;
        return;
    }
    doFind(lastValue,lastTagId,lastTagValueType,startCycle,startRow,lastCS,lastEM,lastRE);
}

void
findImpl::doFind(QString str, INT32 tagId, TagValueType tgtype, INT32 startCycle,INT32 startRow,bool cs,bool em,bool re)
{
    //printf ("lookging for '%s' sCycle=%d sRow=%d\n",str.latin1(),startCycle,startRow);

    sucessFullSearch=false;
    ZDBase* zdb = ZDBase::getInstance();

    INT32 numRows = zdb->getNumRows();
    INT32 maxCycle = zdb->getNumCycles()+zdb->getFirstEffectiveCycle();

    INT32 cycle = startCycle;
    if (cycle< (INT64)(zdb->getFirstEffectiveCycle()))
    {
        cycle = zdb->getFirstEffectiveCycle();
    }
    
    bool goon = true;
    bool eventFound = false;
    INT32 row=-1;

    QApplication::setOverrideCursor( Qt::WaitCursor );
    UINT64 value=0;
    // progress dialog
    QProgressDialog progress("Searching...","Abort",zdb->getNumCycles(),NULL,"fprogress",TRUE);
    progress.setMinimumDuration(500);
    progress.setProgress(1);
    if (tgtype==TagIntegerValue)
    {
        bool parseOk;
        value = AGTTypes::str2uint64(str.latin1(),parseOk);
        //printf("search integer value decimal="FMT64U" hex="FMT64X"\n",value,value);
        if (!parseOk)
        {
            QMessageBox::critical(NULL, "Search Error", "Invalid Value for Integer Tag", "&Dismiss");
            return;
        }

        row = zdb->matchTagIdValue(cycle,(UINT16)tagId,value,startRow);
        eventFound = row>=0;
        startRow =0;
        ++cycle;
        while (!eventFound && (cycle<maxCycle) )
        {
            //printf ("scanning cycle %d\n",cycle);fflush(stdout);
            row = zdb->matchTagIdValue(cycle,(UINT16)tagId,value,startRow);
            eventFound = row>=0;
            if (cycle%1024==0)
            {
                progress.setProgress(cycle-zdb->getFirstEffectiveCycle());
                qApp->processEvents();
                if ( progress.wasCancelled() )
                {
                    break;
                }
            }
            ++cycle;
        }
    }
    else
    {
        if (!re)
        {
            row = zdb->matchTagIdValue(cycle,(UINT16)tagId,str,cs,em,startRow);
            eventFound = row>=0;
            startRow =0;
            ++cycle;
            while (!eventFound && (cycle<maxCycle) )
            {
                row = zdb->matchTagIdValue(cycle,(UINT16)tagId,str,cs,em,startRow);
                eventFound = row>=0;
                if (cycle%1024==0)
                {
                    progress.setProgress(cycle-zdb->getFirstEffectiveCycle());
                    qApp->processEvents();
                    if ( progress.wasCancelled() )
                    {
                        break;
                    }
                }
                ++cycle;
            }
        }
        else
        {
            row = zdb->matchTagIdValue(cycle,(UINT16)tagId,QRegExp(str,cs),startRow);
            eventFound = row>=0;
            ++cycle;
            startRow =0;
            while (!eventFound && (cycle<maxCycle) )
            {
                row = zdb->matchTagIdValue(cycle,(UINT16)tagId,QRegExp(str,cs),startRow);
                eventFound = row>=0;
                if (cycle%1024==0)
                {
                    progress.setProgress(cycle-zdb->getFirstEffectiveCycle());
                    qApp->processEvents();
                    if ( progress.wasCancelled() )
                    {
                        break;
                    }
                }
                ++cycle;
            }
        }
    }

    QApplication::restoreOverrideCursor();

    if ( progress.wasCancelled() )
    {
        progress.setProgress(zdb->getNumCycles());
        return;
    }
    progress.setProgress(zdb->getNumCycles());

    if (!eventFound)
    {
        QMessageBox::critical(NULL, "Search Error", "No event found related with search criterion", "&Dismiss");
    }
    else
    {
        --cycle;
        lastCycle=cycle;
        lastRow=row;
        sucessFullSearch=true;

        // go there!
        myAvt->goToColumn(cycle-zdb->getFirstEffectiveCycle());
        myAvt->goToRow(row);
        myAvt->shadeColumn(cycle-zdb->getFirstEffectiveCycle(),true);

        (myAvt->getHighLightMgr())->contentsSelectEvent(cycle,row,true,((TwoDAvtView*)myAvt)->getCurrentTagSelector(),HLT_DEFAULT,true);
        myAvt->repaintContents(false);
        disWin->alignWith(cycle,row);
    }
}

bool
findImpl::hadSucessFullSearch()
{ return sucessFullSearch; }

void
findImpl::cancelClicked(void)
{
    done(QDialog::Rejected);
}

void
findImpl::aboutToFind()
{
    ToolButton_Search->setEnabled(db->hasLoadedData());
    LineEdit_TAGName->setFocus();
}


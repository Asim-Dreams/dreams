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
  * @file AProgressDialog.cpp
  */

#include <qapplication.h>
#include <qregexp.h>
#include <qprocess.h>

#include "IOMgr.h"
#include "PreferenceMgr.h"

// -----------------------------------------------------------------------
// AProgressDilaog
// -----------------------------------------------------------------------

QSize
AProgressDialog::sizeHint() const
{
    /*
  QSize parent=QProgressDialog::sizeHint();
  return QSize( parent.width()+stopButton->sizeHint().width(),
                  parent.height()+stopButton->sizeHint().height() );
                 */
    QFontMetrics fm = qApp->fontMetrics();
    QString str = "Processed DRAL commands: 999999999999999999999999999";
    QRect tbr = fm.boundingRect(str);
    return QSize(tbr.width()+80,120+tbr.height()*4);

}

void
AProgressDialog::layout()
{
    int sp = 4;
    int mtb = 10;
    int mlr = QMIN(width()/10, 10);
    const bool centered =
	bool(style().styleHint(QStyle::SH_ProgressDialog_CenterCancelButton, this));

    QSize as = abortButton->sizeHint();
    QSize ss = stopButton->sizeHint();

    stopButton->setGeometry(width() - as.width() - ss.width() - 10,height() - 10 - ss.height() + 4,
                ss.width(), ss.height() );

    abortButton->setGeometry(width() - as.width() - 4,height() - 10 - as.height() + 4,
                as.width(), as.height() );
}

void
AProgressDialog::resizeEvent( QResizeEvent *e )
{
    QProgressDialog::resizeEvent(e);
    layout();
}

void
AProgressDialog::styleChange(QStyle& s)
{
    QProgressDialog::styleChange(s);
    layout();
}

// -----------------------------------------------------------------------
// IOMgr
// -----------------------------------------------------------------------
IOMgr* IOMgr::_myInstance = NULL;

IOMgr*
IOMgr::getInstance(QWidget* parent)
{
    if (_myInstance==NULL)
        _myInstance = new IOMgr(parent);Q_ASSERT(_myInstance!=NULL);

    return _myInstance;
}

IOMgr::IOMgr(QWidget* pparent)
{
    dreamsDB = DreamsDB::getInstance();
    this->parent = pparent;

    t = new QTimer( this );Q_ASSERT(t!=NULL);
    Q_ASSERT(t!=NULL);
    connect( t, SIGNAL(timeout()), this, SLOT(processEvent()) );

    progress = new AProgressDialog(parent,"progress",FALSE);Q_ASSERT(progress!=NULL);
    connect( progress, SIGNAL(aborted()), this, SLOT(cancelRead()) );
    connect (progress,SIGNAL(stopped()),SLOT(stopRead()));
    progress->setMinimumDuration(1500);

    qpbar = new QProgressBar (progress,"IOMGR qpbar");Q_ASSERT(qpbar!=NULL);
    progress->setBar(qpbar);

    plabel = new QLabel("\nProcessed DRAL commands:\nCycles:",parent);Q_ASSERT(plabel!=NULL);
    plabel->setMargin(5);
    plabel->setAlignment(Qt::AlignLeft);
    QFont st = qApp->font();
    plabel->setFont(QFont("Courier",st.pointSize(),QFont::Bold));
    progress->setLabel(plabel);

    QString lastUsedDir = PreferenceMgr::getInstance()->getLastUsedDir();
    fdlg = new QFileDialog(lastUsedDir,"DRAL Event-Files (*.drl *.drl.gz)", pparent, "opendlg",TRUE);

    draldb = DralDB::getInstance();
}

IOMgr::~IOMgr()
{
    if (t!=NULL) { delete t; }
    if (progress!=NULL) { delete progress; }
    if (plabel!=NULL) { delete plabel; }
}

bool
IOMgr::loadDRLHeader(QString sfn)
{
    //printf ("IOMgr::loadDRLHeader called\n");
    bool openok = draldb->openDRLFile(sfn);
    if (!openok)
    {
        QMessageBox::critical(NULL,"IO Error",
        "Unable to read "+sfn+" file.",
        QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
        return (false);
    }

    reset();

    //printf("reading drl header...\n");fflush(stdout);
    bool hok = draldb->loadDRLHeader();
    if (!hok)
    {
        (LogMgr::getInstance())->addLog("Error reading header of "+sfn);
        return (false);
    }
    // all right then
    return (true);
}

QString
IOMgr::getEventFile(QString fn)
{
    int dresult=QDialog::Accepted;
    if (fn==QString::null)
    {
        dresult = fdlg->exec();
        fn = fdlg->selectedFile();
        if (fn!=QString::null)
        {
            PreferenceMgr::getInstance()->setLastUsedDir(fdlg->dirPath());
        }
    }

    if ((fn==QString::null)||(dresult!=QDialog::Accepted) )
    {
        return (QString::null);
    }

    return fn;
}

void
IOMgr::reset()
{
    //printf ("IOMgr::reset() called \n");
    avt->reset();
    dreamsDB->reset();
    steps = 0;
}

bool
IOMgr::getADF()
{
    //printf ("IOMgr::getADF() called \n");
    ADFMgr* adfmgr = ADFMgr::getInstance();

    // run all the low level stuff
    bool adfok = adfmgr->getADF();
    if (!adfok) { return false; }

    //printf ("IOMgr::getADF() got a valid dom doc...\n");
    // get an appropiated parser...
    ADFParserInterface* parser = ADFParserBuilder::getSuitableParser(adfmgr->getDomDocument());
    if (parser==NULL)
    {
        QString err = ADFParserBuilder::getErrorMsg();
        (LogMgr::getInstance())->addLog(err);
        return (false);
    }

    //printf ("IOMgr::getADF() got a valid parser \n");

    // propagate the parser
    dreamsDB->setADFParser(parser);

    return true;
}

void
IOMgr::startLoading()
{
    // manually invoque adf evaluation 
    /// @todo this should be removed when startactivity callback gets implemented
    dreamsDB->startActivity(draldb->getFirstEffectiveCycle());

    // init progress bar...
    INT64 fileSize = draldb->getFileSize();
    if (fileSize<0)
    {
        qpbar->hide();
    }
    else
    {
        qpbar->show();
        progress->setTotalSteps(fileSize);
    }

    // start READ timer...
    lastLblRefresh=0;
    lastVPRefresh=0;
    t->start(0);
}

void
IOMgr::processEvent()
{
    //printf("IOMgr::processEvent() called\n");fflush(stdout);
    // to avoid running spureous timer ticks
    if (dreamsDB->reachedEOS()) return;

    // do process a dral command
    INT32 ncmd = draldb->processEvents(IOMGR_PROCESS_STEP);
    //printf("processEvent called and processed %d commands\n",ncmd);fflush(stdout);
    bool ok = !dreamsDB->getHasFatalError();
    steps+=ncmd;
    progress->setProgress( (int)draldb->getNumBytesRead() );

    if ((ncmd==0) || !ok || dreamsDB->reachedEOS())
    {
        stopRead();
        if (!ok)
        {
            QMessageBox::critical(NULL,"Input Error",
            "Severe errors detected while reading DRAL file.\nSee log file for details.",
            QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
        }
        else if (dreamsDB->nonCriticalErrors())
        {
            QMessageBox::warning(NULL,"Input Warnings",
            "Warnings detected while reading DRAL file.\nSee log file for details.",
            QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
        }
        return;
    }

    if ((steps-lastLblRefresh)>10000)
    {
        QString lbl ="\nProcessed DRAL commands: " + QString::number(steps)+"\n";
        lbl = lbl +    "Cycles                 : " + QString::number(dreamsDB->getNumCycles());
        plabel->setText(lbl);
        lastLblRefresh = steps;
    }
    if (steps-lastVPRefresh>100000)
    {
    	refreshAvt();
    	lastVPRefresh=steps;
    }
}

void
IOMgr::refreshAvt()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    // resize avt accordingly
    INT32 ncycles = dreamsDB->getNumCycles();
    INT32 nrows   = dreamsDB->getNumRows();
    avt->setRealContentsSize (ncycles,nrows);
    (LogMgr::getInstance())->flush();

    QApplication::restoreOverrideCursor();
}

void
IOMgr::cancelRead()
{
    t->stop();
    progress->setProgress(draldb->getFileSize());
    progress->reset();
    progress->hide();
    QApplication::setOverrideCursor( Qt::WaitCursor );
    qApp->processEvents();
    dreamsDB->reset();
    avt->reset();
    (LogMgr::getInstance())->flush();

    QApplication::restoreOverrideCursor();
    emit loadCancel();
}

void
IOMgr::stopRead()
{
    t->stop();
    progress->setProgress(draldb->getFileSize());
    progress->reset();
    progress->hide();

    QApplication::setOverrideCursor( Qt::WaitCursor );
    draldb->closeDRLFile();
    LiveDB::getInstance()->Do_EndSimulation();
    qApp->processEvents();  // clean-up pending ticks
    refreshAvt();
    (ZDBase::getInstance())->flushCachedLookUps();
    QApplication::restoreOverrideCursor();
    emit loadFinished();
}

bool
IOMgr::versionCheck(QDomElement docElem, QString filename)
{
  //cout  << "2dreams version:" << docElem.attribute("version") << endl;
  QString strversion = docElem.attribute("version");
  if (strversion==QString::null)
  {
      QString err = "Parsing error (version attribute) reading Architecture Definition File " +
      filename + ".";
      (LogMgr::getInstance())->addLog(err);
      return (false);
  }

  int major,minor,nversion,thisversion;
  int match = sscanf(strversion.latin1(),"%d.%d",&major,&minor);
  nversion = major*100+minor;
  thisversion = DREAMS2_MAJOR_VERSION*100+DREAMS2_MINOR_VERSION;

  if (match!=2)
  {
      QString err = "Parsing error (version encoding) reading Architecture Definition File " +
      filename + ".";
      (LogMgr::getInstance())->addLog(err);
      return (false);
  }

  if (nversion>thisversion)
  {
      QString err = "The Architecture Definition File ("+filename+") version ("+
      strversion+") is higher than the supported (" + DREAMS2_STR_MAJOR_VERSION +
      "." + DREAMS2_STR_MINOR_VERSION+").";
      (LogMgr::getInstance())->addLog(err);
      return (false);
  }
  return (true);
}

bool
IOMgr::isLoading()
{ return t->isActive(); }



TwoDAvtView* IOMgr::avt = NULL;
void
IOMgr::setAVT(TwoDAvtView* _avt)
{ avt = _avt; }



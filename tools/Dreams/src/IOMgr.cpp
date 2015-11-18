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
  * @file AProgressDialog.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "adf/ADFMgr.h"
#include "adf/ADFParserBuilder.h"
#include "avt/ResourceDAvtView.h"
#include "IOMgr.h"
#include "PreferenceMgr.h"
#include "TempDir.h"

// Qt includes.
#include <qapplication.h>
#include <qregexp.h>
#include <qprocess.h>

// -----------------------------------------------------------------------
// AProgressDilaog
// -----------------------------------------------------------------------

QSize
AProgressDialog::sizeHint() const
{
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
    int mlr = QMIN(width() / 10, 10);
    const bool centered =
    bool(style().styleHint(QStyle::SH_ProgressDialog_CenterCancelButton, this));

    QSize as = abortButton->sizeHint();
    QSize ss = stopButton->sizeHint();

    stopButton->setGeometry(width() - as.width() - ss.width() - 10,height() - 10 - ss.height() + 4, ss.width(), ss.height() );
    abortButton->setGeometry(width() - as.width() - 4,height() - 10 - as.height() + 4, as.width(), as.height() );
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

/*
 * Creator of the class.
 *
 * @return new object.
 */
IOMgr::IOMgr(DralDB * _draldb, LogMgr * log, QWidget * pparent)
{
    temp_dir = TempDir::getInstance();
    draldb = _draldb;
    logmgr = log;
    parent = pparent;

    // Creates a timer.
    t = new QTimer(this);
    Q_ASSERT(t != NULL);
    connect(t, SIGNAL(timeout()), this, SLOT(processEvent()));

    // Creates the progress dialog.
    progress = new AProgressDialog(parent, "progress", true);
    Q_ASSERT(progress != NULL);
    progress->setMinimumDuration(1500);
    connect(progress, SIGNAL(aborted()), this, SLOT(cancelRead()));
    connect(progress, SIGNAL(stopped()), SLOT(stopRead()));

    qpbar = new QProgressBar(progress, "IOMGR qpbar");
    Q_ASSERT(qpbar != NULL);
    progress->setBar(qpbar);

    plabel = new QLabel("\nProcessed DRAL commands:\nCycles:", parent);
    Q_ASSERT(plabel != NULL);
    plabel->setMargin(5);
    plabel->setAlignment(Qt::AlignLeft);

    QFont st = qApp->font();
    plabel->setFont(QFont("Courier", st.pointSize(), QFont::Bold));
    progress->setLabel(plabel);

    steps = 0;
}

/*
 * Destructor of the class. Destroys the allocated objects.
 *
 * @return destroys the object.
 */
IOMgr::~IOMgr()
{
    delete t;
    delete progress;
}

/*
 * Starts the loading of the trace.
 *
 * @return void.
 */
void
IOMgr::startLoading()
{
    // Init progress bar.
    INT64 fileSize = draldb->getFileSize();

    if(fileSize < 0)
    {
        qpbar->hide();
        logmgr->addLog("The file size is negative. This usually happens with non-closed gz files.\nHiding the progress bar.");
    }
    else
    {
        qpbar->show();
        progress->setTotalSteps(fileSize);
        progress->setProgress(0);
    }

    // Start READ timer...
    lastLblRefresh = 0;
    lastVPRefresh = 0;
    t->start(0);
}

/*
 * Process events.
 *
 * @return void.
 */
void
IOMgr::processEvent()
{
    // Avoids running spureous timer ticks.
    if(draldb->reachedEOS())
    {
        return;
    }

    // Do process a dral command.
    INT32 ncmd = draldb->processEvents(IOMGR_PROCESS_STEP);

    bool ok = draldb->getLastProcessedEventOk();
    steps += ncmd;
    progress->setProgress((INT32) draldb->getNumBytesRead());

    CLOCK_ID clockId = (* draldb->getClockDomainFreqIterator())->getId();

    if((ncmd == 0) || !ok || draldb->reachedEOS())
    {
        t->stop();
        if(!ok)
        {
            QMessageBox::critical(NULL,"Input Error", "Severe errors detected while reading DRAL file.\nSee log file for details.", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
        }
        stopRead();
        return;
    }

    if((steps - lastLblRefresh) > 10000)
    {
        QString lbl ="\nProcessed DRAL commands: " + QString::number(steps) + "\n";
        lbl = lbl +    "Cycles                 : " + QString::number(draldb->getLastEffectiveCycle(clockId).cycle - draldb->getFirstEffectiveCycle(clockId).cycle);
        plabel->setText(lbl);
        lastLblRefresh = steps;
    }
}

/*
 * Slot called when the cancel button is pressed.
 *
 * @return void.
 */
void
IOMgr::cancelRead()
{
    // Stops the timer and hides the progress bar.
    t->stop();
    progress->setProgress(draldb->getFileSize());
    progress->reset();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    qApp->processEvents();

    QApplication::restoreOverrideCursor();
    emit loadCancel();
}

/*
 * Slot called when the stop button is pressed.
 *
 * @return void.
 */
void
IOMgr::stopRead()
{
    // Hides the progress bar and stops the timer.
    t->stop();
    progress->setProgress(draldb->getFileSize());
    progress->reset();

    // Closes the file.
    QApplication::setOverrideCursor(Qt::WaitCursor);
    draldb->closeDRLFile();
    qApp->processEvents();  // clean-up pending ticks
    QApplication::restoreOverrideCursor();

    emit loadFinish();
}

bool
IOMgr::isLoading()
{
    return t->isActive();
}

/*
 * Function that gets the content of the embedded tar file.
 *
 * @return if everything is ok.
 */
void
IOMgr::getTarContent(const DralComment * comment)
{
    QProcess * proc;

    // Checks that the temp file can be created.
    if(!temp_dir->createTempFile("_ADF_TAR_FILE_") || !temp_dir->copyTempBuffer("_ADF_TAR_FILE_", comment->getComment(), comment->getCommentLength()))
    {
        return;
    }

    // Creates the process.
    proc = new QProcess();
    Q_ASSERT(proc != NULL);

    // Sets the arguments to untar the comment.
    proc->addArgument("tar");
    proc->addArgument("xvf");
    proc->addArgument("_ADF_TAR_FILE_");

    // Sets the working directory of the process to the temp dir.
    proc->setWorkingDirectory(QDir(temp_dir->getTempDir()));

    // Starts the process and checks that started correctly.
    if(!proc->start())
    {
        QString err = "Unable to launch tar subprocess";
        logmgr->addLog(err);
        delete proc;
        return;
    }

    // Waits until the process has finished.
    while(proc->isRunning())
    {
        qApp->processEvents();
    }

    if(!proc->normalExit())
    {
        QString err = "Problems during tar execution.";
        logmgr->addLog(err);
    }
    delete proc;
}

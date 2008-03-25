/*
 * Copyright (C) 2005-2006 Intel Corporation
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
  * @file ADFMgr.cpp
  */

// General includes.
#ifdef Q_OS_UNIX
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
#endif
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/LogMgr.h"
#include "ADFMgr.h"
#include "TempDir.h"

// Qt includes.
#include <qdom.h>
#include <qprocess.h>
#include <qapplication.h>
#include <qregexp.h>
#include <qtimer.h>

/*
 * Creator of the class.
 *
 * @return new object.
 */
ADFMgr::ADFMgr(LogMgr * _log)
{
    // Copies the parameters.
    log = _log;

    // Sets the default values.
    commandLineADF = QString::null;
    defaultADF = QString::null;
    hasEmbeddedADF = false;
    embeddedADFContents = QString::null;
    dtdCheck = false;
    dtdChecker = QString::null;
    hasStdOut = false;
    hasStdErr = false;
    someStdErr = false;
    dtd_timeout = false;
    adf_timeout = false;
    xmlAdf = NULL;
    temp_dir = TempDir::getInstance();
    adf_content_file = "_ADF_CONTENT_";
}

/*
 * Destructor of the class.
 *
 * @return destroys the object.
 */
ADFMgr::~ADFMgr()
{
    delete xmlAdf;
}

bool
ADFMgr::setCommandLineADF(QString path)
{
    if(path == QString::null)
    {
        return false;
    }

    QFile f(path);
    bool ex = f.exists();
    if(ex)
    {
        commandLineADF = path;
    }
    return ex;
}

bool
ADFMgr::setDefaultADF(QString path)
{
    if(path == QString::null)
    {
        return false;
    }

    QFile f(path);
    bool ex = f.exists();
    if (ex)
    {
        defaultADF = path;
    }
    return ex;
}

void
ADFMgr::setEmbeddedADF(QString adf)
{
    embeddedADFContents = adf;
    hasEmbeddedADF = (adf!=QString::null);
}

void
ADFMgr::setCheckDTD(QString checkercmd)
{
    dtdCheck = (checkercmd!=QString::null);
    dtdChecker = checkercmd;
}

QDomDocument*
ADFMgr::getDomDocument() const
{
    return xmlAdf;
}

bool
ADFMgr::getADF()
{
    QString uadf = QString::null;

    if(!temp_dir->createTempFile(ADF_CONTENT))
    {
        return false;
    }

    // Get the adf string.
    QString adf = getADFString();
    if(adf == QString::null)
    {
        temp_dir->deleteTempFile(ADF_CONTENT);
        return false;
    }

    // Check if it is a script.
    uadf = getADFContent(adf);
    if(uadf == QString::null)
    {
        temp_dir->deleteTempFile(ADF_CONTENT);
        return false;
    }

    // TODO: how handle adfs that aren't copied into a file??
    // check it though dtd if needed
    if(dtdCheck)
    {
        if(!dtdcheck(ADF_CONTENT))
        {
            temp_dir->deleteTempFile(ADF_CONTENT);
            return false;
        }
        else
        {
            log->addLog("DTD validation successful.");
        }
    }

    // Try to create the dom object.
    INT32 errorline;
    INT32 errorcolumn;
    QString errmsg;
    QString domErrStr;
    xmlAdf = new QDomDocument();
    bool domOk = xmlAdf->setContent(uadf, false, &errmsg, &errorline, &errorcolumn);
    if(!domOk)
    {
        QString err = "Parsing error reading Architecture Definition File (ADF).";
        err = err + '\n';
        err = err + "Parser message: " + errmsg + ". Line " + QString::number(errorline) +
        " column " + QString::number(errorcolumn);
        err = err + ".";
        log->addLog(err);
        return false;
    }

    // All right then!
    return true;
}

QString
ADFMgr::getADFString()
{
    // command line takes precedence over any other option.
    if(commandLineADF != QString::null)
    {
        log->addLog("Loading External ADF: " + commandLineADF);
        return getADFStringFromFile(commandLineADF);
    }

    // second in importante: embedded string
    if(hasEmbeddedADF)
    {
        log->addLog("Using embedded ADF.");
        return embeddedADFContents;
    }

    // if not embedded, nor command line, try with the default one
    log->addLog("Loading Default ADF: " + defaultADF);
    return getADFStringFromFile(defaultADF);
}

QString
ADFMgr::getADFStringFromFile(QString filename)
{
    //printf ("readADF() called on %s\n",filename.latin1());fflush(stdout);
    QFile f(filename);
    if(!f.open(IO_ReadOnly))
    {
        QString err = "Unable to read " + filename + ".";
        log->addLog(err);
        return (QString::null);
    }

    // read the whole file and produce a string
    QString txt="";
    QTextStream fstream(&f);
    fstream.setEncoding(QTextStream::Latin1);
    QString line=fstream.readLine();
    while (line!=QString::null)
    {
        txt += line+"\n";
        line = fstream.readLine();
    }
    return txt;
}

QString
ADFMgr::getADFContent(QString adf)
{
    QString result;

    if(adf == QString::null)
    {
        return QString::null;
    }
    if(adf.length() < 4)
    {
        return QString::null;
    }

    // check if the file is a script instead a simple txt file (perl or similar)
    int npos = adf.find('\n');
    if(npos < 0)
    {
        log->addLog("Warning, suspicious ADF detected: "+adf);
        return adf;
    }

    QString firstLine = adf.left(npos);
    if((firstLine == QString::null) || (firstLine.length() < 1))
    {
        log->addLog("Invalid ADF '" + firstLine + "'");
        return QString::null;
    }

    //printf ("first line=%s \n",firstLine.latin1());

    // 1) "#!" Kind of:
    bool c1 = (firstLine[0]==QChar('#') && firstLine[1]==QChar('!'));

    // 2) ":" kind of:
    bool c2 = (firstLine[0]==QChar(':'));

    if(c1 || c2)
    {
        if(c1)
        {
            log->addLog("Scripted ADF detected: '" + firstLine + "'");
        }
        else
        {
            log->addLog("Scripted (bash-like) ADF detected");
        }
        return scriptResult(adf);
    }
    else
    {
        // if nothing of this, assume is the ADF itself and copies it to a temp file.
        if(!temp_dir->copyTempString(ADF_CONTENT, adf))
        {
            return QString::null;
        }
        return adf;
    }
}

QString
ADFMgr::scriptResult(QString adf)
{
    // Save the contents in a tmp file
    QFile f;
    QString script;

    if(!temp_dir->createTempFile(ADF_FILE) || !temp_dir->copyTempString(ADF_FILE, adf))
    {
        return QString::null;
    }

/*    // Give execution permitions
#ifdef Q_OS_UNIX
    int gok = chmod(scriptFilename,S_IRUSR|S_IXUSR);
    if (gok<0)
    {
        QString err = "Unable to give execution permission for " + _scriptFilename + ".";
        log->addLog(err);
        return (QString::null);
    }
#endif*/

    // Run it
    bool ok = runScript();

    // Return the contents of the dumped file
    QString result;
    if (ok)
    {
        result = getADFStringFromFile(temp_dir->getTempDir() + ADF_CONTENT);
    }
    else
    {
        result = QString::null;
    }

    // remove tmp files
    temp_dir->deleteTempFile(ADF_FILE);
    return result;
}

bool
ADFMgr::runScript()
{
    // create the child process
    QProcess * valproc;

    valproc = new QProcess(this);
    Q_ASSERT(valproc!=NULL);
    connect(valproc, SIGNAL(readyReadStdout()), this, SLOT(readyReadStdout()));

    valproc->setWorkingDirectory(QDir(temp_dir->getTempDir()));

    // Sets the program arguments.
    valproc->addArgument("perl");
    valproc->addArgument(ADF_FILE);

    // Sets the global program variables.
    adf_timeout = false;
    hasStdOut = false;
    hasStdErr = false;
    writeStdin = true;

    bool started = valproc->start();
    if (!started)
    {
        QString err = "Unable to launch ADF subprocess " + QString(ADF_FILE);
        log->addLog(err);
        delete valproc;
        return false;
    }

    // Aux timer to avoid too long hang-up
    QTimer::singleShot(ADFMGR_ADF_SUBPROCESS_TIMEOUT, this, SLOT(adf_subprocess_timeout()));

    volatile bool keepRunning; ///< Boolean variable to know when the execution has finished.
    QString err = "";          ///< Stores the standard error.
    QString adfstr = "";       ///< Stores the standard output.

    keepRunning = valproc->isRunning();

    while (keepRunning && !adf_timeout)
    {
        // If the process has standard error.
        if (hasStdErr)
        {
            // Reads all the error message.
            while(valproc->canReadLineStderr())
            {
                err += valproc->readLineStderr() + "\n";
            }
            hasStdErr = false;
        }

        // If the process has standard output.
        if (hasStdOut)
        {
            // Reads all the standard output.
            while(valproc->canReadLineStdout())
            {
                adfstr += valproc->readLineStdout() + "\n";
            }
            hasStdOut = false;
        }
        keepRunning = valproc->isRunning();
        qApp->processEvents();
    }

    // Reads all the standard output.
    while(valproc->canReadLineStdout())
    {
        adfstr += valproc->readLineStdout() + "\n";
    }

    // Reads all the standard output.
    while(valproc->canReadLineStderr())
    {
        err += valproc->readLineStderr() + "\n";
    }

    // If an error has been raised is added in the log.
    if(err != "")
    {
        log->addLog("Error while executing the perl script:\n" + err);
    }

    // Copies the unscripted ADF in a temporary file.
    if(!temp_dir->copyTempString(ADF_CONTENT, adfstr))
    {
        return false;
    }

    // check normal completion
    if (!valproc->normalExit() || adf_timeout)
    {
      log->addLog("ADF subprocess failed.");
      if (adf_timeout)
      {
          log->addLog("Time out!");
      }
      delete valproc;
      return false;
    }

    // everything went fine!
    delete valproc;
    return true;
}

bool
ADFMgr::dtdcheck(QString unscriptedadf)
{
    bool result = false;

    someStdErr=false;
    hasStdErr=false;
    dtd_timeout=false;

    // setup validator command line
    QString valcmd = dtdChecker;
    valcmd += " " + unscriptedadf;
    QRegExp exp("\\s");
    const QStringList tokens = QStringList::split(exp,valcmd);
    QProcess* valproc = new QProcess(this);
    Q_ASSERT(valproc!=NULL);
    connect(valproc,SIGNAL(readyReadStderr()),this,SLOT(readyReadStderr()));
    valproc->setArguments(tokens);
    valproc->setWorkingDirectory(QDir(temp_dir->getTempDir()));
    if (!valproc->start())
    {
      log->addLog("Unable to launch validation process as "+valcmd);
      result=false;
    }
    else
    {
      // Aux timer to avoid too long hang-up
      QTimer::singleShot(ADFMGR_ADF_SUBPROCESS_TIMEOUT,this,SLOT(dtd_validation_timeout()));
      QString errorMsg="";
      volatile bool keepRunning = valproc->isRunning();
      while (keepRunning && !dtd_timeout)
      {
          someStdErr = someStdErr || hasStdErr;
          if (hasStdErr)
          {
              QByteArray ba = valproc->readStderr();
              hasStdErr=false;
              QString strMsg(ba);
              errorMsg+=strMsg;
          }
          keepRunning=valproc->isRunning();
          qApp->processEvents();
      }

      if (!valproc->normalExit() || someStdErr || dtd_timeout)
      {
          log->addLog("DTD validation process failed.");
          if (someStdErr)
          {
              log->addLog(errorMsg);
          }
          if (dtd_timeout)
          {
              log->addLog("Time out!");
          }
          result=false;
      }
    }
    delete valproc;
    return true;
}

/**
 * Slot called when the standard error of the qprocess can be read.
 *
 * @return void.
 */
void
ADFMgr::readyReadStderr()
{
    hasStdErr = true;
}

/**
 * Slot called when the standard output of the qprocess can be read.
 *
 * @return void.
 */
void
ADFMgr::readyReadStdout()
{
    hasStdOut = true;
}

/**
 * Slot called when the timeout is reached.
 *
 * @return void.
 */
void
ADFMgr::dtd_validation_timeout()
{
    dtd_timeout = true;
}

/**
 * Slot called when the timeout is reached.
 *
 * @return void.
 */
void
ADFMgr::adf_subprocess_timeout()
{
    adf_timeout = true;
}

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
  * @file ADFMgr.cpp
  */

#include "ADFMgr.h"

#ifdef Q_OS_UNIX
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
#endif

ADFMgr* ADFMgr::_myInstance = NULL;

ADFMgr*
ADFMgr::getInstance()
{
    if (_myInstance==NULL)
    {
        _myInstance = new ADFMgr();
        Q_ASSERT(_myInstance!=NULL);
    }
    return _myInstance;
}

void 
ADFMgr::destroy()
{
    if (_myInstance!=NULL)
    {
        delete _myInstance;
        _myInstance=NULL;
    }
}

ADFMgr::ADFMgr()
{ reset(); }

ADFMgr::~ADFMgr()
{
    if (xmlAdf!=NULL) { delete xmlAdf; }
}

void
ADFMgr::reset()
{
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
    _scriptFilename = QString::null;
    _unscriptedFileName = QString::null;
    xmlAdf = NULL;
}

bool    
ADFMgr::setCommandLineADF(QString path)
{
    QFile f(path);
    bool ex = f.exists();
    if (ex)
    {
        commandLineADF = path;
    }
    return ex;
}

bool
ADFMgr::setDefaultADF(QString path)
{
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
ADFMgr::getDomDocument()
{ return xmlAdf; }

bool
ADFMgr::getADF()
{
    QString uadf=QString::null;
    QFileInfo fi (commandLineADF);

    // check if we have an explicit **BINARY** ADF
    if ((commandLineADF!=QString::null) && (fi.isExecutable()) )
    {
        uadf = runBinaryADF();
        if (uadf==QString::null) { return false; }
    }
    else
    {
        // get the adf string
        QString adf = getADFString();
        if (adf==QString::null) { return false; }

        // check if it is a script
        uadf = getUnscriptedADF(adf);
        if (uadf==QString::null) { return false; }
    }

    // check it though dtd if needed
    if (dtdCheck)
    {
        if (!dtdcheck(_unscriptedFileName))
        { return false; }
        else
        {
            (LogMgr::getInstance())->addLog("DTD validation successful.");
        }
    }

    // try to create the dom object
    int errorline;
    int errorcolumn;
    QString errmsg;
    QString domErrStr;
    xmlAdf = new QDomDocument();
    bool domOk = xmlAdf->setContent(uadf,false,&errmsg,&errorline,&errorcolumn);
    if (!domOk)
    {
        QString err = "Parsing error reading Architecture Definition File (ADF) " + _unscriptedFileName + ".";
        err = err + '\n';
        err = err + "Parser message: " + errmsg + ". Line " + QString::number(errorline) +
        " column " + QString::number(errorcolumn);
        err = err + ".";
        (LogMgr::getInstance())->addLog(err);
        return (false);
    }
    // all right then
    return true;
}

QString
ADFMgr::getADFString()
{
    // command line takes precedence over any other option.
    if (commandLineADF!=QString::null)
    {
        (LogMgr::getInstance())->addLog("Loading External ADF: "+commandLineADF);
        return getADFStringFromFile(commandLineADF);
    }

    // second in importante: embedded string
    if (hasEmbeddedADF)
    {
        (LogMgr::getInstance())->addLog("Using embedded ADF.");
        return embeddedADFContents;
    }

    // if not embedded, nor command line, try with the default one
    (LogMgr::getInstance())->addLog("Loading Default ADF: "+defaultADF);
    return getADFStringFromFile(defaultADF);
}

QString
ADFMgr::getADFStringFromFile(QString filename)
{
    //printf ("readADF() called on %s\n",filename.latin1());fflush(stdout);
    QFile f(filename);
    if ( !f.open( IO_ReadOnly ) )
    {
      QString err = "Unable to read " + filename + ".";
      (LogMgr::getInstance())->addLog(err);
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
ADFMgr::getUnscriptedADF(QString adf)
{
    //printf ("ADFMgr::getUnscriptedADF called with %s\n",adf.latin1());
    if (adf==QString::null) { return QString::null; }
    if (adf.length()<4)     { return QString::null; }

    // check if the file is a script instead a simple txt file (perl or similar)
    int npos = adf.find('\n');
    if (npos<0)
    {
        LogMgr::getInstance()->addLog("Warning, suspicious ADF detected: "+adf);
        return adf;
    }

    QString firstLine = adf.left(npos);
    if ((firstLine==QString::null)||(firstLine.length()<1))
    {
        LogMgr::getInstance()->addLog("Invalid ADF '"+firstLine+"'");
        return QString::null;
    }

    //printf ("first line=%s \n",firstLine.latin1());

    // 1) "#!" Kind of:
    bool c1 = (firstLine[0]==QChar('#') && firstLine[1]==QChar('!'));

    // 2) ":" kind of:
    bool c2 = (firstLine[0]==QChar(':'));

    if (c1 || c2)
    {
        if (c1)
        {
            (LogMgr::getInstance())->addLog("Scripted ADF detected: '"+firstLine+"'");
        }
        else
        {
            (LogMgr::getInstance())->addLog("Scripted (bash-like) ADF detected");
        }
        return scriptResult(adf);
    }
    else
    {
        // if nothing of this, assume is the ADF itself
        return adf;
    }
}

QString
ADFMgr::scriptResult(QString adf)
{
    // Save the contents in a tmp file
    QString session = qApp->sessionId();
    _scriptFilename = session+"_2Dreams_ADF_"+QTime::currentTime().toString();

#ifdef Q_OS_UNIX
    _scriptFilename = _scriptFilename + "_" + QString::number((long)(getpid()));
    _scriptFilename = "/tmp/"+_scriptFilename;
#endif
    QFile f(_scriptFilename);
    if ( !f.open( IO_WriteOnly ) )
    {
        QString err = "Unable to write file " + _scriptFilename + ".";
        (LogMgr::getInstance())->addLog(err);
        return (QString::null);
    }

    QTextStream ostream(&f);
    ostream.setEncoding(QTextStream::Latin1);
    ostream << adf;
    f.flush();
    f.close();

    // Give execution permitions
#ifdef Q_OS_UNIX
    int gok = chmod(_scriptFilename,S_IRUSR|S_IXUSR);
    if (gok<0)
    {
      QString err = "Unable to give execution permission for " + _scriptFilename + ".";
      (LogMgr::getInstance())->addLog(err);
      return (QString::null);
    }
#endif

    // Run it
    _unscriptedFileName = session+"_2Dreams_UNSCRIPTED_ADF_"+QTime::currentTime().toString();
#ifdef Q_OS_UNIX
    _unscriptedFileName = _unscriptedFileName + "_" + QString::number((long)(getpid()));
    _unscriptedFileName = "/tmp/"+_unscriptedFileName;
#endif

    bool ok = runScript();

    // Return the contents of the dumped file
    QString result;
    if (ok)
    {
        result = getADFStringFromFile(_unscriptedFileName);
    }
    else
    {
        result = QString::null;
    }

    // remove tmp files
    deleteTmpFile(_scriptFilename);
    deleteTmpFile(_unscriptedFileName);

    return result;
}

void
ADFMgr::deleteTmpFile(QString file)
{
    QFile f(file);
    bool ok = f.remove();
    if (!ok)
    {
        QString err = "Warning, unable to delete temporary file " + file;
        (LogMgr::getInstance())->addLog(err);
    }
}

QString
ADFMgr::runBinaryADF()
{
    (LogMgr::getInstance())->addLog("Loading External ADF: "+commandLineADF);
    (LogMgr::getInstance())->addLog("Detected Execution flags on ADF, running it...");
    QString session = qApp->sessionId();
    _scriptFilename = commandLineADF;
    _unscriptedFileName = session+"_2Dreams_UNSCRIPTED_ADF_"+QTime::currentTime().toString();
#ifdef Q_OS_UNIX
    _unscriptedFileName = _unscriptedFileName + "_" + QString::number((long)(getpid()));
    _unscriptedFileName = "/tmp/"+_unscriptedFileName;
#endif
    bool ok = runScript();

    QString result;
    if (ok)
    {
        result = getADFStringFromFile(_unscriptedFileName);
    }
    else
    {
        result = QString::null;
    }
    deleteTmpFile(_unscriptedFileName);
    return result;
}

bool
ADFMgr::runScript()
{
    // open up the results file:
    QFile f(_unscriptedFileName);
    if ( !f.open( IO_WriteOnly ) )
    {
        QString err = "Unable to create temporary file " + _unscriptedFileName;
        (LogMgr::getInstance())->addLog(err);
        return false;
    }

    // create the child process
    QProcess* valproc = new QProcess(this);
    Q_ASSERT(valproc!=NULL);
    connect(valproc,SIGNAL(readyReadStdout()),this,SLOT(readyReadStdout()));
    valproc->addArgument(_scriptFilename);
    adf_timeout=false;
    hasStdOut=false;
    QString adfstr = "";
    bool started = valproc->start();
    if (!started)
    {
        QString err = "Unable to launch ADF subprocess " + _scriptFilename;
        (LogMgr::getInstance())->addLog(err);
        delete valproc;
        return false;
    }

    // Aux timer to avoid too long hang-up
    QTimer::singleShot(ADFMGR_ADF_SUBPROCESS_TIMEOUT,this,SLOT(adf_subprocess_timeout()));

    // read the stdout and put it on the defined file (pooling)
    volatile bool keepRunning = valproc->isRunning();
    //printf("reading from stdout...\n");fflush(stdout);
    while (keepRunning && !adf_timeout)
    {
      if (hasStdOut)
      {
          QByteArray ba = valproc->readStdout();
          QString strMsg(ba);
          hasStdOut=false;
          //printf("data read:%s\n",strMsg.latin1());fflush(stdout);
          adfstr+=strMsg;
      }
      keepRunning=valproc->isRunning();
      qApp->processEvents();
    }

    if (hasStdOut)
    {
      QByteArray ba = valproc->readStdout();
      QString strMsg(ba);
      hasStdOut=false;
      //printf("last data read:%s\n",strMsg.latin1());fflush(stdout);
      adfstr+=strMsg;
    }

    // put into the file
    QTextStream ostream(&f);
    ostream.setEncoding(QTextStream::Latin1);
    ostream << adfstr;
    f.flush();

    // check normal completion
    if (!valproc->normalExit() || adf_timeout)
    {
      (LogMgr::getInstance())->addLog("ADF subprocess failed.");
      if (adf_timeout)
      {
          (LogMgr::getInstance())->addLog("Time out!");
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
    if (!valproc->start())
    {
      (LogMgr::getInstance())->addLog("Unable to launch validation process as "+valcmd);
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
          (LogMgr::getInstance())->addLog("DTD validation process failed.");
          if (someStdErr)
          {
              (LogMgr::getInstance())->addLog(errorMsg);
          }
          if (dtd_timeout)
          {
              (LogMgr::getInstance())->addLog("Time out!");
          }
          result=false;
      }
    }
    delete valproc;
    return true;
}

void
ADFMgr::readyReadStderr()
{ hasStdErr=true; }

void
ADFMgr::readyReadStdout()
{ hasStdOut=true; }

void
ADFMgr::dtd_validation_timeout()
{ dtd_timeout = true; }

void
ADFMgr::adf_subprocess_timeout()
{ adf_timeout = true; }




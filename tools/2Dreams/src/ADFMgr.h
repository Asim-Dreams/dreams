// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 

/**
  * @file  ADFMgr.h
  */

#ifndef _ADFMGR_H
#define _ADFMGR_H

// QT Library
#include <qapplication.h>
#include <qfile.h>
#include <qstring.h>
#include <qdom.h>
#include <qregexp.h>
#include <qprocess.h>
#include <qtimer.h>

#include "asim/DralDB.h"

#define ADFMGR_DTD_VALIDATION_TIMEOUT 10000 // 10 secs
#define ADFMGR_ADF_SUBPROCESS_TIMEOUT 15000 // 15 secs

class ADFMgr : QObject
{
    Q_OBJECT
    public:
        static ADFMgr* getInstance ();
        static void destroy();

    protected:
        ADFMgr();
        ~ADFMgr();

    // -----------------------------------------------
    // API
    // -----------------------------------------------
    public:
       // set-up methods
       bool    setCommandLineADF(QString path);
       bool    setDefaultADF(QString path);
       void    setEmbeddedADF(QString adf);
       void    setCheckDTD(QString checkercmd);

       // run method
       bool getADF();
       QDomDocument* getDomDocument();

       // reset before reuse!
       void reset();
    // -----------------------------------------------

    public slots:
        void readyReadStderr();
        void readyReadStdout();
        void dtd_validation_timeout();
        void adf_subprocess_timeout();

    protected:
       QString getADFString();
       QString getADFStringFromFile(QString filename);
       QString getUnscriptedADF(QString adf);
       QString scriptResult(QString adf);
       QString runBinaryADF();
       bool runScript();
       void deleteTmpFile (QString file);
       bool dtdcheck(QString unscriptedadf);

    private:
       QString commandLineADF;
       QString defaultADF;       // set-up in preferences

       // for embedded adf's
       bool    hasEmbeddedADF;
       QString embeddedADFContents;

       // DTD Stuff
       bool dtdCheck;
       QString dtdChecker;

       // for subprocess management
       bool hasStdOut;
       bool hasStdErr;
       bool someStdErr;
       bool dtd_timeout;
       bool adf_timeout;
       QString _scriptFilename;
       QString _unscriptedFileName;

       // the dom representation
       QDomDocument* xmlAdf;

    private:
       static ADFMgr*  _myInstance;
};

#endif


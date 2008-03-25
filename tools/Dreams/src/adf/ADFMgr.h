// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
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

#include "DreamsDefs.h"

// QT includes.
#include <qstring.h>
#include <qobject.h>

// Class forwarding.
class LogMgr;
class QDomDocument;
class TempDir;

// Some defines.
#define ADFMGR_DTD_VALIDATION_TIMEOUT 10000 // 10 secs
#define ADFMGR_ADF_SUBPROCESS_TIMEOUT 15000 // 15 secs

#define ADF_CONTENT "_ADF_CONTENT_"
#define ADF_FILE    "_ADF_FILE_"

class ADFMgr : QObject
{
    Q_OBJECT
    public:
        ADFMgr(LogMgr * _log);
        ~ADFMgr();

    // -----------------------------------------------
    // API
    // -----------------------------------------------
    public:
       bool setCommandLineADF(QString path);
       bool setDefaultADF(QString path);
       void setEmbeddedADF(QString adf);
       void setCheckDTD(QString checkercmd);

       bool getADF();
       QDomDocument * getDomDocument() const;

    public slots:
        void readyReadStderr();
        void readyReadStdout();
        void dtd_validation_timeout();
        void adf_subprocess_timeout();

    protected:
       QString getADFString();
       QString getADFStringFromFile(QString filename);
       QString getADFContent(QString adf);
       QString scriptResult(QString adf);
       bool runScript();
       bool dtdcheck(QString unscriptedadf);

    private:
       QString commandLineADF;
       QString defaultADF;
       QString adf_content_file;
       LogMgr* log;

       // for embedded adf's
       bool    hasEmbeddedADF;
       QString embeddedADFContents;

       // DTD stuff.
       bool dtdCheck;
       QString dtdChecker;

       // Subprocess management.
       bool hasStdOut;
       bool hasStdErr;
       bool writeStdin;
       bool someStdErr;
       bool dtd_timeout;
       bool adf_timeout;

       // Dom representation.
       QDomDocument* xmlAdf;

       TempDir * temp_dir; ///< Temporal directory manager pointer.
};

#endif

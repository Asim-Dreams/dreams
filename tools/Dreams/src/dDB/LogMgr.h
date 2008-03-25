// ==================================================
// Copyright (C) 2004-2006 Intel Corporation
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
  * @file LogMgr.h
  */

#ifndef _DRALDB_LOGMGR_H
#define _DRALDB_LOGMGR_H

// Qt includes.
#include <qstring.h>
#include <qdatetime.h>

// Class forwarding.
class QFile;

/**
  * @brief
  * This class conains the log of the database.
  *
  * @description
  * This class is used to store a global log of an application that
  * uses this database. Typical file access methods are defined to
  * allow an ease use of this class.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class LogMgr
{
    public:
        static LogMgr* getInstance();
        static LogMgr* getInstance(QString filename);
        static void destroy();

    public:
        void addLog(QString msg);
        void flush();
        void clearLogFile();
        QTime getLastTimestamp();
        bool changeFileName(QString newfilename);

    protected:
        LogMgr(QString filename);
        ~LogMgr();

    private:
        QString  fileName; // Name of the log file.
        QFile*   fileObject; // Pointer to the file descriptor.
        bool ok; // 
        QTime lastTimestamp; // Last time a comment was inserted.

    private:
        static LogMgr* _myInstance;
};

#endif

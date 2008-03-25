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
  * @file TempDir.h
  */

#ifndef _TEMP_DIR_H
#define _TEMP_DIR_H

#include "DreamsDefs.h"
#include <qdir.h>
#include <qstring.h>

#include "dDB/DralDBSyntax.h"
#include "dDB/LogMgr.h"

/**
  * @brief
  * Class to manage a temporal directory.
  *
  * @description
  * This singleton class provides to the other classes functions
  * to create a temporal directory and set it as the working dir
  * (and to return to the original program directory).
  *
  * @author Guillem Sole
  * @date started at 2004-06-16
  * @version 0.1
  */
class TempDir
{
    public:
        static TempDir * getInstance();
        static void destroy();

        void create(QString unique_str);
        QString getTempDir();
        QString getProgDir();
        bool setTempWorkDir();
        bool setProgWorkDir();
        bool createTempFile(QString file);
        bool openTempFile(QString file, QFile * f, INT32 mode);
        bool copyTempFile(QString dest, QFile src);
        bool copyTempString(QString dest, QString content);
        bool copyTempBuffer(QString dest, char * buffer, UINT32 length);
        bool deleteTempFile(QString file);
        void removeTempDir(QDir dir);

    protected:
        // They are protected because is a singleton class.
        TempDir();
        ~TempDir();

    private:
        QString temp_dir; ///< Contains the path of the temporal directory created.
        QString prog_dir; ///< Contains the path of the working directory of the program.
        QDir directory;   ///< Object used to create and destroy the directory.
        LogMgr * log;     ///< Pointer to the log manager.

        static TempDir * _myInstance; ///< Unique TempDir instance.
} ;

#endif

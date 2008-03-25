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
  * @file TempDir.cpp
  */

// General includes.
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "TempDir.h"

// Qt includes.
#include <qdatetime.h>
#include <qstringlist.h>

/**
 * The instance is NULL at the beginning.
 */
TempDir * TempDir::_myInstance = NULL;

/**
 * Returns the instance of the class. The first time this function
 * is called is when the instance is created. The other times just
 * returns the pointer.
 *
 * @return the instance of the class.
 */
TempDir *
TempDir::getInstance()
{
    if(_myInstance == NULL)
    {
        _myInstance = new TempDir();
    }
    return _myInstance;
}

/**
 * Destroys the unique instance of the class. The instance is
 * destroyed if it was previously created.
 *
 * @return void.
 */
void
TempDir::destroy()
{
    if(_myInstance != NULL)
    {
        delete _myInstance;
        _myInstance = NULL;
    }
}

/**
 * Cretor of the class.
 *
 * @return new object.
 */
TempDir::TempDir()
{
    temp_dir = QString::null;
    prog_dir = QString::null;
    log = LogMgr::getInstance();
}

/**
 * Destructor of the class.
 *
 * @return destroys the object.
 */
TempDir::~TempDir()
{
    // If a temp directory was created destroys it.
    if(temp_dir != QString::null)
    {
        removeTempDir(QDir(temp_dir));
    }
}

/**
 * Creates the temporal directory for the program using the unique
 * id.
 *
 * @return void.
 */
void
TempDir::create(QString unique_str)
{
    UINT32 act_size; ///< Incremental size of cwd.
    char * cwd;      ///< Stores the cwd directory in char * format.
    char * temp;     ///< Stores the temp directory in char * format.
    bool ok;         ///< Used to verify if the creation of the directory successes.

    act_size = 1024;
    cwd = (char *) malloc(act_size);
    // Loop done to hadle with large directory names.
    while(getcwd(cwd, act_size) == NULL)
    {
        free(cwd);
        act_size += 1024;
        cwd = (char *) malloc(act_size);
    }

    prog_dir = QString(cwd);

    // Finishes the path with /.
    if(prog_dir[prog_dir.length() - 1] != QChar('/'))
    {
        prog_dir += '/';
    }

    temp = getenv("TEMP");
    if(temp == NULL)
    {
        temp = "/tmp/";
    }

    temp_dir = QString(temp);

    // Finishes the path with /.
    if(temp_dir[temp_dir.length() - 1] != QChar('/'))
    {
        temp_dir += '/';
    }
    temp_dir += "._" + unique_str + "_Dreams_temp_dir_" + QTime::currentTime().toString() + "_" + QString::number(getpid()) + "/";
    ok = directory.mkdir(temp_dir);
    if(!ok)
    {
        printf("Can't create the temp directory: %s\n", temp_dir.latin1());
        printf("Try to change the TEMP environment variable.\n");
        exit(-1);
    }
}

/**
 * Returns the path of the temporal directory.
 *
 * @return temp directory.
 */
QString
TempDir::getTempDir()
{
    Q_ASSERT(temp_dir != QString::null);
    return temp_dir;
}

/**
 * Returns the path of the program directory.
 *
 * @return program directory.
 */
QString
TempDir::getProgDir()
{
    Q_ASSERT(prog_dir != QString::null);
    return prog_dir;
}

/**
 * Sets the working directory of the program to the temp directory.
 *
 * @return true if done successfully.
 */
bool
TempDir::setTempWorkDir()
{
    Q_ASSERT(temp_dir != QString::null);
    if(QDir::setCurrent(temp_dir))
    {
        return true;
    }
    else
    {
        QString err = "Unable to change to temp directory " + temp_dir;
        log->addLog(err);
        return false;
    }
}

/**
 * Sets the working directory of the program to the original program
 * directory.
 *
 * @return true if done successfully.
 */
bool
TempDir::setProgWorkDir()
{
    Q_ASSERT(prog_dir != QString::null);
    if(QDir::setCurrent(prog_dir))
    {
        return true;
    }
    else
    {
        QString err = "Unable to change to program directory " + prog_dir;
        log->addLog(err);
        return false;
    }
}

/**
 * Creates the file with name file in the temp directory.
 *
 * @return a QFile object pointing to the file.
 */
bool
TempDir::createTempFile(QString file)
{
    QFile f(temp_dir + file);
    if(!f.open(IO_ReadWrite))
    {
        QString err = "Unable to create temporary file " + file;
        log->addLog(err);
        return false;
    }
    f.close();
    return true;
}

/**
 * Creates the file with name file in the temp directory.
 *
 * @return a QFile object pointing to the file.
 */
bool
TempDir::openTempFile(QString file, QFile * f, INT32 mode)
{
    f->setName(temp_dir + file);
    if(!f->open(mode))
    {
        QString err = "Unable to open temporary file " + file;
        log->addLog(err);
        return false;
    }
    return true;
}

/**
 * Copies the content of the file src to the temporal file dest.
 *
 * @return true if copied correctly.
 */
bool
TempDir::copyTempFile(QString dest, QFile src)
{
    QByteArray ba(4096);
    QFile f(temp_dir + dest);

    // Opens the temporal file.
    if(!f.open(IO_WriteOnly))
    {
        QString err = "Unable to open temporary file " + dest;
        log->addLog(err);
        return false;
    }

    // Copies all the content.
    while(src.readBlock(ba.data(), 4096) > 0)
    {
        if(f.writeBlock(ba) == -1)
        {
            QString err = "Error writing copying data to temporal file";
            log->addLog(err);
            return false;
        }
    }
    f.flush();
    f.close();
    return true;
}

/**
 * Copies the content of QString content to the temporal file
 * dest.
 *
 * @return true if copied correctly.
 */
bool
TempDir::copyTempString(QString dest, QString content)
{
    QFile f(temp_dir + dest);

    // Opens the temporal file.
    if(!f.open(IO_WriteOnly))
    {
        QString err = "Unable to open temporary file " + dest;
        log->addLog(err);
        return false;
    }

    // Copies the string.
    if(f.writeBlock(content.latin1(), content.length()) == -1)
    {
        QString err = "Error writing copying data to temporal file";
        log->addLog(err);
        return false;
    }
    f.flush();
    f.close();
    return true;
}

/**
 * Copies the content of QString content to the temporal file
 * dest.
 *
 * @return true if copied correctly.
 */
bool
TempDir::copyTempBuffer(QString dest, char * buffer, UINT32 length)
{
    QFile f(temp_dir + dest);

    // Opens the temporal file.
    if(!f.open(IO_WriteOnly))
    {
        QString err = "Unable to open temporary file " + dest;
        log->addLog(err);
        return false;
    }

    // Copies the buffer.
    if(f.writeBlock(buffer, length) == -1)
    {
        QString err = "Error writing copying data to temporal file";
        log->addLog(err);
        return false;
    }
    f.flush();
    f.close();
    return true;
}

/**
 * Deletes the file with name file of the temp directory.
 *
 * @return true if removed correctly.
 */
bool
TempDir::deleteTempFile(QString file)
{
    QFile f(temp_dir + file);

    if(!f.remove())
    {
        QString err = "Unable to delete temporary file " + file;
        log->addLog(err);
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * Deletes the directory path and all its files and subdirectories.
 *
 * @return if the directory was removed succesfully.
 */
void
TempDir::removeTempDir(QDir dir)
{
    QDir temp;                ///< Temporal directory.
    QStringList l;            ///< List of files or directories inside the actual directory.
    QStringList::Iterator it; ///< Iterator to run through the string list.
    bool ok;                  ///< Stores the result of removing a file or directory.

    // Deletes all the subdirectories.
    l = dir.entryList(QDir::Dirs);
    for(it = l.begin(); it != l.end(); ++it )
    {
        // Skips recursive removings.
        if((* it != ".") && (* it != ".."))
        {
            temp = dir;
            // Goes to the folder.
            ok = temp.cd(* it);
            if(!ok)
            {
                QString err = "Warning!!! Folder: " + temp.absPath() + " couldn't be removed";
                log->addLog(err);
            }
            else
            {
                removeTempDir(temp);
            }
        }
    }

    // Deletes all the files.
    QDir::setCurrent(dir.absPath());
    l = dir.entryList(QDir::Files);
    for(it = l.begin(); it != l.end(); ++it )
    {
        QFile f((* it));
        ok = f.remove();
        if(!ok)
        {
            QString err = "Warning!!! File: " + * it + " couldn't be removed";
            log->addLog(err);
        }
    }

    // Deletes itself.
    ok = dir.rmdir(dir.absPath());

    if(!ok)
    {
        QString err = "Warning!!! Temp directory: " + dir.absPath() + " couldn't be removed";
        log->addLog(err);
    }
}

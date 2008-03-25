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
  * @file StrTable.h
  */

#ifndef _DRALDB_STRTABLE_H
#define _DRALDB_STRTABLE_H

// General includes.
#include <stdio.h>
#include <stdlib.h>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDBDefinitions.h"
#include "dDB/DRALTag.h"
#include "dDB/aux/AMemObj.h"
#include "dDB/aux/StatObj.h"

// Qt includes.
#include <qstring.h>
#include <qdict.h>
#include <qintdict.h>

/**
  * @brief
  * This class holds a mapping between strings and integers.
  *
  * @description
  * We keep a huge amount of symbols (string) and this table avoid
  * repeated strings to spend memory space.
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */

class StrTable : public AMemObj, public StatObj
{
    public:
        StrTable(INT32 sz = STRTBL_DEFAULT_SIZE);
        virtual ~StrTable();

        // ---- AMemObj Interface methods
        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;
        // ---- StatObj Interface methods
        QString getStats() const;
        // -----------------------------------------------

    public:
        inline UINT32 addString(QString str);
        inline QString getString(UINT32 id);
        inline bool hasString(QString str);

        void resize(INT32 newsize);

    protected:
        void init(INT32 sz);

        inline UINT32 lookForStr(QString);

    private:
        QDict<UINT32> *     strhash; // Hash from string to int.
        QIntDict<QString> * idxhash; // Hash from int to string.

        INT32 nextIdx;
        INT32 primeSize;
} ;

/**
 * Returns the string mapped to the integer idx. If no string is
 * mapped to this integer a null is returned.
 *
 * @return the string.
 */
QString
StrTable::getString(UINT32 id)
{
    QString * str = idxhash->find((long) id);

    if(str == NULL)
    {
        return QString::null;
    }
    else
    {
        return * str;
    }
}

/**
 * Adds a string to the table and returns the index mapped to the
 * string.
 *
 * @return the indexed mapped to the string.
 */
UINT32
StrTable::addString(QString str)
{
    // look for the string to avoid redundant entries
    UINT32 * idx_ptr = strhash->find(str);

    if(idx_ptr == NULL) // not found
    {
        idx_ptr = new UINT32;
        Q_ASSERT(idx_ptr != NULL);
        * idx_ptr = nextIdx;
        strhash->insert(str, idx_ptr);
        QString * tmpstr = new QString(str);
        Q_ASSERT(tmpstr != NULL);
        idxhash->insert((long) nextIdx, tmpstr);
        nextIdx++;
    }
    return * idx_ptr;
}

/**
 * Looks for the integer mapped to the string str.
 *
 * @return the index mapped to the string.
 */
UINT32
StrTable::lookForStr(QString str)
{
   UINT32 * idx_ptr = strhash->find(str);
   if(idx_ptr == NULL)
   {
       return (UINT32) -1;
   }
   else
   {
       return * idx_ptr;
   }
}

/**
 * Looks if the string is already in the table.
 *
 * @return true if the index is in the table. False otherwise.
 */
bool
StrTable::hasString(QString str)
{
    return (lookForStr(str) != (UINT32) -1);
}

#endif

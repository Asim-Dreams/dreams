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
  * @file ColDescVector.cpp
  */
#include "ColDescVector.h"

#include <qprogressdialog.h>
#include <qapplication.h>

LogMgr* ColDescVector::log = NULL;

ColDescVector::ColDescVector()
{
    aecolvector = new ColDescAEVector();
    Q_ASSERT(aecolvector!=NULL);
    nextEntry = 0;
    log = LogMgr::getInstance();
}

ColDescVector::~ColDescVector()
{
    if (aecolvector!=NULL) delete aecolvector;
}

void
ColDescVector::reset()
{
    delete aecolvector;
    aecolvector = new ColDescAEVector();
    nextEntry=0;
}

INT64
ColDescVector::getObjSize() const
{
    INT64 mySize = sizeof(ColDescVector);
    INT64 colarray = sizeof(ColDescriptor) *
          aecolvector->getNumSegments() * aecolvector->getSegmentSize();

    // for each col descriptor ask for its size
    INT64 acum = 0;
    QProgressDialog progress( "Scanning ColDescriptors...", "&Abort",
                    (aecolvector->getNumSegments() * aecolvector->getSegmentSize()) ,NULL,
                    "progress",TRUE);

    progress.setMinimumDuration(1000);
    for (INT32 i=0;i<(aecolvector->getNumSegments()*aecolvector->getSegmentSize());i++)
    {
    	if (i%1024==0)
    	{
            progress.setProgress(i);
            qApp->processEvents();
            if ( progress.wasCancelled() )
            {
            	LogMgr* log = LogMgr::getInstance();
            	log->addLog("Scanning ColDescriptors cancelled y user!");
            	break;
            }
        }
        if (aecolvector->hasElement(i))
           acum += (*aecolvector)[i].getObjSize();

    }
    progress.setProgress(aecolvector->getNumSegments() * aecolvector->getSegmentSize());
    return (acum+mySize+colarray);
}

QString
ColDescVector::getUsageDescription() const
{ return NULL; }

QString
ColDescVector::getStats() const
{
    QString result = "\n";
    return (result);
}


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
  * @file  ADFParser1.h
  */

#ifndef _ADFPARSER2_H
#define _ADFPARSER2_H

#include "asim/DralDB.h"

// 2Dreams
#include "ADFParser1.h"

/**
  * This class holds DRAL Graph and configuration information.
  *
  * @author Federico Ardanaz
  * @date started at 2002-09-05
  * @version 0.1
  */
class ADFParser2 : public ADFParser1
{
    public:
        /**
          * Function description
          */
        ADFParser2(QDomDocument* ref);
        virtual ~ADFParser2();
        // -----------------------------------------------

    protected:
        virtual bool processDomNode         (QDomNode node,INT32* dfsNum);

        virtual bool processNodeStatement   (QDomNode node,INT32* dfsNum);
        virtual bool processNodeSubTags     (QDomNode node,INT32* dfsNum, INT16 row=-1);

        virtual bool processGroupStatement  (QDomNode node,INT32* dfsNum);
        virtual bool processShowStatement   (QDomNode node,INT32* dfsNum, TabSpec* tab);
        virtual bool processEnterNodeStatement (QDomNode node,INT32* dfsNum);
        virtual bool processExitNodeStatement (QDomNode node,INT32* dfsNum);

        virtual bool processSetStatement(QDomNode node,INT32* dfsNum, INT16 row=-1);

        virtual bool processIfStatement     (QDomNode node,INT32* dfsNum, INT16 row=-1,TabSpec* tab=NULL);
        virtual bool processSetInsideIfStatement(QDomElement eset,SetInsideIfData* siid);

        virtual bool processShadeTableStatement (QDomNode node,INT32* dfsNum);
        virtual bool processShadeSubTags        (QDomNode node,INT32 dfsNum,ShadeRuleTbl* shrtbl, bool& hasZero, bool& has100);
        virtual bool processShadingStatement    (QDomNode node,INT32* dfsNum, INT16 row);

        virtual UINT16 getNodeIdFromRow     (INT32 row);
};

#endif


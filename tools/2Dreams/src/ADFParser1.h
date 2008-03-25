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

#ifndef _ADFPARSER1_H
#define _ADFPARSER1_H

// QT library
#include <qdom.h>

#include "asim/DralDB.h"

// 2Dreams
#include "ADFParserInterface.h"
#include "ADFDefinitions.h"
#include "RowRules.h"
#include "TabSpec.h"
#include "TagValueType.h"
#include "DralGraphNode.h"
#include "DralGraphEdge.h"
#include "ConfigDB.h"
#include "ZDBase.h"

/**
  * This class holds DRAL Graph and configuration information.
  *
  * @author Federico Ardanaz
  * @date started at 2002-09-05
  * @version 0.1
  */
class ADFParser1 : public ADFParserInterface
{
    public:
        // ---- ADFParserInteface methods

        /**
          * Function description
          */
        ADFParser1(QDomDocument* ref);
        virtual ~ADFParser1();

        /**
          * Function description
          */
        virtual bool parseADF();

        // -----------------------------------------------


    protected:
        /**
          * Function description
          */
        virtual bool processDomNode        (QDomNode node,INT32* dfsNum);
        virtual bool processEdgeStatement  (QDomNode node,INT32* dfsNum);
        virtual bool processNodeStatement  (QDomNode node,INT32* dfsNum);
        virtual bool processBlankStatement (QDomNode node,INT32* dfsNum);
        virtual bool processTagDescriptorStatement (QDomNode node,INT32* dfsNum);

        virtual bool processShadeTableStatement (QDomNode node,INT32* dfsNum, DralGraphNode* gnode=NULL);
        virtual bool processSetStatement   (QDomNode node,INT32* dfsNum, INT16 row=-1);

        virtual bool processIfStatement    (QDomNode node,INT32* dfsNum, INT16 row=-1,TabSpec* tab=NULL);
        virtual bool processItemFontInsideIfStatement(QDomElement eset,ItemFontInsideIfData* ifiid);
        virtual bool processSetInsideIfStatement(QDomElement eset,SetInsideIfData* siid);
        virtual void applySetInsideIfRules(SetInsideIfData* siid, INT16 row);

        virtual bool processItemWindowStatement (QDomNode node,INT32* dfsNum);
        virtual bool processItemWindowSubTags(QDomNode node,INT32* dfsNum);
        virtual bool processTabStatement(QDomNode node,INT32* dfsNum);
        virtual bool processTabSubTags (QDomNode node,INT32* dfsNum, TabSpec* tab);
        virtual bool processItemFontStatement (QDomNode node,INT32* dfsNum, TabSpec* tab=NULL);
        virtual bool processShowStatement (QDomNode node,INT32* dfsNum, TabSpec* tab);

        virtual bool processEdgeSubTags(QDomNode node,INT32* dfsNum, INT16 row=-1);
        virtual bool processNodeSubTags(QDomNode node,INT32* dfsNum, DralGraphNode* gnode=NULL);
        virtual bool processShadeSubTags (QDomNode node,INT32 dfsNum, ShadeRuleTbl* shrtbl,bool& hasZero, bool& has100);

    protected:
        DralDB*   draldb;
        LogMgr*   myLogMgr;
        ConfigDB* cdb;
        ZDBase*   myZDB;

};

#endif


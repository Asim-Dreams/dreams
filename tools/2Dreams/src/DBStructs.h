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
  * @file  DBStructs.h
  */

#ifndef _DBSTRUCTS_H
#define _DBSTRUCTS_H

// QT Library
#include <qstring.h>
#include <qintdict.h>
#include <qptrlist.h>
#include <qvaluelist.h>
#include <qvaluevector.h>

#include "asim/DralDB.h"

// for delete lists...
typedef AEVector<UINT32,16384,64> DeleteItemList2D; // max 1M deleteitems at once...


// for node-row events
typedef struct
{
    UINT16     row;
    QColor     color;
    EventShape shape;
} NRComputedEvent;

typedef AEVector<NRComputedEvent,4096,16> NRCEList; // max 64k events per cycle...

// for enter/exit node commands
//typedef FValueVector<UINT32> EnterNodeList2D;
//typedef FValueVector<UINT32> ExitNodeList2D;


/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class LMoveItemListNode2D
{
    public:
       LMoveItemListNode2D() {edge_id=0;position=0;cycle=0;};
       LMoveItemListNode2D(UINT32 cyc, UINT16 p_edge_id,UINT16 pos)
       {
           edge_id  = p_edge_id;
           position = pos;
           cycle = cyc;
       };

    public: // this is performance critical so we make them public
       UINT16 edge_id;
       UINT16 position;
       UINT32 cycle;
};

/** @typdef MoveItemList
  * desc.
  */
typedef AEVector<LMoveItemListNode2D,128,128> MoveItemList2D; // max 16k movements per item

/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class LNewItemListNode2D
{
    public:
       LNewItemListNode2D(UINT32 cyc, UINT32 iid)
       {
           myMovs = NULL;
           item_id = iid;
           cycle = cyc;
       }

       ~LNewItemListNode2D()
       {
           if (myMovs!=NULL) delete myMovs;
       }

    public:
       bool hasMovs() { return myMovs !=NULL; }

       MoveItemList2D* getMyMovs()
       {
           if (myMovs==NULL) myMovs = new MoveItemList2D();
           return myMovs;
       }

    public: // this is performance critical so we make them public
       UINT32   item_id;
       UINT32   cycle;

    protected:
       MoveItemList2D* myMovs;

    private:
        static UINT64 _tagListCnt;
        static UINT64 _tagListAcum;
        static UINT64 _movListCnt;
        static UINT64 _movListAcum;
};

/** @typdef NewItemList
  * desc.
  */
typedef QIntDict<LNewItemListNode2D> NewItemList2D;

class AuxItemListNode2D
{
    public:
       AuxItemListNode2D() { cycle =0; item_id=0; }
       AuxItemListNode2D(UINT32 cyc, UINT32 iid)
       {
           item_id = iid;
           cycle = cyc;
       }

    public: // this is performance critical so we make them public
       UINT32   item_id;
       UINT32   cycle;
};

class AuxItemList2D : public QPtrList<AuxItemListNode2D>
{
   public:
        AuxItemList2D() : QPtrList<AuxItemListNode2D>()
        {setAutoDelete(true);}

    protected:
        inline int compareItems ( QPtrCollection::Item item1, QPtrCollection::Item item2 );
};

int
AuxItemList2D::compareItems ( QPtrCollection::Item item1, QPtrCollection::Item item2 )
{
    AuxItemListNode2D* node1 = (AuxItemListNode2D*)item1;
    AuxItemListNode2D* node2 = (AuxItemListNode2D*)item2;
    return node1->cycle - node2->cycle;
}


#endif



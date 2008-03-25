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
    @file   node.h
    @author Federico Ardanaz
 */

#ifndef _VCGDREAMS_NODE_H
#define _VCGDREAMS_NODE_H

#include <string>
#include <map>
#include <iostream>
#include <assert.h>
#include <asim/syntax.h>
#include "vcgNode.h"

using namespace std;

class NODE_REGISTRY
{
  public:
      NODE_REGISTRY()
      {
          nearCnt=0;
          valid  =0;
          treeVLevel=0;
          nodeT=NULL;
          nodeC=NULL;
      };
      
      string nodeName;
      
      UINT16 parent;
      UINT16 id;
      UINT16 treeVLevel;

      // reference to the vcg objects
      vcgNode* nodeT;
      vcgNode* nodeC;

      // flags
      UINT32 valid   : 1;
      UINT32 nearCnt : 2;
};

typedef map<UINT16,NODE_REGISTRY> NODE_MAP_ID;
typedef map<string,NODE_REGISTRY> NODE_MAP_NAME;

class NodeDB
{
  public:
      static  NodeDB* getInstance();
      string  getNode(UINT16 id);
      void    addNode(UINT16 id,string name,UINT16 parent, vcgNode* nodeT,vcgNode* nodeC);
      NODE_REGISTRY getNodeRegisrty(UINT16); 
      NODE_MAP_ID::iterator findNode(UINT16);
      NODE_MAP_ID::iterator getFirstEntry();
      NODE_MAP_ID::iterator getEndmark();
                      
  private:
      NodeDB();
      ~NodeDB();
      static NodeDB* _instance;
      NODE_MAP_ID   nidmap; 
      //NODE_MAP_NAME nnmmap; 
};

#endif




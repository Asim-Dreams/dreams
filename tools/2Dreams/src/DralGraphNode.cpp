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
  * @file DralGraphNode.cpp
  */

#include "DralGraphNode.h"

DralGraphNode::DralGraphNode (QString _name,UINT16 _node_id,UINT16 _parent_id,UINT16 _instance)
{
    this->name = _name;
    this->node_id = _node_id;
    this->parent_id = _parent_id;
    this->instance = _instance;
    this->capacity = 0;
    this->isConfigured=false;

    isConfigured=false;

    rowlist = new RRList();
    rowlist->setAutoDelete(true);
}

DralGraphNode::~DralGraphNode()
{
    delete rowlist;
}


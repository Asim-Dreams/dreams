/*****************************************************************************
 *
 * @author Oscar Rosell
 *
 * Copyright (C) 2006 Intel Corporation
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
 *****************************************************************************/
#ifndef SELECTOR_FACTORY_H
#define SELECTOR_FACTORY_H

#include "selector.h"
#include <Singleton.h>
#include <string>
#include <map>
#include <iostream>

class SELECTOR_FACTORY
{
  public:
    void 
    Register(std::string id, const SELECTOR& selector, std::string info)
    {
        id_to_selector_[id] = selector.Clone();
        id_to_info_[id] = info;
    }
    SELECTOR*
    CreateObject(std::string id)
    {
        if (id_to_selector_.find(id) == id_to_selector_.end())
        {
            return NULL;
        }
        return id_to_selector_[id]->Clone();
    }
    void
    DumpInfo(std::string prefix) const
    {
        for (std::map<std::string, std::string>::const_iterator it = id_to_info_.begin(); it != id_to_info_.end(); ++it)
        {
            std::cout << prefix << it->first << " -- " << it->second << std::endl;
        }
    }

  private:  
    std::map<std::string, SELECTOR*> id_to_selector_;
    std::map<std::string, std::string> id_to_info_;
};

typedef Loki::SingletonHolder<SELECTOR_FACTORY> THE_SELECTOR_FACTORY;

#endif

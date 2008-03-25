
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

#include <Singleton.h>
#include <string>
#include <deque>
#include <map>
#include "node_db.h"

class CONFIGURATION_DB
{
  public:
    bool parseXML(std::string filename);

    std::string getParameter(std::string name) const;
    bool isIgnored(std::string name) const;

  private:
    std::deque<std::string> ignoredNodes_;
    std::deque<std::string> shownTags_;
    std::map<std::string, std::string> parameters_;
};

typedef Loki::SingletonHolder<CONFIGURATION_DB> THE_CONFIGURATION_DB;

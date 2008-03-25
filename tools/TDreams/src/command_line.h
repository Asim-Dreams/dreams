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
#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <Singleton.h>
#include <queue>
#include <string>

class COMMAND_LINE_PARSER
{
  public:
    COMMAND_LINE_PARSER();
    ~COMMAND_LINE_PARSER();

    std::string readLine();
};

typedef Loki::SingletonHolder<COMMAND_LINE_PARSER> THE_COMMAND_LINE_PARSER;

#endif

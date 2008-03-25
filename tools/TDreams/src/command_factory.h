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
#ifndef COMMAND_FACTORY_H
#define COMMAND_FACTORY_H

#include "commands.h"
#include <Singleton.h>
#include <Factory.h>
#include <string>

typedef Loki::Factory<COMMAND, std::string> COMMAND_FACTORY;
typedef Loki::SingletonHolder<COMMAND_FACTORY> THE_COMMAND_FACTORY;

typedef Loki::Factory<SET_COMMAND, std::string> SET_COMMAND_FACTORY;
typedef Loki::SingletonHolder<SET_COMMAND_FACTORY> THE_SET_COMMAND_FACTORY;

#endif

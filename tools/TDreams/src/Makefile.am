#
#  Copyright (C) 2006 Intel Corporation
#  
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#  
#

bin_PROGRAMS= tdreams

tdreams_LDFLAGS = $(ASIMCORE_LFLAGS) -lm -lxml2 -lreadline -lncurses -lz

tdreams_SOURCES = main.cpp listener.cpp selector.cpp node_db.cpp lifetime.cpp commands.cpp command_line.cpp configuration.cpp svg_drawer.cpp dumpers.cpp item_tags.cpp

tdreams_HFILES = command_factory.h command_line.h commands.h comparators.h configuration.h dral_event_types.h dumpers.h endpoint.h extractors.h item_tags.h lifetime.h listener.h node_db.h parameter_dictionary.h selector.h selector_factory.h svg_drawer.h time_object.h

WARNFLAGS = -W -Wall -Wno-unused

AM_CXXFLAGS= $(WARNFLAGS) $(OPTFLAGS) $(COREFLAGS) $(COREINCS) $(ASIMCORE_CFLAGS) $(TDREAMS_CFLAGS)
AM_LDFLAGS = $(CORELIBS) $(ASIMCORE_LFLAGS) 


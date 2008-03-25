#
#  Copyright (C) 2005-2006 Intel Corporation
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

SUBDIRS=vcgen
bin_PROGRAMS= vcgdreams
vcgdreams_LDFLAGS = $(X_PRE_LIBS) $(X_LIBS) $(X_EXTRA_LIBS) -lm -lz

AM_YFLAGS = -d

vcgdreams_SOURCES = vcgDreams.cpp \
			vcgdListener.cpp \
			vcgdNode.cpp 

vcgdreams_HFILES = vcgDreams.h \
			vcgdListener.h \
			vcgdNode.h 

# Note: QT .h files produce tons of warning if -Wall is present
# so we redefine the warning flags to an intermediate level:
WARNFLAGS = -ansi -pedantic -W -Wall -Wcast-align -Wformat=2 -Wconversion -Wno-unused -Wno-parentheses -Wno-long-long
AM_CXXFLAGS += -Ivcgen/src -Lvcgen/src -lvcgen  


#
# Copyright (C) 2003-2006 Intel Corporation
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
# 
#

bin_PROGRAMS= dbtest

dbtest_LDFLAGS = -L../../../lib/libdraldb -ldraldb $(ASIMCORE_LFLAGS) $(QTLIBDIR) $(QTLIBOBJ) -lm -lz

dbtest_SOURCES =   dbtest.cpp 
 
WARNFLAGS = -ansi -pedantic -W -Wall -Wcast-align -Wformat=2 -Wconversion -Winline -Wno-unused -Wno-parentheses -Wno-long-long
AM_CXXFLAGS += $(QTINCDIR) -I../../../lib/libdraldb/include


# maybe explicitly link this, avoiding use of broken libtool:
if X86_64_LIBTOOL_HACK
dbtest: $(dbtest_OBJECTS)
	$(CXXLD) -o dbtest $(dbtest_OBJECTS) $(dbtest_LDFLAGS)
endif

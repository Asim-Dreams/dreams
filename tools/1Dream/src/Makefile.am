#
#  Copyright (C) 2003-2006 Intel Corporation
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

bin_PROGRAMS= 1Dream 

1Dream_LDFLAGS =	$(QTLIBDIR) $(QTLIBOBJ) $(X_PRE_LIBS) $(X_LIBS) $(X_EXTRA_LIBS) $(ASIMCORE_LFLAGS) -lm


# ---------------------------------------------------------------------------
# WARNING!! This is a patch and must be fixed
# Stripchart viewer has problems right now if compiled in optimized mode so
# we enforce debugging
# ---------------------------------------------------------------------------

OPTFLAGS = -g -O0 -DQT_DEBUG


1Dream_SOURCES = 	Main.cpp \
			LittleSelector.cpp \
			Ruler.cpp \
			colors.cpp \
			LittleSelectorImpl.cpp \
			Selector.cpp \
			error.cpp \
			ColorsImpl.cpp \
			Properties.cpp \
			SelectorImpl.cpp \
			PropertiesImpl.cpp \
			StripChartView.cpp



nodist_1Dream_SOURCES = Main_moc.cpp \
			LittleSelector_moc.cpp \
			Ruler_moc.cpp \
			colors_moc.cpp \
			LittleSelectorImpl_moc.cpp \
			Selector_moc.cpp \
			error_moc.cpp \
			ColorsImpl_moc.cpp \
			Properties_moc.cpp \
			SelectorImpl_moc.cpp \
			PropertiesImpl_moc.cpp \
			StripChartView_moc.cpp 

EXTRA_DIST =	xpm \
		ColorsImpl.h \
		LittleSelector.h \
		LittleSelector.ui \
		LittleSelectorImpl.h \
		Main.h \
		Properties.h \
		Properties.ui \
		PropertiesImpl.h \
		Ruler.h \
		Selector.h \
		Selector.ui \
		SelectorImpl.h \
		StripChartView.h \
		StripEntry.h \
		colors.h \
		colors.ui \
		error.h \
		error.ui


%_moc.cpp: %.h %.cpp
	$(QTMOCDIR)/moc $< -o $@ 

# Note: QT .h files produce tons of warning if -Wall is present
# so we redefine the warning flags to an intermediate level:
WARNFLAGS = -ansi -pedantic -W -Wall -Wcast-align -Wno-unused -Wno-parentheses -Wno-long-long
AM_CXXFLAGS += $(QTINCDIR)

CLEANFILES = *moc*

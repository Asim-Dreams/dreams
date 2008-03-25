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

lib_LIBRARIES = libvcgen.a

libvcgen_a_SOURCES =  vcgArrowModeAttr.cpp \
                        vcgArrowStyleAttr.cpp \
                        vcgBackEdge.cpp \
                        vcgBentNearEdge.cpp \
                        vcgColorAttr.cpp \
                        vcgCrossWeightAttr.cpp \
                        vcgEdge.cpp \
                        vcgEdgePrefix.cpp \
                        vcgGraph.cpp \
                        vcgLayoutAlgorithmAttr.cpp \
                        vcgLineStyleAttr.cpp \
                        vcgNearEdge.cpp \
                        vcgNode.cpp \
                        vcgNodeAlignAttr.cpp \
                        vcgNodePrefix.cpp \
                        vcgOrientationAttr.cpp \
                        vcgShapeAttr.cpp \
                        vcgStatusAttr.cpp \
                        vcgTextModeAttr.cpp \
                        vcgViewAttr.cpp \
                        vcgYesNoAttr.cpp 

libvcgen_a_HFILES =  List.h \
                        vcgArrowModeAttr.h \
                        vcgArrowStyleAttr.h \
                        vcgBackEdge.h \
                        vcgBentNearEdge.h \
                        vcgClassNameEntry.h \
                        vcgColorAttr.h \
                        vcgColorEntry.h \
                        vcgComponent.h \
                        vcgComponentList.h \
                        vcgCrossWeightAttr.h \
                        vcgEdge.h \
                        vcgEdgePrefix.h \
                        vcgGraph.h \
                        vcgLayoutAlgorithmAttr.h \
                        vcgLineStyleAttr.h \
                        vcgNearEdge.h \
                        vcgNode.h \
                        vcgNodeAlignAttr.h \
                        vcgNodePrefix.h \
                        vcgOrientationAttr.h \
                        vcgShapeAttr.h \
                        vcgStatusAttr.h \
                        vcgTextModeAttr.h \
                        vcgViewAttr.h \
                        vcgYesNoAttr.h 

# Note: QT .h files produce tons of warning if -Wall is present
# so we redefine the warning flags to an intermediate level:
WARNFLAGS = -ansi -pedantic -W -Wall -Wcast-align -Wformat=2 -Wconversion -Wno-unused -Wno-parentheses -Wno-long-long
AM_CXXFLAGS += $(OPTFLAGS)


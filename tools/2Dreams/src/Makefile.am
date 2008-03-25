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

bin_PROGRAMS= 2dreams

2dreams_LDFLAGS =  $(QTLIBDIR) $(QTLIBOBJ) $(X_PRE_LIBS) $(X_LIBS) $(X_EXTRA_LIBS) -lm

2dreams_SOURCES =   TwoDAvtView.cpp \
                    2Dreams.cpp \
                    ColDescriptor.cpp \
                    ColorShapeItem.cpp \
                    RowDescriptor.cpp \
                    ColDescVector.cpp \
                    RowDescVector.cpp \
		            EventShape.cpp \
	  	            TagValueType.cpp \
                    ZDBase.cpp \
                    AColorPalette.cpp \
                    FavoriteTags.cpp \
                    HighLightMgr.cpp \
                    hlightDialog.cpp \
                    hlightDialogImpl.cpp \
                    ancestorwidget.cpp \
                    ancestorwidgetImpl.cpp \
                    AncestorDockWin.cpp \
                    DisDockWin.cpp \
                    EventDockWin.cpp \
                    TagSelector.cpp \
                    eventTagDialog.cpp \
                    eventTagDialogImpl.cpp \
                    preferences.cpp \
                    preferencesImpl.cpp \
                    CSLEngine.cpp \
		            ConfigDB.cpp \
                    LiveDB.cpp \
                    DreamsDB.cpp \
		            PreferenceMgr.cpp \
                    ADFMgr.cpp \
		            IOMgr.cpp \
		            LogDockWin.cpp \
                    WebMimeSF.cpp\
                    find.cpp\
                    findImpl.cpp\
                    ADFParserBuilder.cpp\
                    ADFParser1.cpp\
                    ADFParser2.cpp\
                    DumpMode.cpp\
                    TabSpec.cpp\
                    DralGraphNode.cpp\
                    DralGraphEdge.cpp\
                    RowRules.cpp\
                    ShowTags.cpp\
                    mtagform.cpp\
                    mtagformImpl.cpp\
                    ltagform.cpp\
                    ltagformImpl.cpp\
                    textOverview.cpp\
                    textOverviewImpl.cpp\
                    tagDescDialogs.cpp\
                    text2dbrowser.cpp\
                    Help2DWindow.cpp\
                    MDI2DWindow.cpp\
					kxesyntaxhighlighter.cpp


nodist_2dreams_SOURCES = 2Dreams_moc.cpp \
		 	            DisDockWin_moc.cpp \
			            EventDockWin_moc.cpp \
                        TagSelector_moc.cpp \
                        TwoDAvtView_moc.cpp \
                        eventTagDialog_moc.cpp \
                        eventTagDialogImpl_moc.cpp \
                        preferences_moc.cpp \
                        preferencesImpl_moc.cpp \
                        hlightDialog_moc.cpp \
                        hlightDialogImpl_moc.cpp \
                        ancestorwidget_moc.cpp \
                        ancestorwidgetImpl_moc.cpp \
                        AncestorDockWin_moc.cpp \
                        HighLightMgr_moc.cpp \
                        ADFMgr_moc.cpp \
                        IOMgr_moc.cpp \
                        WebMimeSF_moc.cpp\
		       	        LogDockWin_moc.cpp \
                        find_moc.cpp\
                        findImpl_moc.cpp\
                        mtagform_moc.cpp\
                        mtagformImpl_moc.cpp\
                        ltagform_moc.cpp\
                        ltagformImpl_moc.cpp\
                        textOverview_moc.cpp\
                        tagDescDialogs_moc.cpp\
                    	textOverviewImpl_moc.cpp\
                        text2dbrowser_moc.cpp\
                        Help2DWindow_moc.cpp\
                        MDI2DWindow_moc.cpp\
 			            PreferenceMgr_moc.cpp 

2dreams_DIALOGS =   ancestorwidget.ui \
                    eventTagDialog.ui \
                    hlightDialog.ui\
                    preferences.ui\
                    find.ui \
                    mtagform.ui

2dreams_HFILES = 2Dreams.h \
		2DreamsDefs.h \
		2DreamsTypes.h \
		AColorPalette.h \
		ADFDefinitions.h \
		ADFParser1.h \
		ADFParser2.h \
		DumpMode.h \
		ADFParserBuilder.h \
		ADFParserInterface.h \
		AncestorDockWin.h \
		ColDescVector.h \
		ColDescriptor.h \
		ColorShapeItem.h \
        CSLEngine.h \
		ConfigDB.h \
		DBStructs.h \
		DefaultSizes.h \
		DisDockWin.h \
		DralGraphEdge.h \
		DralGraphNode.h \
		DrawingTools.h \
		DreamsDB.h \
		EventDockWin.h \
		EventItem.h \
		EventShape.h \
		FavoriteTags.h \
		HighLightMgr.h \
		IOMgr.h \
		LiveDB.h \
		LogDockWin.h \
		PreferenceMgr.h \
		RowDescVector.h \
		RowDescriptor.h \
		RowRules.h \
		ShowTags.h \
		TabSpec.h \
		TagItemDef2D.h \
		TagSelector.h \
		TagValueType.h \
		TwoDAvtView.h \
		UpdColorInterface.h \
		WebMimeSF.h \
		ZDBase.h \
		ancestorwidget.h \
		ancestorwidgetImpl.h \
		eventTagDialog.h \
		eventTagDialogImpl.h \
		find.h \
		findImpl.h \
		hlightDialog.h \
		hlightDialogImpl.h \
		mtagform.h \
		mtagformImpl.h \
		preferences.h \
		preferencesImpl.h

2dreams_MISC = 2Dreams.cpr \
               doFind \
               doMTag \
               doPrefs \
               highlight_colors_palette

EXTRA_DIST =	help \
		xpm \
		$(2dreams_HFILES) \
		$(2dreams_DIALOGS) \
		$(2dreams_MISC)

%_moc.cpp: %.h %.cpp
	$(QTMOCDIR)/moc $< -o $@

# Note: QT .h files produce tons of warning if -Wall is present
# so we redefine the warning flags to an intermediate level:
WARNFLAGS = -ansi -pedantic -W -Wall -Wcast-align -Wformat=2 -Wconversion -Winline -Wno-unused -Wno-parentheses -Wno-long-long
AM_CXXFLAGS= $(WARNFLAGS) $(OPTFLAGS) $(COREFLAGS) $(COREINCS) $(TWODREAMS_CFLAGS) $(ASIMCORE_CFLAGS) $(QTINCDIR)
AM_LDFLAGS = $(CORELIBS) $(TWODREAMS_LFLAGS) $(ASIMCORE_LFLAGS)
CLEANFILES = *moc*

#dtextedit.cpp
#drichtext_p.cpp
#dtextedit_moc.cpp
#drichtext_p_moc.cpp


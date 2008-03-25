#
#  Copyright (C) 2004-2006 Intel Corporation
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

bin_PROGRAMS= dreams

dreams_LDFLAGS = $(QTLIBDIR) $(QTLIBOBJ) $(X_PRE_LIBS) $(X_LIBS) $(X_EXTRA_LIBS) -lm -lpthread

AM_YFLAGS = -d

dreams_SOURCES = Dreams.cpp \
			ColorShapeItem.cpp \
			EventShape.cpp \
			AColorPalette.cpp \
			FavoriteTags.cpp \
			TagSelector.cpp \
			preferences.cpp \
			preferencesImpl.cpp \
			FilterHandler.cpp \
			Filter.cpp \
			CSLCache.cpp \
			WatchWindow.cpp \
			DreamsDB.cpp \
			PreferenceMgr.cpp \
			IOMgr.cpp \
			DumpMode.cpp \
			TabSpec.cpp \
			ShowTags.cpp \
			tagDescDialogs.cpp \
			text2dbrowser.cpp \
			HelpDreamsWindow.cpp \
			TempDir.cpp \
			kxesyntaxhighlighter.cpp \
			ItemTab.cpp \
			TagValueType.cpp \
			CSLData.cpp \
			ItemWFCombo.cpp \
			ItemTabWidget.cpp \
			ItemTabWidgetColumn.cpp \
			adf/parser.yy \
			adf/lexer.ll \
			adf/ADFMgr.cpp \
			adf/ADFParserBuilder.cpp\
			adf/ADFParser.cpp \
			adf/ADFMapMatch.cpp \
			adf/ADFMapColorInterpolate.cpp \
			avt/DAvtView.cpp \
			avt/ResourceDAvtView.cpp \
			avt/FloorplanDAvtView.cpp \
			avt/WaterfallDAvtView.cpp \
			dDB/DBConfig.cpp \
			dDB/LogMgr.cpp \
			dDB/StrTable.cpp \
			dDB/DBListener.cpp \
			dDB/TagDescVector.cpp \
			dDB/Dict2064.cpp \
			dDB/DralDB.cpp \
			dDB/Hash6431.cpp \
			dDB/ClockDomainMgr.cpp \
			dDB/aux/AMemObj.cpp \
			dDB/aux/StatObj.cpp \
			dDB/dralgraph/DralGraph.cpp \
			dDB/dralgraph/DralGraphEdge.cpp \
			dDB/dralgraph/DralGraphNode.cpp \
			dDB/itemheap/ItemHeap.cpp \
			dDB/taghandler/TagHandler.cpp \
			dDB/taghandler/TagFlipHandler.cpp \
			dDB/trackheap/TagIdVec.cpp \
			dDB/trackheap/TagRenameTable.cpp \
			dDB/trackheap/TagVecValue8.cpp \
			dDB/trackheap/TagVecValue8Dense.cpp \
			dDB/trackheap/TagVecValue8NF.cpp \
			dDB/trackheap/TagVecValue8NFDense.cpp \
			dDB/trackheap/TagVecValue31.cpp \
			dDB/trackheap/TagVecValue31Dense.cpp \
			dDB/trackheap/TagVecValue31Short.cpp \
			dDB/trackheap/TagVecValue64.cpp \
			dDB/trackheap/TagVecValue64Dense.cpp \
			dDB/trackheap/TagVecValue64NF.cpp \
			dDB/trackheap/TagVecValue64NFDense.cpp \
			dDB/trackheap/TrackHeap.cpp \
			dDB/trackheap/TrackVec.cpp \
			dialogs/event/EventDialog.cpp \
			dialogs/event/EventDialogImpl.cpp \
			dialogs/find/FindDialog.cpp \
			dialogs/find/FindDialogImpl.cpp \
			dialogs/find/FindDialogImplResource.cpp \
			dialogs/find/FindDialogImplFloorplan.cpp \
			dialogs/gotocycle/GoToCycleDialog.cpp \
			dialogs/gotocycle/GoToCycleDialogImpl.cpp \
			dialogs/highlightmgr/HighLightMgrDialog.cpp \
			dialogs/highlightmgr/HighLightMgrDialogImpl.cpp \
			dialogs/tag/TagDialog.cpp \
			dialogs/tag/TagDialogImpl.cpp \
			dialogs/tag/MutableTagDialogForm.cpp \
			dialogs/tag/MutableTagDialogFormImpl.cpp \
			dialogs/tag/LongTagDialogForm.cpp \
			dialogs/tag/LongTagDialogFormImpl.cpp \
			dialogs/textoverview/TextOverviewDialog.cpp \
			dialogs/textoverview/TextOverviewDialogImpl.cpp \
			dialogs/viewchoice/ViewChoiceDialog.cpp \
			dialogs/viewchoice/ViewChoiceDialogImpl.cpp \
			dockwin/ancestor/AncestorDockWin.cpp \
			dockwin/ancestor/AncestorWidget.cpp \
			dockwin/ancestor/AncestorWidgetImpl.cpp \
			dockwin/disassembly/DisDockWin.cpp \
			dockwin/disassembly/DisDockWinResource.cpp \
			dockwin/disassembly/DisDockWinFloorplan.cpp \
			dockwin/joystick/JoystickDockWin.cpp \
			dockwin/joystick/JoystickWidget.cpp \
			dockwin/log/LogDockWin.cpp \
			dockwin/pushout/PushoutDockWin.cpp \
			dockwin/pushout/PushoutWidget.cpp \
			dockwin/pushout/PushoutWidgetImpl.cpp \
			dockwin/tag/TagDockWin.cpp \
			expressions/Expression.cpp \
			expressions/ExpressionSet.cpp \
			expressions/ExpressionIf.cpp \
			expressions/ExpressionTag.cpp \
			expressions/ExpressionMap.cpp \
			expressions/ExpressionBinaryInteger.cpp \
			expressions/ExpressionBinaryString.cpp \
			expressions/ExpressionBinaryFP.cpp \
			expressions/ExpressionUnary.cpp \
			expressions/RuleEngine.cpp \
			highlight/HighLightMgr.cpp \
			highlight/HighLightMgrResource.cpp \
			highlight/HighLightMgrFloorplan.cpp \
			highlight/HighLightMgrWFall.cpp \
			layout/floorplan/LayoutFloorplan.cpp \
			layout/floorplan/LayoutFloorplanFind.cpp \
			layout/floorplan/LayoutFloorplanNode.cpp \
			layout/floorplan/LayoutFloorplanTable.cpp \
			layout/floorplan/LayoutFloorplanRow.cpp \
			layout/floorplan/LayoutFloorplanColumn.cpp \
			layout/floorplan/LayoutFloorplanRectangle.cpp \
			layout/resource/LayoutResource.cpp \
			layout/resource/LayoutResourceCache.cpp \
			layout/resource/LayoutResourceMultiRow.cpp \
			layout/resource/LayoutResourceGroup.cpp \
			layout/resource/LayoutResourceRow.cpp \
			layout/resource/LayoutResourceNode.cpp \
			layout/resource/LayoutResourceFind.cpp \
			layout/watch/LayoutWatch.cpp \
			layout/waterfall/LayoutWaterfall.cpp \
			mdi/MDIDreamsWindow.cpp \
			mdi/MDIResourceWindow.cpp \
			mdi/MDIResourceWindowCombo.cpp \
			mdi/MDIResourceWindowFold.cpp \
			mdi/MDIFloorplanWindow.cpp \
			mdi/MDIWaterfallWindow.cpp \
			mdi/MDIWatchWindow.cpp

nodist_dreams_SOURCES = Dreams_moc.cpp \
						TagSelector_moc.cpp \
                        preferences_moc.cpp \
						preferencesImpl_moc.cpp \
						IOMgr_moc.cpp \
                        DreamsDB_moc.cpp \
						tagDescDialogs_moc.cpp \
                        text2dbrowser_moc.cpp \
						HelpDreamsWindow_moc.cpp \
						ItemWFCombo_moc.cpp \
						ItemTabWidget_moc.cpp \
						PreferenceMgr_moc.cpp \
						adf/ADFMgr_moc.cpp \
						avt/DAvtView_moc.cpp \
						avt/ResourceDAvtView_moc.cpp \
						avt/FloorplanDAvtView_moc.cpp \
						avt/WaterfallDAvtView_moc.cpp \
						dialogs/event/EventDialog_moc.cpp \
						dialogs/event/EventDialogImpl_moc.cpp \
						dialogs/find/FindDialog_moc.cpp \
						dialogs/find/FindDialogImpl_moc.cpp \
						dialogs/gotocycle/GoToCycleDialog_moc.cpp \
						dialogs/gotocycle/GoToCycleDialogImpl_moc.cpp \
						dialogs/highlightmgr/HighLightMgrDialog_moc.cpp \
						dialogs/highlightmgr/HighLightMgrDialogImpl_moc.cpp \
						dialogs/tag/TagDialog_moc.cpp \
						dialogs/tag/TagDialogImpl_moc.cpp \
						dialogs/tag/MutableTagDialogForm_moc.cpp \
						dialogs/tag/MutableTagDialogFormImpl_moc.cpp \
						dialogs/tag/LongTagDialogForm_moc.cpp \
						dialogs/tag/LongTagDialogFormImpl_moc.cpp \
						dialogs/textoverview/TextOverviewDialog_moc.cpp \
						dialogs/textoverview/TextOverviewDialogImpl_moc.cpp \
                        dialogs/viewchoice/ViewChoiceDialog_moc.cpp \
						dialogs/viewchoice/ViewChoiceDialogImpl_moc.cpp \
						dockwin/ancestor/AncestorDockWin_moc.cpp \
						dockwin/ancestor/AncestorWidget_moc.cpp \
						dockwin/ancestor/AncestorWidgetImpl_moc.cpp \
						dockwin/disassembly/DisDockWin_moc.cpp \
						dockwin/disassembly/DisDockWinResource_moc.cpp \
						dockwin/disassembly/DisDockWinFloorplan_moc.cpp \
						dockwin/joystick/JoystickDockWin_moc.cpp \
						dockwin/joystick/JoystickWidget_moc.cpp \
						dockwin/log/LogDockWin_moc.cpp \
						dockwin/pushout/PushoutDockWin_moc.cpp \
						dockwin/pushout/PushoutWidget_moc.cpp \
						dockwin/pushout/PushoutWidgetImpl_moc.cpp \
						dockwin/tag/TagDockWin_moc.cpp \
						highlight/HighLightMgr_moc.cpp \
						highlight/HighLightMgrResource_moc.cpp \
						highlight/HighLightMgrFloorplan_moc.cpp \
						highlight/HighLightMgrWFall_moc.cpp \
						mdi/MDIDreamsWindow_moc.cpp \
						mdi/MDIResourceWindow_moc.cpp \
						mdi/MDIResourceWindowCombo_moc.cpp \
						mdi/MDIResourceWindowFold_moc.cpp \
						mdi/MDIFloorplanWindow_moc.cpp \
						mdi/MDIWaterfallWindow_moc.cpp \
						mdi/MDIWatchWindow_moc.cpp 

dreams_DIALOGS =   ancestorwidget.ui \
                    preferences.ui \
                    dialogs/event/EventDialog.ui \
                    dialogs/find/FindDialog.ui \
                    dialogs/gotocycle/GoToCycleDialog.ui \
                    dialogs/highlightmgr/HighLightMgrDialog.ui \
                    dialogs/tag/TagDialog.ui \
                    dialogs/tag/MutableTagDialogForm.ui \
                    dialogs/tag/LongTagDialogForm.ui \
                    dialogs/textoverview/TextOverviewDialog.ui \
                    dialogs/viewchoice/ViewChoiceDialog.ui

dreams_HFILES = Dreams.h \
				DreamsDefs.h \
				DreamsTypes.h \
				AColorPalette.h \
				DumpMode.h \
				DumpMode.h \
				ColorShapeItem.h \
				FilterHandler.h \
				Filter.h \
				CSLCache.h \
				DBStructs.h \
				DrawingTools.h \
				DreamsDB.h \
				EventShape.h \
				FavoriteTags.h \
				IOMgr.h \
				WatchWindow.h \
				PreferenceMgr.h \
				ShowTags.h \
				TabSpec.h \
				TagItemDef2D.h \
				TagSelector.h \
				preferences.h \
				TempDir.h \
				preferencesImpl.h \
				ItemTab.h \
				ItemWFCombo.h \
				ItemTabWidget.h \
				ItemTabWidgetColumn.h \
				TagValueType.h \
				adf/ADFDefinitions.h \
				adf/ADFParser.h \
                adf/ADFParserInterface.h \
				adf/ADFParserBuilder.h \
				adf/ADFMap.h \
				adf/ADFMapMatch.h \
				adf/ADFMapColorInterpolate.h \
				adf/parserCtrl.h \
				avt/DAvtView.h \
                avt/ResourceDAvtView.h \
				avt/FloorplanDAvtView.h \
				avt/WaterfallDAvtView.h \
				dDB/DRALTag.h \
				dDB/DBConfig.h \
				dDB/Hash6431.h \
				dDB/ClockDomainMgr.h \
				dDB/DBItoa.h \
				dDB/LogMgr.h \
				dDB/DBListenerDef.h \
				dDB/PrimeList.h \
				dDB/DBListener.h \
				dDB/Dict2064.h \
				dDB/StrTable.h \
				dDB/DralDBDefinitions.h \
				dDB/TagDescVector.h \
				dDB/DralDB.h \
				dDB/aux/AEVector.h \
				dDB/aux/AMemObj.h \
				dDB/aux/FValueVector.h \
				dDB/aux/GVector.h \
				dDB/aux/StatObj.h \
				dDB/aux/ZipObject.h \
				dDB/draldb_syntax.h \
				dDB/dralgraph/DralGraph.h \
				dDB/dralgraph/DralGraphEdge.h \
				dDB/dralgraph/DralGraphNode.h \
				dDB/itemheap/ItemHeap.h \
				dDB/itemheap/ItemHeapDef.h \
				dDB/taghandler/TagHandler.h \
				dDB/taghandler/TagFlipHandler.h \
				dDB/trackheap/TagIdVec.h \
				dDB/trackheap/TagRenameTable.h \
				dDB/trackheap/TagVec.h \
				dDB/trackheap/TagVecValue8.h \
				dDB/trackheap/TagVecValue8Dense.h \
				dDB/trackheap/TagVecValue8NF.h \
				dDB/trackheap/TagVecValue8NFDense.h \
				dDB/trackheap/TagVecValue31.h \
				dDB/trackheap/TagVecValue31Dense.h \
				dDB/trackheap/TagVecValue31Short.h \
				dDB/trackheap/TagVecValue64.h \
				dDB/trackheap/TagVecValue64Dense.h \
				dDB/trackheap/TagVecValue64NF.h \
				dDB/trackheap/TagVecValue64NFDense.h \
				dDB/trackheap/TrackVec.h \
				dDB/trackheap/TrackHeap.h \
				dDB/trackheap/TrackHeapDef.h \
				dialogs/event/EventDialog.h \
				dialogs/event/EventDialogImpl.h \
				dialogs/find/FindDialog.h \
				dialogs/find/FindDialogImpl.h \
				dialogs/find/FindDialogImplResource.h \
				dialogs/find/FindDialogImplFloorplan.h \
				dialogs/gotocycle/GoToCycleDialog.h \
				dialogs/gotocycle/GoToCycleDialogImpl.h \
				dialogs/highlightmgr/HighLightMgrDialog.h \
				dialogs/highlightmgr/HighLightMgrDialogImpl.h \
				dialogs/tag/TagDialog.h \
				dialogs/tag/TagDialogImpl.h \
				dialogs/tag/MutableTagDialogForm.h \
				dialogs/tag/MutableTagDialogFormImpl.h \
				dialogs/tag/LongTagDialogForm.h \
				dialogs/tag/LongTagDialogFormImpl.h \
				dialogs/textoverview/TextOverviewDialogs.h \
				dialogs/textoverview/TextOverviewDialogsImpl.h \
				dialogs/viewchoice/ViewChoiceDialog.h \
				dialogs/viewchoice/ViewChoiceDialogImpl.h \
				dockwin/ancestor/AncestorDockWin.h \
				dockwin/ancestor/AncestorWidget.h \
				dockwin/ancestor/AncestorWidgetImpl.h \
				dockwin/disassembly/DisDockWin.h \
				dockwin/disassembly/DisDockWinResource.h \
				dockwin/disassembly/DisDockWinFloorplan.h \
				dockwin/joystick/JoystickDockWin.h \
				dockwin/joystick/JoystickWidget.h \
				dockwin/log/LogDockWin.h \
				dockwin/pushout/PushoutDockWin.h \
				dockwin/pushout/PushoutWidget.h \
				dockwin/pushout/PushoutWidgetImpl.h \
				dockwin/tag/TagDockWin.h \
				expressions/Expression.h \
				expressions/ExpressionConstant.h \
				expressions/ExpressionIf.h \
				expressions/ExpressionTag.h \
				expressions/ExpressionMap.h \
				expressions/ExpressionSet.h \
				expressions/ExpressionBinaryInteger.h \
				expressions/ExpressionBinaryString.h \
				expressions/ExpressionBinaryFP.h \
				expressions/ExpressionUnary.h \
				expressions/RuleEngine.h \
				highlight/HighLightMgr.h \
				highlight/HighLightMgrResource.h \
				highlight/HighLightMgrFloorplan.h \
				highlight/HighLightMgrWFall.h \
				layout/resource/LayoutResource.h \
				layout/resource/LayoutResourceCache.h \
				layout/resource/LayoutResourceMultiRow.h \
				layout/resource/LayoutResourceGroup.h \
				layout/resource/LayoutResourceRow.h \
				layout/resource/LayoutResourceNode.h \
   				layout/resource/LayoutResourceFind.h \
				layout/floorplan/LayoutFloorplan.h \
				layout/floorplan/LayoutFloorplanFind.h \
				layout/floorplan/LayoutFloorplanNode.h \
				layout/floorplan/LayoutFloorplanTable.h \
				layout/floorplan/LayoutFloorplanRow.h \
				layout/floorplan/LayoutFloorplanColumn.h \
				layout/floorplan/LayoutFloorplanRectangle.h \
				layout/watch/LayoutWatch.h \
				layout/waterfall/LayoutWaterfall.h \
				mdi/MDIDreamsWindow.h \
				mdi/MDIResourceWindow.h \
				mdi/MDIResourceWindowCombo.h \
				mdi/MDIResourceWindowFold.h \
				mdi/MDIFloorplanWindow.h \
				mdi/MDIWaterfallWindow.h \
				mdi/MDIWatchWindow.h

dreams_MISC = 2Dreams.cpr \
               doFind \
               doMTag \
               doPrefs \
               highlight_colors_palette

EXTRA_DIST =	help \
		xpm \
		$(dreams_HFILES) \
		$(dreams_DIALOGS) \
		$(dreams_MISC)

%_moc.cpp: %.h %.cpp
	$(QTMOCDIR)/moc $< -o $@

# Note: QT .h files produce tons of warning if -Wall is present
# so we redefine the warning flags to an intermediate level:
WARNFLAGS = -ansi -pedantic -W -Wall -Wcast-align -Wformat=2 -Wconversion -Wno-unused -Wno-parentheses -Wno-long-long

AM_CXXFLAGS += $(QTINCDIR)

CLEANFILES = adf/parser.cc \
    adf/lexer.cc \
    adf/parser.h \
    adf/lex.yy.c \
    *moc* 

#dtextedit.cpp
#drichtext_p.cpp
#dtextedit_moc.cpp
#drichtext_p_moc.cpp


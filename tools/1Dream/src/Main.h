/*
 * Copyright (C) 2003-2006 Intel Corporation
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
 */

#ifndef _STRIP_MAIN_H
#define _STRIP_MAIN_H

#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>


#include <qprogressdialog.h>
#include <qpen.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qmainwindow.h>
#include <qintdict.h>
#include <qdatetime.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qlabel.h>
#include <qimage.h>
#include <qprogressdialog.h>
#include <qstylefactory.h>
#include <qaction.h>
#include <qsignalmapper.h>
#include <qdict.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qtextbrowser.h>

#include "StripEntry.h"
#include "StripChartView.h"

#include "asim/HelpWindow.h"
#include "asim/BookmarkMgr.h"

#include "SelectorImpl.h"
#include "PropertiesImpl.h"
#include "error.h"
#include "ColorsImpl.h"
#include "LittleSelectorImpl.h"
#include "Ruler.h"

#define NCYCLES 500000
#define MAXSAMPLEVALUE 500000
#define MAXMARKERLIMIT 1000

// icons
#include "xpm/selectTool.xpm"
#include "xpm/paddingTool.xpm"
#include "xpm/zoomingTool.xpm"
#include "xpm/distanceTool.xpm"
#include "xpm/shadingTool.xpm"
#include "xpm/fileopen.xpm"
#include "xpm/filesave.xpm"
#include "xpm/fileprint.xpm"
#include "xpm/help_contents.xpm"
#include "xpm/viewmag+.xpm"
#include "xpm/viewmag-.xpm"
#include "xpm/zhp.xpm"
#include "xpm/zhm.xpm"
#include "xpm/zvp.xpm"
#include "xpm/zvm.xpm"
#include "xpm/resetarto.xpm"
#include "xpm/annRect.xpm"
#include "xpm/annCircle.xpm"
#include "xpm/annLine.xpm"
#include "xpm/annText.xpm"

float MaxOnVisibleStrips(allFilesData *myDB);
INT32 MaxLinesOnVisibleStrips(allFilesData *myDB);

// Structured needed to read the file
struct LineData 
{
    INT32 position;
    char description[100];
    INT32 threads;
    INT32 frequency;
    INT32 base_frequency;
    INT32 max_elems;
};

struct MarkerLimiter
{
    INT64 val0;
    UINT64 val1;
};

class Main : public QMainWindow 
{
    Q_OBJECT

  public:

    Main(QWidget* parent=0, const char* name=0, WFlags f=0);
    ~Main();

  public slots:
    void do_help();
    void do_about();

  public slots:

    void hideProgress();

    // file:
    void do_openMarkerFile(char *str=NULL);
    void do_openStripFile(char *str=NULL);
    void do_openStripFileVersion1_0(INT32 input, QString &fn, INT32 lines, INT32 strips);
    void do_openStripFileVersion1_1(INT32 input, QString &fn);
    
    // view
    void do_selection();
    void do_configuration();
    void do_zoomIn();
    void do_zoomOut();
    void do_zoomInH();
    void do_zoomOutH();
    void do_zoomInV();
    void do_zoomOutV();
    void do_resetAspectRatio();
    void do_z200();
    void do_z100();
    void do_z50();
    void do_z25();
    void do_scaleToFit();
    void updateRulers();

    // tools
    void do_selectToolEnabled();
    void do_paddingToolEnabled();
    void do_zoomingToolEnabled();
    void do_distanceToolEnabled();
    void do_shadingToolEnabled();
    void do_selectEvents();

    // annotations
    void do_drawLine();
    void do_drawRectangle();
    void do_drawCircle();
    void do_insertText();
    
    // bookmarks
    void do_addBookmark();
    void do_manageBookmark();

    // options
    void do_makeStyle(const QString &style);

    // window
    void do_switchAnnotationsBar();
    void do_switchPaletteBar();
    void do_switchViewBar();
    void do_switchToolsBar();
    void do_switchFileBar();
    void do_verticalGrid();
    void do_horizontalGrid();
    void toolsBarVisibilityChanged(bool visible);
    void fileBarVisibilityChanged(bool visible);
    void viewBarVisibilityChanged(bool visible);
    void annotationsBarVisibilityChanged(bool visible);
    
    // deprecated stuff
    void do_print();

  private:
    void addStylesMenu();
    
    void init_random_db();
    void init_general_parameters();
    void abortDueToMemory();
    
  private:
    bool    aspectRatioLocked;
    StripChartView* asv;
    QProgressDialog *progress;
    QFont appFont;
    QStatusBar* myStatusBar;
    QMenuBar* menu;
    QPopupMenu* file;
    QPopupMenu* window;
    QPopupMenu* options;
    QPopupMenu* view;
    QPopupMenu* zfixed;
    QPopupMenu* zoneaxe;
    QPopupMenu* tools;
    QPopupMenu* pointers;
    QPopupMenu* annotation;
    QPopupMenu* style;
    QPopupMenu* bookmark;

    QGridLayout* gridLayout;
    Ruler    *hRuler;
    Ruler    *vRuler;
    
    QToolBar* annotationsTools;    
    QToolBar* viewTools;    
    QToolBar* pointerTools;    
    QToolBar* fileTools;    
    
    int win_annotations_bar_id;
    int win_view_bar_id;
    int win_view_pal_id;
    int win_tool_bar_id;
    int win_file_bar_id;

    int axisH;
    int axisV;
    int printAxisH;
    int printAxisV;

    QPrinter* printer;
    
    allFilesData *theVectorDB;
    ColorsImpl   *visiblePalette;
    
    BookmarkMgr *bkMgr;

  private:        //pixmaps & icons
    QPixmap selectToolIcon, paddingToolIcon, zoomingToolIcon;
    QPixmap distanceToolIcon, shadingToolIcon;
    QPixmap openFileIcon, saveFileIcon,    printIcon;
    QPixmap helpContentsIcon;
    QPixmap zoominIcon,zoomoutIcon;
    QPixmap zhpIcon,zhmIcon,zvpIcon,zvmIcon,resetartoIcon; 
    QPixmap annLineIcon,annRectIcon,annCircleIcon,annTextIcon;
};





#endif

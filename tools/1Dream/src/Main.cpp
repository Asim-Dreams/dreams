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

#include "Main.h"

MarkerLimiter markerLimiter[MAXMARKERLIMIT];

//
// This routine search in all the opened files the maxim number of lines to
// be represented on the canvas. The routine simple run accross all the
// files searching the maximum value and returns it.
// 
//
INT32 MaxLinesOnVisibleStrips(allFilesData *myDB)
{
    INT32 max = 0;    

    for(INT32 i = 0; i<MAX_OPENED_FILES; i++) 
    {
        if(myDB->active[i]==1) 
        {
            if ((myDB->files[i].lines*(myDB->files[i].frequency/1000)) > max) 
            { 
                max = myDB->files[i].lines*(myDB->files[i].frequency/1000);
            }    
        }         
    }        
    return max;
}

//
// This routine search in all the opened files the maximum value of
// selected strips (they must be hide or unhide)
//
float MaxOnVisibleStrips(allFilesData *myDB)
{
    float max = 0.0;

    for(INT32 i = 0; i<MAX_OPENED_FILES; i++) 
    {
        if(myDB->active[i]==1) 
        {
            for(INT32 j = 0; j<myDB->files[i].elems; j++) 
            {
                for(INT32 k = 0; k<myDB->files[i].fullData[j].threads; k++) 
                {
                    if( (myDB->files[i].fullData[j].visible[k]==1) || 
                        (myDB->files[i].fullData[j].activated[k]==1)) 
                    {
                        //
                        // At this point we evaluate if the selected strips
                        // are in relativeView, maxView or standard
                        // valueView because the maximum value if we are
                        // in non standard view must be 100.0.
                        //
                        if(myDB->files[i].fullData[j].relativeView[k]==1) 
                        {
                            max = (max>=100.0)?max:myDB->files[i].fullData[j].maxInStripValue[k];
                        }
                        else if (myDB->files[i].fullData[j].maxView[k]==1)
                        {
                            max = (max>=100.0)?max:myDB->files[i].fullData[j].maxInStripValue[k];
                        }
                        else
                        {
                            max = (max>=myDB->files[i].fullData[j].maxInStripValue[k])?max:myDB->files[i].fullData[j].maxInStripValue[k];
                        }
                    }
                }
            }
        }
    }
    //
    // Now we use three decimal places to store the data. The three less
    // significant digits are the decimals and because that we need max*1000
    // points in the canvas.
    //
    max = max*1000.0;
    return max;
}

// -----------------------------------------------------------------------------
// -- The Main Constructor
// -----------------------------------------------------------------------------
Main::Main(QWidget* parent, const char* name, WFlags f) :
     QMainWindow(parent,name,f)
{
    //
    // At the beggining we set the aspect ratio to unlocked value.
    // This variable can be modified during the execution by the user.
    //
    aspectRatioLocked = false;

    // --------------------------------------------------------------------
    // Toolbar stuff (File...)
    // --------------------------------------------------------------------
    fileTools = new QToolBar( this, "file" );
    fileTools->setLabel( "File" );

    openFileIcon = QPixmap( fileopen );
    QToolButton* openFileBtn = new QToolButton( openFileIcon, 
                                                "Open", 
                                                QString::null,
                                                this, 
                                                SLOT(do_openStripFile()), 
                                                fileTools, 
                                                "open" );

    saveFileIcon = QPixmap( filesave );
    QToolButton* saveFileBtn = new QToolButton( saveFileIcon, 
                                                "Save", 
                                                QString::null,
                                                this, 
                                                SLOT(do_paddingToolEnabled()), 
                                                fileTools, 
                                                "save" );

    printIcon = QPixmap( fileprint );
    QToolButton* printBtn = new QToolButton( printIcon, 
                                             "Print", 
                                             QString::null,
                                             this, 
                                             SLOT(do_shadingToolEnabled()), 
                                             fileTools, 
                                             "print" );
                
    connect( fileTools, 
             SIGNAL(visibilityChanged(bool)), 
             SLOT(fileBarVisibilityChanged(bool)) );
    
    moveDockWindow( fileTools,DockTop);
    
    // --------------------------------------------------------------------
    // Toolbar stuff (Pointer)
    // --------------------------------------------------------------------
    printAxisH = 0;
    printAxisV = 0;    
    pointerTools = new QToolBar( this, "tools" );
    pointerTools->setLabel( "Tools" );
    
    selectToolIcon = QPixmap( selectTool );
    QToolButton* selectToolBtn = new QToolButton( selectToolIcon, 
                                                  "Select", 
                                                  QString::null,
                                                  this, 
                                                  SLOT(do_selectToolEnabled()), 
                                                  pointerTools, 
                                                  "select" );

    paddingToolIcon = QPixmap( paddingTool );
    QToolButton* paddingToolBtn = new QToolButton( paddingToolIcon, 
                                                   "Padding",
                                                   QString::null,
                                                   this, 
                                                   SLOT(do_paddingToolEnabled()), 
                                                   pointerTools, 
                                                   "padding" );

    zoomingToolIcon = QPixmap( zoomingTool );
    QToolButton* zoomingToolBtn = new QToolButton( zoomingToolIcon, 
                                                   "Zooming", 
                                                   QString::null,
                                                   this, 
                                                   SLOT(do_zoomingToolEnabled()), 
                                                   pointerTools, 
                                                   "zooming" );

    distanceToolIcon = QPixmap( distanceTool );
    QToolButton* distanceToolBtn = new QToolButton( distanceToolIcon, 
                                                    "Distance", 
                                                    QString::null,
                                                    this, 
                                                    SLOT(do_distanceToolEnabled()), 
                                                    pointerTools, 
                                                    "distance" );

    connect( pointerTools, 
             SIGNAL(visibilityChanged(bool)), 
             SLOT(toolsBarVisibilityChanged(bool)) );
    
    // --------------------------------------------------------------------
    // Toolbar stuff (View...)
    // --------------------------------------------------------------------
    viewTools = new QToolBar( this, "view" );
    viewTools->setLabel( "View" );
    
    zoominIcon = QPixmap( viewmag_plus );
    QToolButton* openZInBtn = new QToolButton( zoominIcon, 
                                               "Zoom in", 
                                               QString::null,
                                               this, 
                                               SLOT(do_zoomIn()), 
                                               viewTools, 
                                               "zin" );

    zoomoutIcon = QPixmap( viewmag_minus );
    QToolButton* openZOutBtn = new QToolButton( zoomoutIcon, 
                                                "Zoom out", 
                                                QString::null,
                                                this, 
                                                SLOT(do_zoomOut()), 
                                                viewTools, 
                                                "zout" );

    zhpIcon = QPixmap( zhp );
    QToolButton* openZHPBtn = new QToolButton( zhpIcon, 
                                               "Zoom in horizontal", 
                                               QString::null,
                                               this, 
                                               SLOT(do_zoomInH()), 
                                               viewTools, 
                                               "zhp" );

    zhmIcon = QPixmap( zhm );
    QToolButton* openZHMBtn = new QToolButton( zhmIcon, 
                                               "Zoom out horizontal", 
                                               QString::null,
                                               this, 
                                               SLOT(do_zoomOutH()), 
                                               viewTools, 
                                               "zhm" );

    zvpIcon = QPixmap( zvp );
    QToolButton* openZVPBtn = new QToolButton( zvpIcon, 
                                               "Zoom in vertical", 
                                               QString::null,
                                               this, 
                                               SLOT(do_zoomInV()), 
                                               viewTools, 
                                               "zvp" );

    zvmIcon = QPixmap( zvm );
    QToolButton* openZVMBtn = new QToolButton( zvmIcon, 
                                               "Zoom out vertical", 
                                               QString::null,
                                               this, 
                                               SLOT(do_zoomOutV()), 
                                               viewTools, 
                                               "zvm" );
                
    resetartoIcon = QPixmap(resetarto);
    QToolButton* openARTOBtn = new QToolButton( resetartoIcon, 
                                                "Lock/unlock aspect ratio", 
                                                QString::null,
                                                this, 
                                                SLOT(do_resetAspectRatio()), 
                                                viewTools, 
                                                "arto" );

    connect( viewTools, 
             SIGNAL(visibilityChanged(bool)), 
             SLOT(viewBarVisibilityChanged(bool)) );
    
    moveDockWindow( viewTools,DockTop);

    // --------------------------------------------------------------------
    // Main menu stuff
    // --------------------------------------------------------------------
    appFont = QApplication::font();
    menu = menuBar();

    // --------------------------------------------------------------------
    file = new QPopupMenu( menu );
    file->insertItem("Open &strip file...", this, SLOT(do_openStripFile()), CTRL+Key_S);
    file->insertSeparator();
    file->insertItem("E&xit", qApp, SLOT(quit()), CTRL+Key_Q);
    menu->insertItem("&File", file);

    // --------------------------------------------------------------------
    view = new QPopupMenu( menu );
    view->insertSeparator();
    view->insertItem("Select Strips", this, 
                     SLOT(do_selection()), ALT+Key_S);
    view->insertItem("Properties", this, 
                     SLOT(do_configuration()), ALT+Key_C);
    view->insertSeparator();
    axisH = view->insertItem("&Horizontal Grid ", this, 
                             SLOT(do_horizontalGrid()));
    view->setItemChecked(axisH,false);
    axisV = view->insertItem("&Vertical Grid", this, 
                             SLOT(do_verticalGrid()));
    view->setItemChecked(axisV,false);
    view->insertSeparator();
    view->insertItem(zoominIcon,"Zoom &in", this, 
                     SLOT(do_zoomIn()), CTRL+Key_Plus);
    view->insertItem(zoomoutIcon,"Zoom &out", this, 
                     SLOT(do_zoomOut()), CTRL+Key_Minus);
    view->insertSeparator();

    zoneaxe = new QPopupMenu( view );
    zoneaxe->insertItem(zhpIcon,"Zoom in horizontal", this, 
                        SLOT(do_zoomInH()),CTRL+ALT+Key_Plus);
    zoneaxe->insertItem(zvpIcon,"Zoom in vertical", this, 
                        SLOT(do_zoomInV()),CTRL+SHIFT+Key_Plus);
    zoneaxe->insertItem(zhmIcon,"Zoom out horizontal", this, 
                        SLOT(do_zoomOutH()),CTRL+ALT+Key_Minus);
    zoneaxe->insertItem(zvmIcon,"Zoom out vertical", this, 
                        SLOT(do_zoomOutV()),CTRL+SHIFT+Key_Minus);
    view->insertItem("Zoom axe ...", zoneaxe);
    view->insertItem(resetartoIcon,"&Reset aspect ratio", this, 
                     SLOT(do_resetAspectRatio()));
    view->insertSeparator();

    zfixed = new QPopupMenu (view);
    zfixed->insertItem("&200%", this, SLOT(do_z200()));
    zfixed->insertItem("&100%", this, SLOT(do_z100()));
    zfixed->insertItem("&50%", this, SLOT(do_z50()));
    zfixed->insertItem("&25%", this, SLOT(do_z25()));

    view->insertItem("&Scale to fit", this, SLOT(do_scaleToFit()));
    view->insertItem("Scale to ...", zfixed);
    menu->insertItem("&View", view);

    // --------------------------------------------------------------------
    tools = new QPopupMenu(menu);
    
    pointers = new QPopupMenu(tools);
    pointers->insertItem(selectToolIcon,"&Select pointer", this, 
                         SLOT(do_selectToolEnabled()));
    pointers->insertItem(paddingToolIcon,"&Padding pointer", this, 
                         SLOT(do_paddingToolEnabled()));
    pointers->insertItem(zoomingToolIcon,"&Zooming pointer", this, 
                         SLOT(do_zoomingToolEnabled()));
    pointers->insertItem(distanceToolIcon,"&Distance pointer", this, 
                         SLOT(do_distanceToolEnabled()));
    pointers->insertItem(shadingToolIcon,"S&hading pointer", this, 
                         SLOT(do_shadingToolEnabled()));
    tools->insertItem("&Pointers",pointers);

    tools->insertSeparator();        
    tools->insertItem("&Active events", this, 
                      SLOT(do_selectEvents()));
    menu->insertItem("&Tools", tools);

    // --------------------------------------------------------------------
    bookmark = new QPopupMenu(menu);
    bookmark->insertItem("&Add bookmark here", this, 
                         SLOT(do_addBookmark()));
    bookmark->insertItem("&Manage bookmarks", this,  
                         SLOT(do_manageBookmark()));
    bookmark->insertSeparator();
    menu->insertItem("&Bookmarks", bookmark);

    // --------------------------------------------------------------------
    options = new QPopupMenu( menu );
    menu->insertItem("&Options",options);

    // --------------------------------------------------------------------
    window = new QPopupMenu(menu);
    window->setCheckable( TRUE );
    win_tool_bar_id = window->insertItem("&Tools Bar", this, 
                                         SLOT(do_switchToolsBar()));
    window->setItemChecked(win_tool_bar_id,true);

    win_file_bar_id = window->insertItem("&File Bar", this, 
                                         SLOT(do_switchFileBar()));
    window->setItemChecked(win_file_bar_id,true);

    win_view_bar_id = window->insertItem("&View Bar", this, 
                                         SLOT(do_switchViewBar()));
    window->setItemChecked(win_view_bar_id,true);

    win_view_pal_id = window->insertItem("&Selected Strips", this, 
                                         SLOT(do_switchPaletteBar()));
    window->setItemChecked(win_view_pal_id,false);

    menu->insertItem("&Window",window);
    

    // --------------------------------------------------------------------
    addStylesMenu();

    // --------------------------------------------------------------------
    menu->insertSeparator();
    QPopupMenu* help = new QPopupMenu( menu );
    helpContentsIcon = QPixmap(help_contents);
    help->insertItem("&About", this, SLOT(do_about()));
    help->insertItem(helpContentsIcon,"&Contents", this, 
                     SLOT(do_help()), Key_F1);
    menu->insertItem("&Help",help);

    // --------------------------------------------------------------------
    // --------------------------------------------------------------------
    // --------------------------------------------------------------------

    // create the visualization matrix
    theVectorDB = (allFilesData*)malloc(sizeof(allFilesData));
    if (theVectorDB==NULL) abortDueToMemory();
    init_random_db();
    init_general_parameters();

    // Widget Structures
    
    QWidget *w = new QWidget(this);
    gridLayout =  new QGridLayout(w,2,2);

    hRuler = new Ruler (Ruler::Horizontal, w);
    vRuler = new Ruler (Ruler::Vertical, w);
    gridLayout->addWidget (hRuler, 0, 1);
    gridLayout->addWidget (vRuler, 1, 0);

    // create the ASIM-2Dreams-Scroller
    asv = new StripChartView(w,theVectorDB,NCYCLES,MAXSAMPLEVALUE, hRuler, vRuler);
    hRuler->setStripChartView(asv);
    vRuler->setStripChartView(asv);

    connect(asv, SIGNAL(contentsMoving(int,int)), 
            hRuler, SLOT(updateVisibleArea(int,int)));
    connect(asv->horizontalScrollBar(), SIGNAL(sliderReleased()), 
            hRuler, SLOT(drawRuler()));
    connect(asv->horizontalScrollBar(), SIGNAL(sliderMoved(int)), 
            hRuler, SLOT(drawRuler(int)));
    connect(asv->horizontalScrollBar(), SIGNAL(valueChanged(int)), 
            hRuler, SLOT(drawRuler(int)));
    connect(asv, SIGNAL(contentsMoving(int,int)), 
            vRuler, SLOT(updateVisibleArea(int,int)));
    connect(asv->verticalScrollBar(), SIGNAL(sliderReleased()),
            vRuler, SLOT(drawRuler()));
    connect(asv->verticalScrollBar(), SIGNAL(sliderMoved(int)), 
            vRuler, SLOT(drawRuler(int)));
    connect(asv->verticalScrollBar(), SIGNAL(valueChanged(int)), 
            vRuler, SLOT(drawRuler(int)));

    myStatusBar = statusBar();
    asv->setStatusBarInc(myStatusBar);
    
    gridLayout->addWidget (asv, 1, 1);
    setCentralWidget(w);

    printer = NULL;

    // --------------------------------------------------------------------
    // --------------------------------------------------------------------
    bkMgr = new BookmarkMgr(this,bookmark,asv);
    visiblePalette->setSizeGripEnabled(TRUE);
    visiblePalette->setView(asv);
    
    //addZoomDialog(viewTools);

}


void Main::addStylesMenu()
{
    // --------------------------------------------------------------------
    // Styles stuff
    // --------------------------------------------------------------------
    style = new QPopupMenu(menu);
    style->setCheckable( TRUE );
    options->insertItem("&Style",style);
    style->setCheckable( TRUE );

    QActionGroup *ag = new QActionGroup( menu, 0 );
    ag->setExclusive(TRUE);
    QSignalMapper *styleMapper = new QSignalMapper( this );
    connect( styleMapper, SIGNAL( mapped( const QString& ) ),
             this, SLOT( do_makeStyle(const QString&)));
    QStringList list = QStyleFactory::keys();
    list.sort();
    QDict<int> stylesDict( 17, FALSE );

    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
    {
        QString styleStr = *it;
        QString styleAccel = styleStr;
        if( stylesDict[styleAccel.left(1)] )
        {
            for( uint i = 0; i < styleAccel.length(); i++ )
            {
                if( !stylesDict[styleAccel.mid( i, 1 )] )
                {
                    stylesDict.insert(styleAccel.mid( i, 1 ), (const int *)1);
                    styleAccel = styleAccel.insert( i, '&' );
                    break;
                }
            }
        }
        else
        {
            stylesDict.insert(styleAccel.left(1), (const int *)1);
            styleAccel = "&"+styleAccel;
        }

        QAction *a = new QAction( styleStr, QIconSet(), styleAccel, 0, ag, 0, ag->isExclusive() );
        connect( a, SIGNAL( activated() ), styleMapper, SLOT(map()) );
        styleMapper->setMapping( a, a->text() );
    }
    ag->addTo(style);
    // --------------------------------------------------------------------
    // End of Styles stuff
    // --------------------------------------------------------------------
}


void Main::abortDueToMemory()
{
    printf("Unable to allocate memory: %ld bytes\n",(long int)sizeof(StripEntry)*NCYCLES);
    printf("Sorry, aborting application...\n");fflush(stdout);
    exit(-1);
}

void Main::init_random_db()
{
    for(INT32 i=0;i<MAX_OPENED_FILES;i++) 
        theVectorDB->active[i] = 0;
}

void Main::init_general_parameters()
{
    theVectorDB->generalProperties.HAxis =      QColor("Black");
    theVectorDB->generalProperties.VAxis =      QColor("Black");
    theVectorDB->generalProperties.Markers =    QColor("Red");
    theVectorDB->generalProperties.HGrid =      QColor("Violet");
    theVectorDB->generalProperties.VGrid =      QColor("Violet");
    theVectorDB->generalProperties.Background = QColor("White");
    
    visiblePalette = new ColorsImpl(this,"Error",FALSE, 0,theVectorDB);
}

void Main::hideProgress()
{
    progress->hide();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This method opens a binary file that contains stripchart data previous     //
// translated using stc2stb program provided with StripChartViewer            //
// distribution.                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void Main::do_openStripFileVersion1_0(INT32 input, QString &fn, INT32 lines, INT32 strips)
{ 
    float value;
    INT32 Counter = 0;
    INT32 blocks,markers;
    INT32 i;
    INT32 *stripdata;
    INT32 *actual;
    INT32 file;
    float max;
    LineData *readed;
    StripData *tmp;
    INT32 frequency;

    // Reading header. It contains lines,strips,blocks and markers integer. Remember
    // the size of integer element in C++ using cxx in TRU64 and Alpha platform
    read(input,(char *)&blocks,sizeof(int));
    read(input,(char *)&markers,sizeof(int));

    progress = new QProgressDialog("Reading data...","Hide",lines,asv,"progress",TRUE);
    connect(progress, SIGNAL(cancelled()),this,SLOT(hideProgress()));
    progress->show();

    // We have an static structure to maintain opened all the files the 
    // user has requested before during this execution. At this point we 
    // must search the first unused file structure and alloc memory to
    // store it and its data.
    file = 0;
    while(theVectorDB->active[file]!=0) file++;    

    theVectorDB->active[file] = 1;

    theVectorDB->files[file].fileName=new QString(fn); 

    theVectorDB->files[file].fullData = (StripData *)malloc(sizeof(StripData)*blocks);
    theVectorDB->files[file].elems = blocks; 
    theVectorDB->files[file].lines = lines; 
    theVectorDB->files[file].marker_elems = markers; 

    // Now we have all the blocks allocated.
    // We use the LineData structure to read the data from disk.
    readed = new struct LineData[blocks];

    read(input,(char *)readed,blocks*sizeof(struct LineData));
    
 
    // We have the data and we must create all the required stripcharts headers
    for(i = 0;i<blocks;i++)
    {
        // Number of threads active at this block
        theVectorDB->files[file].fullData[i].threads = readed[i].threads;
        // Number of threads active at this block
        theVectorDB->files[file].fullData[i].elems = lines;

        // Copying description 
        for(INT32 j = 0;j<100;j++) 
        {
            theVectorDB->files[file].fullData[i].description[j] = readed[i].description[j];
        }
    
        // Alloc memory for data from stripcharts
        for(INT32 j = 0;j<readed[i].threads;j++) 
        {
            theVectorDB->files[file].fullData[i].values[j] = (float *)malloc(sizeof(float)*lines);
            theVectorDB->files[file].fullData[i].visible[j] = 0;
            theVectorDB->files[file].fullData[i].activated[j] = 0;
            theVectorDB->files[file].fullData[i].smooth[j] = 0;
            theVectorDB->files[file].fullData[i].smoothView[j] = 0;
            theVectorDB->files[file].fullData[i].breakSmooth[j] = 0;
            theVectorDB->files[file].fullData[i].color[j].setRgb(rand()%256,rand()%256,rand()%256);
            theVectorDB->files[file].fullData[i].maxValue[j] = readed[i].max_elems;
        }
        // Calculate Accumulated strip
        if(readed[i].threads>1) 
        {
            INT32 j = readed[i].threads;    
            theVectorDB->files[file].fullData[i].values[j] = (float *)malloc(sizeof(float)*lines);
            theVectorDB->files[file].fullData[i].visible[j] = 0;
            theVectorDB->files[file].fullData[i].activated[j] = 0;
            theVectorDB->files[file].fullData[i].smooth[j] = 0;
            theVectorDB->files[file].fullData[i].smoothView[j] = 0;
            theVectorDB->files[file].fullData[i].breakSmooth[j] = 0;
            theVectorDB->files[file].fullData[i].color[j].setRgb(rand()%256,rand()%256,rand()%256);
        }

        // Updating file frequency
        theVectorDB->files[file].frequency = readed[i].base_frequency;

    }

    delete readed;

    theVectorDB->files[file].markers = (MarkerData *)malloc(sizeof(MarkerData)*markers);

    for(i = 0;i<markers;i++) 
    {
        read(input,(char *)(&(theVectorDB->files[file].markers[i].position)),sizeof(int));
        read(input,(char *)(&(theVectorDB->files[file].markers[i].cycle)),sizeof(int));
        theVectorDB->files[file].markers[i].cycle = theVectorDB->files[file].markers[i].cycle/1000;
        read(input,(char *)(&(theVectorDB->files[file].markers[i].description[0])),sizeof(char)*128);
        theVectorDB->files[file].markers[i].color.setRgb(rand()%256,rand()%256,rand()%256);
    }

    // Allocating space to read all the stripchart data in only one call system

    stripdata = (INT32 *)malloc(sizeof(int)*strips*lines);

    if(stripdata==NULL) 
    {
        printf("Not enougth memory\n");
        exit(-1);
    }

//
// I read 100 blocks and a remainder if we need it.
//

    int lines_iters, lines_rem;

    lines_iters=lines/100;
    lines_rem=lines%100;

    for(i = 0;i<lines;i+=lines_iters) 
    {
        progress->setProgress(i);
        qApp->processEvents();
        read(input,&(((char *)stripdata)[strips*sizeof(int)*i]),strips*sizeof(int)*lines_iters);
    }

    i-=lines_iters;

    if(lines_rem!=0){
        read(input,&(((char *)stripdata)[strips*sizeof(int)*i]),strips*sizeof(int)*lines_rem);
    }
    
    delete progress;
     
    // Skipping the first number because it is the line number
    actual = &(stripdata[0]);

    max = 0.0;

    for(i = 0;i<lines;i++) 
    {
        for(INT32 j = 0;j<blocks;j++) 
        {
            for(INT32 k = 0;k<theVectorDB->files[file].fullData[j].threads;k++) 
            {
                if(actual[0]<0) 
                { 
                    actual[0] = 0; 
                    theVectorDB->files[file].fullData[j].values[k][i] = 0.0;
                } 
                else 
                { 
                    theVectorDB->files[file].fullData[j].values[k][i] = ((float)actual[0])/1000.0;
                    max = (((float)actual[0])/1000.0)>max?(((float)actual[0])/1000.0):max;
                }
                actual++;
            }    
        }
        actual++;
    }

    delete stripdata;

    float Accumulated;

    // We remove negative values on every strip.
    for(i = 0;i<lines;i++) 
    {
        for(INT32 j = 0;j<blocks;j++) 
        {
            Accumulated = 0.0;
            for(INT32 k = 0;k<theVectorDB->files[file].fullData[j].threads;k++) 
            {
                if(theVectorDB->files[file].fullData[j].values[k][i]<0.0) 
                {
                    theVectorDB->files[file].fullData[j].values[k][i] = 0.0;
                }
                Accumulated+=theVectorDB->files[file].fullData[j].values[k][i];
            }
            if(theVectorDB->files[file].fullData[j].threads>1) 
            {
                theVectorDB->files[file].fullData[j].values[theVectorDB->files[file].fullData[j].threads][i] = Accumulated;
            }
        }
    }

    for(INT32 j = 0;j<blocks;j++) 
    {
        if(theVectorDB->files[file].fullData[j].threads>1)
        {
            theVectorDB->files[file].fullData[j].threads++;
        }    
        for(INT32 k = 0;k<theVectorDB->files[file].fullData[j].threads;k++) 
        {
            theVectorDB->files[file].fullData[j].maxInStripValue[k] = 0.0;
            theVectorDB->files[file].fullData[j].lineWidth[k] = 1;
            theVectorDB->files[file].fullData[j].hSkew[k] = 0;
            theVectorDB->files[file].fullData[j].vSkew[k] = 0;
            theVectorDB->files[file].fullData[j].maxView[k] = 0;
            theVectorDB->files[file].fullData[j].relativeView[k] = 0;
            theVectorDB->files[file].fullData[j].breakView[k] = 0;
            theVectorDB->files[file].fullData[j].lineStyle[k] = Qt::SolidLine;
            for(i = 0;i<lines;i++) 
            {
                if(theVectorDB->files[file].fullData[j].maxInStripValue[k] < theVectorDB->files[file].fullData[j].values[k][i]) 
                {
                    theVectorDB->files[file].fullData[j].maxInStripValue[k] = theVectorDB->files[file].fullData[j].values[k][i];
                }
            }
        }
    }

    for(i = 0;i<lines;i++) 
    {
        for(INT32 j = 0;j<blocks;j++) 
        {
            for(INT32 k = 0;k<theVectorDB->files[file].fullData[j].threads;k++) 
            {
                theVectorDB->files[file].fullData[j].maxInStripValue[k] = (theVectorDB->files[file].fullData[j].maxInStripValue[k]>theVectorDB->files[file].fullData[j].values[k][i])?theVectorDB->files[file].fullData[j].maxInStripValue[k]:theVectorDB->files[file].fullData[j].values[k][i];
            }
        }
    }


    // We need to resize the scrollview.
    INT32 w,h;
    INT32 f = theVectorDB->files[file].frequency/1000;

    asv->setRealContentsSize((int)((lines*f*(int)asv->getBaseElementSize())+2*AXIS_SIZE),(int)(max+2*AXIS_SIZE));

    asv->erase();

    do_selection();


}

void Main::do_openStripFileVersion1_1(INT32 input, QString &fn)
{
    using namespace std;                    // cerr requiered that !?
    
    UINT64 lines,strips,blocks,markers,bytes_per_line;
    LineData *readed;
    INT32 file;
    INT32 marker_pointer;
    UINT64 value_readed;
    char *data;
    UINT64 *data_int;
    UINT32 total_bytes;
    UINT32 block_size;
    UINT32 block_remaining;
    UINT64 *actualLine;
    UINT64 tag;
    UINT64 cycle;
    float max;
    INT32 data_line;
    INT32 marker_line;
    INT64 last_cycle=0;

    //
    // Read the header values... with these values we know the size of the file.
    //
    read(input,(char *)&lines,sizeof(UINT64));
    read(input,(char *)&strips,sizeof(UINT64));
    read(input,(char *)&blocks,sizeof(UINT64));
    read(input,(char *)&markers,sizeof(UINT64));
    read(input,(char *)&bytes_per_line,sizeof(UINT64));

    //
    // Open a progress dialog and show it.
    //
    progress = new QProgressDialog("Reading data...","Hide",100,asv,"progress",TRUE);
    connect(progress, SIGNAL(cancelled()),this,SLOT(hideProgress()));
    progress->show();

    // We have an static structure to maintain opened all the files the 
    // user has requested before during this execution. At this point we 
    // must search the first unused file structure and alloc memory to
    // store it and its data.
    file = 0;
    while(theVectorDB->active[file]!=0) file++;    

    theVectorDB->active[file] = 1;

    theVectorDB->files[file].fileName=new QString(fn); 

    theVectorDB->files[file].fullData = (StripData *)malloc(sizeof(StripData)*blocks);
    theVectorDB->files[file].elems = blocks; 
    theVectorDB->files[file].lines = lines-markers; 
    theVectorDB->files[file].marker_elems = markers; 

    // Now we have all the blocks allocated.
    // We use an UINT64 to read the data from disk because internally we use 32
    // bits values in some fields and all the data in disk is stored using 64 bits.
    readed = new struct LineData[blocks];

    for(UINT32 i=0; i<blocks;i++) {
        read(input,(char *)&value_readed,sizeof(value_readed));
	readed[i].position=value_readed;

        read(input,(char *)readed[i].description,sizeof(char)*100);
	// Cleaning the data after blanks.
	for(UINT32 counter=0;counter<100;counter++) if(readed[i].description[counter]==' ')readed[i].description[counter]='\0';
	
        read(input,(char *)&value_readed,sizeof(value_readed));
	readed[i].threads=value_readed;
        read(input,(char *)&value_readed,sizeof(value_readed));
	readed[i].frequency=value_readed;
        read(input,(char *)&value_readed,sizeof(value_readed));
	readed[i].base_frequency=value_readed;
        read(input,(char *)&value_readed,sizeof(value_readed));
	readed[i].max_elems=value_readed;
	
        // Number of threads active at this block
        theVectorDB->files[file].fullData[i].threads = readed[i].threads;
        // Number of threads active at this block
        theVectorDB->files[file].fullData[i].elems = lines-markers;

        // Copying description 
        for(INT32 j = 0;j<100;j++) 
        {
            theVectorDB->files[file].fullData[i].description[j] = readed[i].description[j];
        }
    
        // Alloc memory for data from stripcharts
        for(INT32 j = 0;j<readed[i].threads;j++) 
        {
            theVectorDB->files[file].fullData[i].values[j] = (float *)malloc(sizeof(float)*(lines-markers));
            theVectorDB->files[file].fullData[i].visible[j] = 0;
            theVectorDB->files[file].fullData[i].activated[j] = 0;
            theVectorDB->files[file].fullData[i].smooth[j] = 0;
            theVectorDB->files[file].fullData[i].smoothView[j] = 0;
            theVectorDB->files[file].fullData[i].breakSmooth[j] = 0;
            theVectorDB->files[file].fullData[i].color[j].setRgb(rand()%256,rand()%256,rand()%256);
            theVectorDB->files[file].fullData[i].maxValue[j] = readed[i].max_elems;
        }
        // Calculate Accumulated strip
        if(readed[i].threads>1) 
        {
            INT32 j = readed[i].threads;    
            theVectorDB->files[file].fullData[i].values[j] = (float *)malloc(sizeof(float)*(lines-markers));
            theVectorDB->files[file].fullData[i].visible[j] = 0;
            theVectorDB->files[file].fullData[i].activated[j] = 0;
            theVectorDB->files[file].fullData[i].smooth[j] = 0;
            theVectorDB->files[file].fullData[i].smoothView[j] = 0;
            theVectorDB->files[file].fullData[i].breakSmooth[j] = 0;
            theVectorDB->files[file].fullData[i].color[j].setRgb(rand()%256,rand()%256,rand()%256);
        }

        // Updating file frequency
        theVectorDB->files[file].frequency = readed[i].base_frequency;

    }

    delete readed;

    // At this point we create the data to store markers.
    theVectorDB->files[file].markers = (MarkerData *)malloc(sizeof(MarkerData)*markers);

    // Now the markers data is stored in the same area as the data. We need a pointer
    // know where is the next marker to be readed an filled. When we scan the data we
    // can found markers and we must save the data in the different marker structs.
    marker_pointer=0;

    //
    // We need 16 extra bytes to store the tag and the cycle number.
    //
    total_bytes=sizeof(char)*(bytes_per_line+16)*lines;
    block_size=total_bytes/100;
    block_remaining=total_bytes%100;

    data=(char *)malloc(total_bytes);
  
    INT32 dt=0;
  
    for(UINT32 i=0; i<100;i++) {
        dt+=read(input,(char *)&(data[block_size*i]),block_size);
        progress->setProgress(i);
        qApp->processEvents();
    }
    if(block_remaining!=0) {
        dt+=read(input,(char *)&(data[block_size*100]),block_remaining);
        progress->setProgress(100);
        qApp->processEvents();
    }

    delete progress;

    data_int=(UINT64 *)data;

    data_line=0;
    marker_line=0;

    for(UINT32 i=0; i<lines;i++) {
        actualLine=&(data_int[i*(strips+2)]);
	
        tag=actualLine[0];

        if(tag==0) {
            // Tag 0 is the standard data line
            cycle=actualLine[1];
	    UINT32 position_in_line=2;
             
            for(UINT32 j=0;j<blocks;j++) {
	        if(theVectorDB->files[file].fullData[j].threads==1) {
		    //theVectorDB->files[file].fullData[j].values[0][i]=(float)actualLine[position_in_line++]/(float)theVectorDB->files[file].frequency;
		    theVectorDB->files[file].fullData[j].values[0][data_line]=(float)actualLine[position_in_line++]/(float)theVectorDB->files[file].frequency;
		}
		else {
		    UINT64 Accum=0;
                    for(INT32 thr=0; thr<theVectorDB->files[file].fullData[j].threads;thr++) {
                        Accum+=actualLine[position_in_line];
			theVectorDB->files[file].fullData[j].values[thr][data_line]=(float)actualLine[position_in_line++]/(float)theVectorDB->files[file].frequency;
		    }
		    theVectorDB->files[file].fullData[j].values[theVectorDB->files[file].fullData[j].threads][data_line]=(float)Accum/(float)theVectorDB->files[file].frequency;
		}
	    }
	    data_line++;

	    last_cycle=data_line*theVectorDB->files[file].frequency;
	    
        }
	else if(tag==1) {
	    long long val0;
	    unsigned long long val1;
	    int result;
	    char desc[128];
	    int cnt=0;
            
            // Tag 1 is a marker line
            theVectorDB->files[file].markers[marker_line].cycle = data_line;
            theVectorDB->files[file].markers[marker_line].color.setRgb(255,0,0);

	    strncpy(theVectorDB->files[file].markers[marker_line].description,(char *)(&actualLine[1]),127);
	    theVectorDB->files[file].markers[marker_line].description[127]='\0';

//	    sprintf(theVectorDB->files[file].markers[marker_line].description,"%100s",(char *)(&actualLine[1]));

            // Getting values
            result=sscanf(theVectorDB->files[file].markers[marker_line].description,"%lld %llu %s",&val0, &val1, desc);

            result=-1;

            if(result<0) {
                while((markerLimiter[cnt].val0!=-1) || (markerLimiter[cnt].val0==val0 && markerLimiter[cnt].val1==val1)){
                    cnt++;
	        }

	        if(markerLimiter[cnt].val0==-1) {
                    // not found then visible
		    theVectorDB->files[file].markers[marker_line].visible=true;
	        }
	        else {
                    // found then not visible
		    theVectorDB->files[file].markers[marker_line].visible=false;
	        }
	    }
	    else {
	        theVectorDB->files[file].markers[marker_line].visible=true;
	    } 

	    marker_line++;
	}
	else {
            // With this file version a different Tag is not posible then we stop.
            cerr<<"TAG "<<tag<<" IS UNSUPPORTED"<<endl;
	    exit(-1);
	}
    }

    // We remove negative values on every strip.
    for(UINT32 i = 0;i<(lines-markers);i++) 
    {
        for(UINT32 j = 0;j<blocks;j++) 
        {
            float Accumulated = 0.0;
            for(INT32 k = 0;k<theVectorDB->files[file].fullData[j].threads;k++) 
            {
                if(theVectorDB->files[file].fullData[j].values[k][i]<0.0) 
                {
                    theVectorDB->files[file].fullData[j].values[k][i] = 0.0;
                }
                Accumulated+=theVectorDB->files[file].fullData[j].values[k][i];
            }
            if(theVectorDB->files[file].fullData[j].threads>1) 
            {
                theVectorDB->files[file].fullData[j].values[theVectorDB->files[file].fullData[j].threads][i] = Accumulated;
            }
        }
    }

    for(UINT32 j = 0;j<blocks;j++) 
    {
        if(theVectorDB->files[file].fullData[j].threads>1)
        {
            theVectorDB->files[file].fullData[j].threads++;
        }    
        for(INT32 k = 0;k<theVectorDB->files[file].fullData[j].threads;k++) 
        {
            theVectorDB->files[file].fullData[j].maxInStripValue[k] = 0.0;
            theVectorDB->files[file].fullData[j].lineWidth[k] = 1;
            theVectorDB->files[file].fullData[j].hSkew[k] = 0;
            theVectorDB->files[file].fullData[j].vSkew[k] = 0;
            theVectorDB->files[file].fullData[j].maxView[k] = 0;
            theVectorDB->files[file].fullData[j].relativeView[k] = 0;
            theVectorDB->files[file].fullData[j].breakView[k] = 0;
            theVectorDB->files[file].fullData[j].lineStyle[k] = Qt::SolidLine;
            for(UINT32 i = 0;i<(lines-markers);i++) 
            {
                if(theVectorDB->files[file].fullData[j].maxInStripValue[k] < theVectorDB->files[file].fullData[j].values[k][i]) 
                {
                    theVectorDB->files[file].fullData[j].maxInStripValue[k] = theVectorDB->files[file].fullData[j].values[k][i];
                }
            }
        }
    }

    INT32 w,h;
    INT32 f = theVectorDB->files[file].frequency/1000;

    asv->setRealContentsSize((int)((lines*f*(int)asv->getBaseElementSize())+2*AXIS_SIZE),(int)(max+2*AXIS_SIZE));

    asv->erase();

    do_selection();
}

void Main::do_openMarkerFile(char *str)
{
    FILE *markerFile;
    int result;
    char desc[128];
    int i;


    markerFile=fopen(str,"r");
    if(markerFile==NULL) exit(-1);

    for(i=0;i<MAXMARKERLIMIT;i++) {
        markerLimiter[i].val0=-1;
        markerLimiter[i].val1=0;
    }
    
    i=0;

    do{
        result=fscanf(markerFile,"%lld %llu %s\n",&markerLimiter[i].val0,&markerLimiter[i].val1,desc);
	i++;
    }while(result>0);

    fclose(markerFile); 
}

void Main::do_openStripFile(char *str)
{
    INT32 input;
    QString fn;

    INT32 versionH;
    INT32 versionL;
    INT64 version;
    
    // Open a dialog to select the file and open it. If open crash we return an
    // error.
    if( str==NULL )
    {
        fn = QFileDialog::getOpenFileName( QString::null, tr( "Strip-Files (*.stb);;All Files (*)" ), this );
    }
    else
    {
        fn=QString(str); 
    }

    if(fn.length()==0) return;
    
    for(int i = 0;i<MAX_OPENED_FILES;i++)
    {
        if(theVectorDB->active[i]==1) 
        {
            if(*(theVectorDB->files[i].fileName)==fn) 
            {
                Error *errorDialog = new Error(this,"Error",TRUE, 0);
                errorDialog->show();
                return;
            }
        }
    }

    input = open(fn.latin1(),O_RDONLY);

    if(input==-1)return;

    read(input,(char *)&versionL,sizeof(int));
    read(input,(char *)&versionH,sizeof(int));
   
    version=versionH;
    version=version<<32;
    version=version+versionL;

    if(versionL==11) {
       do_openStripFileVersion1_1(input, fn );
    }
    else {
       do_openStripFileVersion1_0(input, fn, versionL, versionH);
    }   
}

void Main::do_horizontalGrid()
{
    bool s  = !view->isItemChecked(axisH);
    view->setItemChecked(axisH,s);
    if (s)
    {
        printAxisH = 1;
    }    
    else
    {
        printAxisH = 0;
    }    
}

void Main::do_verticalGrid()
{
    bool s = !view->isItemChecked(axisV);
    view->setItemChecked(axisV,s);
    if (s)
    {
        printAxisV = 1;
    }    
    else
    {
        printAxisV = 0;
    }    
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Main::do_switchToolsBar()
{
    bool s = !window->isItemChecked(win_tool_bar_id);
    window->setItemChecked(win_tool_bar_id,s);
    if (s)
    {
        pointerTools->show();
    }    
    else
    {
        pointerTools->hide();
    }    
        
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Main::do_switchViewBar()
{
    bool s = !window->isItemChecked(win_view_bar_id);
    window->setItemChecked(win_view_bar_id,s);
    if (s)
    {
        viewTools->show();
    }    
    else
    {
        viewTools->hide();
    }    
        
}

void Main::do_switchPaletteBar()
{
    bool s = !window->isItemChecked(win_view_pal_id);
    window->setItemChecked(win_view_pal_id,s);
    if (s)
    {
        visiblePalette->show();
    }    
    else
    {
        visiblePalette->hide();
    }    
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Main::do_switchAnnotationsBar()
{
    bool s = !window->isItemChecked(win_annotations_bar_id);
    window->setItemChecked(win_annotations_bar_id,s);
    if (s)
    {
        annotationsTools->show();
    }    
    else
    {
        annotationsTools->hide();
    }    
}

void Main::do_switchFileBar()
{
    bool s = !window->isItemChecked(win_file_bar_id);
    window->setItemChecked(win_file_bar_id,s);
    if (s)
    {
        fileTools->show();
    }    
    else
    {
        fileTools->hide();
    }
}

void Main::toolsBarVisibilityChanged(bool visible)
{
     window->setItemChecked(win_tool_bar_id,visible);
}


void Main::fileBarVisibilityChanged(bool visible)
{
     window->setItemChecked(win_file_bar_id,visible);
}
    
void Main::viewBarVisibilityChanged(bool visible)
{
     window->setItemChecked(win_view_bar_id,visible);
}

void Main::annotationsBarVisibilityChanged(bool visible)
{
     window->setItemChecked(win_annotations_bar_id,visible);
}

void Main::do_selectToolEnabled()
{
    asv->setPointerType(POINTER_SELECT);
}

void Main::do_paddingToolEnabled()
{
    asv->setPointerType(POINTER_PANNING);
}

void Main::do_zoomingToolEnabled()
{
    asv->setPointerType(POINTER_ZOOMING);
}

void Main::do_distanceToolEnabled()
{
    asv->setPointerType(POINTER_DISTANCE);
}

void Main::do_shadingToolEnabled()
{
    asv->setPointerType(POINTER_SHADING);
}


Main::~Main()
{
    delete printer;
}

void Main::do_help()
{
    // just tmp:
    QString home = QString("./help/2DreamsHelp.html");
    HelpWindow *help = new HelpWindow(home, ".", NULL, "Help Viwer");
    help->setCaption("Helpviewer");
    if (    QApplication::desktop()->width() > 400
         && QApplication::desktop()->height() > 500 ) 
    {
        help->show();
    }    
    else
    {
        help->showMaximized();
    }    
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Main::do_about()
{
     static QMessageBox* about = new QMessageBox( "2Dreams",
                                                  "<h2>StripChartViewer tool.</h2>"
                                                  "<h3>This tool was developed by the BSSAD Intel/UPC Lab</h3>"
                                                  "The interface is based on QT (version 3.x.x).<br><br>"
                                                  "<h4>Build information: </h4>"
                                                  "<ul>"
                                                  "<li>Program version: 1.1.0"
                                                  "<li>QT version: 3.0.4"
                                                  "<li>Compilation: Non optimized, statically linked"
                                                  "</ul><br>"
                                                  "<h4>Contact information:</h4>"
                                                  "<ul>"
                                                  "<li> Isaac Hernandez, ihernand@ac.upc.es"
                                                  "<li> Federico Ardanaz, fardanaz@ac.upc.es"
                                                  "<li> Roger Espasa, roger@ac.upc.es"
                                                  "</ul>",QMessageBox::Information, 1, 0, 0, this, 0, FALSE );
     about->setButtonText( 1, "Dismiss" );
     about->show();
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Main::do_selection()
{ 
    SelectorImpl *selector=new SelectorImpl(this,"StripChart",TRUE, 0, theVectorDB, asv, visiblePalette);

    selector->connectRoutines();

    selector->show();
}

void Main::do_configuration()
{ 
    PropertiesImpl *properties=new PropertiesImpl(this,"Properties",TRUE, 0, theVectorDB);

    properties->connectRoutines();

    properties->show();

    asv->erase();
}

void Main::do_zoomIn()
{ 
    asv->ZoomIn(); 
    
    updateRulers();
}

void Main::do_zoomOut()
{ 
    asv->ZoomOut();
    
    updateRulers();
}

void Main::do_zoomInH()
{ 
    asv->ZoomInX();
    
    updateRulers();
}

void Main::do_zoomOutH()
{ 
    asv->ZoomOutX();
    
    updateRulers();
}

void Main::do_zoomInV()
{ 
    asv->ZoomInY();
    
    updateRulers();
}

void Main::do_zoomOutV()
{ 
    asv->ZoomOutY();
    
    updateRulers();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Main::do_resetAspectRatio()
{
    double x,y,r;

    if(aspectRatioLocked==false) {
        asv->lockAspectRatio();
        x = asv->getScallingFactorX();
        y = asv->getScallingFactorY();
        r = x < y ? x : y;
        asv->setScallingFactor(r);
        aspectRatioLocked=true;
        updateRulers();
        asv->setSTFitKeepARatio(true);
        asv->setRectZoomKeepARatio(true);
    } else {
        asv->unlockAspectRatio();
        aspectRatioLocked=false;
        asv->setSTFitKeepARatio(false);
        asv->setRectZoomKeepARatio(false);
    }
        
}

void Main::do_scaleToFit()
{ 
    asv->scaleToFit();
    
    updateRulers();
}

void Main::do_z200()
{ 
    asv->setScallingFactor(2.0);
    
    updateRulers();
}

void Main::do_z100()
{ 
    asv->setScallingFactor(1.0) ;
    
    updateRulers();
}

void Main::do_z50()
{ 
    asv->setScallingFactor(0.5) ;
    
    updateRulers();
}

void Main::do_z25()
{ 
    asv->setScallingFactor(0.25) ;
    
    updateRulers();
}

void Main::updateRulers()
{
    asv->rlH->setZoomFactor(asv->getScallingFactorY());
    asv->rlW->setZoomFactor(asv->getScallingFactorX());
    
    asv->rlH->drawRuler();
    asv->rlW->drawRuler();
}

void Main::do_print()
{
    if ( !printer ) 
    {
        printer = new QPrinter;
    }
       
    if ( printer->setup(this) ) {
        QPainter pp(printer);
    }
    
    asv->erase();
}

void Main::do_selectEvents() {}

void Main::do_makeStyle(const QString &style)
{
    qApp->setStyle(style);
    
    if(style == "Platinum") 
    {
        QPalette p( QColor( 239, 239, 239 ) );
        qApp->setPalette( p, TRUE );
        qApp->setFont( appFont, TRUE );
    }
    else if(style == "Windows") 
    {
        qApp->setFont( appFont, TRUE );
    }
    else if(style == "CDE") 
    {
        QPalette p( QColor( 75, 123, 130 ) );
        
        p.setColor( QPalette::Active,   QColorGroup::Base,            QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Inactive, QColorGroup::Base,            QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Disabled, QColorGroup::Base,            QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Active,   QColorGroup::Highlight,       Qt::white );
        p.setColor( QPalette::Active,   QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Inactive, QColorGroup::Highlight,       Qt::white );
        p.setColor( QPalette::Inactive, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Disabled, QColorGroup::Highlight,       Qt::white );
        p.setColor( QPalette::Disabled, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Active,   QColorGroup::Foreground,      Qt::white );
        p.setColor( QPalette::Active,   QColorGroup::Text,            Qt::white );
        p.setColor( QPalette::Active,   QColorGroup::ButtonText,      Qt::white );
        p.setColor( QPalette::Inactive, QColorGroup::Foreground,      Qt::white );
        p.setColor( QPalette::Inactive, QColorGroup::Text,            Qt::white );
        p.setColor( QPalette::Inactive, QColorGroup::ButtonText,      Qt::white );
        p.setColor( QPalette::Disabled, QColorGroup::Foreground,      Qt::lightGray );
        p.setColor( QPalette::Disabled, QColorGroup::Text,            Qt::lightGray );
        p.setColor( QPalette::Disabled, QColorGroup::ButtonText,      Qt::lightGray );
        
        qApp->setPalette( p, TRUE );
        qApp->setFont( QFont( "times", appFont.pointSize() ), TRUE );
    }
    else if(style == "Motif" || style == "MotifPlus") 
    {
        QPalette p( QColor( 192, 192, 192 ) );
        qApp->setPalette( p, TRUE );
        qApp->setFont( appFont, TRUE );
    }

}

void Main::do_addBookmark()
{
    double scx = asv->getScallingFactorX();
    double scy = asv->getScallingFactorY();
    
    int x  = asv->contentsX();
    int y  = asv->contentsY();
    
    bkMgr->addBookmark(scx,scy,x,y);
}

void Main::do_manageBookmark(){}
void Main::do_drawLine(){}
void Main::do_drawRectangle(){}
void Main::do_drawCircle(){}
void Main::do_insertText(){}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -- The main method
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main(int argc, 
         char** argv)
{
    QApplication app(argc,argv);

    Main m(NULL);
    m.setMinimumSize(640,480);

    qApp->setMainWidget(&m);

    m.setCaption("StripChartViewer - Version 1.1.0 - BSSAD Intel/UPC Lab");
    if (    QApplication::desktop()->width() > m.width() + 10
         && QApplication::desktop()->height() > m.height() +30 )
    { 
        m.show();
    }    
    else
    {
        m.showMaximized();
    }
    
    m.show();
    qApp->setMainWidget(0);
    QObject::connect( qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()) );
    m.resize(640,480);
    if(argc==2) {
        m.do_openStripFile(argv[1]);
    }

    if(argc==3) {
        if(strcmp("-if",argv[1])==0) {
	    m.do_openStripFile(argv[2]);
	}
	else {
            // Print an error message    
	    printf("Parameter Error.\n1Dream -mf <marker_file> -if<strip_file>\n");
	    exit(-1);
	}
    }

    if(argc==5) {
        if(strcmp("-mf",argv[1])==0) {
	    m.do_openMarkerFile(argv[2]);
	}
	else if(strcmp("-mf",argv[3])==0) {
	    m.do_openMarkerFile(argv[4]);
	}
	else {
            // Print an error message    
	    printf("Parameter Error.\n1Dream -mf <marker_file> -if<strip_file>\n");
	    exit(-1);
	}

        if(strcmp("-if",argv[1])==0) {
	    m.do_openStripFile(argv[2]);
	}
	else if(strcmp("-if",argv[3])==0) {
	    m.do_openStripFile(argv[4]);
	}
	else {
            // Print an error message    
	    printf("Parameter Error.\n1Dream -mf <marker_file> -if<strip_file>\n");
	    exit(-1);
	}
    }

    return app.exec();
}

